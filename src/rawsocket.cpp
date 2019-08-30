#include "rawsocket.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>

unsigned short int OnesComplementShortSummation(const unsigned char* data_ptr, unsigned short int data_size)
{
    unsigned int sum = 0;

    for (unsigned short int i = 0; i < data_size/2; i++)
    {
        sum += ntohs(*((unsigned short int*)data_ptr));
        data_ptr += 2;
    }

    if (data_size % 2)
        sum += *data_ptr;

    // sum carry
    sum = (sum >> 16) + (sum & 0x0000ffff);

    return (unsigned short int)sum;
}

// *************************************** RtpHeaderType *********************************************

bool RtpHeaderType::ReadFromBuffer(const unsigned char* buffer_ptr)
{
    // check if buffer_ptr points to somewhere (hopefully) to 12 byte rtp header
    if (NULL == buffer_ptr)
        return false;

    // read whole bunch
    *this = *((const RtpHeaderType*)buffer_ptr);

    // correct seq_num, time stamp and ssrc from network byte order to host byte order
    seq_num = ntohs(seq_num);
    timestamp = ntohl(timestamp);
    ssrc = ntohl(ssrc);

    return true;
}

bool RtpHeaderType::WriteToBuffer(unsigned char* buffer_ptr) const
{
    // check if buffer_ptr points to somewhere (hopefully) to 12 byte rtp header
    if (NULL == buffer_ptr)
        return false;

    RtpHeaderType dummy_rtp_header = *this;
    // correct seq_num, time stamp and ssrc from host byte order to network byte order
    dummy_rtp_header.seq_num = htons(dummy_rtp_header.seq_num);
    dummy_rtp_header.timestamp = htonl(dummy_rtp_header.timestamp);
    dummy_rtp_header.ssrc = htonl(dummy_rtp_header.ssrc);

    // write whole bunch
    *((RtpHeaderType*)buffer_ptr) = dummy_rtp_header;

    return true;
}

bool RtpHeaderType::operator ==(const RtpHeaderType& rhs) const
{
    if (0 == std::memcmp(this, &rhs, sizeof(RtpHeaderType)))
        return true;
    else
        return false;
}

void RtpHeaderType::Display() const
{
    std::cout << "------------------ RTP Header ---------------------" << std::endl;
    std::cout << "Version               : " << (unsigned int) version << std::endl;
    std::cout << "Padding flag          : " << (unsigned int) p << std::endl;
    std::cout << "Extension flag        : " << (unsigned int) x << std::endl;
    std::cout << "cc                    : " << (unsigned int) cc << std::endl;
    std::cout << "Mark flag             : " << (unsigned int) m << std::endl;
    std::cout << "Payload               : " << (unsigned int) payload << std::endl;
    std::cout << "Sequence number       : " << seq_num << std::endl;
    std::cout << "Time stamp            : " << timestamp << std::endl;
    std::cout << "Source identification : " << (std::hex) << ssrc << (std::dec) << std::endl;

}

// *************************************** PseudoIpv4HeaderType *********************************************

bool PseudoIpv4HeaderType::WriteToBuffer(unsigned char* buffer_ptr) const
{

    // check if buffer_ptr points to somewhere (hopefully) to 12 byte pseudo ipv4 header
    if (NULL == buffer_ptr)
        return false;

    PseudoIpv4HeaderType dummy_pseudo_ipv4_header;

    // correct seq_num, time stamp from host byte order to network byte order
    dummy_pseudo_ipv4_header.data_len = htons(data_len);
    dummy_pseudo_ipv4_header.protocol = protocol;
    dummy_pseudo_ipv4_header.src_addr = htonl(src_addr);
    dummy_pseudo_ipv4_header.dst_addr = htonl(dst_addr);

    // write whole bunch
    *((PseudoIpv4HeaderType*)buffer_ptr) = dummy_pseudo_ipv4_header;

    return true;
}

