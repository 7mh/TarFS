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

int get_blk_count(int fd  ){

    int t = 0;
    int size, mode;
    int count = 0;
    int typeflag;
    posix_header * hdr1;
    hdr1 = (posix_header *) malloc(sizeof(*hdr1));
    while (1){
        t = read(fd, hdr1, sizeof(*hdr1));  count++;
        size = strtol(hdr1 -> size, NULL, 8);
        typeflag =  strtol(&hdr1 -> typeflag, NULL, 0);
        mode = strtol (hdr1 -> mode , NULL, 0);
        printf("bytes readed %d, size = %d, type = %d\n",t, 
                size, typeflag);
        
        printf("Name = %s\n", hdr1 -> name);
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
    // End condition
    t = lseek(fd, 0, SEEK_SET);
    printf("Total blocks = %d\n", count);
    return count;

}

//tr_blk * get_path_table(FILE * fd );

