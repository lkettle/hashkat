#ifndef NETWORK_H_
#define NETWORK_H_

#include "lcommon/Range.h"

#include "MemPoolVector.h"

struct Entity {

};

struct NetworkStorage {
    Entity* entities;
    MemPool mem_pool;
};

struct NetworkSegmentCats {

};

struct NetworkSegment {
    Range entity_range;
};

struct NetworkBlock {

};


#endif /* NETWORK_H_ */
