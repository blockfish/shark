// 2021 iitalics
#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "blockfish/common.h"
#include "blockfish/util/fmt.h"
#include "blockfish/util/iter.h"
#include "blockfish/util/macros.h"

namespace blockfish {
namespace move {

enum class input : uint8_t {
    LEFT,
    RIGHT,
    CCW,
    CW,
};

BF_DEFINE_RANGE_ITERATOR(every_input, input, uint8_t, LEFT, CW);

struct kick {
    dim x_ofs;
    dim y_ofs;

    inline void fmt(util::fmt_buf* dst) const
    { util::format(dst, "kick{{},{}}", x_ofs, y_ofs); }
};

/**
 *
 */
class ruleset {
 public:
    static ruleset srs();

    // FIXME(iitalics): don't hardcode these?
    inline dim spawn_x(piece_type) const { return 4; }
    inline dim spawn_y(piece_type) const { return 19; }

    struct cells_result {
        const rowbits* rows;
        dim x_ofs;
        dim y_ofs;
        dim w;
        dim h;
    };

    inline cells_result cells(piece_type type, dim r) const
    {
        size_t idx = static_cast<int32_t>(type) * 4 + r;
        return cells_[idx];
    }

    using kicks_list = util::iterator_range<const kick*>;

    inline kicks_list kicks(piece_type type, dim r0, dim r1) const
    {
        size_t idx = static_cast<int32_t>(type) * 16 + r0 * 4 + r1;
        return kicks_list(kicks_begin_[idx], kicks_end_[idx]);
    }

 private:
    inline ruleset() {}

    const cells_result* cells_;
    const kick* const* kicks_begin_;
    const kick* const* kicks_end_;
};

/**
 *
 */
struct falling_piece {
    piece_type type;
    dim x, y, r;
    // TODO(iitalics): spin detect

    BF_DEFINE_COMPARISONS(falling_piece);

    static falling_piece spawn(const ruleset* rules, piece_type type);

    size_t hash() const;
    int compare(const falling_piece& rhs) const;
    void fmt(util::fmt_buf* dst) const;

    inline void offset(dim dx, dim dy) { x += dx; y += dy; }
    inline void rotate(dim dr) { r += dr; r &= 3; }
};

/**
 * A "normalized" location; represents the precise squares that a location will occupy on
 * the matrix. Two different locations may normalize to the same result if they result in
 * the same occupied squares, such as certain orientations of Z, S, or I piece.
 */
class norm_loc {
 public:
    BF_DEFINE_COMPARISONS(norm_loc);

    norm_loc(const ruleset* rules, const falling_piece& piece);

    size_t hash() const;
    int compare(const norm_loc& rhs) const;
    void fmt(util::fmt_buf* dst) const;

    inline void offset(dim dx, dim dy) { x_ += dx; y_ += dy; }
    bool test(const matrix::bitboard& mat) const;
    void place(matrix::bitboard* mat) const;

 private:
    const rowbits* rows_;
    dim x_, y_, w_, h_;
};

}   // namespace move
}   // namespace blockfish

BF_DEFINE_STD_HASH(blockfish::move::falling_piece);
BF_DEFINE_STD_HASH(blockfish::move::norm_loc);

namespace blockfish {
namespace move {

struct move {
    falling_piece piece;
    norm_loc loc;
};

move sonic_drop(
    const ruleset* rules,
    const matrix::bitboard& mat,
    falling_piece piece);

std::optional<move> apply_input(
    const ruleset* rules,
    const matrix::bitboard& matrix,
    falling_piece piece,
    input input);

/**
 * Search algorithm for enumerating every valid move for a piece on a given matrix.
 */
class moves {
 public:
    moves(const ruleset* rules);

    BF_DISALLOW_MOVE(moves);
    BF_DISALLOW_IMPLICIT_COPY(moves);

    void reset(const matrix::bitboard* matrix);
    void start(const queue::position& queue);
    std::optional<move> next();

 private:
    const ruleset* rules_;
    const matrix::bitboard* matrix_;
    std::vector<falling_piece> stack_;
    std::unordered_set<falling_piece> visited_;
    std::unordered_set<norm_loc> returned_;

    bool push_piece_(falling_piece spot);
};

}   // namespace move
}   // namespace blockfish
