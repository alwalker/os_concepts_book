/**
 * hello.c
 *
 * Kernel module that communicates with /proc file system.
 * 
 * The makefile must be modified to compile this program.
 * Change the line "simple.o" to "hello.o"
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <asm/param.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128

#define JIFFIE_PROC_NAME "jiffies"
#define TIME_PROC_NAME "seconds"
#define MESSAGE "Hello World\n"

/**
 * Function prototypes
 */
ssize_t proc_jiffie_read(struct file *file, char *buf, size_t count, loff_t *pos);
ssize_t proc_time_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct proc_ops proc_jiffie_ops = {
        .proc_read = proc_jiffie_read,
};

static struct proc_ops proc_time_ops = {
        .proc_read = proc_time_read,
};

unsigned long jiffies_at_load = 0;


/* This function is called when the module is loaded. */
int proc_init(void)
{
        jiffies_at_load = jiffies;
        // creates the /proc/hello entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(JIFFIE_PROC_NAME, 0, NULL, &proc_jiffie_ops);
        proc_create(TIME_PROC_NAME, 0, NULL, &proc_time_ops);

        printk(KERN_INFO "Created new procs");

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {

        // removes the /proc/hello entry
        remove_proc_entry(JIFFIE_PROC_NAME, NULL);
        remove_proc_entry(TIME_PROC_NAME, NULL);

        printk( KERN_INFO "Removed new procs");
}

/**
 * This function is called each time the /proc/hello is read.
 * 
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
ssize_t proc_jiffie_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;

        rv = sprintf(buffer, "Jiffies: %lu\n", jiffies);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}

ssize_t proc_time_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;

        unsigned long jiffies_since_load = jiffies - jiffies_at_load;
        unsigned long seconds_since_load = jiffies_since_load / HZ;

        rv = sprintf(buffer, "Time since boot: %lu\n", seconds_since_load);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Module");
MODULE_AUTHOR("SGG");

