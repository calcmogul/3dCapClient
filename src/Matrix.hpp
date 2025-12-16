// Copyright (c) Tyler Veness

#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

template <class T>
class Matrix;

template <class T>
std::ostream& operator<<(std::ostream&, const Matrix<T>&);

/// A utility library for manipulating matrices
template <class T>
class Matrix {
public:
    Matrix(size_t height, size_t width, bool initAsIdent = false)
        : m_height(height), m_width(width) {
        m_matrix = std::make_unique<T[]>(height * width);

        if (initAsIdent) {
            // Create identity matrix
            for (uint32_t i = 0; i < m_height; i++) {
                for (uint32_t j = 0; j < m_width; j++) {
                    if (i == j) {
                        operator()(i, j) = 1;
                    } else {
                        operator()(i, j) = 0;
                    }
                }
            }
        }
    }

    // NOLINTNEXTLINE
    Matrix(T rhs)
        : m_matrix(std::make_unique<T[]>(1)), m_height(1), m_width(1) {
        m_matrix[0] = rhs;
    }

    Matrix(const Matrix<T>& rhs)
        : m_height(rhs.height()), m_width(rhs.width()) {
        m_matrix = std::make_unique<T[]>(m_height * m_width);

        std::memcpy(m_matrix.get(), rhs.m_matrix.get(),
                    m_height * m_width * sizeof(T));
    }

    /// throws std::domain_error with dim mismatch
    Matrix<T>& operator=(const Matrix<T>& rhs) {
        if (this != &rhs) {
            if (height() != rhs.height() || width() != rhs.width()) {
#if 0
            throw std::domain_error(
                      "Domain error: arguments have incompatible dimensions");
#else
                m_matrix = std::make_unique<T[]>(rhs.height() * rhs.width());

                m_height = rhs.height();
                m_width = rhs.width();
#endif
            }

            std::memcpy(m_matrix.get(), rhs.m_matrix.get(),
                        m_height * m_width * sizeof(T));
        }

        return *this;
    }

    Matrix<T>& operator=(Matrix<T>&& rhs) {
        assert(this != &rhs);

        m_matrix = std::move(rhs.m_matrix);
        m_height = rhs.height();
        m_width = rhs.width();
        rhs.m_matrix = nullptr;

        return *this;
    }

    /// throws std::domain_error with dim mismatch
    Matrix<T>& operator+(const Matrix<T>& rhs) { return *this += rhs; }
    Matrix<T>& operator-(const Matrix<T>& rhs) { return *this -= rhs; }
    Matrix<T>& operator*(const Matrix<T>& rhs) { return *this *= rhs; }

    /// throws std::domain_error with dim mismatch
    Matrix<T>& operator+=(const Matrix<T>& rhs) {
        if (height() != rhs.height() || width() != rhs.width()) {
            throw std::domain_error(
                "Domain error: arguments have incompatible dimensions");
        }

        for (uint32_t i = 0; i < height(); i++) {
            for (uint32_t j = 0; j < width(); j++) {
                m_matrix(i, j) += rhs.m_matrix(i, j);
            }
        }

        return *this;
    }
    Matrix<T>& operator-=(const Matrix<T>& rhs) {
        if (height() != rhs.height() || width() != rhs.width()) {
            throw std::domain_error(
                "Domain error: arguments have incompatible dimensions");
        }

        for (uint32_t i = 0; i < height(); i++) {
            for (uint32_t j = 0; j < width(); j++) {
                m_matrix(i, j) -= rhs.m_matrix(i, j);
            }
        }

        return *this;
    }
    Matrix<T>& operator*=(const Matrix<T>& rhs) {
        if (width() != rhs.height()) {
            throw std::domain_error(
                "Domain error: arguments have incompatible dimensions");
        } else {
            Matrix<T> temp(height(), rhs.width());
            std::memset(temp.m_matrix.get(), 0,
                        temp.height() * temp.width() * sizeof(T));

            for (uint32_t i = 0; i < height(); i++) {
                for (uint32_t j = 0; j < rhs.width(); j++) {
                    for (uint32_t k = 0; k < width(); k++) {
                        temp(i, j) += operator()(i, k) * rhs(k, j);
                    }
                }
            }

            return *this = temp;
        }
    }

    bool operator==(const Matrix<T>& rhs) const {
        if (height() != rhs.height() || width() != rhs.width()) {
            return false;
        } else {
            for (uint32_t i = 0; i < height(); i++) {
                for (uint32_t j = 0; j < width(); j++) {
                    if (operator()(i, j) != rhs(i, j)) {
                        return false;
                    }
                }
            }

            return true;
        }
    }
    bool operator!=(const Matrix<T>& rhs) const { return !(*this == rhs); }

    /// Returns value contained by matrix at (h, w)
    /// Evaluates column first and row second.
    T& operator()(size_t h, size_t w) {
        if (h > height() - 1 || w > width() - 1) {
            std::cout << h << " > " << height() - 1 << " || " << w << " > "
                      << width() - 1 << "\n";
            throw std::out_of_range(
                "Out of Range error: array indices out of bounds");
        }

        // Use values in augmented matrix if w is past width of original matrix
        if (w > m_width - 1) {
            return (*m_augment)(h, w - m_width);
        } else {
            return m_matrix[h * width() + w];
        }
    }
    const T& operator()(size_t h, size_t w) const {
        return const_cast<Matrix<T>&>(*this)(h, w);
    }

