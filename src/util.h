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

// Mixed bag of useful functions

// Dirt simple error handling, exits the program:
inline void error_exit(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(2); // Return with error code
}

// Checks that are always on:
#define ASSERT(expr, msg) \
    if (!(expr)) { \
        printf("FAILED CHECK: %s (%s:%d)\n", msg, __FILE__, __LINE__); \
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
