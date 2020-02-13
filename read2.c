#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fuse.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "utilit.h"

#define handle_error(msg)\
    do {perror(msg); exit(EXIT_FAILURE);} while(0)
#define PATH_MAX 255

/////////////////////////////////////////////////////////////////
 

char buff[512];
int blk_count= 0;


posix_header *  hdr = NULL; 

int bb_getattr(const char *path, struct stat *statbuf)
 {
     int retstat = 0;           //return status
     char fpath[PATH_MAX];
     int mod;
     
     mod = (int) strtol(hdr -> mode, NULL, 8);
     // if a regfile type
     if ((hdr -> typeflag == REGTYPE) || (hdr -> typeflag == AREGTYPE) ){
         mod = mod | (1 << 15);
     }
     //if a directory type
     if (hdr -> typeflag == DIRTYPE)
         mod = mod | (1 << 14);
    //debugging
     printf("hdr -> mode = %o, typeflag = %ld \n", mod, strtol(&hdr ->typeflag, NULL, 0));

     statbuf -> st_mode =   mod;
     statbuf -> st_dev = 0;
     statbuf -> st_ino = 0;
     statbuf -> st_nlink = 0;
     statbuf -> st_uid =  strtol(hdr -> uid, NULL, 8);
     statbuf -> st_gid =  strtol(hdr -> gid, NULL, 8);
     statbuf -> st_size = strtol(hdr -> size, NULL ,8);
     statbuf -> st_blksize = 512;
     statbuf -> st_blocks = blk_count;

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
    long int size, mode ;
    long int typeflag;
    
    
    char * dflt_file = "x.tar";
    

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
    //counting blocks
    printf("Block Count is = %d !!!!!!!\n", blk_count);
    blk_count = get_blk_count(fd);
    printf("Block Count is = %d !!!!!!!\n", blk_count);
    
    //Reading tar
    
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
        blk_count++;
        
        size = strtol(hdr -> size, NULL, 8);
        typeflag = strtol(& hdr -> typeflag, NULL, 0);
        mode = strtol (hdr -> mode , NULL, 0);
        //stop reading more blocks cond.
        if ((mode == 0)&&(hdr -> uid == 0)&&(hdr ->gid == 0)
                &&(size == 0) && (typeflag == 0)){         {
            t = read(fd, hdr, sizeof(*hdr));
            if (( mode == 0)&&(hdr -> uid == 0)&&(hdr ->gid == 0)
                &&(size == 0) && (typeflag == 0))         
                 break;
        }
        printf("--------------STARTS FROM HERE-----------------------");      
        printf("\n----------------------\nName file = %s\n", hdr-> name);
        printf("uid, gid = %ld, %ld, size = %ld, typeflag = %ld\n",
                strtol(hdr -> uid, NULL, 8 ), strtol(hdr -> gid, NULL, 8),
                size, typeflag);
        
        struct stat sb;
        bb_getattr(NULL, &sb);
            

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
