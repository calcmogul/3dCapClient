//=============================================================================
//File Name: Matrix.hpp
//Description: A utility library for manipulating matrices
//Author: Tyler Veness
//=============================================================================

#ifndef MATRIX_HPP
#define MATRIX_HPP

template <class T>
class Matrix {
public:
    Matrix( size_t width , size_t height );
    virtual ~Matrix();

    Matrix( T rhs );
    Matrix( const Matrix<T>& rhs );

    // throws std::domain_error with dim mismatch
    Matrix<T>& operator=( const Matrix<T>& rhs );
    Matrix<T>& operator=( Matrix<T>&& rhs );

    // throws std::domain_error with dim mismatch
    Matrix<T>& operator+( const Matrix<T>& rhs );
    Matrix<T>& operator-( const Matrix<T>& rhs );
    Matrix<T>& operator*( const Matrix<T>& rhs );

    // throws std::domain_error with dim mismatch
    Matrix<T>& operator+=( const Matrix<T>& rhs );
    Matrix<T>& operator-=( const Matrix<T>& rhs );
    Matrix<T>& operator*=( const Matrix<T>& rhs );

    bool operator==( const Matrix<T>& rhs ) const;
    bool operator!=( const Matrix<T>& rhs ) const;

    /* Returns value contained by matrix at (w, h)
     * Evaluates column first and row second.
     */
    T& operator() ( size_t w , size_t h );
    const T& operator() ( size_t w , size_t h ) const;

    /* Augment this matrix with mat
     * throws std::domain_error with dim mismatch
     */
    Matrix<T>& augment( const Matrix<T>& mat );

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

    void resize( size_t n );
    size_t size() const;

    size_t width() const;
    size_t height() const;

private:
    // column-major matrix
    T* m_matrix;
    size_t m_width;
    size_t m_height;
};

#include "Matrix.inl"

#endif // MATRIX_HPP
