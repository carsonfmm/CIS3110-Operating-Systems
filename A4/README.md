Name: Carson Mifsud
Date: 2021-04-02
Description: This program generates reports to stdout about the file system that displays
information about the files and directories that appear in a directory that is provided to the
program via command line arguments. This program can display information in both the tree
directory structure and the inode structure. This program can display multiple sub-levels of a
given directory structure up to the 'PATH_MAX' macro defined by limits.h. The program contains
error statements, which print to stdout and exit the program if any fatal errors occur.

Tree directory structure format ( -tree ):
    - Sorted alphabetically by file/directory name
    - Line 1: owner name(group name), inode number, permissions, size in bytes, file/directory name
    - Line 2: Date of last access, Date of last modification

Inodes Format ( -inode ):
    - Sorted by inode number - smallest to largest
    - Line 1: Inode number: size (in bytes), number of 512-byte blocks allocated to the file,
      file size/512, file/directory name

NOTE: See in-line program comments for further description of programs and algorithm processes

Compile:
    make
Execute:
    ./FSreport (-tree/-inode) (full path name of the root directory)
Clean:
    make clean

Assumptions:
    - Maximum user input of 500 characters
    - Maximum 100 files and 100 directories per level

Test Case:
    - The user will compile the program
    - The user will run the program with the first command line being the type of report structure,
      and the second being the specified home directory
    - The directory data will be printed to stdout including information of all levels and sub-levels

NOTE: The Professors test directory provided in the Assignment 4 description fully executes and prints
accurate results to stdout without errors.
