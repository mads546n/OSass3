/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct MessageNode {
  void *message;
  struct MessageNode *next;
} MessageNode;

typedef struct {
  void *alarm_message;
  MessageNode *normal_head;
  MessageNode *normal_tail;
  int message_count;
  int has_alarm;

  pthread_mutex_t lock;
  pthread_cond_t not_empty;
  pthread_cond_t space_available;

} AlarmQueueStruct;

AlarmQueue aq_create( ) {
  AlarmQueueStruct *q = (AlarmQueueStruct *)malloc(sizeof(AlarmQueueStruct));
  if (!q) return NULL;        // return NULL if anything fails
  q->alarm_message = NULL;    // indicating no alarm message is present.
  q->normal_head = NULL;      // no normal messages initially
  q->normal_tail = NULL;
  q->message_count = 0;       
  q->has_alarm = 0;

  // initialize the mutex and condition variables
  if (pthread_mutex_init(&q->lock, NULL) != 0) {
    free(q);
    return NULL;
  }
  
  if (pthread_cond_init(&q->not_empty, NULL) != 0) {
    pthread_mutex_destroy(&q->lock);
    free(q);
    return NULL;
  }

  if (pthread_cond_init(&q->space_available, NULL) != 0) {
    pthread_cond_destroy(&q->not_empty);
    pthread_mutex_destroy(&q->lock);
    free(q);
    return NULL;
  }

  //pthread_mutex_init(&q->lock, NULL);
  //pthread_cond_init(&q->not_empty, NULL);
  //pthread_cond_init(&q->space_available, NULL);

  return (AlarmQueue)q;    // returns pointer to the created struct cast to opaque type
}

int aq_send( AlarmQueue aq, void * msg, MsgKind k){
  AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;

  pthread_mutex_lock(&queue->lock);

  if (k == AQ_ALARM) {
    printf("Producer attempting to send an ALARM message...\n");
    while (queue->has_alarm) {   // no room for another message
      printf("Producer blocked, waiting for space to send ALARM message...\n");
      pthread_cond_wait(&queue->space_available, &queue->lock);
      printf("Producer unblocked, attempting to send ALARM message again...\n");
    }

    queue->alarm_message = msg;   // stores message
    queue->has_alarm = 1;   // sets flag to 1 (alarm is present)
    printf("Producer sent ALARM message successfully.\n");

  } else if (k == AQ_NORMAL) {
    MessageNode *new_node =(MessageNode *)malloc(sizeof(MessageNode));
    if (!new_node) {   // allocation failure
      pthread_mutex_unlock(&queue->lock);
      return AQ_NO_ROOM;
    }
    new_node->message = msg;
    new_node->next = NULL; 

    if (queue->normal_tail) {
      queue->normal_tail->next = new_node; // add new node to end of linked list
    } else {
      queue->normal_head = new_node; 
    }
    queue->normal_tail = new_node;
    queue->message_count++;   // increment counter regardless of type
  }
  pthread_cond_signal(&queue->not_empty);   // signal to indicate if there's a new message to receive
  pthread_mutex_unlock(&queue->lock);

  return 0;
  //return AQ_NOT_IMPL;
}

int aq_recv(AlarmQueue aq, void * * msg) {
  AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;

  pthread_mutex_lock(&queue->lock);

  while (queue->message_count == 0 && !queue->has_alarm) {
    pthread_cond_wait(&queue->not_empty, &queue->lock);
  }

  if (queue->has_alarm) {
    *msg = queue->alarm_message;    // stores the alarm-message in msg
    queue->alarm_message = NULL;    // clear alarm-message from queue
    queue->has_alarm = 0;   // clear flag after processing alarm
    //queue->message_count--;

    printf("Consumer received ALARM message, signaling that space is available for another ALARM...\n");

    pthread_cond_signal(&queue->space_available);   // signal space for another alarm
    pthread_mutex_unlock(&queue->lock);
    return AQ_ALARM;   // return AQ_ALARM if an alarm message is received
 
  } else {
    MessageNode *node = queue->normal_head;   // dequeue first normal message
    *msg = node->message;   // assign message node as msg in function
    queue->normal_head = node->next;

    if (!queue->normal_head) {
      queue->normal_tail = NULL;    // if normal_head becomes NULL, so does normal_tail
    }
    free(node);
    queue->message_count--;   // decrement after receiving a message
    pthread_mutex_unlock(&queue->lock);
    return AQ_NORMAL;   // return accordingly to if a message is received 
    //return AQ_NOT_IMPL;
  }
}

int aq_size(AlarmQueue aq) {
  AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
  pthread_mutex_lock(&queue->lock);
  int size = queue->message_count;
  pthread_mutex_unlock(&queue->lock);
  return size;    // returns total number of messages in queue  
  //return 0;
}

int aq_alarms( AlarmQueue aq) {
  AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
  pthread_mutex_lock(&queue->lock);
  int size = queue->message_count;
  int has_alarm = queue->has_alarm;
  pthread_mutex_unlock(&queue->lock);
  return has_alarm;    // returns 1 if alarm message is present, 0 otherwise
  //return 0;
}

// Function to free all resources associated with the queue
void aq_destroy(AlarmQueue aq) {
  AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;

  while (queue->normal_head) {    // Free normal message nodes
    MessageNode *node = queue->normal_head;   // iterates through linked list of normal messages
    queue->normal_head = node->next;
    free(node);   // frees each MessageNode
  }

  pthread_mutex_destroy(&queue->lock);    // destroy mutex and condition variables
  pthread_cond_destroy(&queue->not_empty);
  pthread_cond_destroy(&queue->space_available);

  free(queue);    // frees queue 
}


