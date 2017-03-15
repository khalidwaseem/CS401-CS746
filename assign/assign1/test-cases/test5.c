#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>

#define MAX_NO_PAGES 102400	// Aprox 400 Mb
#define MIN_NO_PAGES 51200	// Aprox 200 Mb
#define PAGE_SIZE 4096	

int main() {
        int i=0;
	int rno;
	int rnopg;
	char *array = NULL;
	
	printf("pid:%d\n", getpid());
        srand(time(NULL));
	while(1){
		rnopg = rand();
		rnopg %= MIN_NO_PAGES;
		rnopg += MIN_NO_PAGES; 
		//printf("rnopg:%d\n",rnopg);	
		array = (char*)malloc(sizeof(char)*PAGE_SIZE*rnopg);
		
		for(i=0; i<rnopg; i=i+2){
			array[i*PAGE_SIZE] = 'X';
		}
		free(array);
		array = NULL;
                //sleep(15);
	}
	return 0;
}
