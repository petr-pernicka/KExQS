# KEx Quantum Computer Simulator
This is an implementation of a quantum computer simulator written in C++. Its main 
advantage are optimized functions for application quantum logic gates on the state vector.

There are three implementations of quantum register (all inheriting from `QuantumRegister`):
- sequential `BasicQuantumRegister`,
- `CLQuantumRegister` parallelized for GPUs using OpenCL,
- and `VectorizedQuantumRegister` using SIMD instructions.

Only one-qubit gates are parallelized in `CLQuantumRegister` and `VectorizedQuantumRegister`
for now. `BasicQuantumRegister` is fully functional.

## Performance
Performance test was performed with registers of 29 qubits. In this setting, the state
vector has 536'870'912 states and takes up 4096 MB (when using floats).
- Computer specs: Windows 10, AMD Ryzen 5 1600X, RTX 3060 Ti
- Compiler: Clang 18.1.8

First test was performed on the following piece of code:
```c++
for (size_t i = 0; i < qRegister->qubits(); ++i)
	qRegister->hadamard(i);
```

Results:

| Implementation              | Time  |
|-----------------------------|-------|
| `BasicQuantumRegister`      | 610s  |
| `VectorizedQuantumRegister` | 36s   |
| `CLQuantumRegister`         | 692ms |

Second on the following:
```c++
for (size_t i = 1; i < qRegister->qubits(); ++i)
	qRegister->controlledX(0, i);
```

Results:

| Implementation              | Time  |
|-----------------------------|-------|
| `BasicQuantumRegister`      | 375s  |
| `VectorizedQuantumRegister` | 55s   |
| `CLQuantumRegister`         | 726ms |

## Example
File `main.cpp` contains an example of usage:
```c++
size_t numQubits = 4;
std::cout << "Number of states: " << (1 << numQubits) << std::endl;

auto qRegister = std::make_unique<Circuit::BasicQuantumRegister>(numQubits);

for (size_t i = 0; i < qRegister->qubits(); ++i)
	qRegister->hadamard(i);

qRegister->controlledX(1, 2);
qRegister->controlledZ(0, 3);
qRegister->controlledY(1, 3);
qRegister->controlledPhase(0, 1, Constants::PI / 3);
qRegister->controlledX(1, 2);
qRegister->toffoli(2, 1, 3);

std::cout << qRegister->toString() << std::endl;
```
Its Qiskit equivalent should look like:
```python
import math
from qiskit import QuantumCircuit
from qiskit.quantum_info import Statevector

qc = QuantumCircuit(4)
qc.h(range(4))
qc.cx(1, 2)
qc.cz(0, 3)
qc.cy(1, 3)
qc.cp(math.pi / 3, 0, 1)
qc.cx(1, 2)
qc.ccx(2, 1, 3)

sv = Statevector.from_instruction(qc)

for i in range(len(sv.data)):
    print(f"{sv.data[i]:.2f}")
```
You can try that both provide the same result:
```c++
Number of states: 16
|0000>:  0.25 + 0.00i;  p(|0000>) = 0.06
|0001>:  0.25 + 0.00i;  p(|0001>) = 0.06
|0010>:  0.00 - 0.25i;  p(|0010>) = 0.06
|0011>: -0.22 + 0.12i;  p(|0011>) = 0.06
|0100>:  0.25 + 0.00i;  p(|0100>) = 0.06
|0101>:  0.25 + 0.00i;  p(|0101>) = 0.06
|0110>:  0.00 + 0.25i;  p(|0110>) = 0.06
|0111>: -0.22 + 0.12i;  p(|0111>) = 0.06
|1000>:  0.25 + 0.00i;  p(|1000>) = 0.06
|1001>: -0.25 + 0.00i;  p(|1001>) = 0.06
|1010>:  0.00 + 0.25i;  p(|1010>) = 0.06
|1011>: -0.22 + 0.12i;  p(|1011>) = 0.06
|1100>:  0.25 + 0.00i;  p(|1100>) = 0.06
|1101>: -0.25 + 0.00i;  p(|1101>) = 0.06
|1110>:  0.00 - 0.25i;  p(|1110>) = 0.06
|1111>: -0.22 + 0.12i;  p(|1111>) = 0.06
Sum of probabilities: 1.00
```