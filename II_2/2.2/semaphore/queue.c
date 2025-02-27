#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

void *qmonitor(void *arg) 
{
	queue_t *q = (queue_t *)arg;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		queue_print_stats(q);
		sleep(1);
	}

	return NULL;
}

queue_t* queue_init(int max_count) 
{
	int err;

	queue_t *q = malloc(sizeof(queue_t));
	if (!q) {
		printf("Cannot allocate memory for a queue\n");
		abort();
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		abort();
	}

	 if (sem_init(&q->lock, 0, 1) != 0) { 
        perror("sem_init lock");
	}
        
    if (sem_init(&q->empty_count, 0, max_count) != 0) { 
        perror("sem_init empty_count");
    }
    if (sem_init(&q->filled_count, 0, 0) != 0) { 
        perror("sem_init filled_count");
    }

	return q;
}

void queue_destroy(queue_t *q) 
{
    if(q == NULL)
	{
		printf("q is null\n");
		return;
	}

	qnode_t *current = q->first;
	qnode_t *tmp;

	while(current != NULL)
	{
		tmp = current;
		current = current->next;
		free(tmp);
	}

	q->first = NULL;
	q->last = NULL;

	pthread_cancel(q->qmonitor_tid);
	pthread_join(q->qmonitor_tid, NULL);

	sem_destroy(&q->lock);
    sem_destroy(&q->empty_count);
    sem_destroy(&q->filled_count);

	free(q);
	printf("Queue was destroyed\n");
}

/*void sem_wait(int *count)
{
	if (count == 0) sleep(5);
	else{
		count--;
	}
}*/

int queue_add(queue_t *q, int val) 
{
	q->add_attempts++;
    sem_wait(&q->empty_count); // не нужно проверять count 
    sem_wait(&q->lock); 

	qnode_t *new = malloc(sizeof(qnode_t));
	if (!new) {
		printf("Cannot allocate memory for new node\n");
		sem_post(&q->lock);       
    	sem_post(&q->filled_count);
		abort();
	}

	new->val = val;
	new->next = NULL;

	if (!q->first)
		q->first = q->last = new;
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	q->count++;
	q->add_count++;

    sem_post(&q->lock);       
    sem_post(&q->filled_count); 

	return 1;
}

int queue_get(queue_t *q, int *val) {
	q->get_attempts++;
    sem_wait(&q->filled_count);  // не нужно проверять count
    sem_wait(&q->lock);         

	qnode_t *tmp = q->first;

	*val = tmp->val;
	q->first = q->first->next;

	free(tmp);
	q->count--;
	q->get_count++;

    sem_post(&q->lock);      		
    sem_post(&q->empty_count);    
	
	return 1;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}




