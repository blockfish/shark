// 2021 iitalics
#pragma once

#include <vector>

#include "blockfish/common.h"
#include "blockfish/util/iter.h"
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

    inline void set_hold(opt_piece_type ty) { hold_ = ty; }
    inline void push_back(piece_type ty) { next_.push_back(ty); }
    void pop(piece_type ty);
    position pos() const;

 private:
    std::vector<piece_type> next_;
    opt_piece_type hold_;
};

/**
 * Represents a position in a `queue`, reachable by placing pieces or swapping the hold
 * slot.
 */
// WARNING: using `position` is subject to iterator-invalidation bugs if you push to a
// queue while you have any old Pos values in scope.
class position {
 public:
    position();

    void fmt(util::fmt_buf* dst) const;

    inline bool empty() const
    { return !slot_ && next_ == end_; }

    inline size_t size() const
    { return (end_ - next_) + (slot_ ? 1 : 0); }

    inline opt_piece_type hold() const
    { return hold_ ? slot_  : top_(); }

    inline opt_piece_type current() const
    { return hold_ ? top_() : slot_; }

    using piece_type_list = util::iterator_range<const piece_type*>;

    inline piece_type_list next() const
    { return piece_type_list(std::min(next_ + 1, end_), end_); }

    void pop();

    // ASSUMPTION: hold() == ty || current() == ty
    inline void pop(piece_type ty)
    {
        if (!(current() && *current() == ty)) {
            hold_ = !hold_;
        }
        pop();
    }

 private:
    friend class queue;
    const piece_type* next_;
    const piece_type* end_;
    opt_piece_type slot_;
    bool hold_;

    opt_piece_type top_() const;
};

}   // queue
}   // blockfish
