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
#define TOTAL_PAGES 37
#define PAGE_ENTRIES 40
#define MAIN_MEMORY_SIZE 6

char input_file[] = "input_file.txt";

int main_arr[TOTAL_ELEMENTS];
int MM[6][2]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

int main_memory_counter=0;
int sum_p1=0,sum_p2=0,sum_p3=0;
int pf_p1=0,pf_p2=0,pf_p3=0;
int pr_p1=0,pr_p2=0,pr_p3=0;

sem_t *sem1,*sem2,*sem3;

//TO PRINT THE PCB CONTENT
void print_PCB(int process_state,int process_id,int program_counter,int page_no){

	//CHECK FOR STATE AND PRINT ACCORDINGLY
	if(process_state==0)
		printf("State:Running\n");	
	else if(process_state==1)
		printf("State:Waiting\n");	
	else
		printf("State:Exit\n");	
	printf("Process Id:%d\n",process_id);
	printf("Program Counter:%d\n",program_counter);
	printf("Memory Management Information:%d\n",page_no);

}

//TO PRINT THE MAIN MEMORY CONTENT
void print_main_memory(){

	printf("Main Memory Content:\n");
	printf("\n");
	for(int i=0;i<MAIN_MEMORY_SIZE;i++){		
		printf("| P(%d):%d |",MM[i][1],MM[i][0]);

	}
	printf("\n");
	printf("\n");
}

//CHECK FOR PAGE IF NOT FOUND RETURN 0(PAGE FAULT)
int check_for_page(int page_no){

	for(int i=0;i<MAIN_MEMORY_SIZE;i++){
		if(MM[i][0]==page_no)
			return 1;
	}
	return 0;
}

//TO FETCH THE ARRAY VALUES AND DO SUMMATION
int do_sum(int process_id,int page_no){

	page_no--;
	int i;
	int sum=0;
	int start_index=page_no*40;
	int end_index=(page_no*40)+PAGE_ENTRIES;

	//CHECKING THE SHARED PAGES FOR CONDITIONS

	//PAGE 25 IS SHARED BETWEEN PROCESS 2 AND 3
	if(page_no == 24){
		if(process_id==2){
			start_index=page_no*40;
			end_index=(page_no*40)+20;
		}
		if(process_id==3){
			start_index=(page_no*40)+20;
			end_index=(page_no*40)+PAGE_ENTRIES;
		}
	}

	//PAGE 13 IS SHARED BETWEEN PROCESS 1 AND 3
	if(page_no == 12){
		if(process_id==1){
			start_index=page_no*40;
			end_index=(page_no*40)+10;			
		}
		if(process_id==2){
			start_index=(page_no*40)+10;
			end_index=(page_no*40)+PAGE_ENTRIES;
		}
	}
	
	//CALCULATE THE SUM BASED ON CALCULATED INDICES
	printf("No of Entries:%d\n",end_index-start_index);
	for(i=start_index;i<end_index;i++){
		sum = sum + main_arr[i];
	
	}
	
	return sum;
}

//TO PERFORM THE PAGE FAULT , REPLACEMENT AND PAGING STRATEGY
void do_paging(int *current_page,int *replaced_page,int *pf_p,int *pr_p,int *sum_p,int process_id){

	*replaced_page=-1;

	//RESET MAIN MEMORY COUNTER IF GREATER THAN MEMORY SIZE
	if(main_memory_counter>=MAIN_MEMORY_SIZE)
		main_memory_counter=0;

	//CHECK IF THE PAGE IS PRESENT IN MEMORY
	if(!check_for_page(*current_page)){	

		//DO NOT COUNT PAGE REPLACEMENT IF MM IS EMPTY (i.e. in the beginning)		
		if(MM[main_memory_counter][0]!=-1){
			*replaced_page=MM[main_memory_counter][0];
			*pr_p=*pr_p+1;
		}

		*pf_p=*pf_p+1;
		printf("\nPage Fault:%d\n",*current_page);

		//IF PAGE IS REPLACED THEN PRINT INFORMATION
		if(*replaced_page!=-1)
			printf("Replacing Page: %d\n",*replaced_page);		

		//UPDATE THE PAGE IN MAIN MEMORY	
		MM[main_memory_counter][0]=*current_page;	
		MM[main_memory_counter][1]=1;
		main_memory_counter++;	

	//IF PAGE IS ALREADY IN MAIN MEMORY
	}else{
		printf("\nPage Hit:%d\n",*current_page);
	}	
	
	print_main_memory();

	//ADD THE SUM OF CURRENT PAGE
	*sum_p=*sum_p+do_sum(process_id,*current_page);

}

