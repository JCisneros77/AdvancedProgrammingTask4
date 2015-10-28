#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

void * mujer_quiere_entrar();
void * hombre_quiere_entrar();
void mujer_sale();
void hombre_sale();

pthread_mutex_t mutex_bano = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_hombres = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_mujeres = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t permitir_hombres = PTHREAD_COND_INITIALIZER;
pthread_cond_t permitir_mujeres = PTHREAD_COND_INITIALIZER;

int ocupado, mujeres_espera,mujeres_dentro,hombres_espera,hombres_dentro;

int main(){
	srand(time(NULL));	
	mujeres_espera = 0;
	hombres_espera = 0;
	mujeres_dentro = 0;
	hombres_dentro = 0;
	ocupado = 2; // Hombres: 0, Mujeres:1 , Vacio: 2

	pthread_t * cola_entrar = (pthread_t *) malloc(20 * sizeof(pthread_t));

	// Crear hombres y mujeres
	printf("Sanitario Vacio\n");
	int i;
	for(i = 0; i < 20; ++i){
		sleep(rand() % 5);
		if(rand() % 2 == 0)
			pthread_create(cola_entrar+i,NULL,hombre_quiere_entrar,NULL);
		else
			pthread_create(cola_entrar+i,NULL,mujer_quiere_entrar,NULL);
	}
	// Esperar a los threads
	int j;
	for(j = 0; j < 20; ++j)
		pthread_join(*(cola_entrar+j),NULL);

	free(cola_entrar);
	return 0;
}

void * mujer_quiere_entrar(){
	pthread_mutex_lock(&mutex_mujeres);
	mujeres_espera++;
	printf("Llega una mujer (%d en espera)\n",mujeres_espera);
	pthread_mutex_unlock(&mutex_mujeres);

	pthread_mutex_lock(&mutex_bano);
	if(ocupado == 0)
		pthread_cond_wait(&permitir_mujeres,&mutex_bano);

	if(ocupado == 2 || ocupado == 1){
		ocupado = 1;
		pthread_mutex_lock(&mutex_mujeres);
			mujeres_espera--;
			mujeres_dentro++;
			printf("Entra una mujer (%d en espera)\n",mujeres_espera);
			printf("Sanitario ocupado por mujeres\n");
		pthread_mutex_unlock(&mutex_mujeres);
	}
	pthread_mutex_unlock(&mutex_bano);
	sleep(rand() % 5);
	mujer_sale();

	pthread_exit(NULL);
}
void * hombre_quiere_entrar(){
	pthread_mutex_lock(&mutex_hombres);
	hombres_espera++;
	printf("Llega un hombre (%d en espera)\n",hombres_espera);
	pthread_mutex_unlock(&mutex_hombres);

	pthread_mutex_lock(&mutex_bano);
	if(ocupado == 1)
		pthread_cond_wait(&permitir_hombres,&mutex_bano);

	if(ocupado == 2 || ocupado == 0){
		ocupado = 0;
		pthread_mutex_lock(&mutex_hombres);
			hombres_espera--;
			hombres_dentro++;
			printf("Entra un hombre (%d en espera)\n",hombres_espera);
			printf("Sanitario ocupado por hombres\n");
		pthread_mutex_unlock(&mutex_hombres);
	}
	pthread_mutex_unlock(&mutex_bano);
	sleep(rand() % 20);
	hombre_sale();

	pthread_exit(NULL);
}
void mujer_sale(){
	pthread_mutex_lock(&mutex_mujeres);
	mujeres_dentro--;
	printf("Sale una mujer\n");
	if(mujeres_dentro == 0){
		pthread_mutex_lock(&mutex_bano);
		ocupado = 2;
		pthread_cond_broadcast(&permitir_hombres);
		printf("Sanitario vacio\n");
		pthread_mutex_unlock(&mutex_bano);
	}
	pthread_mutex_unlock(&mutex_mujeres);

}
void hombre_sale(){
	pthread_mutex_lock(&mutex_hombres);
	hombres_dentro--;
	printf("Sale un hombre\n");
	if (hombres_dentro == 0){
		pthread_mutex_lock(&mutex_bano);
		ocupado = 2;
		pthread_cond_broadcast(&permitir_mujeres);
		printf("Sanitario vacio\n");
		pthread_mutex_unlock(&mutex_bano);
	}
	pthread_mutex_unlock(&mutex_hombres);
}

