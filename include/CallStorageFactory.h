#pragma once

#include "CallStorage.h"

#include <memory>
#include <string>

namespace ddgen
{
    class CallStorageFactory
    {
    public:
        struct Options {
            bool useS3;
            std::string stackName;
        };

    public:
        static std::shared_ptr<ICallStorage> CreateCallStorage(const Options& options);
    };
}
