/**
 * @brief The main app responsible for handling requests from phone clients
 */

#pragma once

#include "app_delegate.h"
#include "httpd_delegate.h"

#include <stdint.h>


class App : public HTTPDDelegate {
public:

  void set_delegate(AppDelegate* delegate) { _delegate = delegate; }

  // HTTPDDelegate hook
  void handle_request(const HTTPRequest& req);

private:
  AppDelegate* _delegate = NULL;
};
