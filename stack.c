#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 	10
typedef struct Node
{
    int data;
    struct Node *next;
} StackNode;
StackNode *top;
// Mutex lock used to protect the critical sections in push() and pop()
// so that concurrent Push/Pop threads do not race on the shared stack.
pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;
// function prototypes
void push(int v, StackNode **top);
int pop(StackNode **top);
int is_empty(StackNode *top);
// push function
void push(int v, StackNode **top)
{
    StackNode *new_node;
    pthread_mutex_lock(&stack_mutex);
    printf("Thread is running Push() operation for value: %d\n", v);
    new_node = (StackNode *)malloc(sizeof(StackNode));
    new_node->data = v;
    new_node->next = *top;
    *top = new_node;
    pthread_mutex_unlock(&stack_mutex);
}
// pop function
int pop(StackNode **top)
{
	StackNode *temp;
    pthread_mutex_lock(&stack_mutex);
    if (is_empty(*top)) {
        printf("Stack empty \n");
        pthread_mutex_unlock(&stack_mutex);
        return 0;
	}
    else {
        int data = (*top)->data;
        printf("Thread is running Pop() operation and value is: %d\n",data);
		temp = *top;
        *top = (*top)->next;
		free(temp);
        pthread_mutex_unlock(&stack_mutex);
        return data;
    }
}
//Check if top is NULL
int is_empty(StackNode *top) {
    if (top == NULL)
        return 1;
    else
        return 0;
}
// Thread's push function
void* thread_push(void *args)
{
    int *threadId = (int *)args;
    push(*threadId + 1,&top);
    return NULL;
}
// Thread's pop function 
void* thread_pop(){
    pop(&top);
    return NULL;
}
// main function 
int main(void)
{
    // Note: 'top' is the global stack pointer declared above.
    // It is zero-initialized (NULL) by default since it has static storage
    // duration. A local 'top' was previously declared here, which shadowed
    // the global and was a source of confusion (though not itself a race
    // condition, since the global defaults to NULL anyway).
    pthread_t threads_push[NUM_THREADS];
    pthread_t threads_pop[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int i, j;
    // Creating push threads
    for(i=0;i<NUM_THREADS;i++){
	thread_args[i] = i;
	    pthread_create(&threads_push[i],NULL,thread_push, (void *)&thread_args[i]);
	}
	// Create Pop Threads
	for(i=0;i<NUM_THREADS;i++){
	    pthread_create(&threads_pop[i],NULL,thread_pop, NULL);
	}
	// Join Push Threads
	for(j = 0; j < NUM_THREADS; j++)
	{
	pthread_join(threads_push[j],NULL);
	}
    // Join Pop Threads
	for(j = 0; j < NUM_THREADS; j++)
	{
	pthread_join(threads_pop[j],NULL);
	}
    pthread_exit(NULL);
	return 0;
}
