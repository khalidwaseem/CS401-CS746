#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include<linux/moduleparam.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <linux/pid.h>
#include <linux/random.h>
#include <linux/mmu_notifier.h>
#include <linux/timer.h>

MODULE_AUTHOR("Khalid Waseem");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel Assignment 1");

#define MAX_PAGES 8000000
#define MAX_TRACKED 500

static int iPid = 0;
module_param(iPid,int,0);
static int timeInterval = 15000;
module_param(timeInterval,int,0);
static int pagesToTrack = 100;
module_param(pagesToTrack,int,0);

int itrNo=0;
int rss;
int avgRSS=0;
int wss;
int avgWSS=0;
int vas;
int avgVAS=0;
int countEP;
struct task_struct *task;
unsigned long eligiblePages[MAX_PAGES][2];
unsigned long trackedPages[MAX_TRACKED];
static struct timer_list myTimer;


//static void resetVariables();
//static int setTaskStruct();


extern int (*wss_estimator)(int pid, unsigned long address);
//extern void mmu_notifier_change_pte(struct mm_struct *mm, unsigned long address, pte_t);

static void resetVariables(void){
        int i;
        rss = 0;
        wss = 0;
        vas = 0;
        //countEP = 0;
        for(i=0; i<countEP; i++){
                eligiblePages[i][0] = 0;
                eligiblePages[i][1] = 0;
        }
        for(i=0; i<pagesToTrack; i++){
                trackedPages[i] = 0;
        }
        countEP = 0;
}

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

		//if(pte_present(*pte)){
		if((pte_flags(*pte)&(_PAGE_PRESENT)) && !(pte_flags(*pte)&(_PAGE_PROTNONE))){
			pgcount++;
			eligiblePages[countEP][0] = start;
			eligiblePages[countEP][1] = 0;
			countEP++;
		}
		start += PAGE_SIZE;				
	}
	return pgcount;
}

static int unmarkPage(unsigned long addr){
        struct vm_area_struct *vma;
        pgd_t *pgd;
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;
        pte_t pte_temp1,pte_temp2;

	vma = find_vma(task->mm, addr);
        pgd = pgd_offset(vma->vm_mm, addr);
        if(pgd_none(*pgd) || pgd_bad(*pgd))
        	return 0;
        pud = pud_offset(pgd, addr);
        if(pud_none(*pud) || pud_bad(*pud))
                return 0;
        pmd = pmd_offset(pud, addr);
        if(pmd_none(*pmd) || pmd_bad(*pmd))
                return 0;
        pte = pte_offset_map(pmd, addr);
        if(!pte)
                return 0;	
	if(!(pte_flags(*pte)&(_PAGE_PRESENT)) && (pte_flags(*pte)&(_PAGE_PROTNONE))){
        	pte_temp1 = pte_set_flags(*pte, _PAGE_PRESENT);
                pte_temp2 = pte_clear_flags(pte_temp1, _PAGE_PROTNONE);
                set_pte_at(vma->vm_mm,addr,pte,pte_temp2);
                if(!(pte_flags(*pte)&(_PAGE_PRESENT))&&(pte_flags(*pte)&(_PAGE_PROTNONE))){
                	return 0;
                }
        }
	return 1;	
}


int wss_estimator_handler(int pid, unsigned long address){
	int i;
	//printk(KERN_INFO "Page Fault with pid:%d.\n",pid);
	if(pid == iPid){
		//printk(KERN_INFO "Page Fault with pid:%d.\n",pid);
		for(i=0; i<pagesToTrack; i++){
			if((trackedPages[i] & 0xfffffffffffff000)==(address & 0xfffffffffffff000)){
				wss++;
				//printk(KERN_INFO "wss:%d\n",wss);
				if(!unmarkPage(address)){
					//printk(KERN_INFO "Couldn't reset Page flags.\n");
				}
			}
		}
	}
	return 0;
}


static int markRandomPages(void){
	int i=0;
	unsigned random;
	unsigned long addr;
	struct vm_area_struct *vma;
	pgd_t *pgd;
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;
	pte_t pte_temp1,pte_temp2;
	while(i<pagesToTrack){
		get_random_bytes(&random,sizeof(random));
		random %= countEP;
		if(!eligiblePages[random][1]){
			addr = eligiblePages[random][0];
			//eligiblePages[random][1] = 1;
			vma = find_vma(task->mm, addr);
			pgd = pgd_offset(vma->vm_mm, addr);
                	if(pgd_none(*pgd) || pgd_bad(*pgd))
                        	return 0;
                	pud = pud_offset(pgd, addr);
                	if(pud_none(*pud) || pud_bad(*pud))
                        	return 0;
                	pmd = pmd_offset(pud, addr);
                	if(pmd_none(*pmd) || pmd_bad(*pmd))
                        	return 0;
                	pte = pte_offset_map(pmd, addr);
                	if(!pte)
                        	return 0;
			if(pte_present(*pte)){
				if((pte_flags(*pte)&(_PAGE_PRESENT)) && !(pte_flags(*pte)&(_PAGE_PROTNONE))){
					pte_temp1 = pte_clear_flags(*pte, _PAGE_PRESENT);
					pte_temp2 = pte_set_flags(pte_temp1, _PAGE_PROTNONE);
					//mmu_notifier_change_pte(vma->vm_mm,addr,pte_temp2);
					set_pte_at(vma->vm_mm,addr,pte,pte_temp2);
					if(!(!(pte_flags(*pte)&(_PAGE_PRESENT))&&(pte_flags(*pte)&(_PAGE_PROTNONE)))){
						return 0;
					}

				}
			}
			eligiblePages[random][1] = 1;
			trackedPages[i] = addr;
			i++;
		}	
	}
	return 1;
}

