// 2021 iitalics
#pragma once

#include <cstddef>
#include <cstdint>

#include "blockfish/common.h"

namespace blockfish {
namespace heuristic {

using rating = int64_t;

constexpr rating inf_rating = 999999;

struct weights {
    rating rows;
    rating i_deps;
    rating p_est;

    weights();

    rating evaluate(
        const matrix::bitboard& matrix,
        const queue::position& queue) const;
};

}   // heuristic
}   // blockfish
