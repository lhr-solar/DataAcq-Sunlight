#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Configuration parameters for Sunlight
 */

// DEBUG configuration
#define DEBUGGINGMODE               0       // set to 1 to enable debug statements, set to 0 or comment out to disable
#define CAN_LOOPBACK                1       // set to 1 to test CAN interface in loopback mode

// IP Address Configuration
#define IP4_CLIENT_ADDRESS          169, 254, 48, 100
#define IP4_SERVER_ADDRESS          169, 254, 48, 219
#define IP4_NETMASK                 255, 255, 0, 0
#define IP4_GATEWAY                 0, 0, 0, 0
#define SERVER_PORT                 65432




















// Helper Macros

#ifdef LWIP_HDR_DEF_H // def.h

// Function version of the macro LWIP_MAKEU32() in def.h
static inline u32_t lwip_makeu32_func(u32_t a, u32_t b, u32_t c, u32_t d) {
    return (((u32_t)((a) & 0xff) << 24) |
            ((u32_t)((b) & 0xff) << 16) |
            ((u32_t)((c) & 0xff) << 8)  |
            (u32_t)((d) & 0xff));
}
#define CFG_IP4_SETADDR(ipaddr, ipu32) do {(ipaddr)->addr = PP_HTONL(lwip_makeu32_func(ipu32));} while (0)

#endif // LWIP_HDR_DEF_H

#endif // CONFIG_H
