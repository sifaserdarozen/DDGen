#include "programoptions.h"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace ddgen
{

ProgramOptions::ProgramOptions(int argc, char* argv[]) : number_of_calls(10), duration_of_calls(60), start_ip(0xac186536), traffic(Traffic::Mirror), output(Output::Pcap) 
{

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
            unsigned int dst_ip = 0x691e1bac;
            unsigned short int dst_port = 29000;

            if (1 == inet_aton(argv[argv_index + 1], &d_inaddr))
            {
                dst_ip = ntohl(d_inaddr.s_addr);
            }

            dst_port = std::atoi(argv[argv_index + 2]);
            ddgen::IpPort dst_ipport(dst_ip, dst_port);

            drlink_ipport_vector.push_back(dst_ipport);

            if (1 == inet_aton(argv[argv_index + 3], &d_inaddr))
            {
                dst_ip = ntohl(d_inaddr.s_addr);
            }

            dst_port = std::atoi(argv[argv_index + 4]);
            ddgen::IpPort dst_ipport2(dst_ip, dst_port);

            drlink_ipport_vector.push_back(dst_ipport2);
            dst_ipport_vector = drlink_ipport_vector;
            argv_index += 4;

            traffic = Traffic::DrLink;
            output = Output::Socket;
        }
        else if (0 == strcmp("--mirror", argv[argv_index]))
        {
            traffic = Traffic::Mirror;
        }
        else if (0 == strcmp("--pcap", argv[argv_index]))
        {
            output = Output::Pcap;
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

            dst_ipport_vector.push_back(dst_ipport);
            argv_index += 2;

            output = Output::Socket;
        }
        else if ((0 == strcmp("--start", argv[argv_index]) ) && ((argv_index + 1) < argc))
        {
            in_addr s_inaddr;
            if (1 == inet_aton(argv[argv_index + 1], &s_inaddr))
            {
                start_ip = ntohl(s_inaddr.s_addr);
            }
            argv_index += 1;
        }
        else
        {
            std::cout << "unknown option : " << argv[argv_index] << std::endl;
            DisplayUsage();
            exit(-1);
        }
    }
}

void ProgramOptions::DisplayUsage() const
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

}