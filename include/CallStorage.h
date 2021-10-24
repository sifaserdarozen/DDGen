#pragma once

#if defined STORAGE && STORAGE == S3
#include <aws/core/Aws.h>
#include <aws/s3/model/CreateBucketRequest.h>
#else

#endif

#include "CallParameters.h"

#include <memory>
#include <string>
#include <vector>

namespace Aws {
namespace S3 {
using S3ClientPtr = std::unique_ptr<class S3Client>;
}
} // namespace Aws

namespace ddgen {

class ICallStorage
{
public:
public:
    virtual ~ICallStorage() = default;
    virtual void Store(const std::string& fileName) = 0;
};

class NullCallStorage : public ICallStorage
{
public:
    NullCallStorage() = default;
    virtual ~NullCallStorage() = default;
    void Store(const std::string& fileName) override{};
};

#if defined STORAGE && STORAGE == S3
class S3CallStorage : public ICallStorage
{
public:
    struct Options
    {
        std::string bucketName;
        Options(const std::string& stackName) : bucketName(stackName + "-pcaps")
        {
        }
    };

public:
    S3CallStorage(const std::string& stackName);
    virtual ~S3CallStorage();
    void Store(const std::string& fileName) override;

private:
    void _checkBucket();
    bool _doesBucketExists();
    void _createBucket();

private:
    const Options _options;
    Aws::S3::S3ClientPtr _s3Client;
    Aws::SDKOptions _sdkOptions;
    Aws::S3::Model::CreateBucketConfiguration _bucketConfiguration;
};
#endif
} // namespace ddgen
