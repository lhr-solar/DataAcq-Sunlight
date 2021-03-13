#include "ethernet.h"

struct netif init_Struct;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

void ethernet_Init(){
    /* IP addresses initialization with DHCP (IPv4) */
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;

  /* add the network interface (IPv4/IPv6) with RTOS */
  err_t error = netif_add(&init_Struct, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  if (error != ERR_OK); //DO SOMETHING HERE IF THERE IS AN ERROR

  /* Registers the default network interface */
  netif_set_default(&init_Struct);
}

