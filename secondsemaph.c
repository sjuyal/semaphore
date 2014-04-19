#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>

sem_t sem_geek;
sem_t sem_nongeek;
sem_t sem_singer;
sem_t mutex;
sem_t boarding;

int geeks;
int nongeeks;
int singers;

int geekcount;
int nongeekcount;
int singercount;

pthread_t tg;
pthread_t tn;
pthread_t ts;
pthread_t tm;


void *BoardBridge(void *val)
{
	while(1){
		uint32_t tid = (uint32_t)val;
		//printf("Site Thread %d waiting.\n", tid);
		//fflush(stdout);

		if(geekcount+nongeekcount+singercount<4)
			continue;	
		if(geekcount>=4){
			sem_wait(&mutex);
			geekcount=geekcount-4;
			printf("Traveling--> Geek:4 NonGeeks:0 Singer:0\n");
			sem_post(&mutex);
			sem_post(&sem_geek);
			sem_post(&sem_geek);
			sem_post(&sem_geek);
			sem_post(&sem_geek);
		}
		else if(nongeekcount>=4){
			sem_wait(&mutex);
			nongeekcount=nongeekcount-4;
			sem_post(&mutex);
			printf("Traveling--> Geek:0 NonGeeks:4 Singer:0\n");
			sem_post(&sem_nongeek);
			sem_post(&sem_nongeek);
			sem_post(&sem_nongeek);
			sem_post(&sem_nongeek);
		}
		else if(geekcount==0 && nongeekcount==3 && singercount>=1){
			sem_wait(&mutex);
			nongeekcount=nongeekcount-3;	
			singercount--;	
			printf("Traveling--> Geek:0 NonGeeks:3 Singer:1\n");
			sem_post(&mutex);
			sem_post(&sem_nongeek);
			sem_post(&sem_nongeek);
			sem_post(&sem_nongeek);
			sem_post(&sem_singer);	
		}
		else if(nongeekcount==0 && geekcount==3 && singercount>=1){
			sem_wait(&mutex);
			geekcount=geekcount-3;	
			singercount--;
			printf("Traveling--> Geek:3 NonGeeks:0 Singer:1\n");
			sem_post(&mutex);
			sem_post(&sem_geek);
			sem_post(&sem_geek);
			sem_post(&sem_geek);
			sem_post(&sem_singer);	
		}
		else if(geekcount==2 && nongeekcount>=2){
			sem_wait(&mutex);
			nongeekcount=nongeekcount-2;	
			geekcount=geekcount-2;
			printf("Traveling--> Geek:2 NonGeeks:2 Singer:0\n");
			sem_post(&mutex);
			sem_post(&sem_geek);
			sem_post(&sem_geek);
			sem_post(&sem_nongeek);
			sem_post(&sem_nongeek);			
		}
		else if(nongeekcount==2 && geekcount>=2){
			sem_wait(&mutex);
			nongeekcount=nongeekcount-2;	
			geekcount=geekcount-2;
			printf("Traveling--> Geek:2 NonGeeks:2 Singer:0\n");
			sem_post(&mutex);
			sem_post(&sem_geek);
			sem_post(&sem_geek);
			sem_post(&sem_nongeek);
			sem_post(&sem_nongeek);			
		}
		else if(geekcount==2 && nongeekcount==1 && singercount>=1){
			sem_wait(&mutex);
			nongeekcount--;	
			geekcount=geekcount-2;
			singercount--;
			printf("Traveling--> Geek:2 NonGeeks:1 Singer:1\n");
			sem_post(&mutex);
			sem_post(&sem_geek);
			sem_post(&sem_geek);
			sem_post(&sem_nongeek);
			sem_post(&sem_singer);	
		}
		else if(geekcount==1 && nongeekcount==2 && singercount>=1){
			sem_wait(&mutex);
			geekcount--;	
			nongeekcount=nongeekcount-2;
			singercount--;
			printf("Traveling--> Geek:1 NonGeeks:2 Singer:1\n");
			sem_post(&mutex);
			sem_post(&sem_geek);
			sem_post(&sem_nongeek);
			sem_post(&sem_nongeek);
			sem_post(&sem_singer);	
		}
		else{
			continue;		
		}
		sem_wait(&boarding);
		sem_wait(&boarding);
		sem_wait(&boarding);
		sem_wait(&boarding);
	}
	return NULL;
}

