#ifndef BINGROUP_H_
#define BINGROUP_H_

#include <vector>

#include "lcommon/strformat.h"
#include "lcommon/Range.h"
#include "MemPoolVector.h"

#include "entity.h"
#include "util.h"

// BinSet's allow you to efficiently store and update a series of lists that
// organize a discrete set of elements. Since the total content of the lists is
// known, and order is not important, the data-structure can efficiently
// store and move the elements in a fixed array.

struct BinIndex {
	int bin, index;
	explicit BinIndex(int c = -1, int i = -1) {
		bin = c, index = i;
	}
};

template <typename T>
struct NullWatcher {
    void on_set(BinIndex index, const T& element) {
        // Do nothing
    }
};

// Based on a roughly fixed allocation of memory for all categories,
// and a simple shifting insert that does not guarantee any list order.
template <typename T, typename WatcherT = NullWatcher<T>, int INIT_BINS = 8 >
// WatcherT:
//  A generic structure that observes all element shifting, allowing for 'plugging in' logic
//  that would otherwise require a rewrite.
// INIT_BINS:
//  More means more static memory per every bin, but potentially less dynamic allocations.
//  Adjust depending on the average amount of categories your BinSets have.
struct BinSet {
    BinSet(T* d, WatcherT watcher, int cap) : watcher(watcher) {
        data = d;
        capacity = cap;
    }
    size_t n_bins() {
        return bins.size;
    }
    size_t n_elems() {
        if (bins.size == 0) {
            return 0;
        }
        return bins.back().bound;
    }
    void add(MemPool& mem_pool, const T& element, int bin) {
        ensure_exists(mem_pool, bin);

        // Create newly freed space in bins after this one:
        for (int c = bins.size - 1; c >= bin + 1; c--) {
            Range r = bin_range(c);
            // Move front element to after the back end
            if (!r.empty()) {
                move(c, r.min, r.max);
            }
        }

        for (int c = bin; c < bins.size; c++) {
            bins[c].bound++;
        }

        set(BinIndex(bin, bound(bin) - 1), element);
    }
    void remove(BinIndex ci) {
        // Overwrite deleted slot with end element
        move(ci.bin, bound(ci.bin) - 1, ci.index);

        // Use newly freed space for bin after this one:
        for (int c = ci.bin + 1; c < bins.size; c++) {
            Range r = bin_range(c);
            // Move back element to before the front element
            if (!r.empty()) {
                move(c, r.max - 1, r.min - 1);
            }
        }
        // Adjust end-bounds of the bins
        for (int c = ci.bin; c < bins.size; c++) {
            bins[c].bound--;
        }
    }
    void print_bins() {
        for (int i = 0; i < bins.size; i++) {
            printf("Bin %d: %d\n", i, bins[i].bound);
        }
        int last_bound = bins.back().bound;
        printf("Content: [");
        for (int i = 0 ; i < last_bound; i++) {
            printf("%d ", data[i]);
        }
        printf("]\n");
    }
private:

    Range bin_range(size_t index) {
        DEBUG_CHECK(index < bins.size, "BinSet out-of-bounds access");
        int min = 0, max = bins[index].bound;
        if (index > 0) {
            min = bins[index-1].bound;
        }
        return Range(min, max);
    }

    void set(BinIndex index, const T& element) {
        data[index.index] = element;
        watcher.on_set(index, element);
    }

    int bound(int index) {
        return bins[index].bound;
    }
    void move(int bin, int start, int end) {
        printf("moving %d to %d\n", start, end);
        set(BinIndex(bin, end), data[start]);
    }

    void ensure_exists(MemPool& mem_pool, int bin) {
        while (bins.size <= bin) {
            int prev = bins.size > 0 ? bins.back().bound : 0;
            if (!mem_pool.add_if_possible(bins, BinInfo(prev))) {
                error_exit("Bin group memory exhausted -- unexpected; fatal error.");
            }
        }
    }
    T* data;
    int capacity;
    struct BinInfo {
        int bound;
        BinInfo(int b = 0) { bound = b; }
    };
    WatcherT watcher;
    MemPoolVector<BinInfo, INIT_BINS> bins;
};

#endif
