#define FUSE_USE_VERSION 30
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fuse3/fuse.h>
#include <fcntl.h>
#include <string.h>
#include "utilit.h"

///////////////////////////////////   defs  for bbfs
#include <dirent.h>
#include <errno.h>

//#include "log.h"
#include "config.h"
//////////////////////////////////


#define handle_error(msg)\
    do {perror(msg); exit(EXIT_FAILURE);} while(0)
#define PATH_MAX 255

/////////////////////////////////////////////////////////////////
 

char buff[512];
int blk_count= 0;
struct bb_state{
     FILE * logfile;
     char * rootdir;
 };


posix_header *  hdr = NULL; 
static int tar_getattr(const char *path, struct stat *stbuf,
                struct fuse_file_info *fi) {

     int retstat = 0;           //return status
     char fpath[PATH_MAX];
     int mod;
     //HANDELING >  when char * path = "/"  
     char * root = "/";
     printf("testing !!!!!!!!!!!!!!!!!\n");
     if (strcmp(root, path) == 0){
         stbuf = &tar_st;
         printf("Returned RoOt stat !!! \n");
         return retstat;
     }
     //END
    
     //searching path into linked list and filling struct stat from Llist
     List * tmp;
     char * tst = "/test/subdir/data";
     printf("Searching Llist path: %s \n", (tst+1));
     tmp = path2blocknum(tst+1);
     printf(" Found path name: %s\n", tmp -> path);
     
     /* filling up stat struct with global hdr variable
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

     stbuf -> st_mode =   mod;
     stbuf -> st_dev = 0;
     stbuf -> st_ino = 0;
     stbuf -> st_nlink = 0;
     //stbuf -> st_uid =  strtol(hdr -> uid, NULL, 8);
     stbuf -> st_uid = getuid();
     //stbuf -> st_gid =  strtol(hdr -> gid, NULL, 8);
     stbuf -> st_gid = getgid();
     stbuf -> st_size = strtol(hdr -> size, NULL ,8);
     stbuf -> st_blksize = 512;
     stbuf -> st_blocks = blk_count;
     */

     /*retstat = log_syscall("lstat", lstat(fpath, stbuf), 0);

     log_stat(stbuf);
     */
     
     if (retstat > 0){
        handle_error("Error at BB_getattr\n");
     }
        
     return retstat;
 }


/** Function to add an entry in a readdir() operation
 *
 * The *off* parameter can be any non-zero value that enables the
 * filesystem to identify the current point in the directory
 * stream. It does not need to be the actual physical position. A
 * value of zero is reserved to indicate that seeking in directories
 * is not supported.
 * 
 * @param buf the buffer passed to the readdir() operation
 * @param name the file name of the directory entry
 * @param stat file attributes, can be NULL
 * @param off offset of the next entry or zero
 * @param flags fill flags
 * @return 1 if buffer is full, zero otherwise
 */
int tar_open(const char *path, struct fuse_file_info *fi){
    List * tmp;
    char * path_t;
    strcpy(path_t, path);
    printf("At bb_open: path = %s\n", path_t);
    tmp = path2blocknum(path_t);

    return tmp -> block;
}

static void * tar_init(struct fuse_conn_info *conn,
               struct fuse_config *cfg) {


     //log_msg("\nbb_init()\n");
 
     //log_conn(conn);
     //log_fuse_context(fuse_get_context());
     //(void) conn;
     //cfg->use_ino = 1;

     /*cfg->entry_timeout = 0;
     cfg->attr_timeout = 0;
     cfg->negative_timeout = 0;
    */
     return NULL;
 
     //return BB_DATA;
 }

/*        /////////////         fuse_operations   */

