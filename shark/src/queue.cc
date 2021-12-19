// 2021 iitalics
#include "blockfish/queue.h"
#define BF_LOG_NS "queue"

#include "blockfish/logging.h"

namespace blockfish {
namespace queue {

namespace {

void fmt_queue(
    util::fmt_buf* dst,
    const char* tag,
    opt_piece_type hold,
    opt_piece_type current,
    const piece_type* next_begin,
    const piece_type* next_end)
{
    util::format(dst, "{}{", tag);
    const char* sep = "";
    if (auto c = hold) {
        util::format(dst, "hold={}", *c);
        sep = ",";
    }
    if (auto c = current) {
        util::format(dst, "{}current={}", sep, *c);
        sep = ",";
    }
    util::format(dst, "{}next=[", sep);
    for (auto it = next_begin; it != next_end; ++it) {
        util::format(dst, "{}", *it);
    }
    util::format(dst, "]}");
}

}   // namespace

/* queue */

queue::queue()
{
    next_.reserve(8);
}

void queue::copy(const queue& other)
{
    if (this != &other) {
        next_ = other.next_;
        hold_ = other.hold_;
    }
}

void queue::fmt(util::fmt_buf* dst) const
{
    fmt_queue(
        dst,
        "queue",
        hold_,
        next_.empty() ? std::nullopt : opt_piece_type(next_[0]),
        &next_[1],
        &next_[next_.size()]);
}

void queue::pop(piece_type ty)
{
    if (!next_.empty() && ty == next_[0]) {
        next_.erase(next_.begin());
    } else if (hold_ && ty == *hold_) {
        if (next_.empty()) {
            hold_.clear();
        } else {
            hold_ = next_[0];
            next_.erase(next_.begin());
        }
    } else if (!hold_ && next_.size() >= 2 && ty == next_[1]) {
        hold_ = next_[0];
        next_.erase(next_.begin());
        next_.erase(next_.begin());
    } else {
        BF_LOG_WARN("piece type {} not reachable from queue: {}", ty, *this);
    }
}

position queue::pos() const
{
    position pos;
    pos.next_ = &next_[0];
    pos.end_ = &next_[next_.size()];
    if (hold_) {
        pos.slot_ = *hold_;
        pos.hold_ = true;
    } else {
        pos.slot_ = pos.next_[0];
        pos.next_++;
    }
    return pos;
}

/* position */

position::position()
    : next_(nullptr)
    , end_(nullptr)
    , hold_(false)
{}

void position::fmt(util::fmt_buf* dst) const
{
    fmt_queue(
        dst,
        "position",
        hold(),
        current(),
        next().begin(),
        next().end());
}

void position::pop()
{
    if (next_ == end_) {
        if (!hold_) {
            slot_.clear();
        }
    } else {
        if (!hold_) {
            slot_ = *next_;
            hold_ = true;
        }
        ++next_;
    }
}

opt_piece_type position::top_() const
{
    if (next_ < end_) {
        return *next_;
    } else {
        return std::nullopt;
    }
}

}   // queue
}   // blockfish
