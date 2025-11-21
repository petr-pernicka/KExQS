#pragma once

#include <cstdlib>
#include <vector>
#include <array>
#include "../types.h"
#include "QuantumLogicGate.h"

namespace KQS::Circuit {
	class QuantumRegister {
	protected:
		size_t fNumQubits;
		size_t fNumStates;

	public:
		explicit QuantumRegister(size_t numberOfQubits);
		virtual ~QuantumRegister();

		virtual void setStateVector(const std::vector<complex_t> &stateVector) = 0;

		size_t qubits() const;
		virtual std::vector<complex_t> stateVector() const = 0;
		std::string toString() const;
		void toFile(const std::string &fileName) const;

		void pauliX(size_t targetQubit);
		void pauliY(size_t targetQubit);
		void pauliZ(size_t targetQubit);
		void controlledX(size_t controlQubit, size_t targetQubit);
		void controlledY(size_t controlQubit, size_t targetQubit);
		void controlledZ(size_t controlQubit, size_t targetQubit);
		void hadamard(size_t targetQubit);
		void phase(size_t targetQubit, real_t phase);
		void controlledPhase(size_t controlQubit, size_t targetQubit, real_t phase);
		void piOverEight(size_t targetQubit);
		void swap(size_t targetQubit1, size_t targetQubit2);
		void toffoli(size_t controlQubit1, size_t controlQubit2, size_t targetQubit);
		void gate(const QuantumLogicGate &gate, const std::vector<size_t> &targetQubits);

		void isNormalized() const;

	protected:
		virtual void applyOneQubitGate(const QuantumLogicGate &gate, size_t targetQubit) = 0;
		virtual void applyTwoQubitGate(const QuantumLogicGate &gate, std::array<size_t, 2> targetQubits) = 0;
		virtual void applyKQubitGate(const QuantumLogicGate &gate, const std::vector<size_t> &targetQubits) = 0;
	};
}
