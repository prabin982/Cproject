#include "task_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TASKS_FILE "tasks.dat"

static FILE *tasks_fp = NULL;
static int next_task_id = 1;

int initialize_task_manager() {
    // Open the tasks file for reading and writing
    tasks_fp = fopen(TASKS_FILE, "a+b");
    if (tasks_fp == NULL) {
        perror("Failed to open tasks file");
        return -1;
    }

    // Determine the next task ID
    fseek(tasks_fp, 0, SEEK_SET);
    Task task;
    while (fread(&task, sizeof(Task), 1, tasks_fp) == 1) {
        if (task.id >= next_task_id) {
            next_task_id = task.id + 1;
        }
    }

    return 0;
}

int add_task(const char *description) {
    if (tasks_fp == NULL) {
        return -1;
    }

    Task task;
    task.id = next_task_id++;
    strncpy(task.description, description, sizeof(task.description) - 1);
    task.description[sizeof(task.description) - 1] = '\0';

    // Append the task to the file
    fseek(tasks_fp, 0, SEEK_END);
    if (fwrite(&task, sizeof(Task), 1, tasks_fp) != 1) {
        perror("Failed to write task");
        return -1;
    }
    fflush(tasks_fp);

    return 0;
}

void list_tasks() {
    if (tasks_fp == NULL) {
        return;
    }

    // Read all tasks from the file
    fseek(tasks_fp, 0, SEEK_SET);
    Task task;
    int count = 0;

    printf("ID  | Description\n");
    printf("----+-------------\n");
    
    while (fread(&task, sizeof(Task), 1, tasks_fp) == 1) {
        printf("%-3d | %s\n", task.id, task.description);
        count++;
    }

    if (count == 0) {
        printf("No tasks found\n");
    }
}

int remove_task(int task_id) {
    if (tasks_fp == NULL) {
        return -1;
    }

    // Create a temporary file
    FILE *temp_fp = fopen("temp.dat", "wb");
    if (temp_fp == NULL) {
        perror("Failed to create temporary file");
        return -1;
    }

    // Copy all tasks except the one to be removed
    fseek(tasks_fp, 0, SEEK_SET);
    Task task;
    int found = 0;

    while (fread(&task, sizeof(Task), 1, tasks_fp) == 1) {
        if (task.id != task_id) {
            if (fwrite(&task, sizeof(Task), 1, temp_fp) != 1) {
                perror("Failed to write to temporary file");
                fclose(temp_fp);
                return -1;
            }
        } else {
            found = 1;
        }
    }

    // Close both files
    fclose(temp_fp);
    fclose(tasks_fp);

    // Replace the original file with the temporary file
    if (remove(TASKS_FILE) != 0) {
        perror("Failed to remove original file");
        tasks_fp = fopen(TASKS_FILE, "a+b");
        return -1;
    }

    if (rename("temp.dat", TASKS_FILE) != 0) {
        perror("Failed to rename temporary file");
        tasks_fp = fopen(TASKS_FILE, "a+b");
        return -1;
    }

    // Reopen the tasks file
    tasks_fp = fopen(TASKS_FILE, "a+b");
    if (tasks_fp == NULL) {
        perror("Failed to reopen tasks file");
        return -1;
    }

    return found ? 0 : -1;
}

void cleanup_task_manager() {
    if (tasks_fp != NULL) {
        fclose(tasks_fp);
        tasks_fp = NULL;
    }
}
