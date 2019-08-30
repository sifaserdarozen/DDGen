/**
 * @file
 * @brief call leg class
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

#include <vector>
#include "rawsocket.h"
#include "encoder.h"
#include "g722encoder.h"
#include "generator.h"
#include "consumer.h"

#define MAX_PCM_DATA_SIZE 8000

/**
 * @brief Class that will encapsulate call leg information.
 */
class CallLegType
{
private:
    RtpHeaderType m_rtp_header;    /**< rtp header */
    UdpHeaderType m_udp_header;    /**< udp header */
    Ipv4HeaderType m_ipv4_header;    /**< ipv4 header */
    EthHeaderType m_eth_header;    /**< ethernet header */
    PseudoIpv4HeaderType m_pseudo_ipv4_header;    /**< pseudo ipv4 header that will be used in header checksum */

    unsigned int m_remaining_time;    /**< remaining time in ms in this call leg */
    unsigned int m_accumulated_step_time;    /**< accumulated step time that is not handled yet */

    EncoderType* m_encoder_ptr;    /**< encoder that will be used in waveform encoding */
    GeneratorType* m_generator_ptr;    /**< waveform generator */
    ConsumerType* m_consumer_ptr;    /**< consumer that will be used to handle packets */

    short int m_pcm_data_ptr[MAX_PCM_DATA_SIZE];    /**< maximum rtp data size */
    LineDataType m_line_data;    /**< line array that will hold raw data to be processed */

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
    CallLegType(unsigned int src_addr,
                unsigned short int src_port,
                unsigned int dst_addr,
                unsigned short int dst_port,
                unsigned short int id,
                unsigned int timestamp,
                unsigned int ssrc,
                unsigned short int seq_num,
                EncoderFactoryType* encoder_factory_ptr,
                GeneratorFactoryType* generator_factory_ptr,
                ConsumerType* consumer_ptr);

    /**
     * @brief Destructor method
     *
     * In destructor dynamically created variables encoder and waveform generator will be freed.
     * @see m_encoder_ptr
     * @see m_generator_ptr
     */
    ~CallLegType();

    void Step(unsigned short int step_durtion);    /**< Make a step in simulation */
};

/**
 * @brief Abstract class that will encapsulate call information.
 */
class CallType
{
private:

public:
    /**Constructor method
     *
     * In constructor
     */
    CallType() {}

    /**
     * @brief Destructor method
     *
     * In destructor dynamically created variables will be freed.
     */
    virtual ~CallType() {}

    /**
     * @brief Abstract function to step simulation
     *
     * @param step_duration INPUT duration to simulate a call
     * @return success of operation
     */
    virtual bool Step(unsigned int step_duration) = 0;
};

/**
 * @brief class that will implement drlink call information.
 */
class DRLinkCallType : public CallType
{
private:
    std::vector<CallLegType*> m_call_leg_ptr_vector;
    unsigned int m_duration;

public:
    /**Constructor method
     *
     * In constructor
     */
    DRLinkCallType(std::vector<IpPort>& dst_inf, unsigned int src_ip, unsigned int duration,
                    EncoderFactoryType* encoder_factory_ptr,
                    GeneratorFactoryType* generator_factory_ptr,
                    ConsumerType* consumer_ptr);

    /**
     * @brief Destructor method
     *
     * In destructor dynamically created variables will be freed.
     */
    ~DRLinkCallType();

    bool Step(unsigned int step_duration);
};

/**
 * @brief class that will implement mirror call information.
 */
class MirrorCallType : public CallType
{
private:
    std::vector<CallLegType*> m_call_leg_ptr_vector;
    unsigned int m_duration;

public:
    /**Constructor method
     *
     * In constructor
     */
    MirrorCallType(unsigned int src_ip, unsigned int dst_ip, unsigned int duration,
                    EncoderFactoryType* encoder_factory_ptr,
                    GeneratorFactoryType* generator_factory_ptr,
                    ConsumerType* consumer_ptr);

    /**
     * @brief Destructor method
     *
     * In destructor dynamically created variables will be freed.
     */
    ~MirrorCallType();

    bool Step(unsigned int step_duration);
};


/**
 * @brief Abstract call factory interface
 *
 * Call factory interface
 * @see DrLinkCallFactoryType()
 * @see MirrorCallFactoryType()
 */
class CallFactoryType
{
private:

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    CallFactoryType() {}

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~CallFactoryType() {}

    /**
     * @brief pure virtual interface for call creating
     *
     * @return Created call
     */
    virtual CallType* CreateCall(unsigned int duration,
                    EncoderFactoryType* encoder_factory_ptr,
                    GeneratorFactoryType* generator_factory_ptr,
                    ConsumerType* consumer_ptr) = 0;
};

class DRLinkCallFactoryType : public CallFactoryType
{
private:
    std::vector<IpPort> m_dst_inf;
    unsigned int m_src_ip;

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    DRLinkCallFactoryType(std::vector<IpPort>& dst_inf)
    {
        m_dst_inf = dst_inf;
        m_src_ip = 0xac186536;
    }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~DRLinkCallFactoryType() {}

    /**
     * @brief Implementation of drlink call creation
     *
     * @return SingleToneGeneratorType is created and returned to calling object
     * @see CallType()
     * @see DrLinkCallType()
     * @see MirrorCallType()
     */
    virtual CallType* CreateCall(unsigned int duration,
                    EncoderFactoryType* encoder_factory_ptr,
                    GeneratorFactoryType* generator_factory_ptr,
                    ConsumerType* consumer_ptr)
    {
        CallType* call_ptr = new DRLinkCallType(m_dst_inf, m_src_ip, duration, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
        m_src_ip ++;
        return  call_ptr;
    }
};

class MirrorCallFactoryType : public CallFactoryType
{
private:
    unsigned int m_ip_pool;
public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    MirrorCallFactoryType()
    {
        std::cout << __FILE__ << " " << __LINE__ << "in constructor of mirror call factory" << std::endl;
        m_ip_pool = 0xac186536;
    }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~MirrorCallFactoryType() {}

    /**
     * @brief Implementation of sinusoidal generator creation
     *
     * @return SinusoidalGeneratorType is created and returned to calling object
     * @see GeneratorType()
     * @see ZeroGeneratorType()
     * @see SingleToneGeneratorType()
     */
    virtual CallType* CreateCall(unsigned int duration,
                    EncoderFactoryType* encoder_factory_ptr,
                    GeneratorFactoryType* generator_factory_ptr,
                    ConsumerType* consumer_ptr);
};
