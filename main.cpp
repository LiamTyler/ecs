// #include "ecs.hpp"
#include <iostream>
#include <vector>
#include <assert.h>
#include <iterator>
#include <bitset>

// typedef uint64_t EntityId;

//class BasePool {
//    public:
//        BasePool(size_t elementSize, size_t elementsPerBlock) :
//            elementSize_(elementSize),
//            elementsPerBlock_(elementsPerBlock)
//        {
//        }
//
//        virtual ~BasePool() {
//            for (auto block : blocks_)
//                delete[] block;
//        }
//
//        size_t size() const { return size_; }
//        size_t capacity() const { return size_; }
//        size_t blocks() const { return blocks_.size(); }
//
//        inline void expand(size_t n) {
//            if (n > size_) {
//                while (capacity_ < n) {
//                    blocks_.push_back(new char[elementSize_ * elementsPerBlock_]);
//                    capacity_ += elementsPerBlock_;
//                }
//                size_ = n;
//            }
//        }
//
//        inline void* get(size_t n) {
//            assert(n < size_);
//            return blocks_[n / elementsPerBlock_] + (n % elementsPerBlock_) * elementSize_;
//        }
//
//        virtual void destroy(size_t n) = 0;
//
//
//    protected:
//        size_t size_ = 0;
//        size_t capacity_ = 0;
//        size_t elementSize_;
//        size_t elementsPerBlock_;
//        std::vector<char*> blocks_;
//};
//
//template <typename T, std::size_t BlockSize = 1024>
//class Pool : public BasePool {
//    public:
//        Pool() : BasePool(sizeof(T), BlockSize) {}
//
//        // Note: maybe shouldnt bother calling destructors
//        virtual ~Pool() {
//            /*size_t block = 0;
//            size_t i = 0;
//            size_t n = 0;
//            while (n < size_) {
//                T* ptr = static_cast<T*>((void*) (blocks_[block] + i * elementSize_));
//                ptr->~T();
//                ++n;
//                ++i;
//                if (i == elementsPerBlock_) {
//                    i = 0;
//                    ++block;
//                }
//            }*/
//        }
//
//        
//        virtual void destroy(std::size_t n) override {
//            assert(n < size_);
//            T* ptr = static_cast<T*>(get(n));
//            ptr->~T();
//        }
//
//        /*
//        class PoolIterator {
//            using iterator_category = std::forward_iterator_tag;
//            using value_type = T;
//            using difference_type = T;
//            using pointer = T*;
//            using reference = T&;
//
//            PoolIterator(size_t b, size_t i) : block(b), index(i) {}
//
//            protected:
//                size_t block;
//                size_t index;
//
//        };
//
//        PoolIterator begin() {
//            return PoolIterator(0, 0);
//        }
//
//        PoolIterator end() {
//            return PoolIterator(0, 0);
//        }
//        */
//
//};

static const size_t MAX_COMPONENTS = 64;
typedef std::bitset<MAX_COMPONENTS> ComponentMask;

class Entity;

typedef struct EntityId {
    EntityId(uint32_t index = (uint32_t) -1, uint32_t version = 0) : id_(uint64_t(index) | (uint64_t(version) << 32UL)) {}

    uint64_t id() const { return id_; }
    uint32_t index() const { return id_ & 0xFFFFFFFFUL; }
    uint32_t version() const { return id_ >> 32UL; }

    void destory() const;

    bool operator == (const EntityId& other) const { return id_ == other.id_; }
    bool operator != (const EntityId& other) const { return id_ != other.id_; }
    bool operator < (const EntityId& other) const { return id_ < other.id_; }

    friend std::ostream& operator<<(std::ostream& out, const EntityId& id) {
        return out << "index = " << id.index() << ", version = " << id.version();
    }

    Entity* operator->() const;
    operator bool() const;
    Entity& operator*();
    const Entity& operator*() const;

private:
    uint64_t id_;
} EntityId;


class Entity {
    friend class EntityPool;
public:
    Entity(EntityId id = EntityId()) : id_(id) {}

    EntityId id() const { return id_; }
    const ComponentMask& componentMask() const { return componentMask_; }

private:
    EntityId id_;
    ComponentMask componentMask_;
};


class EntityPool {
    const uint32_t BLOCK_SIZE = 2;
public:
    EntityPool() {}
    
    ~EntityPool() {
        for (auto block : blocks_)
            delete[] block;
    }
    
    size_t size() const { return size_; }
    
    inline EntityId create() {
        if (free_list_.empty()) {
            Entity* chunk = new Entity[BLOCK_SIZE];
            for (int i = BLOCK_SIZE - 1; i >= 0; --i) {
                free_list_.push_back(blocks_.size() * BLOCK_SIZE + i);
            }
            blocks_.push_back(chunk);
        }

        int free = free_list_.back();
        free_list_.pop_back();
        Entity& entity = blocks_[free / BLOCK_SIZE][free % BLOCK_SIZE];
        entity.id_ = EntityId(free, entity.id_.version());
        return entity.id_;
    }
    
    inline Entity* get(const EntityId& id) {
        const auto index = id.index();
        Entity* e = blocks_[index / BLOCK_SIZE] + (index % BLOCK_SIZE);
        return e->id() != id ? nullptr : e;
    }
    
    inline void destroy(const EntityId& id) {
        Entity* e = get(id);
        if (e != nullptr) {
            e->id_ = EntityId(id.index(), id.version() + 1);
            free_list_.push_back(id.index());
        }
    }

    inline void printState() {
        using std::cout;
        using std::endl;
        for (int i = 0; i < blocks_.size(); ++i) {
            cout << "Block: " << i << endl;
            for (int ii = 0; ii < BLOCK_SIZE; ++ii) {
                cout << "\t" << blocks_[i][ii].id() << endl;
            }
        }
        cout << endl;
    }
    
    
protected:
    size_t size_ = 0;
    std::vector<Entity*> blocks_;
    std::vector<uint32_t> free_list_;
};

EntityPool pool;

Entity* EntityId::operator->() const {
    std::cout << "in -> operator" << std::endl;
    return pool.get(*this);
}

EntityId::operator bool() const {
    return pool.get(*this) != nullptr;
}

Entity& EntityId::operator* () {
    return *pool.get(*this);
}

const Entity& EntityId::operator* () const {
    return *pool.get(*this);
}

void EntityId::destory() const {
    pool.destroy(*this);
}



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

    auto e1 = pool.create();
    pool.printState();
    pool.create();
    pool.printState();
    auto e3 = pool.create();
    pool.printState();
    auto e = pool.create();
    pool.printState();
    pool.destroy(e);
    pool.printState();
    pool.destroy(e3);
    pool.printState();
    if (!e3) {
        cout << "e3 (" << e3 << ") not found" << endl;
    }
    if (e1) {
        cout << "e1 (" << e1 << ") found" << endl;
    }
    Entity& ee = *e3;

    return 0;
}
