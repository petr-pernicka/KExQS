#include <bitset>
#include <iostream>
#include <random>
#include <chrono>
#include <map>

#include "src/circuit/BasicQuantumRegister.h"
#include "src/circuit/CLQuantumRegister.h"
#include "src/circuit/VectorizedQuantumRegister.h"
#include "src/algebra/Constants.h"
#include "CL/opencl.hpp"

using namespace KQS;

cl::Device getDevice(cl_device_type type) {
	cl::Device device;
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	for (const auto &platform: platforms) {
		std::vector<cl::Device> devices;
		platform.getDevices(type, &devices);

		if (!devices.empty())
			device = devices[0];
	}
	std::cout << "Selected device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

	return device;
}

std::vector<complex_t> randomStateVector(size_t numQubits) {
	std::vector<complex_t> stateVector(1 << numQubits);
	std::uniform_real_distribution<real_t> uniform01(0, 1);
	std::random_device rng;

	real_t sum = 0;
	for (auto &i : stateVector) {
		i = complex_t(uniform01(rng), uniform01(rng));
		sum += std::norm(i);
	}

	for (auto &i : stateVector)
		i /= sum;

	return stateVector;
}

void speedTest() {
	size_t numQubits = 29;
	std::cout << "State vetor size: " << (1 << numQubits) * sizeof(complex_t) / 1024 / 1024 << " MB" << std::endl;
	std::cout << "Number of states: " << (1 << numQubits) << std::endl;

	cl::Device device = getDevice(CL_DEVICE_TYPE_GPU);
	cl::Context context(device);

	std::cout << "Generating random state vector..." << std::endl;
	auto stateVector = randomStateVector(numQubits);

	std::map<std::string, std::unique_ptr<Circuit::QuantumRegister>> registers;
	registers["BasicQuantumRegister"] = std::make_unique<Circuit::BasicQuantumRegister>(numQubits);
	registers["CLQuantumRegister"] = std::make_unique<Circuit::CLQuantumRegister>(numQubits, context, device);
	registers["VectorizedQuantumRegister"] = std::make_unique<Circuit::VectorizedQuantumRegister>(numQubits);

	for (auto &[name, qRegister] : registers) {
		qRegister->setStateVector(stateVector);

		std::cout << "Timing " << name << std::endl;

		auto tick = std::chrono::system_clock::now();
		for (size_t i = 0; i < qRegister->qubits(); ++i)
			qRegister->hadamard(i);

		auto tock = std::chrono::system_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(tock - tick) << std::endl;
	}
}

int main() {
	// speedTest();
	// exit(0);

	size_t numQubits = 4;
	std::cout << "Number of states: " << (1 << numQubits) << std::endl;

	auto qRegister = std::make_unique<Circuit::BasicQuantumRegister>(numQubits);

	for (size_t i = 0; i < qRegister->qubits(); ++i)
		qRegister->hadamard(i);

	qRegister->controlledX(1, 2);
	qRegister->controlledZ(0, 3);
	qRegister->controlledY(1, 3);
	qRegister->controlledPhase(0, 1, PI / 3);
	qRegister->controlledX(1, 2);
	qRegister->toffoli(2, 1, 3);

	std::cout << qRegister->toString() << std::endl;
	qRegister->toFile("vector.bin");
}
