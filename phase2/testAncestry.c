//Kyle Savell

#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define __NR_cs3013_syscall1 333
#define __NR_cs3013_syscall2 334
#define __NR_cs3013_syscall3 335

struct ancestry
{
    pid_t ancestors[10];
    pid_t siblings[100];
    pid_t children[100];
};

long testCall2(unsigned short *target_pid, struct ancestry *response) {
    return (long) syscall(__NR_cs3013_syscall2, target_pid, response);
}

int main(int argc, char *argv[]) {
    unsigned short pid = 1; // Default process is init
    struct ancestry *new_ancestry;
    new_ancestry = malloc(sizeof(struct ancestry));

    //Read user input
    if (argc > 1) {
        sscanf(argv[1], "%hu", &pid); //Copies input to pid
    }

    printf("The return values of the system calls are:\n");
    printf("\tcs3013_syscall2: %ld\n", testCall2(&pid, new_ancestry));
}
