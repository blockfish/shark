// 2021 iitalics
#include "blockfish/matrix.h"
#define BF_LOG_NS "matrix"

#include <cstdint>
#include <stdexcept>

#include "blockfish/logging.h"

namespace blockfish {
namespace matrix {

void bitboard::fmt(util::fmt_buf* dst) const
{
    util::format(dst, "bitboard{height={}}", height_);
    for (dim i = 0; i < height_; ++i) {
        dim y = height_ - i - 1;
        util::format(dst, "\n{}|", util::leftpad(y, 2));
        for (dim x = 0; x < cols; ++x) {
            util::format(dst, "{}", get(x, y) ? 'x' : '_');
        }
        util::format(dst, "|");
    }
}

bitboard::filter_result bitboard::filter(dim y0)
{
    filter_result res = { .lowest_y = height_, .count = 0, };
    dim dst_y = y0;
    for (dim y = y0; y < height_; ++y) {
        if (rows_[y] == full_row) {
            res.lowest_y = std::min(res.lowest_y, y);
            res.count++;
        } else {
            rows_[dst_y++] = rows_[y];
        }
    }
    height_ = dst_y;
    return res;
}

bitboard::residue_result bitboard::residue() const
{
    BF_LOG_TRACE("bitboard::residue(): height = {}", height_);
    residue_result res = { .y0 = 0, .y1 = height_ };
    auto prev_row = empty_row;
    for (dim y = height_ - 1; y >= 0; --y) {
        auto row = rows_[y];
        auto cover = prev_row & ~row;
        if (cover != 0) {
            res.y0 = ++y;
            while (y < height_ && (cover & rows_[y]) != 0) { ++y; }
            res.y1 = y;
            break;
        }
        prev_row = row;
    }
    return res;
}

namespace {

struct flood_fill {
    bitboard* bbd;
    dim y0, y1;
    bitboard::fill_space_result res;

    inline flood_fill(bitboard* bbd_, dim y0_, dim y1_)
        : bbd(bbd_), y0(y0_), y1(y1_)
    {
        res.lowest_y = y1;
        res.blocks = 0;
    }

    bool fill(dim x, dim y)
    {
        if (bbd->get(x, y)) {
            return false;
        }
        bbd->put(x, y);
        res.blocks++;
        res.lowest_y = std::min(res.lowest_y, y);
        if (x - 1 >= 0)   fill(x - 1, y);
        if (x + 1 < cols) fill(x + 1, y);
        if (y - 1 >= y0)  fill(x, y - 1);
        if (y + 1 < y1)   fill(x, y + 1);
        return true;
    }
};

}   // namespace

bitboard::fill_space_result bitboard::fill_space(dim y0, dim y1)
{
    flood_fill ff(this, y0, y1);
    for (dim y = y0; y < y1; ++y) {
        if (rows_[y] == full_row) { continue; } /* optimization */
        for (dim x = 0; x < cols; ++x) {
            if (ff.fill(x, y)) {
                goto exit;
            }
        }
    }
exit:
    return ff.res;
}

void bitboard::filter_result::fmt(util::fmt_buf* dst) const
{
    util::format(dst, "{lowest_y={},count={}}", lowest_y, count);
}

void bitboard::residue_result::fmt(util::fmt_buf* dst) const
{
    util::format(dst, "{y0={},y1={}}", y0, y1);
}

void bitboard::fill_space_result::fmt(util::fmt_buf* dst) const
{
    util::format(dst, "{lowest_y={},blocks={}}", lowest_y, blocks);
}

}   // namespace matrix
}   // namespace blockfish
