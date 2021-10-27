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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "lwip/api.h"
#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/timeouts.h"
#include "lwip/tcp.h"

ethernet::Driver<1550, 10> enet_driver;
UART uart0(UART0_BASE, 115200);

void EthernetMac_ISR(void) {
  enet_driver.interrupt_handler();
}

template <typename T>
err_t eth_tcp_recv(void* state, struct tcp_pcb* conn, struct pbuf* data, err_t err) {
  log_d("Connection received data: {}", data->tot_len);
  tcp_write(conn, data->payload, data->len, TCP_WRITE_FLAG_COPY);

  return ERR_OK;
}

template <typename T>
err_t eth_tcp_accept(void* state, struct tcp_pcb* conn, err_t error) {
  log_d("Connection received");
  tcp_recv(conn, eth_tcp_recv<T>);
  ((T*)state)->push_back(conn);

  return ERR_OK;
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
  logging_set_log_level(LogLevel::debug);
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

  // DHCP start
  err_t ret = dhcp_start(&enet_driver.netif());
  if(ret == ERR_MEM) {
    log_e("Failed to start dhcp");
  }

  // =================== LED Control ===================
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

  // Active connection list
  std::vector<struct tcp_pcb*> connections;
  connections.reserve(10);

  // Create the TCP buffer
  struct tcp_pcb* echo_server = tcp_new();
  if(echo_server == NULL) {
    log_e("Failed to create echo server");
  }

  // Bind the tcp buffer to port 23
  if(tcp_bind(echo_server, IP_ANY_TYPE, 4023) != ERR_OK) {
    log_e("Failed to bind tcp socket");
  }

  // Set the pcb arg
  tcp_arg(echo_server, (void*)&connections);

  // Set listen mode
  echo_server = tcp_listen_with_backlog(echo_server, 10);
  if(echo_server == NULL) {
    log_e("Listen failed");
  }

  // Set the accept conn callback
  tcp_accept(echo_server, eth_tcp_accept<std::vector<struct tcp_pcb*>>);

  while(1) {
    // Run driver tick
    enet_driver.tick();

    sleep(10);
/*
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
*/
  }
}
