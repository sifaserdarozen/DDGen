/**
 * @file
 * @brief program running options
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

#include "ipport.h"

#include <string>
#include <vector>

namespace ddgen
{
struct ProgramOptions
{
    bool shouldStart;
    unsigned int numberOfCalls;
    unsigned int callDuration;
    unsigned int simulationDuration;
    unsigned int startIp;
    std::vector<IpPort> dstIpPortVector;
    std::vector<IpPort> drlinkIpPortVector;
    Traffic traffic;
    Output output;
    bool shouldUseSecureWebInterface;
    std::string dbPath;
    bool useDb;
    bool useS3;
    std::string stackName;

    ProgramOptions(int argc, char*argv[]);
    virtual ~ProgramOptions() = default;

    void DisplayUsage() const;
};

}
