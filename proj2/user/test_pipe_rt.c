#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

typedef struct task_t {
    int priority;
    int x;
    int y;
    char op;
    int *result;
    int *error;
} task_t;

extern int pipe_rt(void);
extern int enqueue_task(int priority, int x, int y, char *op, int *result, int *error);
extern int dequeue_task(task_t *task);

int main() {
    printf("Initializing priority pipe\n");
    pipe_rt();

    printf("Creating and enqueuing tasks\n");

    int result1, error1, result2, error2, result3, error3, result4, error4, result5, error5, result6, error6;
    char op1 = '+', op2 = '-', op3 = '*', op4 = '/', op5 = '*', op6 = '-'; 

    enqueue_task(6, 10, 5, &op1, &result1, &error1);
    enqueue_task(5, 15, 3, &op2, &result2, &error2);
    enqueue_task(4, 7, 2, &op3, &result3, &error3);
    enqueue_task(3, 8, 8, &op4, &result4, &error4);
    enqueue_task(2, 10, 10, &op5, &result5, &error5);
    enqueue_task(1, 400, 200, &op6, &result6, &error6);


    printf("Dequeuing and executing tasks\n");

    task_t task;
    while (dequeue_task(&task) == 0) {
        if (*task.error) {
            printf("Task failed (Invalid operation or division by zero)\n");
        } else {
            printf("Task: %d %d = %d\n", task.x, task.y, *task.result);
        }
    }

    printf("Test complete.\n");
    exit(0);
}
