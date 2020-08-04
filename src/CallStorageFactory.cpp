#include "CallStorageFactory.h"

#include "CallStorage.h"

#include <iostream>

namespace ddgen {
std::shared_ptr<ICallStorage> CallStorageFactory::CreateCallStorage(const Options& options)
{
#if defined STORAGE && STORAGE == S3
    if (options.useS3) {
        return std::make_shared<S3CallStorage>(options.stackName);
    } else {
        return std::make_shared<NullCallStorage>();
    }
#else
    return std::make_shared<NullCallStorage>();
#endif
}
} // namespace ddgen
