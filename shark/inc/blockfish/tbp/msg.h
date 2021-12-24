// 2021 iitalics
#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "blockfish/common.h"
#include "blockfish/matrix.h"
#include "blockfish/move.h"
#include "blockfish/queue.h"
#include "blockfish/think.h"

namespace blockfish {
namespace tbp {

// TODO(iitalics): use this
// enum Orientation {
//     NORTH = 0,
//     EAST = 1,
//     SOUTH = 2,
//     WEST = 3,
// };

/** 
 * Frontend -> Bot message 
 */
struct rx_msg {
    enum class type {
        UNSET = 0,
        RULES,
        START,
        STOP,
        SUGGEST,
        PLAY,
        NEW_PIECE,
        QUIT,
    };

    struct start_data {
        matrix::bitboard board;
        queue::queue queue;
        // UNUSED: combo
        // UNUSED: back to back
    };

    struct play_data {
        move::falling_piece move;
    };

    type type = type::UNSET;
    std::unique_ptr<start_data> start;
    std::unique_ptr<play_data> play;
    opt_piece_type new_piece;

    inline void clear()
    {
        type = type::UNSET;
        start = nullptr;
        play = nullptr;
        new_piece = std::nullopt;
    }

    inline operator bool() const { return type != type::UNSET; }
};

/** 
 * Bot -> Frontend message 
 */
struct tx_msg {
    enum class type {
        UNSET = 0,
        ERROR,
        READY,
        INFO,
        SUGGESTION,
    };

    struct info_data {
        std::string name;
        std::string version;
        std::string author;
        std::span<const std::string> features;
    };

    struct suggestion_data {
        std::vector<move::falling_piece> moves;
        std::vector<think::statistics> stats;
        // XXX(iitalics): stats[i+1] is associated with moves[i]; stats[0] applies to the
        // total running time
    };

    type type = type::UNSET;
    std::unique_ptr<info_data> info;
    std::unique_ptr<suggestion_data> suggestion;
    std::string error_reason;

    inline void clear()
    {
        info = nullptr;
        suggestion = nullptr;
        error_reason.clear();
    }

    inline operator bool() const { return type != type::UNSET; }
};

/* json */

bool from_json_str(std::string str, rx_msg* out, std::string* err);

void to_json_str(const tx_msg& msg, std::string* append_to);

}   // namespace tbp
}   // namespace blockfish
