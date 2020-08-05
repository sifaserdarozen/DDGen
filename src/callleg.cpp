#include "callleg.h"


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iomanip>

#include <chrono>
#include <time.h>
#include <sstream>
#include <iostream>
#include <limits.h>
#include <random>

namespace ddgen {

    CallLeg::CallLeg(unsigned int src_addr,
                     unsigned short int src_port,
                     unsigned int dst_addr,
                     unsigned short int dst_port,
                     unsigned short int id,
                     unsigned int timestamp,
                     unsigned int ssrc,
                     unsigned short int seq_num,
                     EncoderFactory *encoder_factory_ptr,
                     GeneratorFactory *generator_factory_ptr,
                     const std::shared_ptr<IConsumer>& consumer) {
        m_accumulated_step_time = 0;
        m_remaining_time = 0;
        m_encoder_ptr = encoder_factory_ptr->CreateEncoder();
        m_generator_ptr = generator_factory_ptr->CreateGenerator();
        _consumer = consumer;
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
        m_udp_header.checksum = 0;    // set to zero, willbe updated when writing to buffer

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
        *((unsigned int *) (m_eth_header.src_mac + 2)) = htonl(src_addr);
        *((unsigned int *) (m_eth_header.dst_mac + 2)) = htonl(dst_addr);
        m_eth_header.eth_type = 0x0800;
    }

    CallLeg::~CallLeg() {
        if (m_encoder_ptr) {
            delete m_encoder_ptr;
            m_encoder_ptr = NULL;
        }

        if (m_generator_ptr) {
            delete m_generator_ptr;
            m_generator_ptr = NULL;
        }
    }

    void CallLeg::Step(unsigned short int stepDuration) {
        m_accumulated_step_time += stepDuration;

        while (m_accumulated_step_time >= m_encoder_ptr->GetPacketDuration()) {
            if (!m_generator_ptr->Generate(m_pcm_data_ptr, m_line_data.m_rtp_data_size)) {
                std::cerr << __FILE__ << " " << __LINE__ << "m_generator_ptr->Generate() failed" << std::endl;
                return;
            }

            if (!m_encoder_ptr->Encode(m_pcm_data_ptr, m_line_data.m_rtp_data_ptr)) {
                std::cerr << __FILE__ << " " << __LINE__ << "m_encoder_ptr->Encode() failed" << std::endl;
                return;
            }

            if (false == m_rtp_header.WriteToBuffer(m_line_data.m_rtp_hdr_ptr)) {
                std::cerr << __FILE__ << " " << __LINE__ << "m_rtp_header.WriteToBuffer() failed" << std::endl;
                return;
            }

            // update udp length if necessary
            // m_udp_header.tot_len =
            if (false == m_udp_header.UpdateChecksumWriteToBuffer(m_line_data.m_udp_hdr_ptr, m_line_data.m_rtp_hdr_ptr,
                                                                  m_pseudo_ipv4_header)) {
                std::cerr << __FILE__ << " " << __LINE__ << "m_udp_header.UpdateChecksumWriteToBuffer() failed"
                          << std::endl;
                return;
            }

            if (false == m_ipv4_header.UpdateChecksumWriteToBuffer(m_line_data.m_ipv4_hdr_ptr)) {
                std::cerr << __FILE__ << " " << __LINE__ << "m_ipv4_header.UpdateChecksumWriteToBuffer() failed"
                          << std::endl;
                return;
            }

            if (false == m_eth_header.WriteToBuffer(m_line_data.m_eth_hdr_ptr)) {
                std::cerr << __FILE__ << " " << __LINE__ << "m_eth_header.WriteToBuffer() failed" << std::endl;
                return;
            }

            _consumer->Consume(m_line_data.m_line_data, m_line_data.LineDataSize());

            // update necessary fields for the next iteration / step
            m_accumulated_step_time -= m_encoder_ptr->GetPacketDuration();
            m_rtp_header.seq_num++;
            m_rtp_header.timestamp += m_encoder_ptr->GetPacketSize();

            // increment ip identification field
            m_ipv4_header.id++;
        }
    }

    CallParameters::StreamParameters CallLeg::GetParameters() const {
        const in_addr sourceAddr{htonl(m_ipv4_header.src_addr)};
        const auto sourceIp = std::string(inet_ntoa(sourceAddr));
        const in_addr destinationAddr{htonl(m_ipv4_header.dst_addr)};
        const auto destinationIp = std::string(inet_ntoa(destinationAddr));
        return {sourceIp, m_udp_header.src_port, destinationIp, m_udp_header.dst_port, m_rtp_header.payload,
                m_rtp_header.timestamp, m_rtp_header.ssrc, m_rtp_header.seq_num, m_generator_ptr->GetParameters()};
    }

