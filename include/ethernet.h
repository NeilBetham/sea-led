// @brief Classes for dealing with ethernet via DMA
#pragma once

#include "ethernet_defs.h"
#include "registers/emac.h"

#include <array>
#include <string.h>

namespace ethernet {
namespace dma {


/// @brief Manages the underlying buffer for a DMA descriptor
template <size_t BUFFER_SIZE>
class Buffer {
public:
  Buffer() { zero(); };

  constexpr size_t size() { return BUFFER_SIZE; }
  uint8_t* buffer() { return _buffer; };
  void zero() { memset(_buffer, 0, BUFFER_SIZE); };

private:
  ALIGNED uint8_t _buffer[BUFFER_SIZE];
};

/// @brief Manages the RX descriptor and it's underlying buffer
template <size_t BUFFER_SIZE>
class RXDescriptor {
public:
  void setup_descriptor(ENetRXDesc* descriptor) {
    _descriptor = descriptor;
    _descriptor->rbs1 = BUFFER_SIZE;
    _descriptor->rbs2 = 0;
    _descriptor->buffer1_addr = _buffer.buffer();
    reset_owner();
  };

  void reset_owner() { _descriptor->own = 1; };
  void zero() { _buffer.zero(); };
  Buffer<BUFFER_SIZE>& buffer() { return _buffer; };

private:
  Buffer<BUFFER_SIZE> _buffer;
  ENetRXDesc* _descriptor = NULL;
};

/// @brief Manages the TX descriptor and it's underlying buffer
template <size_t BUFFER_SIZE>
class TXDescriptor {
public:
  void setup_descriptor(ENetTXDesc* descriptor) {
    _descriptor = descriptor;
    _descriptor->tbs1 = BUFFER_SIZE;
    _descriptor->tbs2 = 0;
    _descriptor->buffer1_addr = _buffer.buffer();
    reset_owner();
  };

  void reset_owner() { _descriptor->own = 1; };
  void zero() { _buffer.zero(); };
  Buffer<BUFFER_SIZE>& buffer() { return _buffer; };

private:
  Buffer<BUFFER_SIZE> _buffer;
  ENetTXDesc* _descriptor = NULL;
};

/// @brief Manages a list of descriptors and the associated registers in the ENet DMA
template <int BUFFER_COUNT = 10, size_t BUFFER_SIZE = 2000>
class DescriptorMgr {
public:
  DescriptorMgr() {
    for(int index = 0; index < BUFFER_COUNT; index++) {
      _rx[index].setup_descriptor(&_rx_descriptors[index]);
      _tx[index].setup_descriptor(&_tx_descriptors[index]);
    }
    _rx_descriptors[BUFFER_COUNT - 1].rer = 1;
    _tx_descriptors[BUFFER_COUNT - 1].ter = 1;
  };

  void install() {
    EMAC_DMABUSMOD |= BIT_7;  // Set descriptor length to 8 bytes
    EMAC_RXDLADDR = (uint32_t)&_rx_descriptors;
    EMAC_TXDLADDR = (uint32_t)&_tx_descriptors;
  };

  void remove() {
    EMAC_RXDLADDR = 0;
    EMAC_TXDLADDR = 0;
  };

  void disown_all() {
    for(int index = 0; index < BUFFER_COUNT; index++) {
      _rx[index].reset_owner();
      _tx[index].reset_owner();
    }
  }

private:
  ALIGNED ENetRXDesc _rx_descriptors[BUFFER_COUNT];
  ALIGNED ENetTXDesc _tx_descriptors[BUFFER_COUNT];
  std::array<RXDescriptor<BUFFER_SIZE>, BUFFER_COUNT> _rx;
  std::array<TXDescriptor<BUFFER_SIZE>, BUFFER_COUNT> _tx;
};


} // namespace dma
} // namespace ethernet
