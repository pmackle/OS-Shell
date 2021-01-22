
# ECS 150 Project #1 - Simple Shell

## Table of Contents

-  [Introduction](#introduction)

-  [Overview](#overview)

-  [High-Level Process](#high-level-process)

-  [Saving the Command Line](#saving-the-command-line)

-  [Built-In Commands](#built-in-commands)

-  [Pipelining](#pipelining)

-  [Output Redirection](#output-redirection)

-  [Error Handling](#error-handling)

-  [Sources](#sources)

  

### Introduction
The Simple Shell project is an introductory assignment designed to familiarize\
us with the inner workings of a UNIX-like operating system. Our simple shell\
replicates the way `system()` handles user-generated commands by navigating,\
writing, and reading the system's directory. It is capable of `pwd`, `cd`,\
`exit`, output redirection, piping, setting environmental variables, and all\
the other functions that `execvp` provides


### Overview

#### High-Level Process
The implementation of the program is as follows:
1. Save the command line into a struct and parse it into easily-accessible\
arguments.
2. For each valid action the user inputs, call their respective functions\
and output their results to the user.
3. Allow for users to output their request into a file by attaching the file\
name to the struct and opening it.
4. Create a flexible pipeline to redirect input and output to/from process to\
process.
5. Catch both parsing and launching errors in their respective areas and\
identify them with enums.


#### Saving the Command Line
To simplify user input into something we can manage, we read in the\
user-inputted command line and place it in a **struct** called `CommandLine`.\
It made more sense to use a struct rather than an array because there were\
multiple variables to account for, and it translated  well when we expanded \
our program to include piping.

Our `parse_command_line` function is responsible for processing a lot of the\
user input and organizing it into easily accesible locations. By passing in a \
`CommandLine` pointer, we can separate the arguments into individual arguments \
in the `CommandLine.argv` array and return to the main function with the \
updated values.

To actually parse the string, we use whitespace as a delimiter and `strtok()` \
for tokenizing the command line into individual arguments that we could then \
manipulate. 
  

#### Built-In Commands
For the built-in-commands, we have a series of conditional statements that take\
advantage of the assumption that they will never be called incorrectly. We \
compare the first argument of our now-parsed `CommandLine.argv` array of \
pointers with the string literals to determine which course of action to take.

The `exit` branch is fairly straightforward. We print out the `Bye...` \
statement, pass the `EXIT_SUCCESS` macro into an array of exit statuses, and \
pass that through a `display_exit_condition()` function. The function is \
frequently called throughout and allows for improved readability. 

The `set` branch is somewhat more complex, as it has to account for setting \
variables' values that have to persist through loop iterations while also \
having the ability to be dynamic and mutable. We check if the set variable is \
valid by checking to see if it fits the argument count requirement and if the \
character inputted is alphabetical. We then take advantage of the char's \
identity as an ASCII integer value and use it to displace the `argv` array and \
allocate that to a `location` variable. Using a similar technique to the parser\
function, we use tokens to assign each of the set environmental variables into \
the `string_vars` array.

If the argument is `cd`, we use the `chdir` function to change directories. It \
should return 0 on success, and 1 otherwise. If it returns an error, we print \
an error.

Like `cd`, `pwd` is a fairly simple call to a function that returns the \
directory as a string, which we then print out and free.


#### Output Redirection
We recognize redirected output as a part of the command object. This entails \
our retrieval of the command to the left of the redirect delimiter and \
destination file given to the right via `strtok()`. When executing, if our \
command has a specified redirect file retrieved in this manner (`NULL`, \
otherwise), we simply route the contents of `stdout`.

#### Pipelining
Because we previously built a framework around seperating the command line\
arguments, pipelining became an easier process to work with and debug. We keep\
track of `num_commands_piped` and use that to determine how many `CommandLine`\
structs there will be and how many "real" arguments we need to process. We have\
a for loop that allows us to go through multiple iterations of executing \
functions and stops once it executed each `CommandLine` struct. 

We track all processes in `pids[]` and kick off a child process for each to \
execute each command. Depending on where we are (`i`) in the pipeline, we \
redirect its output to its child and/or take output from its parent via `dup2()`

 Crucially, once we are done with the file descriptor in question, we free up \
the corresponding descriptor in both the parent process' file descriptor table \
***and*** the copy created in the child process to ensure that the next command\
 in the pipeline reads from and writes to the right place.

Once the forked processes are completed for ***every*** command in the\
pipeline, we leverage `wait()` to collect and output the exit status of each \
process in `pids`.
  

#### Error Handling
In the spirit of true C programmers, we went with using an **enum** to describe\
all possible parsing errors. After each iteration of parsing, we compare the \
raised error `parsing_error` list of possible errors with a switch statement. \
This switch, combined with careful placements of block in our parsing function \
to set the value of `parsing_error` allows for us to resolve priority conflicts\
between all errors in the command line. 

The beauty that comes with this approach to error handling lies in what we \
believe to be **sensible** `goto` statements. The statements are placed at \
certain error-catching locations and start the main loop over from the \
beginning to receive the user's next input. The nature of nested loops makes \
it  infeasible to break out of or continue to the points you need them to, and \
the `goto` statement allows for us to **safely** start from the beginning by \
bypassing loop restrictions.
  

### Sources
Cited in code.
