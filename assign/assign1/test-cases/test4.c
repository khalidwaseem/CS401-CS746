#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>

#define NO_PAGES 51200	// Aprox 200 Mb
#define PAGE_SIZE 4096	

int main() {
        int i=0;
        char *array = (char*)malloc(sizeof(char)*PAGE_SIZE*NO_PAGES);
        printf("pid:%d\n", getpid());
	srand(time(NULL));
	int rno;
	while(1){

		rno = rand();
		rno %= NO_PAGES;
		array[rno*PAGE_SIZE] = 'X';
                //sleep(1);
	}
	return 0;
}
