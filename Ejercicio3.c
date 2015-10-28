#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define SECTIONS 10
#define ROBOTS 5

pthread_mutex_t * mutex_current_weight;
pthread_cond_t * allow_robot;

void * start_shopping(void *);

int * current_weight;
int * max_weight;

int main(){
	srand(time(NULL));
	current_weight = (int *) malloc(SECTIONS * sizeof(int));
	max_weight = (int *) malloc(SECTIONS * sizeof(int));
	mutex_current_weight = (pthread_mutex_t *) malloc(SECTIONS * sizeof(pthread_mutex_t));
	allow_robot = (pthread_cond_t *) malloc(SECTIONS * sizeof(pthread_cond_t));
	pthread_t * robots = (pthread_t *) malloc(ROBOTS * sizeof(pthread_t));

	int i,j,k;
	for(k = 0; k < SECTIONS; ++k){
		*(current_weight + k) = 0;
		*(max_weight + k) = rand() % 4 + 5; // Max. weight between (5-8)
		pthread_mutex_init(mutex_current_weight + k,NULL);
		pthread_cond_init(allow_robot + k,NULL);
	}
	
	for(i = 0; i < ROBOTS; ++i)
		pthread_create(robots + i,NULL,start_shopping,(void *) i+1);
	

	for(j = 0; j < ROBOTS; ++j)
		pthread_join(*(robots+j),NULL);

	free(robots);
	free(allow_robot);
	free(mutex_current_weight);
	free(max_weight);
	free(current_weight);
	
	return 0;
}

void * start_shopping(void * arg){
	int sections = rand() % SECTIONS + 1;
	int weight = rand() % 3 + 1; // Robot's weight between 1-3
	int i;
	int robotID = (int) arg;
	int ready;

	for(i = 0; i <= sections; ++i){
		if(i != sections){
			ready = 0;
		pthread_mutex_lock(mutex_current_weight+i);
		// Waiting to move to next Section
		while(ready == 0){
		if((*(current_weight + i) + weight) > *(max_weight + i)){
			printf("Robot %d is waiting to enter section %d\n",robotID,i+1);
			pthread_cond_wait(allow_robot + i,mutex_current_weight+i);
			if((*(current_weight + i) + weight) <= *(max_weight + i)){
				ready = 1;
				//printf("Current weight at section %d is %d \n",i+1,*(current_weight + i));
			}
			}
		else
			ready = 1;
		}
		if(i != 0){
			*(current_weight + (i-1)) -= weight;
			pthread_cond_broadcast(allow_robot + (i-1));
			printf("Robot %d left section %d \n",robotID,i);
			//printf("Current weight at section %d is %d \n",i,*current_weight);
		}
	}
		if(i == sections){
			pthread_mutex_lock(mutex_current_weight+(i-1));
				*(current_weight + (i-1)) -= weight;
			pthread_mutex_unlock(mutex_current_weight+(i-1));
			pthread_cond_broadcast(allow_robot + (i-1));
			printf("Robot %d left section %d \n",robotID,i);
			//printf("Current weight at section %d is %d \n",i,*(current_weight + (i-1)));
			break;
		}

		printf("Robot %d entered section %d \n",robotID,i+1);
		*(current_weight + i) += weight;
		//printf("Current weight at section %d is %d \n",i+1,*(current_weight + i));

	pthread_mutex_unlock(mutex_current_weight+i);
		printf("Robot %d is shopping at section %d \n",robotID,i+1);
		sleep(rand() % 3 + 1);
	}
	printf("------- Robot %d has finished shopping ------- \n",robotID);
	pthread_exit(NULL);
}