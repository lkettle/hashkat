#ifndef CAT_GROUPS_H_
#define CAT_GROUPS_H_

#include <vector>

#include "util.h"

#include "network.h"

#include <google/sparse_hash_set>

//
//struct RateBinner : PointerBackbone<RateBinInfo> {
//    RateBinner(RateBinInfo* data, int n_cats = 0) {
//        init(data, n_cats);
//    }
//    void has_room(int cat) {
//
//    }
//    void grow(int cat) {
//        get(cat).bound++;
//    }
//
//    void shrink(int cat) {
//        get(cat).bound--;
//    }
//};
//
//struct HashBinner : PointerBackbone<Range> {
//    HashBinner(Range* data, int n_cats, int cap) {
//        init(data, n_cats);
//        capacity = cap;
//    }
//    void has_room(int cat) {
//        return
//    }
//    void grow(int cat) {
//        get(cat).max++;
//    }
//
//    void shrink(int cat) {
//        get(cat).max--;
//    }
//
//    int capacity;
//};
//
//template <typename T>
//struct UnorderedBinner {
//    // Create an instance when the unordered binning algorithm is desired
//    UnorderedBinner(T* data, int cap, Binner& binner, int bin) :
//            data(data), capacity(cap), binner(binner), bin(bin) {
//    }
//    /* Add an element to the specified bin.
//     * Assumes the element does not exist in the bin.
//     * Only intended for use by a parent bin set. */
//    template <typename Context>
//    double add(Context& C, const T& element) {
//        // Create newly freed space in bins after this one:
//        for (int b = C->n_bins() - 1; b >= bin + 1; b--) {
//            Range r = C->index_range(b);
//            // Move front element to after the back end
//            if (!r.empty()) {
//                set(C, r.max, data(r.min));
//            }
//        }
//
//        for (int b = bin; b < binner.size(); b++) {
//            C.bound(b)++;
//        }
//
//        set(C, BinPosition(bin, C.bound(bin) - 1), element);
//    }
//
//    /* Add an element to the specified bin.
//     * Assumes the element does not exist in the bin.
//     * Only intended for use by a parent bin set. */
//    template <typename Context>
//    void remove(Context& C, const T& element, int index) {
//        C.on_remove(C, bp, element);
//
//        // Overwrite deleted slot with end element
//        relocate(C, BinPosition(bin, C.bound(bin) - 1), index);
//
//        // Use newly freed space for bin after this one:
//        for (int b = bin + 1; b < this->n_bins(); b++) {
//            Range r = C.index_range(b);
//            // Move back element to before the front element
//            if (!r.empty()) {
//                relocate(C, BinPosition(b, r.max - 1), this->get(r.min - 1));
//            }
//        }
//        // Adjust end-bounds of the bins
//        for (int b = ci.bin; b < this->n_bins(); b++) {
//            C.bound(b)--;
//        }
//    }
//
//    T* data;
//    int capacity;
//    Binner& binner;
//    int bin;
//};
//
//// Follower lists are a bin data-structure that ends with a hashed back-end
//// TDR == Time-Dependent-Rates
//template <typename T, typename Parent>
//struct HashLayer {
//
//    HashLayer(int mask) {
//
//    }
//
//    double index() {
//
//    }
//};

//template <typename T>
//struct CatBins {
//
//    Range index_range(int bin) {
//        return index_range(Range(bin, bin));
//    }
//
//    Range index_range(Range r) {
//        DEBUG_CHECK(r.max - 1 < n_bins() && r.min < n_bins(), "BinSet out-of-bins access");
//        int min = 0, max = bins[r.max-1].bound;
//        if (r.min > 0) {
//            min = bins[r.min-1].bound;
//        }
//        return Range(min, max);
//    }
//
//    size_t n_elems() {
//        Range r = index_range(Range(0,bins.size()-1));
//        return (r.max - r.min);
//    }
//
//    // Number of categories
//    size_t n_bins() {
//        return bins.size();
//    }
//
//    void ensure_exists(MemPool& mem_pool, int category) {
//        while (n_bins() <= category) {
//            add_bin(mem_pool);
//        }
//    }
//
//    struct CatBinInfo {
//        int bound; // Last index referenced by this bin
//        double r_total; // Total over bin
//        T sub_cats;
//        CatBinInfo(int bound = 0) {
//            this->bound = bound, this->r_total = 0;
//        }
//    };
//
//    size_t size() {
//        return bins.size();
//    }
//
//    T& operator[](int bin) {
//        return bins[bin];
//    }
//private:
//    std::vector<T> bins;
//};

