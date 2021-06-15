#pragma once

#include <stdint.h>

// Ethernet MAC DMA Descriptors
// RX Enhanced
typedef struct {
  // RDES0
  // Byte 0
  uint8_t blah;

} ENetRXDesc;

// TX Enhanced
typedef struct {
  // TDES0
  // Byte 0
  uint8_t crcr: 1;
  uint8_t ttse: 1;
  uint8_t dp: 1;
  uint8_t dc: 1;
  uint8_t fs: 1;
  uint8_t ls: 1;
  uint8_t ic: 1;
  uint8_t own: 1;

  // Byte 1
  uint8_t ihe: 1;
  uint8_t ttss: 1;
  uint8_t vlic: 2;
  uint8_t tch: 1;
  uint8_t ter: 1;
  uint8_t cic: 2;

  // Byte 2
  uint8_t ec: 1;
  uint8_t lcollision: 1;
  uint8_t nc: 1;
  uint8_t lcarrier: 1;
  uint8_t ipe: 1;
  uint8_t ff: 1;
  uint8_t jt: 1;
  uint8_t es: 1;

  // Byte 3
  uint8_t db: 1;
  uint8_t uf: 1;
  uint8_t ed: 1;
  uint8_t cc: 4;
  uint8_t fv: 1;

  // TDES1
  uint8_t saic: 3;
  uint16_t tbs2: 13;
  uint8_t :3;
  uint16_t tbs1: 13;

  // TDES2
  uint32_t buffer1_addr;

  // TDES3
  union {
    uint32_t buffer2_addr;
    uint32_t next_desc_addr;
  }

  // TDES4/5
  uint64_t;

  // TDES6
  uint32_t tx_ts_l;

  // TDES7
  uint32_t tx_ts_h;

} ENetTXDesc;



