#ifndef ENTITY_H_
#define ENTITY_H_

struct EntityID {
    int slot_id;
    /* The amount of times this slot has been reused. */
    int slot_iter;
    explicit EntityID(int slot_id = -1, int slot_iter = -1) {
        this->slot_id = slot_id;
        this->slot_iter = slot_iter;
    }

    bool operator==(const EntityID& o) const {
        return slot_id == o.slot_id && slot_iter == o.slot_iter;
    }
};

#endif
