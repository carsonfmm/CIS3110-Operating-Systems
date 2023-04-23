// Name: Carson Mifsud
// Date: 2021-02-05
/* Description: Simple UNIX shell program. Handles basic UNIX shell commands in the foreground and background.
 * Handles one level of pipe as well as redirected output to a file and redirected input from a file.
 * Uses the limited environmental variables including PATH, HISTFILE and HOME.
 * Stores users history in a history file and can handle changing directories.
 */

// #includes were provided by Professor Stacey via CourseLink ( 07Processespart1 )
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

// Used to store an array of pids
volatile pid_t *pids;

int main ( int argc, char **argv ) {

    // Declaration of program variables
    int background = 0;
    int start = 0;
    int exit_count = 0;
    int bg = 0;
    int count = 0;
    int tester = 0;
    int hist_tracker = 0;
    int temp_hist = 0;
    int cleared = 1;
    int _check[100];
    int new = 1;
    char **full_command = malloc ( sizeof ( char * ) * 50 );
    char **hist_command = malloc ( sizeof ( char * ) * 100 );
    bool set = false;
    char temp[1000];
    char temp2[1000];
    char buffer[1000];
    char buffer_path[1000];
    char buffer_home[1000];
    char buffer_hist[1000];
    int totalRead = 0;
    bool home_set = false;
    bool path_set = false;
    bool file_set = false;
    char buffer_path2[1000];
    char buffer_home2[1000];
    char buffer_hist2[1000];
    bool home_set2 = false;
    bool path_set2 = false;
    bool file_set2 = false;
    char cwd[100];
    char file_name[1000];
    char save_dir[100];
    pid_t childPid;

    // Map space for shared array
    pids = mmap(0, 32*sizeof(pid_t), PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // If mapping for global variable fails
    if ( !pids ) {
        perror( "Error: mmap failure" );
        exit ( 1 );
    }

    // Set memory for global variable
    memset((void *)pids, 0, 32*sizeof(pid_t));

    // Set all array indexes to be NULL
    for ( int empty = 0; empty < 50; empty++ ) {
        full_command[empty] = NULL;
    }
    for ( int empty = 0; empty < 100; empty++ ) {
        hist_command[empty] = NULL;
    }

    // Creating file pointer for .CIS3110_history
    FILE *fptr;
    // Creating file pointer for .CIS3110_profile
    FILE *fptr2;

    // Opening file in writing mode
    fptr = fopen(".CIS3110_history", "w");
    fptr2 = fopen(".CIS3110_profile", "r");

    // If there is no .CIS3110_profile present, create .CIS3110_profile and open for writting
    if ( fptr2 == NULL ) {
        fptr2 = fopen ( ".CIS3110_profile", "w" );
    }

    if (fptr == NULL) {
        perror ( "Error: File" );
        exit(1);
    }

    // Read in .CIS3110_profile if in current directory
    while ( fgets ( buffer, 1000, fptr2) != NULL ) {

        // Total character count
        totalRead = strlen(buffer);

        // Remove '\n' from buffer
        if ( buffer[totalRead - 1] == '\n'  ) {
            buffer[totalRead - 1] = '\0';
        }

        // $PATH
        if ( buffer[7] == 'P' && buffer[8] == 'A' && buffer[9] == 'T' && buffer[10] == 'H' ) {
            int get_home = 0;
            int org = 0;
            for ( get_home = 12; buffer[get_home] != '\0'; get_home++ ) {
                buffer_path[org] = buffer[get_home];
                org++;
            }
            buffer_path[org] = '\0';
            path_set = true;
        }

        // $HOME
        else if ( buffer[7] == 'H' && buffer[8] == 'O' && buffer[9] == 'M' && buffer[10] == 'E' ) {
            int get_home = 0;
            int org = 0;
            for ( get_home = 12; buffer[get_home] != '\0'; get_home++ ) {
                buffer_home[org] = buffer[get_home];
                org++;
            }
            buffer_home[org] = '\0';
            home_set = true;
        }

        // $HISTFILE
        else if ( buffer[7] == 'H' && buffer[8] == 'I' && buffer[9] == 'S' && buffer[10] == 'T'&& buffer[11] == 'F' && buffer[12] == 'I' && buffer[13] == 'L' && buffer[14] == 'E' ) {
            int get_hist = 0;
            int org = 0;
            for ( get_hist = 16; buffer[get_hist] != '\0'; get_hist++ ) {
                buffer_hist[org] = buffer[get_hist];
                org++;
            }
            buffer_hist[org] = '\0';
            file_set = true;
        }

    }

    // Get current directory
    getcwd ( save_dir, sizeof ( save_dir ) );

    // Check for a change in the .CIS3110_history file name
    if ( file_set == false ) {
        strcpy ( file_name, ".CIS3110_history" );
    } else {
        strcpy ( file_name, buffer_hist );
    }

    // Check for a change in the home diectory
    if ( home_set == false ) {
        getcwd ( cwd, sizeof ( cwd ) );
    } else {
        strcpy ( cwd, buffer_home );
    }

    // Shell while loop
    while ( 1 ) {

        // Declaration of shell variables
        char input[1000];
        char *parameters[10];
        char *before_pipe[10];
        char *after_pipe[10];
        int counter = 1;
        int status;
        bool _path = false;
        int j = 0;
        bool input_f = false;
        bool output_f = false;
        bool pipe_f = false;
        char file_output[1000];
        char file_input[1000];
        int n = 0;
        char dir[1000];
        char commander[1000];
        int bg_error = 0;
        int c_count = 0;
        for ( int empty = 0; empty < 10; empty++ ) {
            before_pipe[empty] = NULL;
            after_pipe[empty] = NULL;
            parameters[empty] = NULL;
        }

        // Shell prompt with current directory
        printf ( "%s> ", getcwd ( dir, 1000 ) );

        // Read user input from stdin
        if ( ( fgets ( input, 1000, stdin ) == NULL ) && ferror ( stdin ) ) {
            perror ( "Error: fgets()" );
        }

        // Duplicate user input
        strcpy ( commander, input );

        for ( int z = 0; commander[z] != '\0'; z++ ) {
            c_count++;
        }
        commander[c_count-1] = '\0';

        if ( feof ( stdin ) ) {
            printf ( "\n" );
            exit ( 0 );
        }

        // Check for '&' symbol indicating a background process
        for ( int i = 0; i <= strlen ( input ); i++ ) {
            if ( i == strlen ( input ) ) {
                if ( input[i-2] == '&' ) {
                    _check[tester] = new;
                    full_command[tester] = (char *) ( calloc ( 100, ( 50 ) + 1 ) );
                    strcpy ( full_command[tester], input );
                    for ( int k = 0; full_command[tester][k] != '\0'; k++ ) {
                        while ( full_command[tester][k] == '&' ) {
                            for ( j = k; full_command[tester][j] != '\0'; j++ ) {
                                full_command[tester][j] = full_command[tester][j+1];
                            }
                            full_command[tester][j] = '\0';
                        }
                    }
                    new++;
                    background = 1;
                }
                else {
                    _check[tester] = 0;
                }
            }
        }

        // Remove '&' from the user input
        if ( background == 1 ) {
            for ( int k = 0; input[k] != '\0'; k++ ) {
                while ( input[k] == '&' ) {
                    for ( j = k; input[j] != '\0'; j++ ) {
                        input[j] = input[j+1];
                    }
                    input[j] = '\0';
                }
            }
        }

        // Add terminating character to input
        input[strlen ( input ) - 1] = '\0';

        // Tokenize input with a ' ' as the delimiter
        char *ptr = strtok ( input, " " );
        parameters[0] = ptr;

        while ( ptr != NULL ) {
            ptr = strtok ( NULL, " " );
            parameters[counter] = ptr;
            counter++;
        }

        // Save command to history
        if ( !(status == -1) ) {
            hist_command[tester] = (char *) ( calloc ( 100, ( 50 ) + 1 ) );
            sprintf(hist_command[tester], " %d  %s", hist_tracker+1, commander);
        }

        // Check if the user has entered a valid file or directory
        if ( (parameters[0][0] == '.' || parameters[0][0] == '/') && status == -1 && bg_error == 0 ) {
            printf ( "-myShell: %s: No such file or directory\n", parameters[0]);
            exit_count++;
            hist_command[tester] = (char *) ( calloc ( 100, ( 50 ) + 1 ) );
            sprintf(hist_command[tester], " %d  %s", hist_tracker+1, commander);
        }
        // Check if the user has entered a valid command
        else if ( bg_error == 0 && status == -1 && !(parameters[0][0] == '.' || parameters[0][0] == '/') ) {
            printf ( "-myShell: %s: command not found\n", parameters[0]);
            exit_count++;
            hist_command[tester] = (char *) ( calloc ( 100, ( 50 ) + 1 ) );
            sprintf(hist_command[tester], " %d  %s", hist_tracker+1, commander);
        }
        // Condiationals for 'cd' command related to $HOME
        else if ( (strcmp ( parameters[0], "cd" ) == 0) && parameters[1] != NULL ) {
            if ( (strcmp ( parameters[1], "$HOME" ) == 0) ) {
                chdir ( cwd );
            }
            else {
                if ( chdir( parameters[1] ) != 0 ) {
                    if ( parameters[1][0] == '~' ) {
                        chdir( save_dir );
                    } else {
                        printf ( "-myShell: %s: No such file or directory\n", parameters[1]);
                    }
                }
            }
        }
        // Handling for $PATH
        else if ( (strcmp ( parameters[0], "echo" ) == 0) && (strcmp ( parameters[1], "$PATH" ) == 0) ) {

            // Ceck if PATH contains other environemtnal variables
            if ( path_set2 == true ) {
                printf ( "%s\n", buffer_path2 );
            } else {
                if ( set == false ) {
                    for ( int h = 0; buffer_path[h] != '\0'; h++ ) {
                        if ( buffer_path[h] == '$' ) {
                            buffer_path[h] = '\0';
                            set = true;
                        }
                    }
                    strcpy( temp, cwd );
                    strcpy( temp2, buffer_path );
                    strcat ( temp2, temp );
                }
                
                if ( set == true ) {
                    printf ( "%s\n", temp2 );
                }
                else if ( path_set == false ) {
                    printf ( "%s\n", getenv("PATH"));
                } else {
                    printf ( "%s\n", buffer_path );
                }
            }
            _path = true;
        }
        // Handling for $HISTFILE
        else if ( (strcmp ( parameters[0], "echo" ) == 0) && (strcmp ( parameters[1], "$HISTFILE" ) == 0) ) {

            if ( file_set2 == true ) {
                printf ( "%s/%s\n", getcwd ( dir, 1000 ), buffer_hist2 );
            } else {
                printf ( "%s/%s\n", getcwd ( dir, 1000 ), file_name );
            }
            _path = true;
        }
        // Handling if the user wishes to alter environemtnal variables 
        else if ( (strcmp ( parameters[0], "export" ) == 0) ) {

            // $PATH
            if ( parameters[1][0] == 'P' && parameters[1][1] == 'A' && parameters[1][2] == 'T' && parameters[1][3] == 'H' ) {
                int get_home2 = 0;
                int org = 0;
                for ( get_home2 = 5; parameters[1][get_home2] != '\0'; get_home2++ ) {
                    buffer_path2[org] = parameters[1][get_home2];
                    org++;
                }
                buffer_path2[org] = '\0';
                path_set2 = true;
            }
            // $HOME
            else if ( parameters[1][0] == 'H' && parameters[1][1] == 'O' && parameters[1][2] == 'M' && parameters[1][3] == 'E' ) {
                int get_home2 = 0;
                int org = 0;
                for ( get_home2 = 5; parameters[1][get_home2] != '\0'; get_home2++ ) {
                    buffer_home2[org] = parameters[1][get_home2];
                    org++;
                }
                buffer_home2[org] = '\0';
                home_set2 = true;
            }
            // $HISTFILE
            else if ( parameters[1][0] == 'H' && parameters[1][1] == 'I' && parameters[1][2] == 'S' && parameters[1][3] == 'T' && parameters[1][4] == 'F' && parameters[1][5] == 'I' && parameters[1][6] == 'L' && parameters[1][7] == 'E' ) {
                int get_hist = 0;
                int org = 0;
                for ( get_hist = 9; parameters[1][get_hist] != '\0'; get_hist++ ) {
                    buffer_hist[org] = parameters[1][get_hist];
                    org++;
                }
                buffer_hist2[org] = '\0';
                file_set2 = true;
            }
            else {
            }
        }
        // Handling for $HOME
        else if ( (strcmp ( parameters[0], "echo" ) == 0) && (strcmp ( parameters[1], "$HOME" ) == 0) ) {
            
            if ( home_set2 == true ) {
                printf ( "%s\n", buffer_home2 );
            } else {
                printf("%s\n", cwd);
            }

            _path = true;
        }
        // Print the users history
        else if ( (strcmp ( parameters[0], "history" ) == 0) && parameters[1] == NULL ) {
            for ( int y = temp_hist; hist_command[y] != NULL; y++ ) {
                printf ( "%s\n", hist_command[y] );
            }
        }
        // Clear the history
        else if ( (strcmp ( parameters[0], "history" ) == 0) && (strcmp ( parameters[1], "-c" ) ) == 0 ) {
            temp_hist = hist_tracker;
            temp_hist++;
            hist_tracker = -1;
            parameters[1] = NULL;
        }
        // Print a desired amount of the users history depeding on the users input
        else if ( (strcmp ( parameters[0], "history" ) == 0) && parameters[1] != NULL ) {
            hist_command[tester] = (char *) ( calloc ( 100, ( 50 ) + 1 ) );
            sprintf(hist_command[tester], " %d  %s", hist_tracker+1, commander);
            int minus = 0;
            start = atoi ( parameters[1] );
            int answer = (tester+1) - start;
            if ( start > tester+1 ) {
                perror ( "The second parameter cannot be larger than the amount of commands in the history\n" );
            } else {
                for ( int empty = answer; empty != tester+1; empty++ ) {
                    printf ( "%s\n", hist_command[empty] );
                    minus++;
                }
            }
        }
        else if ( (bg_error == 0 && _path == false) ) {
            for ( n = 0; parameters[n] != NULL; n++ ) {
                // Checking for redirection and pipe symbols
                if ( strcmp ( parameters[n], ">" ) == 0 ) {
                    output_f = true;
                }
                if ( strcmp ( parameters[n], "<" ) == 0 ) {
                    input_f = true;
                }
                if ( strcmp ( parameters[n], "|" ) == 0 ) {
                    pipe_f = true;
                    int b = n-1;
                    // Create array for commands before the pipe symbol
                    for ( int before = n-1; before > -1; before-- ) {
                        before_pipe[b] = (char *) ( calloc ( 100, ( 50 ) + 1 ) );
                        strcpy ( before_pipe[b], parameters[before] );
                        b--;
                    }
                    b = 0;
                    // Create array for commands after the pipe symbol
                    for ( int after = n+1; parameters[after] != NULL; after++ ) {;
                        after_pipe[b] = (char *) ( calloc ( 100, ( 50 ) + 1 ) );
                        strcpy ( after_pipe[b], parameters[after] );
                        b++;
                    }
                    // Set index of pipe symbol to NULL
                    parameters[n] = NULL;
                }
            }
            // Conditionals handling use of combinations of redirection and pipe
            if ( input_f == true && output_f == false && pipe_f == false ) {
                strcpy ( file_output, parameters[n-1] );
                strcpy ( file_input, parameters[n-1] );

                parameters[n-1] = NULL;
                parameters[n-2] = NULL;
            }
            else if ( input_f == false && output_f == true && pipe_f == false ) {
                strcpy ( file_output, parameters[n-1] );
                strcpy ( file_input, parameters[n-1] );

                parameters[n-1] = NULL;
                parameters[n-2] = NULL;
            }
            else if ( input_f == true && output_f == true && pipe_f == false ) {
                strcpy ( file_output, parameters[n-1] );
                strcpy ( file_input, parameters[n-3] );

                parameters[n-1] = NULL;
                parameters[n-2] = NULL;
                parameters[n-3] = NULL;
                parameters[n-4] = NULL;
            }
            else if ( input_f == true && pipe_f == true && output_f == false ) {
                strcpy ( file_input, parameters[n-3] );

                parameters[n-1] = NULL;
                parameters[n-2] = NULL;
                parameters[n-3] = NULL;
                parameters[n-4] = NULL;
            }
            else if ( input_f == true && pipe_f == true && output_f == true ) {
                strcpy ( file_input, parameters[n-5] );
                strcpy ( file_output, parameters[n-1] );

                parameters[n-1] = NULL;
                parameters[n-2] = NULL;
                parameters[n-3] = NULL;
                parameters[n-4] = NULL;
                parameters[n-5] = NULL;
                parameters[n-6] = NULL;
                after_pipe[1] = NULL;
                after_pipe[2] = NULL;
            }

            // If the user enters exit the program will terminate
            if ( strcmp ( parameters[0], "exit" ) == 0 ) {
                for ( int jh = temp_hist; hist_command[jh] != NULL; jh++ ) {
                    fprintf ( fptr, "%s\n", hist_command[jh] );
                }

                // Check for any outstdaning background processes
                for ( int m = 0; m <= tester-1; m++ ) {
                    pid_t return_pid = waitpid(pids[m], &status, WNOHANG);
                    // Terminate any outstanding background processes before exit
                    if (return_pid == 0 && _check[m] != 0 ) {
                        printf ( "[%d]+  Terminated              %s", _check[m], full_command[m] );
                        kill ( pids[m], -9 );
                    }

                }

                fclose(fptr);
                fclose(fptr2);
                // Print to the user and exit the program
                printf ( "myShell terminating...\n\n" );
                printf ( "[Process completed]\n" );
                exit ( 0 );
            }

            cleared++;
            // Create a new process
            childPid = fork();

            if ( childPid >= 0 ) {
                if ( childPid == 0 ) {
                    // Handling for redirected output
                    if ( output_f == true && input_f == false && pipe_f == false ) {
                        freopen ( file_output, "w+", stdout );
                        status = execvp ( parameters[0], parameters );
                    }
                    // Handling for redirected input
                    else if ( input_f == true && output_f == false && pipe_f == false ) {
                        freopen ( file_input, "rb+", stdin );
                        status = execvp ( parameters[0], parameters );
                        // Check if an invalid file or directory was entered
                        if ( (parameters[0][0] == '.' || parameters[0][0] == '/') && status == -1 ) {
                            printf ( "-myShell: %s: No such file or directory\n", parameters[0]);
                            exit_count++;
                        }
                        // Check if an invalid command was entered
                        else if ( status == -1 && !(parameters[0][0] == '.') ) {
                            printf ( "-myShell: %s: command not found\n", parameters[0]);
                            exit_count++;
                        }
                    }
                    // Handling for redirected input and output
                    else if ( input_f == true && output_f == true && pipe_f == false) {
                        freopen ( file_input, "rb+", stdin );
                        freopen ( file_output, "w+", stdout );
                        status = execvp ( parameters[0], parameters );
                        // Check if an invalid file or directory was entered
                        if ( (parameters[0][0] == '.' || parameters[0][0] == '/') && status == -1 ) {
                            printf ( "-myShell: %s: No such file or directory\n", parameters[0]);
                            exit_count++;
                        }
                        // Check if an invalid command was entered
                        else if ( status == -1 && !(parameters[0][0] == '.') ) {
                            printf ( "-myShell: %s: command not found\n", parameters[0]);
                            exit_count++;
                        }
                    }
                    // Handling for one level of pipe
                    else if ( pipe_f == true && input_f == false && output_f == false ) {

                        int temp = 0;
                        int pipefd[2];
                        pid_t my_pid;

                        pipe ( pipefd );

                        if ( ( my_pid = fork() ) == 0 ) {

                            if ( temp != 0 ) {
                                dup2 ( temp, 0 );
                                close ( temp );
                            }
                            if ( pipefd[1] != 1 ) {
                                dup2 ( pipefd[1], 1 );
                                close ( pipefd[1] );
                            }

                            execvp (before_pipe[0], before_pipe);
                        }

                        close (pipefd[1]);
                        temp = pipefd[0];

                        if ( temp != 0 ) {
                            dup2 ( temp, 0) ;
                        }

                        execvp (after_pipe[0], after_pipe);

                    }
                    // Handling for one level of pipe and redirected input
                    else if ( pipe_f == true && input_f == true && output_f == false ) {

                        freopen ( file_input, "rb+", stdin );
                        // Check if an invalid file or directory was entered
                        if ( (parameters[0][0] == '.' || parameters[0][0] == '/') && status == -1 ) {
                            printf ( "-myShell: %s: No such file or directory\n", parameters[0]);
                            exit_count++;
                        }
                        // Check if an invalid command was entered
                        else if ( status == -1 && !(parameters[0][0] == '.') ) {
                            printf ( "-myShell: %s: command not found\n", parameters[0]);
                            exit_count++;
                        }

                        int temp = 0;
                        int pipefd[2];
                        pid_t my_pid;

                        pipe ( pipefd );

                        if ( ( my_pid = fork() ) == 0 ) {

                            if ( temp != 0 ) {
                                dup2 ( temp, 0 );
                                close ( temp );
                            }
                            if ( pipefd[1] != 1 ) {
                                dup2 ( pipefd[1], 1 );
                                close ( pipefd[1] );
                            }

                            execvp (before_pipe [0], before_pipe);
                        }

                        close (pipefd[1]);
                        temp = pipefd[0];

                        if ( temp != 0 ) {
                            dup2 ( temp, 0) ;
                        }

                        execvp (after_pipe[0], after_pipe);

                    }
                    // Handling for a one level pipe and redirected input and output
                    else if ( pipe_f == true && input_f == true && output_f == true ) {

                        freopen ( file_input, "rb+", stdin );
                        freopen ( file_output, "w+", stdout );
                        // Check if an invalid file or directory was entered
                        if ( (parameters[0][0] == '.' || parameters[0][0] == '/') && status == -1 ) {
                            printf ( "-myShell: %s: No such file or directory\n", parameters[0]);
                            exit_count++;
                        }
                        // Check if an invalid command was entered
                        else if ( status == -1 && !(parameters[0][0] == '.') ) {
                            printf ( "-myShell: %s: command not found\n", parameters[0]);
                            exit_count++;
                        }

                        int temp = 0;
                        int pipefd[2];
                        pid_t my_pid;

                        pipe ( pipefd );

                        if ( ( my_pid = fork() ) == 0 ) {

                            if ( temp != 0 ) {
                                dup2 ( temp, 0 );
                                close ( temp );
                            }
                            if ( pipefd[1] != 1 ) {
                                dup2 ( pipefd[1], 1 );
                                close ( pipefd[1] );
                            }

                            execvp (before_pipe [0], before_pipe);
                        }

                        close (pipefd[1]);
                        temp = pipefd[0];

                        if ( temp != 0 ) {
                            dup2 ( temp, 0) ;
                        }

                        execvp (after_pipe[0], after_pipe);

                    }
                    else {
                        status = execvp ( parameters[0], parameters );
                        // Check if an invalid file or directory was entered
                        if ( (parameters[0][0] == '.' || parameters[0][0] == '/') && status == -1 ) {
                            printf ( "-myShell: %s: No such file or directory\n", parameters[0]);
                            exit_count++;
                        }
                        // Check if an invalid command was entered
                        else if ( status == -1 && !(parameters[0][0] == '.') ) {
                            printf ( "-myShell: %s: command not found\n", parameters[0]);
                            exit_count++;
                        }
                    }
                }
                // Parent process
                else {
                    // If a background process was requested print the number and pid
                    if ( background == 1 ) {
                        if ( exit_count == 0 ) {
                            printf ( "[%d] %d\n", bg+1, childPid );
                            background = 0;
                            bg++;
                        }
                    }
                    else {
                        if ( exit_count == 0 ) {
                            waitpid ( childPid, &status, 0 );
                        }
                    }
                pids[tester] = childPid;
                }
            } else {
                // Fork error
                perror ( "Error: Fork" );
                exit ( -1 );
            }

            // Check if background processes have completed
            for ( int m = 0; m <= tester; m++ ) {
                pid_t return_pid = waitpid(pids[m], &status, WNOHANG);
                // If a background process is completed, print its number and the command used
                if (return_pid == pids[m] && _check[m] != 0 ) {
                    printf ( "[%d]+  Done                    %s", _check[m], full_command[m] );
                    kill ( pids[m], -9 );
                }
            }
        }

        tester = tester + 1;
        hist_tracker++;
        count++;

        if ( exit_count == 1 ) {
            exit ( 0 );
            exit_count = 0;
        }
    }

    return ( 0 );

}
