#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

/*Struct Area !!! */
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////


int main(int argc, char * argv[], char * envp[] ){
    long int t;

    struct stat sb;
    stat("x.tar", &sb);
    printf(" time: %d\n", sb.st_mtim.tv_sec);

    return 0;
}
