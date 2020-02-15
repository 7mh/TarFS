#ifndef UTILIT_H
#define UTILIT_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>      
#include <time.h>

//////////////////////////////////////////
/* Values used in typeflag field.  */
 #define REGTYPE  '0'            /* regular file */
 #define AREGTYPE '\0'           /* regular file */
 #define LNKTYPE  '1'            /* link */
 #define SYMTYPE  '2'            /* reserved */
 #define CHRTYPE  '3'            /* character special */
 #define BLKTYPE  '4'            /* block special */
 #define DIRTYPE  '5'            /* directory */
 #define FIFOTYPE '6'            /* FIFO special */
 #define CONTTYPE '7'            /* reserved */
 
 #define XHDTYPE  'x'            /* Extended header referring to the
                                    next file in the archive */
 #define XGLTYPE  'g'            /* Global extended header */
 /* tar Header Block, from POSIX 1003.1-1990.  */
#define BLOCK_SIZE 512 

/*for bbfs functions*/
#define PATH_MAX 255
#define BB_DATA ((struct bb_state *) fuse_get_context()->private_data)


struct stat tar_st;
/*bbfs def ENDs here*/

 /* POSIX header BEGINS  */
 
 typedef struct __attribute__((__packed__)) posix_header
  {                              /* byte offset */
    char name[100];               /*   0 File name */
    char mode[8];                 /* 100 File mode (in octal ascii)*/
    char uid[8];                  /* 108 User ID (in octal ascii) */
    char gid[8];                  /* 116 Group ID (in octal ascii)*/
    char size[12];                /* 124 File size (s) (in octal ascii)*/
    char mtime[12];               /* 136 Modify time (in octal ascii)*/
    char chksum[8];               /* 148 Header checksum (in octal ascii)*/
    char typeflag;                /* 156 Link flag*/
    char linkname[100];           /* 157 Linkname ('\0' terminated, 99     maxmum     length)*/
    char magic[6];                /* 257 Magic ("ustar  \0")*/
    char version[2];              /* 263 */
    char uname[32];               /* 265 User name*/
    char gname[32];               /* 297 Group name ('\0' terminated, 31    maxmum   length)*/
    char devmajor[8];             /* 329 Major device ID (in octal ascii)*/
    char devminor[8];             /* 337 Minor device ID (in octal ascii)*/
    char prefix[155+12];           //to  make total of 512
    //char prefix[155];              345 
    //                               500 
    }  posix_header ;

typedef struct Dir_list {
    char * path;                    // absolute path name
    int block;                      // Tar block number
    int type;                           // file or dir
    int mode;
    int size;
    int mtime;
    struct Dir_list * next;             //next block
    //struct Dir_list * parent ;          // parent dir node
    //char * pth [];                  // path of dir and files of all children
}List;

List * head;

//////////////////////////////////////////

///////////////////////////////////
// for bbfs functions
char cwd [PATH_MAX];
void absolutepath(char * root_folder);
//////////////////////////////////

//return tree struct

typedef struct directory {  //unused
    char path [150];
    int block_ind;
    struct directory * next;
} tr_blk1; 

typedef struct directory_easy{  //unused
    char path[255];
    int block_ind;
} tr_blk;


int get_blk_count(int fd );

tr_blk * get_path_table(FILE * fd ); 

int isZero(posix_header * hdr);

List * path2blocknum(char  * req);
List * create(char * path, int blockno, int type, int mode, int size,
                 int mtime,  List * head  );
#endif
