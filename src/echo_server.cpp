#include "echo_server.h"

#include "logging.h"

EchoServer::EchoServer(uint32_t port) {
  _port = port;
}

void EchoServer::start() {
  _listen_socket.set_delegate(this);
  _listen_socket.listen(_port);
}

void EchoServer::handle_rx(Socket* conn, std::string& data) {
  conn->write((const uint8_t*)data.data(), data.size());
}

void EchoServer::handle_closed(Socket* conn) {
  if(_active_connections.size() == 1) {
    delete _active_connections[0];
    _active_connections.clear();
  } else {
    for(auto sock_it = _active_connections.begin(); sock_it != _active_connections.end(); sock_it++) {
      if(*sock_it == conn) {
        delete *sock_it;
        _active_connections.erase(sock_it);
      }
    }
  }
  log_i("Active Echo Conns: {}", _active_connections.size());
}

void EchoServer::handle_accept(Socket* conn) {
  _active_connections.push_back(conn);
  conn->set_delegate(this);
  log_i("Active Echo Conns: {}", _active_connections.size());
}
