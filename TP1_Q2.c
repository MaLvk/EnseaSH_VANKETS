#include <unistd.h>
#include <stdlib.h>
#include "string.h"
#include <sys/wait.h>

#define READSIZE 50

const char* start="Bienvenue dans le Shell ENSEA.\nPour quitter,tapez 'exit'\n";
const char* prompt="enseash % ";

char* consoleRead(){
	char * reading = malloc(READSIZE);
	
	int ret = read(STDIN_FILENO, reading, READSIZE);
	reading[ret-1] = '\0'; //sinon la fonction execlp ne fonctionne pas
	return reading;
}

void execute(char *command){
	int status;
	pid_t pid=fork();
	
	if (pid != 0){
		wait(&status);
	}
	
	else {
		execlp(command,command, (char *)NULL);
		exit(EXIT_FAILURE);
	}
}

int main(void){
	char *command;
	write(STDOUT_FILENO, start, strlen(start));
	write(STDOUT_FILENO, prompt, strlen(prompt));
	while(1){
		//REPL
		command=consoleRead();
		execute(command);
		write(STDOUT_FILENO, prompt, strlen(prompt));
	}
	exit(EXIT_SUCCESS);
}
