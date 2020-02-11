#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fuse.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define handle_error(msg)\
    do {perror(msg); exit(EXIT_FAILURE);} while(0)
#define PATH_MAX 255

/////////////////////////////////////////////////////////////////
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
   char linkname[100];           /* 157 Linkname ('\0' terminated, 99 maxmum     length)*/
   char magic[6];                /* 257 Magic ("ustar  \0")*/
   char version[2];              /* 263 */
   char uname[32];               /* 265 User name*/
   char gname[32];               /* 297 Group name ('\0' terminated, 31 maxmum   length)*/
   char devmajor[8];             /* 329 Major device ID (in octal ascii)*/
   char devminor[8];             /* 337 Minor device ID (in octal ascii)*/
   char prefix[155+12];           //to  make total of 512
   //char prefix[155];              345 
   //                               500 
   }  posix_header ;
 

char buff[512];

posix_header *  hdr = NULL; 

int bb_getattr(const char *path, struct stat *statbuf)
 {
     int retstat = 0;           //return status
     char fpath[PATH_MAX];
     

     printf("hdr -> mode = %ld, typeflag = %ld \n", strtol(hdr -> mode, NULL, 10 ), strtol(&hdr ->typeflag, NULL, 0));
     
     //statbuf -> st_mode =  

     /*retstat = log_syscall("lstat", lstat(fpath, statbuf), 0);

     log_stat(statbuf);
     */
     
     if (retstat > 0){
        handle_error("Error at BB_getattr\n");
     }
        
     return retstat;
 }


/*        /////////////         fuse_operations   */

struct fuse_operations bb_oper = {
   .getattr = bb_getattr,
   // no .getdir -- that's deprecated
   .getdir = NULL,
   //.mkdir = bb_mkdir,
   //.unlink = bb_unlink,
   //.rmdir = bb_rmdir,
   //.symlink = bb_symlink,
   //.rename = bb_rename,
   //.link = bb_link,
   //.chmod = bb_chmod,
   //.chown = bb_chown,
//   .open = bb_open,                       to be define
//   .read = bb_read,
//   .write = bb_write,
   /** Just a placeholder, don't set */ // huh???
   //.statfs = bb_statfs,
 
 
//   .opendir = bb_opendir,                 to be defined
//   .readdir = bb_readdir,
   //.releasedir = bb_releasedir,
//   .init = bb_init,                       not necessary
   //.ftruncate = bb_ftruncate,
   //.fgetattr = bb_fgetattr        /same as getattr but takes fd as input arg
 };


/////////////////////////////////////////////////////////////////





int main(int argc, char * argv[]){
    int fd;
    int t = 0;
    long int size ;
    long int typeflag;
    
    
    char * dflt_file = "x.tar";
    printf("size of struct= %lu\n", sizeof(*hdr));   
    

    hdr = (struct posix_header *)malloc(sizeof(*hdr));

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
    /*
    t = read(fd, hdr, sizeof(*hdr));
    
    printf("Name = %s\n", hdr-> name);
    printf("uid, gid = %ld, %ld\n", strtol(hdr -> uid, NULL, 8 ), strtol(hdr -> gid, NULL, 8));
    
    t = read(fd, hdr, sizeof(*hdr));
    printf("Name = %s\n", hdr-> name);
    printf("uid, gid = %ld, %ld\n", strtol(hdr -> uid, NULL, 8 ), strtol(hdr -> gid, NULL, 8));
    */
    while (1){
           
        t = read(fd, hdr, sizeof(*hdr));
        size = strtol(hdr -> size, NULL, 8);
        typeflag = strtol(& hdr -> typeflag, NULL, 0);
        printf("Name file = %s\n", hdr-> name);
        printf("uid, gid = %ld, %ld, size = %ld, typeflag = %ld\n", strtol(hdr -> uid, NULL, 8 ), strtol(hdr -> gid, NULL, 8), size, typeflag);
        
        struct stat sb;
        bb_getattr(NULL, &sb);
            

        if ((size == 0) && (typeflag == 0) )
            break;
        if ((size != 0) && ((hdr -> typeflag == REGTYPE) || 
                (hdr -> typeflag == AREGTYPE))){
            printf("<---- %ld bytes file start here\n", size);
            t = read(fd, buff, 512);
            printf("%s", buff);
            printf("<------------ EOF !!!!!!!!\n");
        }

    }

    return 0;
}
