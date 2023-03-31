// Name: Carson Mifsud
// Date: 2021-04-02
/*
 * Description: This program generates reports to stdout about the file system that display
 * information about the files and directories that appear in a directory that is provided to the
 * program via command line arguments. This program can display information in both the tree
 * directory structure and the inode structure. This program can display multiple sub-levels of a
 * given directory structure up to the 'PATH_MAX' macro defined by limits.h.
 */

#include <limits.h>     /* Limits */
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
#include <semaphore.h>  /* Semaphores */
#include <dirent.h>     /* Directories */
#include <sys/stat.h>   /* Stats */
#include <pwd.h>        /* Current Directory */
#include <fcntl.h>      /* inode */
#include <grp.h>        /* Group */
#include <math.h>       /* Math */

// Struct used to track multiple attributes of a file/directory
typedef struct {

    int level;
    char *path;
    char *name;
    int inode_num;

} directories;

// Global variable to track the total number of files/directories in the entered home directory
int counter = 0;
// Global variable to track the maximum directory level within the entered home directory
int max_level = 0;
// Global variable to track the number of levels in the given directory
int home_level = 0;

// Quick sort algorithm to sort the files/directories in alphabetical order
int alphabetical_qsort ( const void *i, const void *j ) {

    directories *ii = ( directories * ) i;
    directories *ij = ( directories * ) j;
    return strcmp ( ii->path, ij->path );

}

// Quick sort algorithm to sort the files/directories by inode number - smallest to largest
int inode_qsort ( const void *i, const void *j ) {

    directories *ii = ( directories * ) i;
    directories *ij = ( directories * ) j;
    return ( int ) ( 100.f * ii->inode_num - 100.f * ij->inode_num );
}

/*
 * Function that recursively iterates through the users entered directory and stores all
 * files/directories in an array of structs
 */
directories *get_contents ( const char * user_input, directories *all_paths ) {

    // Boolean variable used in function
    bool check = true;

    // DIR struct pointer
    DIR *current_directory;

    // Opens the users entered directory stream
    current_directory = opendir ( user_input) ;

    // Error checking for the current directory
    if ( !(current_directory) ) {
        // Print error message and exit the program
        printf ( "Error: Cannot open then current directory!\n" );
        exit ( -1 );
    }

    // Whie check is true
    while ( check == true ) {

        // dirent struct pointer
        struct dirent *current;
        const char *directory_content;

        // Initialize current with a pointer to the next directory entry
        current = readdir ( current_directory );

        // Error checking for the current path
        if ( !(current) ) {
            check = false;
            break;
        }

        // Initialize directory_content with the name of the directory
        directory_content = current->d_name;

        // If the directory path is not either '.' or '..'
        if ( !( strcmp ( directory_content, "." ) == 0 || strcmp ( directory_content, ".." ) == 0 ) ) {

            // Declare and initialize count2
            int count2 = 0;

            // Declare and initialize the string_array used to store the current directory path
            char *string_array = malloc (strlen (user_input ) * strlen (directory_content ) + 100 );
            strcpy ( string_array,  user_input );
            strcat ( string_array,  "/" );
            strcat ( string_array,  directory_content );

            // Count the number of '/' in the current path to determine its level
            for ( int i = 0; i < strlen(string_array); i++ ) {
                if ( string_array[i] == '/' ) {
                    count2++;
                }
            }

            // If the count2-home_level is greater than the current max_level, set max_level to be the new greatest level
            if ( count2-home_level > max_level ) {
                max_level = count2-home_level;
            }

            // Initialize the level attribute of the all_paths struct
            all_paths[counter].level = count2-home_level;

            // Initialize the path attribute of the all_paths struct
            all_paths[counter].path = malloc ( sizeof ( char * ) * 1000 );
            strcpy ( all_paths[counter].path, user_input );
            strcat ( all_paths[counter].path,  "/" );
            strcat ( all_paths[counter].path,  directory_content );

            // Initialize the name attribute of the all_paths struct
            all_paths[counter].name = malloc ( sizeof ( char * ) * 1000 );
            strcpy ( all_paths[counter].name, directory_content );

            // Increment counter by 1
            counter = counter + 1;
        }

        // Check if the current path is a directory
        if ( current->d_type & DT_DIR ) {
            
            // If the directory path is not either '.' or '..'
            if ( !( strcmp ( directory_content, "." ) == 0 || strcmp ( directory_content, ".." ) == 0 ) ) {

                char path[PATH_MAX];

                strcpy ( path, user_input );
                strcat ( path,  "/" );
                strcat ( path,  directory_content );
                
                // Recursively call the function with the updated directory
                get_contents ( path, all_paths );

            }
	    }
    }

    // Close the directory
    closedir ( current_directory );

    // Return the all_paths struct
    return all_paths;

}

