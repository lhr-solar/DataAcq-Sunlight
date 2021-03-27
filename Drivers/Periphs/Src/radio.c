#include "Ethernet.h"

#define FIFO_NAME ethernet_rx //This fifo is for recieving data from ethernet(could be commands)
#include "fifo.h"
/*
struct netif init_Struct;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

void Ethernet_Init(void){
  // IP addresses initialization with DHCP (IPv4) 
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;
  // add the network interface (IPv4/IPv6) with RTOS 
  netif_add(&init_Struct, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  // Registers the default network interface 
  netif_set_default(&init_Struct);
}

ErrorStatus Ethernet_RX(void* data){
  ethernetif_input(data);
  return SUCCESS;
}
*/