void GoBridge(){
	sem_wait(&mutex);
	sem_post(&boarding);
	sem_post(&mutex);
}

void *GeekArrives(void *val)
{
	uint32_t tid = (uint32_t)val;
	sem_wait(&mutex);
	geekcount++;
	sem_post(&mutex);
	//printf("Geek %d has arrived and waiting\n",tid);
	sem_wait(&sem_geek);
	GoBridge();
	pthread_exit(0);
	return NULL;
}

void *GeekFunc(void *val)
{
	uint32_t tid = (uint32_t)val;
	int i=0;
	int usecs=1000000;
	
	for(i=1;i<=tid;i++){
		int wait=0;
		wait=usecs*(rand()%5);
		//printf("--%d--",wait);
		usleep(wait);
		pthread_create(&tg, NULL, GeekArrives, (void *)i);	
	}

	return NULL;
}

void *NonGeekArrives(void *val)
{
	uint32_t tid = (uint32_t)val;
	sem_wait(&mutex);
	nongeekcount++;
	sem_post(&mutex);
	//printf("Non Geek %d has arrived and waiting\n",tid);
	sem_wait(&sem_nongeek);
	GoBridge();
	pthread_exit(0);
	return NULL;
}


void *NonGeekFunc(void *val)
{
	uint32_t tid = (uint32_t)val;
	int i=0;
	unsigned int usecs=1000000;
	
	for(i=1;i<=tid;i++){
		int wait=0;
		wait=usecs*(rand()%5);
		usleep(wait);
		pthread_create(&tg, NULL, NonGeekArrives, (void *)i);	
	}

	return NULL;
}

void *SingerArrives(void *val)
{
	uint32_t tid = (uint32_t)val;
	sem_wait(&mutex);
	singercount++;
	sem_post(&mutex);
	//printf("Singer %d has arrived and waiting\n",tid);
	sem_wait(&sem_singer);
	GoBridge();
	pthread_exit(0);
	return NULL;
}

void *SingerFunc(void *val)
{
	uint32_t tid = (uint32_t)val;
	int i=0;
	unsigned int usecs=1000000;
	
	
	for(i=1;i<=tid;i++){
		int wait=0;
		wait=usecs*(rand()%5);
		usleep(wait);
		pthread_create(&tg, NULL, SingerArrives, (void *)i);	
	}

	return NULL;
}


int main(int argc, char *argv[])
{
   	uint32_t i=0;

    	/* suppress warnings */
    	
	sem_init(&mutex, 0, 1);	
	sem_init(&sem_geek,0,0);
	sem_init(&sem_nongeek,0,0);
	sem_init(&sem_singer,0,0);
	sem_init(&boarding,0,0);

	geeks=atoi(argv[1]);
	nongeeks=atoi(argv[2]);
	singers=atoi(argv[3]);
	printf("At Arrival:\nGeeks:%d Non-Geeks:%d Singers:%d \n\n",geeks,nongeeks,singers);
    	
	
	pthread_create(&tg, NULL, GeekFunc, (void *)geeks);
	pthread_create(&tn, NULL, NonGeekFunc, (void *)nongeeks);
	pthread_create(&ts, NULL, SingerFunc, (void *)singers);

	pthread_create(&tm, NULL, BoardBridge, (void *)i);

	pthread_join(tg,NULL);
	pthread_join(tn,NULL);
	pthread_join(ts,NULL);
	printf("\nRemaining:\nGeeks:%d NonGeeks:%d Singer:%d\n\n",geekcount,nongeekcount,singercount);
    	return 0;
}