int main ( int argc, char **argv ) {

    // Malloc space for the structs used in this program
    directories *all_paths = ( directories * ) malloc ( sizeof ( directories ) * 1000 );
    directories *directory_string = malloc ( sizeof ( directories ) * 1000 );
    directories *files_string = malloc ( sizeof ( directories ) * 1000 );
    directories *files_at_level = malloc ( sizeof ( directories ) * 1000 );

    // If the user has not entered 2 parameters in the command line argument
    if ( argc != 3 ) {
        // Print error message and exit the program
        printf ( "Error: Invalid command line arguments! Format: \"./FSreport (-tree/-inode) (full path name of the root directory)\n" );
        exit ( -1 );
    }

    // Check if the user wishes to display the tree directory structure
    if ( strcmp ( argv[1], "-tree" ) == 0 ) {

        // Malloc and initialize path_string with the users entered home directory
        char *path_string = malloc ( sizeof ( char ** ) * strlen ( argv[2] ) + 100 );
        strcpy ( path_string, argv[2] );

        // Count the number of '/' in the current path to determine its level
        for ( int i = 0; i < strlen(path_string); i++ ) {
            if ( path_string[i] == '/' ) {
                home_level++;
            }
        }

        // Call the recursive get_contents function on the users entered home directory
        all_paths = get_contents ( path_string, all_paths );

        qsort ( all_paths, counter, sizeof ( directories ), alphabetical_qsort );

        // Malloc and initialize the path and level attributes for the first index in the directory_string array
        directory_string[0].path = ( char * ) malloc ( strlen ( path_string ) * sizeof ( char * ) + 256 );
        strcpy ( directory_string[0].path, path_string );
        directory_string[0].level = 0;

        // Integer variables used throughout the program
        int i = 0;
        int j = 1;
        int k = 0;
        int current_level = 0;
        int file_total_length = 0;
        int directory_total_length = 0;

        // While the current level is less than the max_level
        while ( current_level < max_level ) {

            // Check if the current path is at the desired level
            if ( all_paths[k].level == current_level+1 ) {

                DIR *current_dir;

                // Opens the users current directory stream
                current_dir = opendir ( path_string );

                // If the current directory is not NULL
                if ( current_dir != NULL ) {

                    // stat struct
                    int my_stat;
                    struct stat temp_stat;

                    // Initialize my_stat with list properties of the file identified by all_paths
                    my_stat = stat ( all_paths[k].path, &temp_stat );

                    // Error checking for the stat function
                    if ( my_stat != 0 ) {
                        // Print error message and exit the program
                        printf ( "Error: Cannot read element in directory! Stat error: %d\n", my_stat );
                        exit ( -1 );
                    }

                    // If the current path is a file
                    if ( S_ISREG ( temp_stat.st_mode ) ) {

                        // Malloc and initialize the path attribute of the files_string
                        files_string[i].path = ( char * ) malloc ( strlen ( all_paths[k].path ) * sizeof ( char * ) + 256 );
                        strcpy ( files_string[i].path, all_paths[k].path );

                        // Initialize the level attribute of the files_string
                        files_string[i].level = all_paths[k].level;

                        // Malloc and initialize the name attribute of the files_string
                        files_string[i].name = ( char * ) malloc ( strlen ( all_paths[k].name ) * sizeof ( char * ) + 256 );
                        strcpy ( files_string[i].name, all_paths[k].name );

                        // Increment variables by 1
                        file_total_length = file_total_length + 1;
                        i = i + 1;

                    }
                    
                    // If the current path is a directory
                    else if ( S_ISDIR ( temp_stat.st_mode ) ) {

                        // Malloc and initialize the path attribute of the directory_string
                        directory_string[j].path = ( char * ) malloc ( strlen ( all_paths[k].path ) * sizeof ( char * ) + 256 );
                        strcpy ( directory_string[j].path, all_paths[k].path );

                        // Initialize the level attribute of the directory_string
                        directory_string[j].level = all_paths[k].level;

                        // Malloc and initialize the name attribute of the directory_string
                        directory_string[j].name = ( char * ) malloc ( strlen ( all_paths[k].name ) * sizeof ( char * ) + 256 );
                        strcpy ( directory_string[j].name, all_paths[k].name );

                        // Increment variables by 1
                        directory_total_length = directory_total_length + 1;
                        j = j + 1;

                    }

                }

                // Close the current directory
                closedir ( current_dir );

            }

            // Increment the k variable by 1
            k = k + 1;

            // Check if all paths have iterated, if so increment the current_level
            if ( k == counter ) {
                k = 0;
                current_level = current_level + 1;
            }

        }

        // Quick sort the files_string and directory_string in alphabetical order according to the path attribute
        qsort ( files_string, file_total_length, sizeof ( directories ), alphabetical_qsort );
        qsort ( directory_string, directory_total_length, sizeof ( directories ), alphabetical_qsort );

        // Initialize passwd struct containing user account information
        struct passwd *pws;

        // Reset k to be 0
        k = 0;

        // Delcare and initialize variables
        int level_iterator = 1;
        int checker = 0;

        // Print report method to stdout
        printf ( "\nFile System Report: Tree Directory Structure\n" );

        // Print the first level header to stdout
        printf ( "\nLevel 1: %s\n", directory_string[0].path );

        // While the level_iterate is less than or equal to the max_level
        while ( level_iterator <= max_level ) {

            // Reset variables
            k = 0;
            j = 0;

            // Check if the current directory is greater than 1
            if ( level_iterator > 1 ) {

                // Check for the directories at the current level and store them into a struct
                while ( k < counter ) {

                    // If the current path has a directory
                    if ( directory_string[k].level+1 == level_iterator && directory_string[k].path != NULL ) {

                        // Malloc and Initialize the path attribute of the files_at_level struct
                        files_at_level[j].path = malloc ( sizeof ( char * ) * 1000 );
                        strcpy ( files_at_level[j].path, directory_string[k].path );

                        // Malloc and Initialize the name attribute of the files_at_level struct
                        files_at_level[j].name = malloc ( sizeof ( char * ) * 1000 );
                        strcpy ( files_at_level[j].name, directory_string[k].name );

                        // Increment variable j
                        j = j + 1;

                    }
                    
                    // Increment varaible k
                    k = k + 1;
                    
                }

            }
            
            // If the directory is at level 1
            else {
                j = 1;
            }

            // Integer variable used for iterating through the number of directories
            int other_iter = 0;

            // while other_iter is less than j, iterate as through all directories at the current level
            while ( other_iter < j ) {

                // Print the level titles (after level 1) to stdout
                if ( level_iterator > 1 ) {
                    printf ( "\nLevel %d: %s\n", level_iterator, files_at_level[other_iter].name );
                }

                // Reset variables
                k = 0;
                checker = 0;

                // Iterate through the directory string and check if there are any directories in the current path
                while ( k < counter ) {
                    if ( directory_string[k].path != NULL && directory_string[k].level-other_iter == level_iterator ) {
                        checker = checker + 1;
                    }
                    k = k + 1;
                }

                // If there are no directories in the current path, do not print the "Directories" header
                if ( checker != 0 ) {
                    printf ( "Directories\n" );
                } else {
                    k = counter;
                }

                // Rest the k variable
                k = 0;

                // While k is less than the counter
                while ( k < counter ) {

                    // Check if the current directory is not NULL and it is at the current level
                    if ( directory_string[k].path != NULL && directory_string[k].level == level_iterator ) {

                        // Declare variables
                        struct stat file_stat;
                        int return_value = 0;
                        int fd = 0;
                        int inode = 0;

                        // Get the user name of the current directory
                        pws = getpwuid ( geteuid() );

                        // Declare group struct
                        struct group* grp;

                        // Error checking for group struct
                        if ( ( grp = getgrgid ( getgid() ) ) == NULL ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot get group!\n" );
                            exit ( -1 );
                        }

                        // Open the current directory for reading only
                        fd = open ( directory_string[k].path, O_RDONLY );

                        // Error checking for the current directory
                        if ( fd < 0 ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot open file!\n" );
                            exit ( -1 );
                        }

                        // Initialize return_value with the value returned from the fstat function
                        return_value = fstat ( fd, &file_stat );
                        if ( return_value < 0 ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot get file status!\n" );
                            exit ( -1 );
                        }

                        // Get inode value
                        inode = file_stat.st_ino;

                        // Open the current file directory for reading
                        FILE* fp = fopen ( directory_string[k].path, "r" );

                        // Error checking for the opened file
                        if ( fp == NULL ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot open file!\n" );
                            exit ( -1 );
                        }

                        // Get the size of the current file in bytes
                        fseek ( fp, 0L, SEEK_END );
                        long int file_size_bytes = ftell ( fp );
                    
                        // Close the file
                        fclose ( fp );

                        // Malloc space for the file_permissions string
                        char *file_permissions = malloc ( sizeof ( char * ) * 10 );

                        // 'd' as the starting letter for directory
                        strcpy ( file_permissions, "d" );

                        // Check the owner file permissions for read, write and execute
                        if ((file_stat.st_mode & S_IRUSR) && (file_stat.st_mode & S_IREAD)) {
                            // If the owner has read file permissions
                            strcat ( file_permissions, "r" );
                        } else {
                            // If the owner does not have read file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IWUSR) && (file_stat.st_mode & S_IWRITE)) {
                            // If the owner has write file permissions
                            strcat ( file_permissions, "w" );
                        } else {
                            // If the owner does not have write file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IXUSR) && (file_stat.st_mode & S_IEXEC)) {
                            // If the owner has execute file permissions
                            strcat ( file_permissions, "x" );
                        } else {
                            // If the owner does not have execute file permissions
                            strcat ( file_permissions, "-" );
                        }
                        // Check the group file permissions for read, write and execute
                        if ((file_stat.st_mode & S_IRGRP) && (file_stat.st_mode & S_IREAD)) {
                            // If the group has read file permissions
                            strcat ( file_permissions, "r" );
                        } else {
                            // If the group does not have read file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IWGRP) && (file_stat.st_mode & S_IWRITE)) {
                            // If the group has write file permissions
                            strcat ( file_permissions, "w" );
                        } else {
                            // If the group does not have write file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IXGRP) && (file_stat.st_mode & S_IEXEC)) {
                            // If the group has execute file permissions
                            strcat ( file_permissions, "x" );
                        } else {
                            // If the group does not have execute file permissions
                            strcat ( file_permissions, "-" );
                        }
                        // Check the world file permissions for read, write and execute
                        if ((file_stat.st_mode & S_IROTH) && (file_stat.st_mode & S_IREAD)) {
                            // If the world has read file permissions
                            strcat ( file_permissions, "r" );
                        } else {
                            // If the world does not have read file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IWOTH) && (file_stat.st_mode & S_IWRITE)) {
                            // If the world has write file permissions
                            strcat ( file_permissions, "w" );
                        } else {
                            // If the world does not have write file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IXOTH) && (file_stat.st_mode & S_IEXEC)) {
                            // If the world has execute file permissions
                            strcat ( file_permissions, "x" );
                        } else {
                            // If the world does not have execute file permissions
                            strcat ( file_permissions, "-" );
                        }

                        // Initialize variables for path checking
                        int g = 0;
                        bool printer = false;

                        // If the level is greater than 1
                        if ( level_iterator > 1 ) {

                            // Used to iterate through every letter in the name attribute of the files_at_level struct
                            while ( g < strlen(files_at_level[other_iter].name) ) {

                                // Check if the previous path name equals the current directories path name
                                if ( directory_string[k].path[strlen(directory_string[k].path)-1-strlen(directory_string[k].name)-strlen(files_at_level[other_iter].name)+g] == files_at_level[other_iter].name[g] ) {
                                    // Set the printer boolean variable to true
                                    printer = true;
                                } else {
                                    // Set the printer boolean variable to false
                                    printer = false;
                                }

                                // Increment g by 1
                                g = g + 1;

                            }

                        }
                        
                        // If the level is the first level
                        else {
                            printer = true;
                        }

                        // If printer is true, print information to stdout
                        if ( printer == true ) {
                            // Print first line of information to stdout
                            printf ( "%s(%s)  %7d %-12s %-7ld %s\n", pws->pw_name, grp->gr_name, inode, file_permissions, file_size_bytes, directory_string[k].name );
                        }

                        // Stat struct used for the access and modification dates and times
                        struct stat filestat;
                        stat ( directory_string[k].path, &filestat );

                        // Malloc space for the strings to store the access and modification dates and times
                        char *access_date = malloc ( sizeof ( char * ) * strlen ( directory_string[k].path ) + 100 );
                        char *modify_date = malloc ( sizeof ( char * ) * strlen ( directory_string[k].path ) + 100 );

                        // Copy the dates and times to the variables using ctime
                        strcpy ( access_date, ctime ( &filestat.st_atime ) );
                        strcpy ( modify_date, ctime ( &filestat.st_mtime ) );

                        // Remove the '/n' from the access_date string (ctime includes the \n character which must be removed)
                        for ( i = 0; i < strlen(access_date); i++ ) {}
                        access_date[i-1] = '\0';

                        // Remove the '/n' from the modify_date string (ctime includes the \n character which must be removed)
                        for ( i = 0; i < strlen(modify_date); i++ ) {}
                        modify_date[i-1] = '\0';

                        // If printer is true, print information to stdout
                        if ( printer == true ) {
                            // Print the second line of information to stdout
                            printf ( "\t%s   %s\n", access_date, modify_date );
                        }

                    }

                    // Increment k by 1
                    k = k + 1;

                }

                // Check if any directories were printed out, if so print an extra \n character for spacing
                if ( checker != 0 ) {
                    printf ( "\n" );
                } else {
                    k = counter;
                }

                // Reset variables
                k = 0;
                checker = 0;

                // Iterate through the files string and check if there are any files in the current path
                while ( k < counter ) {
                    if ( files_string[k].path != NULL && files_string[k].level == level_iterator ) {
                        checker = checker + 1;
                    }
                    k = k + 1;
                }

                // If there are no files in the current path, do not print the "Files" header
                if ( checker != 0 ) {
                    printf ( "Files\n" );
                } else {
                    k = counter;
                }

                // Reset variable k
                k = 0;

                // While k is less than the counter
                while ( k < counter ) {

                    // Check if the current file is not NULL and it is at the current level
                    if ( files_string[k].path != NULL && files_string[k].level == level_iterator ) {

                        // Declare variables
                        struct stat file_stat;
                        int return_value = 0;
                        int fd = 0;
                        int inode = 0;

                        // Get the user name of the current file
                        pws = getpwuid ( geteuid() );

                        // Declare group struct
                        struct group* grp;

                        // Error checking for group struct
                        if ( ( grp = getgrgid ( getgid() ) ) == NULL ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot get group!\n" );
                            exit ( -1 );
                        }

                        // Open the current file for reading only
                        fd = open ( files_string[k].path, O_RDONLY );

                        // Error checking for the current file
                        if ( fd < 0 ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot open file!\n" );
                            exit ( -1 );
                        }

                        // Initialize return_value with the value returned from the fstat function
                        return_value = fstat ( fd, &file_stat );
                        if ( return_value < 0 ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot get file status!\n" );
                            exit ( -1 );
                        }

                        // Get inode value
                        inode = file_stat.st_ino;

                        // Open the current file file for reading
                        FILE* fp = fopen ( files_string[k].path, "r" );

                        // Error checking for the opened file
                        if ( fp == NULL ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot open file!\n" );
                            exit ( -1 );
                        }

                        // Get the size of the current file in bytes
                        fseek ( fp, 0L, SEEK_END );
                        long int file_size_bytes = ftell ( fp );
                    
                        // Close the file
                        fclose ( fp );

                        // Malloc space for the file_permissions string
                        char *file_permissions = malloc ( sizeof ( char * ) * 10 );

                        // '-' as the starting letter for file
                        strcpy ( file_permissions, "-" );

                        // Check the owner file permissions for read, write and execute
                        if ((file_stat.st_mode & S_IRUSR) && (file_stat.st_mode & S_IREAD)) {
                            // If the owner has read file permissions
                            strcat ( file_permissions, "r" );
                        } else {
                            // If the owner does not have read file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IWUSR) && (file_stat.st_mode & S_IWRITE)) {
                            // If the owner has write file permissions
                            strcat ( file_permissions, "w" );
                        } else {
                            // If the owner does not have write file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IXUSR) && (file_stat.st_mode & S_IEXEC)) {
                            // If the owner has execute file permissions
                            strcat ( file_permissions, "x" );
                        } else {
                            // If the owner does not have execute file permissions
                            strcat ( file_permissions, "-" );
                        }
                        // Check the group file permissions for read, write and execute
                        if ((file_stat.st_mode & S_IRGRP) && (file_stat.st_mode & S_IREAD)) {
                            // If the group has read file permissions
                            strcat ( file_permissions, "r" );
                        } else {
                            // If the group does not have read file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IWGRP) && (file_stat.st_mode & S_IWRITE)) {
                            // If the group has write file permissions
                            strcat ( file_permissions, "w" );
                        } else {
                            // If the group does not have write file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IXGRP) && (file_stat.st_mode & S_IEXEC)) {
                            // If the group has execute file permissions
                            strcat ( file_permissions, "x" );
                        } else {
                            // If the group does not have execute file permissions
                            strcat ( file_permissions, "-" );
                        }
                        // Check the world file permissions for read, write and execute
                        if ((file_stat.st_mode & S_IROTH) && (file_stat.st_mode & S_IREAD)) {
                            // If the world has read file permissions
                            strcat ( file_permissions, "r" );
                        } else {
                            // If the world does not have read file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IWOTH) && (file_stat.st_mode & S_IWRITE)) {
                            // If the world has write file permissions
                            strcat ( file_permissions, "w" );
                        } else {
                            // If the world does not have write file permissions
                            strcat ( file_permissions, "-" );
                        }
                        if ((file_stat.st_mode & S_IXOTH) && (file_stat.st_mode & S_IEXEC)) {
                            // If the world has execute file permissions
                            strcat ( file_permissions, "x" );
                        } else {
                            // If the world does not have execute file permissions
                            strcat ( file_permissions, "-" );
                        }

                        // Initialize variables for path checking
                        int g = 0;
                        bool printer = false;

                        // If the level is greater than 1
                        if ( level_iterator > 1 ) {

                            // Used to iterate through every letter in the name attribute of the files_at_level struct
                            while ( g < strlen(files_at_level[other_iter].name) ) {
                                // Check if the previous path name equals the current files path name
                                if ( files_string[k].path[strlen(files_string[k].path)-1-strlen(files_string[k].name)-strlen(files_at_level[other_iter].name)+g] == files_at_level[other_iter].name[g] ) {
                                    // Set the printer boolean variable to true
                                    printer = true;
                                } else {
                                    // Set the printer boolean variable to false
                                    printer = false;
                                }

                                // Increment g by 1
                                g = g + 1;

                            }

                        }
                        
                        // If the level is the first level
                        else {
                            printer = true;
                        }

                        // If printer is true, print information to stdout
                        if ( printer == true ) {
                            // Print first line of information to stdout
                            printf ( "%s(%s)  %7d %-12s %-7ld %s\n", pws->pw_name, grp->gr_name, inode, file_permissions, file_size_bytes, files_string[k].name );
                        }

                        // Stat struct used for the access and modification dates and times
                        struct stat filestat;
                        stat ( files_string[k].path, &filestat );

                        // Malloc space for the strings to store the access and modification dates and times
                        char *access_date = malloc ( sizeof ( char * ) * strlen ( files_string[k].path ) + 100 );
                        char *modify_date = malloc ( sizeof ( char * ) * strlen ( files_string[k].path ) + 100 );

                        // Copy the dates and times to the variables using ctime
                        strcpy ( access_date, ctime ( &filestat.st_atime ) );
                        strcpy ( modify_date, ctime ( &filestat.st_mtime ) );

                        // Remove the '/n' from the access_date string (ctime includes the \n character which must be removed)
                        for ( i = 0; i < strlen(access_date); i++ ) {}
                        access_date[i-1] = '\0';

                        // Remove the '/n' from the modify_date string (ctime includes the \n character which must be removed)
                        for ( i = 0; i < strlen(modify_date); i++ ) {}
                        modify_date[i-1] = '\0';

                        // If printer is true, print information to stdout
                        if ( printer == true ) {
                            // Print the second line of information to stdout
                            printf ( "\t%s   %s\n", access_date, modify_date );
                        }

                    }

                    // Increment k by 1
                    k = k + 1;

                }

                // Increment other_iter by 1
                other_iter++;

            }

            // Increment level_iterator by 1
            level_iterator = level_iterator + 1;

        }

    }
    
    // If the user wishes to display the inode report structure
    else if ( strcmp ( argv[1], "-inode" ) == 0 ) {

        // Malloc and initialize the path_string of the users input
        char *path_string = malloc ( sizeof ( char ** ) * strlen ( argv[2] ) + 100 );
        strcpy ( path_string, argv[2] );

        // Count the number of '/' in the current path to determine its level
        for ( int i = 0; i < strlen(path_string); i++ ) {
            if ( path_string[i] == '/' ) {
                home_level++;
            }
        }

        // Call the recursive function to get all_paths in the given directory
        all_paths = get_contents (path_string, all_paths);

        // Intialize the first index of the directory_string array
        directory_string[0].path = ( char * ) malloc ( strlen ( path_string ) * sizeof ( char * ) + 256 );
        strcpy ( directory_string[0].path, path_string );
        directory_string[0].level = 0;

        // Integer variables used throughout the program
        int i = 0;
        int j = 1;
        int k = 0;
        int current_level = 0;
        int file_total_length = 0;
        int directory_total_length = 0;

        // While the current_level is less than the max_level
        while ( current_level < max_level ) {

            // If the all_paths level is equal to the desired current level
            if ( all_paths[k].level == current_level+1 ) {

                // DIR struct pointer
                DIR *current_dir;

                // Opens the users current directory stream
                current_dir = opendir ( path_string );

                // If the current directory is not NULL
                if ( current_dir != NULL ) {

                    // stat struct
                    int my_stat;
                    struct stat temp_stat;

                    // call stat function to obtain information about the named file
                    my_stat = stat ( all_paths[k].path, &temp_stat );

                    // Error checking for stat
                    if ( my_stat != 0 ) {
                        // Print error message and exit the program
                        printf ( "Error: Cannot read element in directory! Stat error: %d\n", my_stat );
                        exit ( -1 );
                    }

                    // If the path is a flie
                    if ( S_ISREG ( temp_stat.st_mode ) ) {

                        // stat struct
                        struct stat file_stat;
                        int return_value = 0;
                        int fd = 0;
                        int inode = 0;

                        // Open the current file for reading only
                        fd = open ( all_paths[k].path, O_RDONLY );

                        // Error checking for file
                        if ( fd < 0 ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot open file!\n" );
                            exit ( -1 );
                        }

                        // Initialize return_value with the value returned from the fstat function
                        return_value = fstat (fd, &file_stat);
                        if ( return_value < 0 ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot get file status!\n" );
                            exit ( -1 );
                        }

                        // Get inode value
                        inode = file_stat.st_ino;

                        // Initialize structs with the inode_num attribute
                        files_string[i].inode_num = inode;
                        all_paths[k].inode_num = inode;

                        // Malloc and initialize the path attribute of the files_string
                        files_string[i].path = ( char * ) malloc ( strlen ( all_paths[k].path ) * sizeof ( char * ) + 256 );
                        strcpy ( files_string[i].path, all_paths[k].path );

                        // Initialize the level attribute of the files_string
                        files_string[i].level = all_paths[k].level;

                        // Malloc and initialize the name attribute of the files_string
                        files_string[i].name = ( char * ) malloc ( strlen ( all_paths[k].name ) * sizeof ( char * ) + 256 );
                        strcpy ( files_string[i].name, all_paths[k].name );

                        // Increment variables by 1
                        file_total_length = file_total_length + 1;
                        i = i + 1;

                    }
                    
                    // If the path is a directory
                    else if ( S_ISDIR ( temp_stat.st_mode ) ) {

                        // stat struct
                        struct stat file_stat;
                        int return_value = 0;
                        int fd = 0;
                        int inode = 0;

                        // Open the current file for reading only
                        fd = open ( all_paths[k].path, O_RDONLY );

                        // Error checking for file
                        if ( fd < 0 ) {  
                            printf ( "Error: Cannot open file!\n" );
                            exit ( -1 );
                        }

                        // Initialize return_value with the value returned from the fstat function
                        return_value = fstat (fd, &file_stat);
                        if ( return_value < 0 ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot get file status!\n" );
                            exit ( -1 );
                        }

                        // Get inode value
                        inode = file_stat.st_ino;

                        // Initialize structs with the inode_num attribute
                        directory_string[j].inode_num = inode;
                        all_paths[k].inode_num = inode;

                        // Malloc and initialize the path attribute of the directory_string
                        directory_string[j].path = ( char * ) malloc ( strlen ( all_paths[k].path ) * sizeof ( char * ) + 256 );
                        strcpy ( directory_string[j].path, all_paths[k].path );

                        // Initialize the level attribute of the directory_string
                        directory_string[j].level = all_paths[k].level;

                        // Malloc and initialize the name attribute of the directory_string
                        directory_string[j].name = ( char * ) malloc ( strlen ( all_paths[k].name ) * sizeof ( char * ) + 256 );
                        strcpy ( directory_string[j].name, all_paths[k].name );

                        // Increment variables
                        directory_total_length = directory_total_length + 1;
                        j = j + 1;

                    }

                }

                // Close the directory
                closedir ( current_dir );

            }

            // Increment the k variable
            k = k + 1;

            // Check if all paths have iterated, if so increment the current_level
            if ( k == counter ) {
                k = 0;
                current_level = current_level + 1;
            }

        }

        // Sort the paths by inode - smallest to largest
        qsort ( all_paths, file_total_length+directory_total_length, sizeof ( directories ), inode_qsort );

        // Reset variable k
        k = 0;
        
        // Integer variables used throughout program
        int level_iterator = 1;
        int other_iter = 0;

        // Print the report type to stdout
        printf ( "\nFile System Report: Inodes\n" );

        // Pritn the first level header to stdout
        printf ( "\nLevel 1 Inodes: %s\n", directory_string[0].path );

        // While the level_iterator is less than or equal to the max_level
        while ( level_iterator <= max_level ) {

            // Reset variables
            k = 0;
            j = 0;
            other_iter = 0;

            // Check if the current directory is greater than 1
            if ( level_iterator > 1 ) {

                // Check for the directories at the current level and store them into a struct
                while ( k < counter ) {

                    // If the current path has a directory
                    if ( all_paths[k].level+1 == level_iterator && all_paths[k].path != NULL ) {

                        // Malloc and Initialize the path attribute of the files_at_level struct
                        files_at_level[j].path = malloc ( sizeof ( char * ) * 1000 );
                        strcpy ( files_at_level[j].path, all_paths[k].path );

                        // Malloc and Initialize the name attribute of the files_at_level struct
                        files_at_level[j].name = malloc ( sizeof ( char * ) * 1000 );
                        strcpy ( files_at_level[j].name, all_paths[k].name );

                        // Increment variable j
                        j = j + 1;

                    }
                    
                    // Increment varaible k
                    k = k + 1;
                    
                }

            }
            
            // If the directory is at level 1
            else {
                j = 1;
            }

            // while other_iter is less than j, iterate as through all directories at the current level
            while ( other_iter < j ) {

                // Print the level header to stdout after the first level
                if ( level_iterator > 1 ) {

                    // stat struct
                    int my_stat;
                    struct stat temp_stat;

                    // call stat function to obtain information about the named file
                    my_stat = stat ( files_at_level[other_iter].path, &temp_stat );

                    // Error checking for stat
                    if ( my_stat != 0 ) {
                        // Print error message and exit the program
                        printf ( "Error: Cannot read element in directory! Stat error: %d\n", my_stat );
                        exit ( -1 );
                    }

                    if ( S_ISDIR ( temp_stat.st_mode ) ) {
                        printf ( "\nLevel %d Inodes: %s\n", level_iterator, files_at_level[other_iter].name );
                        // Reset the k variable
                        k = 0;
                    } else {
                        // Reset the k variable
                        k = counter;
                    }
                }

                // While k is less than the counter
                while ( k < counter ) {

                    // Check if the path is not NULL and it is at the desired level
                    if ( all_paths[k].path != NULL && all_paths[k].level == level_iterator ) {

                        // stat struct
                        struct stat file_stat;
                        int return_value = 0;
                        int fd = 0;

                        // Open the current file for reading only
                        fd = open ( all_paths[k].path, O_RDONLY );

                        // Error checking for file
                        if ( fd < 0 ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot open file!\n" );
                            exit ( -1 );
                        }

                        // Initialize return_value with the value returned from the fstat function
                        return_value = fstat (fd, &file_stat);
                        if ( return_value < 0 ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot get file status!\n" );
                            exit ( -1 );
                        }

                        // Open the current file file for reading
                        FILE* fp = fopen ( all_paths[k].path, "r" );

                        // Error checking for the opened file
                        if ( fp == NULL ) {
                            // Print error message and exit the program
                            printf ( "Error: Cannot open file!\n" );
                            exit ( -1 );
                        }

                        // Get the size of the current file in bytes
                        fseek ( fp, 0L, SEEK_END );
                        long int file_size_bytes = ftell ( fp );
                    
                        // Close the file
                        fclose ( fp );

                        // Calculate the number of blocks allocated for the path
                        int blocks_allocated = file_size_bytes / 512;

                        // Check if the file size is not 0
                        if ( file_size_bytes != 0 ) {
                            blocks_allocated = blocks_allocated + 1;
                        }

                        // Initialize variables for path checking
                        int g = 0;
                        bool printer = false;

                        // If the level is greater than 1
                        if ( level_iterator > 1 ) {

                            // Used to iterate through every letter in the name attribute of the files_at_level struct
                            while ( g < strlen(files_at_level[other_iter].name) ) {
                                // Check if the previous path name equals the current paths path name
                                if ( all_paths[k].path[strlen(all_paths[k].path)-1-strlen(all_paths[k].name)-strlen(files_at_level[other_iter].name)+g] == files_at_level[other_iter].name[g] ) {
                                    // Set the printer boolean variable to true
                                    printer = true;
                                } else {
                                    // Set the printer boolean variable to false
                                    printer = false;
                                }

                                // Increment g by 1
                                g = g + 1;

                            }

                        }
                        
                        // If the level is the first level
                        else {
                            printer = true;
                        }

                        // If printer is true, print information to stdout
                        if ( printer == true ) {
                            // Print the information to stdout
                            printf ( "%d:    %-11ld%-8d%-6ld%s\n", all_paths[k].inode_num, file_size_bytes, blocks_allocated, file_stat.st_blocks, all_paths[k].name );
                        }

                    }

                    // Increment the k variable by 1
                    k = k + 1;

                }

                // Increment the other_iter variable by 1
                other_iter++;

            }

            // Increment the level_iterator variable by 1
            level_iterator = level_iterator + 1;

        }

    }
    
    // If the user has given an invalid command line argument for argv[1]
    else {
        // Print error message and exit the program
        printf ( "Error: Invalid report type! Format: \"./FSreport (-tree/-inode) (full path name of the root directory)\n" );
        exit ( -1 );
    }

    // Print new line character
    printf ( "\n" );

    // End of program
    return ( 0 );

}