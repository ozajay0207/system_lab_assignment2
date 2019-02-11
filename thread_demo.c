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

char input_file[] = "input_file.txt";
int main_arr[TOTAL_ELEMENTS];
int MM[6]={-1,-1,-1,-1,-1,-1};

int main_memory_counter=0;
int sum_p1=0,sum_p2=0,sum_p3=0;

sem_t *sem1,*sem2,*sem3;

void print_PCB(int process_state,int process_id,int program_counter,int page_no){
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

void print_main_memory(){
	printf("Main Memory Content:\n");
	int process_id;
	for(int i=0;i<6;i++){
		if(MM[i]<=12)
			process_id=1;
		else if(MM[i]<=24 && MM[i]>13)
			process_id=2;
		else if(MM[i]<=37 && MM[i]>25)
			process_id=3;
		else
			process_id=100;
		printf("P(%d):%d\n",process_id,MM[i]);

	}
}

int check_for_page(int page_no){
	for(int i=0;i<6;i++){
		if(MM[i]==page_no)
			return 1;
	}
	return 0;
}

int do_sum(int process_id,int page_no){
	page_no--;
	int i;
	int sum=0;
	int start_index=page_no*40;
	int end_index=(page_no*40)+PAGE_ENTRIES;

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
	
	printf("No of Entries:%d\n",end_index-start_index);
	for(i=start_index;i<end_index;i++){
		sum = sum + main_arr[i];
	
	}
	
	return sum;
}

void *p1_fun(void *vargp) 
{ 
	int i,j;
	int current_page=1;
	
	while(current_page<=11){
		if(main_memory_counter>=6)
			main_memory_counter=0;
		sem_wait(sem1);

		if(!check_for_page(current_page)){
			printf("\nPage Fault:%d\n",current_page);
			MM[main_memory_counter++]=current_page;	
		}		
		print_main_memory();
		sum_p1=sum_p1+do_sum(1,current_page);
		current_page++;
		sem_post(sem2);
		//sleep(2);
	}
	
	current_page=13;
	sem_wait(sem1);
	if(!check_for_page(current_page)){
		printf("\nPage Fault:%d\n",current_page);
		MM[main_memory_counter++]=13;	
	}			
	print_main_memory();
	sum_p1=sum_p1+do_sum(1,13);
	sem_post(sem2);
	//sleep(2);

	current_page=12;
	sem_wait(sem1);
	if(!check_for_page(current_page)){
		printf("\nPage Fault:%d\n",current_page);
		MM[main_memory_counter++]=12;	
	}						
	print_main_memory();
	sum_p1=sum_p1+do_sum(1,12);
	sem_post(sem2);
	//sleep(2);
	printf("Sum:%d\n",sum_p1);

	sem_wait(sem1);
	printf("\nSumming up partial sum of the processes\n");
	sum_p1=sum_p1+sum_p2+sum_p3;
	printf("%d\n\n",sum_p1);	

} 


void *p2_fun(void *vargp) 
{ 
	int i,j;
	int current_page=14;
	
	while(current_page<=24){
		if(main_memory_counter>=6)
			main_memory_counter=0;

		sem_wait(sem2);
		if(!check_for_page(current_page)){
			printf("\nPage Fault:%d\n",current_page);
			MM[main_memory_counter++]=current_page;	
		}
		print_main_memory();			
		sum_p2=sum_p2+do_sum(2,current_page);			
		current_page++;

		sem_post(sem3);
		//sleep(2);
	}

	current_page=25;
	sem_wait(sem2);
	if(!check_for_page(current_page)){
		printf("\nPage Fault:%d\n",current_page);
		MM[main_memory_counter++]=25;	
	}						
	print_main_memory();
	sum_p2=sum_p2+do_sum(2,25);
	sem_post(sem3);
	//sleep(2);

	current_page=13;
	sem_wait(sem2);
	if(!check_for_page(current_page)){
		printf("\nPage Fault:%d\n",current_page);
		MM[main_memory_counter++]=13;	
	}						
		
	print_main_memory();
	sum_p2=sum_p2+do_sum(2,13);
	sem_post(sem3);
	//sleep(2);
	printf("Sum:%d\n",sum_p2);
} 


void *p3_fun(void *vargp) 
{ 
	int i,j;
	int current_page=26;
	while(current_page<=36){
		if(main_memory_counter>=6)
			main_memory_counter=0;

		sem_wait(sem3);
		if(!check_for_page(current_page)){
			printf("\nPage Fault:%d\n",current_page);
			MM[main_memory_counter++]=current_page;	
		}
		print_main_memory();		
		sum_p3=sum_p3+do_sum(3,current_page);		
		current_page++;

		sem_post(sem1);
	//	sleep(2);
	}

	current_page=37;
	sem_wait(sem3);
	if(!check_for_page(current_page)){
		printf("\nPage Fault:%d\n",current_page);
		MM[main_memory_counter++]=37;	
	}						

	print_main_memory();
	sum_p3=sum_p3+do_sum(3,37);
	sem_post(sem1);
	//sleep(2);

	current_page=25;
	sem_wait(sem3);
	if(!check_for_page(current_page)){
		printf("\nPage Fault:%d\n",current_page);
		MM[main_memory_counter++]=25;	
	}						

	print_main_memory();
	sum_p3=sum_p3+do_sum(3,25);
	sem_post(sem1);
	//sleep(2);
	printf("Sum:%d\n",sum_p3);
} 



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

void dump_to_file(){
	int i;
	FILE *f;
	f=fopen(input_file,"w");

	for(i=1;i<=TOTAL_ELEMENTS;i++)	
	fprintf(f,"%d\n",1);

	fclose(f);
}

int main() 
{ 
	int i; 
	pthread_t tid1,tid2,tid3; 
	int temp=123;
	int *ptr=&temp;

	sem_unlink("/sema1");	
	sem_unlink("/sema2");
	sem_unlink("/sema3");
	sem1 = sem_open("/sema1", O_CREAT,  0644, 1);
	sem2 = sem_open("/sema2", O_CREAT,  0644, 0);
	sem3 = sem_open("/sema3", O_CREAT,  0644, 0);

	///dump_to_file();
	read_array();
	
	pthread_create(&tid1,NULL, p1_fun, (void *)&temp); 
	pthread_create(&tid2,NULL, p2_fun, (void *)&temp); 
	pthread_create(&tid3,NULL, p3_fun, (void *)&tid3); 

	pthread_join(tid1, NULL); 
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);

	return 0; 
} 

