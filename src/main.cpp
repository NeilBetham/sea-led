#include "utils.h"
#include "status_led.h"
#include "ethernet/driver.h"
#include "registers/sysctl.h"
#include "registers/gpio.h"
#include "registers/wdt.h"
#include "registers/uart.h"
#include "registers/adc.h"
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
volatile bool test_mode_enabled = false;
volatile bool test_mode_next_state = false;
volatile uint32_t internal_temp = 0;  // This sucks but I don't feel like writing a ADC driver for it ATM

void EthernetMac_ISR(void) {
  enet_driver.interrupt_handler();
  sleep_int();
}

void GPIOPortD_ISR() {
  PORT_D_ICR |= BIT_0;
  test_mode_next_state = !test_mode_enabled;
}

void ADC0Sequence0_ISR() {
  // Pull the result from the FIFO
  uint32_t result = ADC_0_SSFIFO0;

  // Convert the result in to C
  internal_temp = (0x00937FFF - ((((75 * 0x00035555) >> 16) * result) << 4)) >> 16;

  // Clear the interrupt
  ADC_0_ISC |= BIT_0;
}

int main(void) {
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
  logging_set_log_level(LogLevel::warn);
  log_i("Hello World!");


  // ============== Setup EMAC / PHY =====================
  enet_driver.init();


  // =============== Setup Peripherals  =======================
  init_status_led();


  // ============== Enable SysTick ======================
  systick_init();

  // Enable the test mode button
  PORT_D_AFSEL &= ~(BIT_0);
  PORT_D_DEN   |= BIT_0;
  PORT_D_DIR   &= ~(BIT_0);
  PORT_D_IS    &= ~(BIT_0);
  PORT_D_IBE   &= ~(BIT_0);
  PORT_D_IEV   &= ~(BIT_0);
  PORT_D_IM    |= BIT_0;

  // Enable the ADC to read the temp sensor
  SYSCTL_RCGCADC |= BIT_0;
  ADC_0_ACTSS    =  0;
  ADC_0_EMUX     |= 0x0000EEE0;
  ADC_0_SSEMUX0  =  0;
  ADC_0_SSMUX0   =  0;
  ADC_0_SSCTL0   |= BIT_1 | BIT_2 | BIT_3;
  ADC_0_SSTSH0   |= 0x8;
  ADC_0_IM       |= BIT_0 | BIT_8;
  ADC_0_ACTSS    |= BIT_0;;


  // Enable interrupts
  CORE_EN0 = 0xFFFFFFFF;
  CORE_EN1 = 0xFFFFFFFF;
  CORE_EN2 = 0xFFFFFFFF;
  CORE_EN3 = 0xFFFFFFFF;
  GIE()

  // Flash some LEDs to make it known we are alive
  set_status_led(0xFF, 0, 0);
  sleep(1000);
  set_status_led(0, 0xFF, 0);
  sleep(1000);
  set_status_led(0, 0, 0xFF);
  sleep(1000);
  set_status_led(0xFF, 0xFF, 0xFF);
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
	constexpr uint32_t LOOP_TIME_MS = 25;

  while(1) {
		loop_start_time = sys_now();

    uint32_t color = 0;

    // Check if test mode has been requested
    if((test_mode_next_state != test_mode_enabled) && (test_mode_next_state == true)) {
      log_i("Test mode enabled");
      light_controller.handle_command_update(0xFF000000, 0xFFFF0000, 0x00FF0000, 0x0000FF00, 0xFF, 0x55, ColorMode::jump);
      test_mode_enabled = test_mode_next_state;
    } else if((test_mode_next_state != test_mode_enabled) && (test_mode_next_state == false)) {
      log_i("Test mode disabled");
      light_controller.handle_command_update(0xFF000000, 0xFFFF0000, 0x00FF0000, 0x0000FF00, 0xFF, 0x55, ColorMode::off);
      test_mode_enabled = test_mode_next_state;
    }

		// Get the current color and configure DMX with it
		light_controller.get_colors(loop_start_time, &color);
    dmx.color() = color;

    // Mirror the current color on the status LED
    uint8_t red = color & 0x000000FF;
    uint8_t green = (color & 0x0000FF00) >> 8;
    uint8_t blue = (color & 0x00FF0000) >> 16;
    set_status_led(red, green, blue);

		// See if the DMX driver has shit to do
		dmx.tick();

    // See if the ethernet driver has shit to do
    enet_driver.tick();

    // Trigger a temp sensor reading
    ADC_0_PSSI |= BIT_0;

    // Pet the WDT
    WDT0_ICR = 1;

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
