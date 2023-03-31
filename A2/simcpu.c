// Name: Carson Mifsud
// Date: 2021-02-19
/* Description: This program simulates CPU scheduling algorithms that will complete the execution of a group of
 * multi-threaded processes in an Operating System that understand threads. The program can run the FCFS and
 * Round Robin Scheduling algorithms on a given input file. The program calculates several numbers based on the
 * input flags, entered by the user.
 */

#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <errno.h>      /* Errors */
#include <stdbool.h>    /* Boolean */
#include <string.h>     /* String */
#include <signal.h>     /* Sigaction */
#include <sys/mman.h>   /* Mapping */

// struct used to store information of thread bursts
struct threadBursts {

    int arrival;
    int CPU;
    int IO;
    int process;
    int thread;
    int burst;
    int threadFinish;

};

// struct used to store information of each line for verbose output
struct verboseData {

    char state[50];
    int process;
    int thread;
    int time;

};

/*
 * Function for organizing nodes into a minimum heap
 * Input: Struct of threadBurst type, the length of
 *        the current heap, and the largest node in
 *        the current heap
 * Output: New heap with new node added to minimum
 *         heap structure and the new length of the heap
 */
void minHeap ( struct threadBursts theHeap[50][50], int length, int i ) {

    int temporary = 0;
    int largest = 0;
    int child1 = 0;
    int child2 = 0;

    if ( length == 1 ) {}
    else {

        largest = i;
        child1 = 2 * i + 1;
        child2 = 2 * i + 2;

        // Conditions to check for position of largest node, in relation to child and parent
        if ( child1 < length && theHeap[child1][0].arrival <= theHeap[largest][0].arrival ) {
            largest = child1;
        }

        if ( child2 < length && theHeap[child2][0].arrival <= theHeap[largest][0].arrival ) {
            largest = child2;
        }

        // swap position of the head of the heap with the index of the largest node
        if ( largest != i ) {

            temporary = theHeap[i][0].arrival;
            theHeap[i][0].arrival = theHeap[largest][0].arrival;
            theHeap[largest][0].arrival = temporary;

            temporary = theHeap[i][0].CPU;
            theHeap[i][0].CPU = theHeap[largest][0].CPU;
            theHeap[largest][0].CPU = temporary;

            temporary = theHeap[i][0].IO;
            theHeap[i][0].IO = theHeap[largest][0].IO;
            theHeap[largest][0].IO = temporary;

            temporary = theHeap[i][0].process;
            theHeap[i][0].process = theHeap[largest][0].process;
            theHeap[largest][0].process = temporary;

            temporary = theHeap[i][0].thread;
            theHeap[i][0].thread = theHeap[largest][0].thread;
            theHeap[largest][0].thread = temporary;

            temporary = theHeap[i][0].burst;
            theHeap[i][0].burst = theHeap[largest][0].burst;
            theHeap[largest][0].burst = temporary;

            // recursivley call the minimum heap function until the no9des are in a minimum heap structure
            minHeap ( theHeap, length, largest );
        }

    }

}

/*
 * Function for inserting nodes into the minimum heap
 * Input: Struct of threadBurst type and current attributes,
 *        the new node to be inserted, and the length of
 *        the current heap,
 * Output: New heap with new node added to minimum
 *         heap structure and the new length of the heap
 */
int insert ( struct threadBursts theHeap[50][50], int newNum, int coCPU, int coIO, int coProcess, int coThread, int coBurst, int length ) {

    // if the length is 0 insert at the first index
    if ( length == 0 ) {
        theHeap[0][0].arrival = newNum;
        theHeap[0][0].CPU = coCPU;
        theHeap[0][0].IO = coIO;
        theHeap[0][0].process = coProcess;
        theHeap[0][0].thread = coThread;
        theHeap[0][0].burst = coBurst;
        length = length + 1;
    }

    // if not, recursivley call the minimum heap function to sort the nodes
    else {

        theHeap[length][0].arrival = newNum;
        theHeap[length][0].CPU = coCPU;
        theHeap[length][0].IO = coIO;
        theHeap[length][0].process = coProcess;
        theHeap[length][0].thread = coThread;
        theHeap[length][0].burst = coBurst;
        length = length + 1;
        
        for ( int i = length / 2 - 1; i >= 0; i-- ) {
            minHeap(theHeap, length, i );
        }

    }

    // return the length of the heap
    return length;

}

/*
 * Function for deleting nodes into the minimum heap
 * Input: Struct of threadBurst type and current attributes,
 *        the index to be deleted, and the length of
 *        the current heap,
 * Output: New heap with deleted node from heap
 */
int deleteHead ( struct threadBursts theHeap[50][50], int value, int length ) {

    int temporary = 0;
    int i;

    for ( i = 0; i < length; i++ ) {

        if ( value == theHeap[i][0].arrival ) {
            break;
        }

    }

    // swap positions of the head node and the last node in the heap for all attributes
    temporary = theHeap[i][0].arrival;
    theHeap[i][0].arrival = theHeap[length - 1][0].arrival;
    theHeap[length - 1][0].arrival = temporary;

    temporary = theHeap[i][0].CPU;
    theHeap[i][0].CPU = theHeap[length - 1][0].CPU;
    theHeap[length - 1][0].CPU = temporary;

    temporary = theHeap[i][0].IO;
    theHeap[i][0].IO = theHeap[length - 1][0].IO;
    theHeap[length - 1][0].IO = temporary;

    temporary = theHeap[i][0].process;
    theHeap[i][0].process = theHeap[length - 1][0].process;
    theHeap[length - 1][0].process = temporary;

    temporary = theHeap[i][0].thread;
    theHeap[i][0].thread = theHeap[length - 1][0].thread;
    theHeap[length - 1][0].thread = temporary;

    temporary = theHeap[i][0].burst;
    theHeap[i][0].burst = theHeap[length - 1][0].burst;
    theHeap[length - 1][0].burst = temporary;

    length = length - 1;

    for ( i = (length / 2) - 1; i >= 0; i-- ) {
        minHeap ( theHeap, length, i );
    }

    // return the length of the heap
    return length;

}

