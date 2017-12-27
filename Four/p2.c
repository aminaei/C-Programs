/*
 ============================================================================
 * CS-570 Program 4 -- p2.c --
 * Instrctor: John Carroll
 * Due Date: November 28, 2017
 *
 *
 * Author Name: Ali Minaei
 * Created on: November 11, 2017
 *
 * Files Need:
 *     p2.c
 *     p2.h
 *     getword.c
 *     getword.h
 *     makefile
 *     
 * Functions:
 * parse(void): 
 * int CHKfileExist(char *filename, char* mode)
 * int CHKfilePremission(char *filename, char* mode)
 * int CHKstat(char *path)
 *
 ============================================================================
 */


#include "p2.h"
#include "CHK.h"


// #include statements
// #define statements
// declarations;

#define MAXFILENAME 50

static char string[MAXITEM][STORAGE];
static char *argvec[MAXITEM];
static char **argvArr[MAXITEM];
static int argvCnt[]={0};

int pipeNum=0;
int pNum;
int IO_flag = 0;
int pipe_flag = 0;
int infile_flag = 0;
int outfile_flag = 0;
int outfileAppend_flag = 0;
int EOF_flag = 0;
int bg_flag=0;


int fd;
int arrIdx=0;
int execflag = 0;

int pipIdx=0;
static int pipes[20];

static char infile[STORAGE];
static char outfile[STORAGE];
int input_fd,output_fd;

typedef enum { false, true } bool;

pid_t gpid = -1;

int parse(void);
int CHKfileExist(char *filename, char* mode){
    struct stat sb;
    sb.st_mode=0;

    struct stat statbuf;
    statbuf.st_mode = 0;


    if(stat(filename, &statbuf) ==0){
        if(strcmp(mode, "WRITE") == 0) {
            fprintf(stderr,"Error: %s Already Exist.\n", filename);
            return 1;
        }
        else
            return 1;
    }
    else
        return 0;
}
int CHKfilePremission(char *filename, char* mode){

    struct stat statbuf;
    statbuf.st_mode = 0;

    stat(filename, &statbuf);


    // printf(" ** CHKfilePremission: file = %s **\n",filename);
    if (strcmp(mode, "READ") == 0) {
        if ((statbuf.st_mode & S_IFMT)==S_IRUSR || (statbuf.st_mode & S_IFMT)==S_IRGRP ) {
            fprintf(stderr,"Error: cannot open %s: Permission denied.\n", filename);
            return -1;
        } else
            return 1;
    }
    if (strcmp(mode, "WRITE") == 0) {
        if ((statbuf.st_mode & S_IFMT) == S_IWGRP || (statbuf.st_mode & S_IFMT)== S_IWUSR) {
            fprintf(stderr,"Error: cannot open %s: Permission denied.\n", filename);
            return -1;
        } else
            return 1;
    }
    if (strcmp(mode, "OVER_WRITE") == 0) {
        if((statbuf.st_mode & S_IFMT) == S_IFDIR){
            fprintf(stderr,"Error: Can Not over write %s directory.\n", filename);
            return -1;

        }
        if ((statbuf.st_mode & S_IWGRP) || (statbuf.st_mode & S_IWUSR)) {
            return 1;
        }
        else
            return -1;
    }

}
int CHKstat(char *path);
void newchild(char **cmds[],int cmdNum) {

    int i =0;
    //printf(" --- Inside of newchild ---\n");
    if (fork() == 0) {

        pid_t pid;
        pid = getpid();

        cmdNum--;
        if (cmdNum > 0) {
            newchild(&(*(cmds-1)), cmdNum);
        }
        // printf("--> Inside of a chile process with PID = %d, cmdNum = %d, cmd = %s\n", pid,cmdNum,*cmds[0]);

        if(cmdNum <1)
            pipIdx = pNum -1;
        else
            pipIdx = pNum - (cmdNum*2);
        // printf(" *** pNum = %d, cmdNum = %d, pIdx = %d ***\n",pNum,cmdNum,pipIdx);
        // printf(" ** cmd[0] = %s cmd[1] = %s \n",*cmds[0],*(*(cmds)+1));
        if(cmdNum == 0){
            if(infile_flag==1){
                dup2(input_fd,0);
                close(input_fd);
            }
            dup2(pipes[pipIdx],1);
        }
        else if(cmdNum % 2 !=0){
            dup2(pipes[pipIdx], 0);
            if(pipIdx > 1)
                dup2(pipes[pipIdx-1], 1);
        }
        else {
            //dup2(pipes[2], 0);
            dup2(pipes[pipIdx], 0);
            if(pipIdx >0)
                dup2(pipes[pipIdx-1], 1);

        }

        if((IO_flag==1)&&(pNum == 2*cmdNum)){
            if(outfile_flag == 1){
                dup2(output_fd, 1);
                close(output_fd);
            }
        }

        i = 0;
        for (i = 0; i < pNum; ++i)
            close(pipes[i]);


        //printf(" -- *cmds[0] = %s \n",*cmds[0]);
        if(strcmp(*cmds[0],"ls-F")==0) { // EXTRA CREDIT
            if (CHKstat(*(*(cmds) + 1)) == 1)
                exit(0);
        }
        else if(strcmp(*cmds[0],"exec")==0) { // EXTRA CREDIT
            char *TempArg[MAXITEM];
            i =1;
            for ( i = 1; *(*(cmds)+i) != NULL ; ++i) {
                TempArg[i-1]=*(*(cmds)+i);
                TempArg[i]=NULL;
                // printf("--> TempArg[%d] = %s\n",i-1,TempArg[i-1]);
            }
            if(execvp(*TempArg, TempArg) < 0){    //execute the command
                fprintf(stderr,"ERROR: %s: Command not found.\n",*TempArg);
                exit(1);
            }
            //exit(0);
        }
        else {
            if(execvp(*cmds[0], *cmds) < 0){    //execute the command
                fprintf(stderr,"ERROR: %s: Command not found.\n",*cmds[0]);
                exit(1);
            }
        }


    }


}

