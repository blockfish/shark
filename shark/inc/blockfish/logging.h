// 2021 iitalics
#pragma once

#include <string>

#include "blockfish/util/fmt.h"

namespace blockfish {
namespace logging {

enum Level {
    ERR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4,
    TRACE = 5,
};

// printer(level, subsystem, message)
using printer_t = void(*)(int, const char*, const char*);

extern const int log_level;
extern const printer_t log_printer;

template <typename... Ts>
inline void print_(int level, const char* subsystem, const char* fmtstr, const Ts&... args)
{
    if (level > log_level) {
        return;
    }
    std::string msg;
    util::format(&msg, fmtstr, args...);
    (log_printer)(level, subsystem, msg.c_str());
}

}   // namespace logging
}   // namespace blockfish

#if defined(BF_LOG_NS) && !defined(BF_LOG_DISABLE_ALL)
# define BF_LOG(_lvl, ...) blockfish::logging::print_((_lvl), BF_LOG_NS, __VA_ARGS__)
#else
# define BF_LOG(...)
#endif

#define BF_LOG_TRACE(...) BF_LOG(blockfish::logging::TRACE, __VA_ARGS__)
#define BF_LOG_DEBUG(...) BF_LOG(blockfish::logging::DEBUG, __VA_ARGS__)
#define BF_LOG_INFO(...) BF_LOG(blockfish::logging::INFO, __VA_ARGS__)
#define BF_LOG_WARN(...) BF_LOG(blockfish::logging::WARN, __VA_ARGS__)
#define BF_LOG_ERR(...) BF_LOG(blockfish::logging::ERR, __VA_ARGS__)

#ifdef BF_ASSERTIONS
# include <cstdlib>
# define BF_ASSERT(_cond)                                       \
    do { if (!(_cond)) {                                        \
            BF_LOG_ERR("Assert failed (" __FILE__ ":{}): {}",   \
                       __LINE__, # _cond );                     \
            abort();                                            \
        } } while(0)
#else
# define BF_ASSERT(_cond)
#endif
