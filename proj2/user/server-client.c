#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

typedef struct {
    int x;
    int y;
    char op;   
    int result;
    int error; 
} task_t;

void calc(task_t *task) {
    task->error = 0;
    switch (task->op) {
        case '+': task->result = task->x + task->y; break;
        case '-': task->result = task->x - task->y; break;
        case '*': task->result = task->x * task->y; break;
        case '/':
            if (task->y == 0) task->error = 1; 
            else task->result = task->x / task->y;
            break;
        default:
            task->error = 1; 
    }
}

void server(int read_fd, int write_fd) {
    task_t task;
    while (read(read_fd, &task, sizeof(task_t)) > 0) {
        calc(&task);
        write(write_fd, &task, sizeof(task_t));
    }
    close(read_fd);
    close(write_fd);
}

void client(int write_fd, int read_fd, int x, int y, char op) {
    task_t task = {x, y, op, 0, 0};
    write(write_fd, &task, sizeof(task_t));
    read(read_fd, &task, sizeof(task_t));

    if (task.error)
        printf("Error in calculation (e.g., division by zero or invalid operation)\n");
    else
        printf("%d\n", task.result);
    
    close(write_fd);
    close(read_fd);
}

int main() {
    int p1[2], p2[2]; 

    if (pipe(p1) < 0 || pipe(p2) < 0) {
        printf("Failed to create pipes\n");
        exit(1);
    }

    int num_clients = 4;
    for (int i = 0; i < num_clients; i++) {
        if (fork() == 0) { 
            close(p1[0]);
            close(p2[1]); 
            
            char ops[] = {'/', '-', '*', '+'};
            client(p1[1], p2[0], i + 2, i + 1, ops[i % 4]);

            exit(0);
        }
    }
 
    close(p1[1]); 
    close(p2[0]); 
    server(p1[0], p2[1]);

    for (int i = 0; i < num_clients; i++) {
        wait(0);
    }

    exit(0);
}
