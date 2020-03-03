#include "CallLogger.h"

#if defined DBASE  &&  DBASE == DynamoDB

#include <aws/core/client/DefaultRetryStrategy.h>
#include <aws/core/utils/Outcome.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/CreateTableRequest.h>
#include <aws/dynamodb/model/DescribeTableRequest.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>

namespace ddgen
{

DynamoDBCallLogger::DynamoDBCallLogger(const std::string& dbPath, const std::string& stackName) : _keys(stackName), _dbPath(dbPath) 
{
    std::cout << "Construction..." << std::endl;
    _options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
    Aws::InitAPI(_options);

    // snippet-start:[dynamodb.cpp.put_item.code]
    Aws::Client::ClientConfiguration clientConfig;
    if (!_dbPath.empty()) {
        clientConfig.endpointOverride = _dbPath;
    }
    clientConfig.region = Aws::Region::US_EAST_2;
    static const long maxConnections = 100;
    clientConfig.maxConnections = maxConnections;
    static const long maxRetries = 6;
    clientConfig.retryStrategy = std::make_shared<Aws::Client::DefaultRetryStrategy>(maxRetries);
    static const long httpRequestTimeoutInMs = 1000;
    clientConfig.httpRequestTimeoutMs = httpRequestTimeoutInMs;
    clientConfig.requestTimeoutMs = httpRequestTimeoutInMs;
    _dynamoClient = std::make_unique<Aws::DynamoDB::DynamoDBClient>(clientConfig);

    _checkTable();
}

DynamoDBCallLogger::~DynamoDBCallLogger()
{
    Aws::ShutdownAPI(_options);
}

void DynamoDBCallLogger::LogCall(const CallParameters& parameters)
{
    Aws::DynamoDB::Model::PutItemRequest pir;
    pir.SetTableName(_keys.table);

    Aws::DynamoDB::Model::AttributeValue av;
    av.SetS(parameters.name);
    pir.AddItem(_keys.name, av);

    Aws::DynamoDB::Model::AttributeValue duration;
    duration.SetN(std::to_string(parameters.duration));
    pir.AddItem(_keys.duration, duration);

    if ( !parameters.streams.empty() ) {

        Aws::Vector< std::shared_ptr< Aws::DynamoDB::Model::AttributeValue >> streamList;
        for ( const auto& p : parameters.streams ) {
            auto stream = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
            auto sourceIp = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
            sourceIp->SetS(p.sourceIp);
            stream->AddMEntry("SourceIp", sourceIp);

            auto sourcePort = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
            sourcePort->SetN(std::to_string(p.sourcePort));
            stream->AddMEntry("SourcePort", sourcePort);

            auto destinationIp = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
            destinationIp->SetS(p.destinationIp);
            stream->AddMEntry("DestinationIp", destinationIp);

            auto destinationPort = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
            destinationPort->SetN(std::to_string(p.destinationPort));
            stream->AddMEntry("DestinationPort", destinationPort);

            auto ssrc = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
            ssrc->SetN(std::to_string(p.ssrc));
            stream->AddMEntry("Ssrc", ssrc);

            auto timestamp = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
            timestamp->SetN(std::to_string(p.timestamp));
            stream->AddMEntry("Timestamp", timestamp);

            auto payload = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
            payload->SetN(std::to_string(p.payload));
            stream->AddMEntry("Payload", payload);

            auto sequenceNumber = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
            sequenceNumber->SetN(std::to_string(p.sequenceNumber));
            stream->AddMEntry("SequenceNumber", sequenceNumber);

            if ( !p.toneParameters.empty() ) {
                Aws::Vector< std::shared_ptr< Aws::DynamoDB::Model::AttributeValue >> tonesList;
                for ( const auto& t : p.toneParameters ) {
                    auto tone = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
                    auto amplitude = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
                    amplitude->SetN(std::to_string(t.amplitude));
                    tone->AddMEntry("Amplitude", amplitude);

                    auto frequency = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
                    frequency->SetN(std::to_string(t.frequency));
                    tone->AddMEntry("Frequency", frequency);

                    auto phase = std::make_shared<Aws::DynamoDB::Model::AttributeValue>();
                    phase->SetN(std::to_string(t.phase));
                    tone->AddMEntry("Phase", phase);

                    tonesList.push_back(tone);
                }

                auto tonesVal = std::make_shared<Aws::DynamoDB::Model::AttributeValue> () ;
                tonesVal->SetL(tonesList);
                stream->AddMEntry("Tones", tonesVal);

            }

            streamList.push_back(stream);
        }

        Aws::DynamoDB::Model::AttributeValue streamsVal;
        streamsVal.SetL(streamList);

//        std::cout << " stream values  =  " << streamsVal.SerializeAttribute() << std::endl;

        pir.AddItem("Streams", streamsVal);


    }

    _dynamoClient->PutItemAsync(pir,
                                                [name = parameters.name](const Aws::DynamoDB::DynamoDBClient*,
                                                  const Aws::DynamoDB::Model::PutItemRequest&,
                                                  const Aws::DynamoDB::Model::PutItemOutcome& result,
                                                  const std::shared_ptr<const Aws::Client::AsyncCallerContext>&) {
            if (!result.IsSuccess()) {
                std::cout << "Failed to write " << name << " with error: " << result.GetError().GetMessage() << std::endl;
            } else {
                std::cout << "Succedded to write " << name << std::endl;
            }
        });
}

void DynamoDBCallLogger::_checkTable()
{
    if ( false == _doesTableExists() ) {
        _createTable();
    }
}

bool DynamoDBCallLogger::_doesTableExists()
{
    Aws::DynamoDB::Model::DescribeTableRequest dtr;
    dtr.SetTableName(_keys.table);
    const Aws::DynamoDB::Model::DescribeTableOutcome& result = _dynamoClient->DescribeTable(dtr);
    if (result.IsSuccess()) {
        std::cout << "Table exists..." << std::endl;
        const Aws::DynamoDB::Model::TableDescription& td = result.GetResult().GetTable();
        std::cout << "Table name : " << td.GetTableName() << std::endl;
        std::cout << "Status : " << Aws::DynamoDB::Model::TableStatusMapper::GetNameForTableStatus(td.GetTableStatus()) << std::endl;
        std::cout << "Item count : " << td.GetItemCount() << std::endl;
        std::cout << "Size (bytes): " << td.GetTableSizeBytes() << std::endl;
        return true;
    } else {
        const auto errorType = result.GetError().GetErrorType();
        std::cout << "Failed to describe table: " << result.GetError().GetMessage() << " with type " << (int)errorType << std::endl;

        if ( Aws::DynamoDB::DynamoDBErrors::RESOURCE_NOT_FOUND == errorType ) {
            std::cout << "Table does not present"  << std::endl;
        } else if ( Aws::DynamoDB::DynamoDBErrors::NETWORK_CONNECTION == errorType ) {
            std::cout << "Database is probably not reachable, quiting..."  << std::endl;
            exit(0);
        }

        return false;
    }
}

void DynamoDBCallLogger::_createTable()
{
    std::cout << "Trying to create table: " << std::endl;

    Aws::DynamoDB::Model::AttributeDefinition haskKey;
    haskKey.SetAttributeName(_keys.name);
    haskKey.SetAttributeType(Aws::DynamoDB::Model::ScalarAttributeType::S);

    Aws::DynamoDB::Model::CreateTableRequest req;
    req.AddAttributeDefinitions(haskKey);

    Aws::DynamoDB::Model::KeySchemaElement keyscelt;
    keyscelt.WithAttributeName(_keys.name).WithKeyType(Aws::DynamoDB::Model::KeyType::HASH);
    req.AddKeySchema(keyscelt);

    Aws::DynamoDB::Model::ProvisionedThroughput thruput;
    thruput.WithReadCapacityUnits(5).WithWriteCapacityUnits(5);
    req.SetProvisionedThroughput(thruput);
    req.SetTableName(_keys.table);

    const Aws::DynamoDB::Model::CreateTableOutcome& result = _dynamoClient->CreateTable(req);
    if (result.IsSuccess())
    {
        std::cout << "Table \"" << result.GetResult().GetTableDescription().GetTableName() << " created!" << std::endl;
    }
    else
    {
        std::cout << "Failed to create table: " << result.GetError().GetMessage();
    }
}

}

#endif
