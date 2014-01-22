///* Used as part of BinSet. */
//template <typename T, typename IndexerT, typename ChildT, int INIT_BINS = 8>
//struct CatLayer : public AbstractLayer<T, RateInfo, INIT_BINS> {
//    CatLayer(const IndexerT& indexer, const ChildT& child) :
//        AbstractLayer<T, RateInfo, IndexerT, INIT_BINS>(indexer), child(child) {
//    }
//
//    template <typename Context>
//    double add(Context& context, const T& element, int bin) {
//        int sub = sub_bin(element, bin);
//        double rate_diff = child.add(context, element, sub);
//
//        RateInfo& info = this->get(sub);
//        info.rate += rate_diff;
//
//        return rate_diff;
//    }
//
//    template <typename Context>
//    double remove(Context& context, const T& element, int bin) {
//        int sub = sub_bin(element, bin);
//        double rate_diff = child.remove(context, element, sub);
//
//        RateInfo& info = this->get(sub);
//        info.rate -= rate_diff;
//
//        return rate_diff;
//    }
//
//private:
//    ChildT child;
//};
