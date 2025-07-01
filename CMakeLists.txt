cmake_minimum_required(VERSION 3.30)
project(routetrace C)

set(CMAKE_C_STANDARD 11)

add_executable(routetrace src/main.c
        src/net_utils.c
        src/packet.h
        src/argsparse.c
        src/argsparse.h
        src/net_utils.h)
