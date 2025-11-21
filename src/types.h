#pragma once

#include <complex>
#include <iostream>

/** The type representing real numbers throughout the project. */
using real_t = float;

/** The type representing complex numbers throughout the project. */
using complex_t = std::complex<real_t>;

/** The type representing vector of complex numbers. */
using ComplexVector = std::vector<complex_t>;

/** Macro for checking for OpenCL errors. */
#define CL_CHECK(err) \
    if (err != CL_SUCCESS) { \
        std::cerr << "OpenCL error at " << __FILE__ << ":" << __LINE__ << " (" << err << ")\n"; \
        std::exit(EXIT_FAILURE); \
    }

