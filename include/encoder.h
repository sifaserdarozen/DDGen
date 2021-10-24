/**
 * @file
 * @brief encoder and corresponding factory's
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

namespace ddgen {

#define G711_PACKET_SIZE 160       /**< G711 packet size is 160 samples, 20ms data at 8kHz sampling */
#define G722_PACKET_SIZE 320       /**< G722 packet size is 320 sapmles, 20ms data at 16kHz sampling */
#define PACKET_DURATION 20         /**< Default packet duration is 20ms */
#define G711a_RTP_PAYLOAD_TYPE 0x8 //*< G711a rtp payload type */
#define G711u_RTP_PAYLOAD_TYPE 0x0 //*< G711u rtp payload type */
#define G722_RTP_PAYLOAD_TYPE 0x9  //*< G722 rtp payload type */

/**
 * @brief Abstract encoder interface
 *
 * Encoder interface
 * @see G711aEncoderType()
 * @see G711uEncoderType()
 */
class EncoderType
{
private:
public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    EncoderType()
    {
    }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~EncoderType()
    {
    }

    /**
     * @brief Pure virtual interface for encoding
     *
     * Calling method should supply at least packet size of input data, and make output buffer contain at least that much of space
     * @param pcm_data_ptr INPUT pointer to input pcm data that will be encoded, should contain GetPacaketSize() of data
     * @param encoded_data_ptr OUTPUT pointer to hold encoded output, should point to GetPacketSize() of data
     * @return indicates success of encoding
     * @see GetPacketSize()
     */
    virtual bool Encode(const short int* pcm_data_ptr, unsigned char* encoded_data_ptr) = 0;

    /**
     * @brief Pure virtual interface for getting rtp payload
     *
     * @return rtp payload type of encoder
     */
    virtual unsigned char GetRtpPayload() const = 0;

    /**
     * @brief Pure virtual interface for obtaining packet size
     *
     * @return default packet size of encoder
     */
    virtual unsigned short int GetPacketSize() const = 0;

    /**
     * @brief Default interface for obtaining packet duration
     *
     * Packet duration corresponds to ms span that default packet size holds.
     * @return default packet duration
     * @see GetPacketSize()
     */
    virtual unsigned short int GetPacketDuration() const
    {
        return PACKET_DURATION;
    }
};

/**
 * @brief G711aEncoder realization
 *
 * Encoder interface
 * @see EncoderType()
 * @see G711uEncoderType()
 */
class G711aEncoderType : public EncoderType
{
private:
public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    G711aEncoderType()
    {
    }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~G711aEncoderType()
    {
    }

    /**
     * @brief g711u encoding implementation
     *
     * Logarithmic g711u encoding used in Europe takes 13 bit signed pcm data to form 8 bit logarithmic encoded data.
     * a law encoding allows more resolution at lower signal levels with being computational more efficient than u law.
     * Calling method should supply at least packet size of input data, and make output buffer contain at least that much of space.
     * @param pcm_data_ptr INPUT pointer to input pcm data that will be encoded, should contain GetPacaketSize() of data
     * @param encoded_data_ptr OUTPUT pointer to hold encoded output, should point to GetPacketSize() of data
     * @return indicates success of encoding
     * @see GetPacketSize()
     */
    virtual bool Encode(const short int* pcm_data_ptr, unsigned char* encoded_data_ptr);

    /**
     * @brief implementation for getting rtp payload
     *
     * @return rtp payload type of encoder
     */
    virtual unsigned char GetRtpPayload() const
    {
        return G711a_RTP_PAYLOAD_TYPE;
    }

    /**
     * @brief Implementation for obtaining packet size
     *
     * Default packet size of G711aEncoderType is G711_PACKET_SIZE samples.
     * @return packet size of encoder
     */
    virtual unsigned short int GetPacketSize() const
    {
        return G711_PACKET_SIZE;
    }
};

/**
 * @brief G711uEncoder realization
 *
 * Encoder interface
 * @see EncoderType()
 * @see G711aEncoderType()
 */
class G711uEncoderType : public EncoderType
{
private:
public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    G711uEncoderType()
    {
    }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~G711uEncoderType()
    {
    }

    /**
     * @brief g711u encoding implementation
     *
     * Logarithmic g711u encoding used in America and Japan takes 14 bit signed pcm data to form 8 bit logarithmic encoded data.
     * u law encoding allows more resolution at higher signal ranges.
     * Calling method should supply at least packet size of input data, and make output buffer contain at least that much of space.
     * @param pcm_data_ptr INPUT pointer to input pcm data that will be encoded, should contain GetPacaketSize() of data
     * @param encoded_data_ptr OUTPUT pointer to hold encoded output, should point to GetPacketSize() of data
     * @return indicates success of encoding
     * @see GetPacketSize()
     */
    virtual bool Encode(const short int* pcm_data_ptr, unsigned char* encoded_data_ptr);

    /**
     * @brief implementation for getting rtp payload
     *
     * @return rtp payload type of encoder
     */
    virtual unsigned char GetRtpPayload() const
    {
        return G711u_RTP_PAYLOAD_TYPE;
    }

    /**
     * @brief Implementation for obtaining packet size
     *
     * Default packet size of G711uEncoderType is G711_PACKET_SIZE samples.
     * @return packet size of encoder
     */
    virtual unsigned short int GetPacketSize() const
    {
        return G711_PACKET_SIZE;
    }
};

/**
 * @brief Abstract encoder factory interface
 *
 * Encoder factory interface
 * @see G711aEncoderFactory()
 * @see G711uEncoderFactory()
 */
class EncoderFactory
{
private:
public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    EncoderFactory()
    {
    }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~EncoderFactory()
    {
    }

    /**
     * @brief pure virtual interface for encoder generation
     *
     * @return Generated encoder
     */
    virtual EncoderType* CreateEncoder() const = 0;
};

class G711aEncoderFactory : public EncoderFactory
{
private:
public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    G711aEncoderFactory()
    {
    }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~G711aEncoderFactory()
    {
    }

    /**
     * @brief Implementation of encoder generation
     *
     * @return G711aEncoderType is created and returned to calling object
     * @see EncoderType()
     * @see G711aEncoderType()
     */
    virtual EncoderType* CreateEncoder() const
    {
        return new G711aEncoderType();
    }
};

class G711uEncoderFactory : public EncoderFactory
{
private:
public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    G711uEncoderFactory()
    {
    }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~G711uEncoderFactory()
    {
    }

    /**
     * @brief Implementation of encoder generation
     *
     * @return G711uEncoderType is created and returned to calling object
     * @see EncoderType()
     * @see G711uEncoderType()
     */
    virtual EncoderType* CreateEncoder() const
    {
        return new G711uEncoderType();
    }
};
} // namespace ddgen
