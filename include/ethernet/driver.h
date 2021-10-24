/**
 * @brief TM4C129 Ethernet Driver
 */

#pragma once

#include "circular_buffer.h"
#include "ethernet/ethernet_defs.h"
#include "ethernet/mac.h"

#include <functional>

namespace ethernet {

class Driver {
public:
  Driver() {};
  ~Driver() {};

  void init();

  void start_rx();
  void stop_rx();

  void start_tx();
  void stop_tx();

  // These are hooks for the runtime
  void interrupt_handler();
  void tick();

  CircularBuffer<dma::Buffer<1600>, 10>& rx_queue() { return _rx_queue; };

  bool queue_frame(const uint8_t* data, uint32_t count);

private:
  Mac _mac;

  void handle_rx();
  void handle_tx();

  void tx_dma_poll();

  CircularBuffer<dma::Buffer<1600>, 10> _rx_queue;
  CircularBuffer<dma::Buffer<1600>, 10> _tx_queue;
};


} // namespace ethernet
