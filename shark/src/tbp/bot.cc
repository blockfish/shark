// 2021 iitalics
#include "blockfish/tbp/bot.h"
#define BF_LOG_NS "tbp(bot)"

#include "blockfish/logging.h"
#include "blockfish/tbp/io.h"
#include "blockfish/tbp/msg.h"

namespace blockfish {
namespace tbp {

io::~io() {}

bot::bot()
{
    static std::string features[] = {
        "Cat powers",
        "Downstacking",
    };

    info_msg_ = std::make_unique<tx_msg>();
    info_msg_->type = tx_msg::type::INFO;
    info_msg_->info = std::make_unique<tx_msg::info_data>(
        (tx_msg::info_data) {
            .name = "blockfish",
            // TODO(iitalics): VERSION file
            .version = "0.10.1",
            .author = "iitalics",
            .features = features,
        });
}

bot::poll_result bot::poll(io* io)
{
    if (thinker_ != nullptr) {
        thinker_->think(50);
    }

    if (info_msg_ != nullptr) {
        io->send(*info_msg_);
        info_msg_ = nullptr;
    }

    rx_msg rx;
    io->recv(&rx);
    switch (rx.type) {
    case rx_msg::type::QUIT:
    {
        return poll_result::SHUTDOWN;
    }

    case rx_msg::type::RULES:
    {
        // this part of spec is WIP
        thinker_ = nullptr;
        rules_ = std::make_unique<move::ruleset>(move::ruleset::srs());
        tx_msg tx;
        tx.type = tx_msg::type::READY;
        io->send(tx);
        break;
    }

    case rx_msg::type::START:
    {
        if (rules_ == nullptr) {
            tx_msg tx;
            tx.type = tx_msg::type::ERROR;
            tx.error_reason = "cannot start before receiving 'rules' message";
            io->send(tx);
            break;
        }

        matrix_.copy(rx.start->board);
        queue_.copy(rx.start->queue);
        start_calculating_();
        break;
    }

    case rx_msg::type::STOP:
    {
        BF_LOG_DEBUG("stopping");
        thinker_ = nullptr;
        break;
    }

    case rx_msg::type::SUGGEST:
    {
        if (thinker_ == nullptr) {
            tx_msg tx;
            tx.type = tx_msg::type::ERROR;
            tx.error_reason = "tried to suggest, not currently calculating";
            io->send(tx);
            break;
        }

        tx_msg tx;
        tx.type = tx_msg::type::SUGGESTION;
        tx.suggestion = std::make_unique<tbp::tx_msg::suggestion_data>();
        suggest_(&tx.suggestion->moves, &tx.suggestion->stats);
        if (tx.suggestion->moves.empty()) {
            BF_LOG_WARN("suggestion requested before any terminal nodes found");
        }
        io->send(tx);
        break;
    }

    case rx_msg::type::PLAY:
    case rx_msg::type::NEW_PIECE:
    {
        if (thinker_ == nullptr) {
            tx_msg tx;
            tx.type = tx_msg::type::ERROR;
            tx.error_reason = "tried to advance state, not currently calculating";
            io->send(tx);
            break;
        }

        if (rx.type == rx_msg::type::PLAY) {
            move::falling_piece& piece = rx.play->move;
            move::norm_loc loc(rules_.get(), piece);
            play_({piece, loc});
        } else { // (rx.type == rx_msg::type::NEW_PIECE)
            new_piece_(*rx.new_piece);
        }

        start_calculating_();
        break;
    }

    case rx_msg::type::UNSET:
        if (thinker_ == nullptr) {
            // XXX(iitalics): no thoughts, heady empty (allow blocking for next msg).
            return poll_result::PARK;
        }
        break;
    }

    return poll_result::YIELD;
}

void bot::start_calculating_()
{
    BF_LOG_DEBUG("start calculating {}", queue_);
    BF_LOG_TRACE("{}", matrix_);
    thinker_ = std::make_unique<think::engine>(
        rules_.get(),
        weights_,
        matrix_,
        queue_);
}

void bot::play_(const move::move& move)
{
    // XXX(iitalics): if running, thinker_ holds a reference to queue, which gets
    // invalidated by modifying the queue. so it MUST be reset.
    thinker_ = nullptr;
    queue_.play(move.piece.type);
    move.loc.place(&matrix_);
    (void) matrix_.filter();
}

void bot::new_piece_(piece_type type)
{
    // XXX(iitalics): if running, thinker_ holds a reference to queue, which gets
    // invalidated by modifying the queue. so it MUST be reset.
    thinker_ = nullptr;
    queue_.push_back(type);
}

void bot::suggest_(
    std::vector<move::falling_piece>* moves,
    std::vector<think::statistics>* stats) const
{
    BF_ASSERT(thinker_ != nullptr);
    BF_LOG_DEBUG("latest: {}", thinker_->stats());
    BF_LOG_DEBUG("best:   {}", thinker_->best_move_stats());
    stats->push_back(thinker_->stats());
    if (auto move = thinker_->best_move()) {
        moves->push_back(move->piece);
        stats->push_back(thinker_->best_move_stats());
    }
}

}   // namespace tbp
}   // namespace blockfish
