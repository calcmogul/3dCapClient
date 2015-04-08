// =============================================================================
// File Name: Matrix.hpp
// Description: A utility library for manipulating matrices
// Author: Tyler Veness
// =============================================================================

#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <memory>

template <class T>
class Matrix;

template <class T>
std::ostream& operator<<(std::ostream&, const Matrix<T>&);

template <class T>
class Matrix {
public:
    Matrix(size_t height, size_t width, bool initAsIdent = false);

    Matrix(T rhs);
    Matrix(const Matrix<T>& rhs);

    // throws std::domain_error with dim mismatch
    Matrix<T>& operator=(const Matrix<T>& rhs);
    Matrix<T>& operator=(Matrix<T>&& rhs);

    // throws std::domain_error with dim mismatch
    Matrix<T>& operator+(const Matrix<T>& rhs);
    Matrix<T>& operator-(const Matrix<T>& rhs);
    Matrix<T>& operator*(const Matrix<T>& rhs);

    // throws std::domain_error with dim mismatch
    Matrix<T>& operator+=(const Matrix<T>& rhs);
    Matrix<T>& operator-=(const Matrix<T>& rhs);
    Matrix<T>& operator*=(const Matrix<T>& rhs);

    bool operator==(const Matrix<T>& rhs) const;
    bool operator!=(const Matrix<T>& rhs) const;

    /* Returns value contained by matrix at (h, w)
     * Evaluates column first and row second.
     */
    T& operator()(size_t h, size_t w);
    const T& operator()(size_t h, size_t w) const;

    /* Augment this matrix with mat
     * throws std::domain_error with dim mismatch
     */
    void augment(const Matrix<T>& mat);

    Matrix<T> getAugment() const;

    /* Causes functions like RREF and inverse() to ignore augmented part of
     * matrix
     */
    void unaugment();

    // throws std::domain_error with dim mismatch
    Matrix<T>& inverse() const;

    /* Computes determinant of this matrix
     * // throws std::domain_error with dim mismatch
     */
    T det() const;

    // Returns transpose of this matrix
    Matrix<T> transpose() const;

    // Returns this matrix in row echelon form
    Matrix<T> ref() const;

    // Returns this matrix in reduced row echelon form
    Matrix<T> rref() const;

    /* Returns column-major matrix as array of elements. Call delete on pointer
     * when done.
     */
    T* data() const;

    void resize(size_t n);
    size_t size() const;

    size_t height() const;
    size_t width() const;

    friend std::ostream & operator<<<T>(std::ostream & output,
                                        const Matrix<T> &rhs);

private:
    // row-major matrix
    std::unique_ptr<T[]> m_matrix;
    size_t m_height;
    size_t m_width;
    bool m_isAugmented;

    std::unique_ptr<Matrix<T>> m_augment;
};

namespace Mat {
template <class T>
Matrix<T> createQuaternion(T angle, T x, T y, T z);

template <class T>
Matrix<T> createIdentity(size_t height, size_t width);
}

#include "Matrix.inl"

#endif // MATRIX_HPP

