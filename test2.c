// Priority of alarm messages over normal messages

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "aq.h"
#include "aux.h"

static AlarmQueue q;

void *producer(void *arg) {
    msleep(100);

    // first normal message
    printf("Producer: Sending normal message 1\n");
    put_normal(q, 1);
    msleep(100);

    // second normal nessage
    printf("Producer: sending normal message 2\n");
    put_normal(q, 2);
    msleep(100);

    // alarm message
    printf("Producer: sending ALARM message\n");
    put_alarm(q, 999);
    msleep(100);

    // third normal message
    printf("Producer: sending normal message 3\n");
    put_normal(q, 3);
    msleep(100);

    // fourth normal message
    printf("Producer: sending normal message 4\n");
    put_normal(q, 4);

    return NULL;
}

void *consumer(void *arg) {
    printf("Consumer: starts receiving messages\n");

    for (int i = 0; i < 5; i++) {
        get(q); // receive all messages
    }

    return NULL;
}

int main(int argc, char **argv) {
    q = aq_create();
    if (q == NULL) {
        printf("AlarmQueue could not be created\n");
        exit(1);
    }

    pthread_t prod_thread;
    pthread_t cons_thread;

    printf("---------------\n");

    // producer- and consumer threads
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&prod_thread, NULL, consumer, NULL);

    // join the threads
    pthread_join(prod_thread, NULL);
    pthread_join(prod_thread, NULL);

    printf("--------------\n");

    print_sizes(q);
    aq_destroy(q);

    return 0;
}