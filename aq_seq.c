/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h" 
#include <stdlib.h>
#include <pthread.h>

 // Error codes to be printed depending on the nature of the message
// #define AQ_NO_MSG -1
// #define AQ_NO_ROOM -2
// #define AQ_NOT_IMPL -3


 // Struct to make a node in the message queue
 typedef struct MessageNode {
  void *message;
  struct MessageNode *next;
 } MessageNode; 

 // Struct to define the structure of the alarm message queue
 typedef struct {
  void *alarm_message;        // single alarm message
  MessageNode *normal_head;   // poiner to head of the "normal" message queue
  MessageNode *normal_tail;   // pointer to tail of the "normal" message queue
  int message_count;          // count to store number of messages
  int has_alarm;              // flag to indicate whether an alarm message is present or not

  //pthread_mutex_t lock;
  //pthread_cond_t not_empty;
  //pthread_cond_t space_available;
 } AlarmQueueStruct; 


AlarmQueue aq_create( ) {
  AlarmQueueStruct *q = (AlarmQueueStruct *)malloc(sizeof(AlarmQueueStruct));
  if (!q) return NULL;        // return NULL if anything fails
  q->alarm_message = NULL;    // indicating no alarm message is present.
  q->normal_head = NULL;      // no normal messages initially
  q->normal_tail = NULL;
  q->message_count = 0;       
  q->has_alarm = 0;

  /*// initialize the mutex and condition variables
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
  }*/

  //pthread_mutex_init(&q->lock, NULL);
  //pthread_cond_init(&q->not_empty, NULL);
  //pthread_cond_init(&q->space_available, NULL);

  return (AlarmQueue)q;    // returns pointer to the created struct cast to opaque type
}

int aq_send( AlarmQueue aq, void * msg, MsgKind k){
  AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;

  //pthread_mutex_lock(&queue->lock);

  if (k == AQ_ALARM) {
    if (queue->has_alarm) return AQ_NO_ROOM;   // no room for another message

    queue->alarm_message = msg;   // stores message
    queue->has_alarm = 1;   // sets flag to 1

  } else if (k == AQ_NORMAL) {
    MessageNode *new_node =(MessageNode *)malloc(sizeof(MessageNode));
    if (!new_node) return AQ_NO_ROOM;   // allocation failure
    new_node->message = msg;
    new_node->next = NULL; 

    if (queue->normal_tail) {
      queue->normal_tail->next = new_node; // add new node to end of linked list
    } else {
      queue->normal_head = new_node; 
    }
    queue->normal_tail = new_node;
  }
  queue->message_count++;   // increment counter regardless of type
  //pthread_cond_signal(&queue->not_empty);   // signal to indicate if there's a new message to receive
  //pthread_mutex_unlock(&queue->lock);

  return 0;
  //return AQ_NOT_IMPL;
}

int aq_recv(AlarmQueue aq, void * * msg) {
  AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;

  //pthread_mutex_lock(&queue->lock);

  if (queue->message_count == 0) return AQ_NO_MSG;

  if (queue->has_alarm) {
    *msg = queue->alarm_message;    // stores the alarm-message in msg
    queue->alarm_message = NULL;    // clear alarm-message from queue
    queue->has_alarm = 0;   // reset to 0 
    //pthread_cond_signal(&queue->space_available);    // signal space for another alarm
  } else {
    MessageNode *node = queue->normal_head;   // dequeue first normal message
    *msg = node->message;   // assign message node as msg in function
    queue->normal_head = node->next;

    if (!queue->normal_head) {
      queue->normal_tail = NULL;    // if normal_head becomes NULL, so does normal_tail
    }
    free(node);
  } 

  //return AQ_NO_MSG;   // if no messages in the queue
  queue->message_count--;   // decrement after receiving a message
  //pthread_mutex_unlock(&queue->lock);
  return queue->has_alarm ? AQ_ALARM : AQ_NORMAL;   // return accordingly to if a message is received 
  //return AQ_NOT_IMPL;
}

int aq_size(AlarmQueue aq) {
  AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
  //pthread_mutex_lock(&queue->lock);
  //int size = queue->message_count;
  //pthread_mutex_unlock(&queue->lock);
  //return size;
  return queue->message_count;    // returns total number of messages in queue  
  //return 0;
}

int aq_alarms( AlarmQueue aq) {
  AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
  //pthread_mutex_lock(&queue->lock);
  //int has_alarm = queue->has_alarm;
  //pthread_mutex_unlock(&queue->lock);
  //return has_alarm;
  return queue->has_alarm;    // returns 1 if alarm message is present, 0 otherwise
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

  //pthread_mutex_destroy(&queue->lock);    // destroy mutex and condition variables
  //pthread_cond_destroy(&queue->not_empty);
  //pthread_cond_destroy(&queue->space_available);

  free(queue);    // frees queue 
}