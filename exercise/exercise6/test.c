#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "myioctl.h"

#define DEVICE_NAME "/dev/myioctl"
//#define MAX_ARRAY_SIZE 10
#define MAX_LOOP 5

void reset_array(int fd){
    if (ioctl(fd, MYIOCTL_CLR) == -1){
        perror("Error ioctl reset");
    }
}

void print_array(int fd){
    int i;
    array_arg q;
    if (ioctl(fd, MYIOCTL_PRINT, &q) == -1){
        perror("Error ioctl reset");
    }else{
	for(i=0; i<q.size; i++){
		printf("%d ", q.array[i]);
	}
	printf("\n");
    }	
}

int get_value(int fd, int index){
    myioctl_arg_t q;

    q.index = index;

    if (ioctl(fd, MYIOCTL_GET_VALUE, &q) == -1){
        perror("ERROR ioctl read");
	return -1;	
    }else{
        printf("Index : %d Value : %d\n", q.index, q.value);
    }
    return q.value;	
}

void set_value(int fd, int value, int index){
    myioctl_arg_t q;

    q.index = index;
    q.value = value;	

    if (ioctl(fd, MYIOCTL_SET_VALUE, &q) == -1){
        perror("query_apps ioctl set");
    }
}

int main(int argc, char *argv[])
{
    char *file_name = DEVICE_NAME;
    int fd;
    int index,value,i;	
    
    fd = open(file_name, O_RDWR);
    if (fd == -1){
        perror("Error opening file\n");
        return 2;
    }

    if (argc > 1){
	if (strcmp(argv[1], "reset") == 0){
		reset_array(fd);
        }else if(strcmp(argv[1], "print") == 0){
		print_array(fd);
	}else{
		fprintf(stderr, "Usage: %s [reset|print]\n", argv[0]);
            	return 1;
	}
    }else{
	srand(time(NULL));
	for(i=0; i<MAX_LOOP; i++){
		index = rand()%MAX_ARRAY_SIZE;
		value = get_value(fd,index);
		if(value > -1){
			value++;
			set_value(fd,value,index);
		}
		printf("After update \t");
		value = get_value(fd,index);
		sleep(1);
	}
    }
    
    close (fd);

    return 0;
}


		
