#include "dmx.h"

#include "timing.h"
#include "utils.h"
#include "registers/gpio.h"
#include "registers/uart.h"


void DMX::init(uint32_t refresh_time_ms) {
  _refresh_time_ms = refresh_time_ms;

  // Init the GPIO and UART
	// DMX TX lives on port B channel 1
  PORT_B_AFSEL   |= BIT_1;
  PORT_B_PCTL    |= BIT_4;
  PORT_B_DIR     |= BIT_1;
  PORT_B_DEN     |= BIT_1;

	// Power gating control lives on port D
	// TX Enable lives on D2 and Power gating on D6
  PORT_D_DIR     |= BIT_2 | BIT_6;
  PORT_D_AFSEL   &= ~(BIT_2 | BIT_6);
  PORT_D_DEN     |= BIT_2 | BIT_6;

  // Setup UART1 for DMX TX
  UART1_CTL       = 0x00000000;
  UART1_IBRD      = 0x0000001E;
  UART1_FBRD      = 0x00000020;
  UART1_LCRH      = 0x00000078;
  UART1_CC        = 0x00000000;
  UART1_CTL       = 0x00000301;

	// Disable the power gating and enable the RS485 transceiver
	PORT_D_DATA |= BIT_2 | BIT_6;
}

void DMX::tick() {
  if((sys_now() - _last_refresh_ms) > _refresh_time_ms) {
    tx();
    _last_refresh_ms = sys_now();
  }
}

void DMX::tx() {
	// The break and mark words need to be bit banged
	// Send the break word
  PORT_B_AFSEL  &= ~(BIT_1);
  PORT_B_DATA   &= ~(BIT_1);
  usleep(100);

  // Send the mark word
  PORT_B_DATA   |= BIT_1;
  usleep(12);
  PORT_B_AFSEL  |= BIT_1;

  // Send the data bytes
  for(uint8_t index = 0; index < 11; index++) {
	  UART1_DR = _buffer[index];
    while(UART1_FR & BIT_5);  // Wait for fifo to have space
  }
}
