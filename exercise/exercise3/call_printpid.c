#include<stdio.h>
#include<stdlib.h>
#include<sys/syscall.h>
#include<linux/kernel.h>
#include<unistd.h>

#define SYSCALL_NO 332

int main(){
	long int ret;
	printf("Invoking system call\n");
	ret = syscall(SYSCALL_NO);
	if(ret < 0){
		printf("Error invoking system call\n");
		exit(1);
	}
	return 0;
}
