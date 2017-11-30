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


        if (fork() == 0) {

            pid_t pid;
            pid = getpid();
            //printf("I am the child with PID = %d\n", (int) getpid());
            //printf(" >>[%d]: cmd: %s\n",(int) getpid(),*cmds[0]);

            cmdNum--;
            if (cmdNum > 0) {
                newchild(&(*(cmds-1)), cmdNum);
            }
             //printf("--> Inside of a chile process with PID = %d, cmdNum = %d, cmd = %s\n", pid,cmdNum,*cmds[0]);

            if(cmdNum <1)
                pIdx = pNum -1;
            else
                pIdx = pNum - (cmdNum*2);
            //printf(" ***[%d] pNum = %d, cmdNum = %d, pIdx = %d *** \n",(int) getpid(),pNum,cmdNum,pIdx);

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


             //printf(" >> cmd: %s\n",*cmds[0]);
             execvp(*cmds[0], cmds[0]);

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


    char **cmds[] = {arg1,arg2,arg3,arg4,arg5,arg6, NULL };
//    char **cmds[] = {arg1,arg2, NULL };

    char *childArr[1024]={NULL};
    char *parentArr[1024]={NULL};

    int cmdNum =  sizeof(cmds)/sizeof(char*) -1;
    pNum=2*(sizeof(cmds)/sizeof(char*) -2);
     printf(" *-*-* size of cmds array = %d, pNum = %d\n", cmdNum,pNum);
    // printf(" *-*-* cmds = %s\n", *cmds[cmdNum-1]);


    for (int i = 0; i < cmdNum-1; ++i) {
        pipe(pipes + i*2); // sets up 1st pipe
    }

    if (fork() == 0) {
        // fork second child (to execute grep)
        //printf("I am the child with PID = %d\n", (int) getpid());


        int j=0;
        while(*(*(cmds+(cmdNum-2))+j)){
            //printf(" >->-> parentArr: %s\n",*(*(cmds+(cmdNum-1))+j));
            childArr[j] = *(*(cmds+(cmdNum-2))+j);
            childArr[++j]=NULL;

        }

        //*cmds=*cmds[cmdNum-2];
        //printf(" >>> childArr = %s, %s \n",*childArr,*(childArr+1));



        cmdNum =cmdNum-2;
        //printf(" ->->-> cmdNum = %d, cmds = %s \n",cmdNum,*cmds[cmdNum-1]);

        if (cmdNum >= 1) {
            newchild(&cmds[cmdNum-1],cmdNum);
        }
        if(cmdNum <1)
            pIdx = pNum -1;
        else
            pIdx = pNum - (cmdNum*2);
        //printf(" ***[%d] pNum = %d, cmdNum = %d, pIdx = %d *** \n",(int) getpid(),pNum,cmdNum,pIdx);


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
//        dup2(pipes[1], 1);
//        dup2(pipes[0], 0);

        for (int i = 0; i < pNum; ++i)
            close(pipes[i]);


        //printf(" >>[%d] cmd: %s\n",(int) getpid(),childArr[0]);
        execvp(childArr[0], childArr);
    }
    else{
        //printf("I am the parent with PID = %d, cmdNum =%d\n",(int)getpid(),cmdNum);
        //printf(" >> cmd: %s\n",*cmds[cmdNum-1]);

        i=0;
        while(*(*(cmds+(cmdNum-1))+i)){
            //printf(" >->-> parentArr: %s\n",*(*(cmds+(cmdNum-1))+i));
            parentArr[i] = *(*(cmds+(cmdNum-1))+i);
            parentArr[++i]=NULL;

        }

        //printf(" >>[%d]: parentArr: %s, %s\n",(int)getpid(),*parentArr,*(parentArr+1));

        dup2(pipes[0],0);

        // only the parent gets here and waits for 3 children to finish
        for (int i = 0; i < pNum ; ++i) {
            close(pipes[i]);
        }

        //execvp(*cmds[cmdNum-1], **(cmds+(cmdNum-1)));
        execvp(*parentArr, parentArr);

        for (i = 0; i < 3; i++)
            wait(&status);
    }


}

