
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "aq.h"

#include "aux.h"

/** 
 * Concurrent program that sends and receives a few integer messages 
 * in order to demonstrate the basic use of the thread-safe Alarm Queue Library
 *
 * By using sleeps we (try to) control the scheduling of the threads in
 * order to obtain specific execution scenarios.  But there is no guarantee.
 *
 */

static AlarmQueue q;

void * producer (void * arg) {
  int id = *((int *)arg);
  msleep(rand() % 200 + 100);
  printf("Producer %d: Sending normal message %d\n", id, id * 10 + 1);
  put_normal(q, id * 10 + 1);
  msleep(rand() % 200 + 100);
  printf("Producer %d: Sending ALARM message %d\n", id, id * 100 + 100);
  put_alarm(q, id * 100 + 100);    // Add alarm-msg to queue
  msleep(rand() % 200 + 100);
  printf("Producer %d: Sending normal message %d\n", id, id * 10 + 2);
  put_normal(q, id * 10 + 2);
  msleep(rand() % 200 + 100);
  printf("Producer %d: Sending normal message %d\n", id, id * 10 + 3);
  put_normal(q, id * 10 + 3);
  msleep(rand() % 200 + 100);
  printf("Producer %d: Sending ALARM message %d\n", id, id * 100 + 101);
  put_alarm(q, id * 100 + 101);    // Add second alarm-msg to queue
  msleep(rand() % 200 + 100);
  
  return 0;
}

void * consumer(void * arg) {
  printf("Consumer: Attempting to receive message 1\n");
  get(q);
  printf("Consumer: Attempting to receive message 2\n");
  get(q);
  printf("Consumer: Attempting to receive message 3\n");
  get(q);
  printf("Consumer: Attempting to receive message 4\n");
  get(q);
  printf("Consumer: Attempting to receive message 5\n");
  get(q);

  return 0;
}

int main(int argc, char ** argv) {
    int ret;

  q = aq_create();

  if (q == NULL) {
    printf("Alarm queue could not be created\n");
    exit(1);
  }
  
  pthread_t t1;
  pthread_t t2;
  pthread_t t3;
  pthread_t t4;

  int producer_id1 = 1;
  int producer_id2 = 2;
  int consumer_id1 = 1;
  int consumer_id2 = 2;

  void * res1 = NULL;
  void * res2 = NULL;
  void * res3 = NULL;
  void * res4 = NULL;

  //pthread_t producer_threads[2];
  //pthread_t consumer_threads[2];

  //void *res1;
  //void *res2;
  //void *res3;
  //void *res4;

  printf("----------------\n");

  /* Fork threads */
  pthread_create(&t1, NULL, producer, &producer_id1);
  pthread_create(&t2, NULL, consumer, &consumer_id1);
  pthread_create(&t3, NULL, producer, &producer_id2);
  pthread_create(&t4, NULL, consumer, &consumer_id2);

  //pthread_create(&producer_threads[0], NULL, producer, NULL);
  //pthread_create(&consumer_threads[0], NULL, consumer, NULL);
  //pthread_create(&producer_threads[1], NULL, producer, NULL);
  //pthread_create(&consumer_threads[1], NULL, consumer, NULL);
  
  /* Join with all threads */
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);

  //pthread_join(producer_threads[0], &res1);
  //pthread_join(consumer_threads[0], &res2);
  //pthread_join(producer_threads[1], &res3);
  //pthread_join(consumer_threads[1], &res4);

  printf("----------------\n");
  printf("Threads terminated with %ld, %ld, %ld, %ld\n", (uintptr_t) res1, (uintptr_t) res2, (uintptr_t) res3, (uintptr_t) res4);

  print_sizes(q);

  aq_destroy(q);
  return 0;
}

