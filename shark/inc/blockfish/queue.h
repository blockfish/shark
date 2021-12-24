// 2021 iitalics
#pragma once

#include <vector>
#include <span>

#include "blockfish/common.h"
#include "blockfish/util/macros.h"

namespace blockfish {
namespace queue {

/**
 * Represents the queue of upcoming pieces, incl. an extra slot to hold.
 */
class queue {
 public:
    BF_DISALLOW_IMPLICIT_COPY(queue);

    queue();

    void copy(const queue& other);
    void fmt(util::fmt_buf* dst) const;

    position pos() const;

    // WARNING: calling these functions may be subject to iterator-invalidation style bugs
    // if the `position` from a previous call to `pos()` is still in scope.
    void set_hold(opt_piece_type ty);
    void push_back(piece_type ty);
    void play(piece_type ty);

 private:
    std::vector<piece_type> queue_;
    bool did_hold_;
};

using pieces = std::span<const piece_type>;

/**
 * Represents a position in a `queue`, reachable by placing pieces or swapping the hold
 * slot.
 */
struct position {
    pieces next = {};

    // Store an extra slot which may contain the hold piece or the current piece. This
    // representation allows us to make basic queue modification wrt. the hold piece,
    // without actually changing the underlying array.
    // INVARIANT: if next isn't empty then slot isn't empty either.
    opt_piece_type slot = std::nullopt;

    // If true, then the hold piece is located in the slot and the current piece is at the
    // front of `next`. Otherwise, they are swapped. Also note that two queues are
    // "equivalent" (provide access to the same pieces) irrespective of this flag.
    bool slot_is_hold = false;

    void fmt(util::fmt_buf* dst) const;

    inline opt_piece_type hold() const { return slot_is_hold ? slot : front(); }
    inline opt_piece_type current() const { return slot_is_hold ? front() : slot; }
    inline opt_piece_type front() const { return next.empty() ? opt_piece_type() : next[0]; }
    inline bool empty() const { return !slot && next.empty(); }
    inline size_t size() const { return slot ? (next.size() + 1) : 0; }
    void play(piece_type ty);
};

}   // queue
}   // blockfish
