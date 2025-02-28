/*
 * authors: Artur Topal (s5942128) + Lucas Chudy (s5732816)
 * date: 27/02/2025
 *
 * This header contains the structures and functions for handling the routing of IPv4
 * addresses via the router.
 *
 * All the functions and macros use prefix R_ or r_ that stands for "routes."
 */

#ifndef ROUTER_H
#define ROUTER_H

#include <stddef.h>
#include <stdint.h>

#define R_NUMBER_OCTETS_IPV4 4
#define R_IP_BIT_SIZE 32

typedef uint8_t u8;
typedef uint32_t u32;



// ---------------------------------------------------------------------------------------



/* Prints the first "prefix" bits of a 32-bit integer number. */
void r_printBinary(u32 num, u8 prefix);

/*
 * Type to store an IP address in the IPv4 format. Indexing starts from the left-most
 * octet of the IP. For instance, "192.88.50.2" is indexed as 0: 192, 1: 88, 2: 50, 3: 2.
 */
typedef u8 IPv4_t[R_NUMBER_OCTETS_IPV4];

/* Prints an IP address */
void r_printIP(const IPv4_t ip);

/* Packs an IP address into a 32-bit number. */
u32 r_packIP(const IPv4_t ip);

/* Unpacks an IP address from the 32-bit number. */
void r_unpackIP(u32 ip, u8 out[R_NUMBER_OCTETS_IPV4]);

/*
 * The subnet structure which holds the IP address of the subnet (the network and the
 * host parts), and the subnet mask which tells the number of bits in the IP address
 * corresponding to the network part.
 */
typedef struct {
    IPv4_t ip;
    u8 mask;
} subnet_t;

/*
 * This function takes a terminated string representation of a subnet, creates and
 * returns the corresponding subnet_t structure.
 * Assumption: Themis tests formats IP's and subnets correctly.
 */
subnet_t r_convertStringSubnet(char *buf, size_t len);

/*
 * This function takes a terminated string representation of an IP address, creates and
 * sets the corresponding IPv4_t.
 * Assumption: Themis tests format IP addresses correctly.
 */
void r_convertStringIPv4(char *buf, size_t len, IPv4_t *ip);



// ---------------------------------------------------------------------------------------



/*
 * The trie-based routing table leaf structure that holds all the information
 * about the current branch (network part of the subnet), including
 * the interface and the corresponding subnet_t structure.
 *
 * Note: if several network parts are the same and fall in the same leaf,
 *       only the last network is chosen. Therefore, the routing table might
 *       have unreachable networks.
 */

typedef struct routes_leaf_t {
    u8 interface;
    subnet_t subnet;
} routes_leaf_t;

/* Returns an empty (invalid) leaf. */
routes_leaf_t r_emptyLeaf();

/* Adds/Changes the subnet and the interface of the leaf. */
void r_uploadLeafInfo(routes_leaf_t *leaf, subnet_t subnet, u8 interface);

/* Prints the data in the leaf. (useful for debug) */
void r_printRoutingLeaf(const routes_leaf_t leaf);

/*
 * The routing table that stores all the subnets and corresponding interfaces in a router.
 * It uses a search trie to allow for O(k) (k = subnet mask) look-up for destination IP addresses.
 * Properties:
 *     - root node does not represent anything
 *     - left node represents 0 bit
 *     - right node represents 1 bit
 *     - each branch (excluding the root and the leaf) represents the subnet IP of the network-part
 *     - each leaf represents the interface of the correponding subnet
 *
 * Note: this trie requires O(n) memory at worst, where n is the number of subnets in the routing table.
 *
 * For example, the default subnet 0.0.0.0/x is the left-most branch; whereas the subnet the broadcast
 * subnet 255.255.255.255/x is the right-most branch. 
 */
typedef enum routes_type_t {
    R_BRANCH = 0,
    R_LEAF,
    R_ROOT
} routes_type_t;

typedef struct routes_node_t *routes_t;
typedef struct routes_node_t {
    routes_t left;
    routes_t right;
    routes_t to;
    
    routes_leaf_t data;
    routes_type_t type;
} routes_node_t;

/* Functions for creating nodes in the trie-based routing table. */
routes_t r_newRoutingTable();
routes_t r_newBranchNode(routes_t left, routes_t right);
routes_t r_newLeafNode(u8 interface, subnet_t subnet);

/*
 * Searches the trie-based router to find the interface that the
 * destination IP will follow.
 */
u8 r_findInterface(routes_t routes, const IPv4_t ip_arr);

/* Adds a subnet and an interface to the trie-based routing table. */
void r_addRoute(routes_t routes, subnet_t subnet, u8 interface);

/* Frees any dynamically allocated memory in routes_t */
void r_freeRoutes(routes_t routes);

/* Prints the trie-based routing table */
void r_printRoutes(routes_t routes);



// ---------------------------------------------------------------------------------------



#endif // ROUTER_H
