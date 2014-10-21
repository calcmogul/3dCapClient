//=============================================================================
//File Name: Matrix.inl
//Description: A utility library for manipulating matrices
//Author: Tyler Veness
//=============================================================================

#include <new>
#include <cstdint>
#include <stdexcept>

template <class T>
Matrix<T>::Matrix( size_t width , size_t height ) :
m_matrix( new T[width * height] ) ,
m_width( width ) ,
m_height( height ) {

}

template <class T>
Matrix<T>::~Matrix() {
    delete[] m_matrix;
}

template <class T>
Matrix<T>::Matrix( std::vector<T> rhs ) {
    m_matrix.push_back( rhs );
}

template <class T>
Matrix<T>::Matrix( T rhs ) :
m_matrix( new T[1 * 1] ) ,
m_width ( 1 ) ,
m_height( 1 ) {
    m_matrix[0][0] = rhs;
}

template <class T>
Matrix<T>& Matrix<T>::operator=( const Matrix<T>& rhs ) {
    if ( this != &rhs ) {
        if ( width() != rhs.width() || height() != rhs.height() ) {
            throw std::domain_error( "arguments have incompatible dimensions" );
        }
        else {
            while ( (*m_matrix[0]++ = *rhs.m_matrix[0]++) );
        }
    }

    return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator+( const Matrix<T>& rhs ) {
    return Matrix<T>( *this ) += rhs;
}

template <class T>
Matrix<T>& Matrix<T>::operator-( const Matrix<T>& rhs ) {
    return Matrix<T>( *this ) -= rhs;
}

template <class T>
Matrix<T>& Matrix<T>::operator*( const Matrix<T>& rhs ) {
    if ( width() != rhs.height() ) {
        throw std::domain_error( "arguments have incompatible dimensions" );
    }
    else {
        Matrix<T> temp( height() * rhs.width() );
        for ( uint32_t i = 0 ; i < temp.width() * temp.height() ; i++ ) {
            temp.m_matrix[i] = 0;
        }

        for ( uint32_t i = 0 ; i < width() ; i++ ) {
            for ( uint32_t j = 0 ; j < height() ; j++ ) {
                for ( uint32_t k = 0 ; k < width() ; k++ ) {
                    temp[i][j] += m_matrix[i + k][j] * rhs.m_matrix[i][j + k];
                }
            }
        }

        return temp;
    }
}

template <class T>
Matrix<T>& Matrix<T>::operator+=( const Matrix<T>& rhs ) {
    if ( width() != rhs.width() || height() != rhs.height() ) {
        throw std::domain_error( "arguments have incompatible dimensions" );
    }

    for ( uint32_t i = 0 ; i < width() ; i++ ) {
        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            m_matrix[i][j] += rhs.m_matrix[i][j];
        }
    }

    return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator-=( const Matrix<T>& rhs ) {
    if ( width() != rhs.width() || height() != rhs.height() ) {
        throw std::domain_error( "arguments have incompatible dimensions" );
    }

    for ( uint32_t i = 0 ; i < width() ; i++ ) {
        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            m_matrix[i][j] -= rhs.m_matrix[i][j];
        }
    }

    return *this;
}

template <class T>
bool Matrix<T>::operator==( const Matrix<T>& rhs ) const {
    if ( width() != rhs.width() || height() != rhs.height() ) {
        return false;
    }
    else {
        for ( uint32_t i = 0 ; i < width() ; i++ ) {
            for ( uint32_t j = 0 ; j < height() ; j++ ) {
                if ( m_matrix[i][j] != rhs.m_matrix[i][j] ) {
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
T* Matrix<T>::operator[]( size_t i ) {
    return &m_matrix[i];

#if 0
    size_t tempSize = 1;
    if ( width() > 1 ) {
        Matrix<T> temp( height() , 1 );

        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            temp[j] = m_matrix[i * height() + j];
        }

        return temp;
    }
    else {
        Matrix<T> temp( 1 , 1 );

        temp[0] = m_matrix[i];

        return temp;
    }
#endif
}

template <class T>
const T* Matrix<T>::operator[]( size_t i ) const {
    return &const_cast<T*>(*this)[i];
}

#if 0
template <class T>
Matrix<T>& Matrix<T>::operator[]( size_t i ) {
    return m_matrix[i];

#if 0
    size_t tempSize = 1;
    if ( width() > 1 ) {
        Matrix<T> temp( height() , 1 );

        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            temp[j] = m_matrix[i * height() + j];
        }

        return temp;
    }
    else {
        Matrix<T> temp( 1 , 1 );

        temp[0] = m_matrix[i];

        return temp;
    }
#endif
}

template <class T>
const Matrix<T>& Matrix<T>::operator[]( size_t i ) const {
    return const_cast<Matrix<T>&>(*this)[i];
}
#endif

template <class T>
Matrix<T>& Matrix<T>::augment( const Matrix<T>& mat ) {
    if ( height() != mat.height() ) {
        throw std::domain_error( "arguments have incompatible dimensions" );
    }

    for ( uint32_t i = 0 ; i < mat.m_matrix.size() ; i++ ) {
        m_matrix.push_back( mat.m_matrix[i] );
    }

    return *this;
}

template <class T>
Matrix<T>& Matrix<T>::inverse() {
    if ( width() != height() || det() == 0 ) {
        throw std::domain_error( "matrix is not invertible" );
    }

    Matrix<T> temp( width() , height() );

    // Create identity matrix
    for ( uint32_t i = 0 ; i < width() ; i++ ) {
        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            if ( i == j ) {
                temp[i][j] = 1;
            }
            else {
                temp[i][j] = 0;
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
        throw std::domain_error( "argument is not a square matrix" );
    }

    Matrix<T> temp = *this;

    for ( uint32_t i = 0 ; i < width() - 1 ; i++ ) {
        temp.augment( (*this)[i] );
    }

    T result = 0;

    for ( uint32_t i = 0 ; i < width() ; i++ ) {
        T product = 1;

        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            product *= temp[i+j][j];
        }

        result += product;
    }

    for ( uint32_t i = width() - 1 ; i > 0 ; i-- ) {
        T product = -1;

        for ( uint32_t j = 0 ; j < height() ; j++ ) {
            product *= temp[i+j][j];
        }

        result += product;
    }

    return result;
}

template <class T>
Matrix<T>& Matrix<T>::ref() const {
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
Matrix<T>& Matrix<T>::rref() const {
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
