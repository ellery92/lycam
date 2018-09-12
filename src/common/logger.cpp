#include <Poco/Logger.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/Formatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FileChannel.h>

using namespace Poco;

Logger &logger()
{
    Logger &root = Logger::root();
    if (root.getChannel().isNull()) {
        // AutoPtr<Formatter> pFormatter =
        //     new PatternFormatter("%m-%d-%Y %h:%M:%S [%p] %U:%u : %t");
        AutoPtr<Formatter> pFormatter =
            new PatternFormatter("[%p] %U:%u: %t");

#ifdef _DEBUG
        AutoPtr<Channel> pChannel = new ConsoleChannel;
        root.setLevel("debug");
#else
        AutoPtr<Channel> pChannel = new FileChannel("lyucam.log");
#endif

        AutoPtr<FormattingChannel> pFormatterChannel
            = new FormattingChannel(pFormatter, pChannel);

        root.setChannel(pFormatterChannel);
    }

    return root;
}
