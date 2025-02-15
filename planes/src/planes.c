#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "LibStack.h"
#include "LibQueue.h"

#define HANGAR_CAPACITY 5
#define WAITLINE_CAPACITY 7

#define MAX_PLANES_NUMBER 1000
#define MAX_BUFFER_SIZE 4
#define REPAIR_POSITIVE_STRING "yes"
#define REPAIR_NEGATIVE_STRING "no"

typedef enum RepairStatus {
    REPAIR_STATUS_NEGATIVE,
    REPAIR_STATUS_POSITIVE,
} RepairStatus;

typedef struct Plane {
    size_t name;
    RepairStatus repair;
} Plane;

/**
 * @brief Reads the name and repair status from stdin.
 *
 * @return Single plane struct (plane_t) with the name and repair status.
 */
static Plane readPlane() {
    size_t name;
    RepairStatus repair_status;
    char buffer[MAX_BUFFER_SIZE];

    if (scanf("%zu %s", &name, buffer) != 2) {
	fprintf(stderr, "Failed to read the number and the string.\n");
	exit(EXIT_FAILURE);
    }
    
    
    // set repair status based on the buffer
    if (strcmp(buffer, REPAIR_NEGATIVE_STRING) == 0) // strcmp returns 0 if strings are equal
	repair_status = REPAIR_STATUS_NEGATIVE;
    else if (strcmp(buffer, REPAIR_POSITIVE_STRING) == 0)
	repair_status = REPAIR_STATUS_POSITIVE;
    else {
	fprintf(stderr, "Error reading input: RepairStatus must be \"yes\" or \"no\"\n");
	exit(EXIT_FAILURE);
    }

    return (Plane){ name, repair_status };
}

/**
 * @brief Transfers planes from the waitline to clearance list.
 *
 * @param clearance array where the correct order of planes departures is stored.
 * @param clearance_index pointer to the index of the next free position in the clearance array.
 * @param waitline pointer to the waitline queue.
 */
static void freeWaitline(size_t clearance[static 1], size_t *clearance_index, Queue * restrict waitline) {
    while (!isEmptyQueue(*waitline)) {
	clearance[*clearance_index] = dequeue(waitline);
	*clearance_index += 1;
    }
}

/**
 * @brief Transfers planes from the harbor into the waitline.
 *
 * @param hangar pointer to the hangar stack.
 * @param waitline pointer to the waitline queue.
 */
static void freeHangar(Stack * restrict hangar, Queue * restrict waitline) { 
    while (!isEmptyStack(*hangar)) {
	enqueue(pop(hangar), waitline);
    }
}

static inline bool isWaitlineFull(size_t waitline_current_size) {
    return waitline_current_size == WAITLINE_CAPACITY;
}

static inline bool isHangarFull(size_t hangar_current_size) {
    return hangar_current_size == HANGAR_CAPACITY;
} 

int main(int argc, char *argv[]) {  
    // the correct order of departures of the planes
    size_t clearance[MAX_PLANES_NUMBER] = {0};
    // next free index in the clearance array
    size_t clearance_index = {0};
    
    Stack hangar = newStack(HANGAR_CAPACITY);
    Queue waitline = newQueue(WAITLINE_CAPACITY);

    size_t hangar_current_size = {0};
    size_t waitline_current_size = {0};
    
    size_t n;
    if (scanf("%zu", &n) != 1) {
	fprintf(stderr, "Failed to read the number of planes.\n");
	exit(EXIT_FAILURE);
    }

    if (n > MAX_PLANES_NUMBER) {
	fprintf(stderr, "Number of planes exceeds maximum allowed: %d", MAX_PLANES_NUMBER);
	exit(EXIT_FAILURE);
    }
    

    // scan each plane
    for (size_t i = 0; i < n; ++i) {
	// if the waitline or the harbor is full, planes depart
	// clearance_index gets incremented because new planes are departed
	if (isWaitlineFull(waitline_current_size) || isHangarFull(hangar_current_size)) {
	    freeWaitline(clearance, &clearance_index, &waitline);
	    waitline_current_size = 0;
	}
	// if hangar is full, planes go to the waitline
	// the order of these conditionals is important 
	if (isHangarFull(hangar_current_size)) {
	    freeHangar(&hangar, &waitline);
	    waitline_current_size = hangar_current_size;
	    hangar_current_size = 0;
	}

	// add the next plane
	Plane plane = readPlane();
	
	switch (plane.repair) {
	case REPAIR_STATUS_NEGATIVE: {
	    enqueue(plane.name, &waitline);
	    waitline_current_size += 1;
	    break;
	}
	case REPAIR_STATUS_POSITIVE: {
	    push(plane.name, &hangar);
	    hangar_current_size += 1;
	    break;
	}
	}
    }

    // empty the waitline and the hangar from the residing planes
    freeWaitline(clearance, &clearance_index, &waitline);
    freeHangar(&hangar, &waitline);
    freeWaitline(clearance, &clearance_index, &waitline);

    // output the result
    for (size_t i = 0; i < n; ++i) {
	printf("%zu\n", clearance[i]);
    }

    freeStack(hangar);
    freeQueue(waitline);
    
    return EXIT_SUCCESS;
}
