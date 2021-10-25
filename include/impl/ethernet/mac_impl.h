#include "ethernet/mac.h"

#include "registers/sysctl.h"
#include "registers/emac.h"

namespace ethernet {

template <uint32_t DESC_COUNT>
void Mac<DESC_COUNT>::init() {
  // PHY needs to be inited first other wise shit will get fucked up
  _phy.init();

  // Enable clock to the MAC
  SYSCTL_RCGCEMAC = BIT_0;

  // Reset the MAC and wait for it to be ready
  SYSCTL_SREMAC = BIT_0;
  for(uint8_t index = 0; index < 16; index++);
  SYSCTL_SREMAC &= ~(BIT_0);
  while(!(SYSCTL_PREMAC & BIT_0));

  // Reset the DMA controller and wait for it to be ready
  EMAC_DMABUSMOD |= BIT_0;
  for(uint8_t index = 0; index < 16; index++);
  while(EMAC_DMABUSMOD & BIT_0);

  // Setup some of the DMA Params
  EMAC_DMABUSMOD |= BIT_25 | BIT_23 | BIT_17;
  EMAC_DMAIM |= BIT_16 | BIT_6 | BIT_0;

  // Setup the DMA descriptors
  _desc_mgr.install();

  // Disable a bunch of not needed interrupts
  EMAC_MMCTXIM |= BIT_20 | BIT_15 | BIT_14 | BIT_1;
  EMAC_MMCRXIM |= BIT_17 | BIT_6 | BIT_5 | BIT_0;
  EMAC_IM |= BIT_9 | BIT_3;

  // Enable interrupts from the PHY
  EPHY_IM |= BIT_0;

  // Configure some of the MAC params
  EMAC_ADDR0L    = 0x5379E738;
  EMAC_ADDR0H    = 0x0000B281;
  EMAC_FRAMEFLTR = 0x00000000;
  EMAC_CFG       = 0x30000400;

  // Configure MAC based on link type
  _phy.wait_for_link();
  EMAC_CFG |= _phy.get_mac_config();

  // Put the DMA into store and forward mode
  EMAC_DMAOPMODE |= BIT_21 | BIT_25;
}

template <uint32_t DESC_COUNT>
void Mac<DESC_COUNT>::enable() {
  EMAC_CFG |= BIT_2 | BIT_3;
}

template <uint32_t DESC_COUNT>
void Mac<DESC_COUNT>::disable() {
  EMAC_CFG &= ~(BIT_2 | BIT_3);
}


} // namespace ethernet
