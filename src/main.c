#include "spi.h"
#include "cc1125.h"
#include "utils.h"
#include "status_led.h"
#include "color.h"
#include "ethernet.h"
#include "registers/uart.h"
#include "registers/emac.h"
#include "registers/sysctl.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

ALIGNED static ENetRXDesc rx_descriptors[10];
ALIGNED static ENetTXDesc tx_descriptors[10];

void setup_rx_descriptor(ENetRXDesc* desc, void* buffer) {
  zero(sizeof(ENetRXDesc), (void*)desc);
  desc->rbs1 = 1504;
  desc->rbs2 = 0;
  desc->buffer1_addr = buffer;
  desc->own = 1;
}

void reset_rx_descriptor(ENetRXDesc* desc) {
  desc->own = 1;
}

void setup_tx_descriptor(ENetTXDesc* desc, void* buffer) {
  zero(sizeof(ENetTXDesc), (void*)desc);
  desc->tbs1 = 1504;
  desc->tbs2 = 0;
  desc->buffer1_addr = buffer;
  desc->own = 1;
}

void reset_tx_descriptor(ENetTXDesc* desc) {
  desc->own = 1;
}


void EthernetMac_ISR(void) {
  for(int index = 0; index < 10; index++) {
    if(rx_descriptors[index].own == 0) {
      if(*((uint32_t*)rx_descriptors[index].buffer1_addr) != 0x00C28001) {
        UART0_DR = '.';
      }
      reset_rx_descriptor(&(rx_descriptors[index]));
    }
  }
  for(int index = 0; index < 10; index++) {
    if(tx_descriptors[index].own = 0) {
      reset_tx_descriptor(&(tx_descriptors[index]));
    }
  }
  EMAC_DMARIS |= BIT_6;
}

