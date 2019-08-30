#include "callleg.h"

#include <chrono>
#include <iostream>
#include <limits.h>
#include <random>

CallLegType::CallLegType(unsigned int src_addr,
                        unsigned short int src_port,
                        unsigned int dst_addr,
                        unsigned short int dst_port,
                        unsigned short int id,
                        unsigned int timestamp,
                        unsigned int ssrc,
                        unsigned short int seq_num,
                        EncoderFactoryType* encoder_factory_ptr,
                        GeneratorFactoryType* generator_factory_ptr,
                        ConsumerType* consumer_ptr)
{
    m_accumulated_step_time = 0;
    m_remaining_time = 0;
    m_encoder_ptr = encoder_factory_ptr->CreateEncoder();
    m_generator_ptr = generator_factory_ptr->CreateGenerator();
    m_consumer_ptr = consumer_ptr;
    m_line_data.m_rtp_data_size = m_encoder_ptr->GetPacketSize();

    // form rtp header
    m_rtp_header.payload = m_encoder_ptr->GetRtpPayload();
    m_rtp_header.timestamp = timestamp;
    m_rtp_header.ssrc = ssrc;
    m_rtp_header.seq_num = seq_num;
    m_rtp_header.version = 2;
    m_rtp_header.cc = 0;
    m_rtp_header.x = 0;
    m_rtp_header.p = 0;
    m_rtp_header.m = 0;

    // form udp header
    m_udp_header.src_port = src_port;
    m_udp_header.dst_port = dst_port;
    m_udp_header.tot_len = m_line_data.m_rtp_data_size + rtp_header_size + udp_header_size;
    m_udp_header.checksum =0;    // set to zero, willbe updated when writing to buffer

    // form ipv4 header
    m_ipv4_header.hdr_len = 0x5;
    m_ipv4_header.version = 0x4;
    m_ipv4_header.service_type = 0xb8;
    m_ipv4_header.fragment = 0;
    m_ipv4_header.ttl = 128;
    m_ipv4_header.src_addr = src_addr;
    m_ipv4_header.dst_addr = dst_addr;
    m_ipv4_header.tot_len = m_udp_header.tot_len + ipv4_header_size;
    m_ipv4_header.protocol = 17;
    m_ipv4_header.id = id;
    m_ipv4_header.checksum = 0;    // set to zero, will be updated when writing to buffer

    // form pseudo ipv4 header
    m_pseudo_ipv4_header.src_addr = m_ipv4_header.src_addr;
    m_pseudo_ipv4_header.dst_addr = m_ipv4_header.dst_addr;
    m_pseudo_ipv4_header.protocol = m_ipv4_header.protocol;
    m_pseudo_ipv4_header.data_len = m_udp_header.tot_len;

    // form ethernet header
    *((unsigned int*) (m_eth_header.src_mac + 2)) = htonl(src_addr);
    *((unsigned int*) (m_eth_header.dst_mac + 2)) = htonl(dst_addr);
    m_eth_header.eth_type = 0x0800;
}

CallLegType::~CallLegType()
{
    if (m_encoder_ptr)
    {
        delete m_encoder_ptr;
        m_encoder_ptr = NULL;
    }

    if (m_generator_ptr)
    {
        delete m_generator_ptr;
        m_generator_ptr = NULL;
    }
}

