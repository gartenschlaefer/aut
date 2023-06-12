// --
// queue
// adapted from: https://de.wikibooks.org/wiki/Algorithmen_und_Datenstrukturen_in_C/_Warteschlange

#include <stdlib.h>
#include "queue.h"


/* ------------------------------------------------------------------*
 *            destroy
 * ------------------------------------------------------------------*/

int queue_destroy(struct Queue *queue)
{
  if(queue == NULL) { return ERR_INVAL; }
  while(queue->front != NULL) 
  {
    struct Node *node = queue->front;
    queue->front = node->next;
    free(node);
  }
  free(queue);
  return SUCCESS;
}


/* ------------------------------------------------------------------*
 *            empty
 * ------------------------------------------------------------------*/

bool queue_empty(struct Queue *queue)
{
  if(queue == NULL || queue->front == NULL) { return true; } 
  else{ return false; }
}


/* ------------------------------------------------------------------*
 *            new
 * ------------------------------------------------------------------*/

struct Queue *queue_new(void)
{
  struct Queue *queue = malloc(sizeof(struct Queue));
  if(queue == NULL) { return NULL; }
  queue->front = queue->back = NULL;
  return queue;
}


/* ------------------------------------------------------------------*
 *            get item
 * ------------------------------------------------------------------*/

void *queue_dequeue(struct Queue *queue)
{ 
  // no element
  if(queue == NULL || queue->front == NULL){ return NULL; }
  struct Node *node = queue->front;
  void *data = node->data;
  queue->front = node->next;
  if(queue->front == NULL)
  {
    queue->back = NULL;
  }
  free(node);
  return data;
}


/* ------------------------------------------------------------------*
 *            add item
 * ------------------------------------------------------------------*/

int queue_enqueue(struct Queue *queue, void *data)
{
  if(queue == NULL){ return ERR_INVAL; }
  struct Node *node = malloc(sizeof(struct Node));
  if(node == NULL){ return ERR_NOMEM; }

  // get pointers
  node->data = data;
  node->next = NULL;

  // only one element handling
  if(queue->back == NULL)
  {
    queue->front = queue->back = node;
  }
  // usual handling
  else 
  {
    queue->back->next = node;
    queue->back = node;
  }
  return SUCCESS;
}


/* ------------------------------------------------------------------*
 *            add item
 * ------------------------------------------------------------------*/

int queue_len(struct Queue *queue)
{
  int len = 0;
  if(queue == NULL){ return len; }
  if(queue->front == NULL){ return len; }
  struct Node *node = queue->front;
  do{ node = node->next; len++; } while(node->next != NULL);
  return len;
}