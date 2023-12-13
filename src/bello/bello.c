#include "bello.h"

void print_info(char *username, char *hostname, char *last_executed_command, char *tty, char *shell_name, char *home_location, time_t time_info, struct tm *time_struct, int process_count);
int is_number(const char *s);
int get_child_processes(pid_t parent_pid);

int main(int argc, char **argv) { return bello(); }

/*
 * Function:  bello
 * --------------------
 * Displays various information about the user and system:
 * 1. Username: Retrieved using getenv("USER").
 * 2. Hostname: Retrieved using gethostname().
 * 3. Last Executed Command: Read from a file called .history
 * 4. TTY: Retrieved using ttyname() on file descriptor 0.
 * 5. Current Shell Name: Retrieved from the SHELL environment variable.
 * 6. Home Location: Retrieved from the HOME environment variable.
 * 7. Current Time and Date: Retrieved using time() and localtime().
 * 8. Current Number of Processes: Retrieved by executing the 'ps -e' command and counting lines of output.
 * returns: 0 if successful, 1 if an error occurs.
 */
int bello() {
    char hostname[256];
    char last_executed_command[256];
    FILE *history_file;
    FILE *fp;
    char path[MAX_PATH_LENGTH];

    // 1. Username
    char *username = getenv("USER");

    // 2. Hostname
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        return 1;
    }

    // 3. Last Executed Command
    history_file = fopen(".history", "r");
    if (history_file) {
        if (fgets(last_executed_command, sizeof(last_executed_command), history_file) != NULL) {
            // Strip newline character
            last_executed_command[strcspn(last_executed_command, "\n")] = 0;
        } else {
            strcpy(last_executed_command, "No history available");
        }
        fclose(history_file);
    } else {
        strcpy(last_executed_command, "No history file found");
    }

    // 4. TTY
    char *tty = ttyname(STDIN_FILENO);

    // 5. Current Shell Name
    char *shell_name = getenv("SHELL");

    // 6. Home Location
    char *home_location = getenv("HOME");

    // 7. Current Time and Date
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);

    // 8. Current Number of Processes
    pid_t ppid = getppid();
    pid_t child_pids[MAX_CHILDREN];
    size_t max_children = sizeof(child_pids) / sizeof(child_pids[0]);
    printf("Parent PID: %d\n", ppid);
    int process_count = get_child_processes(ppid);

    for (int i = 0; i < process_count; ++i) {
        printf("Child process ID: %d\n", child_pids[i]);
    }
    // Print the information
    print_info(username, hostname, last_executed_command, tty, shell_name, home_location, current_time, time_info, process_count);

    return 0;
}

/*
 * Function:  print_info
 * --------------------
 * Prints the information gathered by bello().
 * username: The username of the current user.
 * hostname: The hostname of the current machine.
 * last_executed_command: The last executed command.
 * tty: The TTY of the current shell.
 * shell_name: The name of the current shell.
 * home_location: The home location of the current user.
 * time_info: The current time.
 * time_struct: The current time in struct tm format.
 * process_count: The number of processes currently running.
 */
void print_info(char *username, char *hostname, char *last_executed_command, char *tty, char *shell_name, char *home_location, time_t time_info, struct tm *time_struct, int process_count) {
    printf("1. Usernameeee: %s\n", username);
    printf("2. Hostname: %s\n", hostname);
    printf("3. Last Executed Command: %s\n", last_executed_command);
    printf("4. TTY: %s\n", tty);
    printf("5. Current Shell Name: %s\n", shell_name);
    printf("6. Home Location: %s\n", home_location);
    printf("7. Current Time and Date: %s", asctime(time_struct));
    printf("8. Current Number of Processes: %d\n", process_count);
}

/*
 * Function:  is_number
 * --------------------
 * Checks if a string is a number.
 * s: The string to check.
 * returns: 1 if the string is a number, 0 otherwise.
 */
int is_number(const char *s) {
    while (*s) {
        if (*s < '0' || *s > '9')
            return 0;
        s++;
    }
    return 1;
}

/*
 * Function:  get_child_processes
 * --------------------
 * Retrieves the child processes of a given parent process.
 *
 * parent_pid: The PID of the parent process.
 * child_pids: An array of PIDs to store the child processes in.
 * max_children: The maximum number of child processes to retrieve.
 *
 * returns: The number of child processes found.
 */
int get_child_processes(pid_t parent_pid) {
    int num_children = 0;
    struct kinfo_proc *procs = NULL;
    size_t procs_size = 0;

    // Define MIB array for sysctl call
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};

    // Get size of data
    if (sysctl(mib, 4, NULL, &procs_size, NULL, 0) < 0) {
        perror("sysctl");
        return -1;
    }

    // Allocate memory for process data
    procs = malloc(procs_size);
    if (procs == NULL) {
        perror("malloc");
        return -1;
    }

    // Get process data
    if (sysctl(mib, 4, procs, &procs_size, NULL, 0) < 0) {
        perror("sysctl");
        free(procs);
        return -1;
    }

    // Number of processes
    int num_procs = procs_size / sizeof(struct kinfo_proc);

    // Iterate through processes
    for (int i = 0; i < num_procs; i++) {
        if (procs[i].kp_eproc.e_ppid == parent_pid) {
            num_children++;
        }
    }

    free(procs);
    return num_children;
}