# routetrace

**routetrace** is a custom traceroute-style utility written in C that sends ICMP echo packets with increasing TTL (Time-To-Live) values to trace the route to a given destination.

## Features

- Sends ICMP Echo Requests with increasing TTLs to trace hops.
- Supports both IP addresses and hostnames.
- Select specific network interface for sending packets.
- Displays round-trip time (RTT) for each hop.
- Customizable TTL, timeout, maximum hops and interval.


## Usage

`./routetrace <destination> [-f <interface>] [-m <max ttl>] [-t <timeout>] [-i <interval>] [-v] [-x] [-h]`


## Command-line Arguments

**`<dest_ip | hostname>`**  
The destination to ping (IPv4 address or resolvable hostname)

**`-f <interface>`**  
Use a specific network interface

**`-m <max ttl>`**  
Set the maximum TTL (Time-To-Live) for outgoing packets (default: 64)

**`-c <count>`**  
Number of echo requests to send (default: 4)

**`-t <timeout>`**  
Timeout in seconds for each response (default: 2 seconds)

**`-i <interval>`**  
Interval between requests in seconds (default: 1 second)

**`-h`**  
Show help message and exit

##  Examples

```
./routetrace 8.8.8.8                  # Basic trace
./routetrace 192.168.1.1 -m 30 -t 5   # TTL 30, timeout 5s
./routetrace 8.8.8.8 -f eth0          # Use eth0 interface
