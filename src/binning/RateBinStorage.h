#ifndef RATEBINSTORAGE_H_
#define RATEBINSTORAGE_H_

#include "util.h"
#include "contexts.h"

#include "UnorderedBinStorage.h"

struct RateBinInfo {
    double rate;
    double bound;
};

template <typename Base, typename Storage, typename Parent>
struct RateIndexer : public Base {
    RateIndexer(Parent& P, int bin, Storage& storage) : storage(storage), bin(bin), P(P) {
    }

    void rate_change(double delta) {
        storage.rate_change(delta);
        P.rate_change(delta);
    }

    int bin;
    Storage& storage;
    Parent& P;
};

template <typename T, typename Indexer, typename Storage, typename Parent>
struct RateContext : public ForwardingContext<T, Parent> {
    typedef Indexer IndexerT;

    RateContext(Parent& P, Storage& self, IndexerT& indexer, int bin) :
            self(self), indexer(indexer), ForwardingContext<T, Parent>(P), _bin(bin){
    }

    int bin() {
        return _bin; // Arbitrary, root node
    }

    int n_bins() {
        return self.storage.size;
    }

    int& bound(int bin) {
        return self.storage[bin].bound;
    }

    // Remove from a bin.
    template <typename Context>
    void on_remove(Context& C, BinPosition pos, const T& element) {
        indexer.rate_change(C, -indexer.rate(C, pos, element));
        indexer.set_position(C, BinPosition(-1, -1), element);
    }
    // Move, inside the same bin.
    template <typename Context>
    void on_relocate(Context& C, BinPosition pos, const T& element) {
        indexer.set_position(C, pos, element);
    }

    // Add to new bin.
    template <typename Context>
    void on_add(Context& C, BinPosition pos, const T& element) {
        indexer.rate_change(C, +indexer.rate(C, pos, element));
        indexer.set_position(C, pos, element);
    }

    IndexerT& indexer;
    Storage& self;
    int _bin;
};

/* Rate-binning layer.
 *
 * Uses bins to provide incremental rate updates.
 */
template <typename T, typename IndexerBase, typename Storage = UnorderedBinStorage<RateBinInfo> >
struct RateBinStorage {
    RateBinStorage() {
        r_total = 0;
    }

    double& rate(int bin) {
        return storage[bin].rate;
    }
    /* Add an element to the specified bin.
     * Assumes the element does not exist in the bin.
     * Only intended for use by a parent bin set. */
    template <typename Context>
    void add(Context& C, const T& element) {
        int root_bin = C.bin();
        typedef Context::IndexerT ParentIndexT;
        typedef RateIndexer<IndexerBase, RateBinStorage, ParentIndexT> RIndexer;
        typedef RateContext<T, RIndexer, RateBinStorage, Context> RContext;

        RIndexer indexer;
        RContext context(*this, indexer, C);
        printf("** RATE BIN ADD:\n");
        storage.add(C, element);
    }

    /* Add an element to the specified bin.
     * Assumes the element does not exist in the bin.
     * Only intended for use by a parent bin set. */
    template <typename Context>
    void remove(Context& C, const T& element) {
        int root_bin = C.bin();
        typedef Context::IndexerT ParentIndexT;
        typedef RateIndexer<IndexerBase, RateBinStorage, ParentIndexT> RIndexer;
        typedef RateContext<T, RIndexer, RateBinStorage, Context> RContext;

        RIndexer indexer;
        RContext context(*this, indexer, C);
        printf("** RATE BIN REMOVE:\n");
        storage.remove(context, element);
    }

    double r_total;
    Storage storage;
};


#endif
