/*
 * fork.c
 *
 *  Created on: Sep 20, 2017
 *      Author: am
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	// printf("The Proces ID: %d\n",(int) getpid());
	printf("Before Fork ");



	fflush(stdout);
	pid_t pid = fork();
	// printf("fork returned: %d\n",(int) pid);

	if(pid <0){
		perror("Fork failed ");
	}
	if(pid == 0){
		printf("I am the child with pid = %d\n",(int)getpid());
	}else if (pid > 0){
		printf("I am the parent\n");

	}


	exit(0);
	return 0;
}
