// Multiplicacion de matrices por el algoritmo de Strassen.
// Referencia: Volker Strassen, "Gaussian Elimination is Not Optimal",
// Numerische Mathematik 13, 354-356, 1969. DOI: 10.1007/BF02165411.
// https://eudml.org/doc/131927. Fecha de consulta: 2026-09-04.
// Esta implementacion fue escrita para esta tarea.

#include "matrix_algorithms.hpp"

#include <stdexcept>

namespace {

constexpr std::size_t kNaiveThreshold = 64;

bool isPowerOfTwo(std::size_t value) {
    return value != 0 && (value & (value - 1)) == 0;
}

void validateStrassenInput(const Matrix& left, const Matrix& right) {
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
    if (!isPowerOfTwo(size)) {
        throw std::invalid_argument("Strassen requiere una dimension potencia de dos");
    }
}

Matrix add(const Matrix& left, const Matrix& right) {
    const std::size_t size = left.size();
    Matrix result(size, std::vector<long long>(size));
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            result[i][j] = left[i][j] + right[i][j];
        }
    }
    return result;
}

Matrix subtract(const Matrix& left, const Matrix& right) {
    const std::size_t size = left.size();
    Matrix result(size, std::vector<long long>(size));
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            result[i][j] = left[i][j] - right[i][j];
        }
    }
    return result;
}

Matrix multiplyRecursive(const Matrix& left, const Matrix& right) {
    const std::size_t size = left.size();
    if (size <= kNaiveThreshold) {
        return naiveMultiply(left, right);
    }

    const std::size_t half = size / 2;
    Matrix a11(half, std::vector<long long>(half));
    Matrix a12 = a11, a21 = a11, a22 = a11;
    Matrix b11 = a11, b12 = a11, b21 = a11, b22 = a11;
    for (std::size_t i = 0; i < half; ++i) {
        for (std::size_t j = 0; j < half; ++j) {
            a11[i][j] = left[i][j];
            a12[i][j] = left[i][j + half];
            a21[i][j] = left[i + half][j];
            a22[i][j] = left[i + half][j + half];
            b11[i][j] = right[i][j];
            b12[i][j] = right[i][j + half];
            b21[i][j] = right[i + half][j];
            b22[i][j] = right[i + half][j + half];
        }
    }

    const Matrix m1 = multiplyRecursive(add(a11, a22), add(b11, b22));
    const Matrix m2 = multiplyRecursive(add(a21, a22), b11);
    const Matrix m3 = multiplyRecursive(a11, subtract(b12, b22));
    const Matrix m4 = multiplyRecursive(a22, subtract(b21, b11));
    const Matrix m5 = multiplyRecursive(add(a11, a12), b22);
    const Matrix m6 = multiplyRecursive(subtract(a21, a11), add(b11, b12));
    const Matrix m7 = multiplyRecursive(subtract(a12, a22), add(b21, b22));

    const Matrix c11 = add(subtract(add(m1, m4), m5), m7);
    const Matrix c12 = add(m3, m5);
    const Matrix c21 = add(m2, m4);
    const Matrix c22 = add(subtract(add(m1, m3), m2), m6);
    Matrix product(size, std::vector<long long>(size));
    for (std::size_t i = 0; i < half; ++i) {
        for (std::size_t j = 0; j < half; ++j) {
            product[i][j] = c11[i][j];
            product[i][j + half] = c12[i][j];
            product[i + half][j] = c21[i][j];
            product[i + half][j + half] = c22[i][j];
        }
    }
    return product;
}

}  // namespace

Matrix strassenMultiply(const Matrix& left, const Matrix& right) {
    validateStrassenInput(left, right);
    return multiplyRecursive(left, right);
}
