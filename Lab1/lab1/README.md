## UID: 405999589

## Pipe Up

Implement the shell pipe operator in a C program.

## Building

I started off by looping through the entire program, creating new file descriptors and a child process. Depending on if the fork is successful I link the standard output to the parent or another child (the next command in line). At the end I run the final command and abort the program.

## Running

Running ./pipe ls cat wc. It gives the result "6 6 51". ls lists out all the files: "Makefile  pipe  pipe.c  pipe.o  README.md  test_lab1.py", cat then prints them all out one by one in a new line. wc then counts the number of words and since there are 6 words, one per each line we get "6 6" and since there are 51 characters overall we get "6 6 51". 

## Cleaning up

Use make clean to clean up all the binary files. The Makefile has a command "clean: rm -f ${OBJS} pipe" that removes all OBJS, defined as pipe.o (binary files), from the directory. As a result it is the most efficient way to remove all binary files.
