#include "callleg.h"

#include <arpa/inet.h>
#include <chrono>
#include <cstring>
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

void DisplayUsage()
{
    std::cout << " --- drlink --- " << std::endl;
    std::cout << "Usage is: ddgen --nc 10 [-i ip_of_capturer -p port_of_capturer ... ]" << std::endl;
    std::cout << "ddgen --nc 10 --dc 60 --drlink 192.168.126.1 28008 192.168.126.1 28009" << std::endl;
    std::cout << "to generate 10 calls each having random duration (uniform %10) of 60s" << std::endl;
    std::cout << "and send to drlink media address 192.168.126.1:28008 and 192.168.126.1:28009" << std::endl;
    std::cout << "(if omitted) default values are: 127.0.0.1 and 29000 29001" << std::endl;
    std::cout << "drlink data is send to drlink socket by default. If want to save as pcap, use --pcap flag" << std::endl;
    std::cout << "ddgen --nc 10 --dc 60 --pacp --drlink 192.168.126.1 28008 192.168.126.1 28009" << std::endl;
    std::cout << " --- mirror --- " << std::endl;
    std::cout << "ddgen --nc 10 --dc 60 --mirror" << std::endl;
    std::cout << "to save pair traffic as pcap file, which should be operating in non functional mirror mode." << std::endl;
    std::cout << "If somewhow want to send generated pair data to a socket use; " << std::endl;
    std::cout << "ddgen --nc 10 --dc 60 --socket 192.168.126.1 28008 --mirror" << std::endl;
    std::cout << "send pair traffic to media address 192.168.126.1:28008" << std::endl;
    std::cout << "--start 172.24.101.54 starting point for endpoint ips" << std::endl;
}

enum class TrafficType
{
    DrLink,
    Mirror
};

enum class OutputType
{
    Pcap,
    Socket
};

struct ProgramOptions
{
    unsigned int number_of_calls;
    unsigned int duration_of_calls;
    unsigned int start_ip;
    std::vector<ddgen::IpPort> dst_ipport_vector;
    std::vector<ddgen::IpPort> drlink_ipport_vector;
    TrafficType traffic;
    OutputType output;

    ProgramOptions() : number_of_calls(10), duration_of_calls(60), start_ip(0xac186536), traffic(TrafficType::Mirror), output(OutputType::Pcap) {}
};

