// 2021 iitalics
#define BF_LOG_NS "main(tbp)"

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <optional>

#define BOOST_ASIO_NO_DYNAMIC_BUFFER_V1
#define BOOST_ASIO_DISABLE_THREADS
#include "boost/asio.hpp"
#include "boost/system/detail/error_code.hpp"

#include "blockfish/logging.h"
#include "blockfish/tbp/bot.h"
#include "blockfish/tbp/io.h"
#include "blockfish/tbp/msg.h"

namespace bf = blockfish;
namespace asio = boost::asio;

const int bf::logging::log_level = INFO;

namespace {

class tbp_asio : public bf::tbp::io {
 public:
    BF_DISALLOW_IMPLICIT_COPY(tbp_asio);
    BF_DISALLOW_MOVE(tbp_asio);

    tbp_asio(asio::io_context* ctx)
        : exec_(ctx->get_executor())
        , stdin_(*ctx)
        , stdout_(*ctx)
    {}

    virtual ~tbp_asio() {}

    virtual void send(const bf::tbp::tx_msg& msg)
    {
        bf::tbp::to_json_str(msg, &wr_buf_);
        wr_buf_.push_back('\n');
    }
    
    virtual void recv(bf::tbp::rx_msg* msg)
    {
        std::swap(*msg, rd_msg_);
        rd_msg_.clear();
    }

    void async_start(bf::tbp::bot* bot)
    {
        stdin_.assign(0);
        stdout_.assign(1);
        bot_ = bot;
        poll_();
    }

 private:
    asio::executor exec_;
    bf::tbp::bot* bot_;
    bool shutdown_ = false;

    void poll_()
    {
        bf::tbp::bot* bot = nullptr;
        std::swap(bot_, bot);

        if (bot) {
            switch (bot->poll(this)) {
            case bf::tbp::bot::poll_result::PARK:
                BF_LOG_TRACE("parking");
                bot_ = bot;
                break;

            case bf::tbp::bot::poll_result::YIELD:
                asio::post(exec_, [this, bot]() {
                    bot_ = bot;
                    poll_();
                });
                break;

            case bf::tbp::bot::poll_result::SHUTDOWN:
                shutdown_ = true;
                break;
            }
        }

        poll_read_();
        poll_write_();
    }

    asio::posix::stream_descriptor stdin_;
    bool rd_pending_ = false;
    bf::tbp::rx_msg rd_msg_;
    std::string rd_buf_;

    void poll_read_()
    {
        if (rd_pending_ || rd_msg_ || shutdown_) {
            if (rd_pending_ && shutdown_) {
                stdin_.cancel();
            }
            return;
        }
        BF_LOG_TRACE("reading...");
        rd_pending_ = true;
        asio::async_read_until(
            stdin_,
            asio::dynamic_buffer(rd_buf_),
            '\n',
            [this](const boost::system::error_code& ec, size_t n) {
                if (ec.failed()) {
                    throw std::runtime_error(ec.message());
                }
                BF_LOG_TRACE("read done ({})", n);

                std::string json_err;
                if (!bf::tbp::from_json_str(rd_buf_.substr(0, n - 1), &rd_msg_, &json_err)) {
                    throw std::runtime_error(json_err);
                }
                rd_buf_.erase(0, n);
                rd_pending_ = false;
                poll_();
            });
    }

    asio::posix::stream_descriptor stdout_;
    std::string wr_pending_;
    std::string wr_buf_;

    void poll_write_()
    {
        if (wr_buf_.empty() || !wr_pending_.empty()) {
            return;
        }
        BF_LOG_TRACE("writing ({})...", wr_buf_.size());
        std::swap(wr_buf_, wr_pending_);
        asio::async_write(
            stdout_,
            asio::buffer(wr_pending_),
            [this](const boost::system::error_code& ec, size_t n) {
                if (ec.failed()) {
                    throw std::runtime_error(ec.message());
                }
                BF_LOG_TRACE("write done", wr_buf_.size());
                BF_ASSERT(n == wr_pending_.size());
                wr_pending_.clear();
                poll_();
            });
    }
};
    
}   // namespace

static void bf_log_printer_stderr(int level, const char* sys, const char* msg)
{
    const char* log_prefix[8];
    log_prefix[bf::logging::ERR] = "ERR";
    log_prefix[bf::logging::WARN] = "WARN";
    log_prefix[bf::logging::INFO] = "INFO";
    log_prefix[bf::logging::DEBUG] = "DEBUG";
    log_prefix[bf::logging::TRACE] = "TRACE";
    fprintf(stderr, "%s [bf::%s] %s\n", log_prefix[level], sys, msg);
}

const bf::logging::printer_t bf::logging::log_printer = &bf_log_printer_stderr;

int main(int argc, char** argv)
{
    asio::io_context io_ctx;

    tbp_asio io(&io_ctx);
    BF_LOG_INFO("Initialized I/O");

    bf::tbp::bot bot;
    BF_LOG_INFO("Initialized TBP bot");

    try {
        io.async_start(&bot);
        io_ctx.run();
    } catch (std::exception& exn) {
        if (bf::logging::log_level < bf::logging::ERR
#ifdef BF_LOG_DISABLE_ALL
            || true
#endif
        ) {
            fprintf(stderr, "Error: %s\n", exn.what());
        } else {
            BF_LOG_ERR("Error: {}", exn.what());
        }
        return 1;
    }

    return 0;
}
