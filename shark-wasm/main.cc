// 2021 iitalics
#include <string>
#include <memory>

#define BF_LOG_NS "main(wasm)"
#include "blockfish/logging.h"

// javascript glue API w/ C ABI
extern "C" int RTS_send(const char* ptr, size_t len);
extern "C" int RTS_recv(char* ptr);
extern "C" void RTS_log(int lvl, const char* sys, const char* msg);

// webworker event loop (re-)entry point
__attribute__((used)) extern "C" int WW_poll(void);

enum RTS_poll {
    RTS_YIELD = 0,
    RTS_PARK = 1,
    RTS_SHUTDOWN = 2,
    RTS_ERROR = -1,
};

const int blockfish::logging::log_level = DEBUG;
const blockfish::logging::printer_t blockfish::logging::log_printer = RTS_log;

int WW_poll(void)
{
    BF_LOG_ERR("unimplemented!");
    return RTS_ERROR;
}
