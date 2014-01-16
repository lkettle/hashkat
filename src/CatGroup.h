#ifndef CATGROUP_H_
#define CATGROUP_H_

#include <vector>

#include "lcommon/strformat.h"
#include "MemPoolVector.h"

#include "util.h"

// Each category is defined with respect to a series of bounds
// and a categorization variable. This categorization variable can
// come from any source, and is used with a CatGroup to
// incrementally update a CatIndex.

struct CatIndex {
	int category, index;
	CatIndex(int c = -1, int i = -1) {
		category = c, index = i;
	}
};

// Only used as return result, for convenience
template <typename T>
struct CatList {
    CatList(T* data, int size) :
            data(data), size(size) {
    }

    T back() {
        return (*this)[0];
    }
    T front() {
         return (*this)[size - 1];
    }
    bool empty() {
        return size == 0;
    }
    T& operator[](int index) {
         ASSERT(size > index, "CatList::operator[](): size <= index");
         return data[index];
    }

    T* const data;
    const int size;
};

// Based on a roughly fixed allocation of memory for all categories,
// and a simple shifting insert that does not guarantee any list order.
template <typename T, typename IndexerT, int INIT_CATS = 8>
// IndexerT:
//  A generic structure
//      - turns T into a CatIndex with lookup()
//      - stores CatIndex associated with T with store()
// INIT_CATS:
//  More means more static memory per every category, but potentially less dynamic allocations.
//  Adjust depending on the average amount of categories your CatGroups have.
struct CatGroup {
    CatGroup(T* d, const IndexerT& indexer, int cap) : indexer(indexer) {
        data = d;
        capacity = cap;
    }
    size_t size() const {
        return cats.size;
    }

    void print_cats() {
        for (int i = 0; i < cats.size; i++) {
            printf("Cat %d: %d\n", i, cats[i].bound);
        }
        int last_bound = cats.back().bound;
        printf("Content: [");
        for (int i = 0 ; i < last_bound; i++) {
            printf("%d ", data[i]);
        }
        printf("]\n");
    }
    CatList<T> get_list(size_t index) {
        DEBUG_CHECK(index < cats.size, "CatGroup out-of-bounds access");
        int min = 0, max = cats[index].bound;
        if (index > 0) {
            min = cats[index-1].bound;
        }
        return CatList<T>(data + min, max - min);
    }
    void add(MemPool& mem_pool, const T& element, int category) {
        ensure_exists(mem_pool, category);
        printf("add elem=%d cat=%d\n", element, category);
        print_cats();
        // Create newly freed space in categories after this one:
        for (int c = category + 1; c < cats.size; c++) {
            CatList<T> list = get_list(c);
            // Move front element to after the back end
            if (!list.empty()) {
                list[list.size] = list.front();
            }
        }
        // Adjust end-bounds of the categories
        for (int c = category; c < cats.size; c++) {
            cats[c].bound++;
        }

        // Use newly freed space
        CatList<T> list = get_list(category);
        int new_location = list.size - 1;
        printf("newloc =%d\n", new_location);
        list[new_location] = element;
        printf("post_add\n");
        print_cats();
    }
    T remove(CatIndex ci) {
        print_cats();

        CatList<T> list = get_list(ci.category);
        DEBUG_CHECK(!list.empty(), "List should not be empty!");
        T deleted = list[ci.index];
        // Overwrite deleted slot with end element
        list[ci.index] = list.back();
        // Use newly freed space for categories after this one:
        for (int c = ci.category + 1; c < cats.size; c++) {
            CatList<T> list = get_list(c);
            // Move back element to before the front element
            if (!list.empty()) {
                list[-1] = list.back();
            }
        }
        // Adjust end-bounds of the categories
        for (int c = ci.category; c < cats.size; c++) {
            cats[c].bound--;
        }
        return deleted;
    }
private:
    void ensure_exists(MemPool& mem_pool, int category) {
        while (cats.size <= category) {
            int prev = cats.size > 0 ? cats.back().bound : 0;
            if (!mem_pool.add_if_possible(cats, CatInfo(prev))) {
                error_exit("Cat group memory exhausted -- unexpected; fatal error.");
            }
        }
    }
    T* data;
    int capacity;
    IndexerT indexer;
    struct CatInfo {
        int bound;
        CatInfo(int b = 0) { bound = b; }
    };
    MemPoolVector<CatInfo, INIT_CATS> cats;
};

template <typename T, int IA>
CatIndex categorize(CatGroup<T, IA>& group, const NumVec& thresh, CatIndex cat_index, double x) {
    ASSERT(thresh.size() >= 1, "Logic error");
    int c = cat_index.category;
    while (true) {
        double curr_min = (c >= 1 ? thresh[c-1] : 0);
        double curr_max = thresh[c];
        if (curr_min) {
            c--;
        }
    }
    if (c == cat_index.category) {
        return cat_index;
    }
    // Otherwise, we must recategorize
    T removed = group.remove(cat_index);
    CatIndex new_index(c, cat_index.category);
    group.add(cat_index);
    return new_index;
}

#endif
