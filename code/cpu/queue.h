// --
// queue
// adapted from: https://de.wikibooks.org/wiki/Algorithmen_und_Datenstrukturen_in_C/_Warteschlange

// include guard
#ifndef QUEUE_H   
#define QUEUE_H

#include "enums.h"
#include "structs.h"

#define SUCCESS 0
#define ERR_INVAL 1
#define ERR_NOMEM 2


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

int queue_destroy(struct Queue *queue);
bool queue_empty(struct Queue *queue);
struct Queue *queue_new(void);
void *queue_dequeue(struct Queue *queue);
int queue_enqueue(struct Queue *queue, void *data);
int queue_len(struct Queue *queue);

#endif