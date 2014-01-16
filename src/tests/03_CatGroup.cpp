#include <vector>
#include <map>
#include "tests.h"

#include "dependencies/mtwist.h"

#include "../CatGroup.h"

using namespace std;

// Use an indexer we know is correct, so that we can
// independently test the CatGroup
struct SimpleIndexer {
    CatIndex lookup(int element) {
        printf("Return %d in %d:%d\n", element, elements[element].category, elements[element].index);
        return elements[element];
    }
    void store(int element, CatIndex index) {
        printf("Storing %d in %d:%d\n", element, index.category, index.index);
        elements[element] = index;
    }
    map<int, CatIndex> elements;
};

SUITE(CatGroup) {
    TEST(ints) {
        MemPool mem_pool;
        mem_pool.allocate(10000);
        const int CAPACITY = 1000;
        int* data = new int[CAPACITY];
        CatGroup<int, SimpleIndexer> group(data, SimpleIndexer(), CAPACITY);

        std::vector<int> contents;
        for (int i = 1; i < 100; i++) {
            printf("ADDING %d\n", i);
            group.add(mem_pool, i, i % 10);
        }

        for (int i = 1; i < 100; i++) {
            printf("REMOVING %d\n", i);
            group.remove(i);
        }
        CHECK(group.n_elems() == 0);
        group.print_cats();

        delete[] data;
    }
    TEST(entityid) {
        MemPool mem_pool;
        mem_pool.allocate(10000);
        const int CAPACITY = 1000;
        EntityID* data = new EntityID[CAPACITY];
        CatIndex* cat_data = new CatIndex[CAPACITY];
        CatEntityIndexer indexer(cat_data, CAPACITY);
        CatGroup<EntityID> group(data, indexer, CAPACITY);

        std::vector<int> contents;
        for (int i = 1; i < 100; i++) {
            printf("ADDING %d\n", i);
            group.add(mem_pool, EntityID(i), i % 10);
        }

        for (int i = 1; i < 100; i++) {
            printf("REMOVING %d\n", i);
            group.remove(EntityID(i));
        }
        CHECK(group.n_elems() == 0);
        group.print_cats();

        delete[] cat_data;
        delete[] data;
    }
}
