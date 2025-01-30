#include "../../kernel/types.h"
#include "../../kernel/fs.h"
#include "../../kernel/param.h"
#include "../../kernel/memlayout.h"
#include "../../kernel/riscv.h"
#include "../../kernel/defs.h"
#include "../../kernel/spinlock.h"
#include "../../kernel/proc.h"
#include "../../kernel/syscall.h"
#include "../../kernel/sleeplock.h"
#include "../../kernel/file.h"
#include "../../kernel/fcntl.h"
#include "sys_pipe_rt.h"
#include "pipe_rt.h"

struct priority_pipe pqueue;

uint64 sys_pipe_rt(void) {
    init_priority_pipe(&pqueue);
    return 0;
}

uint64 sys_enqueue_task(void) {
    struct proc *p = myproc();
    task_t *task = (task_t *)kalloc(); 

    if (!task) {
        return -1;
    }

    uint64 op_addr, result_addr, error_addr;
    char op[2] = {0};  

    argint(0, &task->priority);
    argint(1, &task->x);
    argint(2, &task->y);
    argaddr(3, &op_addr);
    argaddr(4, &result_addr);
    argaddr(5, &error_addr);

    if (copyin(p->pagetable, op, op_addr, sizeof(op)) < 0) {
        printf("[ERROR] copyin failed: unable to copy operator\n");
        kfree((void *)task);
        return -1;
    }
    task->op = op[0]; 

    task->result = (int *)result_addr;
    task->error = (int *)error_addr;

    enqueue_task(&pqueue, task);

    return 0;
}

uint64 sys_dequeue_task(void) {
    struct proc *p = myproc();
    uint64 user_task_addr;

    argaddr(0, &user_task_addr);

    task_t *task_ptr = dequeue_task(&pqueue);
    if (!task_ptr) {
        return -1; 
    }

    int kernel_result = 0, kernel_error = 0;

    switch (task_ptr->op) {
        case '+': 
            kernel_result = task_ptr->x + task_ptr->y;
            break;
        case '-': 
            kernel_result = task_ptr->x - task_ptr->y;
            break;
        case '*': 
            kernel_result = task_ptr->x * task_ptr->y;
            break;
        case '/':
            if (task_ptr->y == 0) {
                kernel_error = 1; 
            } else {
                kernel_result = task_ptr->x / task_ptr->y;
            }
            break;
        default:
            kernel_error = 1;
            printf("[ERROR] Invalid operator: %d\n", task_ptr->op);
    }

    if (copyout(p->pagetable, (uint64)task_ptr->result, (char *)&kernel_result, sizeof(int)) < 0 ||
        copyout(p->pagetable, (uint64)task_ptr->error, (char *)&kernel_error, sizeof(int)) < 0) {
        printf("[ERROR] copyout failed: unable to copy result/error\n");
        kfree((void *)task_ptr);
        return -1;
    }

    if (copyout(p->pagetable, user_task_addr, (char *)task_ptr, sizeof(task_t)) < 0) {
        printf("[ERROR] copyout failed: unable to copy task to user space\n");
        kfree((void *)task_ptr);
        return -1;
    }

    kfree((void *)task_ptr); 
    return 0;
}
