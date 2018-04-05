/*
 * William Akinsanya
 *
 * A simple shell for UNIX.
 * Builtins: 'cd' and '>' (redirect from stdout to a text file).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include "change_directory.h"

void print_prompt() {
    char buffer[32];
    time_t current_time = time(0);
    strftime(buffer, 32, "%d/%m %H:%M", localtime(&current_time));
    printf("[%s]# ", buffer);
}

// Handle Ctrl C.
void sig_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n");
        print_prompt();
        fflush(stdout);
    }
}

// Parse and execute the user's command.
void command_handler(char *command) {
    if (command[0] == '\0') {
        return;
    } else {
        char **arguments = malloc(sizeof(char *));
        char *tmp = strtok(command, " ");
        int i = 0;
        int f = 0;
        int redirect_flag = 0;
        for (; tmp != NULL;) {
            if (strcmp(tmp, ">") == 0) {
                redirect_flag = 1;
                tmp = strtok(NULL, " ");
                f = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                break;
            }

            arguments[i] = tmp;
            arguments = realloc(arguments, (++i + 1) * sizeof(char *));
            tmp = strtok(NULL, " ");
        }
        arguments[i] = NULL;

        // Builtin cd.
        if (strcmp(arguments[0], "cd") == 0) {
            change_directory(arguments);
        } else {
            pid_t pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Fork unsuccessful.");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                if (redirect_flag) {
                    dup2(f, 1);
                }
                execvp(arguments[0], arguments);
                exit(EXIT_SUCCESS);
            } else {
                wait(NULL);
                return;
            }
        }
    }

}

// Print prompt for the user and take input.
void command_loop() {

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        printf("\nCan't catch SIGINT\n");
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    print_prompt();
    fflush(stdout);

    while ((read = getline(&line, &len, stdin)) != EOF) {

        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        command_handler(line);
        print_prompt();
    }
}

int main() {

    command_loop();

    return 0;
}