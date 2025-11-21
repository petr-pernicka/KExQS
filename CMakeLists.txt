cmake_minimum_required(VERSION 3.22)
project(semestralka)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -march=native")

find_package(OpenCL REQUIRED)

add_executable(semestralka main.cpp
        src/Constants.h
        src/types.h
        src/circuit/QuantumLogicGate.cpp src/circuit/QuantumLogicGate.h
        src/algebra/Matrix.cpp src/algebra/Matrix.h
        src/circuit/QuantumRegister.cpp src/circuit/QuantumRegister.h
        src/simulator/Simulator.cpp src/simulator/Simulator.h
        src/circuit/CLQuantumRegister.cpp src/circuit/CLQuantumRegister.h
        src/circuit/BasicQuantumRegister.cpp src/circuit/BasicQuantumRegister.h
        src/circuit/VectorizedQuantumRegister.cpp src/circuit/VectorizedQuantumRegister.h
        src/circuit/VectorizedQuantumRegister.cpp src/circuit/VectorizedQuantumRegister.h)

target_include_directories(semestralka PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(semestralka PRIVATE OpenCL::OpenCL)
