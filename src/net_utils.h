#ifndef NET_UTILS_H
#define NET_UTILS_H

int get_local_ip(char* iface, char* dest);
void get_ip_from_hostname(char *hostname, char *ipaddr);
void get_hostname_from_ip(char *ipaddr, char *hostname);
void autodetect_local_ip(char* addr);
uint16_t inet_checksum(u_int16_t* data, size_t length);
void craft_ip_header(char* dest, int ttl, char* ipaddr, char* ip_packet, size_t tot_len);
void craft_icmp_header(struct icmphdr* icmp_hdr, int seq);
void get_time(struct timeval* t);
double print_time(struct timeval* start_time, struct timeval* end_time);

#endif //NET_UTILS_H
