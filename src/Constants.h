#pragma once


#include "types.h"

using namespace std::complex_literals;

namespace KQS::Constants {
	constexpr real_t PI = 3.141592653589793238;
	constexpr real_t recSqrt2 = 0.707106781186547524;

	/** The imaginary unit. It is cast into the type of complex_t. */
	constexpr complex_t I = static_cast<complex_t>(1i);
}
