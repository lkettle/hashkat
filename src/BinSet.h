// BinSet's allow you to efficiently store and update a series of lists that
// organize a discrete set of elements. Since the total content of the lists is
// known, and order is not important, the data-structure can efficiently
// store and move the elements in a fixed array.

#ifndef BINSET_H_
#define BINSET_H_

#include <vector>
#include <map>

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

    template <typename Context>
    void add_bin(Context& context, MemPool& mem_pool) {
        // Do nothing
    }

    template <typename Context>
    void on_set(Context& context, BinPosition index, const T& element) {
        // Do nothing
    }
};

/* Used as part of BinSet. */
template <typename T, typename ControllerT, int INIT_BINS = 8>
struct BinIndexer2 {
    BinIndexer2(ControllerT controller) : _controller(controller) {
    }

    /* Main operations: */
    template <typename Context>
    void add_bin(Context& context) {
        int prev = bounds.size > 0 ? bounds.back() : 0;
        if (!context.get_mem_pool().add_if_possible(bounds, prev)) {
            error_exit("Bin group memory exhausted -- unexpected; fatal error.");
        }
    }

    template <typename Context>
    void on_set(Context& context, BinPosition index, const T& element) {
        _controller.on_set(index, element);
    }

    /* Supplementary operations: */
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

template <typename BinInfo, int INIT_BINS = 8>
struct BinBounds {
    BinBounds() {
    }

    /* Main operations: */
    template <typename Context>
    void add_bin(Context& context) {
        int prev = bounds.size > 0 ? bounds.back().bound : 0;
        if (!context.get_mem_pool().add_if_possible(bounds, prev)) {
            error_exit("Bin group memory exhausted -- unexpected; fatal error.");
        }
    }

    /* Supplementary operations: */
    Range index_range(int bin) {
        return index_range(Range(bin, bin));
    }

    Range index_range(Range bins) {
        DEBUG_CHECK(bins.max < n_bins() && bins.min < n_bins(), "BinSet out-of-bounds access");
        int min = 0, max = bounds[bins.max].bound;
        if (bins.min > 0) {
            min = bounds[bins.min-1].bound;
        }
        return Range(min, max);
    }

    // Number of categories
    size_t n_bins() {
        return bounds.size;
    }

    BinInfo& get(int bin) {
        return bounds[bin];
    }
private:
    MemPoolVector<BinInfo, INIT_BINS> bounds;
};

/* Used as part of BinSet. */
template <typename T, typename IndexerT, typename ChildT, int INIT_BINS = 8>
struct RateIndexer {
    RateIndexer(const IndexerT& indexer, const ChildT& child) :
            indexer(indexer), child(child) {
    }

    template <typename Context>
    double add(Context& context, const T& element, int bin) {
        int sub = sub_bin(element, bin);
        double rate_diff = child.add(context, element, sub);

        RateInfo& info = bounds.get(sub);
        info.rate += rate_diff;

        return rate_diff;
    }

    template <typename Context>
    double remove(Context& context, const T& element, int bin) {
        int sub = sub_bin(element, bin);
        double rate_diff = child.remove(context, element, sub);

        RateInfo& info = bounds.get(sub);
        info.rate -= rate_diff;

        return rate_diff;
    }
private:
    int sub_bin(const T& element, int bin) {
        return indexer.get_slot(bounds.index_range(bin), element);
    }
    struct RateInfo {
        int rate;
        int bound;
    };
    BinBounds<RateInfo, INIT_BINS> bounds;
    IndexerT indexer;
    ChildT child;

};

struct HashLayer {
//    template <typename Context>
//    void lookup(Context& context, const T& element, int bin) {
//
//    }
};

// Based on a roughly fixed allocation of memory for all categories,
// and a simple shifting insert that does not guarantee any list order.
template <typename T, typename IndexerT, int INIT_BINS = 8 >
// Context:
//  Eg, the network segment
// ControllerT:
//  A generic structure that observes all element shifting, allowing for 'plugging in' logic
//  that would otherwise require a rewrite.
// INIT_BINS:
//  More means more static memory per every BinSet, but potentially less dynamic allocations.
//  Adjust depending on the average amount of categories your BinSets have.
struct BinSet {
    BinSet(T* d, IndexerT indexer, int cap) : indexer(indexer) {
        data = d;
        capacity = cap;
    }

    size_t n_elems() {
        return indexer.bound(indexer.n_bins()-1);
    }

    template <typename Context>
    void add(Context& context, const T& element, int bin) {
        ensure_exists(context, bin);

        // Create newly freed space in bins after this one:
        for (int c = indexer.n_bins() - 1; c >= bin + 1; c--) {
            Range r = indexer.index_range(c);
            // Move front element to after the back end
            if (!r.empty()) {
                move(context, c, r.min, r.max);
            }
        }

        for (int c = bin; c < indexer.n_bins(); c++) {
            indexer.bound(c)++;
        }

        set(context, BinPosition(bin, indexer.bound(bin) - 1), element);
    }

    template <typename Context>
    void remove(Context& context, BinPosition ci) {
        // Overwrite deleted slot with end element
        move(context, ci.bin, indexer.bound(ci.bin) - 1, ci.index);

        // Use newly freed space for bin after this one:
        for (int c = ci.bin + 1; c < indexer.n_bins(); c++) {
            Range r = indexer.index_range(c);
            // Move back element to before the front element
            if (!r.empty()) {
                move(context, c, r.max - 1, r.min - 1);
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

    IndexerT& get_indexer() {
        return indexer;
    }

private:

    template <typename Context>
    void set(Context& context, BinPosition index, const T& element) {
        data[index.index] = element;
        indexer.on_set(context, index, element);
    }

    template <typename Context>
    void move(Context& context, int bin, int start, int end) {
        printf("moving %d to %d\n", start, end);
        set(context, BinPosition(bin, end), data[start]);
    }

    template <typename Context>
    void ensure_exists(Context& context, int bin) {
        while (indexer.n_bins() <= bin) {
            indexer.add_bin(context.mem_pool());
        }
    }
    T* data;
    int capacity;

    IndexerT indexer;
};

#endif