//THREAD FUNCTION TO RUN PROCESS 1
void *p1_fun(void *vargp) 
{ 

	int i,j;
	int current_page=1;
	int replaced_page=-1;
	
	//PERFORM PAGING FOR NON-SHARED PAGES IN FCFS ORDER
	while(current_page<=11){	
		sem_wait(sem1);
		do_paging(&current_page,&replaced_page,&pf_p1,&pr_p1,&sum_p1,1);
		current_page++;
		sem_post(sem2);
	}
	
	//MANAGING THE SHARED PAGES EXPLICITLY
	sem_wait(sem1);
	current_page=13;
	replaced_page=-1;
	do_paging(&current_page,&replaced_page,&pf_p1,&pr_p1,&sum_p1,1);
	sem_post(sem2);

	sem_wait(sem1);	
	current_page=12;
	replaced_page=-1;
	do_paging(&current_page,&replaced_page,&pf_p1,&pr_p1,&sum_p1,1);
	sem_post(sem2);

	//PRINT THE FINAL SUM CALCULATED
	printf("Sum by Process 1 :%d\n",sum_p1);


	//WAIT FOR BOTH THE PROCESS 2 AND 3 TO RETURN THE FINAL CONTENTS
	sem_wait(sem1);
	printf("\nSumming up partial sum of the processes...\n");
	sum_p1=sum_p1+sum_p2+sum_p3;
	printf("Final Sum : %d\n\n",sum_p1);	

	printf("\nPage Faults:\n");
	printf("Page Fault Process 1: %d\n",pf_p1);
	printf("Page Fault Process 2: %d\n",pf_p2);
	printf("Page Fault Process 3: %d\n",pf_p3);

	printf("\nPage Replacements:\n");
	printf("Page Replacements Process 1: %d\n",pr_p1);
	printf("Page Replacements Process 2: %d\n",pr_p2);
	printf("Page Replacements Process 3: %d\n",pr_p3);
	
} 

//THREAD FUNCTION TO RUN PROCESS 2
void *p2_fun(void *vargp) 
{ 

	int i,j;
	int current_page=14;
	int replaced_page=-1;
	
	//PERFORM PAGING FOR NON-SHARED PAGES IN FCFS ORDER
	while(current_page<=24){	
		sem_wait(sem2);
		do_paging(&current_page,&replaced_page,&pf_p2,&pr_p2,&sum_p2,2);
		current_page++;
		sem_post(sem3);S
	}

	//MANAGING THE SHARED PAGES EXPLICITLY	
	sem_wait(sem2);
	current_page=25;
	replaced_page=-1;
	do_paging(&current_page,&replaced_page,&pf_p2,&pr_p2,&sum_p2,2);
	sem_post(sem3);

	sem_wait(sem2);	
	current_page=13;
	replaced_page=-1;
	do_paging(&current_page,&replaced_page,&pf_p2,&pr_p2,&sum_p2,2);
	sem_post(sem3);

	//PRINT THE FINAL SUM CALCULATED  BY PROCESS 2
	printf("Sum by Process 2 :%d\n",sum_p2);
} 

//THREAD FUNCTION TO RUN PROCESS 3
void *p3_fun(void *vargp) 
{ 
	
	int i,j;
	int current_page=26;
	int replaced_page=-1;

	//PERFORM PAGING FOR NON-SHARED PAGES IN FCFS ORDER
	while(current_page<=36){	
		sem_wait(sem3);
		do_paging(&current_page,&replaced_page,&pf_p3,&pr_p3,&sum_p3,3);
		current_page++;
		sem_post(sem1);
	}

	//MANAGING THE SHARED PAGES EXPLICITLY	
	sem_wait(sem3);
	current_page=37;
	replaced_page=-1;
	do_paging(&current_page,&replaced_page,&pf_p3,&pr_p3,&sum_p3,3);
	sem_post(sem1);

	sem_wait(sem3);	
	current_page=25;
	replaced_page=-1;
	do_paging(&current_page,&replaced_page,&pf_p3,&pr_p3,&sum_p3,3);
	sem_post(sem1);

	//PRINT THE FINAL SUM CALCULATED  BY PROCESS 3
	printf("Sum by Process 3 :%d\n",sum_p3);
} 


//TO READ 1470 ELEMENTS FROM FILE TO ARRAY
void read_array(){

	int sum=0;
	int temp,i;
	FILE *f;
	f=fopen(input_file,"r");

	for(i=0;i<TOTAL_ELEMENTS;i++){
		fscanf(f,"%d",&main_arr[i]);
		//printf("%d\n",main_arr[i]);
		sum += main_arr[i];
	}

	printf("Sum:%d\n",sum);
	fclose(f);
}

//TO DUMP RANDOM VALUES TO FILE(RUN ONLY ONCE)
void dump_to_file(){

	int i;
	FILE *f;
	f=fopen(input_file,"w");
	
	//WRITING CONTENTS TO FILE
	for(i=1;i<=TOTAL_ELEMENTS;i++)	
		fprintf(f,"%d\n",1);

	fclose(f);
}

//MAIN FUNCTION
int main() 
{ 
	int i; 
	pthread_t tid1,tid2,tid3; 

	//UNLINKING THE SEMAPHORES AND INITIALIZE FOR USE
	sem_unlink("/sema1");	
	sem_unlink("/sema2");
	sem_unlink("/sema3");

	sem1 = sem_open("/sema1", O_CREAT,  0644, 1);
	sem2 = sem_open("/sema2", O_CREAT,  0644, 0);
	sem3 = sem_open("/sema3", O_CREAT,  0644, 0);

	///dump_to_file();
	read_array();
	
	//TO CREATE THREADS EACH FOR RESPECTIVE PROCESS
	pthread_create(&tid1,NULL, p1_fun, (void *)&tid1); 
	pthread_create(&tid2,NULL, p2_fun, (void *)&tid2); 
	pthread_create(&tid3,NULL, p3_fun, (void *)&tid3); 

	//WAITING FOR THREAD TO COMPLETE EXECUTION
	pthread_join(tid1, NULL); 
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);

	return 0; 
} 

