#pragma once

#include "consumer.h"
#include "ipport.h"

#include <memory>
#include <string>

namespace ddgen {
class ConsumerFactory
{
public:
    struct Options
    {
        Output output;
        std::vector<IpPort> dstIpPortVector;
        bool useS3;
        std::string stackName;
    };

public:
    static std::shared_ptr<IConsumer> CreateConsumer(const Options& options);
};
} // namespace ddgen
