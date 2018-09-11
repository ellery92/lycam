#include <Poco/Logger.h>
#include <sstream>
extern Poco::Logger &logger();

#define lyu_log(log_level, msg)                 \
    {                                           \
        std::stringstream ss;                   \
        ss << msg;                              \
        log_level(logger(), ss.str());          \
    }

#define lyu_fatal(msg) lyu_log(poco_fatal, msg)
#define lyu_critical(msg) lyu_log(poco_critical, msg)
#define lyu_error(msg) lyu_log(poco_error, msg)
#define lyu_warning(msg) lyu_log(poco_warning, msg)
#define lyu_notice(msg) lyu_log(poco_notice, msg)
#define lyu_info(msg) lyu_log(poco_information, msg)
#define lyu_debug(msg) lyu_log(poco_debug, msg)
