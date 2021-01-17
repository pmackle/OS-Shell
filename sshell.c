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

struct my_command {
	char cmd[CMDLINE_MAX];
	int num_args;
	char args[ARGS_MAX][TOKEN_MAX];
};

void parse_command(struct my_command *command, char* user_input) {
	char user_input_copy[CMDLINE_MAX];
    strcpy(user_input_copy, user_input);
    //https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
    const char delim[2] = " ";
    char* token;

    /* get the first token */
	
    token = strtok(user_input_copy, delim);
	strcpy(command->cmd, token);
    printf("%s\n", token);
    /* walk through other tokens */
    int f = 0;
    while( token != NULL ) {
        // command->args = (char **) realloc(command->args, (command->num_args)*sizeof(char*));
		token = strtok(NULL, delim);
		//printf("%s\n", token);
		if (token != NULL){
			strcpy(command->args[f], token);
			command->num_args = command->num_args + 1;

		}
        f++;
   }
   
}

int main(void)
{
	char cmd_str[CMDLINE_MAX];
	// char buf[CMDLINE_MAX];
	struct my_command command;

	while (1) {
		command.num_args = 0;

	 	char *nl;
	// 	//int retval;

		/* Print prompt */
		printf("sshell$ ");
		fflush(stdout);

		/* Get command line */
		fgets(cmd_str, CMDLINE_MAX, stdin);


		// command.cmd = cmd;
		/* Print command line if stdin is not provided by terminal */
		if (!isatty(STDIN_FILENO)) {
			printf("%s", cmd_str);
			fflush(stdout);
		}

		/* Remove trailing newline from command line */
		nl = strchr(cmd_str, '\n');
		if (nl)
			*nl = '\0';

        parse_command(&command, cmd_str);

        printf("command = %s\n num_args = %d\n", command.cmd, command.num_args);
		
	// 	/* Builtin command */
	// 	if (!strcmp(cmd, "exit")) {
	// 		fprintf(stderr, "Bye...\n");
	// 		display_success(cmd);
	// 		break;
	// 	} else if (cmd[0] == 'c' && cmd[1] == 'd') {
			
	// 		/* strkok modifies original string, so we keep copy for printing */
	// 		char cmd_copy[CMDLINE_MAX];
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
	// 		fprintf(stdout, "%s", getcwd(buf, CMDLINE_MAX));
	// 		fprintf(stdout, "\n");
	// 		display_success(cmd);
	// 	} else {
	// 		/* Regular command */
	// 		//retval = system(cmd); // needs to be replaced w/ fork+exec+wait
	// 		//fprintf(stdout, "Return status value for '%s': %d\n", cmd, retval);
	// 	}
	}
	free(command.args);
	return EXIT_SUCCESS;
}