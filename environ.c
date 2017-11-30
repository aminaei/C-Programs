/* illustration of how to access the various arguments to main() */
#include   <stdio.h>
#include   <stdlib.h>
int main(int argc, char *argv[], char *env[]) {
/* The following are all valid main() specifiers, depending on how many
 * parameters you need to access.
 * main()
 * main(int argc)
 * main(int argc, char *argv[])
 * main(int argc, char *argv[], char *env[])
 */
int i;
char *ptr;

/* print the number of command line arguments */
printf("argc = %d\n", argc);

/* print the command line arguments */
for (i = 0; i < argc; ++i)
      printf("argv[%d] = %s\n", i, argv[i]);

/* print the entire environment array */
for (i = 0; NULL != env[i]; ++i)
        printf("[%2d] %s\n", i, env[i]);

/* access a single environment variable */
printf("\n%s[%s]\n", "Pager is defined to be: ", ptr = getenv("PAGER"));
/* look at where environment variables live */
   printf("Address of PAGER is %u\n", ptr);
/* note that environment addresses are above the stack frame */
   printf("Address of  argc is %u\n", &argc);

/* Change the value of an environment variable */
   putenv("PAGER=/bin/less");

   if ( NULL == (ptr = getenv("PAGER")) )
      printf("PAGER is not a defined environment variable!\n");
   else
      printf("PAGER is defined to be: [%s]\n", ptr);

/* new definitions don't remain in the u-struct area any more, since they might
 * not 'fit'; consequently, more space has to be malloc()ed, and this string now
 * lives at a much lower address, on the heap.  */
   printf("Address of PAGER is %u\n", ptr);
}
/* [truncated] output:
rohan[45]% a.out     aardvark
argc = 2
argv[0] = a.out
argv[1] = aardvark
[ 0] LANG=C
[ 1] USER=cs570
[ 2] LOGNAME=cs570
[ 3] HOME=/home/cs/faculty/cs570
...
[45] PAGER=/usr/bin/more
[46] EDITOR=/usr/local/bin/vim

Pager is defined to be: [/usr/bin/more]
Address of PAGER is 366632906
Address of  argc is 366626684
PAGER is defined to be: [/bin/less]
Address of PAGER is 4196498
rohan[46]%   */
