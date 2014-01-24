#ifndef UNORDEREDBINSET_H_
#define UNORDEREDBINSET_H_

#include "util.h"

#include "UnorderedBinStorage.h"

//
//template <typename T, typename Storage = UnorderedBinStorage<RateBinInfo>, typename Backbone = MemPoolBackbone<T> >
//struct RateBinSet : public Backbone {
//    template <typename Parent>
//    struct Context : public ForwardingContext<T, Parent> {
//        Context(RateBinSet& self, Parent& P) :
//                self(self), ForwardingContext<T, Parent>(P) {
//        }
//        int& bound(int bin) {
//            return self.storage[bin].bound;
//        }
//        RateBinSet& self;
//    };
//
//    template <typename Parent>
//    void add(Parent& P, const T& element) {
//        Context<Parent> C(*this, P);
//        int bin = P.get_bin(element);
//    }
//
//    Storage storage;
//};

#endif

