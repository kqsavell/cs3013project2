Antony Qin & Kyle Savell
OS Project 2

Summary:
A collection of kernel-space and user-space programs to test system call interceptions. The intercepted system calls are either modified to provide additional functionality (phase 1) or are overwritten entirely to create new OS functions (phase 2).

Phase 1:
The primary program for phase 1 is the p2.c file, with the LKM being p2.ko. This module intercepts all open, close and read system calls. When a user calls the open or close command, a message is printed to the kernel listing the file descriptor. When a user calls the read command, if the word "virus" is read a warning will be printed to the kernel.
Use the "make" command to create the files for the module. "make clean" will delete and then recreate the files for the module.
Inserting the module - "sudo insmod p2.ko"
Removing the module - "sudo rmmod p2.ko"

In the phase1 directory there is also the syscall1 sub-directory. This contains the test module for intercepting syscall1 calls. This directory contains it's own makefile, and making, inserting and removing the module are the same as for the main module of phase 1.

Phase 2:
This phase has two main programs; the kernel-space program is procAncestry.c with the LKM being procAncestry.ko, and the user-space program to test this module is testAncestry.c. This program will print out a given processes information to the kernel, including the processes ancestors, siblings and children.
Use the "make" command to create the files for the module and for the testAncestry executable. "make clean" will delete and then recreate the files for the module and the executable.
Inserting the module - "sudo insmod procAncestry.ko"
Removing the module - "sudo rmmod procAncestry.ko"
Testing the module - "./testAncestry <pid>", Ex. "./testAncestry 3627". The default value for pid is 1.
