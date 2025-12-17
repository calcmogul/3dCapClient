// Copyright (c) Tyler Veness

#pragma once

#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef NOMINMAX
#endif

/// Provides an interface for communicating with an Arduino via the serial port
class SerialPort {
 public:
  ~SerialPort() { disconnect(); }

  /// Initialize serial communication over the given port.
  ///
  /// @param port_name Serial port name.
  void connect(std::string_view port_name);

  /// Closes the connection.
  ///
  /// NOTE: For some reason, after calling this functionm, the executable
  /// needs to be rerun to reconnect.
  void disconnect();

  /// Reads data into a buffer.
  ///
  /// @param buffer Destination buffer.
  /// @param num_chars Number of characters to read.
  /// @return The number of characters actually read or -1 on failure.
  int read(char* buffer, size_t num_chars);

  /// Writes data to the serial connection.
  ///
  /// @param buffer Source buffer.
  /// @param num_chars Number of characters to write.
  /// @return True on success, false on failure.
  bool write(char* buffer, size_t num_chars);

  /// Returns true if serial port is connected.
  bool is_connected() const { return m_connected; }

  /// Returns list of serial port names.
  static std::vector<std::string> get_serial_ports();

 private:
#ifdef _WIN32
  /// COM handle.
  HANDLE m_serial_handle;
#else
  /// Serial port file descriptor.
  int m_fd;
#endif

  /// Serial port name.
  std::string m_port_name;

  /// Connection status.
  bool m_connected = false;
};
