#include "ConsumerFactory.h"
#include "CallStorageFactory.h"

#include "consumer.h"

namespace ddgen
{
    std::shared_ptr<IConsumer> ConsumerFactory::CreateConsumer(const Options& options)
    {
        auto callStorage = ddgen::CallStorageFactory::CreateCallStorage( { options.useS3, options.stackName } );

        if (options.output == ddgen::Output::Pcap) {
            return std::make_shared<ddgen::PcapConsumer> (callStorage);
        } else {
            return std::make_shared<ddgen::SocketConsumer> (options.dstIpPortVector);
        }
    }
}
