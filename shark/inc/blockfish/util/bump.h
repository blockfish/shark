#pragma once

#include <memory>
#include <vector>

namespace blockfish {
namespace util {

template <typename T, size_t page_size = 8096>
class bump_allocator {
 public:
    bump_allocator() {}

    T* allocate()
    {
        if (here >= end) {
            pages_.emplace_back(page_size);
            here = pages_.back().start;
            end = pages_.back().end;
        }
        return here++;
    }

 private:
    struct page {
        std::unique_ptr<T[]> buf;
        T* start;
        T* end;

        explicit page(size_t size)
            : buf(new T[size])
            , start(&buf[0])
            , end(&buf[size])
        {}
    };

    std::vector<page> pages_;
    T* here = nullptr;
    T* end = nullptr;
};

}   // util
}   // blockfish
