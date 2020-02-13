#include "utilit.h"


int get_blk_count(int fd  ){

    int t = 0;
    int size ;
    int count = 0;
    int type;
    posix_header * hdr1;
    hdr1 = (posix_header *) malloc(sizeof(*hdr1));
    while (1){
        t = read(fd, hdr1, sizeof(*hdr1));
        size = strtol(hdr1 -> size, NULL, 8);
        type =  strtol(&hdr1 -> typeflag, NULL, 0);
        printf("bytes readed %d, size = %d, type = %d\n",t, 
                size, type);
        
        printf("Name = %s\n", hdr1 -> name);
        if ((size == 0) && (type == 0) ){
            printf("Broke !!!\n");break;}
    }
    t = lseek(fd, 0, SEEK_SET);
    printf("Total blocks = %d\n", count);
    return count;

}

//tr_blk * get_path_table(FILE * fd );

