#include "CallStorage.h"

#include <fstream>
#include <iostream>

#if defined STORAGE && STORAGE == S3

#include <aws/core/client/DefaultRetryStrategy.h>
#include <aws/core/utils/Outcome.h>

#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

namespace ddgen {

S3CallStorage::S3CallStorage(const std::string& stackName) : _options(stackName)
{
    std::cout << "Construction of S3 Storage ..." << std::endl;
    _bucketConfiguration.SetLocationConstraint(Aws::S3::Model::BucketLocationConstraint::us_east_2);
    _sdkOptions.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
    Aws::InitAPI(_sdkOptions);

    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = Aws::Region::US_EAST_2;
    static const long maxConnections = 100;
    clientConfig.maxConnections = maxConnections;
    static const long maxRetries = 6;
    clientConfig.retryStrategy = std::make_shared<Aws::Client::DefaultRetryStrategy>(maxRetries);
    static const long httpRequestTimeoutInMs = 60000;
    clientConfig.httpRequestTimeoutMs = httpRequestTimeoutInMs;
    clientConfig.requestTimeoutMs = httpRequestTimeoutInMs;
    _s3Client = std::make_unique<Aws::S3::S3Client>(clientConfig);

    _checkBucket();
}

S3CallStorage::~S3CallStorage()
{
    Aws::ShutdownAPI(_sdkOptions);
}

void S3CallStorage::Store(const std::string& fileName)
{
    std::cout << "Store request for filename: " << fileName << std::endl;

    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket(_options.bucketName);

    request.SetKey(fileName);
    const std::shared_ptr<Aws::IOStream> fileToUpload =
        Aws::MakeShared<Aws::FStream>("SampleAllocationTag", fileName.c_str(), std::ios_base::in | std::ios_base::binary);
    request.SetBody(fileToUpload);

    const auto result = _s3Client->PutObject(request);

    if (!result.IsSuccess()) {
        const auto errorType = result.GetError().GetErrorType();
        std::cout << "Failed to put data to bucket: " << result.GetError().GetMessage() << " with type " << (int)errorType << std::endl;
        exit(0);
    } else {
        std::cout << "Uploading pcap file is successful " << std::endl;
    }
}

void S3CallStorage::_checkBucket()
{
    if (false == _doesBucketExists()) {
        _createBucket();
    }
}

bool S3CallStorage::_doesBucketExists()
{
    const auto result = _s3Client->ListBuckets();

    if (result.IsSuccess()) {
        std::cout << "Your Amazon S3 buckets:" << std::endl;

        Aws::Vector<Aws::S3::Model::Bucket> bucket_list = result.GetResult().GetBuckets();

        for (auto const& bucket : bucket_list) {
            if (_options.bucketName == bucket.GetName()) {
                return true;
            } else {
                std::cout << " a bucket: " << bucket.GetName() << std::endl;
            }
        }
    } else {
        const auto errorType = result.GetError().GetErrorType();
        std::cout << "Failed to get buckets: " << result.GetError().GetMessage() << " with type " << (int)errorType << std::endl;
        exit(0);
    }

    return false;
}

void S3CallStorage::_createBucket()
{
    std::cout << "Trying to create bucket: " << _options.bucketName << std::endl;

    // Set up the request
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(_options.bucketName);
    request.SetCreateBucketConfiguration(_bucketConfiguration);

    auto result = _s3Client->CreateBucket(request);
    if (!result.IsSuccess()) {
        const auto errorType = result.GetError().GetErrorType();
        std::cout << "Failed to create bucket: " << result.GetError().GetMessage() << " with type " << (int)errorType << std::endl;
        exit(0);
    } else {
        std::cout << "Bucket created " << std::endl;
    }
}

} // namespace ddgen

#endif
