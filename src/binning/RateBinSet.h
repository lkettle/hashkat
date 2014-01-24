// RateBinSet's allow you to efficiently store and update a series of lists that
// organize a discrete set of elements that have associated action rates.
// Since the total content of the lists is
// known, and order is not important, the data-structure can efficiently
// store and move the elements in a fixed array.

#ifndef RATEBINSET_H_
#define RATEBINSET_H_

#include <vector>
#include <map>

#include <iostream>

#include "lcommon/Range.h"
#include "MemPoolVector.h"

#include "entity.h"
#include "util.h"
#include "network.h"

// Provides implementation to RateIndexer
struct MockIndexerBase {
    template <typename Context>
    int bin(Context& C, EntityID element) {
        return element.slot_id % 10;
    }

    template <typename Context>
    int index(Context& C, EntityID element) {
        return indices[element.slot_id];
    }

    template <typename Context>
    int rate(Context& C, BinPosition pos, EntityID element) {
        return element.slot_id % 10;
    }

    template <typename Context>
    void store(Context& context, BinPosition index, EntityID element) {
        indices[element.slot_id] = index.index;
    }

    std::map<int, int> indices;
};

// Based on a roughly fixed allocation of memory for all categories,
// and a simple shifting insert that does not guarantee any list order.
template <typename Storage = UnorderedBinStorage<RateBinInfo> >
struct RateBinSet {
    void add(Network& segment, const EntityID& element) {
        RootIndexer root_indexer;
        RootContext root_context(segment, root_indexer);
        storage.add();
    }

    void remove(Network& segment, const EntityID& element) {
        RootIndexer root_indexer;
        RootContext root_context(segment, root_indexer);
    }

    double get_total_rate() {
        return storage.r_total;
    }
private:
    // Parent indexer.
    // Note, unlike other Indexer's does not provide index() or rate().
    struct RootIndexer {
        RootIndexer(RateBinSet& self) : self(self) {
        }
        void rate_change(double delta) {
        }
        RateBinSet& self;
    };

    // Parent context.
    struct RootContext {
        typedef RootIndexer IndexerT;
        RootContext(Network& segment, IndexerT& indexer) :
            segment(segment), indexer(indexer) {
        }

        MemPool& mem_pool() {
            return segment.mem_pool();
        }

        Network& network_segment() {
            return segment;
        }

        Network& segment;
        IndexerT& indexer;

    };

    RateBinStorage<EntityID, MockIndexerBase, Storage> storage;
};

#endif
