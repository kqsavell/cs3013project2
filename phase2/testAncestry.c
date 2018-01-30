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

long testCall2(unsigned short *target_pid, struct ancestry *response) 
{
    return (long) syscall(__NR_cs3013_syscall2, target_pid, response);
}

int main(int argc, char *argv[]) 
{
    unsigned short pid = 1; // Default process is init
	
    struct ancestry *new_ancestry;
    new_ancestry = malloc(sizeof(struct ancestry));
    for(int i = 0; i < 10; i++)
    {
        new_ancestry->ancestors[i] = -1;
    }
    for(int i = 0; i < 100; i++)
    {
        new_ancestry->siblings[i] = -1;
	new_ancestry->children[i] = -1;
    }

    //Read user input
    if (argc > 1) 
    {
        sscanf(argv[1], "%hu", &pid); //Copies input to pid
    }

    printf("The return value of the system call is:\n");
    printf("\tcs3013_syscall2: %ld\n", testCall2(&pid, new_ancestry));
    for(int i = 0; i < 10; i++)
    {
	if(new_ancestry->ancestors[i] == -1)
	    break;
	printf("%d is an ancestor of the target process %d\n", new_ancestry->ancestors[i], (int)pid);
    }
    for(int i = 0; i < 100; i++)
    {
	if(new_ancestry->siblings[i] == -1)
	    break;
	printf("%d is a sibling of the target process %d\n", new_ancestry->siblings[i], (int)pid);
    }
    for(int i = 0; i < 100; i++)
    {
	if(new_ancestry->children[i] == -1)
	    break;
	printf("%d is a child of the target process %d\n", new_ancestry->children[i], (int)pid);
    }
}
