#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <math.h>

#define MAX_MEMORY 1024
#define MAX_SWAPS 3
#define BUFFER 500

/*
This is the main structure to represent a process.
 - id is the "pid" of the current process
 - memRequired is the memory required by that process
 - numSwaps is to keep track of the number of swaps a process has gone through
 - key is the key used to define which order the processes should be within the queue
*/
typedef struct
{
    int id;
    int memRequired;
    int numSwaps;
    int key;
} Process;

/** These two functions are used for ordering the queue when moving/inserting processes
 * @post The process list and the index of the current process will be structured accordingly
 * @param processList - The array of all processes
 * @param size - The key for ordering
 * @param i - Starting index of heap/array
 **/
void backShift(Process **processList, int i);
void forwardShift(Process **processList, int size, int i);

/** Helper function to free data
 * @post Data will be freed
 * @param totalMemory - All memory along with the locations with whatever processes are in it
 **/
void freeMemory(Process **totalMemory);

/** Helper functions to execute all memory management algorithms on the current memory state
 * @post Memory will have been correctly managed based on algorithm called
 * @param memory - A array of processes representing the current state of memory
 * @param size - Size needed by current process
 * @return The index of where the process was stored in memory
 **/
int firstFitAlgorithm(Process **memory, int size);
int nextFitAlgorithm(Process **memory, int size, int index);
int bestFitAlgorithm(Process **memory, int size);
int worstFitAlgorithm(Process **memory, int size);

/** Simple function to call one of the algorithms based on user input in command line
 * @param totalMemory - A array of processes representing the current state of memory
 * @param size - Size needed by current process
 * @return The index of where the process was stored in memory
 **/
int executeAlgorithm(Process **totalMemory, int size, int bestFitIndex, char type);

/** Helper function to get total number of holes in memory
 * @param memory - A array of processes representing the current state of memory
 * @return The total number of holes
 **/
int getTotalHoles(Process **memory);

/** Helper function to get current state of memory, i.e. how many "spaces" are being occupied by a process
 * @param memory - A array of processes representing the current state of memory
 * @return Amount of used memory
 **/
double getUsedMemory(Process **memory);

/** Helper function to swap two processes/order the queue and returns the oldest process
 * @param memory - A array of processes representing the current state of memory
 * @return The oldest process
 **/
Process *swapProcess(Process **memory);