void killhandler(){}

void main(void)
//void main(int argc, char *argv[])
{
    // declarations of locals;
    pid_t pid, cpid, pgid;

    int ioPtr = 0;
    int i = 0;

    char *childArr[MAXITEM];
    char *parentArr[MAXITEM];

    int flags;
    flags = O_APPEND | O_WRONLY;
    int status;

    int line;

    char *homedir = getenv("HOME");


    signal(SIGTERM,killhandler);
    setpgid(0,0);

    //    any necessary set-up, including signal catcher and setpgid();
    for (;;) {
        fflush(stdout);
        fflush(stderr);
        fflush(stdin);

        struct stat sb;
        struct stat statbuf;
        statbuf.st_mode = 0;

        execflag = 0;
        status = 0;
        ioPtr = 0;
        ///////////////////////////////////////////

        // issue PROMPT;
        printf("p2: ");


        // call your parse function;
        line=parse();

        if(line==-1)
            break;
        else if(line==0)
            continue;

        if((infile_flag == 1) && (infile[0] != '\0')){

            // printf("--> input_filename = %s\n",infile);
            fflush(stdout);
            fflush(stderr);
            fflush(stdin);
            if(CHKfileExist(infile,"READ")==0){
                fprintf(stderr,"Error: %s No such file or directory\n",infile);
                continue;
            }
            else {
                if (CHKfilePremission(infile, "READ") == 1) {
                    if ((input_fd = open(infile, O_RDONLY)) < 0) {
                        fprintf(stderr,"Error: No such file or directory\n");
                        continue;

                    }
                }
            }

        }


        if((outfile_flag == 1) && (outfile[0] != '\0')) {
            //fflush(stdout);
            //fflush(stderr);
            //fflush(stdin);

            if(CHKfileExist(outfile,"WRITE")==0){
                if ((output_fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, \
                        S_IRUSR | S_IRGRP| S_IWGRP | S_IWUSR |S_IROTH | S_IWOTH)) < 0) {
                    fprintf(stderr,"Error: Can Not over write such file or directory\n");
                    continue;
                }
            }
            else {
                outfile_flag = 0;
                continue;
            }
        }

        // printf(" ** >! : outfileAppend_flag = %d, outfile = %s\n",outfileAppend_flag,outfile);
        if((outfileAppend_flag == 1) && (outfile[0] != '\0')){
            //fflush(stdout);
            fflush(stderr);
            //fflush(stdin);


            /* create and initialize the count file: */
            // CHK(fd = open(outfile,O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
            if (CHKfilePremission(outfile, "OVER_WRITE") == 1) {

                if ((output_fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, \
                         S_IWGRP | S_IWUSR | S_IRWXU)) < 0) {
                    fprintf(stderr,"Error: Can Not override such file or directory\n");
                    continue;
                }
            }

            else {
                outfileAppend_flag =0;
                continue;
            }


        }

        //////////////// cd ///////////////////////////

        if(argvec[0] != NULL &&  strcmp(argvec[0], "cd")==0){
            int argc=0;
            while(argvec[argc] !=NULL)
                ++argc;
            // printf("--- > argc = %d\n", argc);
            if(argc > 2){
                printf("cd: Too many arguments.\n");
                continue;
            }

            else if(argvec[1] == NULL){
                argvec[1] = homedir;
                // printf(" --> argv[1] = %s\n",argv[1]);
                argvec[2] = NULL;
                chdir(homedir);

            }
            else{
                stat(argvec[1], &statbuf);
                if( S_ISREG( statbuf.st_mode ) != 0 )
                    printf( "%s : is a file\n", argvec[1] ) ;
                else if( S_ISDIR(statbuf.st_mode ) != 0)
                    chdir(argvec[1]);
                else
                    printf( "%s : No such file or directory.\n", argvec[1] ) ;


            }
            continue;
        }

        //////////////// ls-F ///////////////////////////
        ///
        // printf(" -- pipe_flag = %d \n",pipe_flag);
        if(argvec[0] != NULL && strcmp(argvec[0], "ls-F")==0 && pipe_flag==0){

            if(argvec[1] == NULL){

                if(CHKstat(".") == -1)
                    fprintf(stderr," ** Error: Path %s\n ",".");
            }
            else{
                int i = 1;
                while(argvec[i] != NULL) {
                    if (argvec[i] != NULL && CHKstat(argvec[i]) == -1)
                        // printf(" ** Error: Path %s\n ", argvec[i]);
                        break;

                    i++;
                }

            }

            continue;
        }

        //////////////// exec ///////////////////////////
        char *TempArg[MAXITEM];
        if(argvec[0] != NULL &&  strcmp(argvec[0], "exec")==0 && pipe_flag==0){
            int argc=0;
            while(argvec[argc] !=NULL)
                ++argc;
            // printf("--- > argc = %d\n", argc);
            if(argc < 2){
                printf("exec: Too few arguments.\n");
                continue;
            }
            else{
                int i =1;
                for ( i = 1; argvec[i] != NULL ; ++i) {
                    TempArg[i-1]=argvec[i];
                    TempArg[i]=NULL;
                    // printf("--> TempArg[%d] = %s\n",i-1,TempArg[i-1]);
                }
                //char *temp[]={"tcsh",NULL};
                // execvp(*TempArg, TempArg);
                // execvp(temp[0],temp);
                if (execvp(*TempArg, TempArg) < 0) {     /* execute the command  */
                    fprintf(stderr,"ERROR: %s: Command not found.\n",*TempArg);
                    exit(0);
                }
            }

        }
        ///////////////////////////////////////////////////


        /////////////////////////
        // if pipe "|" is detected
        // ///////////////////////
        int pIdx=0;
        int cmdNum = arrIdx;
        if(pipe_flag == 1){
            //printf(" *** Inside of Pipe ***\n");
            fflush(stdin);
            fflush(stderr);
            fflush(stdout);


            int arrCnt=0;
            i = 0;

            pNum = 2*pipeNum;
            while(argvCnt[i] != 0){
                //printf(" ** argvCnt[%d] = %d \n",i,argvCnt[i]);
                arrCnt++;
                ++i;
            }

            //printf(" *** pipeNum = %d, pNum = %d, arrCnt =%d arrIdx = %d***\n",pipeNum,pNum,arrCnt,arrIdx);

            if(arrCnt != pipeNum+1){
                fprintf(stderr,"Error p2: Invalid null command.\n");
                continue;
            }

            for (i = 0; i < pipeNum; ++i) {
                pipe(pipes + i*2); // sets up 1st pipe
            }

            ///////////////////////////////////////////////////////
            if (fork() == 0) {

                //printf("I am the child with PID = %d\n", (int) getpid());

                int j=0;
                //printf(" >->-> childArr: %s\n",*(*(argvArr+(cmdNum-2))));
                while(*(*(argvArr+(cmdNum-1))+j)!= NULL){
                    //for (j = 0; j < 3; ++j) {
                    //printf(" >->-> childArr: %s\n",*(*(argvArr+(cmdNum-1))+j));
                    childArr[j] = *(*(argvArr+(cmdNum-1))+j);
                    childArr[++j]=NULL;
                    //childArr[j+1]=NULL;

                }
                cmdNum =cmdNum-1;
                //printf(" ->->->[%d] cmdNum = %d \n", (int) getpid(),cmdNum);

                if (cmdNum >= 1) {
                    //printf(" *** Time to create children, pipeNum = %d, pNum = %d ***\n",pipeNum,pNum);
                    //printf("** argvArr[0] = %s **  \n",*(*(argvArr)));
                    //printf("** argvArr[1] = %s **  \n",*(argvArr[pipeNum]));
                    newchild(&(argvArr[cmdNum-1]), cmdNum);
                }



                if(cmdNum <1)
                    pIdx = pNum -1;
                else
                    pIdx = pNum - (cmdNum*2);

                if (strcmp(*(*(argvArr)), "exec") == 0) { // EXTRA CREDIT

                    exit(0);
                }
                else{
                    if(cmdNum == 0){
                        if(infile_flag==1){
                            dup2(input_fd,0);
                            close(input_fd);
                        }
                        dup2(pipes[pipIdx],1);
                    }
                    else if(cmdNum % 2 !=0){
                        dup2(pipes[pipIdx], 0);
                        if(pipIdx > 1)
                            dup2(pipes[pipIdx-1], 1);
                    }
                    else {
                        //dup2(pipes[2], 0);
                        dup2(pipes[pipIdx], 0);
                        if(pipIdx >0)
                            dup2(pipes[pipIdx-1], 1);

                    }

                    if((IO_flag==1)&&(pNum == 2*cmdNum)){
                        if(outfile_flag == 1){
                            dup2(output_fd, 1);
                            close(output_fd);
                        }
                    }
                    i=0;
                    for (i = 0; i < pNum; ++i)
                        close(pipes[i]);

                    if(execvp(*childArr, childArr) < 0){
                        fprintf(stderr,"ERROR: %s: Command not found.\n",*childArr);
                        exit(1);
                    }
                }


            }
            else { // I'm the parent

                if (strcmp(*(*(argvArr)), "exec") == 0) { // EXTRA CREDIT
                    i=0;
                    while(*(*(argvArr+(cmdNum-1))+i)!= NULL){
                        //for (j = 0; j < 3; ++j) {
                        //printf(" >->-> childArr: %s\n",*(*(argvArr+(cmdNum-1))+j));
                        parentArr[i] = *(*(argvArr+(cmdNum-1))+i);
                        parentArr[++i]=NULL;

                    }
                    dup2(pipes[0],0);
                    for (i = 0; i < pNum; ++i)
                        close(pipes[i]);

                    //printf(" *** pipe:exec: %s\n",*(*(argvArr)));
                    if(execvp(*parentArr, parentArr) < 0){
                        fprintf(stderr,"ERROR: %s: Command not found.\n",*childArr);
                        exit(1);
                    }
                    exit(1);
                }


                for (i = 0; i < pNum; ++i) {
                    close(pipes[i]);
                }
                if ((infile_flag == 1))
                    close(input_fd);

                if ((outfileAppend_flag == 1) || (outfile_flag == 1))
                    close(output_fd);


                for (i = 0; i < pipeNum; i++) {
                    wait(&status);
                }

            }
        }

        /////////////////////////////////////////////
        ///  BG code goes here
        ///  Normal Process
        ////////////////////////////////////////////////

        if(pipe_flag == 0 ) {
            // pid_t cpid, pgid;


            fflush(stdin);
            fflush(stderr);
            fflush(stdout);
            //if (argvec[0] != NULL) {
            cpid = fork();
            if (cpid < 0) {                 // fork a child process
                fprintf(stderr,"*** ERROR: forking child process failed\n");
                exit(1);
            } else if (cpid == 0) {         // for the child process:
                // printf("I am the child with pid = %d\n",(int)getpid());

                //printf("Child background process...");
                setpgid(cpid, 0);

                gpid = getpid();

                if (bg_flag) {
                    // printf("*** Child process is going to background ...\n");

                    if(infile_flag==1){
                        dup2(input_fd,STDIN_FILENO);
                        close(input_fd);
                    }
                    else {
                        int devnull;
                        if((devnull = open("/dev/null",O_RDONLY)) < 0){
                            fprintf(stderr,"Failed to open /dev/null.\n");
                            exit(9);
                        }
                        dup2(devnull,STDIN_FILENO);
                        close(devnull);

                    }
                    if(outfile_flag == 1 || outfileAppend_flag == 1){

                        dup2(output_fd, STDOUT_FILENO);
                        close(output_fd);
                    }
                    if (execvp(argvec[0], argvec) < 0) {    //execute the command
                        fprintf(stderr,"ERROR: %s: Command not found.\n",argvec[0]);
                        exit(1);
                    }

                }
                else {
                    // printf("*** Normal process ...\n");

                    if(infile_flag==1){

                        dup2(input_fd,0);
                        close(input_fd);
                    }
                    else {
                        int devnull;
                        if((devnull = open("/dev/null",O_RDONLY)) < 0){
                            fprintf(stderr,"Failed to open /dev/null.\n");
                            exit(9);
                        }
                        dup2(devnull,STDIN_FILENO);
                        close(devnull);

                    }
                    if(outfile_flag == 1 || outfileAppend_flag == 1){
                        fflush(stdin);
                        fflush(stderr);
                        fflush(stdout);

                        dup2(output_fd, 1);
                        close(output_fd);
                    }
                    if (execvp(argvec[0], argvec) < 0) {    //execute the command
                        fprintf(stderr,"ERROR: %s: Command not found.\n",argvec[0]);
                        exit(1);
                    }
                }


            } else {

                if (bg_flag) {
                    printf("%s [%d]\n", argvec[0],cpid);
                    bg_flag = 0;
                    continue;
                } else {
                    for (;;) {
                        CHK(pid = wait(NULL));
                        if (pid == cpid) {
                            break;
                        }
                    }
                }
            }
            //}
        }



    }// EOF For Loop

    killpg(getpgrp(), SIGTERM); // Terminate any children that are
    // still running. WARNING: giving bad args
    // to killpg() can kill the autograder!
    printf("p2 terminated.\n");// MAKE SURE this printf comes AFTER killpg

    exit(0);
} // EOF main

