#define FUSE_USE_VERSION 30
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fuse3/fuse.h>
#include <fcntl.h>
#include <string.h>
#include "utilit.h"
#include <dirent.h>
#include <errno.h>

///////////////////////////////////   defs  for bbfs

//#include "log.h"
#include "config.h"
//////////////////////////////////


#define handle_error(msg)\
    do {perror(msg); /*exit(EXIT_FAILURE);*/} while(0)
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


      FILE * fdl = fopen("/u1/h3/hashmi/classes/os2Cs671/copyreadtar/2readtar/get_attr", "a+");
      fprintf(fdl, "os Path: %s\n", path);

      //HANDELING >  when char * path = "/"
      char * root = "/";
      printf("testing !!!!!!!!!!!!!!!!!\n");
      if (strcmp(root, path) == 0){
          //stbuf = &tar_st;                     //NOT working
          //filling stbuf with tar_st values
          stbuf -> st_dev = tar_st.st_dev ;
          stbuf -> st_ino = tar_st.st_ino ;
          // making it dir from file (x.tar)
          if ((tar_st.st_mode & S_IFMT) == S_IFREG) {
             tar_st.st_mode =(tar_st.st_mode & (~S_IFREG)) | (S_IFDIR);
          }
          stbuf -> st_mode = tar_st.st_mode;
          stbuf -> st_nlink = tar_st.st_nlink ;
          stbuf -> st_uid = tar_st.st_uid ;
          stbuf -> st_gid = tar_st.st_gid ;
          stbuf -> st_rdev = tar_st.st_rdev ;
          stbuf -> st_size = tar_st.st_size ;
          stbuf -> st_blksize = tar_st.st_blksize ;
          stbuf -> st_blocks = tar_st.st_blocks ;
          stbuf -> st_mtim.tv_sec = tar_st.st_mtim.tv_sec ;

          fprintf(fdl, "mode= %d, size= %ld, blocks= %ld,time = %ld ", tar_st.   st_mode, tar_st.st_size, tar_st.st_blocks, tar_st.st_mtim.tv_sec );


          printf("Returned RoOt stat !!! \n");
          return retstat;
      }
      //END
      //searching path into linked list and filling struct stat from Llist
      List * tmp;
      char * tst = path;
      //strcpy(,)
      printf("Searching Llist path: %s \n", (tst+1));
      tmp = path2blocknum(tst+1);
      if (tmp == NULL){
          return -errno;
      }

      //printf(" Found path name: %s blockno = %d\n", tmp -> path,
      //        tmp -> block);
      fprintf(fdl," Found path name: %s blockno = %d\n", tmp -> path,
              tmp -> block);

      fprintf(fdl, "mode= %d, size= %ld, blocks= %ld,time = %ld ",
              tmp->mode, tmp ->size, (long int)ceil(tmp->size/512.0), tmp ->     mtime );

      stbuf -> st_mode =   tmp -> mode;
      //stbuf -> st_mode =   16840;
      //stbuf -> st_dev = 0;
      stbuf -> st_ino = 0;
      stbuf -> st_nlink = 1;
         //stbuf -> st_uid =  strtol(hdr -> uid, NULL, 8);
      stbuf -> st_uid = getuid();
         //stbuf -> st_gid =  strtol(hdr -> gid, NULL, 8);
      stbuf -> st_gid = getgid();
      stbuf -> st_size = /*4096;*/ tmp -> size;
      stbuf -> st_blksize = 512;
      stbuf -> st_blocks =(int) ceil(tmp -> size / 512.0);        // TO BE TESTED NEXT
      stbuf ->st_mtim.tv_sec = (long ) 1581899947; //tmp -> mtime;

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
      printf("hdr -> mode = %o, typeflag = %ld \n", mod,
                strtol(&hdr ->typeflag, NULL, 0));

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
         handle_error("Error at tar_getattr\n");
      }
      fprintf(fdl,"\nEND of getattr call -----------------\n");
      fclose(fdl);
 
      return 0;
  }

/*
static int tar_access(const char *path, int mask){
   // (void) fi;
    return 0; 
} 
*/



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
    int slen = strlen(path);
    path_t = strdup(path);
    //strcpy(path_t, path);
    FILE * fdo = fopen("/u1/h3/hashmi/classes/os2Cs671/copyreadtar/2readtar/dir_open", "a+");
    
    fprintf(fdo, "os Path: %s\n", path_t);
    tmp = path2blocknum(path_t+1);
    if (tmp == NULL){
        fprintf(fdo,"osPath NOT FOUND !\n");
        fclose(fdo);
        return -errno;
    }
    fprintf(fdo,"At bb_open: path = %s, blk no: %d\n", path_t, tmp -> block);

    fi -> fh =  tmp -> block;     // unlock this
    fclose(fdo);
    return 0;
}

