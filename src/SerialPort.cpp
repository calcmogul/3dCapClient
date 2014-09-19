#include "SerialPort.hpp"

#ifdef _WIN32

SerialPort::SerialPort( const char* portName ) {
    // We're not yet connected
    m_connected = false;

    // Try to connect to the given port throuh CreateFile
    hSerial = CreateFile( portName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    // Check if the connection was successful
    if ( hSerial == INVALID_HANDLE_VALUE ) {
        // If not successful display an Error
        if ( GetLastError() == ERROR_FILE_NOT_FOUND ) {
            printf( "ERROR: Handle was not attached. Reason: %s not available.\n", portName );
        }
        else {
            printf( "ERROR!!!" );
        }
    }
    else {
        DCB dcbSerialPortParams = {0};

        // Try to get the current comm parameters
        if ( !GetCommState( hSerial, &dcbSerialPortParams ) ) {
            // If impossible, show an error
            printf( "failed to get current serial parameters!" );
        }
        else {
            // Define serial connection parameters for the Arduino board
            dcbSerialPortParams.BaudRate = CBR_115200;
            dcbSerialPortParams.ByteSize = 8;
            dcbSerialPortParams.StopBits = ONESTOPBIT;
            dcbSerialPortParams.Parity = NOPARITY;

            // Set the parameters and check for their proper application
            if( !SetCommState( hSerial, &dcbSerialPortParams ) ) {
               printf( "ALERT: Could not set SerialPort Port parameters" );
            }
            else {
                // If everything went fine we're connected
                m_connected = true;

                // We wait 2s as the Arduino board will be reseting
                Sleep( ARDUINO_WAIT_TIME );
            }
        }
    }
}

SerialPort::~SerialPort() {
    // Disconnect if necessary
    if ( m_connected ) {
        CloseHandle( hSerial );
        m_connected = false;
    }
}

int SerialPort::ReadData( char* buffer, unsigned int nbChar ) {
    // Number of bytes we'll have read
    DWORD bytesRead;
    // Number of bytes we'll really ask to read
    unsigned int toRead;

    // Use the ClearCommError function to get status info on the SerialPort port
    ClearCommError( hSerial, &m_errors, &m_status );

    // Check if there is something to read
    if ( m_status.cbInQue > 0 ) {
        /* If there is we check if there is enough data to read the required
         * number of characters, if not we'll read only the available
         * characters to prevent locking of the application.
         */
        if( m_status.cbInQue > nbChar ) {
            toRead = nbChar;
        }
        else {
            toRead = m_status.cbInQue;
        }

        /* Try to read the require number of chars, and return the number of
         * read bytes on success
         */
        if ( ReadFile( hSerial, buffer, toRead, &bytesRead, NULL ) && bytesRead != 0 ) {
            return bytesRead;
        }

    }

    // If nothing has been read, or that an error was detected, return -1
    return -1;
}


bool SerialPort::WriteData( char* buffer, unsigned int nbChar ) {
    DWORD bytesSend;

    // Try to write the buffer on the SerialPort port
    if( !WriteFile( hSerial, (void*)buffer, nbChar, &bytesSend, 0 ) ) {
        // In case it don't work get comm error and return false
        ClearCommError( hSerial, &m_errors, &m_status );

        return false;
    }
    else {
        return true;
    }
}

bool SerialPort::IsConnected() {
    return m_connected;
}

#endif
