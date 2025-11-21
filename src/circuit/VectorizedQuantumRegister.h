#pragma once

#include "BasicQuantumRegister.h"

namespace KQS::Circuit {
	class VectorizedQuantumRegister : public BasicQuantumRegister {

	public:
		explicit VectorizedQuantumRegister(size_t i);

	protected:
		void applyOneQubitGate(const QuantumLogicGate &gate, size_t targetQubit) override;
		void applyTwoQubitGate(const QuantumLogicGate &gate, std::array<size_t, 2> targetQubits) override;
		void applyKQubitGate(const QuantumLogicGate &gate, const std::vector<size_t> &targetQubits) override;
	};
}
