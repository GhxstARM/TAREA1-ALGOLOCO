// Multiplicacion clasica de matrices cuadradas.
// Referencia: Cormen, Leiserson, Rivest y Stein, Introduction to Algorithms,
// 3rd ed., MIT Press, 2009. https://mitpress.mit.edu/9780262033848/
// Fecha de consulta: 2026-09-04. Esta implementacion fue escrita para esta tarea.

#include "matrix_algorithms.hpp"

#include <stdexcept>

namespace {

std::size_t validateSquareCompatible(const Matrix& left, const Matrix& right) {
    if (left.empty() || right.empty()) {
        throw std::invalid_argument("Las matrices no pueden estar vacias");
    }
    const std::size_t size = left.size();
    if (right.size() != size) {
        throw std::invalid_argument("Las matrices deben tener la misma dimension");
    }
    for (const auto& row : left) {
        if (row.size() != size) {
            throw std::invalid_argument("La matriz izquierda debe ser cuadrada");
        }
    }
    for (const auto& row : right) {
        if (row.size() != size) {
            throw std::invalid_argument("La matriz derecha debe ser cuadrada");
        }
    }
    return size;
}

}  // namespace

Matrix naiveMultiply(const Matrix& left, const Matrix& right) {
    const std::size_t size = validateSquareCompatible(left, right);
    Matrix product(size, std::vector<long long>(size, 0));
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            for (std::size_t k = 0; k < size; ++k) {
                product[i][j] += left[i][k] * right[k][j];
            }
        }
    }
    return product;
}
