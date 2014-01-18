#ifndef CATGROUP_H_
#define CATGROUP_H_

#include <vector>

#include "lcommon/strformat.h"
#include "lcommon/Range.h"
#include "MemPoolVector.h"

#include "entity.h"
#include "util.h"

// Each category is defined with respect to a series of bounds
// and a categorization variable. This categorization variable can
// come from any source, and is used with a CatGroup to
// incrementally update a CatIndex.

struct CatIndex {
	int category, index;
	explicit CatIndex(int c = -1, int i = -1) {
		category = c, index = i;
	}
};

struct CatEntityIndexer {
    CatEntityIndexer(CatIndex* data, int cap) : data(data), capacity(cap) {
    }

    CatIndex lookup(const EntityID& id) {
        return data[id.slot_id];
    }

    void store(const EntityID& id, CatIndex index) {
        data[id.slot_id] = index;
    }

    CatIndex* data;
    int capacity;
};

// Based on a roughly fixed allocation of memory for all categories,
// and a simple shifting insert that does not guarantee any list order.
template <typename T, typename IndexerT = CatEntityIndexer, int INIT_CATS = 8>
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
    size_t n_cats() {
        return bins.size;
    }
    size_t n_elems() {
        if (bins.size == 0) {
            return 0;
        }
        return bins.back().bound;
    }
    void add(MemPool& mem_pool, const T& element, int category) {
        ensure_exists(mem_pool, category);

        // Create newly freed space in categories after this one:
        for (int c = bins.size - 1; c >= category + 1; c--) {
            Range r = cat_range(c);
            // Move front element to after the back end
            if (!r.empty()) {
                move(c, r.min, r.max);
            }
        }

        for (int c = category; c < bins.size; c++) {
            bins[c].bound++;
        }

        set(CatIndex(category, bound(category) - 1), element);
    }
    void remove(const T& element) {
        CatIndex ci = lookup(element);
        DEBUG_CHECK(data[ci.index] == element, "Element removed must match stored index!");

        // Overwrite deleted slot with end element
        move(ci.category, bound(ci.category) - 1, ci.index);

        // Use newly freed space for categories after this one:
        for (int c = ci.category + 1; c < bins.size; c++) {
            Range r = cat_range(c);
            // Move back element to before the front element
            if (!r.empty()) {
                move(c, r.max - 1, r.min - 1);
            }
        }
        // Adjust end-bounds of the categories
        for (int c = ci.category; c < bins.size; c++) {
            bins[c].bound--;
        }
    }
    void print_cats() {
        for (int i = 0; i < bins.size; i++) {
            printf("Cat %d: %d\n", i, bins[i].bound);
        }
        int last_bound = bins.back().bound;
        printf("Content: [");
        for (int i = 0 ; i < last_bound; i++) {
            printf("%d ", data[i]);
        }
        printf("]\n");
    }
private:

    Range cat_range(size_t index) {
        DEBUG_CHECK(index < bins.size, "CatGroup out-of-bounds access");
        int min = 0, max = bins[index].bound;
        if (index > 0) {
            min = bins[index-1].bound;
        }
        return Range(min, max);
    }

    CatIndex lookup(const T& element) {
        return indexer.lookup(element);
    }
    void set(CatIndex index, const T& element) {
        data[index.index] = element;
        indexer.store(element, index);
    }

    int bound(int index) {
        return bins[index].bound;
    }
    void move(int cat, int start, int end) {
        printf("moving %d to %d\n", start, end);
        set(CatIndex(cat, end), data[start]);
    }

    void ensure_exists(MemPool& mem_pool, int category) {
        while (bins.size <= category) {
            int prev = bins.size > 0 ? bins.back().bound : 0;
            if (!mem_pool.add_if_possible(bins, CatInfo(prev))) {
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
    MemPoolVector<CatInfo, INIT_CATS> bins;
};

//template <typename T, int IA>
//CatIndex categorize(CatGroup<T, IA>& group, const NumVec& thresh, CatIndex cat_index, double x) {
//    ASSERT(thresh.size() >= 1, "Logic error");
//    int c = cat_index.category;
//    while (true) {
//        double curr_min = (c >= 1 ? thresh[c-1] : 0);
//        double curr_max = thresh[c];
//        if (curr_min) {
//            c--;
//        }
//    }
//    if (c == cat_index.category) {
//        return cat_index;
//    }
//    // Otherwise, we must recategorize
//    T removed = group.remove(cat_index);
//    CatIndex new_index(c, cat_index.category);
//    group.add(cat_index);
//    return new_index;
//}

#endif
