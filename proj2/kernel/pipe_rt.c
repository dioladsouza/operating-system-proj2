#include "../../kernel/types.h"
#include "../../kernel/param.h" 
#include "../../kernel/memlayout.h" 
#include "../../kernel/riscv.h" 
#include "../../kernel/defs.h" 
#include "../../kernel/spinlock.h"
#include "../../kernel/proc.h"
#include "../../kernel/sleeplock.h"
#include "../../kernel/fs.h"
#include "../../kernel/file.h"
#include "../../kernel/fcntl.h"

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

void init_priority_pipe(struct priority_pipe *p) {
    initlock(&p->lock, "priority_pipe");
    p->head = 0;
}

void enqueue_task(struct priority_pipe *p, task_t *task) {
    acquire(&p->lock);

    if (p->head == 0 || task->priority > p->head->priority) {
        task->next = p->head;
        p->head = task;
    } else {
        task_t *curr = p->head;
        while (curr->next && curr->next->priority >= task->priority) {
            curr = curr->next;
        }
        task->next = curr->next;
        curr->next = task;
    }

    release(&p->lock);
}

task_t *dequeue_task(struct priority_pipe *p) {
    acquire(&p->lock);
    if (p->head == 0) {
        release(&p->lock);
        return 0;
    }
    task_t *task = p->head;
    p->head = p->head->next;
    release(&p->lock);
    return task;
}
