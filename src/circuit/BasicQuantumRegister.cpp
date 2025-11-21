#include <format>
#include "BasicQuantumRegister.h"

namespace KQS::Circuit {

	BasicQuantumRegister::BasicQuantumRegister(size_t numberOfQubits)
			: QuantumRegister(numberOfQubits), fStateVector(fNumStates) {
		fStateVector[0] = 1;
	}

	void BasicQuantumRegister::setStateVector(const std::vector<complex_t> &stateVector) {
		fStateVector = stateVector;
	}

	std::vector<complex_t> BasicQuantumRegister::stateVector() const {
		return fStateVector;
	}

	void BasicQuantumRegister::applyOneQubitGate(const QuantumLogicGate &gate, size_t targetQubit) {
		if (targetQubit >= fNumQubits)
			throw std::runtime_error(
					std::format("Cannot apply gate to qubit {} in {}-qubit register", targetQubit, fNumStates));

		size_t groups = fNumStates / 2;

		ComplexVector group(2);
		std::array<size_t, 2> indices{};
		for (size_t i = 0; i < groups; ++i) {

			for (size_t j = 0; j < 2; ++j) {
				// combine i and j into a state
				size_t state = i;

				state = insertBitAtPosition(state, j, targetQubit);

				indices[j] = state;
				group[j] = fStateVector[state];
			}

			ComplexVector result = gate.matrix() * group;

			fStateVector[indices[0]] = result[0];
			fStateVector[indices[1]] = result[1];
		}
	}

	void BasicQuantumRegister::applyTwoQubitGate(const QuantumLogicGate &gate, std::array<size_t, 2> targetQubits) {
		if (targetQubits[0] >= fNumQubits)
			throw std::runtime_error(
					std::format("Cannot apply gate to qubit {} in {}-qubit register", targetQubits[0], fNumStates));
		if (targetQubits[1] >= fNumQubits)
			throw std::runtime_error(
					std::format("Cannot apply gate to qubit {} in {}-qubit register", targetQubits[1], fNumStates));

		size_t groups = fNumStates / 4;

		if (targetQubits[0] > targetQubits[1])
			targetQubits[0]--;

		ComplexVector group(4);
		std::array<size_t, 4> indices{};
		for (size_t i = 0; i < groups; ++i) {

			for (int j = 0; j < 4; ++j) {
				// combine i and j into a state
				size_t state = i;

				state = insertBitAtPosition(state, j & 1, targetQubits[0]);
				state = insertBitAtPosition(state, (j >> 1) & 1, targetQubits[1]);

				indices[j] = state;
				group[j] = fStateVector[state];
			}

			ComplexVector result = gate.matrix() * group;

			for (int j = 0; j < 4; ++j)
				fStateVector[indices[j]] = result[j];
		}
	}

	void BasicQuantumRegister::applyKQubitGate(const QuantumLogicGate &gate, const std::vector<size_t> &targetQubits) {
		size_t groupSize = 1 << targetQubits.size(); // group size is 2^qubits
		size_t groups = fNumStates / groupSize;

		std::vector<size_t> targetQubitsModified(targetQubits.size());
		for (size_t i = 0; i < targetQubits.size(); ++i) {
			size_t pos = targetQubits[i];

			for (size_t j = i + 1; j < targetQubits.size(); ++j)
				if (targetQubits[j] < targetQubits[i])
					pos--;

			targetQubitsModified[i] = pos;
		}


		ComplexVector group(groupSize);
		std::vector<size_t> indices(groupSize);
		for (size_t i = 0; i < groups; ++i) {

			for (size_t j = 0; j < groupSize; ++j) {
				// combine i and j into a state
				size_t state = i;
				for (size_t k = 0; k < targetQubits.size(); ++k) {
					size_t bit = (j >> k) & 1; // k-th bit in j
					size_t position = targetQubitsModified[k];
					state = insertBitAtPosition(state, bit, position);
				}

				indices[j] = state;
				group[j] = fStateVector[state];
			}

			ComplexVector result = gate.matrix() * group;

			for (size_t j = 0; j < groupSize; ++j)
				fStateVector[indices[j]] = result[j];
		}
	}

	size_t BasicQuantumRegister::insertBitAtPosition(size_t x, size_t bit, size_t position) {
		size_t mask = (1ULL << position) - 1; // mask, where first `position` bits are ones
		size_t tmp = x & mask; // first `position` bits of x

		x = x >> position;
		x = x << 1;
		x = x | bit;
		x = x << position;
		x = x | tmp;

		return x;
	}
}
