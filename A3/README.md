Name: Carson Mifsud
Date: 2021-03-19

Part 1 Description: This program is a solution to the dining philosopher's problem. Pseudo code was used for this
algorithm which can be found on page 295 of the "Operating Systems Concepts, Enhanced eText, 10th Edition"
course textbook. Global variables were implemented to keep track of the philosophers. A state array was implemented
to store the state of the current philosopher. A global counter has been implemented to track the amount of
times each philosopher has eaten a meal. Once, all philosophers have eaten the correct amount of meals, the program
terminates. Lastly, this program implements pthreads and semaphores which were implemented via the pseudo code from
the course textbook. This program provides some error checking for the command line arguments.

Part 2 Description: This program emulates a simulator that can take varying sizes or processes, load them into a memory
sumulator of 128 MB, and swap processes out as needed to create holes for processes to execute. The swapping process
is dependent on the algorithm being used. These algorithms are best fit, worst fit, next fit, and first fit. The program
accepts two command line arguments, the first being the name of the file which contains processes and the second being
either 'best', 'first', 'worst', or 'next' which indicates the algorithm to be used on the given file of processes. This
program was completed using several arrays and structs. I allocated one struct array to store the process information
in the queue, and another struct array to store the process information in memory. The memory itself was implemented using
an integer array which is 128 indices long. The array possesses '-1' and '1' characters, where '-1' indicates a hole in
memory at a specific MB and '1' indicates a process in memory at a specific MB. This program provides some error checking
for the command line arguments. The output results are rounded to the identical precision points as given in the assignment
3 description. ( SIDE NOTE: If I were to do this assignment for a second time, I would choose to use heaps as my method
of storing process information. This method would be more efficient for the requirements that need to be met for this
assignment, including the process queue and memory storage. )

NOTE: See in-line program comments for further description of programs and algorithm processes

Compile:
    make all
Execute:
    ./dine (The number of philosophers) (The number of times each philosopher should eat)
    ./holes (test file) (Algorithm to be used: "first", "best", "worst", "next")
Clean:
    make clean

Assumptions:
    - The input file is of the same format given in the assignment 3 description
    - The input file must have a maximum length of 26, with each process assigned a unique letter from the alphabet
    - For part 1, the user must enter integers for the command line arguments or else the program will crash

Test Case for Part 1:
    - The user will compile the program
    - The users terminal will iterate through the philosophers thinking and eating, with a random sleep time between 1 and 3
    - The program ends when all meals have been eaten

Test Case for Part 2:
    - The user will compile the program
    - The program will run the specifc algorithm based on the users input
    - The program will output the state of the memory after each process has been inserted into memory, as well as a final summary after the process queue is empty

NOTE: The Professors test file provided via Course Link fully executes without errors.