static int tar_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi){
    
    FILE * fdw = fopen("/u1/h3/hashmi/classes/os2Cs671/copyreadtar/2readtar/dir_FILE", "a+");
    //printf("DIR_FILE IS OPENED\n");
    fprintf(fdw, "\ntar_read call start--------------------\n");
    char * path_t;
    List * curr;
    off_t pnt_blk;
    ssize_t bytes_r;
    path_t = strdup(path);
    
    if (fi == NULL)
        return -1;
        
    fprintf(fdw, "os Path len: %s , size: %ld compare: %d\n",
            path, size, strcmp(tab1 -> fname, path_t));
    if (strcmp(tab1 -> fname, path_t) != 0){
        strcpy(tab1 -> fname, path_t);
        curr = path2blocknum(path_t+1);
        
        tab1 -> block = curr -> block;
        tab1 -> mode = curr -> mode;
        tab1 -> size = curr -> size;
        tab1 -> open_flg = 1;
        tab1 -> sent = 0;
        
        // actual file size: 1305 asking 4096 refer: 3dir_FILE
        //if (size < curr -> size){
        //    tab1 -> sent = tab1 -> sent + size;
        //}
        //for lseek
        pnt_blk = ( curr -> block + 1 )*512;
    
        fprintf(fdw,"Match 1 found path: %s at block:%d, tar block: %ld , size %d, asked: %lu, offset: %ld, fh: %d \n",
            curr -> path, curr -> block,pnt_blk, curr -> size, size, offset, fi->fh);
  
        //lseek(fd_tar,pnt_blk , SEEK_SET);
        //bytes_r =  read(fd_tar, buf, size+ offset);
        fprintf(fdw,"tar_read ended ------------------\n");
        if (bytes_r == -1)
            bytes_r = -errno;
        else{
            tab1 -> sent = 0;
            tab1 -> sent += bytes_r;
        }
        fclose(fdw);
        return bytes_r;
    }


    else{
        
    fprintf(fdw,"\n2nd call to tar_read ----- \n");
    fprintf(fdw, "OS path: %sread ahead file: %s, already sent; %d bytes, asked: %lu\n"
                ,path_t, tab1 -> fname, tab1 -> sent, size );
    

    pnt_blk = ( tab1 -> block + 1 )*512;
    
    fprintf(fdw,"Match found path: %s at block:%d, tar block: %ld , size %d, asked: %lu, offset: %ld\n",
        tab1 -> fname, tab1 -> block,pnt_blk, tab1 -> size, size, offset);
    
    
    bytes_r =  pread(fd_tar, buf, size, pnt_blk+offset);
    if (bytes_r == -1)
        bytes_r = -errno;
    else{
        //tab1 -> sent = 0;
        tab1 -> sent += bytes_r;
    }
   

    fclose(fdw);
    printf("tar_read ended ------------------\n");
    return bytes_r;

    }

}




static int tar_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                off_t offset, struct fuse_file_info *fi,
                enum fuse_readdir_flags flags){
/*     
 static int rdir(const char *path, void *buf){ 
//                off_t offset, struct fuse_file_info *fi,
//                enum fuse_readdir_flags flags){
*/
    List * curr = head;
     int slashflg = 99 ;                  // gook if 0 bad if . > 0
     struct stat st;
     int rootflg = 0;
     int os_plen;
     //t_path = path;
     strcpy(t_path,path);
    
    os_plen = strlen(t_path);
     
     if ((strcmp(t_path , "/")) == 0){
         rootflg = 1;
     }
    
     FILE * fdr = fopen("/u1/h3/hashmi/classes/os2Cs671/copyreadtar/2readtar/dir_read", "a+  ");

     printf("at tar_readdir----------------------\n");
     // itering through entire lList
     while (curr != NULL){
          slashflg = count_extra_slash(t_path, curr -> path, rootflg);
          if (slashflg == 0 && ( curr -> path[0] != '\0') ){
            
             fprintf(fdr, "os Path: %s subfiles: %s root: %d\n", t_path, curr->path, rootflg );
            //if (curr -> path == NULL /){
            //    fprintf(fdr,"Empty path Mathched \n");
            //}
             memset(&st, 0, sizeof(st));
             st.st_ino = 0;
             st.st_mode = curr -> mode;
             if( rootflg == 1 ){
             if (filler(buf, curr -> path ,  &st, 0, 0)){
                 fclose(fdr);
                 break;}
             }
             else{
                if (filler(buf, curr -> path + os_plen ,  &st, 0, 0)){
                    fclose(fdr);
                    break;}

             }

          }
        
        curr = curr -> next;
     }
     fclose(fdr);
     rootflg = 0;
    return 0;
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
   //.access = tar_access,
   //.mkdir = bb_mkdir,
   //.unlink = bb_unlink,
   //.rmdir = bb_rmdir,
   //.symlink = bb_symlink,
   //.rename = bb_rename,
   //.link = bb_link,
   //.chmod = bb_chmod,
   //.chown = bb_chown,
   .open = tar_open,                       //to be define
   .read = tar_read,
//   .write = bb_write,
   /** Just a placeholder, don't set */ // huh???
   //.statfs = bb_statfs,
 
 
//   .opendir = bb_opendir,                 to be defined
   .readdir = tar_readdir,
   //.releasedir = bb_releasedir,
   .init = tar_init,                       //not necessary
   //.ftruncate = bb_ftruncate,
   //.fgetattr = bb_fgetattr        /same as getattr but takes fd as input arg
 };


