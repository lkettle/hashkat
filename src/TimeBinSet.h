#ifndef CategoryGrouper 
#define CategoryGrouper 

#include <cstdlib>

#include "BinSet.h"
#include "network.h"

// Each category is defined with respect to a series of bounds
// and a categorization variable. This categorization variable can
// come from any source, and is fed into CategoryGroup to
// incrementally update a Categorization.

struct CategoryIndexer {
    CategoryIndexer(int* arr = NULL, int cap) {
        indices = arr;
        capacity = cap;
    }
    int lookup(NetworkSegment& context, int bin, EntityID element) {
        return indices[element.slot_id];
    }
    double rate(NetworkSegment& context, int bin) {
        return bin; // Each rate is proportional to the bin
    }
    void store(NetworkSegment& context, BinPosition index, EntityID element) {
        indices[element.slot_id] = index.index;
    }

    int* indices;
    int capacity;
};

struct CategoryGrouper {
    CategoryGrouper(int cap) {
        entity_arr = (EntityID*)malloc(cap);
        index_arr = (int*)malloc(cap);
        capacity = cap;
        bins.init(CategoryIndexer(index_arr, capacity));
    }

    ~CategoryGrouper(int max_entities) {
        free(index_arr);
    }

    void categorize(int entity, double parameter) {
        if (categorizations.size() <= entity) {
            // Make sure 'entity' is a valid index
            categorizations.resize(entity + 1);
        }
        BinPosition& c = categorizations.at(entity);
        for (int i = 0; i < categories.size(); i++) {
            CategoryEntityList& C = categories[i];
            if (parameter <= C.threshold) {
                if (i != c.bin) {
                    // We have to move ourselves into the new list
                    remove(c);
                    c = add(entity, i);
                }
                return; // Categorized, done.
            }
        }
        DEBUG_CHECK(false, "Logic error");
    }

    void remove(BinPosition& c) {
        if (c.bin != -1) {
            bins.remove(c, )
            // Reset:
            c = BinPosition();
        }
    }

    BinPosition add(int entity, int new_cat) {
        CategoryEntityList& C = categories.at(new_cat);
        C.entities.push_back(entity);
        return BinPosition(new_cat, C.entities.size() - 1);
    }

/* Implementation */
private:
    EntityID* entity_arr;
    int* index_arr;
    int capacity;

    typedef StoreData<EntityID, CategoryIndexer> StoreT;
    StoreLayer<EntityID, StoreT> bins;
};

#endif