void PseudoIpv4HeaderType::Display() const
{
    std::cout << "------------------ Pseudo Ipv4 Header ---------------------" << std::endl;
    std::cout << "Source address         : " << std::hex << src_addr << std::dec << std::endl;
    std::cout << "Destination address    : " << std::hex << dst_addr << std::dec << std::endl;
    std::cout << "Protocol               : " << protocol << std::endl;
    std::cout << "Data length            : " << data_len << std::endl;
}

// *************************************** IPv4HeaderType *********************************************

bool Ipv4HeaderType::ReadFromBuffer(const unsigned char* buffer_ptr)
{
    // check if buffer_ptr points to somewhere (hopefully) to 20 byte ipv4 header
    if (NULL == buffer_ptr)
        return false;

    // read whole bunch
    *this = *((const Ipv4HeaderType*)buffer_ptr);

    // correct seq_num, time stamp from network byte order to host byte order
    tot_len = ntohs(tot_len);
    id = ntohs(id);
    checksum = ntohs(checksum);
    src_addr = ntohl(src_addr);
    dst_addr = ntohl(dst_addr);

    return true;
}

bool Ipv4HeaderType::UpdateChecksumWriteToBuffer(unsigned char* buffer_ptr)
{
    // check if buffer_ptr points to somewhere (hopefully) to 20 byte ipv4 header
    if (NULL == buffer_ptr)
        return false;

    Ipv4HeaderType dummy_ipv4_header;

    // correct seq_num, time stamp from host byte order to network byte order
    dummy_ipv4_header.hdr_len = hdr_len;
    dummy_ipv4_header.version = version;
    dummy_ipv4_header.service_type = service_type;
    dummy_ipv4_header.tot_len = htons(tot_len);
    dummy_ipv4_header.id = htons(id);
    dummy_ipv4_header.fragment = htons(fragment);
    dummy_ipv4_header.ttl = ttl;
    dummy_ipv4_header.protocol = protocol;
    dummy_ipv4_header.checksum = htons(0);
    dummy_ipv4_header.src_addr = htonl(src_addr);
    dummy_ipv4_header.dst_addr = htonl(dst_addr);

    // write whole bunch
    *((Ipv4HeaderType*)buffer_ptr) = dummy_ipv4_header;

    // calculate checksum
    checksum = ~(OnesComplementShortSummation(buffer_ptr, ipv4_header_size));

    // modify checksum in buffer_ptr
    *((unsigned short int*)(buffer_ptr + 10)) = htons(checksum);

    return true;
}

bool Ipv4HeaderType::WriteToBuffer(unsigned char* buffer_ptr)
{
    // check if buffer_ptr points to somewhere (hopefully) to 20 byte ipv4 header
    if (NULL == buffer_ptr)
        return false;

    Ipv4HeaderType dummy_ipv4_header;

    // correct seq_num, time stamp from host byte order to network byte order
    dummy_ipv4_header.hdr_len = hdr_len;
    dummy_ipv4_header.version = version;
    dummy_ipv4_header.service_type = service_type;
    dummy_ipv4_header.tot_len = htons(tot_len);
    dummy_ipv4_header.id = htons(id);
    dummy_ipv4_header.fragment = htons(fragment);
    dummy_ipv4_header.ttl = ttl;
    dummy_ipv4_header.protocol = protocol;
    dummy_ipv4_header.checksum = htons(checksum);
    dummy_ipv4_header.src_addr = htonl(src_addr);
    dummy_ipv4_header.dst_addr = htonl(dst_addr);

    // write whole bunch
    *((Ipv4HeaderType*)buffer_ptr) = dummy_ipv4_header;

    return true;
}

bool Ipv4HeaderType::operator ==(const Ipv4HeaderType& rhs) const
{
    if (0 == std::memcmp(this, &rhs, sizeof(Ipv4HeaderType)))
        return true;
    else
        return false;
}