/////////////////////////////////////////////////////////////////



int main(int argc, char * argv[]){
    //int fd;
    int t = 0;
    int num =0;                     //random counter
    int fuse_stat;
    long int size, mode ;
    long int typeflag;
    struct bb_state * bb_data = (struct bb_state *) malloc(sizeof 
                                                (struct bb_state));
    
    //char * dflt_file = "/u1/h3/hashmi/classes/os2Cs671/copyreadtar/2readtar/x.tar";
    
    char * dflt_file = "/u1/h3/hashmi/classes/os2Cs671/copyreadtar/2readtar/bigdir.tar";
    strcpy(tar_path, dflt_file);
    char * dflt_mount = "mountdir";
    printf("USAGE > ./a.out [mountdir]\n"); 

    hdr = (struct posix_header *)malloc(sizeof(*hdr));
    tab1 = (struct open_files *) malloc(sizeof(* tab1));

    ;//open tar file
    if (argc < 2){
        fd_tar = open(dflt_file, O_RDWR );
        //absolutepath(dflt_mount);
        bb_data -> rootdir = dflt_file;
        //strdup(bb_data -> rootdir, dflt_file);
        // getting tar file stat and saving it in global variable
        argc = 2;
        argv[1] = strdup(dflt_mount);
        argv[2] = NULL;
        printf("FILE NAME after = %s\n", argv[1]);
        if( stat(dflt_file, &tar_st )  != 0){
            perror("Tar file stat error \n");
        }
    }
    else{
        fd_tar = open(argv[1], O_RDWR);
        // To be added later
    }
    if (fd_tar == -1){
        handle_error("Open Tar file!");
    }
    
    //preping  bb_state struct
    //absolutepath();
    //strcpy(bb_data -> rootdir, ); 

    //argv[argc-2] = argv[argc-1];
    //argv[argc-1] = NULL;
    //argc--;
    open(dflt_file, O_RDWR );
    

    //counting blocks
    printf("Block Count is = %d !!!!!!!\n", blk_count);
    blk_count = get_blk_count(fd_tar);
    printf("Block Count is = %d !!!!!!!\n", blk_count);
    
    // turn over control to fuse                    BLOCKED fuse_main !!!
     fprintf(stderr, "about to call fuse_main\n");
     //fuse_stat = fuse_main(argc, argv, &bb_oper, bb_data);
     umask(0);
     fuse_main(argc, argv, &bb_oper, NULL);         //WORKING fuse command

     fprintf(stderr, "fuse_main returned %d\n", fuse_stat);
    
     struct stat sb;
     //tar_getattr("/test/subdir/data", &sb, NULL);
    //       static int tar_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi,
    //                 enum fuse_readdir_flags flags)
    // rdir("/test",t_buff );
    
    
     printf("TESTING \n");
    


   // struct fuse_file_info *fi;
    //fi =  sizeof(*fi);
    //tar_open(const char *path, struct fuse_file_info *fi){
    //tar_open("/test/b.py", fi);
    //printf("block no: %lu\n", fi -> fh );

    //static int tar_read(const char *path, char *buf, size_t size, off_t offset,
    //    struct fuse_file_info *fi);
 //   tar_read("/test/b.py", t_buff, 100, 0, fi );
 //   printf("BUFFER : %s\n", t_buff);





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
    
    close(fd_tar);
    return 0;
}
