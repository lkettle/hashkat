#ifndef CATGROUP_H_
#define CATGROUP_H_

#include <vector>

#include "util.h"

// Each category is defined with respect to a series of bounds
// and a categorization variable. This categorization variable can
// come from any source, and is used with a CatGroup to
// incrementally update a CatIndex.

struct CatIndex {
	int category, index;
	CatIndex(int g = -1, int i = -1) {
		category = g, index = i;
	}
};

// Only used as return result, for convenience
template <typename T>
struct CatList {
    CatList(T* data, int size) {
        this->data = data;
        this->size = size;
    }

    T* data;
    int size;
};

// Based on a roughly fixed allocation of memory for all categories,
// and a simple shifting insert that does not guarantee any list order.
template <typename T, int INIT_CATS = 8>
struct CatGroup {
    CatGroup(T* d, int cap) {
        data = d;
        capacity = cap;
    }
    size_t size() const {
        return cats.size;
    }
    CatList<T> get(size_t index) {
        ASSERT(index < cats.size, "Bounds error");
        int min = 0, max = cats[index];
        if (index > 0) {
            min = cats[index];
        }
        return CatList<T>(data + min, max - min);
    }

    void add(const T& element, CatIndex cat_index) {
        ASSERT(cats[size()-1] < capacity, "Capacity logic error");
        for (int c = size() - 1; c <= capacity; c++) {
            CatList<T> list = get(c);
            // Shift back element to end
            list[list.size] = ;
            list.data++;
            list[list.size - 1] =
        }
    }
    T remove(CatIndex cat_index) {

    }
    T* data;
    T* capacity;
    struct CatInfo {
        int bound;
    };
    MemPoolVector<CatInfo, INIT_CATS> cats;
};

template <typename T, int IA = 8>
CatIndex categorize(CatGroup<T, IA>& group, const NumVec& thresh, CatIndex cat_index, double x) {
    ASSERT(thresh.size() >= 1, "Logic error");
    int c = cat_index.category;
    while (true) {
        double curr_min = (c >= 1 ? thresh[c-1] : 0);
        double curr_max = thresh[c];
        if (curr_min) {
            c--;
        }
    }
    if (c == cat_index.category) {
        return cat_index;
    }
    // Otherwise, we must recategorize
    T removed = group.remove(cat_index);
    CatIndex new_index(c, cat_index.category);
    group.add(cat_index);
    return new_index;
}

//struct CategoryEntityList {
//	// Upper bound that fits into this category
//	double threshold, /*Optional*/ prob;
//	std::vector<int> entities;
//	CategoryEntityList(double t, double p = 0) {
//		threshold = t, prob = p;
//	}
//	size_t size() const {
//	    return entities.size();
//	}
//	int operator[](size_t idx) const {
//	    return entities[idx];
//	}
//};
//
//template <typename T>
//struct CategoryGrouper {
//	std::vector<Cat> categorizations;
//	std::vector<CategoryEntityList> categories;
//	CategoryGrouper() {
//
//	}
//	// Incrementally update a categorization
//	void categorize(int entity, double parameter) {
//		if (categorizations.size() <= entity) {
//			// Make sure 'entity' is a valid index
//			categorizations.resize(entity + 1);
//		}
//		Cat& c = categorizations.at(entity);
//		for (int i = 0; i < categories.size(); i++) {
//			CategoryEntityList& C = categories[i];
//			if (parameter <= C.threshold) {
//				if (i != c.category) {
//					// We have to move ourselves into the new list
//					remove(c);
//					c = add(entity, i);
//				}
//				return; // Categorized, done.
//			}
//		}
//		DEBUG_CHECK(false, "Logic error");
//	}
//
//	void remove(Cat& c) {
//		if (c.category != -1) {
//			CategoryEntityList& C = categories.at(c.category);
//			DEBUG_CHECK(within_range(c.index, 0, (int)C.entities.size()), "Logic error");
//			// Move the element at the top to our index:
//			Cat& c_top = categorizations[C.entities.back()];
//			c_top.index = c.index;
//			C.entities[c.index] = C.entities.back();
//			C.entities.pop_back();
//			// Reset:
//			c = Cat();
//		}
//	}
//
//	Cat add(int entity, int new_cat) {
//		CategoryEntityList& C = categories.at(new_cat);
//		C.entities.push_back(entity);
//		return Cat(new_cat, C.entities.size() - 1);
//	}
//};

#endif