int parse(void){

    int c;
    char s[STORAGE];
    int vIdx=0;
    arrIdx=0;
    int pIdx=0;
    int cnt=0;

    pipeNum=0;
    infile_flag = 0;
    outfile_flag = 0;
    outfileAppend_flag = 0;
    IO_flag = 0;

    EOF_flag = 0;
    pipe_flag = 0;
    bg_flag = 0;
    argvArr[arrIdx] = NULL;
    argvec[vIdx]=NULL;
    infile[0] ='\0';
    outfile[0]= '\0';

    int i =0;
    //argvCnt[cnt]=0;
    while(argvCnt[i] != 0){
        argvCnt[i]=0;
        ++i;
    }
    for(;;) {
        c = getword(s);
        // printf("--> c = %d, s = %s\n",c, s);


        if (c == -1){
            EOF_flag = -1;
            return -1;

        }
        else if(c == -30){
            //perror("Unmatched '.\n");
            fprintf(stderr,"p2 Error: Unmatched '\n");
            return 0;
        }
        else{
            if(c == 0){

                //printf(" ++++ arrIdx =%d, vIdx = %d, pIdx = %d, argvec[%d] = %s, cnt= %d +++ \n",arrIdx,vIdx,pIdx,pIdx,argvec[pIdx],cnt);
                argvArr[arrIdx] = &argvec[pIdx];
                argvArr[++arrIdx] = NULL;

                return 1;
            }

            if(strcmp(s,"|") == 0) {
                pipe_flag = 1;
                pipeNum++;
                // printf(" ++++ vIdx = %d, pIdx = %d, argvec[%d] = %s +++ \n",vIdx,pIdx,pIdx,argvec[pIdx]);
                argvArr[arrIdx] = &argvec[pIdx];
                argvArr[++arrIdx] = NULL;
                pIdx=vIdx+1;
                argvCnt[++cnt]=0;
                vIdx++;
                continue;
            }
            else if(strcmp(s,"&") == 0) {
                bg_flag = 1;

                argvArr[arrIdx] = &argvec[pIdx];
                argvArr[++arrIdx] = NULL;

                return 1;
            }
            else if(strcmp(s,"<") == 0 || strcmp(s,">") == 0 || strcmp(s,">!") == 0){
                IO_flag = 1;
                if(strcmp(s,">!") == 0){
                    outfileAppend_flag = 1;
                    c = getword(s);
                    if(strcmp(s,"<") == 0 || strcmp(s,">") == 0 || strcmp(s,"|") == 0 || strcmp(s,"&") == 0) {
                        fprintf(stderr,"syntax error near unexpected token %s\n", s);
                        return 0;
                    }
                    else {
                        if(s[0] != '\0') {
                            strcpy(outfile, s);
                        }
                        else{
                            fprintf(stderr,"p2: syntax error Missing name for redirect.\n");
                            return 0;
                        }
                    }
                }
                else if(strcmp(s,"<") == 0) {
                    infile_flag = 1;
                    c = getword(s);

                    if(strcmp(s,"<") == 0 || strcmp(s,">") == 0 || strcmp(s,"|") == 0 || strcmp(s,"&") == 0) {
                        fprintf(stderr,"syntax error near unexpected token %s\n", s);
                        return 0;
                    }
                    else {
                        if(s[0] != '\0') {
                            strcpy(infile, s);
                        }
                        else{
                            fprintf(stderr,"p2: syntax error Missing name for redirect.\n");
                            return 0;
                        }
                    }
                }
                else if(strcmp(s,">") == 0) {
                    outfile_flag = 1;
                    c = getword(s);
                    if(strcmp(s,"<") == 0 || strcmp(s,">") == 0 || strcmp(s,"|") == 0 || strcmp(s,"&") == 0) {
                        fprintf(stderr,"syntax error near unexpected token %s\n", s);
                        return 0;
                    }
                    else {
                        if(s[0] != '\0') {
                            strcpy(outfile, s);
                        }
                        else{
                            fprintf(stderr,"p2: syntax error Missing name for redirect.\n");
                            return 0;
                        }
                    }
                }
                else{
                    infile_flag = 0;
                    outfile_flag = 0;
                    outfileAppend_flag = 0;
                }

                continue;

            }

            strcpy(string[vIdx],s);
            // printf(" *** argvec[%d] = %s *** \n",vIdx,string[vIdx]);
            argvec[vIdx]=string[vIdx];

            argvec[++vIdx]=NULL;
            argvCnt[cnt]++;



        }

    }


}

