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

/* -- Reference code -- */
/*asmlinkage long (*ref_sys_cs3013_syscall1)(void);
asmlinkage long new_sys_cs3013_syscall1(void)
{
    printk(KERN_INFO "\"’Hello world?!’ More like ’Goodbye, world!’ EXTERMINATE! TEST\" -- Dalek");
    return 0;
}*/

unsigned long **sys_call_table;
struct kuid_t *reg_user;
reg_user = current_uid();

/* -- Our own storage values for sys_calls -- */
asmlinkage int (*real_open)(const char __user *filename, int flags, int mode);
asmlinkage int (*real_read)(unsigned int fd, char __user *buf, size_t count);
asmlinkage int (*real_close)(unsigned int fd);

/* -- Our own functions for intercepted variables -- */
asmlinkage int new_sys_open(const char __user *filename, int flags, int mode) // Intercept open
{
    if (reg_user->val >= 1000)
    {
    printk(KERN_INFO "dalek kernel: User is opening file (%s, %X, %X)", filename, flags, mode);
    }
    return real_open(filename, flags, mode);
}
asmlinkage int new_sys_read(unsigned int fd, char __user *buf, size_t count) //Intercept read
{
    if (reg_user->val >= 1000)
    {
        printk(KERN_INFO "interceptor: read(%s)", buf);
    }
    return real_read(fd, buf, count);
}
asmlinkage int new_sys_close(unsigned int fd) //Intercept write
{
    if (reg_user->val >= 1000)
    {
        printk(KERN_INFO "dalek kernel: User is closing file descriptor");
    }
    return real_close(fd);
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
    real_open = (void *)sys_call_table[__NR_open];
    real_read = (void *)sys_call_table[__NR_read];
    real_close = (void *)sys_call_table[__NR_close];
    /* Replace the existing system calls */
    disable_page_protection();
    sys_call_table[__NR_open] = (unsigned long *)new_sys_open;
    sys_call_table[__NR_read] = (unsigned long *)new_sys_read;
    sys_call_table[__NR_close] = (unsigned long *)new_sys_close;
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
    sys_call_table[__NR_open] = (unsigned long *)real_open;
    sys_call_table[__NR_read] = (unsigned long *)real_read;
    sys_call_table[__NR_close] = (unsigned long *)real_close;
    enable_page_protection();
    printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);