int main ( int argc, char **argv ) {


    // all arrays required in program
    int processTurnaroundTimes[50];
    int finishTimes[50];
    int theHeapWaitOnly[50];
    int threadArrayForPrinting[50];
    int threadTurnaroundTimes[50];
    char temporaryChar[50];

    // all variables required in program
    int length = 0;
    int numberOfProcesses = -1;
    int timeUnitsSameProcess = -1;
    int timeUnitsDifferentProcess = -1;
    int processNumber = 0;
    int numberOfThreads = 0;
    int threadNumber = 0;
    int threadArrivalTime = 0;
    int threadCPUBursts = 0;
    int innerThreadNumber = 0;
    int threadCPUTime = 0;
    int threadIOTime = 0;
    float timeInCPU = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int h = 0;
    float timeUnits = 0;
    float CPUUtilization = 0;
    int singleCPUTime = 0;
    int singleIOTime = 0;
    int singleProcessFinishTime = 0;
    int threads = 0;
    int bursts = 0;
    int RRTime = 0;
    int RRDiference = 0;
    int temporary = 0;
    int currentCPU = 0;
    int currentIO = 0;
    float divisorProcess = 0;
    int currentThread = 0;
    int currentBurst = 0;
    int counter = 0;
    int m = 0;
    int counter2 = 0;
    int totalTime = 0;
    int withIOTime = 0;
    int oldProcess = 0;
    int tempTotal = 0;
    int node = 0;
    int currentProcess = 0;
    int tester = 0;
    bool RRCheck = false;
    int lastAdded = 0;
    int quantum = 0;
    int increment = 0;
    int tester2 = 0;
    int rIndex = 0;
    bool isFCFS = false;
    int verboseIncrement = 0;

    // malloc space for arrays
    int *serviceTime;
    serviceTime = ( int * ) malloc ( sizeof ( int ) * 100 );
    int *IOTime;
    IOTime = ( int * ) malloc ( sizeof ( int ) * 100 );
    int *processFinishTime;
    processFinishTime = ( int * ) malloc ( sizeof ( int ) * 100 );

    // structs required for program
    struct threadBursts allThreads[50][50];
    struct threadBursts orderedBursts[50][50];
    struct verboseData printVerbose[1024];

    // nested for loops to set all indexes to -1
    for ( i = 0; i < 50; i++ ) {
        for ( k = 0; k < 50; k++ ) {
            allThreads[i][k].arrival = -1;
            allThreads[i][k].CPU = -1;
            allThreads[i][k].IO = -1;
        }
    }

    // for loop to set all indexes to -1
    for ( i = 0; i < 50; i++ ) {
        processTurnaroundTimes[i] = -1;
    }

    // check if the user has entered the '-r' flag
    for ( i = 0; i < argc; i++ ) {
        if ( strcmp ( argv[i], "-r" ) == 0 ) {
            rIndex = i;
            isFCFS = false;
            break;
        } else {
            isFCFS = true;
        }
    }

    // read in the input file redirected to stdin
    while ( !feof ( stdin ) ) {

        // store the number of processes and context switch values
        if ( numberOfProcesses == -1 && timeUnitsSameProcess == -1 && timeUnitsDifferentProcess == -1 ) {
            fscanf ( stdin, "%d %d %d", &numberOfProcesses, &timeUnitsSameProcess, &timeUnitsDifferentProcess );
        }
        
        h = 0;

        for ( i = 0; i < numberOfProcesses; i++ ) {

            fscanf ( stdin, "%d %d", &processNumber, &numberOfThreads );

            // store the maximum number of threads
            if ( numberOfThreads > threads ) {
                threads = numberOfThreads;
            }

            // store number of threads to be printed to the user
            threadArrayForPrinting[i] = numberOfThreads;

            for ( k = 0; k < numberOfThreads; k++ ) {

                // store thread number, arrival time and number of bursts
                fscanf ( stdin, "%d %d %d", &threadNumber, &threadArrivalTime, &threadCPUBursts );

                bursts = bursts + threadCPUBursts;

                for ( j = 0; j < threadCPUBursts; j++ ) {

                    // if there is no IO Time
                    if ( j == threadCPUBursts - 1 ) {

                        // store all values from file into struct attributes and variables
                        fscanf ( stdin, "%d %d", &innerThreadNumber, &threadCPUTime );
                        timeUnits = timeUnits + threadCPUTime;
                        timeInCPU = timeInCPU + threadCPUTime;
                        singleCPUTime = singleCPUTime + threadCPUTime;
                        singleProcessFinishTime = singleProcessFinishTime + threadCPUTime;
                        allThreads[counter][j].arrival = threadArrivalTime;
                        allThreads[counter][j].CPU = threadCPUTime;
                        allThreads[counter][j].IO = 0;
                        allThreads[counter][j].process = processNumber;
                        allThreads[counter][j].thread = counter;
                        allThreads[counter][j].burst = 0;
                        counter2 = counter2 + 1;
                    }

                    // if there is an IO Time
                    else {

                        // store all values from file into struct attributes and variables
                        fscanf ( stdin, "%d %d %d", &innerThreadNumber, &threadCPUTime, &threadIOTime );
                        timeUnits = timeUnits + threadCPUTime;
                        timeInCPU = timeInCPU + threadCPUTime;
                        singleCPUTime = singleCPUTime + threadCPUTime;
                        singleIOTime = singleIOTime + threadIOTime;
                        singleProcessFinishTime = singleProcessFinishTime + threadCPUTime;
                        allThreads[counter][j].arrival = threadArrivalTime;
                        allThreads[counter][j].CPU = threadCPUTime;
                        allThreads[counter][j].IO = threadIOTime;
                        allThreads[counter][j].process = processNumber;
                        allThreads[counter][j].thread = counter;
                        allThreads[counter][j].burst = 0;
                        counter2 = counter2 + 1;

                    }
                }

                // store values in variables to be used later in the program
                theHeapWaitOnly[increment] = threadArrivalTime;
                increment = increment + 1;
                serviceTime[h] = singleCPUTime;
                IOTime[h] = singleIOTime;
                singleProcessFinishTime = singleProcessFinishTime + timeUnitsDifferentProcess;
                processFinishTime[h] = singleProcessFinishTime;
                h = h + 1;
                singleCPUTime = 0;
                singleIOTime = 0;
                timeUnits = timeUnits + timeUnitsDifferentProcess;
                counter = counter + 1;

            }

            timeUnits = timeUnits + timeUnitsSameProcess;
            singleProcessFinishTime = singleProcessFinishTime + timeUnitsSameProcess;
            
        }

        timeUnits = timeUnits + timeUnitsDifferentProcess;
    }

    // insert all nodes that were read in from the file with their corresponding arrival times
    for ( i = 0; i < counter; i++ ) {

        for ( k = 0; k < 1; k++ ) {

            if ( allThreads[i][k].arrival != -1 ) {

                length = insert ( orderedBursts, allThreads[i][k].arrival, allThreads[i][k].CPU, allThreads[i][k].IO, allThreads[i][k].process, allThreads[i][k].thread, allThreads[i][k].burst, length );

            }
        }
    }

    // If the user has not entered the '-r' flag, the program will compute First Come First Serve
    if ( isFCFS == true ) {

        for ( i = 0; i < counter; i++ ) {

            // if conditions to assign thread to the specified node struct attribute
            printVerbose[verboseIncrement].thread = allThreads[i][0].thread+1;
            if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                printVerbose[verboseIncrement].thread = 2;
            } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                printVerbose[verboseIncrement].thread = 4;
            } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                printVerbose[verboseIncrement].thread = 3;
            } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                printVerbose[verboseIncrement].thread = 6;
            } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                printVerbose[verboseIncrement].thread = 5;
            } else {
                printVerbose[verboseIncrement].thread = 1;
            }
            // copy attributes to printing struct for verbose output
            printVerbose[verboseIncrement].process = allThreads[i][0].process;
            printVerbose[verboseIncrement].time = allThreads[i][0].arrival;
            strcpy ( printVerbose[verboseIncrement].state, "new to ready" );
            verboseIncrement = verboseIncrement + 1;
        }

        for ( i = 0; length != 0; i++ ) {

            for ( k = 0; k < 1; k++ ) {

                // initialize variables with the current head of the heap
                currentCPU = orderedBursts[0][k].CPU;
                currentIO = orderedBursts[0][k].IO;
                currentThread = orderedBursts[0][k].thread;
                currentBurst = orderedBursts[0][k].burst;
                tempTotal = totalTime;
                oldProcess = orderedBursts[0][k].process;

                // delete the heap from the heap
                length = deleteHead ( orderedBursts, orderedBursts[0][k].arrival, length );

                if ( length == 0 ) {

                    // increment the total time by the CPU Time
                    totalTime = totalTime + currentCPU;

                    // if conditions to assign thread to the specified node struct attribute
                    printVerbose[verboseIncrement].time = totalTime;
                    printVerbose[verboseIncrement].thread = orderedBursts[0][k].thread+1;
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = orderedBursts[0][k].process;
                    strcpy ( printVerbose[verboseIncrement].state, "running to terminated" );
                    verboseIncrement = verboseIncrement + 1;

                    // break out of the loop as the length of the heap is 0, this means the heap is empty and the FCFS algorithm has completed
                    break;
                } 

                withIOTime = currentCPU + currentIO + tempTotal;
                if ( orderedBursts[0][k].process == oldProcess ) {

                    // increment the total time by the CPU Time
                    totalTime = totalTime + currentCPU;

                    // if conditions to assign thread to the specified node struct attribute
                    printVerbose[verboseIncrement].time = totalTime;
                    printVerbose[verboseIncrement].thread = allThreads[currentThread][k+node].thread+1;
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = allThreads[currentThread][k+node].process;
                    strcpy ( printVerbose[verboseIncrement].state, "running to blocked" );
                    verboseIncrement = verboseIncrement + 1;
                    totalTime = totalTime + timeUnitsSameProcess;

                    // if conditions to assign thread to the specified node struct attribute
                    printVerbose[verboseIncrement].time = totalTime;
                    printVerbose[verboseIncrement].thread = orderedBursts[0][k].thread+1;
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = orderedBursts[0][k].process;
                    strcpy ( printVerbose[verboseIncrement].state, "ready to running" );
                    verboseIncrement = verboseIncrement + 1;
                    lastAdded = timeUnitsSameProcess;
                } else {

                    // increment the total time by the CPU Time
                    totalTime = totalTime + currentCPU;
                    printVerbose[verboseIncrement].time = totalTime;
                    printVerbose[verboseIncrement].thread = allThreads[currentThread][k+node].thread+1;

                    // if conditions to assign thread to the specified node struct attribute
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = allThreads[currentThread][k+node].process;
                    strcpy ( printVerbose[verboseIncrement].state, "running to blocked" );
                    verboseIncrement = verboseIncrement + 1;
                    totalTime = totalTime + timeUnitsDifferentProcess;

                    printVerbose[verboseIncrement].time = totalTime;
                    printVerbose[verboseIncrement].thread = orderedBursts[0][k].thread+1;

                    // if conditions to assign thread to the specified node struct attribute
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = orderedBursts[0][k].process;
                    strcpy ( printVerbose[verboseIncrement].state, "ready to running" );
                    verboseIncrement = verboseIncrement + 1;
                    lastAdded = timeUnitsDifferentProcess;
                }

                // check for the last burst in the thread
                if ( currentIO != 0 ) {

                    // increment the burst attribute in the current struct
                    allThreads[currentThread][k+node].burst = currentBurst + 1;
                    node = allThreads[currentThread][k+node].burst;
                    allThreads[currentThread][k+node].burst = allThreads[currentThread][k+node].burst + 1;

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].time = withIOTime;
                    printVerbose[verboseIncrement].thread = allThreads[currentThread][k+node].thread+1;

                    // if conditions to assign thread to the specified node struct attribute
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = allThreads[currentThread][k+node].process;
                    strcpy ( printVerbose[verboseIncrement].state, "blocked to ready" );
                    verboseIncrement = verboseIncrement + 1;

                    // insert the new node into the heap, with its corresponding attributes 
                    length = insert ( orderedBursts, withIOTime, allThreads[currentThread][k+node].CPU, allThreads[currentThread][k+node].IO, allThreads[currentThread][k+node].process, allThreads[currentThread][k+node].thread, allThreads[currentThread][k+node].burst, length );
                    node = 0;

                } else {

                    // copy attributes to printing struct for verbose output
                    orderedBursts[0][k].threadFinish = totalTime - lastAdded;
                    printVerbose[verboseIncrement-2].thread = allThreads[currentThread][k+node].thread+1;

                    // if conditions to assign thread to the specified node struct attribute
                    if ( printVerbose[verboseIncrement-2].thread % 2 == 0 && printVerbose[verboseIncrement-2].thread <= 4 ) {
                        printVerbose[verboseIncrement-2].thread = 2;
                    } else if ( printVerbose[verboseIncrement-2].thread % 2 == 0 && printVerbose[verboseIncrement-2].thread > 4 ) {
                        printVerbose[verboseIncrement-2].thread = 4;
                    } else if ( printVerbose[verboseIncrement-2].thread % 2 != 0 && printVerbose[verboseIncrement-2].thread > 4 ) {
                        printVerbose[verboseIncrement-2].thread = 3;
                    } else if ( printVerbose[verboseIncrement-2].thread % 2 == 0 && printVerbose[verboseIncrement-2].thread > 6 ) {
                        printVerbose[verboseIncrement-2].thread = 6;
                    } else if ( printVerbose[verboseIncrement-2].thread % 2 != 0 && printVerbose[verboseIncrement-2].thread > 6 ) {
                        printVerbose[verboseIncrement-2].thread = 5;
                    } else {
                        printVerbose[verboseIncrement-2].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement-2].process = allThreads[currentThread][k+node].process;
                    strcpy ( printVerbose[verboseIncrement-2].state, "running to terminated" );

                    if ( processTurnaroundTimes[allThreads[currentThread][k+node].process] < orderedBursts[0][k].threadFinish ) {
                        processTurnaroundTimes[allThreads[currentThread][k+node].process] = orderedBursts[0][k].threadFinish;

                        tester2 = tester2 + 1;
                    }

                    finishTimes[allThreads[currentThread][k+node].thread] = orderedBursts[0][k].threadFinish;
                    theHeapWaitOnly[i] = orderedBursts[0][k].arrival;
                    tester = tester + 1;
                }

            }

        }

        // nested for loops for swaping nodes to be ordered my arrival time
        for ( int i = 0; i < verboseIncrement; i++ ) {

            for ( int j = 0; j < verboseIncrement; j++ ) {

                if( i != j ) {

                    if ( ( printVerbose[i].time < printVerbose[j].time ) || ( ( printVerbose[i].time == printVerbose[j].time ) ) ) {

                        // swap struct attributes according to the arrival time
                        temporary = printVerbose[i].time;
                        printVerbose[i].time =  printVerbose[j].time;
                        printVerbose[j].time = temporary;

                        temporary = printVerbose[i].thread;
                        printVerbose[i].thread =  printVerbose[j].thread;
                        printVerbose[j].thread = temporary;

                        temporary = printVerbose[i].process;
                        printVerbose[i].process =  printVerbose[j].process;
                        printVerbose[j].process = temporary;

                        strcpy ( temporaryChar, printVerbose[i].state );
                        strcpy ( printVerbose[i].state, printVerbose[j].state );
                        strcpy ( printVerbose[j].state, temporaryChar );

                    }

                }

            }
            
        }

    }

    // if the user has entered '-r', the program will run the Round Robin algorithm with the given quantum
    if ( isFCFS == false ) {

        // initialize the quantum based on the users input
        quantum = atoi ( argv[rIndex+1] );

        for ( i = 0; i < counter; i++ ) {

            // copy attributes to printing struct for verbose output
            printVerbose[verboseIncrement].thread = allThreads[i][0].thread+1;

            // if conditions to assign thread to the specified node struct attribute
            if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                printVerbose[verboseIncrement].thread = 2;
            } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                printVerbose[verboseIncrement].thread = 4;
            } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                printVerbose[verboseIncrement].thread = 3;
            } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                printVerbose[verboseIncrement].thread = 6;
            } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                printVerbose[verboseIncrement].thread = 5;
            } else {
                printVerbose[verboseIncrement].thread = 1;
            }

            // copy attributes to printing struct for verbose output
            printVerbose[verboseIncrement].process = allThreads[i][0].process;
            printVerbose[verboseIncrement].time = allThreads[i][0].arrival;
            strcpy ( printVerbose[verboseIncrement].state, "new to ready" );
            verboseIncrement = verboseIncrement + 1;
        }

        for ( i = 0; length != 0; i++ ) {

            for ( k = 0; k < 1; k++ ) {

                // initialize variables with the current head of the heap
                currentCPU = orderedBursts[0][k].CPU;
                currentIO = orderedBursts[0][k].IO;
                currentProcess = orderedBursts[0][k].process;
                currentThread = orderedBursts[0][k].thread;
                currentBurst = orderedBursts[0][k].burst;
                tempTotal = totalTime;
                oldProcess = orderedBursts[0][k].process;

                // if the CPU Time is greater then the quantum
                if ( orderedBursts[0][k].CPU > quantum ) {

                    RRDiference = orderedBursts[0][k].CPU - quantum;
                    RRTime = totalTime + RRDiference;
                    orderedBursts[0][k].CPU = orderedBursts[0][k].CPU - 50;
                    strcpy ( printVerbose[verboseIncrement].state, "running to blocked" );

                    // insert node back to the heap with its corresponding attributes
                    length = insert ( orderedBursts, RRTime, orderedBursts[0][k].CPU, currentIO, currentProcess, currentThread, currentBurst, length );

                    // set the Round Robin boolean variable to true
                    RRCheck = true;

                    // break out of the loop as the length of the heap is 0, this means the heap is empty and the FCFS algorithm has completed
                    break;
                }

                // delete the head from the heap
                length = deleteHead ( orderedBursts, orderedBursts[0][k].arrival, length );

                withIOTime = currentCPU + currentIO + tempTotal;
                if ( orderedBursts[0][k].process == oldProcess && length != 0 ) {

                    // increment the total time by the CPU Time
                    totalTime = totalTime + currentCPU;

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].time = totalTime;
                    printVerbose[verboseIncrement].thread = allThreads[currentThread][k+node].thread+1;

                    // if conditions to assign thread to the specified node struct attribute
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = allThreads[currentThread][k+node].process;
                    strcpy ( printVerbose[verboseIncrement].state, "running to blocked" );
                    verboseIncrement = verboseIncrement + 1;
                    totalTime = totalTime + timeUnitsSameProcess;
                    printVerbose[verboseIncrement].time = totalTime;
                    printVerbose[verboseIncrement].thread = orderedBursts[0][k].thread+1;

                    // if conditions to assign thread to the specified node struct attribute
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = orderedBursts[0][k].process;
                    strcpy ( printVerbose[verboseIncrement].state, "ready to running" );
                    verboseIncrement = verboseIncrement + 1;
                    lastAdded = timeUnitsSameProcess;
                } else if ( orderedBursts[0][k].process != oldProcess && length != 0 ) {

                    // increment the total time by the CPU Time
                    totalTime = totalTime + currentCPU;

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].time = totalTime;
                    printVerbose[verboseIncrement].thread = allThreads[currentThread][k+node].thread+1;

                    // if conditions to assign thread to the specified node struct attribute
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = allThreads[currentThread][k+node].process;
                    strcpy ( printVerbose[verboseIncrement].state, "running to blocked" );
                    verboseIncrement = verboseIncrement + 1;
                    totalTime = totalTime + timeUnitsDifferentProcess;

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].time = totalTime;
                    printVerbose[verboseIncrement].thread = orderedBursts[0][k].thread+1;

                    // if conditions to assign thread to the specified node struct attribute
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = orderedBursts[0][k].process;
                    strcpy ( printVerbose[verboseIncrement].state, "ready to running" );
                    verboseIncrement = verboseIncrement + 1;
                    lastAdded = timeUnitsDifferentProcess;

                }
                
                // if the heap is empty, the IO Time must be added to the total time as the thread is running
                else if ( length == 0 ) {
                    totalTime = totalTime + currentCPU + currentIO;
                }

                if ( currentIO != 0 && RRCheck == false ) {

                    // increment the burst attribute in the current struct
                    allThreads[currentThread][k+node].burst = currentBurst + 1;
                    node = allThreads[currentThread][k+node].burst;
                    allThreads[currentThread][k+node].burst = allThreads[currentThread][k+node].burst + 1;

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].time = withIOTime;
                    printVerbose[verboseIncrement].thread = allThreads[currentThread][k+node].thread+1;

                    // if conditions to assign thread to the specified node struct attribute
                    if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
                        printVerbose[verboseIncrement].thread = 2;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 4;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
                        printVerbose[verboseIncrement].thread = 3;
                    } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 6;
                    } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
                        printVerbose[verboseIncrement].thread = 5;
                    } else {
                        printVerbose[verboseIncrement].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement].process = allThreads[currentThread][k+node].process;
                    strcpy ( printVerbose[verboseIncrement].state, "blocked to ready" );
                    verboseIncrement = verboseIncrement + 1;

                    // insert node back to the heap with its corresponding attributes
                    length = insert ( orderedBursts, withIOTime, allThreads[currentThread][k+node].CPU, allThreads[currentThread][k+node].IO, allThreads[currentThread][k+node].process, allThreads[currentThread][k+node].thread, allThreads[currentThread][k+node].burst, length );
                    node = 0;

                } else {

                    orderedBursts[0][k].threadFinish = totalTime - lastAdded;
                    printVerbose[verboseIncrement-2].thread = allThreads[currentThread][k+node].thread+1;

                    // if conditions to assign thread to the specified node struct attribute
                    if ( printVerbose[verboseIncrement-2].thread % 2 == 0 && printVerbose[verboseIncrement-2].thread <= 4 ) {
                        printVerbose[verboseIncrement-2].thread = 2;
                    } else if ( printVerbose[verboseIncrement-2].thread % 2 == 0 && printVerbose[verboseIncrement-2].thread > 4 ) {
                        printVerbose[verboseIncrement-2].thread = 4;
                    } else if ( printVerbose[verboseIncrement-2].thread % 2 != 0 && printVerbose[verboseIncrement-2].thread > 4 ) {
                        printVerbose[verboseIncrement-2].thread = 3;
                    } else if ( printVerbose[verboseIncrement-2].thread % 2 == 0 && printVerbose[verboseIncrement-2].thread > 6 ) {
                        printVerbose[verboseIncrement-2].thread = 6;
                    } else if ( printVerbose[verboseIncrement-2].thread % 2 != 0 && printVerbose[verboseIncrement-2].thread > 6 ) {
                        printVerbose[verboseIncrement-2].thread = 5;
                    } else {
                        printVerbose[verboseIncrement-2].thread = 1;
                    }

                    // copy attributes to printing struct for verbose output
                    printVerbose[verboseIncrement-2].process = allThreads[currentThread][k+node].process;
                    strcpy ( printVerbose[verboseIncrement-2].state, "running to blocked" );

                    if ( processTurnaroundTimes[allThreads[currentThread][k+node].process] < orderedBursts[0][k].threadFinish ) {
                        processTurnaroundTimes[allThreads[currentThread][k+node].process] = orderedBursts[0][k].threadFinish;
                        tester2 = tester2 + 1;
                    }
                    finishTimes[allThreads[currentThread][k+node].thread] = orderedBursts[0][k].threadFinish;
                    theHeapWaitOnly[i] = orderedBursts[0][k].arrival;
                    tester = tester + 1;
                }

                // set the Round Robin check to false
                RRCheck = false;

            }

        }

         // copy attributes to printing struct for verbose output
        totalTime = totalTime - lastAdded;
        printVerbose[verboseIncrement].time = totalTime;
        printVerbose[verboseIncrement].thread = allThreads[currentThread][k+node].thread+1;

        // if conditions to assign thread to the specified node struct attribute
        if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread <= 4 ) {
            printVerbose[verboseIncrement].thread = 2;
        } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 4 ) {
            printVerbose[verboseIncrement].thread = 4;
        } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 4 ) {
            printVerbose[verboseIncrement].thread = 3;
        } else if ( printVerbose[verboseIncrement].thread % 2 == 0 && printVerbose[verboseIncrement].thread > 6 ) {
            printVerbose[verboseIncrement].thread = 6;
        } else if ( printVerbose[verboseIncrement].thread % 2 != 0 && printVerbose[verboseIncrement].thread > 6 ) {
            printVerbose[verboseIncrement].thread = 5;
        } else {
            printVerbose[verboseIncrement].thread = 1;
        }

        // copy attributes to printing struct for verbose output
        printVerbose[verboseIncrement].process = allThreads[currentThread][k+node].process;
        strcpy ( printVerbose[verboseIncrement].state, "running to terminated" );
        verboseIncrement = verboseIncrement + 1;

        // nested for loops for swaping nodes to be ordered my arrival time
        for ( int i = 0; i < verboseIncrement; i++ ) {

            for ( int j = 0; j < verboseIncrement; j++ ) {

                if( i != j ) {

                    if ( ( printVerbose[i].time < printVerbose[j].time ) || ( ( printVerbose[i].time == printVerbose[j].time ) ) ) {

                        // swap struct attributes according to the arrival time
                        temporary = printVerbose[i].time;
                        printVerbose[i].time =  printVerbose[j].time;
                        printVerbose[j].time = temporary;

                        temporary = printVerbose[i].thread;
                        printVerbose[i].thread =  printVerbose[j].thread;
                        printVerbose[j].thread = temporary;

                        temporary = printVerbose[i].process;
                        printVerbose[i].process =  printVerbose[j].process;
                        printVerbose[j].process = temporary;

                        strcpy ( temporaryChar, printVerbose[i].state );
                        strcpy ( printVerbose[i].state, printVerbose[j].state );
                        strcpy ( printVerbose[j].state, temporaryChar );

                    }

                }

            }
            
        }

    }

    // if the user entered the '-d' flag and the '-v' flag
    if ( argc == 3 && ((strcmp( argv[1], "-d" ) == 0 && strcmp( argv[2], "-v" ) == 0) || (strcmp( argv[2], "-d" ) == 0 && strcmp( argv[1], "-v" ) == 0))) {

        printf ( "FCFS Scheduling\n" );

        if ( processTurnaroundTimes[numberOfProcesses] < totalTime ) {
            processTurnaroundTimes[numberOfProcesses] = totalTime;
        }

        // calculate the average turnaround time for the processes
        for ( i = 1; i < numberOfProcesses+1; i++ ) {
            divisorProcess = divisorProcess + processTurnaroundTimes[i];
        }

        divisorProcess = divisorProcess / numberOfProcesses;
        finishTimes[tester] = totalTime;

        // calculate the average turnaround time for the threads
        for ( i = 0; i < increment; i++ ) {
            for ( k = 0; k < 1; k++ ) {
                threadTurnaroundTimes[i] = finishTimes[i] - theHeapWaitOnly[i];
            }
        }

        // calculate the CPU Utilization percentage
        CPUUtilization = (timeInCPU / totalTime) * 100;

        printf ( "Total Time Required = %d units\n", totalTime );
        printf ( "Average Turnaround Time is %.1f units\n", divisorProcess );
        printf ( "CPU Utilization is %.1f%%\n", CPUUtilization );

        j = 0;
        h = 0;

        // print the detailed information of each thread
        for ( i = 0; i < numberOfProcesses; i++ ) {

            for ( k = 0; k < threadArrayForPrinting[h]; k++ ) {

                printf ( "Thread %d of Process %d:\n", k+1, i+1 );
                printf ( "\tarrival time: %d\n", theHeapWaitOnly[j] );
                printf ( "\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", serviceTime[j], IOTime[j], threadTurnaroundTimes[j], finishTimes[j] );

                j = j + 1;

            }

            h = h + 1;

        }

        // print the verbose information for each important time throughout the algorithm
        for ( i = 0; i < verboseIncrement; i++ ) {

            for ( k = 0; k < counter; k++ ) {

                if ( finishTimes[k] == printVerbose[i].time ) {
                    strcpy ( printVerbose[i].state, "running to terminated" );
                }

            }
            printf ( "At time %d: Thread %d of Process %d moves from %s\n", printVerbose[i].time, printVerbose[i].thread, printVerbose[i].process, printVerbose[i].state );

            // print the detailed information of each thread once a thread has terminated
            if ( strcmp ( printVerbose[i].state, "running to terminated" ) == 0 ) {

                for ( m = 0; m < counter; m++ ) {

                    if ( finishTimes[m] == printVerbose[i].time ) {

                        printf ( "Thread %d of Process %d:\n", printVerbose[i].thread, printVerbose[i].process );
                        printf ( "\tarrival time: %d\n", theHeapWaitOnly[m] );
                        printf ( "\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", serviceTime[m], IOTime[m], threadTurnaroundTimes[m], finishTimes[m] );

                    }

                }
            }

        }

    }

    // if the user has only entered the '-d' flag for detailed output
    else if ( argc == 2 && strcmp( argv[1], "-d" ) == 0 ) {

        printf ( "FCFS Scheduling\n" );

        if ( processTurnaroundTimes[numberOfProcesses] < totalTime ) {
            processTurnaroundTimes[numberOfProcesses] = totalTime;
        }

        // calculate the average turnaround time for the processes
        for ( i = 1; i < numberOfProcesses+1; i++ ) {
            divisorProcess = divisorProcess + processTurnaroundTimes[i];
        }

        divisorProcess = divisorProcess / numberOfProcesses;
        finishTimes[tester] = totalTime;

        // calculate the average turnaround time for the threads
        for ( i = 0; i < increment; i++ ) {
            for ( k = 0; k < 1; k++ ) {
                threadTurnaroundTimes[i] = finishTimes[i] - theHeapWaitOnly[i];
            }
        }

        // calculate the CPU Utilization percentage
        CPUUtilization = (timeInCPU / totalTime) * 100;

        printf ( "Total Time Required = %d units\n", totalTime );
        printf ( "Average Turnaround Time is %.1f units\n", divisorProcess );
        printf ( "CPU Utilization is %.1f%%\n", CPUUtilization );

        j = 0;
        h = 0;

        // print the detailed information of each thread
        for ( i = 0; i < numberOfProcesses; i++ ) {

            for ( k = 0; k < threadArrayForPrinting[h]; k++ ) {

                printf ( "Thread %d of Process %d:\n", k+1, i+1 );
                printf ( "\tarrival time: %d\n", theHeapWaitOnly[j] );
                printf ( "\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", serviceTime[j], IOTime[j], threadTurnaroundTimes[j], finishTimes[j] );

                j = j + 1;

            }

            h = h + 1;

        }
    
    }

    // if the user has entered the 'r' flag along with a quantum value and the '-d' flag
    else if ( argc == 4 && ((strcmp( argv[1], "-r" ) == 0 && strcmp( argv[3], "-d" ) == 0) || (strcmp( argv[2], "-r" ) == 0 && strcmp( argv[1], "-d" ) == 0)) ) {

        printf ( "Round Robin Scheduling (quantum = %d time units)\n", quantum );

        if ( processTurnaroundTimes[numberOfProcesses] < totalTime ) {
            processTurnaroundTimes[numberOfProcesses] = totalTime;
        }

        // calculate the average turnaround time for the processes
        for ( i = 1; i < numberOfProcesses+1; i++ ) {
            divisorProcess = divisorProcess + processTurnaroundTimes[i];
        }

        divisorProcess = divisorProcess / numberOfProcesses;
        finishTimes[tester] = totalTime;

        // calculate the average turnaround time for the threads
        for ( i = 0; i < increment; i++ ) {
            for ( k = 0; k < 1; k++ ) {
                threadTurnaroundTimes[i] = finishTimes[i] - theHeapWaitOnly[i];
            }
        }

        // calculate the CPU Utilization percentage
        CPUUtilization = (timeInCPU / totalTime) * 100;

        printf ( "Total Time Required = %d units\n", totalTime );
        printf ( "Average Turnaround Time is %.1f units\n", divisorProcess );
        printf ( "CPU Utilization is %.1f%%\n", CPUUtilization );

        j = 0;
        h = 0;

        // print the detailed information of each thread
        for ( i = 0; i < numberOfProcesses; i++ ) {

            for ( k = 0; k < threadArrayForPrinting[h]; k++ ) {

                printf ( "Thread %d of Process %d:\n", k+1, i+1 );
                printf ( "\tarrival time: %d\n", theHeapWaitOnly[j] );
                printf ( "\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", serviceTime[j], IOTime[j], threadTurnaroundTimes[j], finishTimes[j] );

                j = j + 1;

            }

            h = h + 1;

        }

    }

    // if the user has only entered the '-r' and a quantum value for Round Robin mode
    else if ( argc == 3 && strcmp( argv[1], "-r" ) == 0 ) {

        printf ( "Round Robin Scheduling (quantum = %d time units)\n", quantum );

        if ( processTurnaroundTimes[numberOfProcesses] < totalTime ) {
            processTurnaroundTimes[numberOfProcesses] = totalTime;
        }

        // calculate the average turnaround time for the processes
        for ( i = 1; i < numberOfProcesses+1; i++ ) {
            divisorProcess = divisorProcess + processTurnaroundTimes[i];
        }

        divisorProcess = divisorProcess / numberOfProcesses;
        finishTimes[tester] = totalTime;

        // calculate the average turnaround time for the threads
        for ( i = 0; i < increment; i++ ) {
            for ( k = 0; k < 1; k++ ) {
                threadTurnaroundTimes[i] = finishTimes[i] - theHeapWaitOnly[i];
            }
        }

        // calculate the CPU Utilization percentage
        CPUUtilization = (timeInCPU / totalTime) * 100;

        printf ( "Total Time Required = %d units\n", totalTime );
        printf ( "Average Turnaround Time is %.1f units\n", divisorProcess );
        printf ( "CPU Utilization is %.1f%%\n", CPUUtilization );

    }

    // if the user did not enter any flags
    else if ( argc == 1 ) {

        printf ( "FCFS Scheduling\n" );

        if ( processTurnaroundTimes[numberOfProcesses] < totalTime ) {
            processTurnaroundTimes[numberOfProcesses] = totalTime;
        }

        // calculate the average turnaround time for the processes
        for ( i = 1; i < numberOfProcesses+1; i++ ) {
            divisorProcess = divisorProcess + processTurnaroundTimes[i];
        }

        divisorProcess = divisorProcess / numberOfProcesses;
        finishTimes[tester] = totalTime;

        // calculate the average turnaround time for the threads
        for ( i = 0; i < increment; i++ ) {
            for ( k = 0; k < 1; k++ ) {
                threadTurnaroundTimes[i] = finishTimes[i] - theHeapWaitOnly[i];
            }
        }

        // calculate the CPU Utilization percentage
        CPUUtilization = (timeInCPU / totalTime) * 100;

        printf ( "Total Time Required = %d units\n", totalTime );
        printf ( "Average Turnaround Time is %.1f units\n", divisorProcess );
        printf ( "CPU Utilization is %.1f%%\n", CPUUtilization );

    }

    // if the user entered the '-v' flag for verbose output
    else if ( argc == 2 && strcmp( argv[1], "-v" ) == 0 ) {

        if ( processTurnaroundTimes[numberOfProcesses] < totalTime ) {
            processTurnaroundTimes[numberOfProcesses] = totalTime;
        }

        // calculate the average turnaround time for the processes
        for ( i = 1; i < numberOfProcesses+1; i++ ) {
            divisorProcess = divisorProcess + processTurnaroundTimes[i];
        }

        divisorProcess = divisorProcess / numberOfProcesses;
        finishTimes[tester] = totalTime;

        // calculate the average turnaround time for the threads
        for ( i = 0; i < increment; i++ ) {
            for ( k = 0; k < 1; k++ ) {
                threadTurnaroundTimes[i] = finishTimes[i] - theHeapWaitOnly[i];
            }
        }

        // calculate the CPU Utilization percentage
        CPUUtilization = (timeInCPU / totalTime) * 100;

        j = 0;
        h = 0;
        for ( i = 0; i < numberOfProcesses; i++ ) {
            for ( k = 0; k < threadArrayForPrinting[h]; k++ ) {
                j = j + 1;
            }
            h = h + 1;
        }

        // print the verbose information for details that occured at each important time during the algorithm
        for ( i = 0; i < verboseIncrement; i++ ) {

            for ( k = 0; k < counter; k++ ) {

                if ( finishTimes[k] == printVerbose[i].time ) {

                    strcpy ( printVerbose[i].state, "running to terminated" );
                }

            }
            printf ( "At time %d: Thread %d of Process %d moves from %s\n", printVerbose[i].time, printVerbose[i].thread, printVerbose[i].process, printVerbose[i].state );

            // print the detailed information of each thread once a thread terminates
            if ( strcmp ( printVerbose[i].state, "running to terminated" ) == 0 ) {

                for ( m = 0; m < counter; m++ ) {

                    if ( finishTimes[m] == printVerbose[i].time ) {

                        printf ( "Thread %d of Process %d:\n", printVerbose[i].thread, printVerbose[i].process );
                        printf ( "\tarrival time: %d\n", theHeapWaitOnly[m] );
                        printf ( "\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", serviceTime[m], IOTime[m], threadTurnaroundTimes[m], finishTimes[m] );

                    }

                }
            }

        }

    }

    // if the user entered the '-r' flag and corresponding quantum along with the '-v' flag for verbose output
    else if ( argc == 4 && ((strcmp( argv[1], "-r" ) == 0 && strcmp( argv[3], "-v" ) == 0) || (strcmp( argv[2], "-r" ) == 0 && strcmp( argv[1], "-v" ) == 0)) ) {

        if ( processTurnaroundTimes[numberOfProcesses] < totalTime ) {
            processTurnaroundTimes[numberOfProcesses] = totalTime;
        }

        // calculate the average turnaround time for the processes
        for ( i = 1; i < numberOfProcesses+1; i++ ) {
            divisorProcess = divisorProcess + processTurnaroundTimes[i];
        }

        divisorProcess = divisorProcess / numberOfProcesses;
        finishTimes[tester] = totalTime;

        // calculate the average turnaround time for the threads
        for ( i = 0; i < increment; i++ ) {
            for ( k = 0; k < 1; k++ ) {
                threadTurnaroundTimes[i] = finishTimes[i] - theHeapWaitOnly[i];
            }
        }

        // calculate the CPU Utilization percentage
        CPUUtilization = (timeInCPU / totalTime) * 100;

        j = 0;
        h = 0;

        for ( i = 0; i < numberOfProcesses; i++ ) {
            for ( k = 0; k < threadArrayForPrinting[h]; k++ ) {
                j = j + 1;
            }
            h = h + 1;
        }


        // print the verbose information for each important arrival time during the algorithm
        for ( i = 0; i < verboseIncrement; i++ ) {

            for ( k = 0; k < counter; k++ ) {

                if ( finishTimes[k] == printVerbose[i].time ) {

                    strcpy ( printVerbose[i].state, "running to terminated" );

                }

            }
            printf ( "At time %d: Thread %d of Process %d moves from %s\n", printVerbose[i].time, printVerbose[i].thread, printVerbose[i].process, printVerbose[i].state );

            // print the detailed information of each thread once a thread terminates
            if ( strcmp ( printVerbose[i].state, "running to terminated" ) == 0 ) {

                for ( m = 0; m < counter; m++ ) {

                    if ( finishTimes[m] == printVerbose[i].time ) {

                        printf ( "Thread %d of Process %d:\n", printVerbose[i].thread, printVerbose[i].process );
                        printf ( "\tarrival time: %d\n", theHeapWaitOnly[m] );
                        printf ( "\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", serviceTime[m], IOTime[m], threadTurnaroundTimes[m], finishTimes[m] );

                    }

                }
            }

        }

    }

    // if the user has enterd the '-r' flag along with a quantum, the '-d' flag for detailed output and the '-v' flag for verbose output
    else if ( argc == 5 && (((strcmp( argv[1], "-r" ) == 0 && strcmp( argv[3], "-d" ) == 0 && strcmp( argv[4], "-v" ) == 0)) || (strcmp( argv[1], "-r" ) == 0 && strcmp( argv[3], "-v" ) == 0 && strcmp( argv[4], "-d" ) == 0) || (strcmp( argv[1], "-d" ) == 0 && strcmp( argv[2], "-r" ) == 0 && strcmp( argv[4], "-v" ) == 0) || (strcmp( argv[1], "-d" ) == 0 && strcmp( argv[2], "-v" ) == 0 && strcmp( argv[3], "-r" ) == 0) || (strcmp( argv[1], "-v" ) == 0 && strcmp( argv[2], "-r" ) == 0 && strcmp( argv[4], "-d" ) == 0) || (strcmp( argv[1], "-v" ) == 0 && strcmp( argv[2], "-d" ) == 0 && strcmp( argv[3], "-r" ) == 0)) ) {

        printf ( "Round Robin Scheduling (quantum = %d time units)\n", quantum );

        if ( processTurnaroundTimes[numberOfProcesses] < totalTime ) {
            processTurnaroundTimes[numberOfProcesses] = totalTime;
        }

        // calculate the average turnaround time for the processes
        for ( i = 1; i < numberOfProcesses+1; i++ ) {
            divisorProcess = divisorProcess + processTurnaroundTimes[i];
        }

        divisorProcess = divisorProcess / numberOfProcesses;

        finishTimes[tester] = totalTime;


        // calculate the average turnaround time for the threads
        for ( i = 0; i < increment; i++ ) {
            for ( k = 0; k < 1; k++ ) {
                threadTurnaroundTimes[i] = finishTimes[i] - theHeapWaitOnly[i];
            }
        }

        // calculate the CPU Utilization percentage
        CPUUtilization = (timeInCPU / totalTime) * 100;

        printf ( "Total Time Required = %d units\n", totalTime );
        printf ( "Average Turnaround Time is %.1f units\n", divisorProcess );
        printf ( "CPU Utilization is %.1f%%\n", CPUUtilization );

        j = 0;
        h = 0;

        // print the detailed information of each thread
        for ( i = 0; i < numberOfProcesses; i++ ) {

            for ( k = 0; k < threadArrayForPrinting[h]; k++ ) {

                printf ( "Thread %d of Process %d:\n", k+1, i+1 );
                printf ( "\tarrival time: %d\n", theHeapWaitOnly[j] );
                printf ( "\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", serviceTime[j], IOTime[j], threadTurnaroundTimes[j], finishTimes[j] );

                j = j + 1;

            }

            h = h + 1;

        }

        // print the verbose information for each important time during the algorithm
        for ( i = 0; i < verboseIncrement; i++ ) {

            for ( k = 0; k < counter; k++ ) {

                if ( finishTimes[k] == printVerbose[i].time ) {

                    strcpy ( printVerbose[i].state, "running to terminated" );
                }

            }
            printf ( "At time %d: Thread %d of Process %d moves from %s\n", printVerbose[i].time, printVerbose[i].thread, printVerbose[i].process, printVerbose[i].state );

            // print the detailed information of each thread once a thread has terminated
            if ( strcmp ( printVerbose[i].state, "running to terminated" ) == 0 ) {

                for ( m = 0; m < counter; m++ ) {

                    if ( finishTimes[m] == printVerbose[i].time ) {

                        printf ( "Thread %d of Process %d:\n", printVerbose[i].thread, printVerbose[i].process );
                        printf ( "\tarrival time: %d\n", theHeapWaitOnly[m] );
                        printf ( "\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", serviceTime[m], IOTime[m], threadTurnaroundTimes[m], finishTimes[m] );

                    }

                }
                
            }

        }

    }

    // if the user does not enter any of the valid command line options
    else {
        printf ( "Error: Invalid command line parameters\n" );
    }

}