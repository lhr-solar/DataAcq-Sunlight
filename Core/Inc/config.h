#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Configuration parameters for Sunlight
 */

// DEBUG Configuration
#define DEBUGGINGMODE               1       // set to 1 to enable debug statements, set to 0 or comment out to disable
#define CAN_LOOPBACK                0       // set to 1 to test CAN interface in loopback mode
#define HEARTBEAT_PERIOD            500     // period of heartbeat led (in # OS ticks)

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



#if DEBUGGINGMODE
    #define debugprintf(...)        printf(__VA_ARGS__)
#else
    #define debugprintf(...)        
#endif

#if DEBUGGINGMODE
#pragma message "Debugging mode is on!"
#endif

#if CAN_LOOPBACK
#pragma message "CAN is in loopback mode!"
#endif

#if DEBUGGINGMODE == 0 && CAN_LOOPBACK == 1
#error "CAN must be in normal mode if debugging is off"
#endif

#endif // CONFIG_H
