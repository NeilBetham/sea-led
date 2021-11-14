/**
 * @brief Handles the info.txt route
 */

#pragma once

#include "httpd_delegate.h"
#include "http_utils.h"

#define FMT_HEADER_ONLY
#include <fmt/core.h>

extern volatile uint32_t internal_temp;

class RouteInfoTxt : public HTTPDDelegate  {
public:

  void handle_request(const HTTPRequest& req) {
    const static std::string info_txt_format = "INFO=FW:01.21,WFW:150410-c2e37a3-SPWF01S,V:12.0,T:{}.0,SN:0004815D07187,CS:";
    auto info_txt = fmt::format(info_txt_format, internal_temp);
    uint8_t cs = checksum(info_txt);
    std::string resp = fmt::format("{}{:02x}", info_txt, cs);
    render_txt(req.connection, resp);
  }
};
