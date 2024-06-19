#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

#define BUF_SIZE 128
#define TRUE 1
#define FALSE 0

// Shared data structures
char buffer[BUF_SIZE];
char data = 'A';
int count = 0;
sem_t sem_Empty;
sem_t sem_Full;
#define M 10 //reader
#define N 20 //writer
int rthread_num[M]={0}, wthread_num[N]={0};
int ExitCondition = 0;

/**
* This thread is responsible for pulling data off of the shared data
* area and processing it using the process_data() API.
*/

void getDataFromShmBuf(char* buff)
{
      //  sem_wait(&sem_Full);
	memcpy(buff,buffer,BUF_SIZE);
	sem_post(&sem_Empty);  /* Signal writer to write */

}

void process_data(char* buff, int bufferSizeInBytes)
{

        int y = 0;
	//Remove from the buffer
	    while( y < bufferSizeInBytes)
			buff[y++]=y*2;
        printf("\n");

}


void *reader_thread(void *arg) 
{
	int thread_num=*((int *) arg); 
    char localBuf[BUF_SIZE] ={0};
	//TODO: Define set-up required
	while(1) 
	{
		//TODO: Define data extraction (queue) and processing

                sem_wait(&sem_Full); /* wait for the writer to write */
		/* Exiting condition when end of data */
		if (ExitCondition && data >= 'Z') {
		   printf("Exiting Rthreadnum %d\n", thread_num);
		   if (thread_num == M-1) sem_post(&sem_Empty); /* Signal waiting writers to exit */
		   sem_post(&sem_Full);
		   break;
		}
		getDataFromShmBuf(localBuf);
		printf("Rthreadnum=%d processing the data %s\n",thread_num, localBuf);

		process_data(localBuf,BUF_SIZE);

	}

	pthread_exit(NULL);

}

/**
* This thread is responsible for pulling data from a device using
* the get_external_data() API and placing it into a shared area
* for later processing by one of the reader threads.
*/

int get_external_data(char *buff, int bufferSizeInBytes)
{
    	int cnt = 0;
    	while(cnt < bufferSizeInBytes)
	{
		//filling the character data to size of buffer
		buff[cnt++] = data;
	}
//	printf("p = %c\n",data);
	return (cnt);
}


int updateDataToShmBuf(char *buff, int bufferSizeInBytes)
{

//	sem_wait(&sem_Empty);
	memset(buffer, 0, BUF_SIZE);
	memcpy(buffer, buff, bufferSizeInBytes);
    	sem_post(&sem_Full); /* Signal reader to read*/

	return bufferSizeInBytes;
}


void *writer_thread(void *arg) {

	int thread_num=*((int *)arg);

    	char localBuf[BUF_SIZE] = {0};
    	int datacount;
	//TODO: Define set-up required
	while(TRUE) 
	{
		//TODO: Define data extraction (device) and storage
	//	printf("WThread_num=%d filling data=%c\n",thread_num,data);
		sem_wait(&sem_Empty); /* wait for reader to read */
		if(data > 'Z' || ExitCondition){
		       printf("Exiting Wthreadnum %d\n", thread_num);
		       ExitCondition = 1;
		       if (thread_num == N-1) sem_post(&sem_Full); /* Signal waiting readers to exit */
    	               sem_post(&sem_Empty);
	      	       break;
		}

	//      get data from the device drivers
		count = get_external_data(localBuf,BUF_SIZE);
		printf("WThread_num=%d filling data=%c\n",thread_num,data);
		datacount = updateDataToShmBuf(localBuf,count);

		data++;
		
	//	count++;
	}

	pthread_exit(NULL);
}

/*
#define M 10 //reader
#define N 20 //writer
*/
int main(int argc, char **argv) 
{

	pthread_t prod[N] = {0};
	pthread_t cons[M] = {0};
	sem_init(&sem_Empty, 0, TRUE);
        sem_init(&sem_Full, 0, FALSE);
	
	int i;
/*	for(i = 0; i < N; i++) 
	{	
		wthread_num[i]=i;
		pthread_create(&prod[i], NULL, writer_thread, &wthread_num[i]);
		sleep(1);
	}*/
	for(i = 0; i < M; i++) 
	{
		rthread_num[i]=i;
		pthread_create(&cons[i], NULL, reader_thread, &rthread_num[i]);
	}

	for(i = 0; i < N; i++)
        {
                wthread_num[i]=i;
                pthread_create(&prod[i], NULL, writer_thread, &wthread_num[i]);
        }



	for(i=0;i<N;i++)
	{
		pthread_join(prod[i],NULL);
	}

	for(i=0;i<M;i++)
	{
		pthread_join(cons[i],NULL);
	}

	sem_destroy(&sem_Empty);
    	sem_destroy(&sem_Full);

	return 1;
}
