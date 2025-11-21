#pragma once

#include <vector>
#include <array>
#include "CL/opencl.hpp"
#include "../types.h"
#include "QuantumLogicGate.h"
#include "QuantumRegister.h"

namespace KQS::Circuit {
	class CLQuantumRegister : public QuantumRegister {
	protected:
		cl::Buffer fStateVector;

		cl::Context fContext;
		cl::CommandQueue fQueue;
		cl::Program fKernels;

	public:
		CLQuantumRegister(size_t numberOfQubits, const cl::Context &context, const cl::Device &device);

		void setStateVector(const std::vector<complex_t> &stateVector) override;
		std::vector<complex_t> stateVector() const override;

	protected:
		void applyOneQubitGate(const QuantumLogicGate &gate, size_t targetQubit) override;
		void applyTwoQubitGate(const QuantumLogicGate &gate, std::array<size_t, 2> targetQubits) override;
		void applyKQubitGate(const QuantumLogicGate &gate, const std::vector<size_t> &targetQubits) override;
	};
}