void CallLegType::Step(unsigned short int step_duration)
{
    m_accumulated_step_time += step_duration;

    while (m_accumulated_step_time >= m_encoder_ptr->GetPacketDuration())
    {
        if (!m_generator_ptr->Generate(m_pcm_data_ptr,  m_line_data.m_rtp_data_size))
        {
            std::cerr << __FILE__ << " " << __LINE__ << "m_generator_ptr->Generate() failed" << std::endl;
            return;
        }

        if (!m_encoder_ptr->Encode(m_pcm_data_ptr, m_line_data.m_rtp_data_ptr))
        {
            std::cerr << __FILE__ << " " << __LINE__ << "m_encoder_ptr->Encode() failed" << std::endl;
            return;
        }

        if (false == m_rtp_header.WriteToBuffer(m_line_data.m_rtp_hdr_ptr))
        {
            std::cerr << __FILE__ << " " << __LINE__ << "m_rtp_header.WriteToBuffer() failed" << std::endl;
            return;
        }

        // update udp length if necessary
        // m_udp_header.tot_len =
        if (false == m_udp_header.UpdateChecksumWriteToBuffer(m_line_data.m_udp_hdr_ptr, m_line_data.m_rtp_hdr_ptr, m_pseudo_ipv4_header))
        {
            std::cerr << __FILE__ << " " << __LINE__ << "m_udp_header.UpdateChecksumWriteToBuffer() failed" << std::endl;
            return;
        }

        if (false == m_ipv4_header.UpdateChecksumWriteToBuffer(m_line_data.m_ipv4_hdr_ptr))
        {
            std::cerr << __FILE__ << " " << __LINE__ << "m_ipv4_header.UpdateChecksumWriteToBuffer() failed" << std::endl;
            return;
        }

        if (false == m_eth_header.WriteToBuffer(m_line_data.m_eth_hdr_ptr))
        {
            std::cerr << __FILE__ << " " << __LINE__ << "m_eth_header.WriteToBuffer() failed" << std::endl;
            return;
        }

        m_consumer_ptr->Consume(m_line_data.m_line_data, m_line_data.LineDataSize());

        // update necessary fields for the next iteration / step
        m_accumulated_step_time -= m_encoder_ptr->GetPacketDuration();
        m_rtp_header.seq_num ++;
        m_rtp_header.timestamp += m_encoder_ptr->GetPacketSize();

        // increment ip identification field
        m_ipv4_header.id ++;
    }
}

