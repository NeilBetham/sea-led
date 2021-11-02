/**
 * @brief Delegate for HTTP events
 */

#pragma once

#include <stdint.h>

#include "picohttpparser.h"

static constexpr uint32_t MAX_HEADERS = 100;

class Socket;

struct HTTPRequest {
  const char* method;
  size_t method_len;

  const char* path;
  size_t path_len;

  struct phr_header headers[MAX_HEADERS];
  size_t headers_len = MAX_HEADERS;

  int http_minor_rev = 0;

  const char* body;
  size_t body_len;

  Socket* connection;
};

class HTTPDDelegate {
public:
  virtual ~HTTPDDelegate() {};

  virtual void handle_request(const HTTPRequest& req) = 0;
};
