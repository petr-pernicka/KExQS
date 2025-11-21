#pragma once

#include <cstdlib>
#include <vector>
#include <array>
#include "../types.h"
#include "QuantumLogicGate.h"
#include "QuantumRegister.h"

namespace KQS::Circuit {
	class BasicQuantumRegister : public QuantumRegister {
	protected:
		std::vector<complex_t> fStateVector;

	public:
		explicit BasicQuantumRegister(size_t numberOfQubits);

		void setStateVector(const std::vector<complex_t> &stateVector) override;
		std::vector<complex_t> stateVector() const override;

	protected:
		void applyOneQubitGate(const QuantumLogicGate &gate, size_t targetQubit) override;
		void applyTwoQubitGate(const QuantumLogicGate &gate, std::array<size_t, 2> targetQubits) override;
		void applyKQubitGate(const QuantumLogicGate &gate, const std::vector<size_t> &targetQubits) override;

		static size_t insertBitAtPosition(size_t x, size_t bit, size_t position);
	};
}
