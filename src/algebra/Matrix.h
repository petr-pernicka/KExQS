#pragma once


#include <cstdlib>
#include <vector>
#include "../types.h"

namespace KQS::Algebra {

	/**
	 * Class representing a matrix of complex numbers.
	 */
	class ComplexMatrix {
	private:
		size_t fRows;
		size_t fColumns;
		std::vector<complex_t> fData;

	public:
		/**
		 * Creates new complex matrix with specified dimensions initialized to zeros.
		 * @param rows number of rows of the matrix
		 * @param columns number of columns of the matrix
		 */
		ComplexMatrix(size_t rows, size_t columns);

		/**
		 * Creates a complex matrix from 2D initializer list.
		 * @param list initializer list
		 */
		ComplexMatrix(std::initializer_list<std::initializer_list<complex_t>> list);

		size_t rows() const;

		size_t columns() const;

		const std::vector<complex_t> &data() const;

		complex_t operator[](size_t row, size_t column) const;

		void operator[](size_t row, size_t column, complex_t value);

		ComplexVector operator*(const ComplexVector &other) const;

		std::vector<complex_t> vector();
	};

}
