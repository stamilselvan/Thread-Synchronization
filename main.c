
#define _GNU_SOURCE /*! for CPU_ZERO() */
#include <stdio.h>
#include <stdlib.h>
#include <sched.h> /*! for sched_getcpu () */
#include <unistd.h> /*! for sleep() */
#include <pthread.h>
#include <string.h>
#include <time.h>

#define NUM_THREADS 4
pthread_mutex_t mutex;
unsigned int readyThreads = 0, awakenedThreads = 0;
pthread_cond_t cv_count;

enum {
	CPU_AFFINITY_OK,
	CPU_AFFINITY_ERROR,
	PTHREAD_ERROR,
};

void wait_for_all_threads() {

	while (awakenedThreads % NUM_THREADS != 0)
		sleep(0.001);

	pthread_mutex_lock(&mutex);
	readyThreads += 1;

	if (readyThreads == NUM_THREADS) {
		pthread_cond_broadcast(&cv_count);
	}

	/*! Check again */
	while (readyThreads != NUM_THREADS) {
		pthread_cond_wait(&cv_count, &mutex);
	}

	/*! wake up one by one */
	++awakenedThreads;
	if (awakenedThreads == NUM_THREADS) {
		awakenedThreads = 0;
		readyThreads = 0;
	}

	pthread_mutex_unlock(&mutex);
}

int set_core_affinity(int core_id)
{
	int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
	if (core_id < 0 || core_id >= num_cores)
		return CPU_AFFINITY_ERROR;

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core_id, &cpuset);

	pthread_t current_thread = pthread_self();
	if(pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset) != 0) return CPU_AFFINITY_ERROR;

	return CPU_AFFINITY_OK;
}

void* run_threads(void *id){

	unsigned int cpuid = sched_getcpu();
	unsigned int tid = *(unsigned int *) id;

	time_t rawtime;
	struct tm * timeinfo;

	printf("Thread:%d running on core:%d \n", tid, cpuid);
	sleep(tid*2);
	wait_for_all_threads(); // Sync point 1

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("Thread:%d  Sync1 time: %s", tid, asctime(timeinfo));

	sleep(tid*3);
	wait_for_all_threads(); // Sync point 2

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("Thread:%d  Sync2 time: %s", tid, asctime(timeinfo));

	return NULL;
}

int main()
{
	pthread_t threads[NUM_THREADS];
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cv_count, NULL);
	int ret, ii;
	int tid[NUM_THREADS];

	for(ii=0; ii<NUM_THREADS; ii++){
		tid[ii] = ii;

		if (set_core_affinity(ii) != CPU_AFFINITY_OK)
			return CPU_AFFINITY_ERROR;

		ret = pthread_create(&threads[ii], NULL, run_threads, (void *)&tid[ii]);
		if (ret){
			printf("Error creating pthread %d\n", ret);
			return PTHREAD_ERROR;
		}
	}

	for(ii=0; ii<NUM_THREADS; ii++)
		pthread_join(threads[ii], NULL);

	printf("Threads over \n");
	return 0;

}
