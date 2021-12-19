#include <string>
#include <memory>

// javascript glue API w/ C ABI
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

namespace rts {

void send(const std::string& msg)
{
    if (RTS_send(msg.c_str(), msg.size()) < 0) {
        exit(1);
    }
}

std::string recv()
{
    size_t size = RTS_recv(nullptr);
    std::string msg(size, '\0');
    RTS_recv(msg.data());
    return msg;
}

}   // namespace rts

/* async loop */

class event_loop {
 public:
    event_loop() {}

    int poll()
    {
        int ret = RTS_PARK;

        if (hello < 5) {
            hello++;
            rts::send(
                "{\"type\":\"hello\""
                ",\"count\":" + std::to_string(hello) +
                "}"
            );
            ret = RTS_YIELD;
        }

        auto arg = rts::recv();
        if (!arg.empty()) {
            echo++;
            rts::send(
                "{\"type\":\"echo\""
                ",\"count\":" + std::to_string(echo) +
                ",\"arg\":" + arg +
                "}");
            if (echo > 3) {
                ret = RTS_SHUTDOWN;
            }
        }

        return ret;
    }

 private:
    int hello = 0;
    int echo = 0;
};

static event_loop el;

int WW_poll(void)
{
    return el.poll();
}
