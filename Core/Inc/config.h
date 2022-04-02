/**
 * @file config.h
 * @brief Configuration parameters for Sunlight
 */

// IP Address Configuration
#define IP4_CLIENT_ADDRESS          169, 254, 48, 100
#define IP4_SERVER_ADDRESS          169, 254, 48, 219
#define IP4_NETMASK                 255, 255, 0, 0
#define IP4_GATEWAY                 0, 0, 0, 0
#define SERVER_PORT                 65432




















// Helper Macros
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> sunlight as a client

// Function version of the macro LWIP_MAKEU32() in def.h
static inline u32_t lwip_makeu32_func(u32_t a, u32_t b, u32_t c, u32_t d) {
  return  (((u32_t)((a) & 0xff) << 24) |
           ((u32_t)((b) & 0xff) << 16) |
           ((u32_t)((c) & 0xff) << 8)  |
           (u32_t)((d) & 0xff));
}
<<<<<<< HEAD
=======
>>>>>>> ethernet fixes
=======
>>>>>>> sunlight as a client
#define CFG_IP4_SETADDR(ipaddr, ipu32) (ipaddr)->addr = PP_HTONL(lwip_makeu32_func(ipu32))
