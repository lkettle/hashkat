#ifndef UNORDEREDBINSTORAGE_H_
#define UNORDEREDBINSTORAGE_H_

#include "util.h"

// Lowest level of the binning storage chain.

struct BinPosition {
    int bin, index;
    explicit BinPosition(int c = -1, int i = -1) {
        bin = c, index = i;
    }
};

template <typename T, typename Backbone = MemPoolList<T> >
/* Relies on Context to provide storage of actual bin ranges.
 * T:
 *  The type to store
 * Backbone:
 *  Either MemPoolBackbone or PointerBackbone
 */
struct UnorderedBinStorage : public Backbone {
    /* Add an element to the specified bin.
     * Assumes the element does not exist in the bin.
     * Only intended for use by a parent bin set. */
    template <typename Context>
    double add(Context& C, const T& element) {
        printf("** ADD:\n");
        print_bins();

        int bin = C.bin();
        this->ensure_exists(C, bin);

        // Create newly freed space in bins after this one:
        for (int b = C->n_bins() - 1; b >= bin + 1; b--) {
            Range r = C->index_range(b);
            // Move front element to after the back end
            if (!r.empty()) {
                set(C, r.max, this->get(r.min));
            }
        }

        for (int b = bin; b < this->n_bins(); b++) {
            C.bound(b)++;
        }

        set(C, BinPosition(bin, C.bound(bin) - 1), element);

        printf("**POST ADD:\n");
        print_bins();
    }

    /* Add an element to the specified bin.
     * Assumes the element does not exist in the bin.
     * Only intended for use by a parent bin set. */
    template <typename Context>
    void remove(Context& C, const T& element) {
        printf("** REMOVE:\n");
        print_bins();
        int bin = C.bin();
        BinPosition bp = this->get_position(C, bin, element);
        C.on_remove(C, bp, element);

        // Overwrite deleted slot with end element
        relocate(C, BinPosition(bin, C.bound(bin) - 1), ci.index);

        // Use newly freed space for bin after this one:
        for (int b = bin + 1; b < this->n_bins(); b++) {
            Range r = C.index_range(b);
            // Move back element to before the front element
            if (!r.empty()) {
                relocate(C, BinPosition(b, r.max - 1), this->get(r.min - 1));
            }
        }
        // Adjust end-bounds of the bins
        for (int b = ci.bin; b < this->n_bins(); b++) {
            C.bound(b)--;
        }

        printf("**POST REMOVE:\n");
        print_bins();
    }

    // Move, inside the same bin.
    template <typename Context>
    void relocate(Context& C, BinPosition pos, const T& element) {
        this->get(pos.index) = element;
        C.on_relocate(C, index, element);
    }

    // Add to new bin.
    template <typename Context>
    void set(Context& C, BinPosition pos, const T& element) {
        this->get(pos.index) = element;
        C.on_add(C, pos, element);
    }

    template <typename Context>
    void ensure_exists(Context& C, int category) {
        while (bin_data.size <= category) {
            this->append(C, C.create_bin(bin_data.size));
        }
    }

    void print_bins() {
        if (this->bounds.size == 0) {
            return;
        }
        for (int i = 0; i < this->bounds.size; i++) {
            std::cout << "Bin " << i << ": " << bound(i) << std::endl;
        }
        int last_bound = this->bounds.back().bound;
        printf("Content: [");
        for (int i = 0 ; i < last_bound; i++) {
            printf("%d ", this->data[i]);
        }
        printf("]\n");
    }
};

#endif /* UNORDEREDBINSTORAGE_H_ */
