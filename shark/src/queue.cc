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
    pieces next)
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
    for (auto c : next) {
        util::format(dst, "{}", c);
    }
    util::format(dst, "]}");
}

}   // namespace

/* queue */

queue::queue()
    : did_hold_(false)
{
    queue_.reserve(16);
}

void queue::copy(const queue& other)
{
    if (this != &other) {
        queue_ = other.queue_;
        did_hold_ = other.did_hold_;
    }
}

void queue::fmt(util::fmt_buf* dst) const
{
    opt_piece_type hold, current;
    size_t off = 0;
    if (queue_.size() > 1) {
        if (did_hold_) {
            hold = queue_[did_hold_ ? 0 : 1];
            off++;
        }
        current = queue_[did_hold_ ? 1 : 0];
        off++;
    } else if (queue_.size() == 1) {
        (did_hold_ ? hold : current) = queue_[0];
        off++;
    }
    fmt_queue(dst, "queue", hold, current, pieces(queue_).subspan(off));
}

position queue::pos() const
{
    position pos;
    if (!queue_.empty()) {
        pos.next = pieces(queue_).subspan(1);
        pos.slot = queue_[0];
        pos.slot_is_hold = did_hold_;
    }
    return pos;
}

void queue::set_hold(opt_piece_type ty)
{
    if (ty && did_hold_) {
        queue_[0] = *ty;
    } else if (ty && !did_hold_) {
        queue_.insert(queue_.begin(), *ty);
        did_hold_ = true;
    } else if (!ty && did_hold_) {
        queue_.erase(queue_.begin());
        did_hold_ = false;
    }
}

void queue::push_back(piece_type ty)
{
    queue_.push_back(ty);
}

void queue::play(piece_type ty)
{
    BF_ASSERT(!queue_.empty());
    if (queue_.size() > 1 && queue_[0] != ty) {
        std::swap(queue_[0], queue_[1]);
        did_hold_ = true;
    }
    BF_ASSERT(queue_[0] == ty);
    queue_.erase(queue_.begin());
    did_hold_ &= !queue_.empty();
}

/* position */

void position::fmt(util::fmt_buf* dst) const
{
    fmt_queue(dst, "position", hold(), current(), next.subspan(next.empty() ? 0 : 1));
}

void position::play(piece_type ty)
{
    BF_ASSERT(slot); // size() >= 1
    if (next.empty()) {
        BF_ASSERT(*slot == ty);
        slot.clear();
    } else {
        if (*slot == ty) {
            slot = next[0];
        } else {
            BF_ASSERT(next[0] == ty);
        }
        next = next.subspan(1);
        // XXX(iitalics): not sure how to justify this other than the fact that i wrote
        // out a decision tree for this flag after each possiblity of slot==ty and
        // slot_is_hold and it always ends up true after the operation.
        slot_is_hold = true;
    }
}

}   // queue
}   // blockfish
