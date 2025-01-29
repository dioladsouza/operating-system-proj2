#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "proj2/user/user.h"
#include "kernel/fcntl.h"

int calc(int x, int y, char* op, int* result) {
    switch (*op) {
        case '+':
            *result = x + y;
            return 0;
        case '-':
            *result = x - y;
            return 0;
        case '*':
            *result = x * y;
            return 0;
        case '/':
            if (y == 0) {
                return 1; // Division by zero error
            } 
            *result = x / y;
            return 0;
        default:
            return 1; // Invalid operator
    }
}

void server(int read_fd, int write_fd) {
    task_t task;

    int result, error;

    while (read(read_fd, &task, sizeof(task)) > 0) {
        error = calc(task.x, task.y, &task.op, &result);

        write(write_fd, &result, sizeof(int));
        write(write_fd, &error, sizeof(int));
    }
    exit(0);
}

void client(int write_fd, int read_fd, task_t task) {
    write(write_fd, &task, sizeof(task));
    read(read_fd, task.result, sizeof(int));
    read(read_fd, task.error, sizeof(int));

    if (*(task.error)) {
        printf("Error in calculation (division by zero or invalid operator)\n");
    } else {
        printf("%d\n", *(task.result));
        //printf("%d + %d = %d\n", task.x, task.y, *(task.result));
    }
    exit(0);
}

int main() {
    int p1[2]; // Pipe 1: Client to Server
    int p2[2]; // Pipe 2: Server to Client

    pipe(p1); 
    pipe(p2);
    
    if (pipe(p1) < 0 || pipe(p2) < 0) {
        printf("Failed to create pipes\n");
        exit(1);
    }

    for (int i = 0; i < 5; i++) {
        if (fork() == 0) { // child process to be client
            task_t task;
            int result = 0, error = 0;

            task.x = i + 10;        // x = [10,11,12,13,14]
            task.y = i + 20;        // y = [20,21,22,23,24]
            task.op = '+'; 
            task.result = &result;  //result = [30,32,34,36,38]
            task.error = &error;

            client(p1[1], p2[0], task);
            exit(0);
        }
    }

    server(p1[0], p2[1]);

    wait(0);
    exit(0);
}
