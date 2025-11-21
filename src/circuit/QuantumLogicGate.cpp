#include "QuantumLogicGate.h"
#include "../Constants.h"

namespace KQS::Circuit {

	QuantumLogicGate::QuantumLogicGate(const ComplexMatrix &matrix)
			: fDimension(matrix.columns()), fMatrix(matrix) {
		if (fMatrix.rows() != fMatrix.columns())
			throw std::runtime_error("Cannot create quantum logic gate from non-square matrix");
		// TODO check if matrix is unitary
	}

	const ComplexMatrix &QuantumLogicGate::matrix() const {
		return fMatrix;
	}

	/// Pauli gates ///

	QuantumLogicGate QuantumLogicGate::pauliX() {
		ComplexMatrix matrix = {
				{0, 1},
				{1, 0}
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::pauliY() {
		constexpr auto i = KQS::Constants::I;
		ComplexMatrix matrix = {
				{0, -i},
				{i, 0}
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::pauliZ() {
		ComplexMatrix matrix = {
				{1, 0},
				{0, -1}
		};

		return QuantumLogicGate(matrix);
	}

	/// Controlled Pauli gates ///

	QuantumLogicGate QuantumLogicGate::controlledX() {
		ComplexMatrix matrix = {
				{1, 0, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 0, 1},
				{0, 0, 1, 0},
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::controlledY() {
		constexpr auto i = KQS::Constants::I;
		ComplexMatrix matrix = {
				{1, 0, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 0, -i},
				{0, 0, i, 0},
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::controlledZ() {
		ComplexMatrix matrix = {
				{1, 0, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 1, 0},
				{0, 0, 0, -1},
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::hadamard() {
		ComplexMatrix matrix = {
				{KQS::Constants::recSqrt2, KQS::Constants::recSqrt2},
				{KQS::Constants::recSqrt2, -KQS::Constants::recSqrt2}
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::phase(real_t phase) {
		complex_t p = std::exp(phase * KQS::Constants::I);
		ComplexMatrix matrix = {
				{1, 0},
				{0, p}
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::controlledPhase(real_t phase) {
		complex_t p = std::exp(phase * KQS::Constants::I);
		ComplexMatrix matrix = {
				{1, 0, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 1, 0},
				{0, 0, 0, p},
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::piOverEight() {
		complex_t p = std::exp(KQS::Constants::PI / 4 * KQS::Constants::I);
		ComplexMatrix matrix = {
				{1, 0},
				{0, p}
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::swap() {
		ComplexMatrix matrix = {
				{1, 0, 0, 0},
				{0, 0, 1, 0},
				{0, 1, 0, 0},
				{0, 0, 0, 1},
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::toffoli() {
		ComplexMatrix matrix = {
				{1, 0, 0, 0, 0, 0, 0, 0},
				{0, 1, 0, 0, 0, 0, 0, 0},
				{0, 0, 1, 0, 0, 0, 0, 0},
				{0, 0, 0, 1, 0, 0, 0, 0},
				{0, 0, 0, 0, 1, 0, 0, 0},
				{0, 0, 0, 0, 0, 1, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 1},
				{0, 0, 0, 0, 0, 0, 1, 0},
		};

		return QuantumLogicGate(matrix);
	}

	QuantumLogicGate QuantumLogicGate::makeControlled(const QuantumLogicGate &gate, size_t numberOfControlQubits) {
		size_t new_size = gate.fDimension << numberOfControlQubits;
		ComplexMatrix matrix(new_size, new_size);

		for (size_t i = 0; i < new_size - gate.fDimension; ++i)
			matrix[i, i] = 1;

		for (size_t i = 0; i < gate.fDimension; ++i)
			for (size_t j = 0; j < gate.fDimension; ++j)
				matrix[new_size - gate.fDimension + i, new_size - gate.fDimension + j] = gate.fMatrix[i, j];

		return QuantumLogicGate(matrix);
	}
}
