/**
 * @brief Abstraction of MAC
 */

#pragma once

#include "ethernet/phy.h"
#include "ethernet/dma.h"

namespace ethernet {


template <uint32_t DESC_COUNT>
class Mac {
public:
  void init();

  void enable();
  void disable();

  Phy& get_phy() { return _phy; };
  dma::DescriptorMgr<DESC_COUNT>& get_desc() { return _desc_mgr; };

private:
  Phy _phy;
  dma::DescriptorMgr<DESC_COUNT> _desc_mgr;
};


} // namespace ethernet

#include "impl/ethernet/mac_impl.h"
