#ifndef __UTIL_H_
#define __UTIL_H_

#include <cmath>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <limits>

#include <string>

// Mixed bag of useful functions

// Dirt simple error handling, exits the program:
inline void error_exit(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(2); // Return with error code
}

// Checks that are always on:
#define ASSERT(expr, msg) \
    if (!(expr)) { \
        printf("FAILED CHECK: %s (%s:%d)\n", /*Hack to take C++ strings too:*/ std::string(msg).c_str(), __FILE__, __LINE__); \
        throw msg; \
    }

// Checks that are turned off in release mode:
#define DEBUG_CHECK ASSERT

// XXX: NB: Not true for all compilers.
typedef int int32;
typedef long long int64;

template <typename T>
inline bool within_range(T val, T a, T b) {
	// Is val within [a,b) ?
	return val >= a && val < b;
}

#ifdef __GNUC__
// Only support prefetch for GCC
#define SPARSE_READ_PREFETCH(x) __builtin_prefetch(x, 0, 0)
#define SPARSE_WRITE_PREFETCH(x) __builtin_prefetch(x, 1, 0)
#else
#define SPARSE_READ_PREFETCH(x)
#define SPARSE_WRITE_PREFETCH(x)
#endif

inline std::vector<double> parse_numlist(std::string s) {
	std::vector<double> ret;
	std::stringstream sstream(s);
	while (!sstream.eof()) {
		double val;
		sstream >> val;
		ret.push_back(val);
	}
	return ret;
}

// Generic function for using a vector as a simple memory allocator
// Takes a vector of any type, and returns a pointer to a memory region
// of the desired size.
//
// WARNING: All vector memory *must* be preallocated with reserve()!
// Failure to do so will result in a program-terminating failure.
//
// Reason: this allocation scheme relies on the fact that a vector
// will keep a contiguous block of memory as long as there is room
// in its buffer.
template <typename T>
inline T* allocate(std::vector<T>& vec, int block) {
    int old_size = vec.size();
    int new_size = old_size + block;
    ASSERT(new_size < vec.capacity(), "allocate: Not enough reserved memory for requested allocation!");
    vec.resize(new_size);
    return &vec[block];
}

inline double parse_num(std::string s) {
    double ret = 0.0;
    std::stringstream string_converter(s); //** Think of this like 'cin', but instead of standard input its an arbitrary string.
    string_converter >> ret; //** Will adapt to the appropriate type! also like 'cin' would.
    return ret;
}

#define LOG(msg, ...) \
    printf(msg, ...);

typedef std::vector<double> NumVec;

#endif
