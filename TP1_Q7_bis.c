#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include "string.h"
#include <time.h>
#include <fcntl.h>

#define READSIZE 50
#define PROMPTSIZE 50
#define COMMANDSIZE 10
#define MILLION 1000000

const char* start="Bienvenue dans le Shell ENSEA.\nPour quitter,tapez 'exit'\n";
const char* prompt="enseash % ";
const char* quit="Bye bye ...\n";
const char* failure="Error: invalid command\n";

char* consoleRead(){
	
	char * reading = malloc(READSIZE);
	int ret = read(STDIN_FILENO, reading, READSIZE);
	reading[ret-1] = '\0'; //sinon la fonction execlp ne fonctionne pas
	return reading;
}

static int execute(char* command, struct timespec tmps_start, struct timespec tmps_stop){
	char* cmd_exit = "exit";
	int status;
	
	// commande pour quitter le programme : on vérifie aussi la longueur pour pas qu'il prenne "exi" ou "exite" comme "exit"
		if(((strncmp(command,cmd_exit,strlen(command))==0)&&(strlen(command)==4)) || strlen(command)==0){ // la seule chaîne de caractère qui a une longueur nulle est "ctrl+D"
			write(STDOUT_FILENO, quit, strlen(quit));
			exit(EXIT_SUCCESS);
		}
	
	pid_t pid = fork(); //création d'un fils car la commande execvp contient un exit qui nous
						//ferait sortir du programme
	if (pid != 0){//père
		wait(&status);
		clock_gettime(CLOCK_REALTIME, &tmps_stop);
		return status;
	}
	else { //fils
		clock_gettime(CLOCK_REALTIME, &tmps_start);
		char * commandToken[COMMANDSIZE];
		char * filename;
		char * token;
	//GESTION DES REDIRECTIONS ">"
		token=strtok(command,">");
		if (token!=NULL){
			filename=strtok(NULL, ">");
			int descripteur=open(filename,O_CREAT|O_RDWR|O_CLOEXEC,S_IRWXG|S_IRWXU|S_IRWXO);//acces au fichier en écriture et en lecture pour tous
			dup2(descripteur,STDOUT_FILENO);
			close(descripteur);
		}
	
	//GESTION DES REDIRECTIONS "<"
		token=strtok(command,"<");
		if (token!=NULL){
			filename=strtok(NULL,"<");
			int descripteur=open(filename,O_CREAT|O_RDWR);
			close(STDIN_FILENO);
			dup2(descripteur,STDIN_FILENO);
			close(descripteur);
		}
		
	//GESTION DES ESPACES
		token = strtok(command," "); // extraction du premier token
		
		int i=0;
		while (token != NULL){ //strtok renvoie "NULL" si il n'y a plus de token
			commandToken[i]=token;
			token = strtok(NULL, " "); //prend le token suivant
			i++;
		}
		execvp(commandToken[0],commandToken);
		write(STDOUT_FILENO, failure, strlen(failure));
		exit(EXIT_FAILURE);
	}
} 
//programme pour vérifier si le fils s'est terminé normalement ou non
void checkStatus(int status, struct timespec start, struct timespec stop){
	char *prompt_retour=malloc(PROMPTSIZE);
	
	//code de retour
	if (WIFEXITED(status)){
		sprintf(prompt_retour, "ensea [exit:%d|%ld ms] %% ", WEXITSTATUS(status), (stop.tv_nsec - start.tv_nsec)/MILLION);
	}
	//signal	
	else if (WIFSIGNALED(status)) {
		sprintf(prompt_retour, "ensea [sign:%d|%ld ms] %% ", WTERMSIG(status), (stop.tv_nsec - start.tv_nsec)/MILLION);
	}
	
	else {
		sprintf(prompt_retour,"ensea %% ");
	}
	
	write(STDOUT_FILENO, prompt_retour, strlen(prompt_retour));
}


int main(){
	char* command;
	int status;
	struct timespec tmps_start, tmps_stop;

	write(STDOUT_FILENO, start, strlen(start));
	write(STDOUT_FILENO, prompt, strlen(prompt));
	
	while(1){
		//REPL
		command = consoleRead();
		status = execute(command, tmps_start, tmps_stop);
		checkStatus(status, tmps_start, tmps_stop);
	}
	exit(EXIT_SUCCESS);
}
