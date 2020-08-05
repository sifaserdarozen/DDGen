/**
 * @file
 * @brief Layer 2 and Layer 3 headers
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

namespace ddgen
{

/**
 * @brief Method that will calculate ones complement 16 bit summation .
 *
 * 16 bit ones complement addition is performed on data_ptr.
 * If data size is not multiple of 2, zero is padded to higher bit of last character.
 * @param data_ptr INPUT data array that summation will be handled on
 * @param data_size INPUT size of data array
 * @return ones complement 16 bit summation result
 */
unsigned short int OnesComplementShortSummation(const unsigned char* data_ptr, unsigned short int data_size);

/**
 * @brief Rtp header
 *
 * Rtp header that encapsulate real time audio/video data.
 * Since operations are minimum and trivial, data is kept in structure to prefer ease of use instead of encapsulation.
 */
struct RtpHeaderType
{
    unsigned char cc:4;    /**< Contributor source identifier count */
    unsigned char x:1;    /**< Extension flag */
    unsigned char p:1;    /**< Padding flag */
    unsigned char version:2;    /**< Version information. */
    unsigned char payload:7;    /**< Payload indicated type of rtp payload */
    unsigned char m:1;    /**< Mark bit. Set to indicate important event such as starting of data after silence suppression period */
    unsigned short int seq_num;    /**< Sequence number of rtp data. Expected to be incremented with each rtp packet*/
    unsigned int timestamp;    /**< Time stamp of header. Expected to be incremented to reflect sampling instant of first sample of each packet.*/
    unsigned int ssrc;    /**< Source identification of rtp data. Should be session wide unique */

    /**
     * @brief Read contents from raw line buffer
     *
     * Network data is read from line buffer and converted into host byte order.
     * Line buffer is expected to be pointing 12 bytes of Rtp header.
     * @param buffer_ptr INPUT line buffer to read from
     * @return success or failure due to buffer read
     * @see WriteToBuffer()
     */
    bool ReadFromBuffer(const unsigned char* buffer_ptr);

    /**
     * @brief Write contents to raw line buffer
     *
     * Structure data is converted into network byte order and written into buffer.
     * Line buffer is expected to be pointing 12 bytes to write into.
     * @param buffer_ptr OUTPUT line buffer to write to
     * @return success or failure due to buffer write
     * @see ReadFromBuffer()
     */
    bool WriteToBuffer(unsigned char* buffer_ptr) const;

    /**
     * @brief overloading equivalence operator
     *
     * Since there is not a dynamic allocated data, std::memcmp is used in comparison
     * @param rhs other RtpHeaderType to compare with
     * @return boolean result of equivalence check
     */
    bool operator ==(const RtpHeaderType& rhs) const;

    /**
     * @brief Display contents of Rtp header
     */
    void Display() const;
};

const unsigned short int rtp_header_size = sizeof(RtpHeaderType);    /**< size of rtp header */

/**
 * @brief Pseudo Ipv4 header that will be used in udp checksum calculation
 *
 * This presudo header will be used in udp checksum calculation. It is included to detect packet routing errors.
 * @see CheckUdpChecksum()
 * @see CalculateUdpChecksum()
 */
struct PseudoIpv4HeaderType
{
    unsigned int src_addr;            /**< source address */
    unsigned int dst_addr;            /**< destination address */
    unsigned char zero_pad;           /**< zero pad */
    unsigned char protocol;           /**< layer 4 protocol */
    unsigned short int data_len;      /**< data length (layer4 packet length) */

    PseudoIpv4HeaderType() : src_addr(0), dst_addr(0), zero_pad(0), protocol(0), data_len(0) {}

    /**
     * @brief Write contents to raw line buffer
     *
     * Structure data is converted into network byte order and written into buffer.
     * Line buffer is expected to be pointing 12 bytes to write into.
     * @param buffer_ptr OUTPUT line buffer to write to
     * @return success or failure due to buffer write
     * @see ReadFromBuffer()
     */
    bool WriteToBuffer(unsigned char* buffer_ptr) const;

    /**
     * @brief Display contents of pseudo Ipv4 header
     */
    void Display() const;
};

const unsigned short int pseudo_ipv4_header_size = sizeof(PseudoIpv4HeaderType);    /**< size of pseudo ipv4 header */

/**
 * @brief IPv4 header deceleration
 *
 * Ip header length is by default 20 bytes. Optional fields will not be used in current version of ddgen.
 * @see CheckIpv4Checksum()
 * @see CalculateIpv4Checksum()
 */
