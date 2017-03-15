#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

#define NO_PAGES 51200	// Aprox 200 Mb
#define PAGE_SIZE 4096	

int main() {
        int i=0;
        char *array = (char*)malloc(sizeof(char)*PAGE_SIZE*NO_PAGES);
        printf("pid:%d\n", getpid());
	
	while(1){
		for(i=0; i<NO_PAGES; i=i+2) {
                        array[i*PAGE_SIZE] = 'X';
                }
		sleep(15);
	}
	return 0;
}
