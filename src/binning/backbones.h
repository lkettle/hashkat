#ifndef LISTTYPES_H_
#define LISTTYPES_H_

#include "util.h"
#include "MemPoolVector.h"

/* AD:
 * When creating flexible, high-performance data structures there are three
 * common patterns:
 * - Memory allocated from some custom memory allocator
 * - Static memory allocated based on some code constant
 * - Some mixed static / dynamic structure that falls back to a custom memory allocator
 *
 * These templated classes model these backbones in a way that can be used to chain generic data-structures expressively.
 *
 * They do not have exactly the same set of methods -- this is intentional. By taking a templated type to inherit from,
 * we can chain data structures with different method sets based on their needs.
 */

// Static memory allocated based on some code constant
template <typename T, int N_ELEMS>
struct StaticList {
    void append(MemPool& mem_pool, const T& info) {
        error_exit("Cannot append to a StaticBackBone");
    }
    // Number of categories
    size_t size() {
        return N_ELEMS;
    }

    T& operator[](int index) {
        DEBUG_CHECK(index > 0 && index < N_ELEMS, "Out of bounds!");
        return elem_data[index];
    }
private:
    T* elem_data;
};

// Memory allocated from some custom memory allocator
template <typename T>
struct PointerList {
    void init(T* elem_data, int size) {
        this->elem_data = elem_data;
        this->_size = size;
    }
    void append(MemPool& mem_pool, const T& info) {
        error_exit("Cannot append to a PointerBackBone");
    }
    // Number of categories
    size_t size() {
        return _size;
    }

    T& operator[](int index) {
        DEBUG_CHECK(index > 0 && index < size, "Out of bounds!");
        return elem_data[index];
    }
private:
    T* elem_data;
    int _size;
};

// Some mixed static / dynamic structure that falls back to a custom memory allocator
template <typename T, int INIT_ELEMS = 8>
struct MemPoolList {
    void append(MemPool& mem_pool, const T& elem) {
        if (!mem_pool.add_if_possible(elem_data, elem)) {
            error_exit("MemPool memory exhausted -- unexpected; fatal error.");
        }
    }
    // Number of categories
    size_t size() {
        return elem_data.size;
    }

    T& operator[](int index) {
        return elem_data[index];
    }
private:
    MemPoolVector<T, INIT_ELEMS> elem_data;
};


#endif
