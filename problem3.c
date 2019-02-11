#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TOTAL_ELEMENTS 1470

char input_file[] = "input_file.txt";
int input_arr[TOTAL_ELEMENTS];


void dump_to_file(){
	int i;
	FILE *f;
	f=fopen(input_file,"w");

	for(i=1;i<=TOTAL_ELEMENTS;i++)	
	fprintf(f,"%d\n",i);

	fclose(f);
}

void read_file_input(){

	int sum=0;
	int temp,i;
	FILE *f;
	f=fopen(input_file,"r");

	for(i=0;i<TOTAL_ELEMENTS;i++){
		fscanf(f,"%d",&input_arr[i]);
		sum += input_arr[i];
	}

	printf("Sum:%d\n",sum);
	fclose(f);
}

int main(){
	//dump_to_file();	
	read_file_input();
	return 0;
}
