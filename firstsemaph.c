#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>

sem_t sem_hyd[10000];
sem_t sem_oxy[10000];
sem_t sem_site;
sem_t mutex;
sem_t printmutex;

int hydrogen;
int oxygen;
int hc;
int oc;
int sites;
int energy;

int sitearray[10000];

pthread_t th[1000];
pthread_t to[1000];
pthread_t ts[1000];

/* the thread function */
void *SiteFunc(void *site)
{
	while(1){
		uint32_t tid = (uint32_t)site;
		//printf("Site Thread %d waiting.\n", tid);
		fflush(stdout);
		sem_wait(&sem_site);
		sem_wait(&mutex);
		int critical=0;

		if(tid==1 && sitearray[tid]==0){
			sitearray[2]++;
			sitearray[tid]=1;
			critical=1;
		}
		if(tid==sites && sitearray[tid]==0){
			sitearray[sites-1]++;
			sitearray[sites]=1;
			critical=1;
		}
		if(sitearray[tid]==0){
			sitearray[tid-1]++;		
			sitearray[tid+1]++;
			sitearray[tid]=1;		
			critical=1;
		}
	
		if(!critical){
			sem_post(&sem_site);
			sem_post(&mutex);
			continue;
		}

		if(critical){
			//printf("Thread %d : Oxygen: %d Hydrogen: %d\n",tid,oc,hc);
			if(hc<=1){
				if(tid==1){
					sitearray[2]--;
					sitearray[tid]=0;
				}
				else if(tid==sites){
					sitearray[sites-1]--;
					sitearray[sites]=0;
				}
				else {
					sitearray[tid-1]--;		
					sitearray[tid+1]--;		
					sitearray[tid]=0;
				}
				sem_post(&sem_site);
				sem_post(&mutex);
				pthread_exit(0);
			}
			if(oc<=0){
				if(tid==1){
					sitearray[2]--;
					sitearray[tid]=0;
				}
				else if(tid==sites){
					sitearray[sites-1]--;
					sitearray[sites]=0;
				}
				else {
					sitearray[tid-1]--;		
					sitearray[tid+1]--;		
					sitearray[tid]=0;
				}
				sem_post(&sem_site);
				sem_post(&mutex);
				pthread_exit(0);
			}
			int store1=hc;
			sem_post(&sem_hyd[hc--]);
			int store2=hc;
			sem_post(&sem_hyd[hc--]);
			int store3=oc;
			sem_post(&sem_oxy[oc--]);
			
			pthread_join(th[store1],NULL);
			pthread_join(th[store2],NULL);
			pthread_join(to[store3],NULL);
			
			//usleep(100);
			sem_wait(&printmutex);
			printf("Site %d is generating: Hydrogen : %d Oxygen : %d ---generating total 1 E Mj of energy \n",tid,store1-2,store3-1);
			sem_post(&printmutex);
			sem_post(&mutex);
			sleep(3);
			sem_wait(&mutex);
			if(tid==1){
				sitearray[2]--;
				sitearray[tid]=0;
			}
			else if(tid==sites){
				sitearray[sites-1]--;
				sitearray[sites]=0;
			}
			else {
				sitearray[tid-1]--;		
				sitearray[tid+1]--;		
				sitearray[tid]=0;
			}
			sem_post(&mutex);
			sem_post(&sem_site);
			unsigned int usecs=1000;
			usleep(usecs);	
		}
	}
	return NULL;
}

void *HydFunc(void *hyd)
{
	uint32_t tid = (uint32_t)hyd;
	//printf("Hydrogen Thread %d waiting.\n", tid);
	sem_wait(&sem_hyd[tid]);
	
	//printf("Hydrogen Thread %d exiting.\n", tid);
	pthread_exit(0);
	return NULL;
}

void *OxyFunc(void *oxy)
{
	uint32_t tid = (uint32_t)oxy;
	//printf("Oxygen Thread %d waiting.\n", tid);
	sem_wait(&sem_oxy[tid]);

	//printf("Oxygen Thread %d exiting.\n", tid);

	return NULL;
}

int main(int argc, char *argv[])
{
   	uint32_t i=0,j=0,k=0;

    	/* suppress warnings */
    	
	sem_init(&mutex, 0, 1);	
	sem_init(&printmutex, 0, 1);		

	hc=hydrogen=atoi(argv[1]);
	oc=oxygen=atoi(argv[2]);
	sites=atoi(argv[3]);
	energy=atoi(argv[4]);
	printf("Hydrogen:-->%d Oxygen:-->%d Sites:-->%d Threshold Energy:-->%d\n",hydrogen,oxygen,sites,energy);
    	
	
    	/* create and detach several threads */
   	for (i = 1; i <=hydrogen; i++){
		sem_init(&sem_hyd[i],0,0);
		pthread_create(&th[i], NULL, HydFunc, (void *)i);
		//pthread_detach(th);
    	}

	for (j = 1; j <=oxygen; j++){
		sem_init(&sem_oxy[j],0,0);
		pthread_create(&to[j], NULL, OxyFunc, (void *)j);
		//pthread_detach(th);
    	}
	sem_init(&sem_site,0,energy);
	for (k = 1; k <=sites; k++){
		pthread_create(&ts[k], NULL, SiteFunc, (void *)k);
		//pthread_detach(th);
    	}
	for(k=1;k<=sites;k++){
		pthread_join(ts[k], NULL);
	}
    	
	/*while(1){
		if (hydrogen<=1 || oxygen <=0){
		    break;
		}
		
        	// release a thread to execute 
        	sem_post(&g_Sem);
        	sleep(1);
    	}*/

    	return 0;
}
