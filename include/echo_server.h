/**
 * @brief Example echo server
 */

#pragma once

#include "socket_delegate.h"
#include "socket.h"

#include <stdint.h>
#include <vector>

class EchoServer : public SocketDelegate {
public:
  EchoServer(uint32_t port);

  void start();

  void handle_rx(Socket* conn, std::string& data);
  void handle_closed(Socket* conn);
  void handle_accept(Socket* conn);

private:
  uint32_t _port = 0;
  Socket _listen_socket;
  std::vector<Socket*> _active_connections;
};
