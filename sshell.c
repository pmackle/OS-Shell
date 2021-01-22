#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

// Provided assumptions.
#define MAX_CMDLINE_SIZE 512
#define MAX_NUM_ARGS 16
#define MAX_TOKEN_SIZE 32
#define MAX_NUM_PIPE_SIGNS 3
#define NUM_STRING_VARS 26

enum parsing_errors{args, missing, no_output, permissions, output_location, none};

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

void parse_command_line(struct CommandLine* MyCommandLine, char* command_line, int i, char command_line_copy_whitespaces[MAX_NUM_PIPE_SIGNS][MAX_CMDLINE_SIZE], enum parsing_errors *parsing_error, int num_commands_piped)
{
	MyCommandLine->argc = 0;
	MyCommandLine->specified_file = NULL;

	strcpy(command_line_copy_whitespaces[i], command_line);

	const char arg_delim[2] = " ";

	char* redir_positionptr = strchr(command_line, '>');
	if (redir_positionptr) {
		// Second copy to keep whole command line intact.
		// Search through for output redirection.
		char command_line_copy_redirects[MAX_CMDLINE_SIZE];
		strcpy(command_line_copy_redirects, command_line);

		const char redirect_delim[2] = ">";

		char* redirect_token = strtok(command_line_copy_redirects, redirect_delim);

        // Nothing to be redirected.
        if (!redirect_token) {
            *parsing_error = missing;
        }

		strcpy(command_line_copy_whitespaces[i], redirect_token);

		redirect_token = strtok(NULL, redirect_delim);
		if (redirect_token) {
            if (i != num_commands_piped - 1) {
                *parsing_error = output_location;
            } else {
                int fd = open(redirect_token, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                if (fd == -1) {
                    *parsing_error = permissions;
                } else {
                    // Delete the file. We opened it up just to see if we could. Meaningful actions come later.
                    remove(redirect_token);
                }
                close(fd);

                MyCommandLine->specified_file = strtok(redirect_token, arg_delim);

                // If the redirect filename is comprised exclusively of whitespace.
                if (!MyCommandLine->specified_file) {
                    *parsing_error = no_output;
                }
            }
		} else {
			*parsing_error = no_output;
		}
	}

	// Derived from https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm.
	char* current_token;

	// Get the first token describing the command.
	current_token = strtok(command_line_copy_whitespaces[i], arg_delim);
	// Fill args with remaining tokens.

    // Nothing to be done.
    if (!current_token) {
        *parsing_error = missing;
    }

	int j = 0;
	while (current_token != NULL) {
		if (current_token[0] == '$') {
			// Create enough space to fit any token.
			MyCommandLine->argv[j] = (char*)malloc((MAX_TOKEN_SIZE + 1)*sizeof(char));
			int len_current_token = strlen(current_token);
			int k;
			for(k = 0; k < len_current_token; k++){
					MyCommandLine->argv[j][k] = current_token[k];
			}
			MyCommandLine->argv[j][k] = '\0';
		} else {
			MyCommandLine->argv[j] = current_token;
		}
		// Finalize addition of new token to command.
		MyCommandLine->argc = MyCommandLine->argc + 1;
		current_token = strtok(NULL, arg_delim);
		j++;
	}
	// Add the extra null argument for exec.
	MyCommandLine->argv[j] = NULL;
}

void command_has_errors(struct CommandLine* MyCommandLine, enum parsing_errors *parsing_error)
{
	if (MyCommandLine->argc > MAX_NUM_ARGS) {
		*parsing_error = args;
	}
}

int main(void)
{
	char string_vars[NUM_STRING_VARS][MAX_TOKEN_SIZE];
    // When the shell launches, all string vars are empty.
	for (int i = 0; i < NUM_STRING_VARS; i++) {
	  string_vars[i][0] = '\0';
	}

// Restart the shell if invalid commands are present in the pipeline.
continue_label:
	while (1) {
		enum parsing_errors parsing_error = none;

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

        // Check for the edge cases where nothing is entered, or exactly the first character the user enters is a pipe or redirect.
        int edge_cases_fail = 0;

        if (strlen(command_line) == 0) {
            edge_cases_fail = 1;
        }
        if (command_line[0] == '>' || command_line[0] == '|') {
            edge_cases_fail = 1;
        }

        if (edge_cases_fail) {
            fprintf(stderr, "Error: missing command\n");
            goto continue_label;
        }

		// For separating each command in pipeline into different array indeces.
        char* commands[MAX_NUM_PIPE_SIGNS + 1];

		// The whole command line as inputted by the user.
        char command_line_copy_pipes[MAX_CMDLINE_SIZE];
        strcpy(command_line_copy_pipes, command_line);

		// Seperate commands by pipe symbol.
        const char pipe_delim[2] = "|";
        char *token;
        token = strtok(command_line_copy_pipes, pipe_delim);

        int num_commands_piped = 0;
        while( token != NULL ) {
            commands[num_commands_piped] = token;
            token = strtok(NULL, pipe_delim);
            num_commands_piped++;
        }

		// Ignore unused pipe.
		int len_command_line = strlen(command_line);
		if(command_line[len_command_line-1] == '|'){
			num_commands_piped--;
		}
		// Our solution for piping requires us to parse each piped command into an individual CommandLine struct. This allows us to easily manipulate and access each command in the pipeline at any point in time.
		struct CommandLine MyCommandLine[num_commands_piped];

		// Copy the command line to incrementally destroy. Preserve the original while finding all the pipes in copy1.
		char command_line_copy_whitespaces[MAX_NUM_PIPE_SIGNS][MAX_CMDLINE_SIZE];

		for (int i = 0; i < num_commands_piped; i++) {
			// Initialize an empty list of commands.
			MyCommandLine[i].argc = 0;
			MyCommandLine[i].specified_file = NULL;

			parse_command_line(&MyCommandLine[i],commands[i], i, command_line_copy_whitespaces, &parsing_error, num_commands_piped);
			command_has_errors(&MyCommandLine[i], &parsing_error);
			// What's causing the errant behavior?
			switch (parsing_error) {
				case args:
					fprintf(stderr, "Error: too many process arguments\n");
					break;
				case missing:
					fprintf(stderr, "Error: missing command\n");
					break;
				case no_output:
					fprintf(stderr, "Error: no output file\n");
					break;
				case permissions:
					fprintf(stderr, "Error: cannot open output file\n");
					break;
				case output_location:
					fprintf(stderr, "Error: mislocated output redirection\n");
					break;
				default:
					break;
			}

			if (parsing_error != none) {
				// The nested nature of this loop requires us to hop to the right exit case in this first of two similar cases.
				goto continue_label;
			}

			for (int j = 0; j < MyCommandLine[i].argc; j++) {
				if (MyCommandLine[i].argv[j][0] == '$') {
					// The environmental variable must be exactly one character in the lowercase Latin alphabet.
					if (strlen(MyCommandLine[i].argv[j]) != 2 || (MyCommandLine[i].argv[j][1] < 'a' || MyCommandLine[i].argv[j][1] > 'z')) {
						fprintf(stderr, "Error: invalid variable name\n");
						// The second of the aforementioned cases.
						goto continue_label;
					} else {
						int location = MyCommandLine[i].argv[j][1] - 'a';
						// Delete whatever the variable initially was.
						free(MyCommandLine[i].argv[j]);
                        MyCommandLine[i].argv[j] = string_vars[location];
					}
				}
			}
		}

		if (!strcmp(MyCommandLine[0].argv[0], "exit")) {
			fprintf(stderr, "Bye...\n");
			int builtin_exit_status[1] = {EXIT_SUCCESS};
			display_exit_condition(command_line, builtin_exit_status, 1);
			break;
		} else if (!strcmp(MyCommandLine[0].argv[0], "set")) {
			if (MyCommandLine[0].argc == 1 || strlen(MyCommandLine[0].argv[1]) != 1 || (MyCommandLine[0].argv[1][0] < 'a' || MyCommandLine[0].argv[1][0] > 'z')) {
				fprintf(stderr, "Error: invalid variable name\n");
				int builtin_exit_status[1] = {EXIT_FAILURE};
				display_exit_condition(command_line, builtin_exit_status, 1);
			} else {
				// Assumes input for this builtin is correct: 3 correctly-sized args.
				int builtin_exit_status[1] = {EXIT_SUCCESS};

				int location = MyCommandLine[0].argv[1][0] - 'a';
				int token_length = strlen(MyCommandLine[0].argv[2]);
				int x;
				for (x = 0; x < token_length; x++) {
					// Copy the second set arg into our list of simple environment variables.
					string_vars[location][x] = MyCommandLine[0].argv[2][x];
				}
				string_vars[location][x] = '\0';

				display_exit_condition(command_line, builtin_exit_status, 1);
			}
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

            // Basic methodology derived from https://stackoverflow.com/questions/8389033/implementation-of-multiple-pipes-in-c.
			for (int i = 0; i < num_commands_piped; i++) {
                // Don't pipe the last argument.
				if (i < num_commands_piped - 1) {
					if(pipe(fd[i]) == -1) {
						perror("pipe");
						return EXIT_FAILURE;
					}
				}

				pids[i] = fork();
				if (pids[i] == 0) { // Child process.
					// Not the first in the pipeline. We do want to redirect the input.
					if (i > 0) {
						// Copy stdin to current file descriptor's readonly fd.
						dup2(fd[i-1][0], STDIN_FILENO);
						// Free up the right fd from the duplicated file descriptor table.
						close(fd[i-1][0]);
					}
					if (i < num_commands_piped - 1) {
						// Copy stdout to next file descriptor's writeonly fd.
						dup2(fd[i][1], STDOUT_FILENO);
						// Free up the right fd from the duplicated file descriptor table.
						close(fd[i][1]);
					}
					// Final command to execute.
					if (i == num_commands_piped - 1) {
						if (MyCommandLine[i].specified_file) {
							int fd;
							fd = open(MyCommandLine[i].specified_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
							dup2(fd, STDOUT_FILENO);
							close(fd);
							MyCommandLine[i].specified_file = NULL; // Make sure not to open the file again unless new redirect is called.
						}
					}

					execvp(MyCommandLine[i].argv[0], MyCommandLine[i].argv);
                    fprintf(stderr, "Error: command not found\n");
                    return EXIT_FAILURE;
				} else if (pids[i] > 0) { // Parent process.
					// Close open fd's in parent copy of file descriptor table.
					if (i > 0) {
						close(fd[i-1][0]);
					}
					if (i < num_commands_piped - 1) {
						close(fd[i][1]);
					}
                } else {
                    return EXIT_FAILURE;
                }
			}

            // Check last character of entry last.
            int len_command_line = strlen(command_line);
            if (command_line[len_command_line - 1] == '>' || command_line[len_command_line - 1] == '|') {
                fprintf(stderr, "Error: missing command\n");
                goto continue_label;
            }

			// Collect all exit statuses after completion of every command, to print sequentially.
			for (int i = 0; i < num_commands_piped; i++) {
				waitpid(pids[i], &(statuses[i]), 0);
				statuses[i] = WEXITSTATUS(statuses[i]);
			}

			display_exit_condition(command_line, statuses, num_commands_piped);
		}
	}

	return EXIT_SUCCESS;
}
