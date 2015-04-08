// =============================================================================
// File Name: SerialPort.cpp
// Description: Provides an interface for communicating with an Arduino via the
//             serial port
// Author: Tyler Veness
// =============================================================================

#include "SerialPort.hpp"
#include <iostream>
#include <cstring>

#ifndef _WIN32
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <termios.h> // POSIX terminal control definitions
#include <dirent.h>  // Method for listing serial ports
#include <sys/types.h>
#include <sys/stat.h>
#endif

SerialPort::SerialPort(std::string portName) {
    m_portName = portName;

    // We're not yet connected
    m_connected = false;
}

SerialPort::~SerialPort() {
    disconnect();
}

void SerialPort::connect(std::string portName) {
    // Disconnect before reconnecting or connecting to a different serial port
    if (m_connected) {
        disconnect();
    }

    // Update name if necessary
    if (portName != "") {
        m_portName = portName;
    }
    // If no stored name, don't attempt a connection
    else if (m_portName == "") {
        return;
    }

    // Try to connect to the given port
#ifdef _WIN32
    hSerial = CreateFile(portName.c_str(),
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         nullptr,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         nullptr);
#else
    m_fd = open(portName.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY);
#endif

    // Check if connection was successful
#ifdef _WIN32
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cout << __FILE__ << ": Unable to open " << portName << '\n';
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            std::cout << ": No such file or directory\n";
        }

        return;
    }
#else
    if (m_fd == -1) {
        std::cout << __FILE__ << ": Unable to open " << m_portName
                  << ": No such file or directory\n";

        return;
    }
    else {
        fcntl(m_fd, F_SETFL, FNDELAY);
    }
#endif

    // Set baud rate
#ifdef _WIN32
    DCB dcbSerialPortParams = {0};

    // Try to get the current comm parameters
    if (!GetCommState(hSerial, &dcbSerialPortParams)) {
        // If impossible, show an error
        std::cout << __FILE__
                  << ": Unable to retrieve current serial parameters for "
                  << portName << '\n';

        return;
    }
    else {
        // Define serial connection parameters for the Arduino board
        dcbSerialPortParams.BaudRate = CBR_115200;
        dcbSerialPortParams.ByteSize = 8;
        dcbSerialPortParams.StopBits = ONESTOPBIT;
        dcbSerialPortParams.Parity = NOPARITY;

        // Set the parameters and check for their proper application
        if (!SetCommState(hSerial, &dcbSerialPortParams)) {
            std::cout << __FILE__
                      << ": Unable to set serial port parameters for "
                      << portName << '\n';

            return;
        }
    }
#else
    struct termios options;

    // Get the current options for the port
    tcgetattr(m_fd, &options);

    // Set the baud rates
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    // Enable the receiver and set local mode
    options.c_cflag |= (CLOCAL | CREAD);

    // Set the new options for the port
    tcsetattr(m_fd, TCSANOW, &options);
#endif

    m_connected = true;
}

void SerialPort::disconnect() {
    // Disconnect if necessary
    if (m_connected) {
#ifdef _WIN32
        CloseHandle(hSerial);
#else
        close(m_fd);
#endif
        m_connected = false;
    }
}

int SerialPort::read(char* buffer, unsigned int nbChar) {
#ifdef _WIN32
    // Number of bytes we'll have read
    DWORD bytesRead;
#else
    // Number of bytes we'll have read
    int bytesRead;
#endif

#ifdef _WIN32
    // Use the ClearCommError function to get status info on the SerialPort port
    ClearCommError(hSerial, &m_errors, &m_status);

    // Check if there is something to read
    if (m_status.cbInQue > 0) {
        /* If there is we check if there is enough data to read the required
         * number of characters, if not we'll read only the available
         * characters to prevent locking of the application.
         */
        // Number of bytes that will actually be read
        unsigned int toRead;
        if (m_status.cbInQue > nbChar) {
            toRead = nbChar;
        }
        else {
            toRead = m_status.cbInQue;
        }

        /* Try to read the require number of chars, and return the number of
         * read bytes on success
         */
        if (ReadFile(hSerial, buffer, toRead, &bytesRead, nullptr)) {
            return bytesRead;
        }
        else if (GetLastError() != ERROR_IO_PENDING) {
            return -1;
        }
        else {
            buffer[0] = '\0';
            return 0;
        }
    }
    else {
        // There are no bytes to read
        return 0;
    }
#else
    bytesRead = ::read(m_fd, buffer, nbChar);

    // Check for disconnection
    if (bytesRead == 0) {
        struct stat fileStats;
        if (stat(m_portName.c_str(), &fileStats) == -1) {
            return -1;
        }
        else {
            return 0;
        }
    }
    else if (bytesRead == -1 && errno != EAGAIN) {
        return -1;
    }
    else {
        return std::max(bytesRead, 0);
    }
#endif
}

bool SerialPort::write(char* buffer, unsigned int nbChar) {
#ifdef _WIN32
    DWORD bytesSent = 0;
#else
    int bytesSent = 0;
#endif

#ifdef _WIN32
    // Try to write the buffer on the serial port
    if (!WriteFile(hSerial, static_cast<void*>(buffer), nbChar, &bytesSent,
                   0)) {
        // Write failed, so retrieve comm error
        ClearCommError(hSerial, &m_errors, &m_status);

        return false;
    }
    else {
        return true;
    }
#else
    unsigned int pos = 0;

    while (pos < nbChar) {
        bytesSent = ::write(m_fd, &buffer[pos], nbChar);

        if (bytesSent > 0) {
            pos += bytesSent;
        }
        else if (bytesSent == -1 && errno != EAGAIN) {
            return false;
        }
    }

    return true;
#endif
}

bool SerialPort::isConnected() const {
    return m_connected;
}

std::vector<std::string> SerialPort::getSerialPorts() {
    std::vector<std::string> ports;

#ifdef _WIN32
    HKEY hRegAdapters;
    LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, AdapterKey, 0, KEY_READ,
                            &hRegAdapters);

    for (DWORD Index = 0; res == ERROR_SUCCESS; Index++) {
        char SubKeyName[255];
        DWORD cName = 255;
        res = RegEnumKeyEx(hRegAdapters,
                           Index,
                           SubKeyName,
                           &cName,
                           nullptr,
                           nullptr,
                           nullptr,
                           nullptr);
        if (res != ERROR_SUCCESS) {
            break;
        }
        // do something with Name
    }
#else
    DIR* d;
    struct dirent* dir;
    d = opendir("/dev");
    if (d) {
        while ((dir = readdir(d)) != nullptr) {
            if (dir->d_type == DT_CHR &&
                (std::strncmp(dir->d_name, "ttyS", sizeof("ttyS") - 1) == 0 ||
                 std::strncmp(dir->d_name, "ttyACM",
                              sizeof("ttyACM") - 1) == 0)) {
                // std::cout << dir->d_name << '\n';
                std::string tmp = "/dev/";
                tmp += dir->d_name;
                ports.push_back(tmp);
            }
        }

        closedir(d);
    }
#endif

    return ports;
}

