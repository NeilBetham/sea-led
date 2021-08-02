#pragma once

#include <stdint.h>

// Ethernet MAC DMA Descriptors
// RX Enhanced
struct ENetRXDesc {
  // RDES0
  // Byte 0
  uint8_t ext_stat: 1;
  uint8_t ce: 1;
  uint8_t dribble_err: 1;
  uint8_t re: 1;
  uint8_t rwt: 1;
  uint8_t ft: 1;
  uint8_t lc: 1;
  uint8_t tagf: 1;

  // Byte 1
  uint8_t ls: 1;
  uint8_t fs: 1;
  uint8_t vlan: 1;
  uint8_t oe: 1;
  uint8_t le: 1;
  uint8_t saf: 1;
  uint8_t desc_err: 1;
  uint8_t es: 1;

  // Byte 2/3
  uint16_t fl: 14;
  uint8_t afm: 1;
  uint8_t own: 1;

  // RDES1
  uint16_t rbs1: 13;
  uint8_t : 1;
  uint8_t rch: 1;
  uint8_t rer: 1;
  uint16_t rbs2: 13;
  uint8_t :2;
  uint8_t dioc: 1;

  // RDES2
  void* buffer1_addr;

  // RDES3
  union {
    void* buffer2_addr;
    void* next_desc_addr;
  };

  // RDES4
  // Byte 0
  uint8_t ip_type: 3;
  uint8_t ip_hdr_err: 1;
  uint8_t ip_pyld_err: 1;
  uint8_t ip_chk_bp: 1;
  uint8_t ip_v4_rcv: 1;
  uint8_t ip_v6_rcv: 1;

  // Byte 1
  uint8_t message_type: 4;
  uint8_t ptp_frm_type: 1;
  uint8_t ptp_ver: 1;
  uint8_t ts_dropped: 1;
  uint8_t : 1;

  // Byte 2/3
  uint16_t : 16;

  // RDES5
  uint32_t : 32;

  // RDES6
  uint32_t rx_ts_l;

  // RDES7
  uint32_t rx_ts_h;
} PACKED;

// TX Enhanced
struct ENetTXDesc {
  // TDES0
  // Byte 3
  uint8_t db: 1;
  uint8_t uf: 1;
  uint8_t ed: 1;
  uint8_t cc: 4;
  uint8_t fv: 1;

  // Byte 2
  uint8_t ec: 1;
  uint8_t lcollision: 1;
  uint8_t nc: 1;
  uint8_t lcarrier: 1;
  uint8_t ipe: 1;
  uint8_t ff: 1;
  uint8_t jt: 1;
  uint8_t es: 1;

  // Byte 1
  uint8_t ihe: 1;
  uint8_t ttss: 1;
  uint8_t vlic: 2;
  uint8_t tch: 1;
  uint8_t ter: 1;
  uint8_t cic: 2;

  // Byte 0
  uint8_t crcr: 1;
  uint8_t ttse: 1;
  uint8_t dp: 1;
  uint8_t dc: 1;
  uint8_t fs: 1;
  uint8_t ls: 1;
  uint8_t ic: 1;
  uint8_t own: 1;

  // TDES1
  uint16_t tbs1: 13;
  uint8_t :3;
  uint16_t tbs2: 13;
  uint8_t saic: 3;

  // TDES2
  void* buffer1_addr;

  // TDES3
  union {
    void* buffer2_addr;
    void* next_desc_addr;
  };

  // TDES4/5
  uint32_t : 32;
  uint32_t : 32;

  // TDES6
  uint32_t tx_ts_l;

  // TDES7
  uint32_t tx_ts_h;

} PACKED;

