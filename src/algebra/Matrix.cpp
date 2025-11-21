#include <format>
#include "Matrix.h"

namespace KQS::Algebra {

	ComplexMatrix::ComplexMatrix(size_t rows, size_t columns)
			: fRows(rows), fColumns(columns), fData(rows * columns) {}

	ComplexMatrix::ComplexMatrix(std::initializer_list<std::initializer_list<complex_t>> list) {

		fRows = list.size();
		if (fRows == 0) {
			fColumns = 0;
			fData = std::vector<complex_t>(0);
			return;
		}

		fColumns = list.begin()->size();
		fData = std::vector<complex_t>(fRows * fColumns);

		for (size_t i = 0; const auto &row: list) {
			if (row.size() != fColumns)
				throw std::runtime_error("All rows must have the same number of columns");

			for (size_t j = 0; const auto &value: row) {
				fData[i * fColumns + j] = value;
				++j;
			}
			++i;
		}
	}

	size_t ComplexMatrix::rows() const {
		return fRows;
	}

	size_t ComplexMatrix::columns() const {
		return fColumns;
	}

	const std::vector<complex_t> &ComplexMatrix::data() const {
		return fData;
	}

	complex_t ComplexMatrix::operator[](size_t row, size_t column) const {
		if (row >= fRows)
			throw std::runtime_error(std::format("Row {} is out of range for matrix of size {}x{}", row, fRows, fColumns));
		if (column >= fColumns)
			throw std::runtime_error(std::format("Column {} is out of range for matrix of size {}x{}", column, fRows, fColumns));

		return fData[row * fColumns + column];
	}

	void ComplexMatrix::operator[](size_t row, size_t column, complex_t value) {
		if (row >= fRows)
			throw std::runtime_error(std::format("Row {} is out of range for matrix of size {}x{}", row, fRows, fColumns));
		if (column >= fColumns)
			throw std::runtime_error(std::format("Column {} is out of range for matrix of size {}x{}", column, fRows, fColumns));

		fData[row * fColumns + column] = value;
	}

	ComplexVector ComplexMatrix::operator*(const ComplexVector &other) const {
		if (other.size() != fColumns) {
			throw std::runtime_error(std::format("Vector of size {} cannot be multiplied by matrix of size {}x{}",
												 other.size(), fRows, fColumns));
		}
		ComplexVector result(fRows);

		for (size_t i = 0; i < fRows; ++i) {

			complex_t value = 0;
			for (size_t j = 0; j < fColumns; ++j)
				value += operator[](i, j) * other[j];
			result[i] = value;
		}

		return result;
	}

	std::vector<complex_t> ComplexMatrix::vector() {
		return fData;
	}
}
