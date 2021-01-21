
# ECS_150 Project #1 - Simple Shell

## Table of Contents

-  [Introduction](#introduction)

-  [Implementations](#implementations)

-  [Structures](#structures)

-  [Parsing the Command Line](#parsing-the-command-line)

-  [Built-In Commands](#built-in-commands)

-  [Pipelining](#pipelining)

-  [Error Handling](#error-handling)

-  [Parsing Errors](#parsing-errors)

-  [Launching Errors](#launching-errors)

-  [Testing](#testing)

-  [Limitations](#limitations)

-  [Sources](#sources)

  

### Introduction

The Simple Shell project is an introductory assignment designed to familiarize us with the inner workings of a UNIX based operating system. In summary, our simple shell replicates the way `system()` handles user-generated commands by navigating, writing, and reading the system's directory. It is capable of `pwd`, `cd`, and `exit`, 
output redirection, piping, setting environmental variables, and the other functions that `execvp` provides.


### Implementations

  

#### Structure
To simplify resolving user input into something we could manage, we wrote the user-inputted `CommandLine` as a struct. It made more sense to use a struct rather than an array because there were multiple variables to account for, and it translated over well for when we increased the scope of our program with piping. 
  

#### Parsing the Command Line
This is where the bulk of where processing our command line comes in and helped make actions manageable as we scaled up. By passing in a `CommandLine` pointer, we can separate the arguments into individual pieces, assign them to an array property in the structure, and return to the main function with the updated values.

To actually parse the string, we use whitespace as a delimiter and `strtok()` tokenizes the command line into individual arguments that we could then manipulate. 

While parsing, we also check for a few special circumstances that can affect the output of what the user inputs. For example, we check for a redirect character and update the struct to recognize that it will need to output the result of the arguments into a file. The parsing function seemed an appropriate place for setting the environmental variables, and we make it mutable by allocating enough space for the token and freeing the space later on in the program. 
  

#### Built-In Commands
For the built-in-commands, we had a series of if-else statements that took advantage of the assumption that they would never be called incorrectly. We compare the first argument of our now-parsed `CommandLine.argv` array of pointers with the string literals to determine which course of action to take.

The `exit` branch is fairly straightforward. We print out to the `Bye...` statement, pass the `EXIT_SUCCESS` macro into an array of exit statuses, and pass that through a `display_exit_condition()` function. The function is frequently called throughout the code and allows for improved readability. 

The `set` branch is somewhat more complex, as it has to account for setting variables' values that have to persist through loop iterations while also having the ability to be dynamic and mutable. We check if the set variable is valid by checking to see if it fits the argument count requirement and if the character inputted is alphabetical. We then take advantage of the char's identity as an ASCII integer value and use it to displace the `argv` array and allocate that to a `location` variable. Using a similar technique to the parser function, we use tokens to assign each of the set environmental variables into the `string_vars` array.

If the argument is `cd`, we use the `chdir` function to change directories. It should return 0 on success, and 1 otherwise. If it returns an error, we print an error.

Like `cd`, `pwd` is a fairly simple call to a function that returns the directory as a string, which we then print out and free. 

#### Pipelining

  

### Error Handling

  

#### Parsing Errors

  

#### Launching Errors

  
  

### Testing

  
  

### Limitations

  
  

### Sources