// 2021 iitalics
#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <queue>

#include "blockfish/heuristic.h"
#include "blockfish/matrix.h"
#include "blockfish/move.h"
#include "blockfish/queue.h"
#include "blockfish/common.h"
#include "blockfish/util/bump.h"
#include "blockfish/util/fmt.h"
#include "blockfish/util/macros.h"

namespace blockfish {
namespace think {

struct statistics {
    uint32_t iterations;
    uint32_t expansions;
    uint32_t beam_width;
    heuristic::rating rating;

    void fmt(util::fmt_buf* dst) const;
};

/**
 * A node discovered by the pathfinding algorithm.
 */
struct node {
    const node* parent = nullptr;
    std::vector<node*> children;
    std::optional<move::move> last_move;

    matrix::bitboard matrix;
    queue::position queue;
    heuristic::rating rating;
    bool is_terminal = false;

    inline node() {}

    BF_DISALLOW_MOVE(node);
    BF_DISALLOW_IMPLICIT_COPY(node);

    struct compare_t {
        bool operator()(
            const node* const& lhs,
            const node* const& rhs) const;
    };

    static compare_t compare;
};

/**
 * The core pathfinding engine driving the AI.
 */
class engine {
 public:
    engine(
        const move::ruleset* rules,
        const heuristic::weights& weights,
        const matrix::bitboard& matrix,
        const queue::queue& queue);

    BF_DISALLOW_IMPLICIT_COPY(engine);
    BF_DISALLOW_MOVE(engine);

    void think(size_t times);

    std::optional<move::move> best_move() const;

    inline const statistics& stats() const { return stats_; }
    inline const statistics& best_move_stats() const { return best_move_stats_; }

 private:
    util::bump_allocator<node> node_alloc_;

    queue::queue queue_;
    node* root_;
    node* best_;
    node* current_ = nullptr;
    size_t next_child_index_;

    move::moves moves_;
    heuristic::weights weights_;

    size_t current_beam_width_;
    std::vector<node*> beam_from_, beam_to_;

    statistics stats_;
    statistics best_move_stats_;

    void restart_();
    void expand_();
    void select_();
    void prune_();
    void back_prop_();
};

}   // think
}   // blockfish
