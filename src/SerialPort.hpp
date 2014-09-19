#ifndef SERIAL_PORT_HPP
#define SERIAL_PORT_HPP

#ifdef _WIN32

#define ARDUINO_WAIT_TIME 2000

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstdlib>

class SerialPort {
public:
    // Initialize SerialPort communication with the given COM port
    SerialPort( const char* portName );

    /* Close the connection
     * NOTE: for some reason you can't connect again before exiting the program
     * and running it again.
     */
    ~SerialPort();

    /* Read data in a buffer, if nbChar is greater than the maximum number of
     * bytes available, it will return only the bytes available. The function
     * return -1 when nothing could be read, the number of bytes actually read.
     */
    int ReadData( char* buffer, unsigned int nbChar );

    /* Writes data from a buffer through the SerialPort connection. Returns true on
     * success.
     */
    bool WriteData( char* buffer, unsigned int nbChar );

    // Check if we are actually connected
    bool IsConnected();

private:
    // SerialPort comm handler
    HANDLE hSerial;

    // Connection status
    bool m_connected;

    // Get various information about the connection
    COMSTAT m_status;

    // Keep track of last error
    DWORD m_errors;
};

#endif

#endif // SERIAL_PORT_HPP
