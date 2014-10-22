//=============================================================================
//File Name: Matrix.inl
//Description: A utility library for manipulating matrices
//Author: Tyler Veness
//=============================================================================

#include <cstdint>
#include <cstring>
#include <stdexcept>

template <class T>
Matrix<T>::Matrix( size_t width , size_t height ) :
m_width( width ) ,
m_height( height ) {
    m_matrix = new T[width * height];
}

template <class T>
Matrix<T>::~Matrix() {
    delete[] m_matrix;
}

template <class T>
Matrix<T>::Matrix( T rhs ) :
m_matrix( new T[1] ) ,
m_width ( 1 ) ,
m_height( 1 ) {
    m_matrix[0] = rhs;
}

template <class T>
Matrix<T>::Matrix( const Matrix<T>& rhs ) :
m_width( rhs.width() ) ,
m_height( rhs.height() ) {
    m_matrix = new T[m_width * m_height];

    T* src = rhs.m_matrix;
    T* dest = m_matrix;

    while ( (*dest++ = *src++) );
}

template <class T>
Matrix<T>& Matrix<T>::operator=( const Matrix<T>& rhs ) {
    if ( this != &rhs ) {
        if ( width() != rhs.width() || height() != rhs.height() ) {
#if 1
            throw std::domain_error( "Domain error: arguments have incompatible dimensions" );
#else
            delete[] m_matrix;
            m_matrix = new T[rhs.width() * rhs.height()];

            m_width = rhs.width();
            m_height = rhs.height();
#endif
        }
        else {
            T* src = rhs.m_matrix;
            T* dest = m_matrix;

            while ( (*dest++ = *src++) );
        }
    }

    return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator=( Matrix<T>&& rhs ) {
    assert( this != &rhs );

    delete[] m_matrix;
    m_matrix = rhs.m_matrix;
    m_width = rhs.width();
    m_height = rhs.height();
    rhs.m_matrix = nullptr;

    return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator+( const Matrix<T>& rhs ) {
    return *this += rhs;
}

template <class T>
Matrix<T>& Matrix<T>::operator-( const Matrix<T>& rhs ) {
    return *this -= rhs;
}

template <class T>
Matrix<T>& Matrix<T>::operator*( const Matrix<T>& rhs ) {
    return *this *= rhs;
}

template <class T>
Matrix<T>& Matrix<T>::operator+=( const Matrix<T>& rhs ) {
    if ( width() != rhs.width() || height() != rhs.height() ) {
        throw std::domain_error( "Domain error: arguments have incompatible dimensions" );
    }

    for ( uint32_t i = 0 ; i < width() ; i++ ) {
        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            m_matrix( i , j ) += rhs.m_matrix( i , j );
        }
    }

    return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator-=( const Matrix<T>& rhs ) {
    if ( width() != rhs.width() || height() != rhs.height() ) {
        throw std::domain_error( "Domain error: arguments have incompatible dimensions" );
    }

    for ( uint32_t i = 0 ; i < width() ; i++ ) {
        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            m_matrix( i , j ) -= rhs.m_matrix( i , j );
        }
    }

    return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator*=( const Matrix<T>& rhs ) {
    if ( width() != rhs.height() ) {
        throw std::domain_error( "Domain error: arguments have incompatible dimensions" );
    }
    else {
        Matrix<T> temp( rhs.width() , height() );
        std::memset( temp.m_matrix , 0 , temp.width() * temp.height() );

        for ( uint32_t i = 0 ; i < width() ; i++ ) {
            for ( uint32_t j = 0 ; j < height() ; j++ ) {
                for ( uint32_t k = 0 ; k < width() ; k++ ) {
                    temp( i , j ) += operator()( k , j ) * rhs( i , k );
                }
            }
        }

        return *this = temp;
    }
}

template <class T>
bool Matrix<T>::operator==( const Matrix<T>& rhs ) const {
    if ( width() != rhs.width() || height() != rhs.height() ) {
        return false;
    }
    else {
        for ( uint32_t i = 0 ; i < width() ; i++ ) {
            for ( uint32_t j = 0 ; j < height() ; j++ ) {
                if ( operator()( i , j ) != rhs( i , j ) ) {
                    return false;
                }
            }
        }

        return true;
    }
}

template <class T>
bool Matrix<T>::operator!=( const Matrix<T>& rhs ) const {
    return !(*this == rhs);
}

template <class T>
T& Matrix<T>::operator()( size_t w , size_t h ) {
    if ( w > width() - 1 || h > height() - 1 ) {
        throw std::out_of_range( "Out of Range error: array indices out of bounds" );
    }

    return m_matrix[w * height() + h];
}

template <class T>
const T& Matrix<T>::operator()( size_t w , size_t h ) const {
    return const_cast<Matrix<T>&>(*this)( w , h );
}

template <class T>
Matrix<T>& Matrix<T>::augment( const Matrix<T>& mat ) {
    if ( height() != mat.height() ) {
        throw std::domain_error( "Domain error: arguments have incompatible dimensions" );
    }

    for ( uint32_t i = 0 ; i < mat.size() ; i++ ) {
        T temp[size() + mat.size()];

        T* src = mat.m_matrix;
        T* dest = m_matrix;

        while ( (*dest++ = *src++) );
    }

    return *this;
}

template <class T>
Matrix<T>& Matrix<T>::inverse() const {
    if ( width() != height() || det() == 0 ) {
        throw std::domain_error( "Domain error: matrix is not invertible" );
    }

    Matrix<T> temp( width() , height() );

    // Create identity matrix
    for ( uint32_t i = 0 ; i < width() ; i++ ) {
        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            if ( i == j ) {
                temp( i , j ) = 1;
            }
            else {
                temp( i , j ) = 0;
            }
        }
    }

    augment( temp );

    Matrix<T> temp2 = rref();

    // Only keep the augmented part of the matrix
    for ( uint32_t i = 0 ; i < width() * height() ; i++ ) {
        temp.m_matrix[i] = temp2.m_matrix[width() + i];
    }

    return temp;
}

template <class T>
T Matrix<T>::det() const {
    if ( width() != height() ) {
        throw std::domain_error( "Domain error: argument is not a square matrix" );
    }

    Matrix<T> temp = *this;

    for ( uint32_t i = 0 ; i < width() - 1 ; i++ ) {
        temp.augment( (*this)[i] );
    }

    T result = 0;

    for ( uint32_t i = 0 ; i < width() ; i++ ) {
        T product = 1;

        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            product *= temp( i+j , j );
        }

        result += product;
    }

    for ( uint32_t i = width() - 1 ; i > 0 ; i-- ) {
        T product = -1;

        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            product *= temp( i+j , j );
        }

        result += product;
    }

    return result;
}

