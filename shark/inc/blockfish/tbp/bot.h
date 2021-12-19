// 2021 iitalics
#pragma once

#include <memory>
#include <vector>

#include "blockfish/heuristic.h"
#include "blockfish/matrix.h"
#include "blockfish/move.h"
#include "blockfish/queue.h"
#include "blockfish/think.h"
#include "blockfish/util/macros.h"

namespace blockfish {
namespace tbp {

class io;
class tx_msg;

class bot {
 public:
    bot();

    BF_DISALLOW_MOVE(bot);
    BF_DISALLOW_IMPLICIT_COPY(bot);

    enum class poll_result {
        // Bot is passing control back to IO to check for messages, but still thinking so
        // should be polled again promptly.
        YIELD = 0,
        // Bot is passing control back to IO because it has nothing to do, and does not
        // need to be polled until a message is available.
        PARK,
        // Bot wants to quit.
        SHUTDOWN,
    };

    /**
     * Polls the bot, allowing it to:
     * - send messages to the frontend
     * - respond to any incoming messages
     * - perform thinking work
     */
    poll_result poll(io* io);

 private:
    std::unique_ptr<tx_msg> info_msg_;
    heuristic::weights weights_;
    std::unique_ptr<move::ruleset> rules_ = nullptr;
    matrix::bitboard matrix_;
    queue::queue queue_;
    std::unique_ptr<think::engine> thinker_ = nullptr;

    void start_calculating_();
    void play_(const move::move& move);
    void new_piece_(piece_type type);
    void suggest_(
        std::vector<move::falling_piece>* moves,
        std::vector<think::statistics>* stats) const;
};

}   // namespace tbp
}   // namespace blockfish
