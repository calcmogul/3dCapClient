// Copyright (c) Tyler Veness

#include "serial_port.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <format>
#include <print>
#include <string>
#include <string_view>
#include <vector>

#ifndef _WIN32
#include <fcntl.h>  // File control definitions
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>  // POSIX terminal control definitions
#include <unistd.h>   // UNIX standard function definitions

#include <cerrno>  // Error number definitions
#endif

void SerialPort::connect(std::string_view port_name) {
  // Disconnect before reconnecting or connecting to a different serial port
  if (m_connected) {
    disconnect();
  }

  m_port_name = port_name;

#ifdef _WIN32
  // Try to connect to the given port
  m_serial_handle =
      CreateFile(m_port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
#else
  // Try to connect to the given port
  m_fd = open(m_port_name.c_str(), O_RDONLY | O_NOCTTY | O_NONBLOCK);
#endif

#ifdef _WIN32
  // Check if connection was successful
  if (m_serial_handle == INVALID_HANDLE_VALUE) {
    if (GetLastError() == ERROR_FILE_NOT_FOUND) {
      std::print("{}: Unable to open {}: No such file or directory", __FILE__,
                 m_port_name);
    } else {
      std::println("{}: Unable to open {}", __FILE__, m_port_name);
    }
    return;
  }
#else
  // Check if connection was successful
  if (m_fd == -1) {
    std::println("{}: Unable to open {}: No such file or directory", __FILE__,
                 m_port_name);
    return;
  }

  // Set socket to nonblocking
  fcntl(m_fd, F_SETFL, O_NONBLOCK);
#endif

#ifdef _WIN32
  // Set baud rate
  DCB serial_port_params = {0};

  // Try to get the current comm parameters
  if (!GetCommState(m_serial_handle, &serial_port_params)) {
    // If impossible, show an error
    std::println("{}: Unable to retrieve current serial parameters for {}",
                 __FILE__, m_port_name);

    return;
  } else {
    // Define serial connection parameters for the Arduino board
    serial_port_params.BaudRate = CBR_115200;
    serial_port_params.ByteSize = 8;
    serial_port_params.StopBits = ONESTOPBIT;
    serial_port_params.Parity = NOPARITY;

    // Set the parameters and check for their proper application
    if (!SetCommState(m_serial_handle, &serial_port_params)) {
      std::println("{}: Unable to set serial port parameters for {}", __FILE__,
                   m_port_name);
      return;
    }
  }
#else
  // Set baud rate
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
    CloseHandle(m_serial_handle);
#else
    close(m_fd);
#endif
    m_connected = false;
  }
}

int SerialPort::read(char* buffer, size_t num_chars) {
#ifdef _WIN32
  // Get status info on the serial port
  DWORD errors;
  COMSTAT status;
  ClearCommError(m_serial_handle, &errors, &status);

  // Check if there is something to read
  if (status.cbInQue > 0) {
    // Number of characters to read
    size_t to_read = std::min<size_t>(num_chars, status.cbInQue);

    /* Try to read the require number of chars, and return the number of
     * read bytes on success
     */
    DWORD bytes_read;
    if (ReadFile(m_serial_handle, buffer, to_read, &bytes_read, nullptr)) {
      return bytes_read;
    } else if (GetLastError() != ERROR_IO_PENDING) {
      return -1;
    } else {
      buffer[0] = '\0';
      return 0;
    }
  } else {
    // There are no bytes to read
    return 0;
  }
#else
  int bytes_read = ::read(m_fd, buffer, num_chars);

  // Check for disconnection
  if (bytes_read == 0) {
    struct stat file_stats;
    return stat(m_port_name.c_str(), &file_stats);
  } else if (bytes_read == -1 && errno != EAGAIN) {
    return -1;
  } else {
    return std::max(bytes_read, 0);
  }
#endif
}

bool SerialPort::write(char* buffer, size_t num_chars) {
#ifdef _WIN32
  DWORD bytes_sent = 0;
#else
  int bytes_sent = 0;
#endif

#ifdef _WIN32
  // Try to write the buffer on the serial port
  if (!WriteFile(m_serial_handle, static_cast<void*>(buffer), num_chars,
                 &bytes_sent, 0)) {
    // Write failed, so retrieve comm error
    DWORD errors;
    COMSTAT status;
    ClearCommError(m_serial_handle, &errors, &status);

    return false;
  } else {
    return true;
  }
#else
  size_t pos = 0;

  while (pos < num_chars) {
    bytes_sent = ::write(m_fd, &buffer[pos], num_chars);

    if (bytes_sent > 0) {
      pos += bytes_sent;
    } else if (bytes_sent == -1 && errno != EAGAIN) {
      return false;
    }
  }

  return true;
#endif
}

std::vector<std::string> SerialPort::get_serial_ports() {
  std::vector<std::string> ports;

#ifdef _WIN32
  // Enumerate COM ports by checking if QueryDosDevice() returns nonzero
  for (int i = 0; i < 255; ++i) {
    char device_name[4096];
    if (QueryDosDevice(std::format("COM{}", i).c_str(), device_name, 4096) >
        0) {
      ports.emplace_back(device_name);
    }
  }
#else
  for (const auto& dir_entry : std::filesystem::directory_iterator{"/dev"}) {
    auto stem = dir_entry.path().stem().string();
    if (stem.starts_with("ttyACM") || stem.starts_with("ttyUSB")) {
      ports.emplace_back(dir_entry.path());
    }
  }
#endif

  return ports;
}
