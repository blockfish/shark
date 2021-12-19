// 2021 iitalics
#include "blockfish/think.h"
#define BF_LOG_NS "think"

#include <algorithm>

#include "blockfish/logging.h"

namespace blockfish {
namespace think {

/* statistics */

void statistics::fmt(util::fmt_buf* dst) const
{
    util::format(
        dst,
        "{iter={},expa={},beam={},rating={}}",
        iterations,
        expansions,
        beam_width,
        rating);
}

/* node */

bool node::compare_t::operator()(
    const node* const& lhs,
    const node* const& rhs) const
{
    return lhs->rating < rhs->rating;
}

node::compare_t node::compare;

/* engine */

engine::engine(
    const move::ruleset* rules,
    const heuristic::weights& weights,
    const matrix::bitboard& init_matrix,
    const queue::queue& init_queue)
    : moves_(rules)
    , weights_(weights)
{
    queue_.copy(init_queue);

    root_ = node_alloc_.allocate();
    root_->matrix.copy(init_matrix);
    root_->queue = queue_.pos();
    root_->rating = heuristic::inf_rating;
    best_ = root_;

    stats_.iterations = 0;
    stats_.expansions = 0;
    stats_.beam_width = 1;

    // XXX(iitalics): empty queue gives us a better point of comparison to backed-up
    // evaluations, since we only back up terminal nodes which are typically at the end of
    // the queue.
    stats_.rating = weights_.evaluate(root_->matrix, queue::position());

    current_ = nullptr;
    current_beam_width_ = 1;
    restart_();
}

std::optional<move::move> engine::best_move() const
{
    // XXX(iitalics): find the first non-root node in the sequence since it tells us the
    // first move to make in the sequence.
    const node* iter = best_;
    const node* first_child = iter;
    while (iter->parent != nullptr) {
        first_child = iter;
        iter = iter->parent;
    }
    return first_child->last_move;
}

void engine::think(size_t times)
{
    for (size_t i = 0; i < times; ++i) {
        stats_.iterations++;

        while (!current_) {
            select_();
        }
        expand_();
    }
}

void engine::restart_()
{
    BF_ASSERT(beam_from_.empty());
    BF_ASSERT(beam_to_.empty());

    beam_from_.push_back(root_);

    // FIXME(iitalics): hmmm
    if (current_beam_width_ < 1000000) { 
        current_beam_width_ *= 2;
        BF_LOG_DEBUG("new beam width: {}", current_beam_width_);

        stats_.beam_width = current_beam_width_;
    }
}

void engine::expand_()
{
    if (auto move = moves_.next()) {
        stats_.expansions++;

        auto succ = node_alloc_.allocate();
        succ->parent = current_;
        succ->last_move = *move;

        // compute new matrix
        succ->matrix.copy(current_->matrix);
        move->loc.place(&succ->matrix);
        auto filtered = succ->matrix.filter();

        // compute new queue
        succ->queue = current_->queue;
        succ->queue.pop(move->piece.type);

        // compute new rating
        if (filtered.lowest_y == 0) {
            // XXX(iitalics): special case for clearing bottom row: don't consider the
            // board state, only the number of pieces left in queue.
            succ->is_terminal = true;
            succ->rating = weights_.evaluate(matrix::bitboard(), succ->queue);
        } else {
            succ->is_terminal = succ->queue.empty();
            succ->rating = weights_.evaluate(succ->matrix, succ->queue);
        }

        current_->children.push_back(std::move(succ));
    }

    if (next_child_index_ >= current_->children.size()) {
        current_ = nullptr;
        return;
    }
    
    beam_to_.push_back(current_->children[next_child_index_]);
    next_child_index_++;
}

void engine::select_()
{
    if (beam_from_.empty()) {
        if (beam_to_.empty()) {
            restart_();
        } else {
            prune_();
        }
        BF_ASSERT(!beam_from_.empty());
    }

    current_ = std::move(beam_from_.back());
    beam_from_.pop_back();

    if (current_->is_terminal) {
        back_prop_();
        return;
    }

    moves_.reset(&current_->matrix);
    next_child_index_ = 0;
    if (current_->children.empty()) {
        moves_.start(current_->queue);
    }
}

void engine::prune_()
{
    BF_ASSERT(beam_from_.empty());

    std::sort(beam_to_.begin(), beam_to_.end(), node::compare);
    if (beam_to_.size() > current_beam_width_) {
        beam_to_.resize(current_beam_width_);
    }

    std::swap(beam_from_, beam_to_);
}

void engine::back_prop_()
{
    BF_ASSERT(current_ != nullptr);

    if (node::compare(current_, best_)) {
        BF_LOG_DEBUG("back prop: rating --> {}", current_->rating);
        std::swap(best_, current_);
        best_move_stats_ = stats_;
        best_move_stats_.rating = best_->rating;
    }

    current_ = nullptr;
}

}   // think
}   // blockfish
