#if REAL_PRECISION == 1
typedef float real_t;
#elif REAL_PRECISION == 2
typedef double real_t;
#elif REAL_PRECISION == 3
typedef long double real_t;
#endif

typedef struct { // maps directly to complex_t on host
	real_t re;
	real_t im;
} complex_t;

inline complex_t c_add(complex_t a, complex_t b) {
	complex_t result;
	result.re = a.re + b.re;
	result.im = a.im + b.im;
	return result;
}

inline complex_t c_mul(complex_t a, complex_t b) {
	complex_t result;
	result.re = a.re * b.re - a.im * b.im;
	result.im = a.re * b.im + a.im * b.re;
	return result;
}

inline size_t insertBitAtPosition(size_t x, size_t bit, size_t position) {
	size_t mask = ((1ULL << position) - 1); // mask, where first `position` bits are ones
	size_t tmp = x & mask; // first `position` bits of x

	x = x >> position;
	x = x << 1;
	x = x | bit;
	x = x << position;
	x = x | tmp;

	return x;
}

__kernel void applyOneQubitGate(__global complex_t *stateVector, ulong targetQubit,
								complex_t gate00, complex_t gate01, complex_t gate10, complex_t gate11) {
	size_t i = get_global_id(0);

	complex_t group[2];
	size_t indices[2];

	for (int j = 0; j < 2; ++j) {
		// combine i and j into a state
		size_t state = i;
		state = insertBitAtPosition(state, j, targetQubit);

		indices[j] = state;
		group[j] = stateVector[state];
	}

	complex_t result0 = c_add(c_mul(gate00, group[0]), c_mul(gate01, group[1]));
	complex_t result1 = c_add(c_mul(gate10, group[0]), c_mul(gate11, group[1]));

	stateVector[indices[0]] = result0;
	stateVector[indices[1]] = result1;
}
