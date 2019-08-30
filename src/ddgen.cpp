#include "callleg.h"

#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>

int kbhit()
{
    timeval tv;
    fd_set readfd;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&readfd);
    FD_SET(0, &readfd);
    return select(1, &readfd, NULL, NULL, &tv);
}

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

/*
void GetCurrentTimeInTv(unsigned int& sec, unsigned int& usec)
{
#if defined __linux__ || defined __FreeBSD__
    timeval tv;
    if (0 == gettimeofday(&tv, NULL))
    {
        sec = (unsigned int)tv.tv_sec;
        usec = (unsigned int)tv.tv_usec;
    }
    else
        std::cerr << __FILE__ << " " << __LINE__ << " unable to obtain time info" << std::endl;
#elif defined _WIN32
    static const unsigned long long int EPOCH = ((unsigned long long int) 116444736000000000ULL);

    SYSTEMTIME system_time;
    FILETIME file_time;
    unsigned long long int time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((unsigned long long int)file_time.dwLowDateTime );
    time += ((unsigned long long int)file_time.dwHighDateTime) << 32;

    sec  = (unsigned int) ((time - EPOCH) / 10000000L);
    usec = (unsigned int) (system_time.wMilliseconds * 1000);
#endif
}
*/

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
}

int main(int argc, char*argv[])
{
    std::cout << std::endl << "Must have root privileges !" << std::endl << "Press a key to stop operation" << std::endl << std::endl;

    unsigned int number_of_calls = 10;
    unsigned int duration_of_calls = 60;

    unsigned int dst_ip = 0x691e1bac;
    unsigned short int dst_port = 29000;
    std::vector<IpPort> dst_ipport_vector;
    std::vector<IpPort> drlink_ipport_vector;

    G711aEncoderFactoryType g711a_encoder_factory;
    SingleToneGeneratorFactoryType single_tone_generator_factory;

    CallFactoryType* call_factory_ptr = NULL;
    ConsumerType* consumer_ptr = NULL;

    for (int argv_index = 1; argv_index < argc; ++argv_index)
    {
        if ((0 == strcmp("--nc", argv[argv_index])) && ((argv_index + 1) < argc))
        {
            number_of_calls = std::atoi(argv[argv_index + 1]);
            argv_index++;
        }
        else if ((0 == strcmp("--dc", argv[argv_index])) && ((argv_index + 1) < argc))
        {
            duration_of_calls = std::atoi(argv[argv_index + 1]);
            argv_index++;
        }
        else if ((0 == strcmp("--drlink", argv[argv_index])) && ((argv_index + 4) < argc))
        {
            in_addr d_inaddr;
            if (1 == inet_aton(argv[argv_index + 1], &d_inaddr))
            {
                dst_ip = ntohl(d_inaddr.s_addr);
            }

            dst_port = std::atoi(argv[argv_index + 2]);
            IpPort dst_ipport(dst_ip, dst_port);

            drlink_ipport_vector.push_back(dst_ipport);

            if (1 == inet_aton(argv[argv_index + 3], &d_inaddr))
            {
                dst_ip = ntohl(d_inaddr.s_addr);
            }

            dst_port = std::atoi(argv[argv_index + 4]);
            IpPort dst_ipport2(dst_ip, dst_port);

            drlink_ipport_vector.push_back(dst_ipport2);
            argv_index += 4;

            // if consumer is has not set already, default to socket
            if (!consumer_ptr)
            {
                consumer_ptr = new SocketConsumerType(drlink_ipport_vector);
                if (!consumer_ptr)
                {
                    std::cerr << __FILE__ << " " << __LINE__ << " consumer is not able to be set" << std::endl;
                    return -1;
                }
            }

            call_factory_ptr = new DRLinkCallFactoryType(drlink_ipport_vector);
            if (!call_factory_ptr)
            {
                std::cerr << __FILE__ << " " << __LINE__ << " call factory is not able to be generated" << std::endl;
                return -1;
            }
        }
        else if (0 == strcmp("--mirror", argv[argv_index]))
        {
            // set call generator to mirror mode

            // if consumer is has not set already, default to socket
            if (!consumer_ptr)
            {
                consumer_ptr = new PcapConsumerType();
                if (NULL == consumer_ptr)
                {
                    std::cerr << __FILE__ << " " << __LINE__ << " consumer is not able to be set" << std::endl;
                    return -1;
                }
            }

            call_factory_ptr = new MirrorCallFactoryType();
            if (!call_factory_ptr)
            {
                std::cerr << __FILE__ << " " << __LINE__ << " call factory is not able to be generated" << std::endl;
                return -1;
            }
        }
        else if (0 == strcmp("--pcap", argv[argv_index]))
        {
            if (consumer_ptr)
            {
                delete consumer_ptr;
                consumer_ptr = NULL;
            }

            consumer_ptr = new PcapConsumerType();
            if (!consumer_ptr)
            {
                std::cerr << __FILE__ << " " << __LINE__ << " consumer is not able to be set" << std::endl;
                return -1;
            }
        }
        else if ((0 == strcmp("--socket", argv[argv_index]) ) && ((argv_index + 2) < argc))
        {
            in_addr d_inaddr;
            if (1 == inet_aton(argv[argv_index + 1], &d_inaddr))
            {
                dst_ip = ntohl(d_inaddr.s_addr);
            }

            dst_port = std::atoi(argv[argv_index + 2]);
            IpPort dst_ipport(dst_ip, dst_port);

            dst_ipport_vector.push_back(dst_ipport);

            argv_index += 2;

            if (consumer_ptr)
            {
                delete consumer_ptr;
                consumer_ptr = NULL;
            }

            consumer_ptr = new SocketConsumerType(dst_ipport_vector);
            if (!consumer_ptr)
            {
                std::cerr << __FILE__ << " " << __LINE__ << " consumer is not able to be set" << std::endl;
                return -1;
            }
        }
        else
        {
            std::cout << "unknown option : " << argv[argv_index] << std::endl;
            DisplayUsage();
            return -1;
        }
    }

    /*
    unsigned int src_addr = 0xac186536;
    unsigned short int src_port = 58274;
    unsigned int dst_addr = 0xac1b1e69;
    unsigned short int dst_port = 22698;
    unsigned short int id = 0xd962;
    unsigned int timestamp = 2286760;
    unsigned int ssrc = 0x3705a01b;
    unsigned short int seq_num = 2821;

    CallLegType call_leg(src_addr, src_port, dst_addr, dst_port, id, timestamp, ssrc, seq_num, &g711a_encoder_factory, &single_tone_generator_factory);
    call_leg.Step(20);
    call_leg.Step(20);

    */

    // std::vector<IpPort> drlink_ipport_vector;
    // drlink_ipport_vector.push_back(IpPort(0xac1b1e69, 22698));
    // drlink_ipport_vector.push_back(IpPort(0xac1b1e7a, 22799));
    // unsigned int src_ip = 0xac186536;

    // std::vector<DRLinkCallType*> drlink_call_ptr_vector;
    std::vector<CallType*> call_ptr_vector;

    unsigned int last_epoch_sec = 0;
    unsigned int last_epoch_usec = 0;
    unsigned int current_sec = 0;
    unsigned int current_usec = 0;

    GetCurrentTimeInTv(last_epoch_sec, last_epoch_usec);
    std::clog << "last epoch sec: " << last_epoch_sec << " usec: " << last_epoch_usec << std::endl;

    // form a seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // introduce generator
    std::minstd_rand generator(seed);
    std::uniform_int_distribution<unsigned short int> usint_distribution(duration_of_calls*0.75, duration_of_calls*1.25);

    while(true)
    {
        if (kbhit())
        {
            /*
            char in_ch = std::cin.getchar();
            if (('Q' == in_ch) || ('q' == in_ch))
            {
                std::cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " terminating ddgen..." << std::endl;
                break;
            }
            */
            break;
        }

        //std::cout << "a loop" << std::endl;
        if (call_ptr_vector.size() < number_of_calls)
        {
            unsigned short int call_duration = usint_distribution(generator);
            if (!call_factory_ptr)
            {
                std::cout << __FILE__ << " " << __LINE__ << " call factory ptr is null " << std::endl;
                return -1;
            }

            CallType* call_ptr = call_factory_ptr->CreateCall(call_duration, &g711a_encoder_factory, &single_tone_generator_factory, consumer_ptr);
            if (!call_ptr)
            {
                std::cerr << __FILE__ << " " << __LINE__ << "unable to create call_ptr" << std::endl;
                return -1;
            }

            call_ptr_vector.push_back(call_ptr);
            std::cout << " a call is created with duration " << call_duration << std::endl;

            /*
            DRLinkCallType* drlink_call_ptr = new DRLinkCallType(drlink_ipport_vector, src_ip, call_duration, &g711a_encoder_factory,
                                      &single_tone_generator_factory, consumer_ptr);
            if (!drlink_call_ptr)
            {
                std::cerr << __FILE__ << " " << __LINE__ << "unable to create drlink_call_ptr" << std::endl;
                return -1;
            }

            drlink_call_ptr_vector.push_back(drlink_call_ptr);
            std::cout << " a call is created with duration " << call_duration << std::endl;
            src_ip ++;

            */
        }

        GetCurrentTimeInTv(current_sec, current_usec);
        unsigned int ellapsed_time = ((current_sec - last_epoch_sec)*1000000 + (current_usec - last_epoch_usec));

        // std::clog << "current sec: " << current_sec << " usec: " << current_usec << " ellapsed time " << ellapsed_time << " ms " << std::endl;

        if (ellapsed_time > 40000)
        {
            std::clog << __FILE__ << " " << __LINE__ << "... too much lag: " << ellapsed_time / 1000 << " ms " << std::endl;
        }

        if (ellapsed_time > 20000)
        {
            // std::clog << "......time for step " << std::endl;
            /*
            for (std::vector<DRLinkCallType*>::iterator it = drlink_call_ptr_vector.begin(); it != drlink_call_ptr_vector.end(); ++it)
            {
                // std::clog << "......iterating" << std::endl;
                if (*it)
                {
                    bool step_result = (*it) -> Step(20);
                    if (false == step_result)
                    {
                        std::clog << "a calltimed out " << std::endl;
                        delete (*it);
                        it = drlink_call_ptr_vector.erase(it);
                        --it;
                    }
                }
                else
                {
                    std::cerr << __FILE__ << " " << __LINE__ << "iteratoris null " << std::endl;
                    return -1;
                }
            }
            */

            for (std::vector<CallType*>::iterator it = call_ptr_vector.begin(); it != call_ptr_vector.end(); ++it)
            {
                // std::clog << "......iterating" << std::endl;
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

            last_epoch_usec += 20000;
            last_epoch_sec += (last_epoch_usec / 1000000);
            last_epoch_usec = last_epoch_usec % 1000000;
        }
        else
        {
            // std::clog << "..starting to sleep" << (20000 -ellapsed_time) << std::endl;
            SleepSystemUsec(20000 - ellapsed_time);
            // std::clog << "..awake" << std::endl;
        }
    }

    // free all iterators
    /*
    for (std::vector<DRLinkCallType*>::iterator it = drlink_call_ptr_vector.begin(); it != drlink_call_ptr_vector.end(); ++it)
    {
        if (*it)
        {
            delete (*it);
            *it = NULL;
        }
    }
    */

    for (std::vector<CallType*>::iterator it = call_ptr_vector.begin(); it != call_ptr_vector.end(); ++it)
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

    if (call_factory_ptr)
    {
        delete call_factory_ptr;
        call_factory_ptr = NULL;
    }

    return 0;
}
