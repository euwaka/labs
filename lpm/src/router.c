#include "router.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>



// ---------------------------------------------------------------------------------------



void r_printBinary(u32 num, u8 prefix) {
    int bitsPrinted = 0;
    for (int i = 31; i >= 0 && bitsPrinted < prefix; i--) {
        printf("%d", (num >> i) & 1);
        bitsPrinted++;

        // Print a space every 4 bits for readability
        if (bitsPrinted % 4 == 0 && bitsPrinted < prefix) {
            printf(" ");
        }
    }
    printf("\n");
}


void r_printIP(const IPv4_t ip) {
    printf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

u32 r_packIP(const IPv4_t ip) {
    u32 result = 0x0000;
    result |= (u32)ip[0] << 24;
    result |= (u32)ip[1] << 16;
    result |= (u32)ip[2] << 8;
    result |= (u32)ip[3] << 0;

    return result;
}

void r_unpackIP(u32 ip, u8 out[R_NUMBER_OCTETS_IPV4]){
    out[0] = (ip >> 24) & 0xFF;
    out[1] = (ip >> 16) & 0xFF;
    out[2] = (ip >> 8)  & 0xFF;
    out[3] = ip & 0xFF;
}

subnet_t r_convertStringSubnet(char *buf, size_t len) {
    subnet_t subnet = {0};

    // the index telling which part of the IP address (out of 4 parts) is currently
    // being retrieved.
    short ip_part_idx = 0; 
    u8 number = 0;
    for (size_t i = 0; i < len; ++i) {
	char symbol = buf[i];
	
	if (symbol != '\0' && symbol != '.' && symbol != '/' && !isdigit(symbol)) {
	    fprintf(stderr, "subnet string contains unknown symbol: %c.\n", symbol);
	    abort();
	}

	// examine all possible cases for the current symbol
	switch (symbol) {
	case '\0':
	    subnet.mask = number;
	    break;
	case '.':
	case '/':
	    subnet.ip[ip_part_idx++] = number;
	    number = 0;
	    break;
	default:
	    number = number * 10 + (symbol - '0');
	}
    }
   
    return subnet;
}

void r_convertStringIPv4(char *buf, size_t len, IPv4_t *ip) {
    // the index telling which part of the IP address (out of 4 parts) is currently
    // being retrieved.
    short ip_part_idx = 0; 
    u8 number = 0;
    for (size_t i = 0; i < len; ++i) {
	char symbol = buf[i];
	
	if (symbol != '\0' && symbol != '.' && !isdigit(symbol)) {
	    fprintf(stderr, "IP string contains unknown symbol: %c.\n", symbol);
	    abort();
	}

	// examine all possible cases for the current symbol
	switch (symbol) {
	case '.':
	    (*ip)[ip_part_idx++] = number;
	    number = 0;
	    break;
	case '\0':
	    (*ip)[ip_part_idx] = number;
	    break;
	default:
	    number = number * 10 + (symbol - '0');
	}
    }
}



// ---------------------------------------------------------------------------------------



routes_leaf_t r_emptyLeaf() {
    return (routes_leaf_t){0};
}

void r_uploadLeafInfo(routes_leaf_t *leaf, subnet_t subnet, u8 interface) {
    leaf->interface = interface;
    leaf->subnet = subnet;
}

static void _printSubnet(const subnet_t subnet) {
    printf("%d.%d.%d.%d/%d",
	   subnet.ip[0],
	   subnet.ip[1],
	   subnet.ip[2],
	   subnet.ip[3],
	   subnet.mask
	);
}

void r_printRoutingLeaf(const routes_leaf_t leaf) {
    _printSubnet(leaf.subnet);
    printf(" ===> %d\n", leaf.interface);
} 

routes_t r_newRoutingTable() {
    routes_t trie = (routes_t)malloc( sizeof(routes_node_t) );
    assert(trie != NULL);

    trie->data = r_emptyLeaf();
    trie->left = NULL;
    trie->right = NULL;
    trie->to = NULL;
    trie->type = R_ROOT;

    return trie;
}

routes_t r_newBranchNode(routes_t left, routes_t right) {
    routes_t trie = (routes_t)malloc( sizeof(routes_node_t) );
    assert(trie != NULL);

    trie->data = r_emptyLeaf();
    trie->left  = left;
    trie->right = right;
    trie->to = NULL;
    trie->type = R_BRANCH;

    return trie;
}

routes_t r_newLeafNode(u8 interface, subnet_t subnet) { 
    routes_t trie = (routes_t)malloc( sizeof(routes_node_t) );
    assert(trie != NULL);

    trie->data = r_emptyLeaf();
    r_uploadLeafInfo(&trie->data, subnet, interface);
    
    trie->left = NULL;
    trie->right = NULL;
    trie->to = NULL;
    trie->type = R_LEAF;

    return trie;
}

u8 r_findInterface(routes_t routes, const IPv4_t ip_arr) {
    u32 ip = r_packIP(ip_arr);

    routes_t best_match = NULL;
    for (size_t i = 1; i <= R_IP_BIT_SIZE; ++i) {
	int bit = ip & (0x0001 << (R_IP_BIT_SIZE - i));

	if (routes->to != NULL)
		best_match = routes->to;
	
        if (bit == 0) {    
	    if (routes->left != NULL) {
		routes = routes->left;
		continue;
	    }
	    break;
	}
	
	// otherwise, bit is 1
	if (routes->right != NULL) {
	    routes = routes->right;
	    continue;
	}
	
	break;
    }

    if (best_match == NULL) {
	fprintf(stderr, "failed to find the interface for IP.\n");
    }

    // choose the last found interface.
    return best_match->data.interface;
}

void r_addRoute(routes_t routes, subnet_t subnet, u8 interface) {
    if (!routes) {
	fprintf(stderr, "cannot add subnet/interface to NULL.\n");
	abort();
    }
    
    u32 ip = r_packIP(subnet.ip);
    u8 mask = subnet.mask;

    routes_t previous = routes;
    for (u8 i = 1; i <= mask + 1; ++i) {
	// extrapolate current bit on IP
	u32 bit = ip & (0x0001 << (R_IP_BIT_SIZE - i));
	
	if (bit != 0) {
	    if (routes->right == NULL)
		routes->right = r_newBranchNode(NULL, NULL);

	    previous = routes;
	    routes = routes->right;
	    continue;
	}

	// bit is 0
	if (routes->left == NULL)
	    routes->left = r_newBranchNode(NULL, NULL);

	previous = routes;
	routes = routes->left;
    }
    
    if (previous->to == NULL)
	previous->to = r_newLeafNode(interface, subnet);
    else {
	routes_t leaf = previous->to;
	r_uploadLeafInfo(&leaf->data, subnet, interface);
    }
}

void r_freeRoutes(routes_t routes) {
    if (routes == NULL)
	return;

    if (routes->type != R_LEAF) {
	r_freeRoutes(routes->left);
	r_freeRoutes(routes->right);
	r_freeRoutes(routes->to);
    }
    free(routes);
}

static void _printRoutes(routes_t routes, u32 ip, u8 idx) {
    if (routes == NULL)
	return;

    if (routes->type == R_LEAF) {
	IPv4_t ip_arr;
	r_unpackIP(ip, ip_arr);

	printf("Trie-branch is ");
	r_printIP(ip_arr);
	printf(", ");
	r_printBinary(ip, idx - 1);
        r_printRoutingLeaf(routes->data);
	printf("\n");
    }

    if (routes->left != NULL)
	_printRoutes(routes->left, ip, idx + 1);
    if (routes->right != NULL) {
	u32 new_ip = ip | (0x0001 << (R_IP_BIT_SIZE - idx));
	_printRoutes(routes->right, new_ip, idx + 1);
    }
    if (routes->to != NULL)
	_printRoutes(routes->to, ip, idx);
}

void r_printRoutes(routes_t routes) {
    _printRoutes(routes, 0x0000, 1);
}



// ---------------------------------------------------------------------------------------
