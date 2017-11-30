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

int i =0; // Global variables are EVIL!!!!
void doSomeWork(char *name){
	const int NUM_TIMES = 5;
	for(; i<NUM_TIMES;i++){
		sleep(rand() %4);
		printf("Done pass %d for %s\n",i,name);
	}
}

int main(int argc, char *argv[]){
	printf("The Proces ID: %d\n",(int) getpid());

	pid_t pid = fork();
	srand((int) pid);
	printf("fork returned: %d\n",(int) pid);


	if(pid <0){
		perror("Fork failed ");
	}

	if(pid == 0){
		printf("I am the child with pid = %d\n",(int)getpid());
		doSomeWork("Child");
		printf("Child exiting \n");
		exit(0);
	}

	// We must be the parent
	printf("I am the parent: waiting for child to end. \n");
	doSomeWork("Parent");
	wait(NULL);
	printf("Parent ending.\n");



	return 0;
}
