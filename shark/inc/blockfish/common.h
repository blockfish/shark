// 2021 iitalics
#pragma once

#include <cstdint>
#include <optional>

#include "blockfish/util/fmt.h"

namespace blockfish {

// forward declarations

namespace matrix {
class bitboard;
}
namespace move {
class ruleset;
struct falling_piece;
class norm_loc;
struct move;
class moves;
}
namespace queue {
class position;
class queue;
}
namespace think {
struct statistics;
struct node;
class engine;
}


// piece_type

enum class piece_type : int8_t {
    I = 'I',
    J = 'J',
    L = 'L',
    O = 'O',
    S = 'S',
    T = 'T',
    Z = 'Z',
};

inline char piece_name(piece_type ty)
{
    return static_cast<char>(ty);
}

template <>
inline void util::fmt_1<piece_type>(util::fmt_buf* dst, const piece_type& ty)
{
    fmt_1(dst, piece_name(ty));
}

class opt_piece_type {
 public:
    inline opt_piece_type(piece_type x) : val_(static_cast<int8_t>(x)) {}
    inline opt_piece_type(std::nullopt_t null = std::nullopt) : val_(0) {}

    // ASSUMPTION: not null
    inline piece_type operator*() const { return (piece_type) val_; } 

    inline operator bool() const { return val_ != 0; }
    inline void clear() { val_ = 0; }

    inline void fmt(util::fmt_buf* dst) const
    {
        util::fmt_1(dst, (*this) ? piece_name(**this) : '_');
    }

 private:
    int8_t val_;
};


// type aliases

using dim = int16_t;
using rowbits = uint16_t;

}   // namespace blockfish
