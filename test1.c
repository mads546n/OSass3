// Alarm blocking and unblocking test-scenario 

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "aq.h"
#include "aux.h"

static AlarmQueue q;

void *producer1(void *arg) {
    msleep(200);        // Enough time for consumer to start
    printf("Producer 1: Sending first ALARM message (Should succeed immediately)\n");
    put_alarm(q, 100);     // First alarm-msg
    return 0;
}

void *producer2(void *arg) {
    msleep(250);        // Runs after producer1
    printf("Producer 2: Attempting to send second ALARM msg (Should get blocked)\n");
    put_alarm(q, 200);      // Should be blocked until first alarm is received
    return 0;
}

void *consumer(void *arg) {
    msleep(500);        // Start after producer1 sends first msg
    printf("Consumer: attempting to receive first Alarm msg\n");
    get(q);     // Receive first alarm. Producer2 can now send its alarm
    printf("Consimer: attempting to receive second alarm msg\n");
    get(q);
    return 0;
}

int main(int argc, char **argv) {
    q = aq_create();
    if (q == NULL) {
        printf("AlarmQueue could not be created\n");
        exit(1);
    }

    pthread_t t1, t2, t3;
    void *res1, *res2, *res3;

    printf("---------------\n");

    // Fork the threads
    pthread_create(&t1, NULL, producer1, NULL);
    pthread_create(&t2, NULL, producer2, NULL);
    pthread_create(&t3, NULL, consumer, NULL);

    // Join threads
    pthread_join(t1, &res1);
    pthread_join(t2, &res2);
    pthread_join(t3, &res3);

    printf("---------------\n");
    printf("Threads terminated with %ld, %ld, %ld\n", (uintptr_t)res1, (uintptr_t)res2, (uintptr_t)res3);

    print_sizes(q);
    aq_destroy(q);

    return 0;
}