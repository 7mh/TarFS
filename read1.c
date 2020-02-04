#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fuse.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define handle_error(msg)\
    do {perror(msg); exit(EXIT_FAILURE);} while(0)

/* tar Header Block, from POSIX 1003.1-1990.  */

/* POSIX header.  */

typedef struct __attribute__((__packed__)) posix_header
{                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155+12];           //to  make total of 512
  //char prefix[155];              345 
  //                               500 
  } * posix_header ;

char buff[512];
    
int main(int argc, char * argv[]){
    int fd;
    int t = 0;
    long int size ;
    
    posix_header  hdr; 
    hdr = (struct posix_header *)malloc(sizeof(*hdr));
    
    char * dflt_file = "x.tar";
    printf("size of struct= %lu\n", sizeof(*hdr));   
    
    //open tar file
    if (argc < 2){
        fd = open(dflt_file, O_RDWR );
    }
    else{
        fd = open(argv[1], O_RDWR);
    }
    if (fd == -1){
        handle_error("Open Tar file!");
    }
    
    //reading tar
    //t = read(fd, hdr, sizeof(*hdr) );
    t = read(fd, hdr, sizeof(*hdr));
    
    printf("Name = %s\n", hdr-> name);
    printf("uid, gid = %ld, %ld\n", strtol(hdr -> uid, NULL, 8 ), strtol(hdr -> gid, NULL, 8));
    
    t = read(fd, hdr, sizeof(*hdr));
    printf("Name = %s\n", hdr-> name);
    printf("uid, gid = %ld, %ld\n", strtol(hdr -> uid, NULL, 8 ), strtol(hdr -> gid, NULL, 8));

    while (1){
           
        t = read(fd, hdr, sizeof(*hdr));
        size = strtol(hdr -> size, NULL, 8);
        printf("Name = %s\n", hdr-> name);
        printf("uid, gid = %ld, %ld\n", strtol(hdr -> uid, NULL, 8 ), strtol(hdr -> gid, NULL, 8));
        if (size == 0)
            break;
        printf("<---- %ld bytes file start here\n", size);
        t = read(fd, buff, 512);
        printf("%s", buff);
        printf("<------------ EOF !!!!!!!!\n");
    }

    return 0;
}
