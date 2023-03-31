// Name: Carson Mifsud
// Date: 2021-03-19
/*
 * Description: This program emulates a simulator that can take varying sizes or processes, load them into a memory
 * sumulator of 128 MB, and swap processes out as needed to create holes for processes to execute. The swapping process
 * is dependent on the algorithm being used. These algorithms are best fit, worst fit, next fit, and first fit. The program
 * accepts two command line arguments, the first being the name of the file which contains processes and the second being
 * either 'best', 'first', 'worst', or 'next' which indicates the algorithm to be used on the given file of processes.
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

// Struct for a pid which holds its unique letter, the process size, and the age which is the amount of time the process has been in memory
typedef struct pid {

	char *letter;
	int process_size;
    int age;

} pid;

// Struct for a pid in memory which holds its unique letter, the process size, the age which is the amount of time the process has been in memory,
// the starting memory number, and the time which is the amount of time the pid has been re-added to the queue
typedef struct in_memory {

	int time;
    int process_size;
    int process_start;
    char *letter;
    int age;

} in_memory;

/*
 * This function emulates the first fit algorithm with the given processes
 */
void first_fit ( pid *process_array ) {

    // Integer variables used throughout the function
    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    int l = 0;
    int tracker = 0;
    int temp_memory = 0;
    int start_temp = 0;
    int swap_out = 0;
    int added_counter = 0;
    int counter = 0;
    int number_of_processes = 0;
    int total_number_of_processes = 0;
    int number_of_holes = 0;
    int *total_memory = malloc ( sizeof ( int ) * 1000 );
    int *average_number_processes = malloc ( sizeof ( int ) * 1000 );
    int *average_number_holes = malloc ( sizeof ( int ) * 1000 );

    // Float variables used throughout the function
    float cumulative_mem_final = 0;
    float average_number_of_processes = 0;
    float average_number_of_mem = 0;
    float average_number_of_holes = 0;
    float mem_usage = 0;
    float *average_number_mem = malloc ( sizeof ( float ) * 1000 );
    float *cumulative_mem = malloc ( sizeof ( float ) * 1000 );

    // Boolean variables used throughout the function
    bool insert_check = false;
    bool swap_check = false;

    // Array of structs to hold process information in memory
    in_memory *process_in_memory = malloc ( ( sizeof ( int ) + sizeof ( int ) + sizeof ( int ) + sizeof ( int ) + sizeof ( char * ) ) * 1000 );

    // Initialize array to empty
    for ( i = 0; i < 128; i++ ) {
        total_memory[i] = -1;
    }

    i = 0;

    // While the queue is not empty, continue to iterate
    while ( process_array[i].letter != NULL ) {

        // Reset variables
        k = 0;
        number_of_holes = 0;

        // Initialize with process size
        temp_memory = process_array[i].process_size;

        // Iterate through memory and check if there is a hole large enough 
        for ( j = 0; j < 128; j++ ) {

            insert_check = false;

            // Check for a hole and get size of the hole
            if ( total_memory[j] == -1 ) {
                k = k + 1;
            } else {
                k = 0;
            }

            // If the size of the hole is large enough to store the process, insert the process into memory
            if ( k == temp_memory ) {

                // Initialize memory struct with the current processes information
                process_in_memory[tracker].process_size = k;
                process_in_memory[tracker].letter = process_array[i].letter;
                process_in_memory[tracker].age = process_array[i].age;
                start_temp = j - k + 1;
                process_in_memory[tracker].process_start = start_temp;

                // Insert the process into memory by filling the memory array with 1, to indicate memory space has been filled
                for ( m = start_temp; m < (k + start_temp); m++ ) {
                    total_memory[m] = 1;
                }

                // Set the insert_check variable to true as a process has been inserted into memory
                insert_check = true;

                // Increment the age and time related variables
                number_of_processes = number_of_processes + 1;
                total_number_of_processes = total_number_of_processes + 1;
                added_counter = added_counter + 1;
                j = 128;

            }

        }

        tracker = tracker + 1;

        // If no hole was large enough for the current process, and the process was not inserted into memory
        while ( insert_check == false ) {

            // Decrement the number of processes in memory
            if ( process_in_memory[swap_out].letter != NULL ) {
                number_of_processes = number_of_processes - 1;
            }

            swap_check = true;

            swap_out = counter;

            // Find the process which has been in memory the longest
            for ( l = counter; l < added_counter; l++ ) {
                
                if ( process_in_memory[swap_out].time < process_in_memory[l].time ) {
                    swap_out = l;
                }

            }

            // Remove the oldest process from memory
            for ( l = process_in_memory[swap_out].process_start; l < (process_in_memory[swap_out].process_size + process_in_memory[swap_out].process_start); l++ ) {
                total_memory[l] = -1;
            }

            for ( l = 0; process_array[l].letter != NULL; l++ ) {}

            // Increment the age of the process which was swapped out
            process_in_memory[swap_out].age = process_in_memory[swap_out].age + 1;

            // If the next process has been added less than 3 times, it may be re-added to the memory
            if ( process_in_memory[swap_out].age < 3 ) {
                process_array[l].letter = process_in_memory[swap_out].letter;
                process_array[l].process_size = process_in_memory[swap_out].process_size;
                process_array[l].age = process_in_memory[swap_out].age;
            }

            k = 0;

            // Check if there is a hole large enough to store the next process into memory
            for ( j = 0; j < 128; j++ ) {

                if ( total_memory[j] == -1 ) {
                    k = k + 1;
                } else {
                    k = 0;
                }

                // If there is a hole large enough to store the process into memory, set the insert_check variable to true
                if ( k == temp_memory ) {
                    insert_check = true;
                }

            }

            counter = counter + 1;

        }

        // Increment the age of the process which was swapped out
        for ( l = 0; l < added_counter; l++ ) {
            process_in_memory[l].time = process_in_memory[l].time + 1;
        }

        // If no process was swapped
        if ( swap_check == false ) {

            // Increment the counter used to get the process sizes
            i = i + 1;

            k = 0;

            // Get the total number of holes in memory at its current state
            for ( j = 0; j < 128; j++ ) {

                if ( total_memory[j] == -1 ) {
                    number_of_holes = number_of_holes + 1;
                    while ( total_memory[j] == -1 ) {
                        j = j + 1;
                    }
                } else {
                    while ( total_memory[j] == 1 ) {
                        k = k + 1;
                        j = j + 1;
                    }
                }

            }

            cumulative_mem_final = 0;

            // Calculate the percentage of memory currently being used
            mem_usage = k / 128.0;
            mem_usage = mem_usage * 100;

            // Calculate the average of percentages of memory used
            cumulative_mem[i-1] = mem_usage;

            // Calculate the final cumulative memory percentage
            for ( k = 0; k < i; k++ ) {
                cumulative_mem_final = cumulative_mem_final + cumulative_mem[k];
            }
            cumulative_mem_final = cumulative_mem_final / k;

            average_number_processes[i-1] = number_of_processes;
            average_number_holes[i-1] = number_of_holes;
            average_number_mem[i-1] = mem_usage;

            // Print the calculated information and the current state of the memory to stdout
            printf ( "pid loaded, #processess = %d, #holes = %d, %%memusage = %.0f, cumulative %%mem = %.0f\n", number_of_processes, number_of_holes, mem_usage, cumulative_mem_final );

        } else {
            swap_check = false;
        }

    }

    // Calculate the average number of processes
    for ( k = 0; k < i; k++ ) {
        average_number_of_processes = average_number_of_processes + average_number_processes[k];
    }
    average_number_of_processes = average_number_of_processes / k;

    // Calculate the average number of holes
    for ( k = 0; k < i; k++ ) {
        average_number_of_holes = average_number_of_holes + average_number_holes[k];
    }
    average_number_of_holes = average_number_of_holes / k;

    // Calculate the average percentage of memory usage
    for ( k = 0; k < i; k++ ) {
        average_number_of_mem = average_number_of_mem + average_number_mem[k];
    }
    average_number_of_mem = average_number_of_mem / k;

    // Print the calculated information and the final averages to stdout
    printf ( "Total loads = %d, average #processes = %.1f, average #holes = %.1f, culmulative %%mem = %.0f\n", total_number_of_processes, average_number_of_processes, average_number_of_holes, average_number_of_mem );

}

