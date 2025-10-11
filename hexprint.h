/* hexprint.h - Helper functions for z88dk hex printing */
#ifndef HEXPRINT_H
#define HEXPRINT_H

#include <stdio.h>

/* Print a single hex byte with leading zero if needed */
static void print_hex_byte(unsigned char value) {
    if (value < 16) {
        printf("0%x", value);
    } else {
        printf("%x", value);
    }
}

/* Print MAC address in xx.xx.xx.xx.xx.xx format */
static void print_mac(unsigned char *mac) {
    int i;
    for (i = 0; i < 6; i++) {
        print_hex_byte(mac[i]);
        if (i < 5) printf(".");
    }
}

#endif /* HEXPRINT_H */