struct Ipv4HeaderType
{
    unsigned char hdr_len:4;        /**< Header length in 32 bits. Minimum is 5 (20bytes) */
    unsigned char version:4;        /**< Version information which is 4 */
    unsigned char service_type;     /**< Type of service field controlling the priority of the packet */
    unsigned short int tot_len;     /**< Total length Ipv4 packet*/
    unsigned short int id;          /**< Sequence identification */
    unsigned short int fragment;    /**< combined fragmentation flag (3bits) and offset information (13 bits) */
    unsigned char ttl;              /**< Time to live field, which is decremented in each hop */
    unsigned char protocol;         /**< Layer 4 protocol indicator. 17 for udp, 6 for tcp */
    unsigned short int checksum;    /**< Header only checksum */
    unsigned int src_addr;          /**< Source address */
    unsigned int dst_addr;          /**< Destination address */

    /**
     * @brief Read contents from raw line buffer
     *
     * Ipv4 header data is read from line buffer and converted into host byte order.
     * Line buffer is expected to be pointing 20 bytes of Ipv4 header.
     * @param buffer_ptr INPUT line buffer to read from
     * @return success or failure due to buffer read
     * @see WriteToBuffer()
     */
    bool ReadFromBuffer(const unsigned char* buffer_ptr);

    /**
     * @brief Update checksum and write contents to raw line buffer
     *
     * Structure data is converted into network byte order and written into buffer.
     * Line buffer is expected to be pointing 20 bytes to write into. Checksum is calculated before operation
     * @param buffer_ptr OUTPUT line buffer to write to
     * @return success or failure due to buffer write
     * @see ReadFromBuffer()
     */
    bool UpdateChecksumWriteToBuffer(unsigned char* buffer_ptr);

    /**
     * @brief Write contents to raw line buffer
     *
     * Structure data is converted into network byte order and written into buffer.
     * Line buffer is expected to be pointing 20 bytes to write into. It is assumed that checksum
     * field is calculated and set accordingly.
     * @param buffer_ptr OUTPUT line buffer to write to
     * @return success or failure due to buffer write
     * @see ReadFromBuffer()
     */
    bool WriteToBuffer(unsigned char* buffer_ptr);

    /**
     * @brief overloading equivalence operator
     *
     * Since there is not a dynamic allocated data, std::memcmp is used in comparison
     * @param rhs other Ipv4HeaderType to compare with
     * @return boolean result of equivalence check
     */
    bool operator ==(const Ipv4HeaderType& rhs) const;

    /**
     * @brief Display contents of Ipv4 header
     */
    void Display() const;
};

const unsigned short int ipv4_header_size = sizeof(Ipv4HeaderType);    /**< size of ipv4 header */

/**
 * @brief Method that will be used to check ipv4 header against errors.
 *
 * 16 bit ones complement addition is performed on ipv4 header to check against 0xffff
 * @param line_ipv4_header_ptr INPUT ipv4 header in network byte order that will be checked against errors
 * @return success or failure of checksum control
 * @see Ipv4HeaderType
 * @see OnesComplementShortSummation()
 */
bool CheckIpv4Checksum(const unsigned char* line_ipv4_header_ptr);

/**
 * @brief Udp header deceleration
 *
 * Udp header length is 8 bytes. Used in connectionless data transfer.
 */
struct UdpHeaderType
{
    unsigned short int src_port;    /**< Source port */
    unsigned short int dst_port;    /**< Destination port */
    unsigned short int tot_len;     /**< Total length of Udp packet */
    unsigned short int checksum;    /**< Checksum of header and data */

    /**
     * @brief Read contents from raw line buffer
     *
     * Udp header data is read from line buffer and converted into host byte order.
     * Line buffer is expected to be pointing 8 bytes of Udp header.
     * @param buffer_ptr INPUT line buffer to read from
     * @return success or failure due to buffer read
     * @see WriteToBuffer()
     */
    bool ReadFromBuffer(const unsigned char* buffer_ptr);

    /**
     * @brief compute checksum and write contents to raw line buffer
     *
     * Structure data is converted into network byte order and written into buffer.
     * Line buffer is expected to be pointing 8 bytes to write into. Checksum is updated internally.
     * @param buffer_ptr OUTPUT line buffer to write to
     * @param udp_data_ptr INPUT udp payload, whose size is assumed to be (tot_len - 8)
     * @param pseudo_ipv4_header INPUT pseudo ip header that will be used in checksum calculation
     * @return success or failure due to buffer write
     * @see PseudoIpv4HeaderType
     * @see ReadFromBuffer()
     */
    bool UpdateChecksumWriteToBuffer(unsigned char* buffer_ptr, const unsigned char* udp_data_ptr, const PseudoIpv4HeaderType& pseudo_ipv4_header);

