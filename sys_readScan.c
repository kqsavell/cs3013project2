#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

unsigned long **sys_call_table;
asmlinkage long (*ref_sys_cs3013_syscall1)(void);

asmlinkage long new_sys_cs3013_syscall1(void)
{
    printk(KERN_INFO "\"’Hello world?!’ More like ’Goodbye, world!’ EXTERMINATE!\" -- Dalek");
    return 0;
}

asmlinkage int (*real_open)(const char __user *filename, int flags, int mode);
asmlinkage int (*real_read)(unsigned int fd, char __user *buf, size_t count);
asmlinkage int (*real_close)(unsigned int fd);

/* -- Our own functions for intercepted variables -- */
asmlinkage int new_sys_open(const char __user *filename, int flags, int mode) // Intercept open
{
    int file;
    char buf[1];

    mm_segment_t old_fs = get_fs();
    set_fs(KERNEL_DS);

    file = real_open(filename, O_RDONLY, 0); // O_RDONLY means reading only
    if(file >= 0)
    {
        int counter = 0;
        int yesVirus = -1;

        while(real_read(file, buf, 1) == 1)
        {
            if(counter == 4 && (buf[0] == 's' || buf[0] == 'S'))
            {
                yesVirus = 0;
                break;
            }
            else if(counter == 4)
                counter = 0;

            if(counter == 3 && (buf[0] == 'u' || buf[0] == 'U'))
                counter++;
            else if(counter == 3)
                counter = 0;

            if(counter == 2 && (buf[0] == 'r' || buf[0] == 'R'))
                counter++;
            else if(counter == 2)
                counter = 0;

            if(counter == 1 && (buf[0] == 'i' || buf[0] == 'I'))
                counter++;
            else if(counter == 1)
                counter = 0;

            if(counter == 0 && (buf[0] == 'v' || buf[0] == 'V'))
            {
                counter++;
            }
        }

        real_close(file);

        if(yesVirus == 0)
            printk(KERN_INFO "ON NOES: ENCOUNTERED A SCARY VIRUS\n");
    }
    set_fs(old_fs);

    return real_open(filename, flags, mode);
}
asmlinkage int new_sys_read(unsigned int fd, char __user *buf, size_t count) //Intercept read
{
    return real_read(fd, buf, count);
}
asmlinkage int new_sys_close(unsigned int fd) //Intercept write
{
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