static void setEligiblePages(struct vm_area_struct *vma){

	while(vma != NULL){
                unsigned long start = vma->vm_start;
                unsigned long end = vma->vm_end;
		if(stack_guard_page_start(vma, start))
                        start += PAGE_SIZE;
                if(stack_guard_page_end(vma, end))
                        end -= PAGE_SIZE;

		vas = vas + ((end-start)>>12);
		rss = rss + calculateRSS(vma);

		vma = vma->vm_next;
	}
	
}

static int setTaskStruct(void){
	struct pid *pidStruct;
	pidStruct = find_get_pid(iPid);
	if(!pidStruct){
                printk(KERN_INFO "Invalid Input pid\n");
                return 1;
        }
	task = pid_task(pidStruct, PIDTYPE_PID);
	if(!(task && (task->pid==iPid))){
                printk(KERN_INFO "Couldn't get task_struct for Input pid\n");
                return 1;
        }
        if(!task->mm){
                printk(KERN_INFO "Input pid is kernel thread\n");
                return 1;
        }
	return 0;
}


void myTimerCallback(unsigned long data){
	int ret,i;
        struct pid *pidStruct = find_get_pid(iPid);
        if(pidStruct){
                task=pid_task(pidStruct, PIDTYPE_PID);
                if(task && task->mm){
			
			avgRSS += rss;
			avgWSS += wss;
			avgVAS += vas;
			itrNo++;
			for(i=0; i<pagesToTrack; i++){
				unmarkPage(trackedPages[i]);
			}

                        printk(KERN_INFO "wss:%d\n",wss);
                        resetVariables();
                        setEligiblePages(task->mm->mmap);
                        if(!markRandomPages()){
                                printk(KERN_INFO "Couldn't mark random pages.\n");
                                //return 0;
                        }
                        printk(KERN_INFO "RSS:%d\tVAS:%d\n",rss,vas);

                        ret = mod_timer(&myTimer, jiffies+msecs_to_jiffies(timeInterval));
                        if(ret){
                                printk(KERN_INFO "Error in Timer.\n");
                        }

                        //printk(KERN_INFO "RSS:%d\tVAS:%d\tEligible Pages:%d\n",rss,vas,countEP);

                }
        }
}


static int __init wssestimator_start(void){
	int ret; 
	
	printk(KERN_INFO "WSS Estimator Module Loaded.\n");
	printk(KERN_INFO "Input pid:%d\tTime Interval:%d\tPages To Track:%d\n",iPid, timeInterval, pagesToTrack);
	
	if(setTaskStruct()){
		return 0;
	}
	wss_estimator = &wss_estimator_handler;
	resetVariables();
	setEligiblePages(task->mm->mmap);
	if(!markRandomPages()){
		printk(KERN_INFO "Couldn't mark random pages.\n");
		return 0;
	}
	printk(KERN_INFO "RSS:%d\tVAS:%d\n",rss,vas);

	setup_timer(&myTimer, myTimerCallback, 0);
	ret = mod_timer(&myTimer, jiffies+msecs_to_jiffies(timeInterval));
	if(ret){
		printk(KERN_INFO "Error in Timer.\n");
                return 0;
	}
	
	//printk(KERN_INFO "RSS:%d\tVAS:%d\tEligible Pages:%d\n",rss,vas,countEP);
	/*for(i=0; i<pagesToTrack; i++){
		printk(KERN_INFO "%d\t%08lx\n",i,trackedPages[i]);
	}*/

	return 0;
}

static void __exit wssestimator_end(void){
	int ret;
	wss_estimator = NULL;
	printk(KERN_INFO "Avg RSS:%d\tAvg VAS:%d\tAvg WSS:%d\n",avgRSS/itrNo,avgVAS/itrNo,avgWSS/itrNo);
	ret = del_timer(&myTimer);
	if(ret){
		printk(KERN_INFO "Error deleting Timer.\n");
	}
	printk(KERN_INFO "WSS Estimator Module removed.\n");	
}

module_init(wssestimator_start);
module_exit(wssestimator_end);