    /**
     * @brief Write contents to raw line buffer
     *
     * Structure data is converted into network byte order and written into buffer.
     * Line buffer is expected to be pointing 8 bytes to write into. It is assumed that checksum
     * field is calculated and set accordingly.
     * @param buffer_ptr OUTPUT line buffer to write to
     * @return success or failure due to buffer write
     * @see ReadFromBuffer()
     */
    bool WriteToBuffer(unsigned char* buffer_ptr);

    /**
     * @brief overloading equivalence operator
     *
     * Since there is not a dynamic allocated data, std::memcmp is used in comparison
     * @param rhs other UdpHeaderType to compare with
     * @return boolean result of equivalence check
     */
    bool operator ==(const UdpHeaderType& rhs) const;

    /**
     * @brief Display contents of Udp header
     */
    void Display() const;
};

const unsigned short int udp_header_size = sizeof(UdpHeaderType);    /**< size of udp header */

/**
 * @brief Method that will be used to check udp packet against errors.
 *
 * All 16 bits of the pseudo ipv4 header and udp packet are ones complement added
 * and the result is checked against equivalence to 0xffff
 * @param udp_packet_ptr udp header and data
 * @param pseudo_ipv4_header INPUT pseudo ipv4 header that will be used in calculation
 * @retrun success or failure of checksum control
 * @see PseudoIpv4HeaderType
 * @see CalculateUdpChecksum()
 */
bool CheckUdpChecksum(const unsigned char* line_udp_packet_ptr, const PseudoIpv4HeaderType& pseudo_ipv4_header);

/**
 * @brief Ethernet header deceleration
 *
 * Ethernet header length is 14 bytes by default. Currently VLAN is not supported.
 */
struct EthHeaderType
{
    unsigned char dst_mac[6];    /**< Destination mac address */
    unsigned char src_mac[6];    /**< Source mac address */
    unsigned short int eth_type;     /**< Type of payload */

    /**
     * @brief Read contents from raw line buffer
     *
     * Ethernet header data is read from line buffer and converted into host byte order.
     * Line buffer is expected to be pointing 14 bytes of Ethernet header.
     * @param buffer_ptr INPUT line buffer to read from
     * @return success or failure due to buffer read
     * @see WriteToBuffer()
     */
    bool ReadFromBuffer(const unsigned char* buffer_ptr);

    /**
     * @brief Write contents to raw line buffer
     *
     * Structure data is converted into network byte order and written into buffer.
     * Line buffer is expected to be pointing 14 bytes to write into.
     * @param buffer_ptr OUTPUT line buffer to write to
     * @return success or failure due to buffer write
     * @see ReadFromBuffer()
     */
    bool WriteToBuffer(unsigned char* buffer_ptr);

    /**
     * @brief Display contents of Ethernet header
     */
    void Display() const;
};

const unsigned short int eth_header_size = sizeof(EthHeaderType);    /**< size of ethernet header */

#define MAX_LINE_DATA_SIZE 65536    /**< maximum line packet size */

/**
 * @brief Line  data that will be denerated
 *
 * Combined network data of encoded real time audio/video data.
 * Since operations are minimum and trivial, data is kept in structure to prefer ease of use instead of stronger class encapsulation.
 */
struct LineDataType
{
    unsigned char m_line_data[MAX_LINE_DATA_SIZE];    /**< pointer to start of line data */
    unsigned char* m_eth_hdr_ptr;    /**< pointer to start of ethernet header */
    unsigned char* m_ipv4_hdr_ptr;    /**< pointer to start of internet header */
    unsigned char* m_udp_hdr_ptr;    /**< pointer to start of udp header */
    unsigned char* m_rtp_hdr_ptr;    /**< pointer to start of rtp header */
    unsigned char* m_rtp_data_ptr;    /**< pointer to start of rtp data */
    unsigned short int m_rtp_data_size;    /**< rtp data size */

    /**
     * @brief Constructor of line data
     *
     * Network data will be filled into this template.
     * Constructor just fills pointers to their corresponding locations.
     */
    LineDataType()
    {
        m_eth_hdr_ptr = m_line_data;
        m_ipv4_hdr_ptr = m_eth_hdr_ptr + eth_header_size;
        m_udp_hdr_ptr = m_ipv4_hdr_ptr + ipv4_header_size;
        m_rtp_hdr_ptr = m_udp_hdr_ptr + udp_header_size;
        m_rtp_data_ptr = m_rtp_hdr_ptr + rtp_header_size;
        m_rtp_data_size = 0;
    }

    unsigned int LineDataSize() const
    {
        return (eth_header_size + ipv4_header_size + udp_header_size + rtp_header_size + m_rtp_data_size);
    }
};
}
