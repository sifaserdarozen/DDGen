#include "CallLoggerFactory.h"
#include "CallStorageFactory.h"
#include "ConsumerFactory.h"

#include "callleg.h"
#include "programoptions.h"
#include "webinterface.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <sys/time.h>
#include <vector>

bool SleepSystemUsec(unsigned long long int sleep_usec)
{
    timespec sleep_timespec = {((time_t) sleep_usec/1000000), ((long int)((sleep_usec % 1000000) * 1000))};
    timespec unused_timespec;

    for (;;)
    {
        if (0 == nanosleep(&sleep_timespec, &unused_timespec))
        {
            return true;
        }
        else if (EINTR == errno )
        {
            std::cerr << "Interrupted sleep with sleep value: " << sleep_usec << " at: bool SleepSystemUsec()" << std::endl;
            sleep_timespec = unused_timespec;
        }
        else
        {
            std::cerr << "Errror in nanosleep(), error: " << errno << " sleep value: " << sleep_usec << " at: bool SleepSystemUsec()" << std::endl;
            return false;
        }
    }
}

int main(int argc, char* argv[])
{
    ddgen::ProgramOptions program_options(argc, argv);

    auto callLogger = ddgen::CallLoggerFactory::CreateCallLogger( { program_options.useDb, program_options.dbPath, program_options.stackName } );

    ddgen::G711aEncoderFactory g711a_encoder_factory;
    ddgen::SingleToneGeneratorFactory single_tone_generator_factory;

    auto callFactory = ddgen::CallFactoryFactory::CreateCallFactory( { program_options.traffic, program_options.drlinkIpPortVector, program_options.startIp } );
    auto consumer = ddgen::ConsumerFactory::CreateConsumer( { program_options.output, program_options.dstIpPortVector, program_options.useS3, program_options.stackName } );

    std::vector<std::unique_ptr<ddgen::Call>> call_ptr_vector;

    const auto simulationDuration = program_options.simulationDuration * 1000;

    const auto start_time = std::chrono::steady_clock::now();
    auto last_time = start_time;

    // form a seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // introduce generator
    std::minstd_rand generator(seed);
    std::uniform_int_distribution<unsigned short int> usint_distribution(program_options.callDuration * 0.75, program_options.callDuration * 1.25);

    std::cout << std::endl << "Should have root privileges for some of operations! (for --secure / for )" << std::endl;
    std::cout << "Simulation will end in " << simulationDuration << " seconds" << std::endl;

    ddgen::WebInterface web_interface(program_options.shouldUseSecureWebInterface);

    while(true)
    {
        if (call_ptr_vector.size() < program_options.numberOfCalls)
        {
            unsigned short int call_duration = usint_distribution(generator);

            std::unique_ptr<ddgen::Call> call = callFactory->CreateCall({ call_duration, callLogger, &g711a_encoder_factory, &single_tone_generator_factory, consumer });

            call_ptr_vector.push_back(std::move(call));
            std::cout << " a call is created with duration " << call_duration << std::endl;
        }

        const auto current_time = std::chrono::steady_clock::now();
        const auto ellapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_time).count();

        if (ellapsed_time > 40000)
        {
            std::clog << __FILE__ << " " << __LINE__ << "... too much lag: " << ellapsed_time / 1000 << " ms " << std::endl;
        }

        if (ellapsed_time > 20000)
        {
            for (std::vector<std::unique_ptr<ddgen::Call>>::iterator it = call_ptr_vector.begin(); it != call_ptr_vector.end(); ++it)
            {
                const bool step_result = (*it) -> Step(20);
                if (false == step_result)
                {
                    std::clog << "a calltimed out " << std::endl;
                    it = call_ptr_vector.erase(it);
                    --it;
                }
            }

            last_time = current_time;
        }
        else
        {
            const auto sleep_time = 20000 - ellapsed_time;
            SleepSystemUsec(sleep_time);
        }

        const auto ellapsed_time_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
        if (ellapsed_time_in_ms > simulationDuration) {
            std::cout << "Simulation time " << program_options.simulationDuration << " is over" << std::endl;
            break;
        }
    }

    return 0;
}
