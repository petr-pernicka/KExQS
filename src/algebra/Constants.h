#pragma once


#include "../types.h"

using namespace std::complex_literals;

namespace KQS::Algebra {
	/** Pi. */
	constexpr real_t PI = 3.141592653589793238;

	/** Reciprocal square root of 2, i.e. 1/sqrt(2). */
	constexpr real_t RecSqrt2 = 0.707106781186547524;

	/** The imaginary unit. It is cast into the type of complex_t. */
	constexpr complex_t I = static_cast<complex_t>(1i);
}
