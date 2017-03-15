#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/moduleparam.h>
#include<linux/jiffies.h>

MODULE_AUTHOR("Khalid Waseem");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel Programming Exercise 2");

static int mychoice = 1;
module_param(mychoice,int,0);

int init_module(void){
	int i = 0;
	unsigned long jiff=jiffies;
	printk(KERN_INFO "LKM Exercise 2 Loaded\n");
	if(mychoice==1){
		printk(KERN_INFO "Hello World\n");
	}else if(mychoice==2){
		printk(KERN_ALERT "Hello World Console\n");
	}else if(mychoice==3){
		for(i=1; i<=50000000; i++){
			if(i%10000000==0){
				printk(KERN_INFO "Current Jiffies:%lu\n", jiff);
				jiff = jiffies;
			}
		}
	}
	return 0;
}

void cleanup_module(void){
	printk(KERN_INFO "LKM Exercise 2 exited\n");
}
