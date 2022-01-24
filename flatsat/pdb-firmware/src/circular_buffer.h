#ifndef _circular_buffer_h
#define _circular_buffer_h

#include <cstdio>
#include <memory>
// #include <mutex>

template <class T>
class circular_buffer {
    public:
        explicit circular_buffer(size_t size) :
        buf_(std::unique_ptr<T[]>(new T[size])),
        max_size_(size)
        {
        }

        void put(T item);
        T get();
        void reset();
        bool empty();
        bool full();
        size_t capacity();
        size_t size();

    private:
        // std::mutex mutex_;
        std::unique_ptr<T[]> buf_;
        size_t head_ = 0;
        size_t tail_ = 0;
        const size_t max_size_;
        bool full_ = 0;
};

#endif