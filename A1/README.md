Name: Carson Mifsud
Date: 2021-02-05
Description: Simple UNIX shell program. This program handles basic UNIX shell commands in the foreground and background.
Handles one level of pipe as well as redirected output to a file and redirected input from a file. Uses the limited environmental
variables including PATH, HISTFILE and HOME. Stores users history in a history file and can handle changing directories.

Compile:
    make all
Execute:
    ./myShell
Clean:
    make clean

Limitations:
    - The user can enter a maximum of 10 parameters into the shell
    - The user input has a maximum length of 1000
    - The shell can only handle a maximum of 50 background processes
    - The program can store 100 commands in the history file

Assumptions:
    - There must be a space seperating parameters and symbols such as '<', '>' and '|'.
    - Export changes the variables $HOME, $PATH, and $HISTFILE

Test Case:
    - The user is presented with a prompt of the current directory
    - The user can enter 'exit' at any time to end the myShell program
    - The user can enter basic UNIX shell commands such as ls and commands with parameters including ls -l
    - The user can enter a command using the '&' symbol to pass the command as a background process
    - If a background process is executed the user will be presented with the processes pid and process number
    - When the background process completes, the user is prompted with the process number followed by a 'Done' message and  the command used.
    - The user may kill any processes by typing kill followed by the signal and pid (Ex. 'kill -9 29945')
    - If there are any outstanding background processes once the user exits the shell, they will immediately terminate just before exiting the myShell program
    - The user may redirect output to a file using the '>' symbol
    - The user may redirect input from a file using the '<' symbol
    - The user may implement one level of piping using the '|' symbol
    - The three environmental variables PATH, HOME and HISTFILE may be exported from the '.CIS3100_profile' file
    - The variables may also be changed in the middle of a session using the identical export format
    - The user may view their history during a session by using the 'history' command
    - Once the session has ended the users history will be printed to the file '.CIS3110_history'
    - If either of these files are not provided in the users current working directory, they will be created
    - The use may also change directories throughout the seesion using various 'cd' commands

NOTE: The Professors test harness provided via Course Link fully executes without errors. The sample input will produce identical output to that of the test harness.
      This test harness, the sample input provided via the assignment description and my own inputs were used to test this program.

The online education environment possesses some challenges and one being the possible miscommunication of assignments, labs and expectations. I wanted to note that whilst working on this
assignment, various instructiosns were not clear to me. Mainly focusing on the export command and '.CIS3110_profile' file functionality. I attended multiple labs,
arrived for lectures early and asked questions in the course discord. During lab hours, when I asked about the export funcationality of the assignment, the teaching assistant was confused
of the instructions as well as myself. Although I am confident in my program as it passes all test cases provided by you. I wanted to inform the marker of this program what I had
understood from the assignment instructions. I understood these environamntal varibales may be altered by the user during the program or at the beginning of the program. I read
in the '.CIS3110_file and store these variables with the assigned values. If a variable is nested within a path. the variable name is replaced with its value when it is printed to the user.