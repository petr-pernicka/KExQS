#pragma once


#include <memory>
#include "../circuit/QuantumRegister.h"

using namespace KQS::Circuit;

namespace KQS::Simulator {

	class Simulator {
	private:
		std::unique_ptr<QuantumRegister> fRegister;
		std::vector<size_t> fStatesCounts;

	public:
		explicit Simulator(std::unique_ptr<QuantumRegister> qRegister);

		void run(size_t numShots);
		std::string toString();
	};

}

