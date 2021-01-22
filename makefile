sshell : sshell.o
	gcc -Wall -Wextra -Werror -o sshell *.o

sshell.o : sshell.c 
	gcc -02 -Wall -Wextra -Werror -c -o sshell.o sshell.c

clean :
	rm -f sshell *.o *.txt
