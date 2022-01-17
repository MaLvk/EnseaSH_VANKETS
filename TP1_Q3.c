#include <unistd.h>
#include <stdlib.h>
#include "string.h"
#include <sys/wait.h>

#define READSIZE 50

const char* start="Bienvenue dans le Shell ENSEA.\nPour quitter,tapez 'exit'\n";
const char* prompt="enseash % ";
const char* quit="Bye bye ...\n";

char* consoleRead(){
	char * reading = malloc(READSIZE);
	
	int ret = read(STDIN_FILENO, reading, READSIZE);
	reading[ret-1] = '\0'; //sinon la fonction execlp ne fonctionne pas
	return reading;
}

void execute(const char *command){
	char* cmd_exit = "exit";
	int status;
	
	// commande pour quitter le programme : on vérifie aussi la longueur pour pas qu'il prenne "exi" ou "exite" comme "exit"
		if(((strncmp(command,cmd_exit,strlen(command))==0)&&(strlen(command)==4)) || strlen(command)==0){ // la seule chaîne de caractère qui a une longueur nulle est "ctrl+D"
			write(STDOUT_FILENO, quit, strlen(quit));
			exit(EXIT_SUCCESS);
		}

	pid_t pid = fork(); //création d'un fils car la commande execlp contient un exit qui nous
				        //ferait sortir du programme
	if (pid != 0){ // père
		wait(&status);
	}
	
	else { //fils
		execlp(command,command, (char *)NULL);
		exit(EXIT_FAILURE);
	}
}

int main(void){
	const char *command;

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

