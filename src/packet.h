#ifndef PACKET_H
#define PACKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef struct {
    int ttl;
    int time_out;
    int interval;
    char ip_dest[INET_ADDRSTRLEN];
    char ipaddr[INET_ADDRSTRLEN];
    char hostname[NI_MAXHOST];
    char iface[64];
} PingArgs;


#ifdef __APPLE__ // create icmphdr struct for macOS
#include <stdint.h>

struct icmphdr
{
    uint8_t type;       /* message type */
    uint8_t code;       /* type sub-code */
    uint16_t checksum;
    union
    {
        struct
        {
            uint16_t id;
            uint16_t sequence;
        } echo;          /* echo request/reply */
        uint32_t gateway; /* gateway address */
        struct
        {
            uint16_t __unused;
            uint16_t mtu;
        } frag;          /* path mtu discovery */
    } un;
};
#endif


// define ttl
#define TTL 32




#endif //PACKET_H