int main(void){
  uint32_t rx_desc_size = sizeof(ENetRXDesc);
  uint32_t reset_reason = SYSCTL_RESC;
  // ============== Crystal Init =======================
  // Set the crytsal range to high and clear the power down bit
  SYSCTL_MOSCCTL  |=   0x00000010;
  SYSCTL_MOSCCTL  &= ~(0x0000000C);

  // Wait for the crystal to stabilize
  while(!(SYSCTL_RIS & BIT_8))


  // ================= PLL Setup =======================
  // Bump up the clock speed
  // Set the PLL freq
  SYSCTL_RSCLKCFG &= ~(0x0F0003FF);
  SYSCTL_RSCLKCFG |=   0x03000000;
  SYSCTL_PLLFREQ1 &= ~(0x00001F1F);
  SYSCTL_PLLFREQ0 &= ~(0x008FFFFF);
  SYSCTL_PLLFREQ1 |=   0x00000004;
  SYSCTL_PLLFREQ0 |=   0x00800060;

  // Wait for the PLL to stabilize
  while (!(SYSCTL_PLLSTAT & 0x1))

  // Set the correct memory timings for 120MHz
  SYSCTL_MEMTIM0  &= ~(0x03EF03EF);
  SYSCTL_MEMTIM0  |=   0x01850185;

  // Setup Sysclk to use the PLL
  SYSCTL_RSCLKCFG |=   0x90000003;


  // ================= Peripheral Setup ==================
  // Enable peripheral clock gates
  SYSCTL_RCGCGPIO = 0xFFFFFFFF;
  SYSCTL_RCGCUART = 0x000000FF;


  // ============== Setup EMAC / PHY =====================
  // Setup descriptors and buffers
  char rx_buffer_0[1504];
  char rx_buffer_1[1504];
  char rx_buffer_2[1504];
  char rx_buffer_3[1504];
  char rx_buffer_4[1504];
  char rx_buffer_5[1504];
  char rx_buffer_6[1504];
  char rx_buffer_7[1504];
  char rx_buffer_8[1504];
  char rx_buffer_9[1504];

  setup_rx_descriptor(&rx_descriptors[0], rx_buffer_0);
  setup_rx_descriptor(&rx_descriptors[1], rx_buffer_1);
  setup_rx_descriptor(&rx_descriptors[2], rx_buffer_2);
  setup_rx_descriptor(&rx_descriptors[3], rx_buffer_3);
  setup_rx_descriptor(&rx_descriptors[4], rx_buffer_4);
  setup_rx_descriptor(&rx_descriptors[5], rx_buffer_5);
  setup_rx_descriptor(&rx_descriptors[6], rx_buffer_6);
  setup_rx_descriptor(&rx_descriptors[7], rx_buffer_7);
  setup_rx_descriptor(&rx_descriptors[8], rx_buffer_8);
  setup_rx_descriptor(&rx_descriptors[9], rx_buffer_9);
  rx_descriptors[9].rer = 1;

  char tx_buffer_0[1504];
  char tx_buffer_1[1504];
  char tx_buffer_2[1504];
  char tx_buffer_3[1504];
  char tx_buffer_4[1504];
  char tx_buffer_5[1504];
  char tx_buffer_6[1504];
  char tx_buffer_7[1504];
  char tx_buffer_8[1504];
  char tx_buffer_9[1504];

  setup_tx_descriptor(&tx_descriptors[0], tx_buffer_0);
  setup_tx_descriptor(&tx_descriptors[1], tx_buffer_1);
  setup_tx_descriptor(&tx_descriptors[2], tx_buffer_2);
  setup_tx_descriptor(&tx_descriptors[3], tx_buffer_3);
  setup_tx_descriptor(&tx_descriptors[4], tx_buffer_4);
  setup_tx_descriptor(&tx_descriptors[5], tx_buffer_5);
  setup_tx_descriptor(&tx_descriptors[6], tx_buffer_6);
  setup_tx_descriptor(&tx_descriptors[7], tx_buffer_7);
  setup_tx_descriptor(&tx_descriptors[8], tx_buffer_8);
  setup_tx_descriptor(&tx_descriptors[9], tx_buffer_9);
  tx_descriptors[9].ter = 1;


  // Setup Jack LEDS
  PORT_F_AFSEL = 0x000000FF;
  PORT_F_PCTL  = 0x55555555;
  PORT_F_DEN   = 0x000000FF;

  // Turn on the MAC
  SYSCTL_RCGCEMAC = 0x00000001;
  SYSCTL_PCEMAC   = 0x00000001;

  // Wait for the mac to boot
  while(!(SYSCTL_PREMAC & BIT_0))

  // Enable the PHY
  EMAC_PC |= BIT_0;
  SYSCTL_RCGCEPHY = 0x00000001;
  SYSCTL_PCEPHY   = 0x00000001;

  // Wait for the PHY to boot
  while(!(SYSCTL_PREPHY & BIT_0))
  EMAC_PC &= ~(BIT_0);

  // DMA Setup
  while(EMAC_DMABUSMOD & BIT_0)

  // Ext. desc size
  EMAC_DMABUSMOD = 0x00020180;
  EMAC_RXDLADDR = (void*)&rx_descriptors[0];
  EMAC_TXDLADDR = (void*)&tx_descriptors[0];
  EMAC_DMAIM = BIT_6 | BIT_7 | BIT_8 | BIT_13 | BIT_15;

  // Configure the MAC
  EMAC_ADDR0L    = 0x5379E738;
  EMAC_ADDR0H    = 0x0000B281;
  EMAC_FRAMEFLTR = 0x00000010;
  EMAC_CFG       = 0x30004C00;

  // Enable DMA
  EMAC_DMAOPMODE |= BIT_1;

  // ============== Setup UART ===========================
  // Enable UART Clock and associated GPIO port
  PORT_A_AFSEL   = 0x000000FF;
  PORT_A_PCTL    = 0x00000011;
  PORT_A_DEN     = 0x000000FF;

  // Setup UART
  UART0_CTL       = 0x00000000;
  UART0_IBRD      = 0x00000041;
  UART0_FBRD      = 0x00000007;
  UART0_LCRH      = 0x00000070;
  UART0_CC        = 0x00000000;
  UART0_CTL       = 0x00000301;


  // =============== Setup Peripherals  =======================
  init_status_led();

  // Enable interrupts
  CORE_EN0 = 0xFFFFFFFF;
  CORE_EN1 = 0xFFFFFFFF;
  CORE_EN2 = 0xFFFFFFFF;
  CORE_EN3 = 0xFFFFFFFF;
  GIE()

  // Enable RX
  EMAC_CFG |= 0x0000000C;

  // Flash some LEDs to make it known we are alive
  set_status_led(1, 0, 0);
  sleep(10000000);
  set_status_led(0, 1, 0);
  sleep(10000000);
  set_status_led(0, 0, 1);
  sleep(10000000);
  set_status_led(1, 1, 1);
  sleep(10000000);
  set_status_led(0, 0, 0);

  // Enable GPIOB GPIOD and UART1
  PORT_B_AFSEL   = 0x00000002;
  PORT_B_PCTL    = 0x00000010;
  PORT_B_DIR     = 0x00000002;
  PORT_B_DEN     = 0x00000002;

  PORT_D_DIR     = BIT_2 | BIT_6;
  PORT_D_AFSEL   = 0x00000000;
  PORT_D_DEN     = 0x000000FF;

  // Setup UART
  UART1_CTL       = 0x00000000;
  UART1_IBRD      = 0x0000001E;
  UART1_FBRD      = 0x00000020;
  UART1_LCRH      = 0x00000078;
  UART1_CC        = 0x00000000;
  UART1_CTL       = 0x00000301;

  // Enable RS485 TX and remote power enable line
  PORT_D_DATA     = BIT_2 | BIT_6;
  uint8_t slot_cycle_counter = 0;
  uint8_t slot_index_counter = 0;
  uint8_t data_buffer[11] = {0x00};

  set_status_led(0, 1, 0);

  // HSL State
  float hue = 0.0f;
  float saturation = 1.0f;
  float lightness = 1.0f;

  // RGB State
  float red = 0.0f;
  float green = 0.0f;
  float blue = 0.0f;

  while(1) {
    // Setup the data buffer for this cycle
    memset(data_buffer, 0, sizeof(data_buffer));

    // Convert the current HSL to RGB
    hsl_to_rgb(hue, saturation, lightness, &red, &green, &blue);
    data_buffer[1] = (uint8_t)(((float)UINT8_MAX) * red);
    data_buffer[2] = (uint8_t)(((float)UINT8_MAX) * green);
    data_buffer[3] = (uint8_t)(((float)UINT8_MAX) * blue);
    hue += 0.05;
    if(hue > 360.0f) {
      hue = 0.0f;
    }

    // Send the break word
    PORT_B_AFSEL  = 0x00000000;
    PORT_B_DATA   = 0x00000000;
    sleep(1965);

    // Send the mark word
    PORT_B_DATA   = 0x00000002;
    sleep(380);
    PORT_B_AFSEL  = 0x00000002;

    // Send the data bytes
    for(uint8_t index = 0; index < 11; index++) {
      UART1_DR = data_buffer[index];
      while(UART1_FR & BIT_5) {};  // Wait for fifo to have space
    }

    sleep(560000);
  }
}