void Ipv4HeaderType::Display() const
{
    std::cout << "------------------ Ipv4 Header ---------------------" << std::endl;
    std::cout << "Version                 : " << (unsigned int) version << std::endl;
    std::cout << "Header length           : " << (unsigned int) hdr_len << std::endl;
    std::cout << "Service type            : " << std::hex << (unsigned int) service_type << std::dec << std::endl;
    std::cout << "Total length            : " << tot_len << std::endl;
    std::cout << "Sequence identification : " << std::hex << id << std::dec << std::endl;
    std::cout << "Fragment information    : " << std::hex << fragment << std::dec << std::endl;
    std::cout << "Time to live            : " << (unsigned int) ttl << std::endl;
    std::cout << "Protocol                : " << (unsigned int) protocol << std::endl;
    std::cout << "Header checksum         : " << std::hex << checksum << std::dec << std::endl;
    std::cout << "Source address          : " << std::hex << src_addr << std::dec << std::endl;
    std::cout << "Destination address     : " << std::hex << dst_addr << std::dec << std::endl;
}

bool CheckIpv4Checksum(const unsigned char* line_ipv4_header_ptr)
{
    unsigned short int sum_result = OnesComplementShortSummation(line_ipv4_header_ptr, ipv4_header_size);
    return (sum_result == 0xffff);
}

// *************************************** UdpHeaderType *********************************************
bool UdpHeaderType::ReadFromBuffer(const unsigned char* buffer_ptr)
{
    // check if buffer_ptr points to somewhere (hopefully) to 8 byte udp header
    if (NULL == buffer_ptr)
        return false;

    // read whole bunch
    *this = *((const UdpHeaderType*)buffer_ptr);

    // correct seq_num, time stamp from network byte order to host byte order
    src_port = ntohs(src_port);
    dst_port = ntohs(dst_port);
    tot_len = ntohs(tot_len);
    checksum = ntohs(checksum);

    return true;
}

bool UdpHeaderType::UpdateChecksumWriteToBuffer(unsigned char* buffer_ptr, const unsigned char* udp_data_ptr, const PseudoIpv4HeaderType& pseudo_ipv4_header)
{
    // check if buffer_ptr points to somewhere (hopefully) to 8 byte udp header
    if (NULL == buffer_ptr)
        return false;

    unsigned char line_pseudo_ipv4_ptr[pseudo_ipv4_header_size];
    pseudo_ipv4_header.WriteToBuffer(line_pseudo_ipv4_ptr);

    unsigned int sum = OnesComplementShortSummation(line_pseudo_ipv4_ptr, pseudo_ipv4_header_size);
    // std::cout << std::hex << sum << " ";

    sum += OnesComplementShortSummation(udp_data_ptr, (tot_len - udp_header_size));
    // std::cout << sum << " ";

    UdpHeaderType dummy_udp_header;
    // correct seq_num, time stamp from host byte order to network byte order
    dummy_udp_header.src_port = htons(src_port);
    dummy_udp_header.dst_port = htons(dst_port);
    dummy_udp_header.tot_len = htons(tot_len);
    dummy_udp_header.checksum = htons(0);

    // write whole bunch
    *((UdpHeaderType*)buffer_ptr) = dummy_udp_header;

    // calculate checksum
    sum  += OnesComplementShortSummation(buffer_ptr, udp_header_size);
    // std::cout << sum << " " << std::dec << std::endl;
    checksum = ~(((sum >> 16) + (sum & 0x0000ffff)));

    // write it also into buffer_ptr
    *((unsigned short int*)(buffer_ptr + 6)) = htons(checksum);

    return true;
}

