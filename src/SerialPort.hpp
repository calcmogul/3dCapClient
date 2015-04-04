// =============================================================================
// File Name: SerialPort.hpp
// Description: Provides an interface for communicating with an Arduino via the
//             serial port
// Author: Tyler Veness
// =============================================================================

#ifndef SERIAL_PORT_HPP
#define SERIAL_PORT_HPP

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <vector>
#include <string>

class SerialPort {
public:
    // Initialize SerialPort communication with the given COM port
    explicit SerialPort(std::string portName = "");

    /* Close the connection
     * NOTE: for some reason you can't connect again before exiting the program
     * and running it again.
     */
    ~SerialPort();

    /* Initialize SerialPort communication with the given COM port. If NULL is
     * passed as an argument, the previously assigned name will be used. This
     * should be done in the case of a reconnection attempt.
     */
    void connect(std::string portName = "");

    /* Close the connection
     * NOTE: for some reason you can't connect again before exiting the program
     * and running it again.
     */
    void disconnect();

    /* Read data in a buffer, if nbChar is greater than the maximum number of
     * bytes available, it will return only the bytes available. The function
     * return -1 when nothing could be read, the number of bytes actually read.
     */
    int read(char* buffer, unsigned int nbChar);

    /* Writes data from a buffer through the SerialPort connection. Returns
     * true on success; returns false on failure.
     */
    bool write(char* buffer, unsigned int nbChar);

    // Check if we are actually connected
    bool isConnected() const;

    static std::vector<std::string> getSerialPorts();

private:
#ifdef _WIN32
    // SerialPort comm handler
    HANDLE hSerial;

    // Get various information about the connection
    COMSTAT m_status;

    // Keep track of last error
    DWORD m_errors;
#else
    int m_fd;
#endif

    // Contains OS-specific name for serial port
    std::string m_portName;

    // Connection status
    bool m_connected;
};

#endif // SERIAL_PORT_HPP

