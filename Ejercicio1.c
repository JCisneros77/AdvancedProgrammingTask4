#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>

void mining(char*);
void eating(char*,int);
void feed_dwarf(int);

pthread_mutex_t * mutex_chairs;
pthread_cond_t * food_on_table;
pthread_mutex_t mutex_snowWhite = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait_for_snowWhite = PTHREAD_COND_INITIALIZER;
pthread_cond_t * allow_sitting;

void * snowWhite();
void * dwarf(void *);
int * chair_occupied;
int * served;
pthread_t snow;
int is_snowWhite_Available;

int main(){
	const char * dwarf_names[7] = {"Doc", "Grumpy", "Happy", "Sleepy", "Bashful", "Sneezy", "Dopey"};
	srand(time(NULL));

	is_snowWhite_Available = 1;
	chair_occupied = (int *) malloc(4 * sizeof(int));
	served = (int *) malloc(4 * sizeof(int));
	pthread_t * dwarfs = (pthread_t *) malloc(7 * sizeof(pthread_t));
	mutex_chairs = (pthread_mutex_t *) malloc(4 * sizeof(pthread_mutex_t));
	food_on_table = (pthread_cond_t *) malloc(4 * sizeof(pthread_cond_t));
	allow_sitting = (pthread_cond_t *) malloc(4 * sizeof(pthread_cond_t));

	int i,j,k;
	for(k = 0; k < 4; ++k){
		*(chair_occupied + k) = 0;
		*(served + k) = 0;
		pthread_mutex_init(mutex_chairs + k,NULL);
		pthread_cond_init(food_on_table + k,NULL);
	}

	pthread_create(&snow,NULL,snowWhite,NULL);

	for(i = 0; i < 7; ++i)
		pthread_create(dwarfs + i, NULL, dwarf,(void *) *(dwarf_names+i));
	
	for(j = 0; j < 7; ++j)
		pthread_join(*(dwarfs + j),NULL);

	pthread_join(snow,NULL);

	free(chair_occupied);
	free(food_on_table);
	free(dwarfs);
	free(mutex_chairs);
	free(allow_sitting);
	free(served);

	return 0;
}

void * snowWhite(){
	signal(SIGUSR1,feed_dwarf);
	int i,someone_on_chair;
	while(1){
		someone_on_chair = 0;
	for(i = 0; i < 4; ++i){
		pthread_mutex_lock(mutex_chairs + i);
			if(*(chair_occupied + i) == 1){
					someone_on_chair = 1;
			}
		pthread_mutex_unlock(mutex_chairs + i);
	}
		if(someone_on_chair == 0){
			pthread_mutex_lock(&mutex_snowWhite);
			is_snowWhite_Available = 0;
			pthread_mutex_unlock(&mutex_snowWhite);
			printf("Snow White went out for a walk.\n");
			sleep(rand() % 5 + 1);
			pthread_mutex_lock(&mutex_snowWhite);
				is_snowWhite_Available = 1;
			pthread_mutex_unlock(&mutex_snowWhite);
			pthread_cond_broadcast(&wait_for_snowWhite);
			printf("Snow White came back from her walk.\n");
		}

	}
}

void * dwarf(void * arg){
	char * dwarf_name = (char *) arg;
	int i,ate;
	while(1){
		ate = 0;
		mining(dwarf_name);
		while(ate == 0){
			for(i = 0; i < 4; ++i){
				pthread_mutex_lock(mutex_chairs + i);
					if(*(chair_occupied + i) == 0){ // Free chair
						*(chair_occupied + i) = 1;
						pthread_mutex_lock(&mutex_snowWhite);
							if(is_snowWhite_Available == 0){
								printf("%s is waiting for Snow White to come back from her walk.\n",dwarf_name);
								pthread_cond_wait(&wait_for_snowWhite,&mutex_snowWhite);
							}
						pthread_mutex_unlock(&mutex_snowWhite);
						pthread_mutex_unlock(mutex_chairs + i);
						pthread_kill(snow,SIGUSR1);
						pthread_mutex_lock(mutex_chairs + i);
						printf("%s is waiting for Snow White to serve his plate on chair %d.\n",dwarf_name,i + 1);
						pthread_cond_wait(food_on_table + i,mutex_chairs+i);
						eating(dwarf_name,i);
						ate = 1;
						*(served + i) = 0;
							*(chair_occupied + i) = 0;
						printf("%s left chair %d.\n",dwarf_name,i+1);
						break;
					}
				pthread_mutex_unlock(mutex_chairs + i);
			}
			pthread_mutex_unlock(mutex_chairs + i);
		}
	}
}

void mining(char * dwarf_name){
	printf("%s is mining.\n",dwarf_name);
	sleep(rand() % 5 + 1);
	printf("%s has finished mining.\n",dwarf_name);
}

void eating(char * dwarf_name,int chair){
	printf("%s is eating in chair %d. \n",dwarf_name,chair+1);
	sleep(rand() % 5 + 1);
}

void feed_dwarf(int sig_id){
	int i;
	for (i = 0; i < 4; ++i){
		pthread_mutex_lock(mutex_chairs + i);
			if(*(chair_occupied + i) == 1 && *(served + i) == 0){
				printf("Snow White is serving a plate on chair %d.\n",i+1);
				*(served + i) = 1;
				pthread_mutex_unlock(mutex_chairs + i);
				pthread_cond_broadcast(food_on_table + i);
			}
			else
				pthread_mutex_unlock(mutex_chairs + i);
	}

}