DRLinkCallType::DRLinkCallType(std::vector<IpPort>& dst_inf, unsigned int src_ip, unsigned int duration,
                    EncoderFactoryType* encoder_factory_ptr,
                    GeneratorFactoryType* generator_factory_ptr,
                    ConsumerType* consumer_ptr)
{
    m_duration = duration * 1000;

    int no_of_call_legs = dst_inf.size();
    unsigned short id_offset = USHRT_MAX / no_of_call_legs;
    unsigned short int id = 1;

    // form a seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // introduce generator
    std::minstd_rand generator(seed);

    std::uniform_int_distribution<unsigned int> uint_distribution(0, UINT_MAX);
    std::uniform_int_distribution<unsigned short int> usint_distribution(0, USHRT_MAX);

    unsigned short int src_port = 32514;
    for (std::vector<IpPort>::iterator it = dst_inf.begin(); it != dst_inf.end(); ++it)
    {
        unsigned int dst_ip = it -> m_ipv4;
        unsigned short int dst_port = it -> m_port;

        unsigned int timestamp = uint_distribution(generator);
        unsigned int ssrc = uint_distribution(generator);
        unsigned short int seq_num = usint_distribution(generator);

        CallLegType* call_leg_ptr = new CallLegType(src_ip, src_port, dst_ip, dst_port, id, timestamp, ssrc, seq_num, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
        if (call_leg_ptr)
        {
            std::clog << "-------------------------------- CALL LEG CREATION -------------------------------------" << std::endl;
            std::clog << "HOST src : " << std::hex << src_ip << std::dec << ":" << src_port << " dst: " << std::hex << dst_ip << std::dec << ":" << dst_port << std::endl;
            std::clog << "NET src : " << std::hex << htonl(src_ip) <<std::dec << ":" << htons(src_port) << " dst: " << std::hex << htonl(dst_ip) << std::dec << ":" << htons(dst_port) << std::endl;
            m_call_leg_ptr_vector.push_back(call_leg_ptr);
        }

        src_port += 2;
        id += id_offset;
    }
}

DRLinkCallType::~DRLinkCallType()
{
    for (std::vector<CallLegType*>::iterator it = m_call_leg_ptr_vector.begin(); it != m_call_leg_ptr_vector.end(); ++it)
    {
        delete *it;
        *it = NULL;
    }
}

bool DRLinkCallType::Step(unsigned int step_duration)
{
    bool still_has_time = true;
    if (m_duration < step_duration)
    {
        // change step duration to whatever time call has
        step_duration = m_duration;
        still_has_time = false;
    }

    for (std::vector<CallLegType*>::iterator it = m_call_leg_ptr_vector.begin(); it != m_call_leg_ptr_vector.end(); ++it)
    {
        (*it)-> Step(step_duration);
    }

    m_duration -= step_duration;
    return still_has_time;
}

MirrorCallType::MirrorCallType(unsigned int src_ip, unsigned int dst_ip, unsigned int duration,
                    EncoderFactoryType* encoder_factory_ptr,
                    GeneratorFactoryType* generator_factory_ptr,
                    ConsumerType* consumer_ptr)
{
    m_duration = duration * 1000;

    int no_of_call_legs = 2;
    unsigned short id_offset = USHRT_MAX / no_of_call_legs;
    unsigned short int id = 1;

    // form a seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // introduce generator
    std::minstd_rand generator(seed);

    std::uniform_int_distribution<unsigned int> uint_distribution(0, UINT_MAX);
    std::uniform_int_distribution<unsigned short int> usint_distribution(0, USHRT_MAX);

    unsigned short int src_port = 32514;
    unsigned short int dst_port = 32514;


    unsigned int timestamp = uint_distribution(generator);
    unsigned int ssrc = uint_distribution(generator);
    unsigned short int seq_num = usint_distribution(generator);

    CallLegType* src_call_leg_ptr = new CallLegType(src_ip, src_port, dst_ip, dst_port, id, timestamp, ssrc, seq_num, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
    if (src_call_leg_ptr)
    {
        m_call_leg_ptr_vector.push_back(src_call_leg_ptr);
    }

    id += id_offset;
    timestamp = uint_distribution(generator);
    ssrc = uint_distribution(generator);
    seq_num = usint_distribution(generator);

    CallLegType* dst_call_leg_ptr = new CallLegType(dst_ip, dst_port, src_ip, src_port, id, timestamp, ssrc, seq_num, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
    if (dst_call_leg_ptr)
    {
        m_call_leg_ptr_vector.push_back(dst_call_leg_ptr);
    }

    std::clog << "-------------------------------- MIRROR CALL CREATION -------------------------------------" << std::endl;
    std::clog << "HOST : " << std::hex << src_ip << std::dec << ":" << src_port << " <--> " << std::hex << dst_ip << std::dec << ":" << dst_port << std::endl;
    std::clog << "NET  : " << std::hex << htonl(src_ip) <<std::dec << ":" << htons(src_port) << " <--> " << std::hex << htonl(dst_ip) << std::dec << ":" << htons(dst_port) << std::endl;
}

MirrorCallType::~MirrorCallType()
{
    for (std::vector<CallLegType*>::iterator it = m_call_leg_ptr_vector.begin(); it != m_call_leg_ptr_vector.end(); ++it)
    {
        delete *it;
        *it = NULL;
    }
}

bool MirrorCallType::Step(unsigned int step_duration)
{
    bool still_has_time = true;
    if (m_duration < step_duration)
    {
        // change step duration to whatever time call has
        step_duration = m_duration;
        still_has_time = false;
    }

    for (std::vector<CallLegType*>::iterator it = m_call_leg_ptr_vector.begin(); it != m_call_leg_ptr_vector.end(); ++it)
    {
        (*it)-> Step(step_duration);
    }

    m_duration -= step_duration;
    return still_has_time;
}

CallType* MirrorCallFactoryType::CreateCall(unsigned int duration,
                    EncoderFactoryType* encoder_factory_ptr,
                    GeneratorFactoryType* generator_factory_ptr,
                    ConsumerType* consumer_ptr)
{
    unsigned int src_ip = m_ip_pool++;
    unsigned int dst_ip = m_ip_pool++;
    return new MirrorCallType(src_ip, dst_ip, duration, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
}
