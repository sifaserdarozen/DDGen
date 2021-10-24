/**
 * @file
 * @brief call parameters
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

#include <string>
#include <vector>

namespace ddgen {

struct CallParameters
{
    struct StreamParameters
    {
    public:
        struct ToneParameters
        {
            float amplitude; /** < amplitude of tone 0, < m_amplitude < 1 */
            float frequency; /**< frequency in radians of tone */
            float phase;     /**< phase in radians of tone */

            ToneParameters() : amplitude(0), frequency(0), phase(0)
            {
            }
        };

        std::string sourceIp;
        unsigned short int sourcePort;
        std::string destinationIp;
        unsigned short int destinationPort;
        unsigned int payload;
        unsigned long timestamp;
        unsigned long ssrc;
        unsigned short int sequenceNumber;
        std::vector<ToneParameters> toneParameters;
    };

    std::string name;
    unsigned int duration;
    std::vector<StreamParameters> streams;
};

}; // namespace ddgen