/*
 * This function emulates the best fit algorithm with the given processes
 */
void best_fit ( pid *process_array ) {

    // Integer variables used throughout the function
    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    int n = 0;
    int l = 0;
    int tracker = 0;
    int temp_memory = 0;
    int start_temp = 0;
    int swap_out = 0;
    int added_counter = 0;
    int counter = 0;
    int number_of_processes = 0;
    int total_number_of_processes = 0;
    int number_of_holes = 0;
    int *total_memory = malloc ( sizeof ( int ) * 1000 );
    int *average_number_processes = malloc ( sizeof ( int ) * 1000 );
    int *average_number_holes = malloc ( sizeof ( int ) * 1000 );

    // Float variables used throughout the function
    float mem_usage = 0;
    float cumulative_mem_final = 0;
    float average_number_of_processes = 0;
    float average_number_of_mem = 0;
    float average_number_of_holes = 0;
    float *average_number_mem = malloc ( sizeof ( float ) * 1000 );
    float *cumulative_mem = malloc ( sizeof ( float ) * 1000 );

    // Boolean variables used throughout the function
    bool insert_check = false;
    bool swap_check = false;

    // Array of structs to hold process information in memory
    in_memory *process_in_memory = malloc ( ( sizeof ( int ) + sizeof ( int ) + sizeof ( int ) + sizeof ( int ) + sizeof ( char * ) ) * 1000 );

    // Initialize array to empty
    for ( i = 0; i < 128; i++ ) {
        total_memory[i] = -1;
    }

    i = 0;

    // While the queue is not empty, continue to iterate
    while ( process_array[i].letter != NULL ) {

        // Declare integer arrays
        int *hole_space = malloc ( sizeof ( int ) * 1000 );
        int *hole_space_end = malloc ( sizeof ( int ) * 1000 );
        int closest_start = 0;
        int temp_calc = 0;
        int final_temp_calc = 0;

        // Reset variables
        k = 0;
        n = 0;
        number_of_holes = 0;

        // Initialize with process size
        temp_memory = process_array[i].process_size;

        k = 0;

        // Check and store all sizes of holes and processes currently occupied in memory 
        for ( j = 0; j < 128; j++ ) {

            if ( total_memory[j] == -1 ) {
                while ( total_memory[j] == -1 ) {
                    j = j + 1;
                }
                hole_space_end[n] = j;
                n = n + 1;
                k = k + 1;
            } else {
                while ( total_memory[j] == 1 ) {
                    j = j + 1;
                }
                hole_space[n] = j;
                k = 0;
            }

        }

        // If there are no processes in memory, the hole is of size 128
        if ( k == 128 ) {
            hole_space[n] = 0;
            n = n + 1;
        }

        // Calculate the size of the first hole
        temp_calc = hole_space_end[0] - hole_space[0];
        final_temp_calc = hole_space_end[0] - hole_space[0];

        // Calculate the sizes of all holes currently in the memory
        for ( j = 0; j < n; j++ ) {

            temp_calc = hole_space_end[j] - hole_space[j];

            // If the temp_cal is less than the newly calculated hole size
            if ( temp_calc < final_temp_calc ) {

                // If the current process fits in the current hole
                if ( (hole_space[j] + temp_memory) <= hole_space_end[j] ) {

                    // Initialize closet_fit to the hole that is of best fit for the current process
                    closest_start = hole_space[j];

                    final_temp_calc = temp_calc;

                }
            }

        }

        k = 0;

        // Begin adding the new process in from the first index of the hole of best fit
        for ( j = closest_start; j < 128; j++ ) {

            insert_check = false;

            // Check for a hole and get size of the hole
            if ( total_memory[j] == -1 ) {
                k = k + 1;
            } else {
                k = 0;
            }

            // If the size of the hole is large enough to store the process, insert the process into memory
            if ( k == temp_memory ) {

                // Initialize memory struct with the current processes information
                process_in_memory[tracker].process_size = k;
                process_in_memory[tracker].letter = process_array[i].letter;
                process_in_memory[tracker].age = process_array[i].age;
                start_temp = j - k + 1;
                process_in_memory[tracker].process_start = start_temp;

                // Insert the process into memory by filling the memory array with 1, to indicate memory space has been filled
                for ( m = start_temp; m < (k + start_temp); m++ ) {
                    total_memory[m] = 1;
                }

                // Set the insert_check variable to true as a process has been inserted into memory
                insert_check = true;

                // Increment the age and time related variables
                number_of_processes = number_of_processes + 1;
                total_number_of_processes = total_number_of_processes + 1;
                added_counter = added_counter + 1;
                j = 128;

            }

        }

        tracker = tracker + 1;

        // If no hole was large enough for the current process, and the process was not inserted into memory
        if ( insert_check == false ) {

            // Decrement the number of processes in memory
            if ( process_in_memory[swap_out].letter != NULL ) {
                number_of_processes = number_of_processes - 1;
            }

            swap_check = true;

            swap_out = counter;

            // Find the process which has been in memory the longest
            for ( l = counter; l < added_counter; l++ ) {
                
                if ( process_in_memory[swap_out].time < process_in_memory[l].time ) {
                    swap_out = l;
                }

            }

            // Remove the oldest process from memory
            for ( l = process_in_memory[swap_out].process_start; l < (process_in_memory[swap_out].process_size + process_in_memory[swap_out].process_start); l++ ) {
                total_memory[l] = -1;
            }

            for ( l = 0; process_array[l].letter != NULL; l++ ) {}

            // Increment the age of the process which was swapped out
            process_in_memory[swap_out].age = process_in_memory[swap_out].age + 1;

            // If the next process has been added less than 3 times, it may be re-added to the memory
            if ( process_in_memory[swap_out].age < 3 ) {
                process_array[l].letter = process_in_memory[swap_out].letter;
                process_array[l].process_size = process_in_memory[swap_out].process_size;
                process_array[l].age = process_in_memory[swap_out].age;
            }

            counter = counter + 1;

        }

        // Increment the time variable of the current process
        for ( l = 0; l < added_counter; l++ ) {
            process_in_memory[l].time = process_in_memory[l].time + 1;
        }

        // If no process was swapped
        if ( swap_check == false ) {

            // Increment the counter used to get the process sizes
            i = i + 1;

            k = 0;

            // Get the total number of holes in memory at its current state
            for ( j = 0; j < 128; j++ ) {

                if ( total_memory[j] == -1 ) {
                    number_of_holes = number_of_holes + 1;
                    while ( total_memory[j] == -1 ) {
                        j = j + 1;
                    }
                } else {
                    while ( total_memory[j] == 1 ) {
                        k = k + 1;
                        j = j + 1;
                    }
                }

            }

            cumulative_mem_final = 0;

            // Calculate the percentage of memory currently being used
            mem_usage = k / 128.0;
            mem_usage = mem_usage * 100;

            // Calculate the average of percentages of memory used
            cumulative_mem[i-1] = mem_usage;

            // Calculate the final cumulative memory percentage
            for ( k = 0; k < i; k++ ) {
                cumulative_mem_final = cumulative_mem_final + cumulative_mem[k];
            }

            cumulative_mem_final = cumulative_mem_final / k;

            average_number_processes[i-1] = number_of_processes;
            average_number_holes[i-1] = number_of_holes;
            average_number_mem[i-1] = mem_usage;

            // Print the calculated information and the current state of the memory to stdout
            printf ( "pid loaded, #processess = %d, #holes = %d, %%memusage = %.0f, cumulative %%mem = %.0f\n", number_of_processes, number_of_holes, mem_usage, cumulative_mem_final );

        } else {
            swap_check = false;
        }

    }

    // Calculate the average number of processes
    for ( k = 0; k < i; k++ ) {
        average_number_of_processes = average_number_of_processes + average_number_processes[k];
    }
    average_number_of_processes = average_number_of_processes / k;

    // Calculate the average number of holes
    for ( k = 0; k < i; k++ ) {
        average_number_of_holes = average_number_of_holes + average_number_holes[k];
    }
    average_number_of_holes = average_number_of_holes / k;

    // Calculate the average percentage of memory usage
    for ( k = 0; k < i; k++ ) {
        average_number_of_mem = average_number_of_mem + average_number_mem[k];
    }
    average_number_of_mem = average_number_of_mem / k;

    // Print the calculated information and the final averages to stdout
    printf ( "Total loads = %d, average #processes = %.1f, average #holes = %.1f, culmulative %%mem = %.0f\n", total_number_of_processes, average_number_of_processes, average_number_of_holes, average_number_of_mem );

}

