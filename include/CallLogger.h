#pragma once

#if defined DBASE  &&  DBASE == DynamoDB
#include <aws/core/Aws.h>
#else

#endif

#include "CallParameters.h"

#include <memory>
#include <string>
#include <vector>

namespace Aws {
namespace DynamoDB
{
    using DynamoDBClientPtr = std::unique_ptr<class DynamoDBClient>;
}
}

namespace ddgen
{

class ICallLogger
{
public:

public:
   virtual ~ICallLogger() = default;
   virtual void LogCall(const CallParameters& parameters) = 0;
};

class NullCallLogger : public ICallLogger
{
public:
   NullCallLogger() = default;
   virtual ~NullCallLogger() = default;
   void LogCall(const CallParameters& parameters) override {};
};

#if defined DBASE  &&  DBASE == DynamoDB
class DynamoDBCallLogger : public ICallLogger
{
public:
    struct Keys {
        const std::string table;
        const std::string name;
        const std::string duration;
        Keys(const std::string& stackName) : table(stackName + "-calls"), name("Name"), duration("DurationInMs") {}
            
    };

 public:
   DynamoDBCallLogger(const std::string& dbPath, const std::string& stackName);
   virtual ~DynamoDBCallLogger();
   void LogCall(const CallParameters& parameters) override;

private:
    void _checkTable();
    bool _doesTableExists();
    void _createTable();

private:
    const Keys _keys;
    const std::string& _dbPath;
    Aws::DynamoDB::DynamoDBClientPtr _dynamoClient;
    Aws::SDKOptions _options;
};
#endif
}
