#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHELL_BUFF_SIZE 1024;

char **get_input(char *);
char* cshl_read_line(void);

int main() {
    char **command;
    char *input;
    pid_t child_pid;
    int stat_loc;

    while (1) {
	printf("> ");
	input = cshl_read_line();
	
        command = get_input(input);

	// if the command is empty:
        if (!command[0]) {
            free(input);
            free(command);
            continue;
        }

	if (strcmp(command[0], "cd") == 0) {
		if (chdir(command[1]) < 0) {
			perror(command[1]);
		}
		// if the command is cd and it doesnt return error -> skip the fork
		continue;
	}

        child_pid = fork();
	if (child_pid < 0) {
		perror("Fork failed");
		exit(1);
	}
        if (child_pid == 0) {
		// this is a child process
	    if(execvp(command[0], command) < 0) { 
		    perror(command[0]);
		    exit(1);
	    }
        } else {
            waitpid(child_pid, &stat_loc, WUNTRACED);
        }

        free(input);
        free(command);
    }

    return 0;
}

char **get_input(char *input) {
    char **command = malloc(8 * sizeof(char *));
    if (command == NULL) {
	    perror("malloc failed");
	    exit(1);
    }
    char *separator = " ";
    char *parsed;
    int index = 0;

    parsed = strtok(input, separator);
    while (parsed != NULL) {
        command[index] = parsed;
        index++;

        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}

char* cshl_read_line(void) {
	int bufsize = SHELL_BUFF_SIZE;
	int pos = 0;
	char* buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer) {
		fprintf(stderr, "cshell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		c = getchar();

		if (c == EOF || c == '\n') {
			buffer[pos] = '\0';
			return buffer;
		} else {
			buffer[pos] = c;
		}
		pos++;

		// if buffer is overflowing, reallocate
		if (pos >= bufsize) {
			bufsize += SHELL_BUFF_SIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "cshell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}
