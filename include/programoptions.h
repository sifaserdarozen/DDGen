/**
 * @file
 * @brief program running options
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

#include "ipport.h"

#include <vector>

namespace ddgen
{
      
enum class Traffic
{
    DrLink,
    Mirror
};

enum class Output
{
    Pcap,
    Socket
};

struct ProgramOptions
{
    unsigned int number_of_calls;
    unsigned int duration_of_calls;
    unsigned int start_ip;
    std::vector<IpPort> dst_ipport_vector;
    std::vector<IpPort> drlink_ipport_vector;
    Traffic traffic;
    Output output;
    bool shouldUseSecureWebInterface;

    ProgramOptions(int argc, char*argv[]);
    virtual ~ProgramOptions() = default;

    void DisplayUsage() const;
};

}