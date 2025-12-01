#include "VectorizedQuantumRegister.h"
#include "immintrin.h"

namespace KQS::Circuit {

	VectorizedQuantumRegister::VectorizedQuantumRegister(size_t i)
			: BasicQuantumRegister(i) {}

	void VectorizedQuantumRegister::applyOneQubitGate(const QuantumLogicGate &gate, size_t targetQubit) {
		size_t groups = fNumStates / 2;

		// prepare registers for matrix-vector multiplication
		__m256 m = _mm256_loadu_ps(reinterpret_cast<const float *>(gate.matrix().data().data()));

		__m128 low = _mm256_extractf128_ps(m, 0);  // [b_im, b_re, a_im, a_re]
		__m128 high = _mm256_extractf128_ps(m, 1); // [d_im, d_re, c_im, c_re]
		__m128 ac0 = _mm_shuffle_ps(low, high, _MM_SHUFFLE(1, 0, 1, 0)); // [c_im, c_re, a_im, a_re]
		__m128 bd0 = _mm_shuffle_ps(low, high, _MM_SHUFFLE(3, 2, 3, 2)); // [d_im, d_re, b_im, b_re]
		__m128 ac1 = _mm_shuffle_ps(low, high, _MM_SHUFFLE(0, 1, 0, 1)); // [c_re, c_im, a_re, a_im]
		__m128 bd1 = _mm_shuffle_ps(low, high, _MM_SHUFFLE(2, 3, 2, 3)); // [d_re, d_im, b_re, b_im]
		__m128 mask = _mm_set_ps(1, -1, 1, -1);
		ac1 = _mm_mul_ps(ac1, mask); // [c_re, -c_im, a_re, -a_im]
		bd1 = _mm_mul_ps(bd1, mask); // [d_re, -d_im, b_re, -b_im]

		// prepare registers for vectorized insertBit
		__m128i j = _mm_set_epi64x(1, 0);
		size_t mask2 = (1ULL << targetQubit) - 1;

		for (size_t i = 0; i < groups; ++i) {

			// vectorized insertBit function
			__m128i tmp = _mm_set1_epi64x(i & mask2);
			__m128i xx = _mm_set1_epi64x((i >> targetQubit) << 1);
			xx = _mm_or_si128(xx, j);
			xx = _mm_slli_epi64(xx, targetQubit);
			xx = _mm_or_si128(xx, tmp);

			alignas(16) size_t indices[2];
			_mm_store_si128((__m128i *) indices, xx);

			// complex matrix multiplication
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
		size_t groups = fNumStates / 4;

		if (targetQubits[0] > targetQubits[1])
			targetQubits[0]--;

		// prepare registers for matrix-vector multiplication
		const ComplexMatrix &m = gate.matrix();
		__m256 col0 = _mm256_set_ps(m[0, 0].real(), m[0, 0].imag(), m[1, 0].real(), m[1, 0].imag(),
									m[2, 0].real(), m[2, 0].imag(), m[3, 0].real(), m[3, 0].imag());
		__m256 col1 = _mm256_set_ps(m[0, 1].real(), m[0, 1].imag(), m[1, 1].real(), m[1, 1].imag(),
									m[2, 1].real(), m[2, 1].imag(), m[3, 1].real(), m[3, 1].imag());
		__m256 col2 = _mm256_set_ps(m[0, 2].real(), m[0, 2].imag(), m[1, 2].real(), m[1, 2].imag(),
									m[2, 2].real(), m[2, 2].imag(), m[3, 2].real(), m[3, 2].imag());
		__m256 col3 = _mm256_set_ps(m[0, 3].real(), m[0, 3].imag(), m[1, 3].real(), m[1, 3].imag(),
									m[2, 3].real(), m[2, 3].imag(), m[3, 3].real(), m[3, 3].imag());
		__m256 mask = _mm256_set_ps(-1, 1, -1, 1, -1, 1, -1, 1);
		__m256 col4 = _mm256_permute_ps(col0, 0b10'11'00'01);
		col4 = _mm256_mul_ps(col4, mask);
		__m256 col5 = _mm256_permute_ps(col1, 0b10'11'00'01);
		col5 = _mm256_mul_ps(col5, mask);
		__m256 col6 = _mm256_permute_ps(col2, 0b10'11'00'01);
		col6 = _mm256_mul_ps(col6, mask);
		__m256 col7 = _mm256_permute_ps(col3, 0b10'11'00'01);
		col7 = _mm256_mul_ps(col7, mask);


		std::array<size_t, 4> indices{};
		for (size_t i = 0; i < groups; ++i) {

			for (int j = 0; j < 4; ++j) {
				// combine i and j into a state
				size_t state = i;

				state = insertBitAtPosition(state, j & 1, targetQubits[0]);
				state = insertBitAtPosition(state, (j >> 1) & 1, targetQubits[1]);

				indices[j] = state;
			}

			__m256 x0_re = _mm256_set1_ps(fStateVector[indices[0]].real());
			__m256 x0_im = _mm256_set1_ps(fStateVector[indices[0]].imag());
			__m256 x1_re = _mm256_set1_ps(fStateVector[indices[1]].real());
			__m256 x1_im = _mm256_set1_ps(fStateVector[indices[1]].imag());
			__m256 x2_re = _mm256_set1_ps(fStateVector[indices[2]].real());
			__m256 x2_im = _mm256_set1_ps(fStateVector[indices[2]].imag());
			__m256 x3_re = _mm256_set1_ps(fStateVector[indices[3]].real());
			__m256 x3_im = _mm256_set1_ps(fStateVector[indices[3]].imag());

			__m256 y = _mm256_setzero_ps();
			y = _mm256_fmadd_ps(x0_re, col0, y);
			y = _mm256_fmadd_ps(x0_im, col4, y);
			y = _mm256_fmadd_ps(x1_re, col1, y);
			y = _mm256_fmadd_ps(x1_im, col5, y);
			y = _mm256_fmadd_ps(x2_re, col2, y);
			y = _mm256_fmadd_ps(x2_im, col6, y);
			y = _mm256_fmadd_ps(x3_re, col3, y);
			y = _mm256_fmadd_ps(x3_im, col7, y);

			float res[8];
			_mm256_storeu_ps(res, y);

			fStateVector[indices[0]] = complex_t(res[7], res[6]);
			fStateVector[indices[1]] = complex_t(res[5], res[4]);
			fStateVector[indices[2]] = complex_t(res[3], res[2]);
			fStateVector[indices[3]] = complex_t(res[1], res[0]);
		}
	}

	void VectorizedQuantumRegister::applyKQubitGate(const QuantumLogicGate &gate, const std::vector<size_t> &targetQubits) {
	}
}