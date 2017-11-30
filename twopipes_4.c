#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>



static int pipes[20];
static int pIdx;
static char **cmds[];
static int pNum;

void newchild(char **cmds[],int cmdNum) {

    if(cmdNum > 0){

        if (fork() == 0) {

            pid_t pid;
            pid = getpid();

            cmdNum--;
            if (cmdNum > 0) {
                newchild(&(*(cmds-1)), cmdNum);
            }

             printf("--> Inside of a chile process with PID = %d, cmdNum = %d, cmd = %s\n", pid,cmdNum,*cmds[0]);


            if(cmdNum <1)
                pIdx = pNum -1;
            else
                pIdx = pNum - (cmdNum*2);
             printf(" *** pNum = %d, cmdNum = %d, pIdx = %d *** \n",pNum,cmdNum,pIdx);


            if(cmdNum == 0){
                dup2(pipes[pIdx],1);
            }
            else if(cmdNum % 2 !=0){
                dup2(pipes[pIdx], 0);
                if(pIdx > 1)
                    dup2(pipes[pIdx-1], 1);
            }
            else {
                //dup2(pipes[2], 0);
                dup2(pipes[pIdx], 0);
                if(pIdx >0)
                    dup2(pipes[pIdx-1], 1);

            }


            for (int i = 0; i < pNum; ++i)
                 close(pipes[i]);


             // printf(" >> cmd: %s\n",*cmds[0]);
             execvp(*cmds[0], *cmds);


        }


    }

}

int main(int argc, char *argv) {
    int status;
    int i;
    //echo dog feet | tr o a | tr e o | tr d c | tr t d | tr g t
    // ans: cat food
    char *arg1[] = {"echo", "dog", "feet", NULL };
    char *arg2[] = {"tr", "o", "a", NULL};
    char *arg3[] = {"tr", "e", "o", NULL};
    char *arg4[] = {"tr", "d", "c", NULL};
    char *arg5[] = {"tr", "t", "d", NULL};
    char *arg6[] = {"tr", "g", "t", NULL};

    char **cmds[] = {arg1,arg2, NULL };


    int cmdNum =  sizeof(cmds)/sizeof(char*) -1;
    pNum=2*(sizeof(cmds)/sizeof(char*) -2);
     printf(" *-*-* size of cmds array = %d, pNum = %d\n", cmdNum,pNum);
    // printf(" *-*-* cmds = %s\n", *cmds[cmdNum-1]);


    for (int i = 0; i < cmdNum-1; ++i) {
        pipe(pipes + i*2); // sets up 1st pipe
    }

    newchild(&cmds[cmdNum-1],cmdNum);


    // only the parent gets here and waits for 3 children to finish
    for (int i = 0; i < pNum ; ++i) {
        close(pipes[i]);
    }


    for (i = 0; i < 3; i++)
        wait(&status);


}