int CHKstat(char *path)
{
    DIR *dirp;
    struct dirent *dp;
    char *target_path;
    char *fullpath;	/* contains full pathname for every file */
    int len;
    int plen=0;

    struct stat sb;
    sb.st_mode=0;
    int result;

    char * temp;

    struct stat statbuf;
    statbuf.st_mode = 0;


    if ((dirp = opendir (path)) == NULL) {
        fprintf(stderr,"Error: Cannot open %s\n",path);
        return -1;
    }


    while((dp = readdir(dirp)) != NULL){
        if ( (fullpath = malloc(strlen(path) + 1024)) == NULL) {
            fprintf(stderr, "malloc error for pathname\n");
            exit(3);
        }
        target_path = malloc(strlen(path) + 1024);

        temp = malloc(strlen(path) + 1024);

        strcpy(fullpath, path);	/* initialize fullpath */
        strcat(fullpath,"/");

        //printf("** CHKstat: Fullpath = %s ** \n",fullpath);
        //printf("** CHKstat: dp->d_name = %s ** \n",dp->d_name);
        strcat(fullpath,dp->d_name);
        // printf("** CHKstat: Fullpath = %s ** \n",fullpath);

        if (strcmp(dp->d_name, ".") == 0  ||
            strcmp(dp->d_name, "..") == 0) {
            printf("%s/"
                           "\n",dp->d_name);
            continue;
        }

        result = lstat(fullpath, &statbuf);
        // printf(" ** result = %d\n",result);
        if ( result == -1) {
            perror("stat");
            //exit(EXIT_FAILURE);
            //return -1;
        }

        // printf("--> st_mode =%04x,  S_IFMT = %04x\n",statbuf.st_mode,S_IFMT);
        switch (statbuf.st_mode & S_IFMT) {
            case S_IFREG: // File
                if((statbuf.st_mode & S_IEXEC) || (statbuf.st_mode & S_IXGRP) || (statbuf.st_mode & S_IXOTH))
                    printf("%s*\n",dp->d_name);
                else
                    printf("%s\n",dp->d_name);
                break;
            case S_IFDIR: // directory
                printf("%s/\n",dp->d_name);
                break;
            case S_IFLNK: // symbolic link

                // printf(" -- Fullpath = %s\n",fullpath);
                /* Attempt to read the target of the symbolic link. */
                len = readlink (fullpath,target_path, sizeof (fullpath));
                target_path[len] = '\0';
                // printf(" -- S_IFLNK: len = %d target_path = %s \n",len,target_path);

                if (len == -1) {
                    /* The call failed. */
                    if (errno == EINVAL)
                        /* It's not a symbolic link; report that. */
                        fprintf (stderr, "%s is not a symbolic link\n", dp->d_name);
                    else
                        /* Some other problem occurred; print the generic message. */
                        perror ("readlink");
                }
                else {
                    /* NUL-terminate the target path. */


                    strcpy(temp,path);
                    strcat(temp,"/");
                    strcat(temp,target_path);
                    /* Print it. */
                    // printf (">>>> target_path = %s\n", target_path);
                    // printf (">>>> temp = %s\n", temp);

                    if ( lstat(temp, &sb) == -1) {
                        // perror("stat");
                        //exit(EXIT_FAILURE);
                        printf("%s&\n",dp->d_name);
                    }
                    else
                        printf("%s@\n",dp->d_name);

                }

                //target_path = '\0';
                //printf("%s@\n",dp->d_name);
                break;
                // case S_IFBLK:  printf("block device\n");            break;
                // case S_IFCHR:  printf("character device\n");        break;
                // case S_IFIFO:  printf("FIFO/pipe\n");               break;
                // case S_IFSOCK: printf("socket\n");                  break;
            default:       printf("unknown?\n");                break;
        }

        free(fullpath);
        free(target_path);


    }

    closedir(dirp);

    return 1;

}
