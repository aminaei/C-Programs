

#include <stdio.h>
#include <stdlib.h>
#include "p2.h"
#include "CHK.h"


// #include statements
// #define statements
// declarations;

#define MAXFILENAME 50

static char string[MAXITEM][STORAGE];
char *argvec[MAXITEM];
static char **argvArr[MAXITEM];
static int argvCnt[]={0};
int pipeNum=0;
int pNum;
int IO_flag = 0;
int pipe_flag = 0;
int infile_flag = 0;
int outfile_flag = 0;
int EOF_flag = 0;
int bg_flag=0;
int parse(void);
int pipIdx=0;
static int pipes[20];

static char infile[STORAGE];
static char outfile[STORAGE];
int input_fd,output_fd;

pid_t gpid = -1;

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




void killhandler(int sig){
    // printf("--> trying to kill pid <--- \n" );
    killpg(getpgid(gpid),SIGTERM);
}

void main(void)
{
    //    declarations of locals;
    //struct getline getline;
    //struct getargcs *ptr;

    int ioPtr = 0;

//    char input_filename[MAXFILENAME];
//    char output_filename[MAXFILENAME];

    int flags;
    flags = O_APPEND | O_WRONLY;
    int status;

    int line;
    // char *argv[MAXITEM]={NULL};
    // char *pipe_argv[MAXITEM]={NULL};

    char *homedir = getenv("HOME");


    signal(SIGTERM,killhandler);

    //    any necessary set-up, including signal catcher and setpgid();
    for (;;) {
        fflush(stdout);
        fflush(stderr);
        fflush(stdin);

        struct stat sb;
        sb.st_mode=0;

        struct stat statbuf;
        statbuf.st_mode = 0;

        status = 0;
        ioPtr = 0;
        ///////////////////////////////////////////

        //*** issue PROMPT;
        printf("p2: ");


        //*** call your parse function;
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
            if ((input_fd=open(infile, O_RDONLY)) < 0) {
                printf("Error: No such file or directory\n");
                break;

            }
            //dup2(input_fd,STDIN_FILENO);
            //close(input_fd);


        }


        if((outfile_flag == 1) && (outfile[0] != '\0')){
            // printf("--> output_filename = %s\n",outfile);
            fflush(stdout);
            fflush(stderr);
            fflush(stdin);
            // stat(output_filename, &statbuf);
            // if(S_ISREG(statbuf.st_mode) != 0){
            //     printf("Error: %s Already Exist. \n",output_filename);
            // }

            if((output_fd = open(outfile, O_WRONLY  | O_CREAT,\
                            S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR))<0){
                printf("Error: Can Not override such file or directory\n");
            }



        }
        //////////////// cd ///////////////////////////

        if(argvec[0] != NULL && strcmp(argvec[0], "cd")==0){
            int argc=0;
            while(argvec[argc] != NULL)
                ++argc;

//             printf("--- > argc = %d\n", argc);
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


        if(argvec[0] != NULL && strcmp(argvec[0], "ls-F")==0){

            if(argvec[1] == NULL){

                if(CHKstat(".") == -1)
                    printf(" ** ls-F Error: Path %s\n ",".");
            }
            else{
                int i = 1;
                while(argvec[i] != NULL) {
                    if (argvec[i] != NULL && CHKstat(argvec[i]) == -1)
                        printf(" ** ls-F Error: Path %s\n ", ".");

                    i++;
                }

            }

            continue;
        }

        ///////////////////////////////////////////////////

    }// EOF For Loop

    killpg(getpgrp(), SIGTERM); // Terminate any children that are
    // still running. WARNING: giving bad args
    // to killpg() can kill the autograder!
    printf("p2 terminated.\n");// MAKE SURE this printf comes AFTER killpg

    //exit(0);
} // EOF main



int parse(void){

    int c;
    char s[STORAGE];
    int vIdx=0;
    int arrIdx=0;
    int pIdx=0;
    int cnt=0;
    argvCnt[cnt]=0;
    pipeNum=0;

    EOF_flag = 0;
    IO_flag = 0;
    pipe_flag = 0;
    bg_flag = 0;
    argvArr[arrIdx] = NULL;
    argvec[vIdx]=NULL;
    infile[0] ='\0';
    outfile[0]= '\0';

    for(;;) {
        c = getword(s);
         //printf("--> c = %d, s = %s\n",c, s);


        if (c == -1){
            EOF_flag = -1;
            return -1;

        }
        else{
            if(c == 0){

                //printf(" ++++ arrIdx =%d, vIdx = %d, pIdx = %d, argvec[%d] = %s +++ \n",arrIdx,vIdx,pIdx,pIdx,argvec[pIdx]);
                argvArr[arrIdx] = &argvec[pIdx];
                argvArr[++arrIdx] = NULL;

                return 1;
            }


            if(strcmp(s,"<") == 0 || strcmp(s,">") == 0 || strcmp(s,"|") == 0 || strcmp(s,"&") == 0){

                if(strcmp(s,"|") == 0) {
                    pipe_flag = 1;
                    pipeNum++;
                    // printf(" ++++ vIdx = %d, pIdx = %d, argvec[%d] = %s +++ \n",vIdx,pIdx,pIdx,argvec[pIdx]);
                    argvArr[arrIdx] = &argvec[pIdx];
                    argvArr[++arrIdx] = NULL;
                    pIdx=vIdx+1;
                    argvCnt[++cnt]=0;
                    vIdx++;
                }
                else {
                    IO_flag = 1;
                    if(strcmp(s,"<") == 0) {
                        infile_flag = 1;
                        c = getword(s);
                        if(strcmp(s,"<") == 0 || strcmp(s,">") == 0 || strcmp(s,"|") == 0 || strcmp(s,"&") == 0) {
                            printf("syntax error near unexpected token %s\n", s);
                            return 0;
                        }
                        else
                            strcpy(infile,s);
                    }
                    else if(strcmp(s,">") == 0) {
                        outfile_flag = 1;
                        c = getword(s);
                        if(strcmp(s,"<") == 0 || strcmp(s,">") == 0 || strcmp(s,"|") == 0 || strcmp(s,"&") == 0) {
                            printf("syntax error near unexpected token %s\n", s);
                            return 0;
                        }
                        else
                            strcpy(outfile,s);
                    }
                    else{
                        infile_flag = 0;
                        outfile_flag = 0;
                    }
                }
                if(strcmp(s,"&") == 0)
                    bg_flag = 1;
                else
                    bg_flag = 0;

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
