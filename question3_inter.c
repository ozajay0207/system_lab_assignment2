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
int p1[PAGE_ENTRIES],p2[PAGE_ENTRIES],p3[PAGE_ENTRIES],p4[PAGE_ENTRIES],p5[PAGE_ENTRIES],p6[PAGE_ENTRIES];
int PMT[TOTAL_PAGES][2];
int MM[6]={-1,-1,-1,-1,-1,-1};
int main_memory_counter=0;


sem_t *sem1,*sem2,*sem3;


int do_sum(int process_id,int page_no){
	int i;
	int sum=0;

	if(page_no == 25){
		if(process_id==2){
			for(i=page_no*40;i<(page_no*40)+20;i++){
				sum = sum + main_arr[i];
			}
			return sum;
		}
		if(process_id==3){
			for(i=(page_no*40)+20;i<(page_no*40)+PAGE_ENTRIES;i++){
				sum = sum + main_arr[i];
			}
			return sum;
		}
	}
	if(page_no == 13){
		if(process_id==1){
			for(i=page_no*40;i<(page_no*40)+10;i++){
				sum = sum + main_arr[i];
			}
			return sum;
		}
		if(process_id==2){
			for(i=(page_no*40)+10;i<(page_no*40)+PAGE_ENTRIES;i++){
				sum = sum + main_arr[i];
			}
			return sum;
		}
	}
	for(i=page_no*40;i<(page_no*40)+PAGE_ENTRIES;i++){
		sum = sum + main_arr[i];
	
	}
	//printf("\n%d %d",page_no,sum);
	return sum;
}

void print_main_memory(){
	printf("Main Memory Content:\n");
	for(int i=0;i<6;i++){
		printf("Page : %d\n",MM[i]);
	}
}


void *p1_fun(void *vargp) 
{ 
	int i,j;
	int sum=0;
	int current_page=1;
	
	while(current_page<=11){
		if(main_memory_counter>=6)
			main_memory_counter=0;
		sem_wait(sem1);
		//for(i=0;i<2 && current_page<=11;i++){
			//printf("\nPage Fault:%d",current_page);
			MM[main_memory_counter++]=current_page;			
			print_main_memory();
			sum=sum+do_sum(1,current_page);
			current_page++;
		//}
		sem_post(sem2);
		sleep(2);
	}
	
	sem_wait(sem1);
	MM[main_memory_counter++]=13;			
	print_main_memory();
	sum=sum+do_sum(1,13);
	sem_post(sem2);
	sleep(2);

	sem_wait(sem1);
	MM[main_memory_counter++]=12;			
	print_main_memory();
	sum=sum+do_sum(1,12);
	sem_post(sem2);
	sleep(2);
} 


void *p2_fun(void *vargp) 
{ 
	int i,j;
	int sum=0;
	int current_page=14;
	
	while(current_page<=24){
		if(main_memory_counter>=6)
			main_memory_counter=0;

		sem_wait(sem2);
		//for(i=0;i<2 && current_page<25;i++){
			//printf("\nPage Fault:%d",current_page);
			MM[main_memory_counter++]=current_page;	
			print_main_memory();			
			sum=sum+do_sum(2,current_page);
			current_page++;
		//}
		sem_post(sem3);
		sleep(2);
	}

	sem_wait(sem2);
	MM[main_memory_counter++]=25;			
	print_main_memory();
	sum=sum+do_sum(2,25);
	sem_post(sem3);
	sleep(2);

	sem_wait(sem2);
	MM[main_memory_counter++]=13;			
	print_main_memory();
	sum=sum+do_sum(2,13);
	sem_post(sem3);
	sleep(2);

} 


void *p3_fun(void *vargp) 
{ 
	int i,j;
	int sum=0;
	int current_page=26;
	while(current_page<=36){
		if(main_memory_counter>=6)
			main_memory_counter=0;

		sem_wait(sem3);
		//for(i=0;i<2 && current_page<=37;i++){
			//printf("\nPage Fault:%d",current_page);
			MM[main_memory_counter++]=current_page;	
			print_main_memory();		
			sum=sum+do_sum(3,current_page);
			current_page++;
		//}
		sem_post(sem1);
		sleep(2);
	}

	sem_wait(sem3);
	MM[main_memory_counter++]=37;			
	print_main_memory();
	sum=sum+do_sum(3,37);
	sem_post(sem1);
	sleep(2);

	sem_wait(sem3);
	MM[main_memory_counter++]=25;			
	print_main_memory();
	sum=sum+do_sum(3,25);
	sem_post(sem1);
	sleep(2);
	
} 


void dump_to_file(){
	int i;
	FILE *f;
	f=fopen(input_file,"w");

	for(i=1;i<=TOTAL_ELEMENTS;i++)	
	fprintf(f,"%d\n",i);

	fclose(f);
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


	read_array();
	
	pthread_create(&tid1,NULL, p1_fun, (void *)&temp); 
	pthread_create(&tid2,NULL, p2_fun, (void *)&temp); 
	pthread_create(&tid3,NULL, p3_fun, (void *)&tid3); 

	//initialize_PMT();

	pthread_join(tid1, NULL); 
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);

	return 0; 
} 

