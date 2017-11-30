/*
 * fork.c
 *
 *  Created on: Sep 20, 2017
 *      Author: am
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	printf("The Proces ID: %d\n",(int) getpid());

	pid_t pid = fork();
	printf("fork returned: %d\n",(int) pid);

	if(pid <0){
		perror("Fork failed ");
	}

	if(pid == 0){
		printf("I am the child with pid = %d\n",(int)getpid());
		sleep(5);
		printf("Child exiting \n");
		exit(0);
	}

	// We must be the parent
	printf("I am the parent: waiting for child to end. \n");
	wait(NULL);
	printf("Parent ending.\n");



	return 0;
}