    /// Augment this matrix with mat
    /// throws std::domain_error with dim mismatch
    void augment(const Matrix<T>& mat) {
        if (height() != mat.height()) {
            throw std::domain_error(
                "Domain error: arguments have incompatible dimensions");
        }

        m_augment = std::make_unique<Matrix<T>>(mat.height(), mat.width());

        *m_augment = mat;
    }

    Matrix<T> getAugment() const { return *m_augment; }

    /// Causes functions like RREF and inverse() to ignore augmented part of
    /// matrix
    void unaugment() {
        m_isAugmented = false;

        m_augment = nullptr;
    }

    /// throws std::domain_error with dim mismatch
    Matrix<T>& inverse() const {
        if (height() != width() || det() == 0) {
            throw std::domain_error("Domain error: matrix is not invertible");
        }

        Matrix<T> temp(height(), width(), true);

        augment(temp);

        Matrix<T> temp2 = rref();

        *this = temp2.getAugment();

        unaugment();

        return *this;
    }

    /// Computes determinant of this matrix
    /// throws std::domain_error with dim mismatch
    T det() const {
        if (height() != width()) {
            throw std::domain_error(
                "Domain error: argument is not a square matrix");
        }

        Matrix<T> temp = *this;

        for (uint32_t i = 0; i < height() - 1; i++) {
            temp.augment((*this)[i]);
        }

        T result = 0;

        for (uint32_t i = 0; i < height(); i++) {
            T product = 1;

            for (uint32_t j = 0; j < width(); j++) {
                product *= temp(i + j, j);
            }

            result += product;
        }

        for (uint32_t i = height() - 1; i > 0; i--) {
            T product = -1;

            for (uint32_t j = 0; j < width(); j++) {
                product *= temp(i + j, j);
            }

            result += product;
        }

        return result;
    }

    /// Returns transpose of this matrix
    Matrix<T> transpose() const {
        Matrix<T> temp(height(), width());

        for (uint32_t i = 0; i < height(); i++) {
            for (uint32_t j = 0; j < width(); j++) {
                temp(j, i) = operator()(i, j);
            }
        }

        return temp;
    }

    /// Returns this matrix in row echelon form
    Matrix<T> ref() const {
        Matrix<T> temp = *this;

        uint32_t i = 0;
        uint32_t j = 0;

        uint32_t k = 0;
        while (temp(k, 0) == 0) {
            k++;
        }
        i = k;
        temp(i, j);

        for (uint32_t i = 0; i < height(); i++) {
            T divisor = temp[i * width()];

            if (divisor != 1) {
                for (uint32_t j = 0; j < width(); j++) {
                    uint32_t idx = i * width() + j;
                    for (uint32_t k = temp[idx]; k < temp[idx] + width(); k++) {
                        temp[k] /= divisor;
                    }
                }
            }
        }

        return temp;
    }

    /// Returns this matrix in reduced row echelon form
    Matrix<T> rref() const { Matrix<T> temp = *this; }

    /// Returns column-major matrix as array of elements. Call delete on pointer
    /// when done.
    T* data() const { return m_matrix.get(); }

    void resize(size_t n) { m_matrix.resize(n); }
    size_t size() const { return m_height * m_width; }

    size_t height() const { return m_height; }
    size_t width() const {
        if (m_isAugmented) {
            return m_width + m_augment->m_width;
        } else {
            return m_width;
        }
    }

    friend std::ostream& operator<<(std::ostream& output,
                                    const Matrix<T>& rhs) {
        output << '[';
        for (uint32_t i = 0; i < rhs.height(); i++) {
            output << '[';
            for (uint32_t j = 0; j < rhs.width(); j++) {
                output << rhs(i, j);
                if (j < rhs.width() - 1) {
                    output << ' ';
                }
            }
            output << ']';
        }
        output << ']';

        return output;
    }

private:
    /// row-major matrix
    std::unique_ptr<T[]> m_matrix;
    size_t m_height;
    size_t m_width;
    bool m_isAugmented{false};

    std::unique_ptr<Matrix<T>> m_augment{nullptr};
};

namespace Mat {

template <class T>
Matrix<T> createQuaternion(T angle, T x, T y, T z) {
    Matrix<T> tempMat(4, 4);

    float mag = std::sqrt(x * x + y * y + z * z);
    float c = std::cos(angle * M_PI / 180.f);
    float s = std::sin(angle * M_PI / 180.f);

    if (mag != 0) {
        x /= mag;
        y /= mag;
        z /= mag;
    }

    tempMat(0, 0) = x * x * (1 - c) + c;
    tempMat(0, 1) = y * x * (1 - c) + z * s;
    tempMat(0, 2) = x * z * (1 - c) - y * s;
    tempMat(0, 3) = 0;
    tempMat(1, 0) = x * y * (1 - c) - z * s;
    tempMat(1, 1) = y * y * (1 - c) + c;
    tempMat(1, 2) = y * z * (1 - c) + x * s;
    tempMat(1, 3) = 0;
    tempMat(2, 0) = x * z * (1 - c) + y * s;
    tempMat(2, 1) = y * z * (1 - c) - x * s;
    tempMat(2, 2) = z * z * (1 - c) + c;
    tempMat(2, 3) = 0;
    tempMat(3, 0) = 0;
    tempMat(3, 1) = 0;
    tempMat(3, 2) = 0;
    tempMat(3, 3) = 1;

    return tempMat;
}

template <class T>
Matrix<T> createIdentity(size_t height, size_t width) {
    Matrix<T> temp(height, width, true);

    return temp;
}

}  // namespace Mat
