#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// fork(), exec(), wait()

// given assumptions of limits
#define MAX_CMDLINE_SIZE 512
#define MAX_NUM_ARGS 16
#define MAX_TOKEN_SIZE 32

// for builtin functions show that it worked
// void display_success(char* cmd) 
// {
// 	fprintf(stderr, "+ completed '%s' ", cmd);
// 	fprintf(stderr, "[%i]\n", EXIT_SUCCESS);
// }

// resolve user input into thre unique elements
struct CommandLine {
	char command[MAX_CMDLINE_SIZE];
	int num_args;
	char args[MAX_NUM_ARGS][MAX_TOKEN_SIZE];
};

void parse_user_input(struct CommandLine *MyCommandLine, char* user_input) {
	// copy to preserve original input
	char user_input_copy[MAX_CMDLINE_SIZE];
    strcpy(user_input_copy, user_input);
    //https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
    
	const char delim[2] = " ";
    char* token;

    /* get the first token */
    token = strtok(user_input_copy, delim);
	strcpy(MyCommandLine->command, token);
    /* walk through other tokens */
    int f = 0;
	token = strtok(NULL, delim);
    while( token != NULL ) {
        // command->args = (char **) realloc(command->args, (command->num_args)*sizeof(char*));
		strcpy(MyCommandLine->args[f], token);
		MyCommandLine->num_args = MyCommandLine->num_args + 1;
        f++;
		token = strtok(NULL, delim);
   }
   
}

int main(void)
{
	char user_input[MAX_CMDLINE_SIZE];
	// char buf[MAX_CMDLINE_SIZE];
	struct CommandLine MyCommandLine;

	while (1) {
		MyCommandLine.num_args = 0;

	 	char *nl;
		//int retval;

		/* Print prompt */
		printf("sshell$ ");
		fflush(stdout);

		/* Get command line */
		fgets(user_input, MAX_CMDLINE_SIZE, stdin);

		// command.cmd = cmd;
		/* Print command line if stdin is not provided by terminal */
		if (!isatty(STDIN_FILENO)) {
			printf("%s", user_input);
			fflush(stdout);
		}

		/* Remove trailing newline from command line */
		nl = strchr(user_input, '\n');
		if (nl)
			*nl = '\0';

        parse_user_input(&MyCommandLine, user_input);
		// printf("%s\n%d\n", MyCommandLine.command, MyCommandLine.num_args);
		
	// 	/* Builtin command */
	// 	if (!strcmp(cmd, "exit")) {
	// 		fprintf(stderr, "Bye...\n");
	// 		display_success(cmd);
	// 		break;
	// 	} else if (cmd[0] == 'c' && cmd[1] == 'd') {
			
	// 		/* strkok modifies original string, so we keep copy for printing */
	// 		char cmd_copy[MAX_CMDLINE_SIZE];
	// 		strcpy(cmd_copy, cmd);
	// 		//https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
	// 		const char delim[2] = " ";
	// 		char* token;
			
	// 		/* get the first token */
	// 		token = strtok(cmd_copy, delim);
			
	// 		/* walk through other tokens */
	// 		token = strtok(NULL, delim);
	// 		//printf("%s\n", token);
	// 		chdir(token);

	// 		display_success(cmd);
	// 	}
	// 	else if (!strcmp(cmd, "pwd")) {
	// 		fprintf(stdout, "%s", getcwd(buf, MAX_CMDLINE_SIZE));
	// 		fprintf(stdout, "\n");
	// 		display_success(cmd);
	// 	} else {
	// 		/* Regular command */
	// 		//retval = system(cmd); // needs to be replaced w/ fork+exec+wait
	// 		//fprintf(stdout, "Return status value for '%s': %d\n", cmd, retval);
	// 	}
	}
	return EXIT_SUCCESS;
}