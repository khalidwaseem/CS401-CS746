#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/sched.h>

MODULE_AUTHOR("Khalid Waseem");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel Programming Exercise 3");

#define PRINTPID_NO 332
unsigned long **sys_call_table;
asmlinkage int (*orig_sys_printpid)(void);

// New system call that replaces original sys_printpid system call
asmlinkage int new_sys_printpid(void){   
    	printk(KERN_INFO "System call printpid over written. Will print priority now.\n");
	printk(KERN_INFO "Process: %s\tPriority: %d\n",current->comm, current->normal_prio);
	return 0;
}

// This function returns the pointer of the system call table
static unsigned long **aquire_sys_call_table(void){
    	unsigned long int offset = PAGE_OFFSET;
    	unsigned long **sct;

    	while (offset < ULLONG_MAX) {
        	sct = (unsigned long **)offset;

        	if (sct[__NR_close] == (unsigned long *) sys_close) 
            		return sct;

        	offset += sizeof(void *);
    	}
    	return NULL;
}


// This function is executed when the module is loaded.
// 1. It gets the pointer of system call table
// 2. It disables page protection
// 3. It modifies the entry of the printpid system call
// 4. It enables page protection
static int __init interceptor_start(void){
	if(!(sys_call_table = aquire_sys_call_table())){
		printk(KERN_INFO "Couldn't accuire system call table.\n");
             	return -1;
    	}

    	//disable_page_protection
	write_cr0(read_cr0() & (~ 0x10000));

    	orig_sys_printpid = (void *)sys_call_table[PRINTPID_NO];
    	sys_call_table[PRINTPID_NO] = (unsigned long *)new_sys_printpid;

    	//enable_page_protection
	write_cr0(read_cr0() | 0x10000);

	printk(KERN_INFO "Module Loaded.\n");

    	return 0;
}

// This method is executed when the module is removed/unloaded
// 1. It disables page protection
// 2. It restores the entry of the printpid system call
// 3. It enables page protection
static void __exit interceptor_end(void){
	if(!sys_call_table)
        	return;
    	
	//disable_page_protection
	write_cr0(read_cr0() & (~ 0x10000));

    	sys_call_table[PRINTPID_NO] = (unsigned long *)orig_sys_printpid;
    	
	//enable_page_protection
	write_cr0(read_cr0() | 0x10000);
	
	printk(KERN_INFO "Module removed.\n");
}

module_init(interceptor_start);
module_exit(interceptor_end);
