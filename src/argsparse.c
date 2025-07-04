#include "packet.h"
#include "net_utils.h"

void print_help() {
    printf("Usage: ./routetrace dest_ip (in IPv4) (or hostname) [-f <interface> -m <max ttl> -t <timeout> -i <interval> -h]\n\n");
    printf("Options:\n");
    printf("  -f <interface>   Specify the network interface to use (default: eth0 (or en0))\n");
    printf("  -m <max ttl>     Set the maximum TTL (Time-To-Live) for packets (default: 64)\n");
    printf("  -t <timeout>     Response timeout in seconds (default: 2 seconds)\n");
    printf("  -i <interval>    Interval between sending packets in seconds (default: 1 second)\n");
    printf("  -v               Show detailed packet information (IP & ICMP headers)\n");
    printf("  -x               Show raw packet information in bytes\n");
    printf("  -h               Display this help menu and exit\n\n");
}

// Parse command line arguments
void parse_args(int argc, char* argv[], PingArgs* args) {
    // Check if enough arguments are provided (at least destination IP)
    if (argc <= 1) {
        fprintf(stderr, "Error: Insufficient arguments!\n");
        fprintf(stderr, "Usage: %s dest_ip [-f <interface> -m <max ttl> -t <timeout> -i <interval> -h]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Print Help menu
    if (strcmp(argv[1], "-h") == 0) {
        print_help();
        exit(EXIT_SUCCESS);
    }

    // Default ping arguments
    args->ttl = TTL;                // Default TTL value
    args->time_out = 2;             // Default timeout in seconds
    args->interval = 1;             // Default interval between ping ping in seconds
    args->hostname[0] = '\0';       // Store destination hostname (or IP address)
    args->iface[0] = '\0';

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        // Handle the -f flag for specifying network interface
        if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: Missing argument after -f\n");
                exit(EXIT_FAILURE);
            }
            strncpy(args->iface, argv[i + 1], sizeof(args->iface) - 1);
            args->iface[sizeof(args->iface) - 1] = '\0';
            i++; // Move to the next argument
        }
        // Handle the -m flag for setting maximum TTL (max hops)
        else if (strcmp(argv[i], "-m") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: Missing argument after -m\n");
                exit(EXIT_FAILURE);
            }
            // Convert the argument to a long integer using strtol
            char *endptr;
            errno = 0;

            // Attempt to convert argument to long integer
            long temp = strtol(argv[i + 1], &endptr, 10);

              // Check for errors (Underflow, Overflow and Invalid number)
            if (errno == ERANGE || temp <= 0 || temp > 255 || *endptr != '\0') {
                fprintf(stderr, "Error: Invalid TTL value. Must be a number between 1 and 255.\n");
                exit(EXIT_FAILURE);
            }
            args->ttl = (int)temp;
            i++;
        }
        // Handle the -t flag for setting the timeout in seconds
        else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: Missing argument after -t\n");
                exit(EXIT_FAILURE);
            }
            char *endptr;
            errno = 0;
            long temp = strtol(argv[i + 1], &endptr, 10);

            if (errno == ERANGE || temp <= 0 || *endptr != '\0') {
                fprintf(stderr, "Error: Timeout must be an integer greater than 0.\n");
                exit(EXIT_FAILURE);
            }
            args->time_out = (int)temp;
            i++;
        }
        // Handle the -i flag for setting the interval between pings
        else if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: Missing argument after -i\n");
                exit(EXIT_FAILURE);
            }
            char *endptr;
            errno = 0;
            long temp = strtol(argv[i + 1], &endptr, 10);

            if (errno == ERANGE || temp <= 0 || *endptr != '\0') {
                fprintf(stderr, "Error: Interval must be an integer greater than 0.\n");
                exit(EXIT_FAILURE);
            }
            args->interval = (int)temp;
            i++;
        }
        // Print Help Menu
        else if (strcmp(argv[i], "-h") == 0) {
            print_help();
            exit(EXIT_SUCCESS);
        }

        // Get destination hostname
        else if (argv[i][0] != '-') {
            if (args->hostname[0] == '\0') {
                strncpy(args->hostname, argv[i], NI_MAXHOST - 1);
                args->hostname[NI_MAXHOST - 1] = '\0';
                get_ip_from_hostname(args->hostname, args->ip_dest);
            }
            else {
                fprintf(stderr, "Error: Multiple inputs provided for destination\n");
                exit(EXIT_FAILURE);
            }
        }

        // Handle unknown flags
        else {
            fprintf(stderr, "Error: Unknown argument %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    // Set destination IP address to 127.0.0.1, if source is localhost (127.0.0.1)
    if (strcmp(args->ip_dest, "127.0.0.1") == 0) {
        strncpy(args->ipaddr, "127.0.0.1", INET_ADDRSTRLEN);
        return;
    }

    if (args->hostname[0] == '\0') {
        fprintf(stderr, "Error: Destination not provided!\n");
        exit(EXIT_FAILURE);
    }

    // If interface is not provided Get local IP Address automatically
    if (args->iface[0] == '\0') {
        autodetect_local_ip(args->ipaddr);
    }
    // Get local IP address from specified Network Interface
    else{
        if (get_local_ip(args->iface, args->ipaddr) == -1) {
            fprintf(stderr, "Error: Interface %s not found!\n", args->iface);
            exit(EXIT_FAILURE);
        }
    }
}
