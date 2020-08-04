/**
 * @file
 * @brief call leg class
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

#include "CallLogger.h"
#include "CallParameters.h"
#include "consumer.h"
#include "encoder.h"
#include "generator.h"

#include <memory>
#include <vector>

namespace ddgen {
#define MAX_PCM_DATA_SIZE 8000

/**
 * @brief Class that will encapsulate call leg information.
 */
class CallLeg
{
private:
    RtpHeaderType m_rtp_header;                /**< rtp header */
    UdpHeaderType m_udp_header;                /**< udp header */
    Ipv4HeaderType m_ipv4_header;              /**< ipv4 header */
    EthHeaderType m_eth_header;                /**< ethernet header */
    PseudoIpv4HeaderType m_pseudo_ipv4_header; /**< pseudo ipv4 header that will be used in header checksum */

    unsigned int m_remaining_time;        /**< remaining time in ms in this call leg */
    unsigned int m_accumulated_step_time; /**< accumulated step time that is not handled yet */

    EncoderType* m_encoder_ptr;           /**< encoder that will be used in waveform encoding */
    GeneratorType* m_generator_ptr;       /**< waveform generator */
    std::shared_ptr<IConsumer> _consumer; /**< consumer that will be used to handle packets */

    short int m_pcm_data_ptr[MAX_PCM_DATA_SIZE] = { 0 }; /**< maximum rtp data size */
    LineDataType m_line_data;                            /**< line array that will hold raw data to be processed */

public:
    /**
     * @brief Class that will encapsulate call leg information.
     *
     * Constructor that will set necessary variables forcall leg operation
     * @param src_addr INPUT source ipv4 address
     * @param src_port INPUT source udp port
     * @param dst_addr INPUT destination ipv4 address
     * @param dst_port INPUT destination udp port
     * @param id INPUT ipv4 id
     * @param timestamp INPUT rtp timestamp
     * @param ssrc INPUT rtp source identifiction
     * @param seq_num rtp sequence number
     * @param encoder_factory_ptr INPUT encoder factory that will be used in creating encoder
     * @param generator_factory_ptr INPUT generator factory that will be used in creating waveform generator
     * @param consumer_ptr INPUT consumer that will be used to handle generated packet
     */
    CallLeg(unsigned int src_addr,
            unsigned short int src_port,
            unsigned int dst_addr,
            unsigned short int dst_port,
            unsigned short int id,
            unsigned int timestamp,
            unsigned int ssrc,
            unsigned short int seq_num,
            EncoderFactory* encoder_factory_ptr,
            GeneratorFactory* generator_factory_ptr,
            const std::shared_ptr<IConsumer>& consumer);

    /**
     * @brief Destructor method
     *
     * In destructor dynamically created variables encoder and waveform generator will be freed.
     * @see m_encoder_ptr
     * @see m_generator_ptr
     */
    ~CallLeg();

    void Step(unsigned short int stepDuration); /**< Make a step in simulation */

    CallParameters::StreamParameters GetParameters() const;
};

/**
 * @brief Base class that will encapsulate call information.
 */
class Call
{
protected:
    Call(unsigned int duration, const std::shared_ptr<ICallLogger>& callLogger);
    std::vector<std::unique_ptr<CallLeg>> m_call_leg_ptr_vector;
    unsigned int m_duration;
    const std::shared_ptr<ICallLogger> _callLogger;

public:
    struct Options
    {
        unsigned int duration;
        std::shared_ptr<ICallLogger> callLogger;
        EncoderFactory* encoder_factory_ptr;
        GeneratorFactory* generator_factory_ptr;
        std::shared_ptr<IConsumer> consumer;
    };

public:
    Call() = delete;

    virtual ~Call() = default;

    /**
     * @brief Function to step simulation
     *
     * @param step_duration INPUT duration to simulate a call
     * @return success of operation
     */
    virtual bool Step(unsigned int step_duration);

    virtual void Log();
};

/**
 * @brief class that will implement drlink call information.
 */
class DRLinkCall : public Call
{
public:
    DRLinkCall(std::vector<IpPort>& dst_inf, unsigned int src_ip, const Call::Options& options);

    ~DRLinkCall() = default;
};

/**
 * @brief class that will implement mirror call information.
 */
class MirrorCall : public Call
{
public:
    MirrorCall(unsigned int src_ip, unsigned int dst_ip, const Call::Options& options);

    ~MirrorCall() = default;
};

/**
 * @brief Abstract call factory interface
 *
 * Call factory interface
 * @see DRLinkCallFactory()
 * @see MirrorCallFactory()
 */
class ICallFactory
{
public:
    ICallFactory()
    {
    }

    virtual ~ICallFactory() = default;

    virtual std::unique_ptr<Call> CreateCall(const Call::Options& options) = 0;
};

class DRLinkCallFactory : public ICallFactory
{
private:
    std::vector<IpPort> m_dst_inf;
    unsigned int m_src_ip;

public:
    DRLinkCallFactory(const std::vector<IpPort>& dst_inf, unsigned int start_ip) : m_dst_inf(dst_inf), m_src_ip(start_ip)
    {
    }

    virtual ~DRLinkCallFactory() = default;

    /**
     * @brief Implementation of drlink call creation
     *
     * @return SingleToneGeneratorType is created and returned to calling object
     * @see Call()
     * @see DrLinkCall()
     * @see MirrorCall()
     */
    virtual std::unique_ptr<Call> CreateCall(const Call::Options& options);
};

class MirrorCallFactory : public ICallFactory
{
private:
    unsigned int m_ip_pool;

public:
    explicit MirrorCallFactory(unsigned int start_ip) : m_ip_pool(start_ip)
    {
    }

    virtual ~MirrorCallFactory() = default;

    /**
     * @brief Implementation of sinusoidal generator creation
     *
     * @return SinusoidalGeneratorType is created and returned to calling object
     * @see GeneratorType()
     * @see ZeroGeneratorType()
     * @see SingleToneGeneratorType()
     */
    virtual std::unique_ptr<Call> CreateCall(const Call::Options& options);
};

class CallFactoryFactory
{
public:
    struct Options
    {
        Traffic traffic;
        std::vector<IpPort> drlinkIpPortVector;
        unsigned int startIp;
    };

public:
    static std::unique_ptr<ICallFactory> CreateCallFactory(const Options& options);
};

} // namespace ddgen
