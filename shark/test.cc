#define BF_LOG_NS "test"

#include <cstdint>
#include <stdexcept>

#include "blockfish/logging.h"
#include "blockfish/queue.h"
#include "blockfish/util/fmt.h"

using namespace blockfish;
using piece_type::I;
using piece_type::J;
using piece_type::L;
using piece_type::O;
using piece_type::S;
using piece_type::T;
using piece_type::Z;

namespace {

std::string test_name;
int32_t test_oks = 0;

class test_fail_exception : public std::runtime_error {
 public:
    test_fail_exception(const std::string& what)
        : std::runtime_error(what) {}
};

template <typename... Ts>
void test_fail(const char* spec, Ts&&... arg)
{
    std::string msg = test_name + ": test failed: ";
    util::format(&msg, spec, arg...);
    throw test_fail_exception(msg);
}

template <typename T>
void test_fmt(const T& what, std::string expected)
{
    std::string actual;
    util::format(&actual, "{}", what);
    if (actual != expected) {
        test_fail("expected '{}', got '{}'", expected.c_str(), actual.c_str());
    }
    test_oks++;
}

void test_queue()
{
    test_name = "queue";
    queue::queue q;
    q.push_back(L);
    q.push_back(O);
    q.push_back(J);
    q.push_back(S);
    test_fmt(q, "queue{current=L,next=[OJS]}");
    q.set_hold(T);
    test_fmt(q, "queue{hold=T,current=L,next=[OJS]}");
    q.set_hold(std::nullopt);
    test_fmt(q, "queue{current=L,next=[OJS]}");
    q.play(L);
    test_fmt(q, "queue{current=O,next=[JS]}");
    q.set_hold(T);
    q.play(T);
    test_fmt(q, "queue{hold=O,current=J,next=[S]}");
    q.push_back(Z);
    q.set_hold(std::nullopt);
    q.play(S);
    test_fmt(q, "queue{hold=J,current=Z,next=[]}");
    q.play(J);
    q.play(Z);
    q.push_back(O);
    q.push_back(I);
    q.push_back(L);
    q.push_back(S);
    q.push_back(Z);
    auto p = q.pos();
    test_fmt(p, "position{hold=I,current=O,next=[LSZ]}");
    p.play(O);
    test_fmt(p, "position{hold=I,current=L,next=[SZ]}");
    p.play(I);
    test_fmt(p, "position{hold=L,current=S,next=[Z]}");
    p.play(S);
    p.play(L);
    test_fmt(p, "position{hold=Z,next=[]}");
}

void test_all()
{
    test_queue();
}

static void log_printer_stderr(int level, const char* sys, const char* msg)
{
    const char* log_prefix[8];
    log_prefix[logging::ERR] = "ERR";
    log_prefix[logging::WARN] = "WARN";
    log_prefix[logging::INFO] = "INFO";
    log_prefix[logging::DEBUG] = "DEBUG";
    log_prefix[logging::TRACE] = "TRACE";
    fprintf(stderr, "%s [bf::%s] %s\n", log_prefix[level], sys, msg);
}

}   // namespace

const int logging::log_level = TRACE;
const logging::printer_t logging::log_printer = &log_printer_stderr;

int main()
{
    try {
        test_all();
        BF_LOG_INFO("{} tests passed", test_oks);
    } catch (test_fail_exception& exn) {
        BF_LOG_ERR("{}", exn.what());
        return 1;
    }
    return 0;
}
