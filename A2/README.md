Name: Carson Mifsud
Date: 2021-02-19
Description: This program simulates CPU scheduling algorithms that will complete the execution of a group of
multi-threaded processes in an Operating System that understand threads. The program can run the FCFS and
Round Robin Scheduling algorithms on a given input file. The program calculates several numbers based on the
input flags, entered by the user.

Compile:
    make all
Execute:
    ./simcpu [-d] [-v] [-r quantum] < input_file
Clean:
    make clean

For program execution:
    - Any flags must not be passed with '[]' around them
    - For the Round Robin algorithm, the quantum number must be placed directly to the right of the -r flag, and seperated by a space ( Example: '-r 50' )
    - Multiple combinations of flags may be entered at once as long as -r flag condition above is true, if not the program will print an error message to the user
    - Flags must be entered after the './simcpu' and before the '< input_file'
    - For verbose ouput the summary information about each thread ( detailed information ) is printed after a thread terminated

Assumptions:
    - If two threads have the same arrival time, they are then ordered by pid
    - 50 is the maximum number of threads a process may have
    - The input_file must be redirected to stdin using the '<' symbol ( Example: ./simcpu < input_file )

Test Case:
    - The user will compile the program
    - The user may execute the program using any combination of flags as long as the above conditions are met
    - The program will calculate the appropriate output based on the given schedulling algorithm and print out the corresponding numbers to stdout based on the files given

NOTE: The Professors test files provided via Course Link fully execute without errors. This includes the FCFS file and the Round Robin file. my program output will
      produce identical output to that given in the example PDF.