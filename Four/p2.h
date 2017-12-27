#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>


#include "getword.h"
#define MAXITEM 100 /* max number of words per line */

#define STAT_CHK
#define MAXFILENAME 50