/*
 * This function emulates the next fit algorithm with the given processes
 */
void next_fit ( pid *process_array ) {

    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    int n = 0;
    int l = 0;
    int tracker = 0;
    int temp_memory = 0;
    float mem_usage = 0;
    int start_temp = 0;
    int swap_out = 0;
    int added_counter = 0;
    int counter = 0;
    int number_of_processes = 0;
    float cumulative_mem_final = 0;
    int total_number_of_processes = 0;
    float average_number_of_processes = 0;
    float average_number_of_mem = 0;
    float average_number_of_holes = 0;
    int number_of_holes = 0;
    int *total_memory = malloc ( sizeof ( int ) * 1000 );
    int *average_number_processes = malloc ( sizeof ( int ) * 1000 );
    int *average_number_holes = malloc ( sizeof ( int ) * 1000 );
    float *average_number_mem = malloc ( sizeof ( float ) * 1000 );
    float *cumulative_mem = malloc ( sizeof ( float ) * 1000 );

    bool insert_check = false;
    bool swap_check = false;

    // Array of structs to hold process information in memory
    in_memory *process_in_memory = malloc ( ( sizeof ( int ) + sizeof ( int ) + sizeof ( int ) + sizeof ( int ) + sizeof ( char * ) ) * 1000 );

    // Initialize array to empty
    for ( i = 0; i < 128; i++ ) {
        total_memory[i] = -1;
    }

    i = 0;

    // While the queue is not empty, continue to iterate
    while ( process_array[i].letter != NULL ) {

        // Declare integer arrays
        int *hole_space = malloc ( sizeof ( int ) * 1000 );
        int *hole_space_end = malloc ( sizeof ( int ) * 1000 );
        int closest_start = 0;

        // Reset variables
        k = 0;
        n = 0;
        number_of_holes = 0;

        // Initialize with process size
        temp_memory = process_array[i].process_size;

        k = 0;

        // Check and store all sizes of holes and processes currently occupied in memory 
        for ( j = 0; j < 128; j++ ) {

            if ( total_memory[j] == -1 ) {
                while ( total_memory[j] == -1 ) {
                    j = j + 1;
                }
                hole_space_end[n] = j;
                n = n + 1;
                k = k + 1;
            } else {
                while ( total_memory[j] == 1 ) {
                    j = j + 1;
                }
                hole_space[n] = j;
                k = 0;
            }

        }

        // If there are no processes in memory, the hole is of size 128
        if ( k == 128 ) {
            hole_space[n] = 0;
            n = n + 1;
        }

        closest_start = hole_space[0];

        // Loop through the holes in memory
        for ( j = 0; j < n; j++ ) {

            // IF the hole is large enough to store the current processes size
            if ( (hole_space[j] + temp_memory) <= hole_space_end[j] ) {

                // Initialize closet_fit to the hole that is next
                closest_start = hole_space[j];

                if ( j < 0 ) {
                    j = 128;
                }

            }

        }

        k = 0;

        // Begin adding the new process in from the first index of the next hole
        for ( j = closest_start; j < 128; j++ ) {

            insert_check = false;

            // Check for a hole and get size of the hole
            if ( total_memory[j] == -1 ) {
                k = k + 1;
            } else {
                k = 0;
            }

            // If the size of the hole is large enough to store the process, insert the process into memory
            if ( k == temp_memory ) {

                // Initialize memory struct with the current processes information
                process_in_memory[tracker].process_size = k;
                process_in_memory[tracker].letter = process_array[i].letter;
                process_in_memory[tracker].age = process_array[i].age;
                start_temp = j - k + 1;
                process_in_memory[tracker].process_start = start_temp;

                // Insert the process into memory by filling the memory array with 1, to indicate memory space has been filled
                for ( m = start_temp; m < (k + start_temp); m++ ) {
                    total_memory[m] = 1;
                }

                // Set the insert_check variable to true as a process has been inserted into memory
                insert_check = true;

                // Increment the age and time related variables
                number_of_processes = number_of_processes + 1;
                total_number_of_processes = total_number_of_processes + 1;
                added_counter = added_counter + 1;
                j = 128;

            }

        }

        tracker = tracker + 1;

        // If no hole was large enough for the current process, and the process was not inserted into memory
        if ( insert_check == false ) {

            // Decrement the number of processes in memory
            if ( process_in_memory[swap_out].letter != NULL ) {
                number_of_processes = number_of_processes - 1;
            }

            swap_check = true;

            swap_out = counter;

            // Find the process which has been in memory the longest
            for ( l = counter; l < added_counter; l++ ) {
                
                if ( process_in_memory[swap_out].time < process_in_memory[l].time ) {
                    swap_out = l;
                }

            }

            // Remove the oldest process from memory
            for ( l = process_in_memory[swap_out].process_start; l < (process_in_memory[swap_out].process_size + process_in_memory[swap_out].process_start); l++ ) {
                total_memory[l] = -1;
            }

            for ( l = 0; process_array[l].letter != NULL; l++ ) {}

            // Increment the age of the process which was swapped out
            process_in_memory[swap_out].age = process_in_memory[swap_out].age + 1;

            // If the next process has been added less than 3 times, it may be re-added to the memory
            if ( process_in_memory[swap_out].age < 3 ) {
                process_array[l].letter = process_in_memory[swap_out].letter;
                process_array[l].process_size = process_in_memory[swap_out].process_size;
                process_array[l].age = process_in_memory[swap_out].age;
            }

            counter = counter + 1;

        }

        // Increment the time variable of the current process
        for ( l = 0; l < added_counter; l++ ) {
            process_in_memory[l].time = process_in_memory[l].time + 1;
        }

        // If no process was swapped
        if ( swap_check == false ) {

            // Increment the counter used to get the process sizes
            i = i + 1;

            k = 0;

            // Get the total number of holes in memory at its current state
            for ( j = 0; j < 128; j++ ) {

                if ( total_memory[j] == -1 ) {
                    number_of_holes = number_of_holes + 1;
                    while ( total_memory[j] == -1 ) {
                        j = j + 1;
                    }
                } else {
                    while ( total_memory[j] == 1 ) {
                        k = k + 1;
                        j = j + 1;
                    }
                }

            }

            cumulative_mem_final = 0;

            // Calculate the percentage of memory currently being used
            mem_usage = k / 128.0;
            mem_usage = mem_usage * 100;

            // Calculate the average of percentages of memory used
            cumulative_mem[i-1] = mem_usage;

            // Calculate the final cumulative memory percentage
            for ( k = 0; k < i; k++ ) {
                cumulative_mem_final = cumulative_mem_final + cumulative_mem[k];
            }

            cumulative_mem_final = cumulative_mem_final / k;

            average_number_processes[i-1] = number_of_processes;
            average_number_holes[i-1] = number_of_holes;
            average_number_mem[i-1] = mem_usage;

            // Print the calculated information and the current state of the memory to stdout
            printf ( "pid loaded, #processess = %d, #holes = %d, %%memusage = %.0f, cumulative %%mem = %.0f\n", number_of_processes, number_of_holes, mem_usage, cumulative_mem_final );

        } else {
            swap_check = false;
        }

    }

    // Calculate the average number of processes
    for ( k = 0; k < i; k++ ) {
        average_number_of_processes = average_number_of_processes + average_number_processes[k];
    }
    average_number_of_processes = average_number_of_processes / k;

    // Calculate the average number of holes
    for ( k = 0; k < i; k++ ) {
        average_number_of_holes = average_number_of_holes + average_number_holes[k];
    }
    average_number_of_holes = average_number_of_holes / k;

    // Calculate the average percentage of memory usage
    for ( k = 0; k < i; k++ ) {
        average_number_of_mem = average_number_of_mem + average_number_mem[k];
    }
    average_number_of_mem = average_number_of_mem / k;

    // Print the calculated information and the final averages to stdout
    printf ( "Total loads = %d, average #processes = %.1f, average #holes = %.1f, culmulative %%mem = %.0f\n", total_number_of_processes, average_number_of_processes, average_number_of_holes, average_number_of_mem );

}

