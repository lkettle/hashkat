#ifndef ENTITYCATS_H_
#define ENTITYCATS_H_

#include "lcommon/Range.h"

#include "entity.h"

struct HashIndexer {
    template <typename Context>
    int get_index(Context& context, Range r, EntityID id) {
        int span = (r.max - r.min);
        return (id.slot_id % span)  + r.min;
    }
};

#endif
