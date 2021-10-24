#include "CallLoggerFactory.h"

#include "CallLogger.h"

namespace ddgen {
std::shared_ptr<ICallLogger> CallLoggerFactory::CreateCallLogger(const Options& options)
{
#if defined DBASE && DBASE == DynamoDB
    if (options.useDb) {
        return std::make_shared<DynamoDBCallLogger>(options.dbPath, options.stackName);
    } else {
        return std::make_shared<NullCallLogger>();
    }
#else
    return std::make_shared<NullCallLogger>();
#endif
}
} // namespace ddgen