/*
 * This function emulates the worst fit algorithm with the given processes
 */
void worst_fit ( pid *process_array ) {

    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    int n = 0;
    int l = 0;
    int tracker = 0;
    int temp_memory = 0;
    float mem_usage = 0;
    int start_temp = 0;
    int swap_out = 0;
    int added_counter = 0;
    int counter = 0;
    int number_of_processes = 0;
    float cumulative_mem_final = 0;
    int total_number_of_processes = 0;
    float average_number_of_processes = 0;
    float average_number_of_mem = 0;
    float average_number_of_holes = 0;
    int number_of_holes = 0;
    int *total_memory = malloc ( sizeof ( int ) * 1000 );
    int *average_number_processes = malloc ( sizeof ( int ) * 1000 );
    int *average_number_holes = malloc ( sizeof ( int ) * 1000 );
    float *average_number_mem = malloc ( sizeof ( float ) * 1000 );
    float *cumulative_mem = malloc ( sizeof ( float ) * 1000 );

    bool insert_check = false;
    bool swap_check = false;

    // Array of structs to hold process information in memory
    in_memory *process_in_memory = malloc ( ( sizeof ( int ) + sizeof ( int ) + sizeof ( int ) + sizeof ( int ) + sizeof ( char * ) ) * 1000 );

    // Initialize array to empty
    for ( i = 0; i < 128; i++ ) {
        total_memory[i] = -1;
    }

    i = 0;

    // While the queue is not empty, continue to iterate
    while ( process_array[i].letter != NULL ) {

        // Declare integer arrays
        int *hole_space = malloc ( sizeof ( int ) * 1000 );
        int *hole_space_end = malloc ( sizeof ( int ) * 1000 );
        int closest_start = 0;
        int temp_calc = 0;
        int final_temp_calc = 0;

        // Reset variables
        k = 0;
        n = 0;
        number_of_holes = 0;

        // Initialize with process size
        temp_memory = process_array[i].process_size;

        k = 0;

        // Check and store all sizes of holes and processes currently occupied in memory 
        for ( j = 0; j < 128; j++ ) {

            if ( total_memory[j] == -1 ) {
                while ( total_memory[j] == -1 ) {
                    j = j + 1;
                }
                hole_space_end[n] = j;
                n = n + 1;
                k = k + 1;
            } else {
                while ( total_memory[j] == 1 ) {
                    j = j + 1;
                }
                hole_space[n] = j;
                k = 0;
            }

        }

        // If there are no processes in memory, the hole is of size 128
        if ( k == 128 ) {
            hole_space[n] = 0;
            n = n + 1;
        }

        // Calculate the size of the first hole
        temp_calc = hole_space_end[0] - hole_space[0];
        final_temp_calc = hole_space_end[0] - hole_space[0];

        // Calculate the sizes of the rest of the holes
        for ( j = 0; j < n; j++ ) {

            temp_calc = hole_space_end[j] - hole_space[j];

            // If the temp_cal is greater than the newly calculated hole size
            if ( temp_calc > final_temp_calc ) {

                // If the current process fits in the current hole
                if ( (hole_space[j] + temp_memory) <= hole_space_end[j] ) {

                    // Initialize closet_fit to the hole that is of worst fit for the current process
                    closest_start = hole_space[j];

                    final_temp_calc = temp_calc;

                }
            }

        }

        k = 0;

        // Begin adding the new process in from the first index of the hole of worst fit
        for ( j = closest_start; j < 128; j++ ) {

            insert_check = false;

            // Check for a hole and get size of the hole
            if ( total_memory[j] == -1 ) {
                k = k + 1;
            } else {
                k = 0;
            }

            // If the size of the hole is large enough to store the process, insert the process into memory
            if ( k == temp_memory ) {

                // Initialize memory struct with the current processes information
                process_in_memory[tracker].process_size = k;
                process_in_memory[tracker].letter = process_array[i].letter;
                process_in_memory[tracker].age = process_array[i].age;
                start_temp = j - k + 1;
                process_in_memory[tracker].process_start = start_temp;

                // Insert the process into memory by filling the memory array with 1, to indicate memory space has been filled
                for ( m = start_temp; m < (k + start_temp); m++ ) {
                    total_memory[m] = 1;
                }

                // Set the insert_check variable to true as a process has been inserted into memory
                insert_check = true;

                // Increment the age and time related variables
                number_of_processes = number_of_processes + 1;
                total_number_of_processes = total_number_of_processes + 1;
                added_counter = added_counter + 1;
                j = 128;

            }

        }

        tracker = tracker + 1;

        // If no hole was large enough for the current process, and the process was not inserted into memory
        if ( insert_check == false ) {

            // Decrement the number of processes in memory
            if ( process_in_memory[swap_out].letter != NULL ) {
                number_of_processes = number_of_processes - 1;
            }

            swap_check = true;

            swap_out = counter;

            // Find the process which has been in memory the longest
            for ( l = counter; l < added_counter; l++ ) {
                
                if ( process_in_memory[swap_out].time < process_in_memory[l].time ) {
                    swap_out = l;
                }

            }

            // Remove the oldest process from memory            
            for ( l = process_in_memory[swap_out].process_start; l < (process_in_memory[swap_out].process_size + process_in_memory[swap_out].process_start); l++ ) {
                total_memory[l] = -1;
            }

            for ( l = 0; process_array[l].letter != NULL; l++ ) {}

            // Increment the age of the process which was swapped out
            process_in_memory[swap_out].age = process_in_memory[swap_out].age + 1;

            // If the next process has been added less than 3 times, it may be re-added to the memory
            if ( process_in_memory[swap_out].age < 3 ) {
                process_array[l].letter = process_in_memory[swap_out].letter;
                process_array[l].process_size = process_in_memory[swap_out].process_size;
                process_array[l].age = process_in_memory[swap_out].age;
            }

            counter = counter + 1;

        }

        // Increment the time variable of the current process
        for ( l = 0; l < added_counter; l++ ) {
            process_in_memory[l].time = process_in_memory[l].time + 1;
        }

        // If no process was swapped
        if ( swap_check == false ) {

            // Increment the counter used to get the process sizes
            i = i + 1;

            k = 0;

            // Get the total number of holes in memory at its current state
            for ( j = 0; j < 128; j++ ) {

                if ( total_memory[j] == -1 ) {
                    number_of_holes = number_of_holes + 1;
                    while ( total_memory[j] == -1 ) {
                        j = j + 1;
                    }
                } else {
                    while ( total_memory[j] == 1 ) {
                        k = k + 1;
                        j = j + 1;
                    }
                }

            }

            cumulative_mem_final = 0;

            // Calculate the percentage of memory currently being used
            mem_usage = k / 128.0;
            mem_usage = mem_usage * 100;

            // Calculate the average of percentages of memory used
            cumulative_mem[i-1] = mem_usage;

            // Calculate the final cumulative memory percentage
            for ( k = 0; k < i; k++ ) {
                cumulative_mem_final = cumulative_mem_final + cumulative_mem[k];
            }
            cumulative_mem_final = cumulative_mem_final / k;

            average_number_processes[i-1] = number_of_processes;
            average_number_holes[i-1] = number_of_holes;
            average_number_mem[i-1] = mem_usage;

            // Print the calculated information and the current state of the memory to stdout
            printf ( "pid loaded, #processess = %d, #holes = %d, %%memusage = %.0f, cumulative %%mem = %.0f\n", number_of_processes, number_of_holes, mem_usage, cumulative_mem_final );

        } else {
            swap_check = false;
        }

    }

    // Calculate the average number of processes
    for ( k = 0; k < i; k++ ) {
        average_number_of_processes = average_number_of_processes + average_number_processes[k];
    }
    average_number_of_processes = average_number_of_processes / k;

    // Calculate the average number of holes
    for ( k = 0; k < i; k++ ) {
        average_number_of_holes = average_number_of_holes + average_number_holes[k];
    }
    average_number_of_holes = average_number_of_holes / k;

    // Calculate the average percentage of memory usage
    for ( k = 0; k < i; k++ ) {
        average_number_of_mem = average_number_of_mem + average_number_mem[k];
    }
    average_number_of_mem = average_number_of_mem / k;

    // Print the calculated information and the final averages to stdout
    printf ( "Total loads = %d, average #processes = %.1f, average #holes = %.1f, culmulative %%mem = %.0f\n", total_number_of_processes, average_number_of_processes, average_number_of_holes, average_number_of_mem );

}

