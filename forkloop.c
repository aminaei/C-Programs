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

int main(int argc, char const *argv[])
{
	for (int i = 1; i <=3; i++)
	{
		fork();
		printf("%d: My PID = %d\n",i, (int)getpid());
	}
	sleep(1);
	printf("--> My PID = %d\n",(int)getpid());
	return 0;
}