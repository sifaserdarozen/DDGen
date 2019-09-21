/**
 * @file
 * @brief waveform generator and corresponding factory's
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

#include "ipport.h"
#include "rawsocket.h"

#include <fstream>
#include <netinet/in.h>
#include <vector>

namespace ddgen
{

/** @brief Gets current time in sec and usec.
    @param sec OUTPUT system time seconds part
    @param usec OUTPUT system time in micro seconds part
    @return void.
 */
void GetCurrentTimeInTv(unsigned int& sec, unsigned int& usec);

/**
 * @brief Abstract packet consumer interface
 *
 * Consumer interface
 * @see SocketConsumer()
 * @see PcapConsumer()
 */
class Consumer
{
private:

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    Consumer() {}

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~Consumer() {}

    /**
     * @brief Pure virtual interface for consuming packets
     *
     * @param pcm_data_ptr INPUT pointer to data that will be consumed
     * @param data_size INPUT size, of data that will be consumed
     * @return indicates success of generation
     */
    virtual bool Consume(const unsigned char* data_ptr, unsigned short int data_size) = 0;
};

/**
 * @brief SocketConsumer realization
 *
 * Socket Consumer that will consume packets through socket
 * @see Consumer()
 * @see PcapConsumer()
 */
class SocketConsumer : public Consumer
{
private:
    std::vector<int> m_dst_sock_vector;    /**< destination socket vector */
    std::vector<sockaddr_in> m_dst_sockaddr_vector;    /**< destination socket addr */

public:
    /**
     * @brief Constructor for initializing socket
     *
     *
     */
    SocketConsumer(std::vector<IpPort> & dst_ipport);

    /**
     * @brief destructor, does close socket
     */
    ~SocketConsumer();

     /**
     * @brief Socket consumer for generated packets
     *
     * @param pcm_data_ptr INPUT pointer to data that will be consumed
     * @param data_size INPUT size, of data that will be consumed
     * @return indicates success of generation
     */
    virtual bool Consume(const unsigned char* data_ptr, unsigned short int data_size);
};

/**
 * @brief PcapConsumer realization
 *
 * Pcap Consumer that will consume packets through writing a pcap file
 * @see Consumer()
 * @see SocketConsumer()
 */
class PcapConsumer : public Consumer
{
private:
    /** @brief Types of Pcap File Header.
     *
     * Header information for classical .pcap file
    */
    struct PcapHdrType
    {
        unsigned int magic_number;    /**< An integer that takes major number. */
        unsigned short int version_major;    /**< major version number */
        unsigned short int version_minor;    /**< minor version number */
        unsigned int thiszone;
        unsigned int sigfigs;    /**< flags */
        unsigned int snaplen;
        unsigned int network;
    };

    /** @brief Types of Pcap packet header.
     *
     * Header information for classical .pcap packet
     */
    struct PcapPacHdrType
    {
        unsigned int ts_sec;    /**< packet time in sec */
        unsigned int ts_usec;    /**< packet time in micro sec */
        unsigned int incl_len;    /**< packet capture length */
        unsigned int orig_len;    /**< original packet length */
    };

private:
    std::string m_file_name;    /**< file name for pcap file */
    std::fstream m_file_stream;    /**< file stream for pcap file */
    unsigned int m_file_size;    /**< An integer that shows size of pcap file. */
    const PcapHdrType m_pcap_file_header = {0xa1b2c3d4, 2, 4, 0, 0, 65535, 1};    /**< pcap file haader for .pcap */

    /** @brief Generates file name
		@return Returns 1 if file is generated successfully.
	*/
    void GenerateFileName();

public:
    /**
     * @brief Constructor for initializing pcap file consumer
     *
     * @param file_name INPUT desired filename of pcap file. If already present, overriden
     */
    PcapConsumer(std::string file_name = "");

    /**
     * @brief destructor, does close pcap file
     */
    ~PcapConsumer();

     /**
     * @brief Pcap file consumer for generated packets
     *
     * @param pcm_data_ptr INPUT pointer to data that will be consumed
     * @param data_size INPUT size, of data that will be consumed
     * @return indicates success of generation
     */
    virtual bool Consume(const unsigned char* data_ptr, unsigned short int data_size);
};
}
