/**
 * @brief Abstraction of MAC
 */

#pragma once

#include "ethernet/phy.h"
#include "ethernet/dma.h"

namespace ethernet {


class Mac {
public:
  void init();

  void enable();
  void disable();

  Phy& get_phy() { return _phy; };
  dma::DescriptorMgr<20>& get_desc() { return _desc_mgr; };

private:
  Phy _phy;
  dma::DescriptorMgr<20> _desc_mgr;
};


} // namespace ethernet