bool UdpHeaderType::WriteToBuffer(unsigned char* buffer_ptr)
{
    // check if buffer_ptr points to somewhere (hopefully) to 8 byte udp header
    if (NULL == buffer_ptr)
        return false;

    UdpHeaderType dummy_udp_header;
    // correct seq_num, time stamp from host byte order to network byte order
    dummy_udp_header.src_port = htons(src_port);
    dummy_udp_header.dst_port = htons(dst_port);
    dummy_udp_header.tot_len = htons(tot_len);
    dummy_udp_header.checksum = htons(checksum);

    // write whole bunch
    *((UdpHeaderType*)buffer_ptr) = dummy_udp_header;

    return true;
}

bool UdpHeaderType::operator ==(const UdpHeaderType& rhs) const
{
    if (0 == std::memcmp(this, &rhs, sizeof(UdpHeaderType)))
        return true;
    else
        return false;
}

void UdpHeaderType::Display() const
{
    std::cout << "------------------ Udp Header ---------------------" << std::endl;
    std::cout << "Source port         : " << src_port << std::endl;
    std::cout << "Destination port    : " << dst_port << std::endl;
    std::cout << "Packet length       : " << tot_len << std::endl;
    std::cout << "Packet checksum     : " << std::hex << checksum << std::dec << std::endl;
}

bool CheckUdpChecksum(const unsigned char* line_udp_packet_ptr, const PseudoIpv4HeaderType& pseudo_ipv4_header)
{
    // check if line_udp_packet_ptr points to somewhere (hopefully) at least 8 byte of udp packet
    if (NULL == line_udp_packet_ptr)
        return false;

    unsigned char line_pseudo_ipv4_ptr[pseudo_ipv4_header_size];
    pseudo_ipv4_header.WriteToBuffer(line_pseudo_ipv4_ptr);

    unsigned int sum = OnesComplementShortSummation(line_pseudo_ipv4_ptr, pseudo_ipv4_header_size);

    sum += OnesComplementShortSummation(line_udp_packet_ptr,  ntohs(*((unsigned short int*)(line_udp_packet_ptr + 4))));

    return ((unsigned short int)((sum >> 16) + (sum & 0x0000ffff)) == 0xffff);
}

// *************************************** EthernetHeaderType *********************************************

bool EthHeaderType::ReadFromBuffer(const unsigned char* buffer_ptr)
{
    // check if buffer_ptr points to somewhere (hopefully) to 14 byte ethernet header
    if (NULL == buffer_ptr)
        return false;

    // read whole bunch
    *this = *((const EthHeaderType*)buffer_ptr);

    // correct ether type to host byte order
    eth_type = ntohs(eth_type);

    return true;
}

bool EthHeaderType::WriteToBuffer(unsigned char* buffer_ptr)
{
    // check if buffer_ptr points to somewhere (hopefully) to 14 byte ethernet header
    if (NULL == buffer_ptr)
        return false;

    EthHeaderType dummy_eth_header = *this;
    // correct ether type to network byte order
    dummy_eth_header.eth_type = htons(eth_type);

    // write whole bunch
    *((EthHeaderType*)buffer_ptr) = dummy_eth_header;

    return true;
}

void EthHeaderType::Display() const
{
    std::cout << "------------------ Ethernet Header ---------------------" << std::endl;
    std::cout << std::hex;
    std::cout << "Source mac         : " << (unsigned int)src_mac[0] << ":" << (unsigned int)src_mac[1] << ":" << (unsigned int)src_mac[2] << ":" << (unsigned int)src_mac[3] << ":" << (unsigned int)src_mac[4] << ":" << (unsigned int)src_mac[5] << std::endl;
    std::cout << "Destination max    : " << (unsigned int)dst_mac[0] << ":" << (unsigned int)dst_mac[1] << ":" << (unsigned int)dst_mac[2] << ":" << (unsigned int)dst_mac[3] << ":" << (unsigned int)dst_mac[4] << ":" << (unsigned int)dst_mac[5] << std::endl;
    std::cout << "Protocol           : " << eth_type << std::endl;
    std::cout << std::dec;
}
