#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 512

void print_info(char *username, char *hostname, char *last_executed_command, char *tty, char *shell_name, char *home_location, time_t time_info, struct tm *time_struct, int process_count);
int is_number(const char *s);
int get_child_processes(pid_t parent_pid);
int bello();