int main ( int argc, char **argv ) {

    // File pointer
    FILE *fp1;

    // Integer variables used throughout the program
    int i = 0;
    int k = 0;
    int temp = 0;

    // Char * variables used to store user input
    char *input1 = malloc ( sizeof ( char * ) + 1000 );
    char *input2 = malloc ( sizeof ( char * ) + 1000 );
    char *temporary = malloc ( sizeof ( char * ) + 1000 );

    // array of struct pids to store the information from the file
    pid *process_array = malloc ( ( sizeof ( char * ) + sizeof ( int ) + sizeof ( int ) ) * 1000 );

    // Error checking for the command line arguments
    if ( argc != 3 ) {
        printf ( "Error: Input format: \"./dine <testfile1> <first / best / next / worst>\"\n" );
        exit ( -1 );
    }

    // Get the first and second command line arguments
    strcpy ( input1, argv[1] );
    strcpy ( input2, argv[2] );

    // Open the file
    fp1 = fopen ( input1, "r" );

    // Scan through the given file and store the letter, and the process size in the array of pid structs
    while ( fscanf ( fp1, "%s", temporary ) != EOF ) {

        if ( k == 0 || k % 2 == 0 ) {
            process_array[i].letter = ( char * ) malloc ( strlen ( temporary ) * sizeof ( char * ) + 10 );
            strcpy ( process_array[i].letter, temporary );
        } else {
            temp = atoi ( temporary );
            process_array[i].process_size = temp;
            process_array[i].age = 0;
            i = i + 1;
        }

        k = k + 1;

    }

    // If the user wishes to run the first fit algorithm
    if ( strcmp ( input2, "first" ) == 0 ) {

        first_fit ( process_array );

    }

    // If the user wishes to run the best fit algorithm
    else if ( strcmp ( input2, "best" ) == 0 ) {

        best_fit ( process_array );

    }

    // If the user wishes to run the worst fit algorithm
    else if ( strcmp ( input2, "worst" ) == 0 ) {

        worst_fit ( process_array );

    }

    // If the user wishes to run the next fit algorithm
    else if ( strcmp ( input2, "next" ) == 0 ) {

        next_fit ( process_array );

    }

    // Close the file
    fclose ( fp1 );

    // End of program
    return ( 0 );

}