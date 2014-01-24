#ifndef INDEXERS_H_
#define INDEXERS_H_

#include "network.h"

struct HashIndexer {
    HashIndexer(int mask) : mask(mask) {
    }
    int index(Network& context, EntityID element) {
        return (element.slot_id & mask);
    }
    int mask;
};

#endif
