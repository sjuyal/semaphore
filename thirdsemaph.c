#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>

sem_t mutex;

int students;
int courses;

pthread_t ts[10000];

int course_group[10000];
int course_branch[10000][4];
int student_branch[10000];
int student_course[10000][8];

int total=0;

int Random (int size) {
	int i, n;
	static int x = 0;
	static int A[10000];

	if (size >= 0) {
        	for (i=0;i<size;i++)
			A[i]=i;
        	x = size;
    	}
	if (x == 0)
		return -1;	
	n = rand() % x;
    	i = A[n];
    	A[n] = A[x-1];
    	x--;
    	
    	return i;
}


void *AllocationFunc(void *val)
{
	uint32_t tid = (uint32_t)val;
	int i=0;
	//printf("val:%d",tid);
	int bid=student_branch[tid];
	int cid;	
	int count=0;
	int check[4]={1,1,1,1};
	for(i=0;i<8;i++){
		sem_wait(&mutex);
		cid=student_course[tid][i];
		//printf("cid:%d bid:%d",cid,bid);
		if(course_branch[cid][bid]>=1){
			
			if(check[course_group[cid]]){
				//printf("%d Here !",tid);
				course_branch[cid][bid]--;
				count++;
				student_course[tid][i]=-1;
				check[course_group[cid]]=0;	
			}
		}
		sem_post(&mutex);
		if(count==4)
			break;
	}
	
	if(count!=4){
		sem_wait(&mutex);
		printf("Student %d Not Allocated !!\n",tid);
		total++;
		sleep(1);
		sem_post(&mutex);
	}
	
	return NULL;
}


int main(int argc, char *argv[])
{
  	uint32_t i,j=0;

    	sem_init(&mutex, 0, 1);	

	students=atoi(argv[1]);
	courses=atoi(argv[2]);
	
	printf("Total:\nStudents:%d Courses:%d\n\n",students,courses);

	for(i=0;i<courses;i++){
		course_group[i]=i%4;
		course_branch[i][0]=course_branch[i][1]=course_branch[i][3]=12;
		course_branch[i][2]=24;	
	}	
	
	srand (time (NULL));
	for(i=0;i<students;i++){
		student_branch[i]=rand()%4;
		int x=Random(courses);
		for(j=0;j<8;j++){
			student_course[i][j]=x;
			x=Random(-1);
		}	
	}   	

	for (i=0; i<students; i++){
		pthread_create(&ts[i], NULL, AllocationFunc, (void *)i);
    	}
	
	for (i=0; i<students; i++){
		pthread_join(ts[i], NULL);
    	}
	
	printf("\nTotal number of students not allocated: %d\n",total);


	FILE * pFile;
	pFile = fopen ("allocation.txt","w");
	
	printf("\n");
	fprintf(pFile,"Courses  :   B1  B2  B3  B4\n\n");	
	for(i=0;i<courses;i++){
		fprintf(pFile,"Course[%d]: %3d %3d %3d %3d\n",i+1,course_branch[i][0],course_branch[i][1],course_branch[i][2],course_branch[i][3]);	
	}

	fclose (pFile);

	return 0;
}
