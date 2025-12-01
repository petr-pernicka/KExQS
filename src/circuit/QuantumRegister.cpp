#include <bitset>
#include <iomanip>
#include <cstdint>
#include <fstream>
#include "QuantumRegister.h"

namespace KQS::Circuit {

	QuantumRegister::QuantumRegister(size_t numberOfQubits)
			: fNumQubits(numberOfQubits), fNumStates(1 << numberOfQubits) {}

	QuantumRegister::~QuantumRegister() = default;

	size_t QuantumRegister::qubits() const {
		return fNumQubits;
	}

	std::string QuantumRegister::toString() const {
		auto vector = stateVector();

		std::stringstream ss;
		ss << std::fixed << std::setprecision(2);

		real_t probSum = 0;
		for (size_t i = 0; i < fNumStates; ++i) {
			real_t prob = std::norm(vector[i]);
			probSum += prob;

			std::string a = std::bitset<64>(i).to_string().substr(64 - fNumQubits);
			ss << '|' << a << ">: ";
			if (vector[i].real() >= 0)
				ss << ' ';

			char sign = vector[i].imag() >= 0 ? '+' : '-';
			ss << vector[i].real() << ' ' << sign << ' ' << std::abs(vector[i].imag())
			   << "i;  p(|" << a << ">) = " << prob << "\n";
		}
		ss << "Sum of probabilities: " << probSum << "\n";

		return ss.str();
	}

	void QuantumRegister::toFile(const std::string &fileName) const {
		std::vector<complex_t> vector = stateVector();
		std::ofstream outFile(fileName, std::ios::out | std::ios::binary);

		struct header_t {
			uint8_t padding[6];
			uint8_t precision;
			uint8_t numQubits;
		};

		header_t header{};
		header.precision = sizeof(real_t);
		header.numQubits = fNumQubits;

		outFile.write(reinterpret_cast<const char *>(&header), sizeof(header_t));
		outFile.write(reinterpret_cast<const char *>(vector.data()), vector.size() * sizeof(complex_t));
	}

	/////////////// Gates ///////////////

	void QuantumRegister::pauliX(size_t targetQubit) {
		applyOneQubitGate(QuantumLogicGate::pauliX(), targetQubit);
	}

	void QuantumRegister::pauliY(size_t targetQubit) {
		applyOneQubitGate(QuantumLogicGate::pauliY(), targetQubit);
	}

	void QuantumRegister::pauliZ(size_t targetQubit) {
		applyOneQubitGate(QuantumLogicGate::pauliZ(), targetQubit);
	}

	void QuantumRegister::controlledX(size_t controlQubit, size_t targetQubit) {
		applyTwoQubitGate(QuantumLogicGate::controlledX(), {targetQubit, controlQubit});
	}

	void QuantumRegister::controlledY(size_t controlQubit, size_t targetQubit) {
		applyTwoQubitGate(QuantumLogicGate::controlledY(), {targetQubit, controlQubit});
	}

	void QuantumRegister::controlledZ(size_t controlQubit, size_t targetQubit) {
		applyTwoQubitGate(QuantumLogicGate::controlledZ(), {targetQubit, controlQubit});
	}

	void QuantumRegister::hadamard(size_t targetQubit) {
		applyOneQubitGate(QuantumLogicGate::hadamard(), targetQubit);
	}

	void QuantumRegister::phase(size_t targetQubit, real_t phase) {
		applyOneQubitGate(QuantumLogicGate::phase(phase), targetQubit);
	}

	void QuantumRegister::controlledPhase(size_t controlQubit, size_t targetQubit, real_t phase) {
		applyTwoQubitGate(QuantumLogicGate::controlledPhase(phase), {targetQubit, controlQubit});
	}

	void QuantumRegister::piOverEight(size_t targetQubit) {
		applyOneQubitGate(QuantumLogicGate::piOverEight(), targetQubit);
	}

	void QuantumRegister::swap(size_t targetQubit1, size_t targetQubit2) {
		applyTwoQubitGate(QuantumLogicGate::swap(), {targetQubit2, targetQubit1});
	}

	void QuantumRegister::toffoli(size_t controlQubit1, size_t controlQubit2, size_t targetQubit) {
		applyKQubitGate(QuantumLogicGate::toffoli(), {targetQubit, controlQubit2, controlQubit1});
	}

	void QuantumRegister::gate(const QuantumLogicGate &gate, const std::vector<size_t> &targetQubits) {
		switch (targetQubits.size()) {
			case 1:
				applyOneQubitGate(gate, targetQubits[0]);
				break;
			case 2:
				applyTwoQubitGate(gate, {targetQubits[0], targetQubits[1]});
				break;
			default:
				applyKQubitGate(gate, targetQubits);
				break;
		}
	}

	/// Private methods ///

	void QuantumRegister::isNormalized() const {
		real_t sum = 0;
		for (const auto &state: stateVector())
			sum += std::norm(state);

		if (std::abs(sum - 1) > 1e-8)
			throw std::runtime_error("State vector is not normalized. Sum of probs: " + std::to_string(sum));
	}
}
