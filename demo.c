#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5

void *worker(void *arguments);

int main(void) {
  pthread_t threads[NUM_THREADS];
  int thread_args[NUM_THREADS];
  int i, rc;
  
  //create all threads one by one
  for (i = 0; i < NUM_THREADS; i++) {
    printf("In main: Creating thread %d.\n", i);
    thread_args[i] = i;
    rc = pthread_create(&threads[i], NULL, worker, &thread_args[i]);
    assert(!rc);
  }

  printf("In main: All threads are created.\n");

  //wait for each thread to complete
  for (i = 0; i < NUM_THREADS; i++) {
    rc = pthread_join(threads[i], NULL);
    assert(!rc);
    printf("In main: Thread %d has ended.\n", i);
  }

  printf("Main program has ended.\n");
  return 0;
}

void *worker(void *arguments) {
  int index = *((int *) arguments);
  int sleep_time = 1 + rand() % NUM_THREADS;

  printf("Thread %d: Started.\n", index);
  printf("Thread %d: Will be sleeping for %d seconds.\n", index, sleep_time);

  sleep(sleep_time);

  printf("Thread %d: Ended.\n", index);

  return NULL;
}
