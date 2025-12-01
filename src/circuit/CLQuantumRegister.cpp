#include <fstream>
#include "CLQuantumRegister.h"

namespace KQS::Circuit {

	std::string readFileToString(const char *path) {
		std::ifstream f(path);
		if (f.fail())
			throw std::runtime_error("Failed to open file " + std::string(path));

		std::stringstream ss;
		ss << f.rdbuf();

		return ss.str();
	}

	CLQuantumRegister::CLQuantumRegister(size_t numberOfQubits, const cl::Context &context, const cl::Device &device)
			: QuantumRegister(numberOfQubits), fContext(context) {
		cl_int err;

		fQueue = cl::CommandQueue(context, device, cl::QueueProperties::None, &err);
		CL_CHECK(err)

		std::vector<complex_t> init(fNumStates);
		init[0] = 1;
		fStateVector = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
								  fNumStates * sizeof(complex_t), init.data(), &err);
		CL_CHECK(err)

		std::string source = readFileToString("cl/CLQuantumRegister.cl");
		fKernels = cl::Program(context, source, false, &err);
		CL_CHECK(err)

		if constexpr (std::is_same_v<real_t, float>)
			err = fKernels.build("-cl-std=CL3.0 -D REAL_PRECISION=1");
		if constexpr (std::is_same_v<real_t, double>)
			err = fKernels.build("-cl-std=CL3.0 -D REAL_PRECISION=2");
		if constexpr (std::is_same_v<real_t, long double>)
			err = fKernels.build("-cl-std=CL3.0 -D REAL_PRECISION=3");

		if (err != CL_SUCCESS) {
			std::string log = fKernels.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
			std::cerr << log << std::endl;
		}
		CL_CHECK(err)
	}

	void CLQuantumRegister::setStateVector(const std::vector<complex_t> &stateVector) {

	}

	std::vector<complex_t> CLQuantumRegister::stateVector() const {
		std::vector<complex_t> result(fNumStates);
		cl_int err = fQueue.enqueueReadBuffer(fStateVector, CL_TRUE, 0, fNumStates * sizeof(complex_t), result.data());
		CL_CHECK(err)
		return result;
	}

	void CLQuantumRegister::applyOneQubitGate(const QuantumLogicGate &gate, size_t targetQubit) {
		size_t groups = fNumStates / 2;

		cl::Kernel kernel(fKernels, "applyOneQubitGate");
		kernel.setArg(0, fStateVector);
		kernel.setArg(1, targetQubit);
		kernel.setArg(2, gate.matrix()[0, 0]);
		kernel.setArg(3, gate.matrix()[0, 1]);
		kernel.setArg(4, gate.matrix()[1, 0]);
		kernel.setArg(5, gate.matrix()[1, 1]);

		cl_int err = fQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(groups));
		CL_CHECK(err)
	}

	void CLQuantumRegister::applyTwoQubitGate(const QuantumLogicGate &gate, std::array<size_t, 2> targetQubits) {

	}

	void CLQuantumRegister::applyKQubitGate(const QuantumLogicGate &gate, const std::vector<size_t> &targetQubits) {

	}
}
