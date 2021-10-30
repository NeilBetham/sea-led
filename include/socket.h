/**
 * @brief Manages a LWIP Socket connection
 */

#pragma once

#include "socket_delegate.h"

#include "lwip/tcp.h"

#include <stdint.h>

enum class SocketMode {
  unknown = 0,
  listen,
  connection,
};

enum class SocketState {
  unknown = 0,
  init,
  connected,
  disconnected,
};

class Socket {
public:
  Socket() {};
  ~Socket() {};

  // Since this class can be targeted by c callbacks the this pointer is not allowed to change
  Socket(const Socket& other) = delete;
  Socket& operator=(const Socket& other) = delete;
  Socket(Socket&& other) = delete;
  Socket& operator=(Socket&& other) = delete;

  uint32_t write(const uint8_t* buffer, uint32_t size);
  void set_delegate(SocketDelegate* delegate) { _delegate = delegate; }

  // These two functions determine what type of connection this will be
  bool listen(uint32_t port);
  bool connect(uint32_t ip, uint32_t port) { return true; }; // <- TODO

  bool is_connected() { return _state == SocketState::connected; }

  // Hooks for LWIP; shouldn't be called by clients
  void read(std::string& data);
  void accept(struct tcp_pcb* conn);
  void error(err_t error);

private:
  uint32_t _port = 0;
  struct tcp_pcb* _tcp_handle = NULL;
  SocketDelegate* _delegate = NULL;
  SocketMode _mode = SocketMode::unknown;
  SocketState _state = SocketState::unknown;

  // This constructor is used by listen mode sockets when a new connection is accepted
  Socket(struct tcp_pcb* conn, uint32_t port);
};
