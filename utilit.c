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

List * create(char * path, int block, int type, List * head  ){
    
    List * curr= (List *) malloc(sizeof(List));
    
    //max path length is 110 coz tar struct has len 100 extra 10 for debugging  
    curr -> path =strndup( path,100 ); 
    curr ->  block = block;  //block numbetr
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
    int count = 0;
    int typeflag;
    int skip;
    posix_header * hdr1;
    hdr1 = (posix_header *) malloc(sizeof(*hdr1));
    List * head;

    while (1){
        t = read(fd, hdr1, sizeof(*hdr1));  count++;
        size = strtol(hdr1 -> size, NULL, 8);
        typeflag =  strtol(&hdr1 -> typeflag, NULL, 0);
        mode = strtol (hdr1 -> mode , NULL, 0);
        printf("bytes readed %d, size = %d, type = %d\n",t, 
                size, typeflag);
        
        //printf("Name = %s\n", hdr1 -> name);
        //Filling Linked list
        head = create(hdr1 -> name, count, ((typeflag != 0)?1:0),head);
        //printf("Testing !!!!! %s\n", head -> path);
        skip =  (int) ceilf((float) (size /512.0)) ;
        skip = skip * 512;
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
    print_list(head);
    // End condition
    t = lseek(fd, 0, SEEK_SET);
    printf("Total blocks = %d\n", count);
    return count;
}


//tr_blk * get_path_table(FILE * fd );

