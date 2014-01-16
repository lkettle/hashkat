#include <vector>
#include "tests.h"

#include "../CatGroup.h"

using namespace std;

SUITE(CatGroup) {
    TEST(correctness) {
        MemPool mem_pool;
        mem_pool.allocate(10000);
        const int CAPACITY = 1000;
        int* data = new int[CAPACITY];
        CatGroup<int> group(data, CAPACITY);

        vector<CatIndex> vec;
        for (int i = 0; i < 200; i++) {
            for (int i = 0; i < 10; i++) {
                vec.push_back(group.add(mem_pool, i, i));
            }
        }
        for (int i = 0; i < vec.size(); i++) {
            printf("%d\n", i);
            int elem = group.remove(vec[i]);
            printf("i: %d elem %d\n", i, elem);
        }

        group.print_cats();

        delete[] data;
    }
}
