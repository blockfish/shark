// 2021 iitalics
#include "blockfish/heuristic.h"
#define BF_LOG_NS "heuristic"

#include "blockfish/logging.h"
#include "blockfish/matrix.h"
#include "blockfish/queue.h"

namespace blockfish {
namespace heuristic {

weights::weights()
{
    // default weights
    rows = 50;
    i_deps = 100;
    p_est = 100;
}

namespace {

constexpr uint8_t hardcoded_i_dep_height = 3;
constexpr uint16_t num_pieces_ceil(uint16_t blocks) { return (blocks + 3) / 4; }

// XXX(iitalics): piece cost parameter decides the baseline of how the other parameters
// should be weighed.
constexpr rating base_piece_cost = 100;

/* heuristic: i dependencies */

uint16_t count_i_deps(const matrix::bitboard& mat)
{
    uint16_t count = 0;
    uint8_t height[matrix::cols] = {0};
    for (dim y = mat.height() - 1; y >= 0; --y) {
        for (dim x = 0; x < matrix::cols; ++x) {
            if (height[x] >= hardcoded_i_dep_height) {
                // prevent double-counting the same i-dependency
                continue;
            }
            if (mat.test_well_shape(x, y)) {
                height[x]++;
                if (height[x] >= hardcoded_i_dep_height) {
                    count++;
                }
            } else {
                height[x] = 0;
            }
        }
    }
    return count;
}

/* heuristic: piece estimate */

// XXX(iitalics): takes matrix by mutable ptr because it needs to modify it in order to
// perform flood fill + filtering.
//
// Credit to mystery for this approach to estimating pieces needed to clear a board.
uint32_t mystery_piece_est_inplace(matrix::bitboard* mat)
{
    matrix::bitboard::residue_result r;
    matrix::bitboard::fill_space_result fs;

    uint32_t iterations = 0;
    uint32_t extra_pieces = 0;

    while ((r = mat->residue()).y0 > 0) {
        BF_LOG_TRACE("[{},{}] {}", iterations, extra_pieces, r);
        BF_LOG_TRACE("{}", *mat);

        uint32_t pieces = 0;
        dim filter_y = r.y0;
        while ((fs = mat->fill_space(r.y0, r.y1)).blocks > 0) {
            pieces += num_pieces_ceil(fs.blocks);
            filter_y = std::min(filter_y, fs.lowest_y);
        }
        mat->filter(filter_y);

        iterations++;
        if (pieces > iterations) {
            extra_pieces += pieces - iterations;
        }
    }

    BF_LOG_TRACE("[{},{}] done.", iterations, extra_pieces);
    return iterations + extra_pieces;
}

inline uint32_t mystery_piece_est(const matrix::bitboard& mat)
{
    matrix::bitboard temp;
    temp.copy(mat);
    return mystery_piece_est_inplace(&temp);
}

}   // namespace

rating weights::evaluate(
    const matrix::bitboard& matrix,
    const queue::position& queue) const
{
    rating res = 0;
    res += rating(matrix.height()) * rows;
    res += rating(count_i_deps(matrix)) * i_deps;
    res += rating(mystery_piece_est(matrix)) * p_est;
    res -= rating(queue.size()) * base_piece_cost;
    return res;
}

}   // heuristic
}   // blockfish
