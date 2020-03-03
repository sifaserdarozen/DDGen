#include "programoptions.h"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace ddgen
{

ProgramOptions::ProgramOptions(int argc, char* argv[]) : numberOfCalls(10), callDuration(60), simulationDuration(600), startIp(0xac186536), traffic(Traffic::Mirror), output(Output::Pcap), shouldUseSecureWebInterface(false), useDb(false), useS3(false), stackName("ddgen")
{

    for (int argv_index = 1; argv_index < argc; ++argv_index)
    {
        if ((0 == strcmp("--nc", argv[argv_index])) && ((argv_index + 1) < argc))
        {
            numberOfCalls = std::atoi(argv[argv_index + 1]);
            argv_index++;
        }
        else if ((0 == strcmp("--dc", argv[argv_index])) && ((argv_index + 1) < argc))
        {
            callDuration = std::atoi(argv[argv_index + 1]);
            argv_index++;
        }
        else if ((0 == strcmp("--ds", argv[argv_index])) && ((argv_index + 1) < argc))
        {
            simulationDuration = std::atoi(argv[argv_index + 1]);
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

            drlinkIpPortVector.push_back(dst_ipport);

            if (1 == inet_aton(argv[argv_index + 3], &d_inaddr))
            {
                dst_ip = ntohl(d_inaddr.s_addr);
            }

            dst_port = std::atoi(argv[argv_index + 4]);
            ddgen::IpPort dst_ipport2(dst_ip, dst_port);

            drlinkIpPortVector.push_back(dst_ipport2);
            dstIpPortVector = drlinkIpPortVector;
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

            dstIpPortVector.push_back(dst_ipport);
            argv_index += 2;

            output = Output::Socket;
        }
        else if ((0 == strcmp("--start", argv[argv_index]) ) && ((argv_index + 1) < argc))
        {
            in_addr s_inaddr;
            if (1 == inet_aton(argv[argv_index + 1], &s_inaddr))
            {
                startIp = ntohl(s_inaddr.s_addr);
            }
            argv_index += 1;
        }
        else if ((0 == strcmp("--dbPath", argv[argv_index]) ) && ((argv_index + 1) < argc))
        {
            useDb = true;
            dbPath = argv[argv_index + 1];
            argv_index += 1;
        }
        else if (0 == strcmp("--secure", argv[argv_index]))
        {
            shouldUseSecureWebInterface = true;
        }
        else if (0 == strcmp("--useDb", argv[argv_index]))
        {
            useDb = true;
        }
        else if (0 == strcmp("--useS3", argv[argv_index]))
        {
            useS3 = true;
        }
        else if ((0 == strcmp("--sn", argv[argv_index])) && ((argv_index + 1) < argc))
        {
            stackName = std::string(argv[argv_index + 1]);
            argv_index++;
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
    std::cout << "To disable db usage (in case it is build) use  --noDb" << std::endl;
    std::cout << "To force a database path use --dbPath http://localhost:8000" << std::endl;
    std::cout << "To push pcap (if any) to s3 (in case it is build with) use --useS3" << std::endl;
}

}
