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

//TO UNLINK THE USED SEMAPHORES
void unlink_semaphores(){
		
	sem_unlink("/milk");
	sem_unlink("/suggar");
	sem_unlink("/agent");
}

void controller(int n){

	int ing1,ing2,i;
	time_t time_var;
	pid_t pid1,pid2,pid3,pid4;	
	sem_t *sem_coffee,*sem_milk,*sem_suggar,*sem_agent;

	unlink_semaphores();

	sem_coffee = sem_open("/coffee", O_CREAT,  0644, 0);
	sem_milk = sem_open("/milk", O_CREAT,  0644, 0);
	sem_suggar = sem_open("/suggar", O_CREAT,  0644, 0);
	sem_agent = sem_open("/agent", O_CREAT,  0644, 1);

	printf("************************************\n");
	printf("Agent Initialized\n");
	printf("Coffee Lover 1 has : Coffee\n");
	printf("Coffee Lover 2 has : Milk\n");	
	printf("Coffee Lover 3 has : Suggar\n");
	printf("************************************\n\n");

	pid1 = fork();

	if(pid1 == 0){
		for(i=0;i<n;i++){
			sem_wait(sem_coffee);
			printf("Coffee Lover 1 Drinks Coffee\n");
			sem_post(sem_agent);
		}
	}else{
		pid2 = fork();
		if(pid2 == 0){
			for(i=0;i<n;i++){
				sem_wait(sem_milk);
				printf("Coffee Lover 2 Drinks Coffee\n");
				sem_post(sem_agent);
			}
		}else{
			pid3 = fork();
			if(pid3 == 0){
				for(i=0;i<n;i++){
					sem_wait(sem_suggar);
					printf("Coffee Lover 3 Drinks Coffee\n");
					sem_post(sem_agent);
				}
			}else{
				srand((unsigned) time(&time_var));
				for(i=0;i<n;i++){
					sem_wait(sem_agent);
					printf("Parent: %d %d\n",getpid(),getppid());
					
					ing1 = rand()%3;
					while((ing2 = rand()%3)==ing1);
					printf("Ingridients Generated: %d %d\n",ing1,ing2);
					if((ing1 == 0 && ing2 == 1) || (ing1 ==1 && ing2 == 0)){
						printf("Agent Produced : Coffee and Milk\n");
						sem_post(sem_suggar);
					}else if((ing1 == 0 && ing2 == 2) || (ing1 == 2 && ing2 == 0)){
						printf("Agent Produced : Coffee and Suggar\n");
						sem_post(sem_milk);

					}else{
						printf("Agent Produced : Milk and suggar\n");
						sem_post(sem_coffee);
					}
				}
			}
		}
	}
	unlink_semaphores();
}

int main(int argc,char *argv[]){
	int n=5;
	if(argc == 2)
		n=atoi(argv[1]);
	controller(n);	
	return 0;
}
