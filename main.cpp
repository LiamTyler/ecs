// #include "ecs.hpp"
#include <iostream>
#include <vector>
#include <assert.h>
#include <iterator>
#include <bitset>
#include <algorithm>

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


/*
    BaseComponent and Component<T> classes are for internal use only
    to map a type T to a uint32_t respresenting. A user's components
    can be POD's, and don't have to inherit from this. The first user component
    added will be mapped to 0, the 2nd to 1, etc.
*/
class BaseComponent {
public:
    static uint32_t typeCounter_;
};

uint32_t BaseComponent::typeCounter_ = 0;

template <typename Derived>
class Component : public BaseComponent {
public:
    static uint32_t typeCounter() {
        static uint32_t typeIndex = typeCounter_++;
        return typeIndex;
    }
};

template <typename T>
class ComponentHandle {
public:
    typedef T type;
    ComponentHandle() = default;
};


class Entity {
    friend class EntityPool;
public:
    Entity(EntityId id = EntityId()) : id_(id) {}

    EntityId id() const { return id_; }
    const ComponentMask& componentMask() const { return componentMask_; }

    template <typename T, typename... Args>
    void addComponent(Args&&... args) {
        std::cout << Component<T>::typeCounter() << std::endl;
    }

private:
    EntityId id_;
    ComponentMask componentMask_;
};


class EntityPool {
public:
    static const uint32_t BLOCK_SIZE = 2;

    class EntityIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Entity;
        using difference_type = std::ptrdiff_t;
        using pointer = Entity * ;
        using reference = Entity& ;

        EntityIterator(const EntityPool& pool, int b, int i) : p(pool), block(b), index(i) {}
        EntityIterator(const EntityPool& pool, uint32_t id) : p(pool), block(id / BLOCK_SIZE), index(id % BLOCK_SIZE) {}

        void next();
        void prev();

        value_type& operator*();
        value_type* operator->();

        EntityIterator& operator++() { next(); return *this; }
        EntityIterator operator++(int junk) { EntityIterator i = *this; next(); return i; }

        bool operator==(const EntityIterator& iter) {
            return block == iter.block && index == iter.index;
        }
        bool operator!=(const EntityIterator& iter) {
            return block != iter.block || index != iter.index;
        }
        bool operator<(const EntityIterator& iter) {
            return block * BLOCK_SIZE + index < iter.block * BLOCK_SIZE + iter.index;
        }
        bool operator>(const EntityIterator& iter) {
            return block * BLOCK_SIZE + index > iter.block * BLOCK_SIZE + iter.index;
        }

        friend std::ostream& operator<<(std::ostream& out, const EntityIterator& it) {
            return out << it.block << " " << it.index;
        }

        const EntityPool& p;
        int block;
        int index;
    };


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
        ++size_;

        // Assign the free ID to the corresponding entity
        uint32_t free = free_list_.back();
        free_list_.pop_back();
        int block = free / BLOCK_SIZE;
        int index = free % BLOCK_SIZE;
        Entity& entity = blocks_[block][index];
        entity.id_ = EntityId(free, entity.id_.version());

        
        // update the iterators
        if (block < beginning_.block)
            beginning_.index = index;
        else if (block == beginning_.block)
            beginning_.index = std::min(index, beginning_.index);
        beginning_.block = std::min(block, beginning_.block);

        ++free;
        block = free / BLOCK_SIZE;
        index = free % BLOCK_SIZE;
        if (block > end_.block)
            end_.index = index;
        else if (block == end_.block)
            end_.index = std::max(index, end_.index);
        end_.block = std::max(block, end_.block);

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
            e->id_ = EntityId(-1, id.version() + 1);
            free_list_.push_back(id.index());
            --size_;

            if (EntityIterator(*this, id.index()) == beginning_) {
                beginning_.next();
            }
            if (EntityIterator(*this, id.index() + 1) == end_) {
                end_.prev();
                ++end_.index;
                if (end_.index == BLOCK_SIZE) {
                    ++end_.block;
                    end_.index = 0;
                }
            }
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
        
    EntityIterator begin() const {
        return beginning_;
    }
        
    EntityIterator end() const {
        if (!size_)
            return beginning_;
        else
            return end_;
    }
    
    
protected:
    size_t size_ = 0;
    std::vector<Entity*> blocks_;
    std::vector<uint32_t> free_list_;

    EntityIterator beginning_ = EntityIterator(*this, 0, 0);
    EntityIterator end_ = EntityIterator(*this, 0, 0);
};
EntityPool pool;


EntityPool::EntityIterator::value_type& EntityPool::EntityIterator::operator*() {
    return pool.blocks_[block][index];
}

EntityPool::EntityIterator::value_type* EntityPool::EntityIterator::operator->() {
    return pool.blocks_[block] + index;
}

void EntityPool::EntityIterator::next() {
    do {
        ++index;
        if (index == BLOCK_SIZE) {
            ++block;
            index = 0;
        }
    } while (*this < p.end() && p.blocks_[block][index].id_.index() == (uint32_t)-1);
}

void EntityPool::EntityIterator::prev() {
    do {
        --index;
        if (index < 0) {
            index = BLOCK_SIZE - 1;
            --block;
        }
    } while (*this > p.begin() && p.blocks_[block][index].id_.index() == (uint32_t)-1);
}

Entity* EntityId::operator->() const {
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

typedef struct Test2 {
    int x = 1;
};

template <typename T>
class Foo {
public:
    using type = T;
};

using namespace std;

int main() {

    auto e1 = pool.create();
    auto e2 = pool.create();
    auto e3 = pool.create();
    auto e4 = pool.create();
    pool.destroy(e3);
    pool.destroy(e2);
    pool.printState();
    cout << "looping over entities" << endl;
    for (const auto& e : pool) {
        cout << e.id() << endl;
    }
    cout << endl;
    pool.destroy(e4);
    pool.destroy(e1);
    pool.destroy(pool.create());
    cout << pool.begin() << endl;
    cout << pool.end() << endl;

    cout << "looping over entities" << endl;
    for (const auto& e : pool) {
        cout << e.id() << endl;
    }
    cout << endl;

    /*if (!e3) {
        cout << "e3 (" << e3 << ") not found" << endl;
    }
    if (e1) {
        cout << "e1 (" << e1 << ") found" << endl;
    }*/

    /*e1->addComponent<Test>();
    e1->addComponent<Test>();
    e1->addComponent<Test2>();*/
    cout << sizeof(EntityPool::EntityIterator) << endl;

    return 0;
}
