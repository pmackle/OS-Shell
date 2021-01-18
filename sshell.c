#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Provided assumptions.
#define MAX_CMDLINE_SIZE 512
#define MAX_NUM_ARGS 16
#define MAX_TOKEN_SIZE 32

// Resolve user input into two unique elements.
struct CommandLine {
	int argc;
	char* argv[MAX_NUM_ARGS];
};

// Display success of builtin functions.
void display_exit_condition(char* command_line, int exit_status)
{
	fprintf(stderr, "+ completed '%s' ", command_line);
	fprintf(stderr, "[%i]\n", exit_status);
}

void parse_command_line(struct CommandLine* MyCommandLine, char* command_line)
{
	// Copy to preserve original input.
	char command_line_copy[MAX_CMDLINE_SIZE];
    strcpy(command_line_copy, command_line);

    // Derived from https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm.
	const char delim[2] = " ";
    char* current_token;

    // Get the first token describing the command.
    current_token = strtok(command_line_copy, delim);
    // Fill args with remaining tokens.
    int i = 0;
    while (current_token != NULL) {

		MyCommandLine->argv[i] = current_token;
		MyCommandLine->argc = MyCommandLine->argc + 1;
		current_token = strtok(NULL, delim);

        i++;
   }
   // Add the extra null argument.
   MyCommandLine->argv[i] = NULL;
}

int has_errors(struct CommandLine* MyCommandLine)
{
    if (MyCommandLine->argc > 16) {
        fprintf(stderr, "Error: too many process arguments\n");
        return EXIT_FAILURE;
    }
	
	//if ()
	return EXIT_SUCCESS;
}

int main(void)
{
	while (1) {
        char command_line[MAX_CMDLINE_SIZE];
        char *nl;
	    struct CommandLine MyCommandLine;

        MyCommandLine.argc = 0;

		// Print prompt.
		printf("sshell$ ");
		fflush(stdout);

		// Get command line.
		fgets(command_line, MAX_CMDLINE_SIZE, stdin);

		// Print command line if stdin is not provided by terminal.
		if (!isatty(STDIN_FILENO)) {
			printf("%s", command_line);
			fflush(stdout);
		}

		// Remove trailing newline from command line.
		nl = strchr(command_line, '\n');
		if (nl)
			*nl = '\0';

        parse_command_line(&MyCommandLine, command_line);
		if (has_errors(&MyCommandLine)) {
            return EXIT_FAILURE;
        }

        if (!strcmp(MyCommandLine.argv[0], "exit")) {
		    fprintf(stderr, "Bye...\n");
			display_exit_condition(command_line, EXIT_SUCCESS);
			break;
        } else if (!strcmp(MyCommandLine.argv[0], "cd")) {
		    // Valid input assumed.
            int error_status = (-1) * chdir(MyCommandLine.argv[1]);
			if (error_status == EXIT_FAILURE) {
				fprintf(stderr, "Error: cannot cd into directory\n");
            }
			display_exit_condition(command_line, error_status);
        } else if (!strcmp(MyCommandLine.argv[0], "pwd")) {
			char* directory = getcwd(NULL, 0);
			fprintf(stdout, "%s\n", directory);
            free(directory);
        } else {
            // Derived from lecture.
            pid_t pid = fork();
            
            if (pid == 0) {
                // Child process.
                execvp(MyCommandLine.argv[0], MyCommandLine.argv);
                perror("execvp");
                return EXIT_FAILURE;
            } else if (pid > 0) {
                // Parent process.
                int status;
                waitpid(pid, &status, 0);
                display_exit_condition(command_line,
                WEXITSTATUS(status));
            } else {
                perror("fork");
                return EXIT_FAILURE;
            }
        }
	}

	return EXIT_SUCCESS;
}
