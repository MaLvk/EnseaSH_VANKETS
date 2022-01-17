#include <unistd.h>
#include <stdlib.h>
#include "string.h"

const char* start="Bienvenue dans le Shell ENSEA.\nPour quitter,tapez 'exit'\n";
const char* prompt="enseash % ";

int main(void){
	write(STDOUT_FILENO, start, strlen(start));
	write(STDOUT_FILENO, prompt, strlen(prompt));
	exit(EXIT_SUCCESS);
}
