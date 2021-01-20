#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// Provided assumptions.
#define MAX_CMDLINE_SIZE 512
#define MAX_NUM_ARGS 16
#define MAX_TOKEN_SIZE 32
#define MAX_NUM_PIPE_SIGNS 3

// Resolve user input into two unique elements.
struct CommandLine {
	int argc;
	char* argv[MAX_NUM_ARGS];
	char* specified_file;
};

// Display success of builtin functions.
void display_exit_condition(char* command_line, int exit_statuses[], int num_commands_piped)
{
	fprintf(stderr, "+ completed '%s' ", command_line);
	for (int i = 0; i < num_commands_piped; i++) {
		fprintf(stderr, "[%d]", exit_statuses[i]);
	}
	fprintf(stderr, "\n");
}

// void parse_command_line(struct CommandLine* MyCommandLine, char* command_line)
// {
// 	// Copy to preserve original input.
// 	char command_line_copy[MAX_CMDLINE_SIZE];
// 	strcpy(command_line_copy, command_line);

// 	const char arg_delim[2] = " ";

//     // Parsing so ">" completion statement prints correctly.
// 	char* redir_positionptr = strchr(command_line, '>');
// 	if (redir_positionptr) {
//         // Second copy to keep whole command line intact.
//         // Search through for output redirection
// 		char command_line_copy2[MAX_CMDLINE_SIZE];
// 		strcpy(command_line_copy2, command_line);

// 		const char redirect_delim[2] = ">";

// 		char* current_token2 = strtok(command_line_copy2, redirect_delim);
// 		strcpy(command_line_copy, current_token2);

// 		current_token2 = strtok(NULL, redirect_delim);
// 		(MyCommandLine)->specified_file = strtok(current_token2, arg_delim);
// 	}

//     // Derived from https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm.
//     char* current_token;

//     // Get the first token describing the command.
//     current_token = strtok(command_line_copy, arg_delim);
//     // Fill args with remaining tokens.
//     int i = 0;
//     while (current_token != NULL) {
// 		(MyCommandLine)->argv[i] = current_token;
// 		(MyCommandLine)->argc = (MyCommandLine)->argc + 1;
// 		current_token = strtok(NULL, arg_delim);
//         i++;
//    }
//    // Add the extra null argument.
//    (MyCommandLine)->argv[i] = NULL;
// }

