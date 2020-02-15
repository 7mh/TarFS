 /*
   Big Brother File System
   Copyright (C) 2012 Joseph J. Pfeiffer, Jr., Ph.D. <pfeiffer@cs.nmsu.edu>
 
   This program can be distributed under the terms of the GNU GPLv3.
   See the file COPYING.
 
   This code is derived from function prototypes found /usr/include/fuse/fuse.h
   Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>
   His code is licensed under the LGPLv2.
   A copy of that code is included in the file fuse.h
   
   The point of this FUSE filesystem is to provide an introduction to
   FUSE.  It was my first FUSE filesystem as I got to know the
   software; hopefully, the comments in this code will help people who
   follow later to get a gentler introduction.
 
   This might be called a no-op filesystem:  it doesn't impose
   filesystem semantics on top of any other existing structure.  It
   simply reports the requests that come in, and passes them to an
   underlying filesystem.  The information is saved in a logfile named
   bbfs.log, in the directory from which you run bbfs.
 */

#include "utilit.h"

int isZero(posix_header * hdr){
    int count = 0;
    char * tmp = (char *) hdr;
    //printf("at isZero function  !!!!!!!!!%d \n",sizeof(*tmp) );
    
    for (int i =0; i< 512; i++){
        if (tmp[i] == 0 ){
            count++;
        }
    }
    if (count == 512){
        //printf("Parsed 512 Blocks !!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        return 1;
    }
    else
        return 0;
}

List * create(char * path, int blockno, int type, int mode, int size,
                int mtime,  List * head  ){
    
    List * curr= (List *) malloc(sizeof(List));
    
    //max path length is 110 coz tar struct has len 100 extra 10 for debugging  
    curr -> path =strndup( path,100 ); 
    curr ->  block = blockno;  //block numbetr
    curr -> type = type;    // 0 if file  and 1 if dir
    
    if (head == NULL){
        curr -> next = NULL;
        printf("First Block here !!!!!!!!\n");
        printf("%s\n", curr -> path);
        return curr;
    }
    else{
        curr -> next = head;
        printf("Next block\n");
        printf("%s\n", curr -> path);
        return curr;
    }
}

void print_list(List * head   ){
    printf("printing Link list !!!!!!!!!!!!!!!!!!!!\n");
    printf("print head %s\n", head -> path);
    while(head){
        printf("%s\n", head -> path);
        head = head -> next;
    }
}

int get_blk_count(int fd  ){

    int t = 0;
    int size, mode;
    int count = 0;      // this is blockno at Llist
    int typeflag;
    int skip;
    posix_header * hdr1;
    hdr1 = (posix_header *) malloc(sizeof(*hdr1));
    //List * head;  // making first header a global var

    while (1){
        t = read(fd, hdr1, sizeof(*hdr1));  count++;
        size = strtol(hdr1 -> size, NULL, 8);
        typeflag =  strtol(&hdr1 -> typeflag, NULL, 0);
        mode = strtol (hdr1 -> mode , NULL, 0);
         // if a regfile type
        if ((hdr1 -> typeflag == REGTYPE) || (hdr1 -> typeflag == AREGTYPE) ){
            mode = mode | (1 << 15);
        }
        //if a directory type
        if (hdr1 -> typeflag == DIRTYPE)
            mode = mode | (1 << 14);

        printf("bytes readed %d, size = %d, type = %d,mode= %o\n"
                ,t,size, typeflag, mode);
        
        //printf("Name = %s\n", hdr1 -> name);
        //Filling Linked list last arg is Llist head
        head = create(hdr1 -> name, count, ((typeflag != 0)?1:0),mode, 
                size, strtol(hdr1 -> mtime,NULL,8)  ,head);
        //printf("Testing !!!!! %s\n", head -> path);

        //SKIPPING data blocks of data files
        skip =  (int) ceilf((float) (size /(float) BLOCK_SIZE)) * BLOCK_SIZE ;
        //skip = skip * 512;
        //printf("Skipping %d bytes for file data size %d \n ", skip, size);
        if ((size > 0) ||(typeflag <= 0)){
            lseek(fd,  skip,SEEK_CUR);
        }

        //stop reading more blocks cond.
        if (isZero(hdr1)){
            t = read(fd, hdr1, sizeof(*hdr1));
            if (isZero(hdr1)){
                break;
            }
            else {
                printf("Next block was not empty !!!!!!!!!!!\n");
            }
        }
    }

    // print list
    print_list(head);     // this works
    printf("DOne printing \n");
    

    //absolutepath("/ROOT_DIR");
    //printf("Absolute path = %s\n", cwd);
    // End condition
    t = lseek(fd, 0, SEEK_SET);
    printf("Total blocks = %d\n", count);
    return count;
}

// for directory respective block is returned 
// But for files the Next block is returned Since File name and contents are
// stored in separate blocks
List * path2blocknum(char  * req){
    List * curr = head;
    while(curr){
        if (strcmp(req, curr -> path) == 0 ){
            return curr;
        }
        else{
            curr = curr -> next;
        }
    }
    perror("Path Not found in Tar File - path2blocknum\n");
}
//This function writes into global variable = (cwd + root_folder)
void absolutepath(char * root_folder){
   char * tmp;
   strcpy (tmp ,cwd);
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
       //printf("Current working dir (absolutepath funct): %s\n", cwd);
       strcpy(cwd , strcat(cwd, root_folder));
       //printf("Combined path is = %s\n", cwd);
   } else {
       perror("getcwd() error");
   }
}


////////////////////////////////////////////////////////////////////////
/* from bbfs */
/*static void bb_fullpath(char fpath[PATH_MAX], const char *path)
 {
     absolutepath()
     strcpy(fpath, BB_DATA->rootdir);
     strncat(fpath, path, PATH_MAX); // ridiculously long paths will
                     // break here
 
     //log_msg("    bb_fullpath:  rootdir = \"%s\", path = \"%s\", fpath = \"%s\"\n",
     //    BB_DATA->rootdir, path, fpath);
 }*/
 
////////////////////////////////////////////////////////////////////////


//tr_blk * get_path_table(FILE * fd );

