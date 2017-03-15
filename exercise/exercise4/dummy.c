#include<stdio.h>
#include<stdlib.h>

int main(){
	
	char *arr = (char*)malloc(sizeof(char)*1024*4*51200);
	int i=0;
	while(1){
		for(i=0; i<1024*4*51200; i+(4*1024)){
			arr[i] = 'a';
		}
	}
	return 0;
}
