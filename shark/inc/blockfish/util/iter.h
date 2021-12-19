// 2021 iitalics
#pragma once

#define BF_DEFINE_RANGE_ITERATOR(_Range, _Enum, _IndexType, _FIRST, _LAST)              \
    class _Range {                                                                      \
     public:                                                                            \
        class iterator {                                                                \
         public:                                                                        \
            inline explicit iterator(_Enum x) : i_(static_cast<_IndexType>(x)) {}       \
            inline iterator& operator++() { i_++; return *this; }                       \
            inline _Enum operator*() const { return static_cast<_Enum>(i_); }           \
            inline bool operator!=(const iterator& rhs) const { return i_<=rhs.i_; }    \
         private:                                                                       \
            _IndexType i_;                                                              \
        };                                                                              \
     public:                                                                            \
        inline _Range() {}                                                              \
        inline iterator begin() const { return iterator(_Enum::_FIRST); }               \
        inline iterator end() const { return iterator(_Enum::_LAST); }                  \
    }

namespace blockfish {
namespace util {

/**
 * Helper for pair of begin/end iterators, so you can use `for(auto x : c)` syntax.
 */
template <typename Iterator>
struct iterator_range
{
 public:
    using iterator = Iterator;

 public:
    inline iterator_range(iterator b, iterator e) : begin_(b), end_(e) {}
    inline iterator begin() const { return begin_; }
    inline iterator end() const { return end_; }
    inline size_t size() const { return end_ - begin_; }

 private:
    Iterator begin_, end_;
};

}   // util
}   // blockfish
