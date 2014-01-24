//#include <vector>
//#include <map>
//#include "tests.h"
//
//#include "dependencies/mtwist.h"
//
//#include "../BinSet.h"
//#include "../CatGrouper.h"
//
//using namespace std;
//
//SUITE(SimpleBinSet) {
//    struct SimpleContext {
//        MemPool pool;
//        SimpleContext () {
//            pool.allocate(10000);
//        }
//        MemPool& get_mem_pool() {
//            return pool;
//        }
//    };
//    static SimpleContext context;
//
//    struct SimpleIndexer {
//        int lookup(SimpleContext& context, int bin, EntityID element) {
//            return elements[element.slot_id];
//        }
//        double rate(SimpleContext& context, int bin) {
//            return bin; // Each rate is proportional to the bin
//        }
//        void store(SimpleContext& context, BinPosition index, EntityID element) {
//            elements[element.slot_id] = index.index;
//        }
//        map<int, int> elements;
//    };
//    typedef StoreData<EntityID, SimpleIndexer> StoreT;
//
//    TEST(add_remove) {
//        vector<EntityID> data(/*Capacity*/ 1000);
//        StoreLayer<EntityID, StoreT> bins;
//        bins.init(SimpleIndexer(), &data[0], data.size());
//
//        for (int i = 1; i < 100; i++) {
//            bins.add(context, EntityID(i), i % 10);
//        }
//
//        for (int i = 1; i < 100; i++) {
//            bins.remove(context, EntityID(i), i % 10);
//        }
//        CHECK(bins.n_elems() == 0);
//    }
//
//    TEST(rates_and_cohorts) {
//        const int INIT_BINS = 10;
//
//        vector<EntityID> data(/*Capacity*/ 1000);
//        StoreLayer<EntityID, StoreT> bins;
//        bins.init(SimpleIndexer(), &data[0], data.size());
//
//        /* Add two elements to every bin */
//        for (int i = 0; i < INIT_BINS * 2; i++) {
//            bins.add(context, EntityID(i), i % 10);
//        }
//
//        /* Ensure that the rates check out */
//        for (int i = 0; i < INIT_BINS; i++) {
//            CHECK(bins.get(i).r_total == i * 2);
//        }
//
//        CHECK(bins.n_bins() == INIT_BINS);
//        CHECK(bins.n_elems() == INIT_BINS * 2);
//
//        bins.shift_cohorts(context, Range(0, bins.n_bins()));
//        bins.print_bins();
//
//        /* Check that the cohort shift worked */
//        CHECK(bins.n_bins() == INIT_BINS + 1);
//        CHECK(bins.n_elems() == INIT_BINS * 2);
//
//        for (int i = 1; i < bins.n_bins(); i++) {
//            CHECK(bins.get(i).r_total == i * 2);
//        }
//
//        /* Remove everything from its new bin */
//        for (int i = 0; i < INIT_BINS * 2; i++) {
//            bins.remove(context, EntityID(i), (i % 10) + 1);
//        }
//        CHECK(bins.n_elems() == 0);
//    }
//}
