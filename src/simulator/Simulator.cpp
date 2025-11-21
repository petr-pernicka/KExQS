#include <utility>
#include <format>
#include <bitset>
#include <iomanip>
#include <random>
#include "Simulator.h"

std::uniform_real_distribution<real_t> uniform01(0, 1);
std::random_device rng;

namespace KQS::Simulator {

	Simulator::Simulator(std::unique_ptr<QuantumRegister> qRegister)
			: fRegister(std::move(qRegister)), fStatesCounts(1 << fRegister->qubits()) {}

	bool measure(const std::vector<complex_t> &stateVector, size_t qubit) {
		real_t prob = 0; // probability of one state
		for (size_t i = 0; i < stateVector.size(); ++i)
			if ((i >> qubit & 1) == 1)
				prob += std::norm(stateVector[i]);

		real_t R = uniform01(rng);

		return R < prob;
	}

	void Simulator::run(size_t numShots) {

		for (size_t n = 0; n < numShots; ++n) {

			size_t i = 0;
			for (size_t b = 0; b < fRegister->qubits(); ++b) {
				if (measure(fRegister->stateVector(), b))
					i = i | (1 << b);
			}

			++fStatesCounts[i];
		}
	}

	std::string Simulator::toString() {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2);

		size_t total = 0;
		for (size_t count: fStatesCounts)
			total += count;

		for (size_t i = 0; i < fStatesCounts.size(); ++i) {
			std::string ket = std::format("|{}>", std::bitset<64>(i).to_string().substr(64 - fRegister->qubits()));
			real_t fraction = (real_t) fStatesCounts[i] / (real_t) total * 100;
			ss << ket << ": " << fStatesCounts[i] << " (" << fraction << "%)\n";
		}

		return ss.str();
	}
}