#pragma once

#include "CallLogger.h"

#include <memory>
#include <string>

namespace ddgen {
class CallLoggerFactory
{
public:
    struct Options
    {
        bool useDb;
        std::string dbPath;
        std::string stackName;
    };

public:
    static std::shared_ptr<ICallLogger> CreateCallLogger(const Options& options);
};
} // namespace ddgen
