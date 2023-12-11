#include "bello.h"


int main(int argc, char **argv) {
    return bello();
}

/*
 * Function:  bello
 * --------------------
 * Displays various information about the user and system:
 * 1. Username: Retrieved using getlogin().
 * 2. Hostname: Retrieved using gethostname().
 * 3. Last Executed Command: This would need to be tracked by your shell and passed to this function.
 * 4. TTY: Retrieved using ttyname() on file descriptor 0.
 * 5. Current Shell Name: Retrieved from the SHELL environment variable.
 * 6. Home Location: Retrieved from the HOME environment variable.
 * 7. Current Time and Date: Retrieved using time() and localtime().
 * 8. Current Number of Processes: Retrieved by executing the 'ps -e' command and counting lines of output.
 *
 * returns: 0 if successful, 1 if an error occurs.
 */
int bello() {
    char hostname[256];
    char *username;
    char *tty;
    char *shell;
    char *home;
    time_t currentTime;
    struct tm *timeInfo;
    FILE *fp;
    int processCount = 0;
    char path[1035];

    // Retrieve and print the username
    username = getlogin();
    if (username == NULL) {
        printf("Error: Unable to retrieve username.\n");
        return 1;
    }
    printf("Username: %s\n", username);

    // Retrieve and print the hostname
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        printf("Error: Unable to retrieve hostname.\n");
        return 1;
    }
    printf("Hostname: %s\n", hostname);

    // Retrieve and print the TTY
    tty = ttyname(STDIN_FILENO);
    if (tty == NULL) {
        printf("Error: Unable to retrieve TTY.\n");
        return 1;
    }
    printf("TTY: %s\n", tty);

    // Retrieve and print the shell name
    shell = getenv("SHELL");
    if (shell == NULL) {
        printf("Error: Unable to retrieve shell name.\n");
        return 1;
    }
    printf("Current Shell Name: %s\n", shell);

    // Retrieve and print the home location
    home = getenv("HOME");
    if (home == NULL) {
        printf("Error: Unable to retrieve home location.\n");
        return 1;
    }
    printf("Home Location: %s\n", home);

    // Retrieve and print the current time and date
    time(&currentTime);
    timeInfo = localtime(&currentTime);
    printf("Current Time and Date: %s", asctime(timeInfo));

    // Count and print the number of processes
    fp = popen("ps -e | wc -l", "r");
    if (fp == NULL) {
        printf("Error: Failed to run command\n");
        return 1;
    }
    if (fgets(path, sizeof(path), fp) != NULL) {
        processCount = atoi(path);
    }
    pclose(fp);
    printf("Number of Processes: %d\n", processCount);

    return 0;
}
