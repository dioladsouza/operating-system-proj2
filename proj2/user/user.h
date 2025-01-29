typedef struct task_t {
    int priority; 
    int x;
    int y;
    char op; // Supports "+", "-", "*", "/"
    int *result;
    int *error; 
} task_t;


int calc(int x, int y, char* op, int* result);
void server(int read_fd, int write_fd);
void client(int write_fd, int read_fd, task_t task);
int pipe_rt(int *); 