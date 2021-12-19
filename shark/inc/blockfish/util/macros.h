#pragma once

#include <stdexcept>

#define BF_DEFINE_STD_HASH(_Type)                                       \
    template <> struct std::hash<_Type> {                               \
        inline size_t operator()(const _Type& what) const noexcept {    \
            return what.hash();                                         \
        }                                                               \
    }

#define BF_DEFINE_COMPARISONS(_Rhs)                                             \
    inline bool operator==(const _Rhs& rhs) const { return compare(rhs) == 0; } \
    inline bool operator<(const _Rhs& rhs) const { return compare(rhs) < 0; }

#define BF_DISALLOW_IMPLICIT_COPY(_Class)       \
    _Class (const _Class&) = delete;            \
    _Class& operator=(const _Class&) = delete

#define BF_DISALLOW_MOVE(_Class)                \
    _Class (_Class&&) = delete;                 \
    _Class& operator=(_Class&&) = delete
