#ifndef _PIPE_RT_H
#define _PIPE_RT_H

#include "../../kernel/types.h"
#include "../../kernel/spinlock.h"

typedef struct task_t {
    int priority;
    int x;
    int y;
    char op;
    int *result;
    int *error;
    struct task_t *next;  
} task_t;

struct priority_pipe {
    struct spinlock lock;
    task_t *head;
};

void init_priority_pipe(struct priority_pipe *p);
void enqueue_task(struct priority_pipe *p, task_t *task);
task_t *dequeue_task(struct priority_pipe *p);

#endif
