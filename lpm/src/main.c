#include "router.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GETLINE_SIZE 256

/* Simplified implementation of the POSIX C getline function. */
int getline(char **buf, size_t *len, FILE *stream) {
    if (len == NULL || stream == NULL) {
        return -1;
    }

    if (*buf == NULL) {
	*buf = (char *)malloc(sizeof(char) * GETLINE_SIZE);
	if (*buf == NULL) {
	    return -1;
	}
	*len = GETLINE_SIZE;
    }

    size_t pos = 0;
    char c;
    while((c = fgetc(stream)) != EOF && c != '\n') {
        if (pos >= *len - 1) {
	    *len *= 2;
	    *buf = (char *)realloc(*buf, *len);
	    if (*buf == NULL) {
	        return -1;
	    }
	}

	(*buf)[pos++] = c;
    }

    (*buf)[pos] = '\0';

    if (c == EOF && pos == 0) {
        return -1;
    }
    
    return pos;
}

int main() {
    // retrieve the number of subnets
    int n;
    if (scanf("%d\n", &n) != 1) {
	fprintf(stderr, "could not read the number of subnets n.\n");
	abort();
    }

    routes_t routes = r_newRoutingTable();
    
    // retrieve actual subnets
    for (int i = 0; i < n; ++i) {
	char *buf = NULL;
	size_t len   = 0;
	
	if (getline(&buf, &len, stdin) == -1) {
	    fprintf(stderr, "could not read the line (subnet).\n");
	    free(buf);
	    abort();
	}

	// parse "IP INTERFACE" buffer using tokens
	char *subnet_str = strtok(buf, " ");

	// add 1 to include \0
	subnet_t subnet = r_convertStringSubnet(subnet_str, strlen(subnet_str) + 1);

	char *interface_str = strtok(NULL, " ");
	u8 interface = (u8)atoi(interface_str);
	
	// update the routing table
	r_addRoute(routes, subnet, interface);
	
	free(buf);
    }

    // r_printRoutes(routes);
    
    // retrieve the number of IP addresses
    int m;
    if (scanf("%d\n", &m) != 1) {
	fprintf(stderr, "could not read the number of IP addresses m.\n");
	abort();
    }

    for (int i = 0; i < m; ++i) {
	char *buf = NULL;
	size_t len = 0;
	
	if (getline(&buf, &len, stdin) == -1) {
	    fprintf(stderr, "could not read the line (IP).\n");
	    free(buf);
	    abort();
	}
	
	IPv4_t ip = {0};
        r_convertStringIPv4(buf, strlen(buf) + 1, &ip); // count the terminating sign
	// r_printIP(ip);
        // printBinary(r_packIP(ip));
	printf("%d\n", r_findInterface(routes, ip));
	
	free(buf);
    }

    r_freeRoutes(routes);
    
    return 0;
}
