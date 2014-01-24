#ifndef CONTEXTS_H_
#define CONTEXTS_H_

/* Template boilerplate necessary to chain bin functionality
 * with a high degree of detail.
 *
 */
template <typename T, typename Parent>
struct ForwardingContext {
    ForwardingContext(Parent& P) : P(P) {
    }

    MemPool& mem_pool() {
        return P.mem_pool();
    }

    Network& network_segment() {
        return P.network_segment();
    }

    // Remove from a bin.
    void on_remove(BinPosition pos, const T& element) {
        P.on_remove(pos, element);
    }
    // Move, inside the same bin.
    void on_relocate(BinPosition pos, const T& element) {
        P.on_relocate(pos, element);
    }

    // Add to new bin.
    void on_add(BinPosition pos, const T& element) {
        P.on_add(pos, element);
    }

    Parent& P;
};


#endif
