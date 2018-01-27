//Kyle Savell & Antony Qin
//OS Project 2

//Undefine and redefine in Kernel space
#undef __KERNEL__
#undef MODULE

#define __KERNEL__
#define MODULE

//Part 1:

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <asm/current.h>
#include <linux/uaccess.h>
#include <linux/pid.h>

/* -- Reference code -- */
/*asmlinkage long (*ref_sys_cs3013_syscall1)(void);
asmlinkage long new_sys_cs3013_syscall1(void)
{
    printk(KERN_INFO "\"’Hello world?!’ More like ’Goodbye, world!’ EXTERMINATE! TEST\" -- Dalek");
    return 0;
}*/

unsigned long **sys_call_table;

/* -- Our own storage values for sys_calls -- */
struct ancestry
{
pid_t ancestors[10];
pid_t siblings[100];
pid_t children[100];
};

asmlinkage long (*ref_sys_cs3013_syscall2)(void);
unsigned long kinput;
unsigned long koutput;
unsigned long source = 1;
unsigned long src_length = 2;

asmlinkage long new_sys_cs3013_syscall2(unsigned short *target_pid, struct ancestry *response)
{

    struct pid *pid_struct;
    struct task_struct *task;
    struct list_head *head;
    int i = 0; //Counter for parents
    int j = 0; //Counter for siblings and children
    printk(KERN_INFO "Ancestry function was entered\n");

    // Test for valid pid
    if (copy_from_user(&kinput, target_pid, sizeof(target_pid)) != 0) {
        printk(KERN_INFO "Trouble with copy_from_user\n");
        return EFAULT;
    }

    // Get task struct from pid
    pid_struct = find_get_pid((int)*target_pid); //Returns NULL pointer if pid is invalid
    task = pid_task(pid_struct, PIDTYPE_PID);

    // Find task's children
    printk(KERN_INFO "Analyzing target process %d", (int)*target_pid);
    list_for_each(head, &task->children)
    {
        if (list_entry(head, struct task_struct, sibling)->pid != (int)*target_pid)
        {
            if (j <= 99)
            {
                response->children[j] = list_entry(head, struct task_struct, sibling)->pid;
            }
            printk(KERN_INFO "%d is a child of target process %d\n", response->children[j], (int)*target_pid);
            j++;
        }
    }

    // Find task's ancestors and siblings
    j = 0;
    for (; task != &init_task; task = task->parent)
    {
        if (task->pid != (int)*target_pid)
        {
            if (i <= 9)
            {
                response->ancestors[i] = task->pid;
            }
            printk(KERN_INFO "%d is an ancestor of target process %d\n", response->ancestors[i], (int)*target_pid);
            i++;
            if (i == 1)
            {
                list_for_each(head, &task->children)
                {
                    if (list_entry(head, struct task_struct, sibling)->pid != (int)*target_pid)
                    {
                        if (j <= 99)
                        {
                            response->siblings[j] = list_entry(head, struct task_struct, sibling)->pid;
                        }
                        printk(KERN_INFO "%d is a sibling of target process %d\n", response->siblings[j], (int)*target_pid);
                        j++;
                    }
                }
            }
        }
    }

    // Test for valid return values
    if (copy_to_user(response, response, sizeof(response)) != 0) {
        printk(KERN_INFO "Trouble with copy_to_user\n");
        return EFAULT;
    }
    printk(KERN_INFO "Ancestry function was exited\n");
    return 0;
}

static unsigned long **find_sys_call_table(void)
{
    unsigned long int offset = PAGE_OFFSET;
    unsigned long **sct;
    while (offset < ULLONG_MAX)
    {
        sct = (unsigned long **)offset;
        if (sct[__NR_close] == (unsigned long *) sys_close)
        {
            printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
                   (unsigned long) sct);
            return sct;
        }
        offset += sizeof(void *);
    }
    return NULL;
}

static void disable_page_protection(void)
{
    /*
    Control Register 0 (cr0) governs how the CPU operates.
    Bit #16, if set, prevents the CPU from writing to memory marked as
    read only. Well, our system call table meets that description.
    But, we can simply turn off this bit in cr0 to allow us to make
    changes. We read in the current value of the register (32 or 64
    bits wide), and AND that with a value where all bits are 0 except
    the 16th bit (using a negation operation), causing the write_cr0
    value to have the 16th bit cleared (with all other bits staying
    the same. We will thus be able to write to the protected memory.
    It’s good to be the kernel!
    */
    write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void)
{
    /*
    See the above description for cr0. Here, we use an OR to set the
    16th bit to re-enable write protection on the CPU.
    */
    write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void)
{
    /* Find the system call table */
    if(!(sys_call_table = find_sys_call_table()))
    {
        /* Well, that didn’t work.
        Cancel the module loading step. */
        return -1;
    }
    /* Store a copy of all the existing functions */
    ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];
    /* Replace the existing system calls */
    disable_page_protection();
    sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
    enable_page_protection();
    /* And indicate the load was successful */
    printk(KERN_INFO "Loaded interceptor!");
    return 0;
}

static void __exit interceptor_end(void)
{
    /* If we don’t know what the syscall table is, don’t bother. */
    if(!sys_call_table)
        return;
    /* Revert all system calls to what they were before we began. */
    disable_page_protection();
    sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
    enable_page_protection();
    printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