struct fuse_operations bb_oper = {
   .getattr = tar_getattr,
   // no .getdir -- that's deprecated
   //.getdir = NULL,
   //.access = bb_access,
   //.mkdir = bb_mkdir,
   //.unlink = bb_unlink,
   //.rmdir = bb_rmdir,
   //.symlink = bb_symlink,
   //.rename = bb_rename,
   //.link = bb_link,
   //.chmod = bb_chmod,
   //.chown = bb_chown,
   .open = tar_open,                       //to be define
//   .read = bb_read,
//   .write = bb_write,
   /** Just a placeholder, don't set */ // huh???
   //.statfs = bb_statfs,
 
 
//   .opendir = bb_opendir,                 to be defined
//   .readdir = bb_readdir,
   //.releasedir = bb_releasedir,
   .init = tar_init,                       //not necessary
   //.ftruncate = bb_ftruncate,
   //.fgetattr = bb_fgetattr        /same as getattr but takes fd as input arg
 };


/////////////////////////////////////////////////////////////////


int main(int argc, char * argv[]){
    int fd;
    int t = 0;
    int num =0;                     //random counter
    int fuse_stat;
    long int size, mode ;
    long int typeflag;
    struct bb_state * bb_data = (struct bb_state *) malloc(sizeof 
                                                (struct bb_state));
    
    
    char * dflt_file = "/u1/h3/hashmi/classes/os2Cs671/readtar/x.tar";
    char * dflt_mount = "/mountdir";
    printf("USAGE > ./a.out [mountdir]\n"); 

    hdr = (struct posix_header *)malloc(sizeof(*hdr));

    //open tar file
    if (argc < 2){
        fd = open(dflt_file, O_RDWR );
        //absolutepath(dflt_mount);
        bb_data -> rootdir = dflt_file;
        //strdup(bb_data -> rootdir, dflt_file);
        // getting tar file stat and saving it in global variable
        if( stat(dflt_file, &tar_st )  != 0){
            perror("Tar file stat error \n");
        }
    }
    else{
        fd = open(argv[1], O_RDWR);
        // To be added later
    }
    if (fd == -1){
        handle_error("Open Tar file!");
    }
    
    //preping  bb_state struct
    //absolutepath();
    //strcpy(bb_data -> rootdir, ); 

    //argv[argc-2] = argv[argc-1];
    //argv[argc-1] = NULL;
    //argc--;
    
    

    //counting blocks
    printf("Block Count is = %d !!!!!!!\n", blk_count);
    blk_count = get_blk_count(fd);
    printf("Block Count is = %d !!!!!!!\n", blk_count);
    
    // turn over control to fuse                    BLOCKED fuse_main !!!
     fprintf(stderr, "about to call fuse_main\n");
     //fuse_stat = fuse_main(argc, argv, &bb_oper, bb_data);
     fprintf(stderr, "fuse_main returned %d\n", fuse_stat);
    
     struct stat sb;
     tar_getattr("a", &sb, NULL);
    
    //tar_getattr()

    //Reading tar
    /*
    while (1){
           
        t = read(fd, hdr, sizeof(*hdr)); // num++;           //reading here
        printf("IN WHILE BLOCK NUMBER %d !!!!!!!!!!!!!!\n", num);
        
        size = strtol(hdr -> size, NULL, 8);
        typeflag = strtol(& hdr -> typeflag, NULL, 0);
        mode = strtol (hdr -> mode , NULL, 0);
        //stop reading more blocks cond.
           if (isZero(hdr)){
               t = read(fd, hdr, sizeof(*hdr));
               if (isZero(hdr)){
                   break;
               }
               else {
                   printf("Next block was not empty !!!!!!!!!!!\n");
               }
           }
        // End condition
        
        printf("--------------STARTS FROM HERE-----------------------");      
        printf("\n----------------------\nName file = %s\n", hdr-> name);
        printf("uid, gid = %ld, %ld, size = %ld, typeflag = %ld\n",
                strtol(hdr -> uid, NULL, 8 ), strtol(hdr -> gid, NULL, 8),
                size, typeflag);
        
        struct stat sb;
        tar_getattr("/", &sb, NULL);
            

        if ((size != 0) && ((hdr -> typeflag == REGTYPE) || 
                (hdr -> typeflag == AREGTYPE))){
            printf("<---- %ld bytes file start here\n", size);
            t = read(fd, buff, 512);                            // reading here
            printf("%s", buff);
            printf("<------------ EOF !!!!!!!!\n");
        }
    }*/
    
    close(fd);
    return 0;
}
