// 2021 iitalics
#include "blockfish/move.h"
#define BF_LOG_NS "move"

#include <memory>
#include <optional>

#include "blockfish/logging.h"
#include "blockfish/matrix.h"
#include "blockfish/queue.h"

namespace blockfish {
namespace move {

/* falling_piece */

falling_piece falling_piece::spawn(const ruleset* rules, piece_type type)
{
    dim x = rules->spawn_x(type);
    dim y = rules->spawn_y(type);
    dim r = 0;
    return {type, x, y, r};
}

size_t falling_piece::hash() const
{
    size_t hsh = static_cast<int8_t>(type);
    hsh = (hsh * 10) + y;
    hsh = (hsh * 10) + x;
    hsh = (hsh * 4) + r;
    return hsh;
}

int falling_piece::compare(const falling_piece& rhs) const
{
    if (type != rhs.type) return static_cast<int8_t>(type) - static_cast<int8_t>(rhs.type);
    if (r != rhs.r) return r - rhs.r;
    if (x != rhs.x) return x - rhs.x;
    /*if (y != rhs.y)*/ return y - rhs.y;
}

void falling_piece::fmt(util::fmt_buf* dst) const
{
    util::format(dst, "falling_piece{{},x={},y={},r={}}", type, x, y, r);
}


/* norm_loc */

norm_loc::norm_loc(const ruleset* rules, const falling_piece& piece)
{
    auto cells = rules->cells(piece.type, piece.r);
    rows_ = cells.rows;
    x_ = piece.x + cells.x_ofs;
    y_ = piece.y + cells.y_ofs;
    w_ = cells.w;
    h_ = cells.h;
}

size_t norm_loc::hash() const
{
    size_t hsh = y_;
    hsh = (hsh * 10) + x_;
    hsh = (hsh * 4) + w_;
    hsh = (hsh * 4) + h_;
    // TODO(iitalics): encorporate .cells_ into hash?
    return hsh;
}

int norm_loc::compare(const norm_loc& rhs) const
{
    if (x_ != rhs.x_) return x_ - rhs.x_;
    if (y_ != rhs.y_) return y_ - rhs.y_;
    /*if (rows_ != rhs.rows_)*/ return (intptr_t) rows_ - (intptr_t) rhs.rows_;

    // XXX(iitalics): pointer comparison is defined behavior right? right?
    // If we can guaruntee that 'rows_' are always subarrays of the same array then it
    // definitely should be defined.
}

void norm_loc::fmt(util::fmt_buf* dst) const
{
    util::format(dst, "norm_loc{x={},y={},cells=[", x_, y_);
    const char* sep = "";
    for (dim y = h_ - 1; y >= 0; --y) {
        util::format(dst, sep);
        for (dim x = 0; x < w_; ++x) {
            util::format(dst, (rows_[y] & (1 << x)) ? "x" : ".");
        }
        sep = "|";
    }
    util::format(dst, "]}");
}

bool norm_loc::test(const matrix::bitboard& matrix) const
{
    if (x_ < 0 || x_ + w_ > matrix::cols) {
        return true;
    }
    for (dim i = 0; i < h_; ++i) {
        if (matrix.test_row(y_ + i, matrix::offset_bits(x_, rows_[i]))) {
            return true;
        }
    }
    return false;
}

void norm_loc::place(matrix::bitboard* matrix) const
{
    for (int16_t i = 0; i < h_; ++i) {
        matrix->put_row(y_ + i, matrix::offset_bits(x_, rows_[i]));
    }
}


/* move */

move sonic_drop(
    const ruleset* rules,
    const matrix::bitboard& mat,
    falling_piece piece)
{
    norm_loc loc(rules, piece); 
    while (!loc.test(mat)) {
        piece.offset(0, -1);
        loc.offset(0, -1);
    }
    piece.offset(0, +1);
    loc.offset(0, +1);
    return {piece, loc};
}

std::optional<move> apply_input(
    const ruleset* rules,
    const matrix::bitboard& matrix,
    falling_piece piece,
    input input)
{
    int16_t dir = 1;

    switch (input) {
    case input::LEFT: dir = -1; /* fallthrough */
    case input::RIGHT: {
        piece.offset(dir, 0);
        norm_loc loc(rules, piece);
        if (loc.test(matrix)) {
            return std::nullopt;
        }
        return (move) {piece, loc};
    }

    case input::CCW: dir = -1; /* fallthrough */
    case input::CW: {
        auto r0 = piece.r;
        piece.rotate(dir);
        auto r1 = piece.r;
        norm_loc loc(rules, piece);
        for (auto kick : rules->kicks(piece.type, r0, r1)) {
            // TODO(iitalics): config kick capabilities
            loc.offset(kick.x_ofs, kick.y_ofs);
            if (!loc.test(matrix)) {
                piece.offset(kick.x_ofs, kick.y_ofs);
                return (move) {piece, loc};
            }
            loc.offset(-kick.x_ofs, -kick.y_ofs);
        }
        return std::nullopt;
    }

    default:
        BF_LOG_ERR("input not handled");
        return std::nullopt;
    }
}

/* moves */

moves::moves(const ruleset* rules)
    : rules_(rules)
    , matrix_(nullptr)
{}

void moves::reset(const matrix::bitboard* matrix)
{
    matrix_ = matrix;
    stack_.clear();
    visited_.clear();
    returned_.clear();
}

void moves::start(const queue::position& queue)
{
    BF_ASSERT(matrix_ != nullptr);
    BF_ASSERT(stack_.empty());
    BF_ASSERT(visited_.empty());
    BF_ASSERT(returned_.empty());

    if (auto type = queue.hold()) {
        // TODO(iitalics): config hold capability
        push_piece_(falling_piece::spawn(rules_, *type));
    }
    if (auto type = queue.current()) {
        push_piece_(falling_piece::spawn(rules_, *type));
    }
}

bool moves::push_piece_(falling_piece piece)
{
    if (!visited_.insert(piece).second) {
        return false;
    }
    stack_.push_back(piece);
    return true;
}

std::optional<move> moves::next()
{
    while (!stack_.empty()) {
        auto piece = stack_.back();
        stack_.pop_back();

        // TODO(iitalics): config input capabilities (particularly, 180)
        const auto all_inputs = { input::LEFT, input::RIGHT, input::CCW, input::CW };

        for (auto input : all_inputs) {
            if (auto succ = apply_input(rules_, *matrix_, piece, input)) {
                // if input is valid, push result of that input
                if (push_piece_(succ->piece)) {
                    // also try soft dropping the piece
                    push_piece_(sonic_drop(rules_, *matrix_, succ->piece).piece);
                }
            }
        }

        // drop the piece, then only return it if it is unique wrt. normalization
        auto drop = sonic_drop(rules_, *matrix_, piece);
        if (!returned_.insert(drop.loc).second) {
            BF_LOG_TRACE("discarded duplicate: {}", drop.loc);
            continue;
        }
        return drop;
    }

    return std::nullopt;
}

}   // namespace move
}   // namespace blockfish
