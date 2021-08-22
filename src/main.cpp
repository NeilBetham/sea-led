#include "utils.h"
#include "status_led.h"
#include "color.h"
#include "ethernet/mac.h"
#include "registers/uart.h"
#include "registers/emac.h"
#include "registers/sysctl.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

ethernet::Mac mac_mgr;

void EthernetMac_ISR(void) {
  mac_mgr.get_desc().disown_all();
  if(EMAC_DMARIS & (BIT_6 | BIT_14)) {
    UART0_DR = '.';
    EMAC_DMARIS |= BIT_6 | BIT_14;
  }
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


  // ============== Setup EMAC / PHY =====================
  mac_mgr.init();

  // =============== Setup Peripherals  =======================
  init_status_led();

  // Enable interrupts
  CORE_EN0 = 0xFFFFFFFF;
  CORE_EN1 = 0xFFFFFFFF;
  CORE_EN2 = 0xFFFFFFFF;
  CORE_EN3 = 0xFFFFFFFF;
  GIE()

  // Enable the MAC
  mac_mgr.enable();

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
