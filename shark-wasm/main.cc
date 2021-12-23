// 2021 iitalics
#include <string>

#define BF_LOG_NS "main(wasm)"
#include "blockfish/logging.h"
#include "blockfish/tbp/io.h"
#include "blockfish/tbp/bot.h"

namespace bf = blockfish;

/* JS glue API / C ABI */

// runtime support
extern "C" void RTS_log(int lvl, const char* sys, const char* msg);
extern "C" int RTS_send(const char* ptr, size_t len);
extern "C" int RTS_recv(char* ptr);

// webworker event loop (re-)entry point
__attribute__((used)) extern "C" int WW_poll(void);

enum RTS_poll {
    RTS_YIELD = 0,
    RTS_PARK = 1,
    RTS_SHUTDOWN = 2,
    RTS_ERROR = -1,
};


/* I/O, event loop */

const int bf::logging::log_level = WARN;
const bf::logging::printer_t bf::logging::log_printer = RTS_log;

namespace {

class tbp_ww : public blockfish::tbp::io {
 public:
    BF_DISALLOW_IMPLICIT_COPY(tbp_ww);
    BF_DISALLOW_MOVE(tbp_ww);

    tbp_ww() {}

    virtual void send(const bf::tbp::tx_msg& msg)
    {
        bf::tbp::to_json_str(msg, &send_);
        RTS_send(send_.data(), send_.size());
        send_.clear();
    }

    virtual void recv(bf::tbp::rx_msg* msg)
    {
        size_t size = RTS_recv(nullptr);
        if (size == 0) {
            return;
        }
        recv_.resize(size);
        RTS_recv(recv_.data());
        if (!bf::tbp::from_json_str(recv_, msg, &recv_err_)) {
            BF_LOG_ERR("error parsing message: {}", recv_err_.c_str());
            msg->clear();
        }
        recv_.clear();
        recv_err_.clear();
    }

 private:
    std::string send_;
    std::string recv_;
    std::string recv_err_;
};

}   // namespace

int WW_poll(void)
{
    static tbp_ww io;
    static bf::tbp::bot bot;

    switch (bot.poll(&io)) {
    case blockfish::tbp::bot::poll_result::YIELD:
        return RTS_YIELD;
    case blockfish::tbp::bot::poll_result::PARK:
        return RTS_PARK;
    case blockfish::tbp::bot::poll_result::SHUTDOWN:
        return RTS_SHUTDOWN;
    }
}
