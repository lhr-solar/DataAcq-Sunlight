#include "radio.h"

#define FIFO_NAME ethernet_rx //This fifo is for recieving data from ethernet(could be commands)
#include "fifo.h"

struct netif init_Struct;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

void radio_Init(void){
  // IP addresses initialization with DHCP (IPv4) 
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;
  // add the network interface (IPv4/IPv6) with RTOS 
  netif_add(&init_Struct, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  // Registers the default network interface 
  netif_set_default(&init_Struct);
  if (netif_is_link_up(&gnetif)){
    netif_set_up(&gnetif); // When the netif is fully configured this function must be called
  }
  else{
    netif_set_down(&gnetif); // When the netif link is down this function must be called
  }
  // Set the link callback function, this function is called on change of link status
  netif_set_link_callback(&gnetif, ethernetif_update_config);
  // create semaphore used for informing ethernetif of frame reception
  Netif_LinkSemaphore = osSemaphoreNew(1, 1, NULL); 
  link_arg.netif = &gnetif;
  link_arg.semaphore = Netif_LinkSemaphore;
  // Create the Ethernet link handler thread
  memset(&attributes, 0x0, sizeof(osThreadAttr_t));
  attributes.name = "Radio_Thread";
  attributes.stack_size = INTERFACE_THREAD_STACK_SIZE;
  attributes.priority = osPriorityBelowNormal;
  osThreadNew(ethernetif_set_link, &link_arg, &attributes);
  // Start DHCP negotiation for a network interface (IPv4)
  dhcp_start(&gnetif);
}

ErrorStatus radio_RX(void* data){
  ethernetif_input(data);
  return SUCCESS;
}

ErrorStatus radio_TX(CANMSG_t data){
  return SUCCESS;
}
