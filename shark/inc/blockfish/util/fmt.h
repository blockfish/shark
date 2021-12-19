// 2021 iitalics
#pragma once

#include <string>
#include <cinttypes>

namespace blockfish {
namespace util {

using fmt_buf = std::string;

inline void fmt_1(fmt_buf* dst, char what) { dst->push_back(what); }
inline void fmt_1(fmt_buf* dst, const char* what) { dst->append(what); }
inline void fmt_1(fmt_buf* dst, char* what) { dst->append(what); }

template <typename T>
typename std::enable_if<std::is_integral<T>::value>::type
fmt_1(fmt_buf* dst, const T& what)
{
    char tmp[64];
    sprintf(tmp, "%" PRId64, int64_t(what));
    dst->append(tmp);
}

template <typename T>
typename std::enable_if<!std::is_integral<T>::value>::type
fmt_1(fmt_buf* dst, const T& what)
{
    what.fmt(dst);
}

inline size_t format_split_(const char* spec, const char** tail)
{
    size_t i;
    for (i = 0; spec[i]; ++i) {
        if (spec[i] == '{' && spec[i + 1] == '}') {
            *tail = &spec[i + 2];
            return i;
        }
    }
    return i;
}

inline void format(fmt_buf* dst, const char* spec)
{
    size_t n;
    const char* tail;
    while (spec[(n = format_split_(spec, &tail))]) {
        dst->append(spec, n);
        spec = tail;
    }
    dst->append(spec);
}

template <typename T, typename... Ts>
inline void format(fmt_buf* dst, const char* spec, const T& arg0, const Ts&... args)
{
    size_t n;
    const char* tail;
    if (spec[(n = format_split_(spec, &tail))]) {
        dst->append(spec, n);
        fmt_1(dst, arg0);
        spec = tail;
    }
    format(dst, spec, args...);
}

template <typename T>
struct leftpad_t {
    const T& what;
    size_t len;
    void fmt(fmt_buf* dst) const
    {
        size_t idx = dst->size();
        size_t tgt = idx + len;
        fmt_1(dst, what);
        if (dst->size() < tgt) {
            dst->insert(idx, tgt - dst->size(), ' ');
        }
    }
};

template <typename T>
leftpad_t<T> leftpad(const T& what, size_t len)
{
    return {what, len};
}

}   // util
}   // blockfish