int main(int argc, char*argv[])
{
    ProgramOptions program_options;

    ddgen::G711aEncoderFactory g711a_encoder_factory;
    ddgen::SingleToneGeneratorFactory single_tone_generator_factory;

    std::unique_ptr<ddgen::CallFactory> call_factory_ptr;
    ddgen::Consumer* consumer_ptr = NULL;

    for (int argv_index = 1; argv_index < argc; ++argv_index)
    {
        if ((0 == strcmp("--nc", argv[argv_index])) && ((argv_index + 1) < argc))
        {
            program_options.number_of_calls = std::atoi(argv[argv_index + 1]);
            argv_index++;
        }
        else if ((0 == strcmp("--dc", argv[argv_index])) && ((argv_index + 1) < argc))
        {
            program_options.duration_of_calls = std::atoi(argv[argv_index + 1]);
            argv_index++;
        }
        else if ((0 == strcmp("--drlink", argv[argv_index])) && ((argv_index + 4) < argc))
        {
            in_addr d_inaddr;
            unsigned int dst_ip = 0x691e1bac;
            unsigned short int dst_port = 29000;

            if (1 == inet_aton(argv[argv_index + 1], &d_inaddr))
            {
                dst_ip = ntohl(d_inaddr.s_addr);
            }

            dst_port = std::atoi(argv[argv_index + 2]);
            ddgen::IpPort dst_ipport(dst_ip, dst_port);

            program_options.drlink_ipport_vector.push_back(dst_ipport);

            if (1 == inet_aton(argv[argv_index + 3], &d_inaddr))
            {
                dst_ip = ntohl(d_inaddr.s_addr);
            }

            dst_port = std::atoi(argv[argv_index + 4]);
            ddgen::IpPort dst_ipport2(dst_ip, dst_port);

            program_options.drlink_ipport_vector.push_back(dst_ipport2);
            program_options.dst_ipport_vector = program_options.drlink_ipport_vector;
            argv_index += 4;

            program_options.traffic = TrafficType::DrLink;
            program_options.output = OutputType::Socket;
        }
        else if (0 == strcmp("--mirror", argv[argv_index]))
        {
            program_options.traffic = TrafficType::Mirror;
        }
        else if (0 == strcmp("--pcap", argv[argv_index]))
        {
            program_options.output = OutputType::Pcap;
        }
        else if ((0 == strcmp("--socket", argv[argv_index]) ) && ((argv_index + 2) < argc))
        {
            in_addr d_inaddr;
            unsigned int dst_ip = 0x691e1bac;
            unsigned short int dst_port = 29000;

            if (1 == inet_aton(argv[argv_index + 1], &d_inaddr))
            {
                dst_ip = ntohl(d_inaddr.s_addr);
            }

            dst_port = std::atoi(argv[argv_index + 2]);
            ddgen::IpPort dst_ipport(dst_ip, dst_port);

            program_options.dst_ipport_vector.push_back(dst_ipport);
            argv_index += 2;

            program_options.output = OutputType::Socket;
        }
        else if ((0 == strcmp("--start", argv[argv_index]) ) && ((argv_index + 1) < argc))
        {
            in_addr s_inaddr;
            if (1 == inet_aton(argv[argv_index + 1], &s_inaddr))
            {
                program_options.start_ip = ntohl(s_inaddr.s_addr);
            }
            argv_index += 1;
        }
        else
        {
            std::cout << "unknown option : " << argv[argv_index] << std::endl;
            DisplayUsage();
            return -1;
        }
    }

    if (program_options.output == OutputType::Pcap) {
        consumer_ptr = new ddgen::PcapConsumer();
    } else {
        consumer_ptr = new ddgen::SocketConsumer(program_options.dst_ipport_vector);
    }

    if (program_options.traffic == TrafficType::DrLink) {
        call_factory_ptr = std::make_unique<ddgen::DRLinkCallFactory>(program_options.drlink_ipport_vector, program_options.start_ip);	
    } else {
       call_factory_ptr = std::make_unique<ddgen::MirrorCallFactory>(program_options.start_ip);
    }

    std::vector<ddgen::Call*> call_ptr_vector;

    const auto desired_run_time = program_options.duration_of_calls * 10 * 1000;

    const auto start_time = std::chrono::steady_clock::now();
    auto last_time = start_time;

    // form a seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // introduce generator
    std::minstd_rand generator(seed);
    std::uniform_int_distribution<unsigned short int> usint_distribution(program_options.duration_of_calls * 0.75, program_options.duration_of_calls * 1.25);

    if (!call_factory_ptr)
    {
        std::cout << __FILE__ << " " << __LINE__ << " call factory ptr is null " << std::endl;
        return -1;
    }

    std::cout << std::endl << "Should have root privileges !" << "Simulation will end in " << desired_run_time / 1000.0 << " seconds" << std::endl;

    while(true)
    {
        if (call_ptr_vector.size() < program_options.number_of_calls)
        {
            unsigned short int call_duration = usint_distribution(generator);

            ddgen::Call* call_ptr = call_factory_ptr->CreateCall(call_duration, &g711a_encoder_factory, &single_tone_generator_factory, consumer_ptr);
            if (!call_ptr)
            {
                std::cerr << __FILE__ << " " << __LINE__ << "unable to create call_ptr" << std::endl;
                return -1;
            }

            call_ptr_vector.push_back(call_ptr);
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
            for (std::vector<ddgen::Call*>::iterator it = call_ptr_vector.begin(); it != call_ptr_vector.end(); ++it)
            {
                if (*it)
                {
                    bool step_result = (*it) -> Step(20);
                    if (false == step_result)
                    {
                        std::clog << "a calltimed out " << std::endl;
                        delete (*it);
                        it = call_ptr_vector.erase(it);
                        --it;
                    }
                }
                else
                {
                    std::cerr << __FILE__ << " " << __LINE__ << "iteratoris null " << std::endl;
                    return -1;
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
        if (ellapsed_time_in_ms > desired_run_time) {
            std::cout << "Simulation time " << desired_run_time << " is over" << std::endl;
            break;
        }
    }

    for (std::vector<ddgen::Call*>::iterator it = call_ptr_vector.begin(); it != call_ptr_vector.end(); ++it)
    {
        if (*it)
        {
            delete (*it);
            *it = NULL;
        }
    }

    // free consumer block
    if (consumer_ptr)
    {
        delete consumer_ptr;
        consumer_ptr = NULL;
    }

    return 0;
}