int has_errors(struct CommandLine* MyCommandLine)
{
	if (MyCommandLine->argc > 16) {
		fprintf(stderr, "Error: too many process arguments\n");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

int main(void)
{
	while (1) {
        char command_line[MAX_CMDLINE_SIZE];
        char *nl;

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


        // int has_pipe = 1;
        char* commands[MAX_NUM_PIPE_SIGNS + 1];

        char command_line_copy[MAX_CMDLINE_SIZE];
        strcpy(command_line_copy, command_line);

        const char pipe_delim[2] = "|";
        char *token;
    
        token = strtok(command_line_copy, pipe_delim);
        // if (!strcmp(command_line, token)) {
        //     has_pipe = 0;
        // }
    
        int num_commands_piped = 0;
        while( token != NULL ) {
            commands[num_commands_piped] = token;
            token = strtok(NULL, pipe_delim);
            num_commands_piped++;
        }
		
		// for (int i = 0; i < num_commands_piped; i++) {
		// 	printf("%d: %s\n", i, commands[i]);
		// }

		struct CommandLine MyCommandLine[num_commands_piped];

		char command_line_copy1[num_commands_piped][MAX_CMDLINE_SIZE];
		// char* current_tokens[num_commands_piped];

		// int hasErrors[num_commands_piped];
		for (int i = 0; i < num_commands_piped; i++) {
			MyCommandLine[i].argc = 0;
			MyCommandLine[i].specified_file = NULL;
			
			// parse_command_line(&(MyCommandLine[i]), commands[i]);
			// Copy to preserve original input.
			// char command_line_copy1[MAX_CMDLINE_SIZE];
			strcpy(command_line_copy1[i], commands[i]);

			const char arg_delim[2] = " ";

			// Parsing so ">" completion statement prints correctly.
			char* redir_positionptr = strchr(commands[i], '>');
			if (redir_positionptr) {
				// Second copy to keep whole command line intact.
				// Search through for output redirection
				char command_line_copy2[MAX_CMDLINE_SIZE];
				strcpy(command_line_copy2, commands[i]);

				const char redirect_delim[2] = ">";

				char* current_token2 = strtok(command_line_copy2, redirect_delim);
				strcpy(command_line_copy1[i], current_token2);

				current_token2 = strtok(NULL, redirect_delim);
				MyCommandLine[i].specified_file = strtok(current_token2, arg_delim);
			}

			// Derived from https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm.
			char* current_token;

			// Get the first token describing the command.
			current_token = strtok(command_line_copy1[i], arg_delim);
			// Fill args with remaining tokens.
			int j = 0;
			while (current_token != NULL) {
				MyCommandLine[i].argv[j] = current_token;
				MyCommandLine[i].argc = MyCommandLine[i].argc + 1;
				current_token = strtok(NULL, arg_delim);
				j++;
			}
			// Add the extra null argument.
			MyCommandLine[i].argv[j] = NULL;

			// for (int j = 0; j < MyCommandLine[i].argc; j++) {
			// 	printf("%d: %s ", i, MyCommandLine[i].argv[j]);
			// }
			// printf("\n");
			// hasErrors[i] = has_errors(&(MyCommandLine[i]));
		}
		// for (int i = 0; i < num_commands_piped; i++) {
		// 	for (int j = 0; j < MyCommandLine[i].argc; j++) {
		// 		printf("%d: %s ", i, MyCommandLine[i].argv[j]);
		// 	}
		// 	printf("\n");
		// }

		if (!strcmp(MyCommandLine[0].argv[0], "exit")) {
			fprintf(stderr, "Bye...\n");
			int builtin_exit_status[1] = {EXIT_SUCCESS};

			display_exit_condition(command_line, builtin_exit_status, 1);
			break;
		} else if (!strcmp(MyCommandLine[0].argv[0], "cd")) {
			// Valid input assumed.
			int builtin_exit_status[1] = {(-1) * chdir(MyCommandLine[0].argv[1])};
			if (builtin_exit_status[0] == EXIT_FAILURE) {
				fprintf(stderr, "Error: cannot cd into directory\n");
			}
			display_exit_condition(command_line, builtin_exit_status, 1);
		} else if (!strcmp(MyCommandLine[0].argv[0], "pwd")){
			int builtin_exit_status[1] = {EXIT_SUCCESS};

			char* directory = getcwd(NULL, 0);
			fprintf(stdout, "%s\n", directory);
			free(directory);
			display_exit_condition(command_line, builtin_exit_status, 1);
		} else {
			pid_t pids[num_commands_piped];

			// Number of pipes to create.
			int fd[num_commands_piped - 1][2];
			int statuses[num_commands_piped];

			for (int i = 0; i < num_commands_piped; i++) {
                // Don't pipe the last argument.
				if (i < num_commands_piped - 1) {
					if(pipe(fd[i]) == -1) {
						perror("pipe");
						return EXIT_FAILURE;
					}
				}

				pids[i] = fork();

				if (pids[i] == 0) {
					// Child process.

					// Not the first in the pipeline. We do want to redirect the input.
					if (i > 0) {
						// Copy stdin to current file descriptor's readonly fd.
						dup2(fd[i-1][0], STDIN_FILENO);
						// Free a duplicated file descriptor.
						close(fd[i-1][0]);
					}

					if (i < num_commands_piped - 1) {
						// Copy stdout to next file descriptor's writeonly fd.
						dup2(fd[i][1], STDOUT_FILENO);
						// Free a duplicated file descriptor.
						close(fd[i][1]);
					}

					// final command to execute
					if (i == num_commands_piped - 1) {
						if (MyCommandLine[i].specified_file) {
                            // printf("u here\n");
							int fd;
							fd = open(MyCommandLine[i].specified_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
							dup2(fd, STDOUT_FILENO);
							close(fd);
							MyCommandLine[i].specified_file = NULL; // Make sure not to open the file again unless new redirect is called.
						}
					}

					execvp(MyCommandLine[i].argv[0], MyCommandLine[i].argv);
                    fprintf(stderr, "Error: command not found\n");
                    perror("execvp");
                    return EXIT_FAILURE;
				} else if (pids[i] > 0) {
					if (i > 0) {
						close(fd[i-1][0]);
					}
					if (i < num_commands_piped - 1) {
						close(fd[i][1]);
					}
                    // Parent process.
                    // waitpid(pids[i], &(statuses[i]), 0);
					// statuses[i] = WEXITSTATUS(statuses[i]);
                    // display_exit_condition(command_line, WEXITSTATUS(status));
                } else {
                    perror("fork");
                    return EXIT_FAILURE;
                }
			}
			for (int i = 0; i < num_commands_piped; i++) {
				waitpid(pids[i], &(statuses[i]), 0);
				statuses[i] = WEXITSTATUS(statuses[i]);
			}

			display_exit_condition(command_line, statuses, num_commands_piped);
		}
	}

	return EXIT_SUCCESS;
}
