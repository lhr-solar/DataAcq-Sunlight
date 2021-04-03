#include "radio.h"

#define CMD_SIZE 64

static struct netif radio_Struct;
static ip4_addr_t ipaddr;
static ip4_addr_t netmask;
static ip4_addr_t gw;
osSemaphoreId radio_RXSemaphore;
static struct link_str radio_arg;
osThreadAttr_t attributes;
static struct pbuf* radio_RXBuffer;
static struct pbuf* radio_TXBuffer;

void radio_Init(void){
  // IP addresses initialization with DHCP (IPv4) 
  ipaddr.addr = 0; //THIS MUST BE CHANGED WHEN WE FIND OUT WHAT GOES HERE
  netmask.addr = 0;
  gw.addr = 0;
  // add the network interface (IPv4/IPv6) with RTOS 
  netif_add(&radio_Struct, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  // Registers the default network interface 
  netif_set_default(&radio_Struct);
  if (netif_is_link_up(&radio_Struct)){
    netif_set_up(&radio_Struct); // When the netif is fully configured this function must be called
  }
  else{
    netif_set_down(&radio_Struct); // When the netif link is down this function must be called
  }
  // Set the link callback function, this function is called on change of link status
  netif_set_link_callback(&radio_Struct, ethernetif_update_config);
  // create semaphore used for informing ethernetif of frame reception
  radio_RXSemaphore = osSemaphoreNew(1, 1, NULL); 
  radio_arg.netif = &radio_Struct;
  radio_arg.semaphore = radio_RXSemaphore;
  // Create the Ethernet link handler thread
  attributes.name = "Radio_Thread";
  attributes.stack_size = 1024;
  attributes.priority = osPriorityBelowNormal;
  memset(&attributes, 0x0, sizeof(osThreadAttr_t));
  osThreadNew(ethernetif_set_link, &radio_arg, &attributes);
  // Start DHCP negotiation for a network interface (IPv4)
  dhcp_start(&radio_Struct);
  radio_RXBuffer = pbuf_alloc(PBUF_RAW_TX, 64, PBUF_RAM);
  radio_TXBuffer = pbuf_alloc(PBUF_LINK, 64, PBUF_RAM);
}

ErrorStatus radio_RX(void* data){
  err_t error = radio_Struct.input(radio_RXBuffer, &radio_Struct);
  if (error != ERR_OK){} //DO SOMETHING IF ERROR OCCURRED
  int x = pbuf_get_contiguous(radio_RXBuffer, data, CMD_SIZE, CMD_SIZE, 0);
  if (!x) return ERROR;
  return SUCCESS;
}

ErrorStatus radio_TX(CANMSG_t data){
  err_t error = pbuf_take(radio_TXBuffer, &data, sizeof(CANMSG_t));
  if (error != ERR_OK){ return ERROR;} //DO SOMETHING ELSE IF ERROR OCCURRED (LIKE MAKING BUFFER LARGER)
  radio_Struct.linkoutput(&radio_Struct, radio_TXBuffer);
  return SUCCESS;
}
