#ifndef NETWORK_H_
#define NETWORK_H_

#include <vector>

#include "lcommon/Range.h"

#include "MemPoolVector.h"

#include "util.h"
#include "entity.h"

struct EntityTypeStorage {

};

struct NetworkStorage {
//    std::vector<NetworkSegment> segments;
    std::vector<Entity> entity_data;
//    std::vector<EntityTypeSegment> entity_cats;
    MemPool mem_pool;
};

struct EntityCats {

    // Storage for entity categorization data
};

struct Network {
    Network(NetworkStorage* storage) :
            storage(storage) {
    }

    int start_slot() {
        return _start_slot;
    }
    int size() {
        return n_entity;
    }

    Entity& operator[](EntityID slot) {
        return entity_data[slot.slot_id];
    }

    MemPool mem_pool() {
        return _mem_pool;
    }
private:
    /* Saved as member for convenience*/
    NetworkStorage* storage;
    MemPool _mem_pool;

    int _start_slot;
    int n_entity;
    int cap_entity;
    // Storage for entity data
    Entity* entity_data;
    // Storage for entity categorization data
    EntityCats* entity_cats;
};

//struct Network {
//    Network(int max_entity, int segment_size) {
//        ASSERT(max_entity % segment_size == 0, "Network::Network: max_entities must be divisible by segment size!");
//        int n_segments = (max_entity / segment_size);
//        int n_entity_per_segment = (max_entity / n_segments);
//        for (int i = 0; i < n_segments; i++) {
//            segments.push_back(NetworkSegment(&storage));
//        }
//    }
//
//    NetworkStorage storage;
//    std::vector<NetworkSegment> segments;
//};


#endif /* NETWORK_H_ */