namespace cats {

template <typename Elem>
struct CatLeafNode {
    int n_elems() {
        return elems.size();
    }

    double total_rate; // Total rate of the leaf node
    google::sparse_hash_set<Elem> elems;
};

template <typename SubCat>
struct CatTreeNode {
    int n_elems() {
        return _n_elems;
    }

    int _n_elems;
    double total_rate; // Total weight of the subtree
    std::vector<SubCat> cats;
};

// Leaf node
template <typename T>
bool add_element(CatLeafNode<T>& tree, double& rate) {

}

// Takes: rate, the rate to update
template <typename SubCat>
bool add_element(CatTreeNode<SubCat>& tree, double& rate) {

}

}

//struct CatBins {

//typedef PointerList<EntityID> EntityList;
//
//struct HashBins {
//    /* Main operations: */
//    void add_bin(MemPool& mem_pool) {
//        int prev = bins.size() > 0 ? bins[bins.size() - 1].range.min : 0;
//        bins.append(mem_pool, HashBinInfo(Range(prev, prev)));
//    }
//
//    Range index_range(int bin) {
//        return bins[bin].range;
//    }
//
//    // NOTE: may include empty regions.
//    Range index_range(Range r) {
//        DEBUG_CHECK(r.max < n_bins() && r.min < n_bins(), "BinSet out-of-bins access");
//        int min = bins[r.min].range.min;
//        int max = bins[r.max - 1].range.max;
//        return Range(min, max);
//    }
//
//    size_t n_elems() {
//        Range r = index_range(Range(0,bins.size()-1));
//        return (r.max - r.min);
//    }
//
//    // Number of categories
//    size_t n_bins() {
//        return bins.size();
//    }
//
//    void ensure_exists(MemPool& mem_pool, int category) {
//        while (n_bins() <= category) {
//            add_bin(mem_pool);
//        }
//    }
//
//    template <typename R>
//    double remove(EntityList data, MemPool& mem_pool, const R& rates, const EntityID& element) {
//        double rate = rates.get_rate(); // Should be fully-determined by this point
//    }
//
//    template <typename R>
//    double add(EntityList data, MemPool& mem_pool, const R& rates, const EntityID& element) {
//        double rate = rates.get_rate(); // Should be fully-determined by this point
//
//    }
//
//    void recategorize(MemPool& mem_pool, EntityList data, Range cap) {
//        std::vector<char>& buf = mem_pool.buffer;
//        int len = (cap.max - cap.min) * sizeof(EntityID);
//        buf.resize(len);
//        memcpy((void*)&buf[0], (void*)&data[0], len);
//    }
//
//    struct HashBinInfo {
//        Range range;
//        HashBinInfo(Range r = Range()) {
//            range = r;
//        }
//    };
//

//    HashBinInfo& get(int bin) {
//        return bins[bin];
//    }
//private:
//    int mask;
//    MemPoolList<HashBinInfo> bins;
//};
//
//// Model as a flow of cohorts
//struct TimeEntityCat {
//    TimeEntityCat(EntityList data) : data(data) {
//
//    }
//
//    template <typename R>
//    void add(MemPool& mem_pool, const R& rates, const EntityID& element) {
////        printf("** ADD:\n");
////        this->ensure_exists(context, bin);
////
////        // Create newly freed space in bins after this one:
////        for (int c = this->n_bins() - 1; c >= bin + 1; c--) {
////            Range r = this->index_range(c);
////            // Move front element to after the back end
////            if (!r.empty()) {
////                this->move(context, c, r.min, r.max);
////            }
////        }
////
////        for (int c = bin; c < this->n_bins(); c++) {
////            this->get(c).bound++;
////        }
////
////        this->set(context, BinPosition(bin, this->get(bin).bound - 1), element);
////        double delta = CatOps::add(mem_pool, )
////        r_total += delta;
////
////        printf("**POST ADD:\n");
////        return delta;
//    }
//
//    double get_rate_total() {
//        return r_total;
//    }
//
//private:
//    EntityList data;
//    CatBins<CatBins<CatBins<HashBins> > > cats;
//    std::vector<HashBins> bins;
//};

#endif
