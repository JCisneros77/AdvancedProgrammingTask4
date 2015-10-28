#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>

#define UADS 5
#define WARNING_MEASUREMENT 10

pthread_mutex_t * mutex_measurements;
pthread_mutex_t * mutex_alerts;
pthread_mutex_t mutex_file = PTHREAD_MUTEX_INITIALIZER;

pthread_t UCR;
pthread_t * list_of_UADS;
int * measurements;
int * alerts;
FILE * f;

void * start_reading(void * attr);
void * start_monitoring();
void write_measurements(int sig_id);
void write_alert(int sig_id); 
void quit (int sig_id);

int main(){
	printf("------- To Quit press Ctrl-C --------- \n");
	signal(SIGINT,quit);
	srand(time(NULL));
	list_of_UADS = (pthread_t *) malloc(UADS * sizeof(pthread_t));
	mutex_measurements = (pthread_mutex_t *) malloc(UADS * sizeof(pthread_mutex_t));
	mutex_alerts = (pthread_mutex_t *) malloc(UADS * sizeof(pthread_mutex_t));
	measurements = (int *) malloc (UADS * sizeof(int));
	alerts = (int *) malloc (UADS * sizeof(int));

	f = fopen("measurements.txt", "w+");
	int i,j;
	pthread_create(&UCR,NULL,start_monitoring,NULL);
	for(i = 0; i < UADS; ++i){
		pthread_create(list_of_UADS + i,NULL,start_reading,(void *) i);
		pthread_mutex_init(mutex_measurements + i,NULL);
		*(measurements + i) = 0;
		*(alerts + i) = 0;
	}

	for(j = 0; j < UADS; ++j)
		pthread_join(*(list_of_UADS+j),NULL);

	fclose(f);
	free(list_of_UADS);
	free(mutex_measurements);
	free(mutex_alerts);
	free(measurements);
	free(alerts);

	return 0;
}

void quit(int sig_id){
	fclose(f);
	free(list_of_UADS);
	free(mutex_measurements);
	free(mutex_alerts);
	free(measurements);
	free(alerts);
	exit(0);
}

void * start_monitoring(){
		signal(SIGUSR1,write_alert);
		signal(SIGALRM,write_measurements);
		alarm(rand() % 2 + 4);
	while(1){

	}
	pthread_exit(NULL);
}

void * start_reading(void * attr){
	int measurement;
	int UAD = (int) attr;

	while(1){
		measurement = rand() % (WARNING_MEASUREMENT + 2) + 1;
		pthread_mutex_lock(mutex_measurements + UAD);
			*(measurements + UAD) = measurement;
			if(measurement >= WARNING_MEASUREMENT){
				pthread_mutex_lock(mutex_alerts + UAD);
					*(alerts + UAD) = 1;
				pthread_mutex_unlock(mutex_alerts + UAD);
				pthread_kill(UCR,SIGUSR1);
			}
		pthread_mutex_unlock(mutex_measurements + UAD);
		sleep(2);
	}
	pthread_exit(NULL);

}

void write_measurements(int sig_id){
	pthread_mutex_lock(&mutex_file);
		if (f == NULL)
		{
    printf("Error opening file!\n");
    return;
		}
	int i;
	for(i = 0; i < UADS; ++i){
		pthread_mutex_lock(mutex_measurements + i);
		printf("Writing measurements\n");
		fprintf(f, "The latest reading on UAD %d is: %d.\n", i + 1,*(measurements + i));
		pthread_mutex_unlock(mutex_measurements + i);
	}
	pthread_mutex_unlock(&mutex_file);

}
void write_alert(int sig_id){
	pthread_mutex_lock(&mutex_file);
		if (f == NULL)
		{
    printf("Error opening file!\n");
    return;
		}
	int i;
	for(i = 0; i < UADS; ++i){
		pthread_mutex_lock(mutex_measurements + i);
			pthread_mutex_lock(mutex_alerts + i);
				if(*(alerts + i) == 1){
					printf("Writing alert\n");
					fprintf(f, "The UAD %d raised an alert: (Measurement is %d) \n", i + 1,*(measurements+i));
					*(alerts+i) = 0;
				}
			pthread_mutex_unlock(mutex_alerts + i);
		pthread_mutex_unlock(mutex_measurements + i);
	}
	pthread_mutex_unlock(&mutex_file);
	alarm(rand() % 2 + 4);
}