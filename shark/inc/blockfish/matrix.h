// 2021 iitalics
#pragma once

#include <cstdint>
#include <cstddef>
#include <array>

#include "blockfish/common.h"
#include "blockfish/util/macros.h"

namespace blockfish {
namespace matrix {

constexpr dim cols = 10;
constexpr dim max_rows = 40;

constexpr rowbits empty_row = ~0x7fe;
constexpr rowbits full_row = ~0;

/**
 * Convert relative bits (e.g., from shape data) into absolute bits, which is usable for
 * bitboard::test_row or bitboard::put_row.
 */
inline rowbits offset_bits(dim x, rowbits bits) {
    return bits << (x + 1);
}

class bitboard {
 public:
    BF_DISALLOW_IMPLICIT_COPY(bitboard);

    inline bitboard()
        : height_(0)
    {}

    // inline bitboard(bitboard&& other) {
    //     if (this != &other) {
    //         copy(other);
    //         other.clear();
    //     }
    // }

    void fmt(util::fmt_buf* dst) const;

    inline void copy(const bitboard& other) {
        height_ = other.height_;
        for (dim y = 0; y < height_; ++y) {
            rows_[y] = other.rows_[y];
        }
    }

    inline void clear() {
        height_ = 0;
    }

    inline dim height() const {
        return height_;
    }

    inline bool get(dim x, dim y) const {
        return x < 0 || x >= cols ||
            test_row(y, offset_bits(x, 1));
    }

    inline bool test_row(dim y, rowbits mask) const {
        if (y < 0) {
            return true;
        }
        if (y >= height_) {
            return false;
        }
        return (rows_[y] & mask) != 0;
    }

    /**
     * "....X_X....."
     *       ^-------- test for this shape
     */
    // ASSUMPTION: 0 <= x < cols
    inline bool test_well_shape(dim x, dim y) const {
        return y >= 0 && y < height_ &&
            (rows_[y] & (7 << x)) == (5 << x);
    }

    inline void put(dim x, dim y) {
        if (x >= 0 && x < cols && y >= 0 && y < max_rows) {
            put_row(y, offset_bits(x, 1));
        }
    }

    // ASSUMPTION: 0 <= y < max_rows
    inline void put_row(dim y, rowbits mask) {
        while (height_ <= y) { rows_[height_++] = empty_row; }
        rows_[y] |= mask;
    }

    // return value of filter()
    struct filter_result {
        /** y of the lowest row filtered (equals height() if no rows cleared). */
        dim lowest_y;
        /** number of rows filtered. */
        uint16_t count;

        void fmt(util::fmt_buf* dst) const;
    };

    /**
     * Filters out any full lines in the matrix. Returns the number of filtered lines. If
     * the bottom row is filtered as a result, then the return value is negated.
     */
    // ASSUMPTION: y0 >= 0
    filter_result filter(dim y0 = 0);

    // return value of residue()
    struct residue_result {
        /** y of the bottom of the residue. */
        dim y0;
        /** y above the top of the residue. */
        dim y1;

        void fmt(util::fmt_buf* dst) const;
    };

    /**
     * Determines what subset of the matrix is "residue" by finding the highest gap which
     * has blocks covering it (typically this is the second line of cheese, which makes
     * the first line of cheese part of the residue).
     */
    residue_result residue() const;

    struct fill_space_result {
        /** y of the lowest block filled. */
        dim lowest_y;
        /** number of blocks filled. */
        uint16_t blocks;

        void fmt(util::fmt_buf* dst) const;
    };

    /**
     * Finds and fills in some contiguous region between rows y0 (inclusive) and y1
     * (exclusive) if possible. If the region is entirely full then the return value will
     * have .count == 0.
     */
    // ASSUMPTION: 0 <= y0,y1 < (rows() - 1)
    fill_space_result fill_space(dim y0, dim y1);

 private:
    // TODO(iitalics): add per-column heights? is that useful?

    dim height_;
    std::array<rowbits, static_cast<size_t>(max_rows)> rows_;
};

}   // namespace matrix
}   // namespace blockfish
