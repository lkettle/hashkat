//#ifndef CATGROUPER_H_
//#define CATGROUPER_H_
//
//#include <cstdlib>
//
//#include "util.h"
//
//#include "BinSet.h"
//#include "network.h"
//
//// Each category is defined with respect to a series of bounds
//// and a categorization variable. This categorization variable can
//// come from any source, and is fed into CategoryGroup to
//// incrementally update a Categorization.
//
//template <typename CatConfigT>
//struct CatIndexer {
//    CatIndexer(BinPosition* arr = NULL, int cap = 0) {
//        indices = arr;
//        capacity = cap;
//    }
//    int lookup(NetworkSegment& context, int bin, EntityID element) {
//        return indices[element.slot_id].index;
//    }
//    double rate(NetworkSegment& context, int bin) {
//        return bin; // Each rate is proportional to the bin
//    }
//    void store(NetworkSegment& context, BinPosition index, EntityID element) {
//        indices[element.slot_id] = index;
//    }
//
//    int get_bin(EntityID element) {
//        return indices[element.slot_id].bin;
//    }
//
//    int get_category(EntityID element) {
//        return CatConfigT::get_category(element);
//    }
//
//    BinPosition* indices;
//    int capacity;
//};
//
//template <typename CatConfigT>
//struct CatGrouper {
//    CatGrouper(int cap) {
//        entity_arr = (EntityID*) malloc(cap);
//        index_arr = (BinPosition*) malloc(cap);
//        capacity = cap;
//        CatIndexer<CatConfigT> indexer(index_arr, capacity);
//        bins.init(indexer, entity_arr, capacity);
//    }
//
//    ~CatGrouper() {
//        free(index_arr);
//    }
//
//    double categorize(NetworkSegment& context, EntityID entity) {
//        ASSERT(entity.slot_id > 0 && entity.slot_id < capacity, "CatGrouper::categorize out of bounds");
//
//        double delta = 0.0;
//        CatIndexer<CatConfigT>& indexer = bins.get_indexer();
//
//        /* Remove if already added */
//        BinPosition& c = indexer.indices[entity.slot_id];
//        if (c.bin != -1) {
//            delta += bins.remove(context, entity, c.bin);
//        }
//
//        /* Add and return effect on rate */
//        delta += bins.add(context, entity, indexer.get_category(entity));
//        return delta;
//    }
//
///* Implementation */
//private:
//    EntityID* entity_arr;
//    BinPosition* index_arr;
//    int capacity;
//
//    typedef StoreData<EntityID, CatIndexer<CatConfigT> > StoreT;
//    StoreLayer<EntityID, StoreT> bins;
//};
//
//#endif
