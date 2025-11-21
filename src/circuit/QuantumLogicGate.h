#pragma once


#include <cstdlib>
#include "../algebra/Matrix.h"

using namespace KQS::Algebra;

namespace KQS::Circuit {

	class QuantumLogicGate {
	private:
		size_t fDimension;
		ComplexMatrix fMatrix;

	public:
		explicit QuantumLogicGate(const ComplexMatrix &matrix);

		const ComplexMatrix &matrix() const;

		static QuantumLogicGate pauliX();
		static QuantumLogicGate pauliY();
		static QuantumLogicGate pauliZ();
		static QuantumLogicGate controlledX();
		static QuantumLogicGate controlledY();
		static QuantumLogicGate controlledZ();
		static QuantumLogicGate hadamard();
		static QuantumLogicGate phase(real_t phase);
		static QuantumLogicGate controlledPhase(real_t phase);
		static QuantumLogicGate piOverEight();
		static QuantumLogicGate swap();
		static QuantumLogicGate toffoli();

		static QuantumLogicGate makeControlled(const QuantumLogicGate &gate, size_t numberOfControlQubits);
	};

}
