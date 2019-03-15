// #include "ecs.hpp"
#include <iostream>
#include <vector>
#include <assert.h>
#include <iterator>

class BasePool {
    public:
        BasePool(size_t elementSize, size_t elementsPerBlock) :
            elementSize_(elementSize),
            elementsPerBlock_(elementsPerBlock)
        {
        }

        virtual ~BasePool() {
            for (auto block : blocks_)
                delete[] block;
        }

        size_t size() const { return size_; }
        size_t capacity() const { return size_; }
        size_t blocks() const { return blocks_.size(); }

        inline void expand(size_t n) {
            if (n > size_) {
                while (capacity_ < n) {
                    blocks_.push_back(new char[elementSize_ * elementsPerBlock_]);
                    capacity_ += elementsPerBlock_;
                }
                size_ = n;
            }
        }

        inline void* get(size_t n) {
            assert(n < size_);
            return blocks_[n / elementsPerBlock_] + (n % elementsPerBlock_) * elementSize_;
        }

        virtual void destroy(size_t n) = 0;


    protected:
        size_t size_ = 0;
        size_t capacity_ = 0;
        size_t elementSize_;
        size_t elementsPerBlock_;
        std::vector<char*> blocks_;
};

template <typename T, std::size_t BlockSize = 4096>
class Pool : public BasePool {
    public:
        Pool() : BasePool(sizeof(T), BlockSize) {}

        // Note: maybe shouldnt bother calling destructors
        virtual ~Pool() {
            size_t block = 0;
            size_t i = 0;
            size_t n = 0;
            while (n < size_) {
                T* ptr = static_cast<T*>((void*) (blocks_[block] + i * elementSize_));
                ptr->~T();
                ++n;
                ++i;
                if (i == elementsPerBlock_) {
                    i = 0;
                    ++block;
                }
            }
        }

        virtual void destroy(std::size_t n) override {
            assert(n < size_);
            T* ptr = static_cast<T*>(get(n));
            ptr->~T();
        }

        class PoolIterator {
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = T;
            using pointer = T*;
            using reference = T&;

            PoolIterator(size_t b, size_t i) : block(b), index(i) {}

            protected:
                size_t block;
                size_t index;

        };

        PoolIterator begin() {
            return PoolIterator(0, 0);
        }

        PoolIterator end() {
            return PoolIterator(0, 0);
        }

};


typedef struct Test {
    ~Test() {
        std::cout << "Test destructor" << std::endl;
    }
    int x = 0;
    int y = 1;
    int z = 2;

} Test;

template <typename T>
class Foo {
public:
    using type = T;
};

using namespace std;

int main() {

    Pool<Test> pool;
    pool.expand(10);

    return 0;
}
