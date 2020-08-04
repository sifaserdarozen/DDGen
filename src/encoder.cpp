#include "encoder.h"

#include <iostream>

namespace ddgen {

bool G711aEncoderType ::Encode(const short int* pcm_data_ptr, unsigned char* encoded_data_ptr)
{
    if (!encoded_data_ptr) {
        std::cerr << __FILE__ << " " << __LINE__ << "encoded_data_ptr is null" << std::endl;
        return false;
    }

    if (!pcm_data_ptr) {
        std::cerr << __FILE__ << " " << __LINE__ << "pcm_data_ptr is null" << std::endl;
        return false;
    }

    for (unsigned int k = 0; k < G711_PACKET_SIZE; k++) {
        unsigned char encoded_data = 0;
        short int pcm_data = *pcm_data_ptr++;
        short int quantization_value = (pcm_data < 0) ? ((~pcm_data) >> 4) : (pcm_data >> 4);

        if (quantization_value > 15) {
            short int quantization_segment = 1;
            while (quantization_value > (16 + 15)) {
                quantization_value >>= 1;
                quantization_segment++;
            }
            quantization_value -= 16;

            encoded_data = quantization_value + (quantization_segment << 4);
        }

        if (pcm_data >= 0)
            encoded_data |= 0x80;

        encoded_data ^= 0x55;

        *encoded_data_ptr++ = encoded_data;
    }

    return true;
}

bool G711uEncoderType ::Encode(const short int* pcm_data_ptr, unsigned char* encoded_data_ptr)
{
    if (!encoded_data_ptr) {
        std::cerr << __FILE__ << " " << __LINE__ << "encoded_data_ptr is null" << std::endl;
        return false;
    }

    if (!pcm_data_ptr) {
        std::cerr << __FILE__ << " " << __LINE__ << "pcm_data_ptr is null" << std::endl;
        return false;
    }

    for (unsigned int k = 0; k < G711_PACKET_SIZE; k++) {
        short int pcm_data = *pcm_data_ptr++;
        short int quantization_value = (pcm_data < 0) ? (((~pcm_data) >> 2) + 33) : ((pcm_data >> 2) + 33);

        if (quantization_value > (0x1FFF)) // clip to 8192
            quantization_value = (0x1FFF);

        short int quantization_segment = 1;
        // Determination of quantization segment
        for (short int i = (quantization_value >> 6); i; i >>= 1)
            quantization_segment++;

        unsigned char encoded_data = (((0x08 - quantization_segment) << 4) | (0x000F - ((quantization_value > quantization_segment) & 0x000F)));

        if (pcm_data >= 0)
            encoded_data |= 0x80;

        *encoded_data_ptr++ = encoded_data;
    }

    return true;
}
} // namespace ddgen
