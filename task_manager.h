#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

// Task structure
typedef struct {
    int id;
    char description[256];
} Task;

// Function prototypes
int initialize_task_manager();
int add_task(const char *description);
void list_tasks();
int remove_task(int task_id);
void cleanup_task_manager();

#endif // TASK_MANAGER_H
