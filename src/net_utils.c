#include "packet.h"

uint16_t inet_checksum(void* data, size_t length) {
    uint32_t sum = 0;
    const uint16_t* buf = (const uint16_t*)data;

    while (length > 1) {
        sum += *buf++;
        length -= 2;
    }

    if (length == 1) {
        sum += *(uint8_t *)buf;
    }

    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return ~sum;
}


void get_ip_from_hostname(char *hostname, char *ipaddr) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    int code = getaddrinfo(hostname, NULL, &hints, &res);

    if (code != 0) {
        fprintf(stderr, "Error: %s\n", gai_strerror(code));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *s = (struct sockaddr_in *) res->ai_addr;
    inet_ntop(AF_INET, &(s->sin_addr), ipaddr, INET_ADDRSTRLEN);
    freeaddrinfo(res);
}

void get_hostname_from_ip(char *ipaddr, char *hostname) {
    struct sockaddr_in s;
    memset(&s, 0, sizeof(s));
    s.sin_family = AF_INET;

    if (inet_pton(AF_INET, ipaddr, &(s.sin_addr)) <= 0) {
        strncpy(hostname, ipaddr, INET_ADDRSTRLEN);
        return;
    }

    if (getnameinfo((struct sockaddr *)&s, sizeof(s), hostname, NI_MAXHOST , NULL, 0, NI_NAMEREQD) != 0) {
        strncpy(hostname, ipaddr, INET_ADDRSTRLEN);
    }
}

// Function to get local IP address from a specified Network Interface
int get_local_ip(char* iface, char* dest) {
    // Create a socket (for ioctl parameter)
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, "Error getting IP address");
        exit(EXIT_FAILURE);
    }

    // Use ioctl to get host IP Address
    struct ifreq ifr;
    strncpy(ifr.ifr_name, iface, IFNAMSIZ-1);

    // Query for host IP Address
    if (ioctl(sockfd, SIOCGIFADDR, &ifr) == -1) {
        fprintf(stderr, "No IP Address found for interface %s\n", ifr.ifr_name);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Get IP Address from ifr
    const struct sockaddr_in *s = (struct sockaddr_in*) &ifr.ifr_addr;
    inet_ntop(AF_INET, &(s->sin_addr.s_addr), dest, INET_ADDRSTRLEN);
    close(sockfd);

}


// Automatically get local IP Address (without specifying an interface)
void autodetect_local_ip(char* addr) {
    // Create a placeholder UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, "Error getting IP address");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in s, local_ip;
    // Sample address
    s.sin_addr.s_addr = inet_addr("8.8.8.8");
    s.sin_family = AF_INET;
    s.sin_port = htons(53); // DNS (port 53) set as demo port

    // UDP connection
    if (connect(sockfd, (struct sockaddr*)&s, sizeof(s)) < 0) {
        perror("Error: Failed to determine local IP Address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Get current address to which the socket sockfd is bound
    socklen_t addrlen = sizeof(local_ip);
    if (getsockname(sockfd, (struct sockaddr*)&local_ip, &addrlen) < 0) {
        perror("Error: Failed to determine local IP Address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Store the local ip address
    inet_ntop(AF_INET, &(local_ip.sin_addr.s_addr), addr, INET_ADDRSTRLEN);
    close(sockfd);
}


void craft_ip_header(char* dest, int ttl, char* ipaddr, char* ip_packet, size_t tot_len) {
    struct ip* iphdr = (struct ip*) ip_packet;
    iphdr->ip_v = 0x4;
    iphdr->ip_hl = 0x5;
    iphdr->ip_tos = 0x0;
    iphdr->ip_ttl = ttl;
    iphdr->ip_len = htons(tot_len);

    iphdr->ip_id = arc4random() & 0xffff;
    iphdr->ip_off = htons(IP_DF); // do not fragment
    iphdr->ip_p = IPPROTO_ICMP; // icmp

    inet_pton(AF_INET, ipaddr, &iphdr->ip_src);
    inet_pton(AF_INET, dest, &iphdr->ip_dst);

    iphdr->ip_sum = 0;
    iphdr->ip_sum = inet_checksum((u_int16_t*) iphdr, sizeof(struct ip));
}

void craft_icmp_header(struct icmphdr* icmp_hdr, int seq) {
    memset(icmp_hdr, 0, sizeof(struct icmphdr));
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0x0;
    icmp_hdr->un.echo.id = arc4random() & 0xffff;
    icmp_hdr->un.echo.sequence = htons(seq);

    icmp_hdr->checksum = 0;
    icmp_hdr->checksum = inet_checksum((u_int16_t*) icmp_hdr, sizeof(struct icmphdr));
}



void get_time(struct timeval* t) {
    gettimeofday(t, NULL);
}

double print_time(struct timeval* start_time, struct timeval* end_time) {
    double start = (start_time->tv_sec * 1000.0) + (start_time->tv_usec / 1000.0);
    double end = (end_time->tv_sec * 1000.0) + (end_time->tv_usec / 1000.0);

    return end-start;
}