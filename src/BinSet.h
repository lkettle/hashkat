// BinSet's allow you to efficiently store and update a series of lists that
// organize a discrete set of elements. Since the total content of the lists is
// known, and order is not important, the data-structure can efficiently
// store and move the elements in a fixed array.

#ifndef BINSET_H_
#define BINSET_H_

#include <vector>
#include <map>

#include <iostream>
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
struct BinList {
    const T* data;
    const int size;
    BinList(T* data, int size) :
            data(data), size(size) {
    }
};

template <typename T, typename BinInfo, int INIT_BINS = 8>
struct AbstractLayer {
    /* Main operations: */
    template <typename Context>
    void add_bin(Context& context, BinInfo b = BinInfo()) {
        int prev = bounds.size > 0 ? bounds.back().bound : 0;
        b.bound = prev;
        if (!context.get_mem_pool().add_if_possible(bounds, b)) {
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

    size_t n_elems() {
        Range r = index_range(Range(0,bounds.size-1));
        return (r.max - r.min);
    }

    // Number of categories
    size_t n_bins() {
        return bounds.size;
    }

    BinInfo& get(int bin) {
        return bounds[bin];
    }

    template <typename Context>
    void ensure_exists(Context& context, int category) {
        while (bounds.size <= category) {
            int prev = bounds.size > 0 ? bounds.back().bound : 0;
            if (!context.get_mem_pool().add_if_possible(bounds, BinInfo(prev))) {
                error_exit("Cat group memory exhausted -- unexpected; fatal error.");
            }
        }
    }
public:
    MemPoolVector<BinInfo, INIT_BINS> bounds;
};

struct RateInfo {
    int bound;
    int r_total;
    RateInfo(int bound = 0, int r_total = 0) :
            bound(bound), r_total(r_total) {
    }
};

template <typename T, typename IndexerT, int INIT_BINS = 8>
struct StoreData : public AbstractLayer<T, RateInfo, INIT_BINS> {
    void init(IndexerT indexer, T* d, int cap) {
        data = d;
        capacity = cap;
    }

    template <typename Context>
    BinPosition get_position(Context& context, int bin, const T& element) {
        int index = this->indexer.lookup(context, bin, element);
        return BinPosition(bin, index);
    }

    template <typename Context>
    void set(Context& context, BinPosition index, const T& element) {
        data[index.index] = element;
        this->indexer.store(context, index, element);
    }

    // Helper for incrementally managing the rate for a specific element
    template <typename Context>
    double rate_add(Context& context, int bin, const T& element) {
        double delta = indexer.rate(context, bin);
        this->get(bin).r_total += delta;
        return delta;
    }

    // Helper for incrementally managing the rate for a specific element
    template <typename Context>pre
    double rate_subtract(Context& context, int bin, const T& element) {
        double delta = -indexer.rate(context, bin);
        this->get(bin).r_total += delta;
        return delta;
    }

    // Used for time-dependent rates.
    // Shift every cohort up a bin.
    // NOTE: Will 'push off' users from the end.
    template <typename Context>
    void shift_cohorts(Context& context, Range range) {
        printf("Shifting the categories\n");

        for (int bin = range.max - 1; bin >= range.min; bin--) {
            Range range = this->index_range(bin);
            int n_elems = (range.max - range.min);
            this->get(bin + 1) = this->get(bin);
            this->get(bin + 1).r_total = n_elems * indexer.rate(context, bin + 1);
        }

        this->get(0) = RateInfo();
    }

    template <typename Context>
    void move(Context& context, int bin, int start, int end) {
        printf("moving %d to %d\n", start, end);
        set(context, BinPosition(bin, end), data[start]);
    }
    IndexerT& get_indexer() {
        return indexer;
    }

    BinList<T> get_list(int bin) {
        Range range = this->index_range(bin);
        return BinList<T>(data + range.min, (range.max - range.min));
    }

protected:
    IndexerT indexer;
    T* data;
    int capacity;
};

// Based on a roughly fixed allocation of memory for all categories,
// and a simple shifting insert that does not guarantee any list order.
template <typename T, typename StoreT, int INIT_BINS = 8 >
struct StoreLayer : public StoreT {
    StoreLayer() {
        r_total = 0;
    }

    template <typename Context>
    double add(Context& context, const T& element, int bin) {
        printf("** ADD:\n");
        print_bins();

        this->ensure_exists(context, bin);

        // Create newly freed space in bins after this one:
        for (int c = this->n_bins() - 1; c >= bin + 1; c--) {
            Range r = this->index_range(c);
            // Move front element to after the back end
            if (!r.empty()) {
                this->move(context, c, r.min, r.max);
            }
        }

        for (int c = bin; c < this->n_bins(); c++) {
            this->get(c).bound++;
        }

        this->set(context, BinPosition(bin, this->get(bin).bound - 1), element);
        double delta = this->rate_add(context, bin, element);
        r_total += delta;

        printf("**POST ADD:\n");
        print_bins();
        return delta;
    }

    template <typename Context>
    double remove(Context& context, const T& element, int bin) {
        printf("** REMOVE:\n");
        print_bins();
        BinPosition ci = this->get_position(context, bin, element);

        // Overwrite deleted slot with end element
        this->move(context, ci.bin, this->get(ci.bin).bound - 1, ci.index);

        // Use newly freed space for bin after this one:
        for (int c = ci.bin + 1; c < this->n_bins(); c++) {
            Range r = this->index_range(c);
            // Move back element to before the front element
            if (!r.empty()) {
                this->move(context, c, r.max - 1, r.min - 1);
            }
        }
        // Adjust end-bounds of the bins
        for (int c = ci.bin; c < this->n_bins(); c++) {
            this->get(c).bound--;
        }
        double delta = this->rate_subtract(context, bin, element);
        r_total += delta;

        printf("**POST REMOVE:\n");
        print_bins();
        return delta;
    }
    void print_bins() {
        if (this->bounds.size == 0) {
            return;
        }
        for (int i = 0; i < this->bounds.size; i++) {
            std::cout << "Bin " << i << ": " << this->bounds[i].bound << " r_total: " << this->bounds[i].r_total << '\n';
        }
        int last_bound = this->bounds.back().bound;
        printf("Content: [");
        for (int i = 0 ; i < last_bound; i++) {
            printf("%d ", this->data[i]);
        }
        printf("]\n");
    }

    double get_total_rate() {
        return r_total;
    }

private:
    double r_total;
};

#endif
