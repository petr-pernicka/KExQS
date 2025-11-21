#include "VectorizedQuantumRegister.h"
#include "immintrin.h"

namespace KQS::Circuit {

	VectorizedQuantumRegister::VectorizedQuantumRegister(size_t i)
			: BasicQuantumRegister(i) {}

	void VectorizedQuantumRegister::applyOneQubitGate(const QuantumLogicGate &gate, size_t targetQubit) {
		size_t groups = fNumStates / 2;

		// prepare registers for matrix-vector multiplication
		__m256 m = _mm256_loadu_ps(reinterpret_cast<const float *>(gate.matrix().data().data()));

		__m128 mask = _mm_set_ps(1, -1, 1, -1);
		__m128 low = _mm256_extractf128_ps(m, 0);  // [b_im, b_re, a_im, a_re]
		__m128 high = _mm256_extractf128_ps(m, 1); // [d_im, d_re, c_im, c_re]
		__m128 ac0 = _mm_shuffle_ps(low, high, _MM_SHUFFLE(1, 0, 1, 0)); // [c_im, c_re, a_im, a_re]
		__m128 bd0 = _mm_shuffle_ps(low, high, _MM_SHUFFLE(3, 2, 3, 2)); // [d_im, d_re, b_im, b_re]
		__m128 ac1 = _mm_shuffle_ps(low, high, _MM_SHUFFLE(0, 1, 0, 1)); // [c_re, c_im, a_re, a_im]
		__m128 bd1 = _mm_shuffle_ps(low, high, _MM_SHUFFLE(2, 3, 2, 3)); // [d_re, d_im, b_re, b_im]
		ac1 = _mm_mul_ps(ac1, mask); // [c_re, -c_im, a_re, -a_im]
		bd1 = _mm_mul_ps(bd1, mask); // [d_re, -d_im, b_re, -b_im]

		__m128i j = _mm_set_epi64x(1, 0);
		size_t mask2 = (1ULL << targetQubit) - 1;

		for (size_t i = 0; i < groups; ++i) {

			__m128i tmp = _mm_set1_epi64x(i & mask2);
			__m128i xx = _mm_set1_epi64x((i >> targetQubit) << 1);
			xx = _mm_or_si128(xx, j);
			xx = _mm_slli_epi64(xx, targetQubit);
			xx = _mm_or_si128(xx, tmp);

			alignas(16) size_t indices[2];
			_mm_store_si128((__m128i *) indices, xx);

			__m128 x_re = _mm_set1_ps(fStateVector[indices[0]].real());
			__m128 x_im = _mm_set1_ps(fStateVector[indices[0]].imag());
			__m128 y_re = _mm_set1_ps(fStateVector[indices[1]].real());
			__m128 y_im = _mm_set1_ps(fStateVector[indices[1]].imag());

			__m128 z;
			z = _mm_mul_ps(ac0, x_re);      // ac0 * x_re
			z = _mm_fmadd_ps(ac1, x_im, z); // ac0 * x_re + ac1 * x_im
			z = _mm_fmadd_ps(bd0, y_re, z); // ac0 * x_re + ac1 * x_im + bd0 * y_re
			z = _mm_fmadd_ps(bd1, y_im, z); // ac0 * x_re + ac1 * x_im + bd0 * y_re + bd1 * y_im

			alignas(16) float res[4];
			_mm_store_ps(res, z);

			fStateVector[indices[0]] = complex_t(res[0], res[1]);
			fStateVector[indices[1]] = complex_t(res[2], res[3]);
		}
	}

	void VectorizedQuantumRegister::applyTwoQubitGate(const QuantumLogicGate &gate, std::array<size_t, 2> targetQubits) {
	}

	void VectorizedQuantumRegister::applyKQubitGate(const QuantumLogicGate &gate, const std::vector<size_t> &targetQubits) {
	}
}