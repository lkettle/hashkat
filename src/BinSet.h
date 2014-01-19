// BinSet's allow you to efficiently store and update a series of lists that
// organize a discrete set of elements. Since the total content of the lists is
// known, and order is not important, the data-structure can efficiently
// store and move the elements in a fixed array.

#ifndef BINSET_H_
#define BINSET_H_

#include <vector>

#include "lcommon/strformat.h"
#include "lcommon/Range.h"
#include "MemPoolVector.h"

#include "entity.h"
#include "util.h"

struct BinPosition {
	int bin, index;
	explicit BinPosition(int c = -1, int i = -1) {
		bin = c, index = i;
	}
};

template <typename T>
struct NullController {
    void on_set(BinPosition index, const T& element) {
        // Do nothing
    }
};

template <typename T, typename ControllerT = NullController<T>, int INIT_BINS = 8>
struct BinIndexer {
    BinIndexer(ControllerT controller) : _controller(controller) {
    }

    Range index_range(int bin) {
        return index_range(Range(bin, bin));
    }

    Range index_range(Range bins) {
        DEBUG_CHECK(bins.max < n_bins() && bins.min < n_bins(), "BinSet out-of-bounds access");
        int min = 0, max = bounds[bins.max];
        if (bins.min > 0) {
            min = bounds[bins.min-1];
        }
        return Range(min, max);
    }

    void add_bin(MemPool& mem_pool) {
        int prev = bounds.size > 0 ? bounds.back() : 0;
        if (!mem_pool.add_if_possible(bounds, prev)) {
            error_exit("Bin group memory exhausted -- unexpected; fatal error.");
        }
    }

    void on_set(BinPosition index, const T& element) {
        _controller.on_set(index, element);
    }

    // Number of categories
    size_t n_bins() {
        return bounds.size;
    }

    ControllerT& get_controller() {
        return _controller;
    }
    int& bound(int cat) {
        return bounds[cat];
    }

    MemPoolVector<int, INIT_BINS> bounds;
    ControllerT _controller;
};

// Based on a roughly fixed allocation of memory for all categories,
// and a simple shifting insert that does not guarantee any list order.
template <typename T, typename ControllerT = NullController<T>, int INIT_BINS = 8 >
// ControllerT:
//  A generic structure that observes all element shifting, allowing for 'plugging in' logic
//  that would otherwise require a rewrite.
// INIT_BINS:
//  More means more static memory per every BinSet, but potentially less dynamic allocations.
//  Adjust depending on the average amount of categories your BinSets have.
struct BinSet {
    BinSet(T* d, ControllerT controller, int cap) : indexer(controller) {
        data = d;
        capacity = cap;
    }

    size_t n_elems() {
        return indexer.bound(indexer.n_bins()-1);
    }

    void add(MemPool& mem_pool, const T& element, int bin) {
        ensure_exists(mem_pool, bin);

        // Create newly freed space in bins after this one:
        for (int c = indexer.n_bins() - 1; c >= bin + 1; c--) {
            Range r = indexer.index_range(c);
            // Move front element to after the back end
            if (!r.empty()) {
                move(c, r.min, r.max);
            }
        }

        for (int c = bin; c < indexer.n_bins(); c++) {
            indexer.bound(c)++;
        }

        set(BinPosition(bin, indexer.bound(bin) - 1), element);
    }
    void remove(BinPosition ci) {
        // Overwrite deleted slot with end element
        move(ci.bin, indexer.bound(ci.bin) - 1, ci.index);

        // Use newly freed space for bin after this one:
        for (int c = ci.bin + 1; c < indexer.n_bins(); c++) {
            Range r = indexer.index_range(c);
            // Move back element to before the front element
            if (!r.empty()) {
                move(c, r.max - 1, r.min - 1);
            }
        }
        // Adjust end-bounds of the bins
        for (int c = ci.bin; c < indexer.n_bins(); c++) {
            indexer.bound(c)--;
        }
    }
    void print_bins() {
//        for (int i = 0; i < bins.size; i++) {
//            printf("Bin %d: %d\n", i, bins[i].bound);
//        }
//        int last_bound = bins.back().bound;
//        printf("Content: [");
//        for (int i = 0 ; i < last_bound; i++) {
//            printf("%d ", data[i]);
//        }
//        printf("]\n");
    }

    ControllerT& get_controller() {
        return indexer.get_controller();
    }

private:
    void set(BinPosition index, const T& element) {
        data[index.index] = element;
        indexer.on_set(index, element);
    }

    void move(int bin, int start, int end) {
        printf("moving %d to %d\n", start, end);
        set(BinPosition(bin, end), data[start]);
    }

    void ensure_exists(MemPool& mem_pool, int bin) {
        while (indexer.n_bins() <= bin) {
            indexer.add_bin(mem_pool);
        }
    }
    T* data;
    int capacity;

    BinIndexer<T, ControllerT, INIT_BINS> indexer;
};

#endif
