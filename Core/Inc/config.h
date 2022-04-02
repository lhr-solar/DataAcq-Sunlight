/**
 * @file config.h
 * @brief Configuration parameters for Sunlight
 */

// IP Address Configuration
#define IP4_ADDRESS             192, 168, 0, 100
#define IP4_NETMASK             255, 255, 0, 0




















// Helper Macros
#define CFG_IP4_SETADDR(ipaddr, ipu32) (ipaddr)->addr = PP_HTONL(lwip_makeu32_func(ipu32))
