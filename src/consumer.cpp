#include "consumer.h"

#include <cstring>
#include <cerrno>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/time.h>
#include <unistd.h>

namespace ddgen
{

void GetCurrentTimeInTv(unsigned int& sec, unsigned int& usec)
{
    timeval tv;
    if (0 == gettimeofday(&tv, NULL))
    {
        sec = (unsigned int)tv.tv_sec;
        usec = (unsigned int)tv.tv_usec;
    }
    else
        std::cerr << __FILE__ << " " << __LINE__ << " unable to obtain time info" << std::endl;
}

SocketConsumer::SocketConsumer(const std::vector<IpPort>& dstIpPort)
{
    for (std::vector<IpPort>::const_iterator it = dstIpPort.begin(); it != dstIpPort.end(); ++it)
    {
        sockaddr_in dst_address;
        dst_address.sin_family = AF_INET;

        memset(dst_address.sin_zero, '\0', sizeof (dst_address.sin_zero));

        dst_address.sin_port = htons(it->m_port);
        dst_address.sin_addr.s_addr = htonl(it->m_ipv4);

        std::clog << " socket info in netw: " << std::hex << dst_address.sin_addr.s_addr << std::dec << ":" << dst_address.sin_port << std::endl;
        std::clog << " socket info in host: " << std::hex << it->m_ipv4 << std::dec << ":" << it->m_port << std::endl;

        m_dst_sockaddr_vector.push_back(dst_address);

        // create corresponding socket
        int my_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

        if (-1 == my_socket)
        {
            std::cerr << __FILE__ << " " << __LINE__ << " socket initialisation error: " << errno << " " << strerror(errno) << std::endl;
            return;
        }

        int on = 1;

        if (-1 == setsockopt(my_socket, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)))
        {
            std::cerr << __FILE__ << " " << __LINE__ << " socket options error: "  << errno << " " << strerror(errno) << std::endl;
            std::cerr << "Must have root privileges!" << std::endl;
            close(my_socket);
        }
        else
        {
            m_dst_sock_vector.push_back(my_socket);
            std::cout << "socket options successfully set..." << std::endl;
        }
    }
}

SocketConsumer::~SocketConsumer()
{
    for (std::vector<int>::iterator it = m_dst_sock_vector.begin(); it != m_dst_sock_vector.end(); ++it)
    {
        close(*it);
        *it = -1;
    }
}

bool SocketConsumer::Consume(const unsigned char* data_ptr, unsigned short int data_size)
{
    // first determine the socket
    unsigned int dst_ip = *((unsigned int *)(data_ptr + eth_header_size + 16));
    unsigned short int dst_port = *((unsigned short int *)(data_ptr + eth_header_size + ipv4_header_size + 2));

    std::vector<int>::iterator sock_it = m_dst_sock_vector.begin();

    for (std::vector<sockaddr_in>::iterator it = m_dst_sockaddr_vector.begin(); it != m_dst_sockaddr_vector.end(); ++it, ++sock_it)
    {
        if ((it->sin_port == dst_port) && (it->sin_addr.s_addr == dst_ip))
        {
            const auto sended_data_size = sendto(*sock_it, (const char *)(data_ptr + eth_header_size), (data_size - eth_header_size), 0, (struct sockaddr *)(&(*it)), (socklen_t)sizeof(*it));
            if (-1 == sended_data_size) {
		        std::cerr << __FILE__ << " " << __LINE__ << " unable to send data of size : " << data_size << " to socket : " << *sock_it << std::endl;
                return false;
	        } else {
                return true;
            }
        }
    }

    // if not able to find a match, send it to the first socket, possible pair mode
    std::vector<sockaddr_in>::iterator net_it = m_dst_sockaddr_vector.begin();
    sock_it = m_dst_sock_vector.begin();

    if ((net_it != m_dst_sockaddr_vector.end()) && (sock_it != m_dst_sock_vector.end())) {
        const auto sended_data_size = sendto(*sock_it, (const char *) (data_ptr + eth_header_size), (data_size - eth_header_size), 0, (struct sockaddr *) (&(*net_it)), (socklen_t)sizeof(*net_it));
        if (-1 == sended_data_size)
        {
            std::cerr << __FILE__ << " " << __LINE__ << " unable to send data of size : " << data_size << " to socket : " << *sock_it << std::endl;
            return false;
        } else {
            return true;
        }
    }

    std::cerr << __FILE__ << " " << __LINE__ << " unable to find socket to send for destination: " << std::hex << dst_ip << std::dec << ":" << dst_port << std::endl;
    return false;
}


PcapConsumer::PcapConsumer(const std::shared_ptr<ICallStorage>& callStorage) : _callStorage(callStorage), _fileSize(0)
{
    GenerateFileName();

    _fileStream.open(_fileName.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);

    if (!(_fileStream.is_open()))
    {
        std::cerr << __FILE__ << " " << __LINE__ << " output stream is not able to be added. Filename : " << _fileName << std::endl;
        return;
    }

    int pcap_file_hdr_size = sizeof(_pcapFileHeader);
    _fileStream.write((char*)(&_pcapFileHeader), pcap_file_hdr_size);
    _fileSize = pcap_file_hdr_size;
}

PcapConsumer::~PcapConsumer()
{
    if (_fileStream.is_open())
        _fileStream.close();

    _callStorage->Store(_fileName);

    _fileName.clear();
    _fileStream.clear();
    _fileSize = 0;
}

void PcapConsumer::GenerateFileName()
{
    time_t call_start_sec_tt;
    time(&call_start_sec_tt);

    tm* call_time_ptr = localtime(&call_start_sec_tt);
    tm call_time = *call_time_ptr;

    const int time_part_size = 20; //4+1+2+1+2+1+2+1+2+1+2+1
    char time_part[time_part_size];

    int snprintf_result = snprintf(time_part, time_part_size, "%d %.2d %.2d %.2d %.2d %.2d", ((call_time.tm_year)+1900),
                                ((call_time.tm_mon) + 1), (call_time.tm_mday), (call_time.tm_hour), (call_time.tm_min),
                                      (call_time.tm_sec));

    if ((snprintf_result <= 0) || (snprintf_result >= time_part_size))
    {
        std::cerr << __FILE__ << " " << __LINE__ << "unable to execute snprintf" << std::endl;
        _fileName = "TestFile.pcap";
        return;
    }

    _fileName = std::string(time_part) + ".pcap";
}

bool PcapConsumer::Consume(const unsigned char* data_ptr, unsigned short int data_size)
{
    PcapPacHdrType pcap_packet_header;

    GetCurrentTimeInTv(pcap_packet_header.ts_sec, pcap_packet_header.ts_usec);
    pcap_packet_header.incl_len = data_size;
    pcap_packet_header.orig_len = data_size;

    int pcap_packet_header_size = sizeof(PcapPacHdrType);

    if (!(_fileStream.is_open()))
    {
        _fileStream.open(_fileName.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);

        if (!(_fileStream.is_open()))
        {
            std::cerr << __FILE__ << " " << __LINE__ << " output stream is not able to be added. Filename : " << _fileName << std::endl;
            return false;
        }
    }

    _fileStream.write((char*)(&pcap_packet_header), pcap_packet_header_size);
    _fileSize += pcap_packet_header_size;

    _fileStream.write((const char*)data_ptr, data_size);
    _fileSize += data_size;

    _fileStream.flush();

    return true;
}
}
