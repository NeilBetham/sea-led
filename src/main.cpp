#include "buffer.h"
#include "utils.h"
#include "status_led.h"
#include "color.h"
#include "ethernet/driver.h"
#include "registers/sysctl.h"
#include "registers/gpio.h"
#include "registers/uart.h"
#include "logging.h"
#include "timing.h"
#include "uart.h"
#include "socket.h"
#include "httpd.h"
#include "app.h"
#include "light_controller.h"
#include "dmx.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "lwip/dhcp.h"

ethernet::Driver<1550, 10> enet_driver;
UART uart0(UART0_BASE, 115200);
LightController light_controller;

void EthernetMac_ISR(void) {
  enet_driver.interrupt_handler();
  sleep_int();
}

int main(void){
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
  // Enable UART and associated GPIO port
  PORT_A_AFSEL   = 0x000000FF;
  PORT_A_PCTL    = 0x00000011;
  PORT_A_DEN     = 0x000000FF;

  // Setup UART
  uart0.init();
  logging_init(&uart0);
  logging_set_log_level(LogLevel::info);
  log_i("Hello World!");


  // ============== Setup EMAC / PHY =====================
  enet_driver.init();


  // =============== Setup Peripherals  =======================
  init_status_led();


  // ============== Enable SysTick ======================
  systick_init();


  // Enable interrupts
  CORE_EN0 = 0xFFFFFFFF;
  CORE_EN1 = 0xFFFFFFFF;
  CORE_EN2 = 0xFFFFFFFF;
  CORE_EN3 = 0xFFFFFFFF;
  GIE()

  // Flash some LEDs to make it known we are alive
  set_status_led(1, 0, 0);
  sleep(1000);
  set_status_led(0, 1, 0);
  sleep(1000);
  set_status_led(0, 0, 1);
  sleep(1000);
  set_status_led(1, 1, 1);
  sleep(1000);
  set_status_led(0, 0, 0);

  // DHCP start
  err_t ret = dhcp_start(&enet_driver.netif());
  if(ret == ERR_MEM) {
    log_e("Failed to start dhcp");
  }

	// Setup all the application components
  HTTPD http_server(80);
  App app;
	DMX dmx;
	dmx.init(50);
  http_server.set_delegate(&app);
  app.set_delegate(&light_controller);
  http_server.start();

	uint32_t loop_start_time = 0;
	constexpr uint32_t LOOP_TIME_MS = 50;

  while(1) {
		loop_start_time = sys_now();

		// Get the current color and configure DMX with it
		light_controller.get_colors(loop_start_time, &dmx.color());

		// See if the DMX driver has shit to do
		dmx.tick();

    // See if the ethernet driver has shit to do
    enet_driver.tick();

    // Find out how long it took to execute the loop
    uint32_t exec_time = sys_now() - loop_start_time;
		uint32_t sleep_time = (LOOP_TIME_MS - exec_time);

    if(sleep_time > 0 && sleep_time <= LOOP_TIME_MS) {
      sleep(sleep_time);
    } else {
			log_w("Main loop exceeded exec time of {}ms: {}ms", LOOP_TIME_MS, exec_time);
		}
  }
}
