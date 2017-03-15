#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include<linux/moduleparam.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/pgtable.h>

MODULE_AUTHOR("Khalid Waseem");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel Programming Exercise 4");

static int pid = 0;
module_param(pid,int,0);

unsigned long calculateRSS(struct vm_area_struct *vma){
	unsigned long start = vma->vm_start;
        unsigned long end = vma->vm_end;
	unsigned long pgcount = 0;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	while(start < end){
		pgd = pgd_offset(vma->vm_mm, start);
		if(pgd_none(*pgd) || pgd_bad(*pgd))
			return 0;
		pud = pud_offset(pgd, start);
		if(pud_none(*pud) || pud_bad(*pud))
                        return 0;
		pmd = pmd_offset(pud, start);
		if(pmd_none(*pmd) || pmd_bad(*pmd))
                        return 0;
		pte = pte_offset_map(pmd, start);
		if(!pte)
                        return 0;

		if(pte_present(*pte))
			pgcount++;
		start += PAGE_SIZE;				
	}
	//return pgcount*4;
	return pgcount;
}

void printvma(struct vm_area_struct *vma){
	
	while(vma != NULL){
		unsigned long start = vma->vm_start;
		unsigned long end = vma->vm_end;
		vm_flags_t flags = vma->vm_flags;
		//const char *name = NULL;

		char read = flags & VM_READ ? 'r' : '-';
		char write = flags & VM_WRITE ? 'w' : '-';
		char execute = flags & VM_EXEC ? 'x' : '-';
		char share = flags & VM_MAYSHARE ? 's' : 'p';

		if(stack_guard_page_start(vma, start))
			start += PAGE_SIZE;
		if(stack_guard_page_end(vma, end))
                        end -= PAGE_SIZE;

		//printk(KERN_INFO "%08lx-%08lx %c%c%c%c %s\n",start, end, read, write, execute, share, name);
		printk(KERN_INFO "%08lx-%08lx %c%c%c%c \n",start, end, read, write, execute, share);
		printk(KERN_INFO "Size : %8lu kB\n", (end-start)>>10);
		printk(KERN_INFO "Size : %8lu \n", (end-start)>>12);
		printk(KERN_INFO "RSS : %8lu kB\n", calculateRSS(vma));
				
		vma = vma->vm_next;
	}
	
}

static int __init mymaps_start(void){
	
	struct task_struct *p;

	printk(KERN_INFO "Mymaps Module Loaded.\n");
	printk(KERN_INFO "pid:%d\n",pid);
	
	for_each_process(p){
		if(task_pid_nr(p) == pid){
			printk(KERN_INFO "process:%s\n",p->comm);
			printvma(p->mm->mmap);
			break;
		}
	}
	return 0;
}

static void __exit mymaps_end(void){

	printk(KERN_INFO "Mymaps Module removed.\n");	
}

module_init(mymaps_start);
module_exit(mymaps_end);

