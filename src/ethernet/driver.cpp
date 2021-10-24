#include "ethernet/driver.h"

#include "logging.h"

namespace ethernet {


void Driver::init() {
  _mac.init();
}

void Driver::start_rx() {
  EMAC_DMAOPMODE |= BIT_1;
  _mac.enable();
}

void Driver::stop_rx() {
  _mac.disable();
  EMAC_DMAOPMODE &= ~(BIT_1);
}

void Driver::start_tx() {
  EMAC_DMAOPMODE |= BIT_13;
  _mac.enable();
}

void Driver::stop_tx() {
  _mac.disable();
  EMAC_DMAOPMODE &= ~(BIT_13);
}

void Driver::interrupt_handler() {
  // Check if this is normal or error
  if(EMAC_DMARIS & BIT_16) {
    // Normal - check if TX / RX related
    if(EMAC_DMARIS & BIT_6) {
      // Receive
      EMAC_DMARIS |= BIT_6;
      handle_rx();
    }

    if(EMAC_DMARIS & BIT_0) {
      // Transmit
      EMAC_DMARIS |= BIT_0;
      log_i("TX Compelete");
    }

    if(EMAC_DMARIS & BIT_2) {
      // TX Buff Unavailable
      EMAC_DMARIS |= BIT_2;
      log_i("TX Buff Unvail");
    }

    if(EMAC_DMARIS & BIT_14) {
      // Early Receive Int
      EMAC_DMARIS |= BIT_14;
    }
  } else if(EMAC_DMARIS & BIT_15) {
    log_e("Ethernet error interrupt");
    // There is an abnormal interrupt
    if(EMAC_DMARIS & BIT_1) {
      // TX Stopped
      EMAC_DMARIS |= BIT_1;
      log_e("TX Stopped");
    }

    if(EMAC_DMARIS & BIT_3) {
      // Jabber TO
      EMAC_DMARIS |= BIT_3;
    }

    if(EMAC_DMARIS & BIT_4) {
      // RX FIFO Underflow
      EMAC_DMARIS |= BIT_4;
    }

    if(EMAC_DMARIS & BIT_5) {
      // TX Underflow
      EMAC_DMARIS |= BIT_5;
      log_e("TX Underflow");
    }

    if(EMAC_DMARIS & BIT_7) {
      // RX Buffer Unavailable
      EMAC_DMARIS |= BIT_7;
    }

    if(EMAC_DMARIS & BIT_8) {
      // RX Proc Stopped
      EMAC_DMARIS |= BIT_8;
    }

    if(EMAC_DMARIS & BIT_9) {
      // RX WD TO
      EMAC_DMARIS |= BIT_9;
    }

    if(EMAC_DMARIS & BIT_10) {
      // Early TX Int
      EMAC_DMARIS |= BIT_10;
    }

    if(EMAC_DMARIS & BIT_13) {
      // Fatal Bus Error
      EMAC_DMARIS |= BIT_13;
    }
  } else {
    // Some other interrupt has tripped
  }
}

void Driver::tick() {
//  handle_rx();
  handle_tx();
}

bool Driver::queue_frame(const uint8_t* data, uint32_t count) {
  dma::Buffer<1600> buffer;
  memcpy(buffer.buffer(), data, count);
  buffer.set_size(count);

  if(!_tx_queue.can_push()) { return false; }
  _tx_queue.push(buffer);
  return true;
}

void Driver::handle_rx() {
  // Iterate through the RX descriptors to see which have data.
  for(auto& desc : _mac.get_desc().rx_desc()) {
    // Check if the descriptor has been assigned to us
    if(desc.is_owned() == false) { continue; }

    // Copy the DMA buffer to local queue for processing
    // TODO: zerocopy buffering, prolly need some type of pool for this.
    if(_rx_queue.can_push()) {
      log_d("RX Packet Length: {}", desc.frame_length());
      desc.buffer().set_size(desc.frame_length());
      _rx_queue.push(desc.buffer());
    }

    // Disown descriptor
    desc.zero();
    desc.reset_owner();
  }
}

void Driver::handle_tx() {
  if(!_tx_queue.can_pop()) { return; }

  uint32_t used_desc = 0;
  bool should_start = false;

  if(!_tx_queue.can_pop()) { return; }

  // Load queued messages into TX DMA buffers
  auto& tx_desc = _mac.get_desc().curr_tx_desc();
  auto tx_buf = _tx_queue.pop();
  memcpy(tx_desc.buffer().buffer(), tx_buf.buffer(), tx_buf.size());
  tx_desc.set_frame_length(tx_buf.size());
  tx_desc.prep_for_tx();
  tx_desc.reset_owner();

  log_d("Starting TX DMA");
  tx_dma_poll();
}

void Driver::tx_dma_poll() {
  EMAC_TXPOLLD = 0xDEADBEEF;
}


} // namespace ethernet
