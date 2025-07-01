#include "packet.h"
#include "net_utils.h"
#include "argsparse.h"


int main(int argc, char* argv[]) {
    uint8_t ip_buffer[65536], icmp_type, icmp_code, hlen;
    PingArgs args;
    int count = 0, arrived = 0;
    parse_args(argc, argv, &args);


    char transit_hostname[NI_MAXHOST];
    char transit_ipaddr[INET_ADDRSTRLEN];

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (sockfd < 0) {
        perror("Error: Cannot create socket");
        return 1;
    }

    struct sockaddr_in dest, src;
    dest.sin_family = AF_INET;
    inet_pton(AF_INET, args.ip_dest, &dest.sin_addr);


    socklen_t addrlen = sizeof(src);

    struct timeval timeout, start_time, end_time;
    timeout.tv_sec = args.time_out;
    timeout.tv_usec = 0;

    printf("route trace to %s [%s], max hops %d\n", args.hostname, args.ip_dest, args.ttl);

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error: Failed to set timeout");
        return 1;
    }

    int optval = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0) {
        perror("Error: Failed to craft IP Packet");
        exit(EXIT_FAILURE);
    }


    while (count < args.ttl) {
        printf("HOP %d\n", count+1);
        printf("-----------\n");

        for (int i = 0; i<3; i++) {


            char ip_packet[sizeof(struct ip) + sizeof(struct icmphdr)];
            memset(ip_packet, 0, sizeof(ip_packet));

            craft_ip_header(args.ip_dest, count+1, args.ipaddr, ip_packet, sizeof(struct ip) + sizeof(struct icmphdr));

            struct icmphdr* icmp_hdr = (struct icmphdr*)(ip_packet + sizeof(struct ip));
            craft_icmp_header(icmp_hdr, 0);

            get_time(&start_time);
            if (sendto(sockfd, ip_packet, sizeof(struct ip) + sizeof(struct icmphdr), 0, (struct sockaddr*) &dest, sizeof(dest)) < 0) {
                perror("Error: Failed to send the packet");
                return 1;
            }


            ssize_t bytes = recvfrom(sockfd, ip_buffer, sizeof(ip_buffer), 0, (struct sockaddr*) &src, &addrlen);
            get_time(&end_time);


            hlen = ip_buffer[0] & 0xf;
            icmp_type = ip_buffer[hlen*4];
            icmp_code = ip_buffer[(hlen*4) +1];
            inet_ntop(AF_INET, &(src.sin_addr), transit_ipaddr, INET_ADDRSTRLEN);
            get_hostname_from_ip(transit_ipaddr, transit_hostname);


            if (bytes < 0) {
                fprintf(stdout, "Request timed out\n");
                continue;
            }


            if (icmp_type == 0 && icmp_code == 0) {
                printf("%s [%s] rtt=%.2fms\n",  args.hostname, transit_ipaddr, print_time(&start_time, &end_time));
                arrived = 1;
            }
            else if (icmp_type == 3) {
                printf(" Destination Unreachable");
                switch (icmp_code) {
                    case 0:
                        printf(": Network Unreachable");
                        break;
                    case 1:
                        printf(": Host Unreachable");
                        break;
                    case 9:
                        printf(": Communication with Destination Network is Administratively Prohibited");
                        break;
                    case 10:
                        printf(": Communication with Destination Host is Administratively Prohibited");
                        break;
                    default:
                        printf(": ICMP Error Code %d", icmp_code);
                }
                printf("\n");
            }
            else if (icmp_type == 11) {
                printf("%s [%s] rtt=%.2fms\n", transit_hostname, transit_ipaddr, print_time(&start_time, &end_time));
            }

            else printf("  ICMP Error: ICMP Type %d ICMP Code %d\n", icmp_type, icmp_code);
        }

        if (arrived == 1) break;

        printf("\n\n");
        count++;

        sleep(args.interval);

    }

    printf("\n----trace complete----\n");

    close(sockfd);
    return 0;
}