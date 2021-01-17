#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// fork(), exec(), wait()

#define CMDLINE_MAX 512
#define ARGS_MAX 16
#define TOKEN_MAX 32

// for builtin fns show that it worked
void display_success(char* cmd) 
{
	fprintf(stderr, "+ completed '%s' ", cmd);
	fprintf(stderr, "[%i]\n", EXIT_SUCCESS);
}

int main(void)
{
	char cmd[CMDLINE_MAX];
	char buf[CMDLINE_MAX];
	while (1) {
		char *nl;
		//int retval;

		/* Print prompt */
		printf("sshell$ ");
		fflush(stdout);

		/* Get command line */
		fgets(cmd, CMDLINE_MAX, stdin);

		/* Print command line if stdin is not provided by terminal */
		if (!isatty(STDIN_FILENO)) {
			printf("%s", cmd);
			fflush(stdout);
		}

		/* Remove trailing newline from command line */
		nl = strchr(cmd, '\n');
		if (nl)
			*nl = '\0';

		/* Builtin command */
		if (!strcmp(cmd, "exit")) {
			fprintf(stderr, "Bye...\n");
			display_success(cmd);
			break;
		} else if (cmd[0] == 'c' && cmd[1] == 'd') {
			
			/* strkok modifies original string, so we keep copy for printing */
			char cmd_copy[CMDLINE_MAX];
			strcpy(cmd_copy, cmd);
			//https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
			const char delim[2] = " ";
			char* token;
			
			/* get the first token */
			token = strtok(cmd_copy, delim);
			
			/* walk through other tokens */
			token = strtok(NULL, delim);
			//printf("%s\n", token);
			chdir(token);

			display_success(cmd);
		}
		else if (!strcmp(cmd, "pwd")) {
			fprintf(stdout, "%s", getcwd(buf, CMDLINE_MAX));
			fprintf(stdout, "\n");
			display_success(cmd);
		} else {
			/* Regular command */
			//retval = system(cmd); // needs to be replaced w/ fork+exec+wait
			//fprintf(stdout, "Return status value for '%s': %d\n", cmd, retval);
		}
	}

	return EXIT_SUCCESS;
}