template <class T>
Matrix<T> Matrix<T>::transpose() const {
    Matrix<T> temp( height() , width() );

    for ( uint32_t i = 0 ; i < width() ; i++ ) {
        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            temp( j , i ) = operator()( i , j );
        }
    }

    return temp;
}

template <class T>
Matrix<T> Matrix<T>::ref() const {
    Matrix<T> temp = *this;

#if 0
    for ( uint32_t i = 0 ; i < width() ; i++ ) {
        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            uint32_t idx = j * height() + i;
            if ( m_matrix[idx] != 1 ) {
                T divisor = m_matrix[idx];
                for ( uint32_t k = m_matrix[idx] ; k < m_matrix[idx] + width() ; k++ ) {
                    m_matrix[k] /= divisor;
                }
            }
        }
    }
#endif
}

template <class T>
Matrix<T> Matrix<T>::rref() const {
    Matrix<T> temp = *this;
}

template <class T>
T* Matrix<T>::data() const {
    return m_matrix;
}

template <class T>
void Matrix<T>::resize( size_t n ) {
    m_matrix.resize( n );
}

template <class T>
size_t Matrix<T>::size() const {
    return m_width * m_height;
}

template <class T>
size_t Matrix<T>::width() const {
    return m_width;
}

template <class T>
size_t Matrix<T>::height() const {
    return m_height;
}
