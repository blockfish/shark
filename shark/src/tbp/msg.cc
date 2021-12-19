// 2021 iitalics
#include "blockfish/tbp/msg.h"
#define BF_LOG_NS "tbp::msg"

#include <cstring>
#include <memory>
#include <stdexcept>

#include "blockfish/logging.h"
#include "blockfish/util/bump.h"

#include "tiny-json.h"
#include "json-maker/json-maker.h"

namespace blockfish {
namespace tbp {

namespace {

/* from json */

struct bump_pool {
    jsonPool_t vtable;
    util::bump_allocator<json_t, 512> bump;
    bump_pool();
};

json_t* bump_pool_alloc_from_vtable(jsonPool_t* vt)
{
    bump_pool* pl = json_containerOf(vt, bump_pool, vtable);
    return pl->bump.allocate();
}

bump_pool::bump_pool()
{
    vtable.init = vtable.alloc = &bump_pool_alloc_from_vtable;
}

bool parse_board(const json_t* j, matrix::bitboard* out)
{
    if (!j || j->type != JSON_ARRAY) { return false; }
    dim y = 0;
    for (auto row_j = json_getChild(j); row_j; row_j = json_getSibling(row_j)) {
        if (row_j->type != JSON_ARRAY || y >= matrix::max_rows) {
            return false;
        }
        dim x = 0;
        for (auto cell_j = json_getChild(row_j); cell_j; cell_j = json_getSibling(cell_j)) {
            if (x >= matrix::cols) {
                return false;
            } else if (cell_j->type == JSON_TEXT) {
                out->put(x, y);
            } else if (cell_j->type != JSON_NULL) {
                return false;
            }
            x++;
        }
        if (x != matrix::cols) {
            return false;
        }
        y++;
    }
    return true;
}

bool parse_piece_type(const json_t* j, piece_type* out)
{
    if (!j || j->type != JSON_TEXT) { return false; }
    auto cstr = json_getValue(j);
    if (!cstr[0] || cstr[1]) {
        return false;
    }
    switch (cstr[0]) {
    case 'I': *out = piece_type::I; return true;
    case 'J': *out = piece_type::J; return true;
    case 'L': *out = piece_type::L; return true;
    case 'O': *out = piece_type::O; return true;
    case 'T': *out = piece_type::T; return true;
    case 'S': *out = piece_type::S; return true;
    case 'Z': *out = piece_type::Z; return true;
    default: return false;
    }
}

bool parse_opt_piece_type(const json_t* j, opt_piece_type* out)
{
    if (j && j->type == JSON_NULL) {
        out->clear();
        return true;
    }
    piece_type ty;
    if (!parse_piece_type(j, &ty)) {
        return false;
    }
    *out = ty;
    return true;
}

bool parse_queue(const json_t* j, queue::queue* out)
{
    if (!j || j->type != JSON_ARRAY) { return false; }
    for (auto ty_j = json_getChild(j); ty_j; ty_j = json_getSibling(ty_j)) {
        piece_type ty;
        if (!parse_piece_type(ty_j, &ty)) {
            return false;
        }
        out->push_back(ty);
    }
    return true;
}

bool parse_hold(const json_t* j, queue::queue* out)
{
    opt_piece_type ty;
    if (!parse_opt_piece_type(j, &ty)) {
        return false;
    }
    out->set_hold(ty);
    return true;
}

bool parse_dim(const json_t* j, dim* out)
{
    if (!j || j->type != JSON_INTEGER) { return false; }
    *out = strtol(json_getValue(j), nullptr, 10);
    return true;
}

bool parse_rot(const json_t* j, dim* out)
{
    if (!j || j->type != JSON_TEXT) { return false; }
    auto cstr = json_getValue(j);
    if (strcmp(cstr, "north") == 0) { *out = 0; return true; }
    if (strcmp(cstr, "east") == 0) { *out = 1; return true; }
    if (strcmp(cstr, "south") == 0) { *out = 2; return true; }
    if (strcmp(cstr, "west") == 0) { *out = 3; return true; }
    return false;
}

bool parse_move(const json_t* j, move::falling_piece* out)
{
    if (!j || j->type != JSON_OBJ) { return false; }
    auto loc_j = json_getProperty(j, "location");
    if (!loc_j || loc_j->type != JSON_OBJ) { return false; }
    return parse_piece_type(json_getProperty(loc_j, "type"), &out->type)
        && parse_dim(json_getProperty(loc_j, "x"), &out->x)
        && parse_dim(json_getProperty(loc_j, "y"), &out->y)
        && parse_rot(json_getProperty(loc_j, "orientation"), &out->r);
}

bool parse_rx_msg(const json_t* j, rx_msg* out)
{
    auto ty_j = json_getProperty(j, "type");
    if (!ty_j || ty_j->type != JSON_TEXT) { return false; }
    auto cstr = json_getValue(ty_j);

    if (strcmp(cstr, "rules") == 0) {
        out->type = rx_msg::type::RULES;
        return true;
    }

    if (strcmp(cstr, "start") == 0) {
        out->type = rx_msg::type::START;
        out->start = std::make_unique<rx_msg::start_data>();
        return parse_board(json_getProperty(j, "board"), &out->start->board)
            && parse_queue(json_getProperty(j, "queue"), &out->start->queue)
            && parse_hold(json_getProperty(j, "hold"), &out->start->queue);
    }

    if (strcmp(cstr, "stop") == 0) {
        out->type = rx_msg::type::STOP;
        return true;
    }

    if (strcmp(cstr, "suggest") == 0) {
        out->type = rx_msg::type::SUGGEST;
        return true;
    }

    if (strcmp(cstr, "play") == 0) {
        out->type = rx_msg::type::PLAY;
        out->play = std::make_unique<rx_msg::play_data>();
        return parse_move(json_getProperty(j, "move"), &out->play->move);
    }

    if (strcmp(cstr, "new_piece") == 0) {
        out->type = rx_msg::type::NEW_PIECE;
        return parse_opt_piece_type(json_getProperty(j, "piece"), &out->new_piece);
    }

    if (strcmp(cstr, "quit") == 0) {
        out->type = rx_msg::type::QUIT;
        return true;
    }

    // unknown type is OK, should be ignored
    BF_LOG_DEBUG("unknown type: {}", cstr);
    out->type = rx_msg::type::UNSET;
    return true;
}

/* to json */

inline char* json_string(char* dst, const char* name, const std::string& str, size_t* len)
{ return json_nstr(dst, name, str.data(), str.size(), len); }

char* json_move(char* dst, const char* name, const move::falling_piece& move, size_t* len)
{
    char type[2];
    type[0] = piece_name(move.type);
    type[1] = '\0';

    const char* orientation = "";
    switch (move.r & 3) {
    case 0: orientation = "north"; break;
    case 1: orientation = "east"; break;
    case 2: orientation = "south"; break;
    case 3: orientation = "west"; break;
    }

    dst = json_objOpen(dst, name, len);
    dst = json_objOpen(dst, "location", len);
    dst = json_str(dst, "type", type, len);
    dst = json_str(dst, "orientation", orientation, len);
    dst = json_int(dst, "x", move.x, len);
    dst = json_int(dst, "y", move.y, len);
    dst = json_objClose(dst, len); // location
    dst = json_nstr(dst, "spin", "none", 4, len);
    return json_objClose(dst, len);
}

char* json_tx_msg(char* dst, const char* name, const tx_msg& msg, size_t* len)
{
    dst = json_objOpen(dst, name, len);
    switch (msg.type) {
    case tx_msg::type::ERROR:
        dst = json_str(dst, "type", "error", len);
        dst = json_string(dst, "reason", msg.error_reason, len);
        break;

    case tx_msg::type::READY:
        dst = json_str(dst, "type", "ready", len);
        break;

    case tx_msg::type::INFO:
        dst = json_str(dst, "type", "info", len);
        dst = json_string(dst, "name", msg.info->name, len);
        dst = json_string(dst, "version", msg.info->version, len);
        dst = json_string(dst, "author", msg.info->author, len);
        dst = json_arrOpen(dst, "features", len);
        for (const auto& feat : msg.info->features) {
            dst = json_string(dst, nullptr, feat, len);
        }
        dst = json_arrClose(dst, len); // features
        break;

    case tx_msg::type::SUGGESTION:
        dst = json_str(dst, "type", "suggestion", len);
        dst = json_arrOpen(dst, "moves", len);
        for (const auto& move : msg.suggestion->moves) {
            dst = json_move(dst, nullptr, move, len);
        }
        dst = json_arrClose(dst, len); // moves
        break;

    default:
        BF_ASSERT(msg.type != tx_msg::type::UNSET);
        std::abort();
    }

    return json_objClose(dst, len);
}

}   // namespace

bool from_json_str(std::string str, rx_msg* out, std::string* err)
{
    bump_pool pool;
    const json_t* root = json_createWithPool(str.data(), &pool.vtable);
    if (!root) {
        *err = "invalid JSON syntax";
        return false;
    }

    if (!parse_rx_msg(root, out)) {
        *err = "invalid message";
        return false;
    }

    return true;
}

void to_json_str(const tx_msg& msg, std::string* append_to)
{
    constexpr size_t initial_cap = 128;
    size_t cap = initial_cap;

    // XXX(iitalics): first try to encode into a stack allocated buffer; only if that is
    // not big enough do we allocate buffers on the heap.
    char stack_buf[initial_cap];
    std::unique_ptr<char[]> heap_buf = nullptr;
    char* start = stack_buf;

    for (;;) {
        size_t rem = cap - 1;
        char* end = json_end(json_tx_msg(start, nullptr, msg, &rem), &rem);
        if (rem > 0) {
            append_to->append(start, end);
            return;
        }

        BF_LOG_DEBUG("buffer size {} is too small; expanding", cap);
        cap *= 2;
        heap_buf = std::make_unique<char[]>(cap - 1);
        start = &heap_buf[0];
    }
}

}   // namespace tbp
}   // namespace blockfish

