#ifndef MATRIX_ALGORITHMS_HPP
#define MATRIX_ALGORITHMS_HPP

#include <vector>

using Matrix = std::vector<std::vector<long long>>;

Matrix naiveMultiply(const Matrix& left, const Matrix& right);
Matrix strassenMultiply(const Matrix& left, const Matrix& right);

#endif  // MATRIX_ALGORITHMS_HPP