    Call::Call(unsigned int duration, const std::shared_ptr <ICallLogger> &callLogger) : m_duration(duration * 1000),
                                                                                         _callLogger(callLogger) {
    }

    bool Call::Step(unsigned int step_duration) {
        bool still_has_time = true;
        if (m_duration < step_duration) {
            // change step duration to whatever time call has
            step_duration = m_duration;
            still_has_time = false;
        }

        for (auto &cl : m_call_leg_ptr_vector) {
            cl->Step(step_duration);
        }

        m_duration -= step_duration;
        return still_has_time;
    }

    void Call::Log() {
        CallParameters parameters;

        for (const auto &cl : m_call_leg_ptr_vector) {
            parameters.streams.push_back(cl->GetParameters());
        }

        const auto now = std::chrono::system_clock::now();
        time_t tt = std::chrono::system_clock::to_time_t(now);
        tm utc_tm = *gmtime(&tt);

        std::ostringstream dateStr;
        dateStr << std::setw(4) << std::setfill('0') << utc_tm.tm_year + 1900;
        dateStr << std::setw(2) << utc_tm.tm_mon + 1 << utc_tm.tm_mday << "_" << utc_tm.tm_hour << utc_tm.tm_min
                << utc_tm.tm_sec << "_";

        std::chrono::duration<double> subSec = now - std::chrono::system_clock::from_time_t(tt);
        int usec = subSec.count() * 1000000;
        dateStr << std::setw(6) << usec;


        auto name = parameters.streams.front().sourceIp;
        name.append(":");
        name.append(std::to_string(parameters.streams.front().sourcePort));
        name.append("_to_");
        name.append(parameters.streams.back().sourceIp);
        name.append(":");
        name.append(std::to_string(parameters.streams.back().sourcePort));
        name.append("_at_");
        name.append(dateStr.str());

        std::cout << " name : " << name << std::endl;
        parameters.name = name,
                parameters.duration = m_duration;

        _callLogger->LogCall(parameters);
    }

    DRLinkCall::DRLinkCall(std::vector <IpPort> &dst_inf, unsigned int src_ip, const Call::Options &options) : Call(
            options.duration, options.callLogger) {
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
        for (std::vector<IpPort>::iterator it = dst_inf.begin(); it != dst_inf.end(); ++it) {
            unsigned int dst_ip = it->m_ipv4;
            unsigned short int dst_port = it->m_port;

            unsigned int timestamp = uint_distribution(generator);
            unsigned int ssrc = uint_distribution(generator);
            unsigned short int seq_num = usint_distribution(generator);

            auto call_leg = std::make_unique<CallLeg>(src_ip, src_port, dst_ip, dst_port, id, timestamp, ssrc, seq_num,
                                                      options.encoder_factory_ptr, options.generator_factory_ptr,
                                                      options.consumer);

            m_call_leg_ptr_vector.push_back(std::move(call_leg));

            src_port += 2;
            id += id_offset;
        }

        Call::Log();
    }

    std::unique_ptr <Call> DRLinkCallFactory::CreateCall(const Call::Options &options) {
        return std::make_unique<DRLinkCall>(m_dst_inf, m_src_ip++, options);
    }

    MirrorCall::MirrorCall(unsigned int src_ip, unsigned int dst_ip, const Call::Options &options) : Call(
            options.duration, options.callLogger) {
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

        auto src_call_leg = std::make_unique<CallLeg>(src_ip, src_port, dst_ip, dst_port, id, timestamp, ssrc, seq_num,
                                                      options.encoder_factory_ptr, options.generator_factory_ptr,
                                                      options.consumer);
        m_call_leg_ptr_vector.push_back(std::move(src_call_leg));

        id += id_offset;
        timestamp = uint_distribution(generator);
        ssrc = uint_distribution(generator);
        seq_num = usint_distribution(generator);

        auto dst_call_leg = std::make_unique<CallLeg>(dst_ip, dst_port, src_ip, src_port, id, timestamp, ssrc, seq_num,
                                                      options.encoder_factory_ptr, options.generator_factory_ptr,
                                                      options.consumer);
        m_call_leg_ptr_vector.push_back(std::move(dst_call_leg));

        Call::Log();
    }

    std::unique_ptr <Call> MirrorCallFactory::CreateCall(const Call::Options &options) {
        unsigned int src_ip = m_ip_pool++;
        unsigned int dst_ip = m_ip_pool++;
        return std::make_unique<MirrorCall>(src_ip, dst_ip, options);
    }

    std::unique_ptr <ICallFactory> CallFactoryFactory::CreateCallFactory(const Options &options) {
        if (options.traffic == ddgen::Traffic::DrLink) {
            return std::make_unique<ddgen::DRLinkCallFactory>(options.drlinkIpPortVector, options.startIp);
        } else {
            return std::make_unique<ddgen::MirrorCallFactory>(options.startIp);
        }
    }

}
