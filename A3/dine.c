// Name: Carson Mifsud
// Date: 2021-03-19
/*
 * Description: This program is a solution to the dining philosopher's problem. Pseudo code was used for this
 * algorithm which can be found on page 295 of the "Operating Systems Concepts, Enhanced eText, 10th Edition"
 * course textbook.
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
#include <pthread.h>    /* Pthreads */
#include <time.h>       /* Time */
#include <semaphore.h>  /*Semaphores */

// Global variables to keep track of the amount of meals eaten by philosophers
int eat = 0;
int number_eat = 0;
int philosophers = 0;

// The several states which a philosopher can be in
enum states {

    THINKING, EATING, HUNGRY
    
};

// Struct to store threads of philosophers along with their current state
typedef struct {

    pthread_cond_t condition[1000];
    pthread_mutex_t mutex;
    enum states state[10000];

} DiningPhilosophers ;

// Struct to store threads of philosophers along with their current state
DiningPhilosophers dine;

/*
 * This function is used to test if a philosophers neighbours are in the EATING
 * state and to test if if the current philosopher is starving.
 */
void test ( int i ) {

    // Check neighbours state along with starvation
    if ( dine.state[(i + 1) % philosophers] != EATING && dine.state[i] == HUNGRY && dine.state[(i + philosophers - 1) % philosophers] != EATING ) {

        // Change current philosophers state to EATING
        dine.state[i] = EATING;

        // Alter the condition of the current thread
        pthread_cond_signal ( &dine.condition[i] );

    }

}

/*
 * This function is used to change the state of the current philosopher
 * to THINKING as the current philosopher has 'put down' their chopsticks
 * allowing its nieghbours to check for chopsticks
 */
void putdown ( int i ) {

    // Integer variables used throughout the function
    int temp1 = 0;
    int temp2 = 0;

    // Increment the total number of meals eaten
    number_eat = number_eat + 1;

    // Alter the condition of the current thread
    pthread_mutex_lock ( &dine.mutex );

    // Change current philosophers state to THINKING
    dine.state[i] = THINKING;

    // If the total number of meals have been reached, exit the program
    if ( eat == number_eat ) {

        exit ( 0 );

    }

    // Decrement to the previous philosopher and send it to the test function
    temp1 = philosophers + i-1;
    test ( temp1 % philosophers );

    // Increment to the next philosopher and send it to the test function
    temp2 = i+1;
    test ( temp2 % philosophers );

    pthread_mutex_unlock ( &dine.mutex );

}

/*
 * This function is used to change the state of the current philosopher
 * to HUNGRY as the current philosopher has 'picked up' their chopsticks
 * allowing its nieghbours to check for chopsticks
 */
void pickup ( int i ) {

    // Alter the condition of the current thread
    pthread_mutex_lock ( &dine.mutex );

    // Change current philosophers state to HUNGRY
    dine.state[i] = HUNGRY;

    // Send the current philosopher to the test function
    test ( i );

    // Check if the current philosopher is not EATING
    if ( !(EATING == dine.state[i]) ) {

        // Allow other threads to share data in relation to the philosophers condition
        pthread_cond_wait ( &dine.condition[i], &dine.mutex );

    }

    pthread_mutex_unlock ( &dine.mutex );

}

/*
 * This function will iterate until the maximum number of meals have been
 * reached
 */
void *philosopher ( void *temp ) {

    // Integer variables used throughout this function
    int random_time = 0;
    int i = * ( int * ) temp;

    // Infinitely loop until the maximum number of meals have been reached
    while ( true ) {

        // Assign the current philosopher to the THINKING state
        dine.state[i] = THINKING;

        // Print to stdout
        printf ( "Philosopher %d thinking\n", i+1 );

        // Randomize program sleep time between 1 and 3
        srand ( time ( NULL ) + i );
        random_time = ( rand() % 3 ) + 1;
        sleep ( random_time );

        // Send the current philosopher to 'pick up' chopsticks
        pickup ( i );

        // Print to stdout
        printf ( "Philosopher %d eating\n", i+1 );

        // Randomize program sleep time between 1 and 3
        srand ( time ( NULL ) + i );
        random_time = ( rand() % 3 ) + 1;
        sleep ( random_time );

        // Send the current philosopher to 'put down' chopsticks
        putdown ( i );

    }

}

/*
 * This function is used to initialize all philosophers to the THINKING state
 */
void initialization_code () {

    int i = 0;

    // Assign all philosophers to the THINKING state
    for ( i = 0; i < philosophers; i++ ) {

        dine.state[i] = THINKING;

    }

}

int main ( int argc, char **argv ) {

    // Integer variables used throughout the program
    int i = 0;
    int input1 = 0;
    int input2 = 0;

    // Char variables used for the user input
    char *user_input = malloc ( sizeof ( char * ) + 100 );
    char *token = malloc ( sizeof ( char * ) + 100 );

    // Error checking for command line arguments
    if ( argc != 3 ) {
        printf ( "Error: Input format: \"./dine <number of philosophers> <number of times each philosopher should eat>\"\n" );
        exit ( -1 );
    }

    // Convert char * to integers
    input1 = atoi ( argv[1] );
    input2 = atoi ( argv[2] );

    // Error checking for command line arguments
    if ( input1 < 2 || input1 > 25 ) {
        printf ( "Error: The number of philosophers must be in the range of 2-25\n" );
        exit ( -1 );
    }

    // Error checking for command line arguments
    if ( input2 < 1 || input2 > 1000 ) {
        printf ( "Error: The number of times each philosopher should eat must be in the range of 1-1000\n" );
        exit ( -1 );
    }

    // Declare pthread and id
    pthread_t my_thread[input1];
    int id[input1];

    // Initialize the total number of philosophers
    philosophers = input1;
    // Initialize the total number of meals
    eat = input2 * philosophers;

    // Initialize the mutex
    pthread_mutex_init ( &dine.mutex, NULL );

    // Initialize all philosophers to the THINKING state
    initialization_code ( input1 );

    // Initialize pthreads and is
    for ( i = 0; i < input1; i++ ) {

        id[i] = i;
        pthread_create ( &my_thread[i], NULL, &philosopher, &id[i] );

    }

    // Thread will continue to execute while other threads concurrently execute
    for ( i = 0; i < input1; i++ ) {

        pthread_join ( my_thread[i], NULL );

    }

    // Free char * variables
    free ( user_input );
    free ( token );

    // End of program
    return ( 0 );

}