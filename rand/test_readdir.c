#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

/*Struct Area !!! */
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////

// for 0 or 1 extra '/' returns 0
// for more than 1 extra '/' return 1
// if both paths are same return 2
int count_extra_slash(char * os_path, char * lst_path, int flg){ 
    int count = 0;
    int match;
    int os_pathlen = strlen(os_path);
    int lst_pathlen = strlen(lst_path)+1;
    char tmp[512] = "/";
    strcat(tmp, lst_path);
    printf("LST : %s\n", tmp);
    
    if (strcmp(os_path, tmp) == 0){
        return 2;
    }
    for (int i = 0; i < os_pathlen; i++){   //compare matchd char 
        if (tmp[i] == os_path[i] )
            match++;
        else 
            return 99;
    }


    for (int i = os_pathlen ; i <= lst_pathlen; i++){  // count '/'
        printf("%c", tmp[i]);
        if (tmp[i] == '/'){
            count++;
        }
        if (count > 2)      // more than 1 '/' found return
            return 1;
    }
    if (flg == 1 && count == 0 )    // if asked for root and no '/' 
            return 0;
    if (flg == 0 && count == 1 )    // root flg 0 and found '/'
        return 0;
    //os Path: /bigdir/bill/shakes subfiles: bigdir/bill/shakespeare.txt root: 0
    if (flg == 0 && count == 0)     // root flg 0 and not found '/'
        return 98;

    return count; 
}


int main(int argc, char * argv[], char * envp[] ){
    
    char * tmp;
    char str[] ="/u1/h3/hashmi/classes/os2Cs671/copyreadtar/readtar/";
    //char req[] ="/u1/h3/hashmi/classes/os2Cs671/copyreadtar";
    //char req1[] ="/u1/h3/hashmi/classes/os2Cs671/copyreadtar/";
    
    //char req[] = "/";
    char req[] = "/test";
    //char req[] = "/test/subdir";

    char req1[] = "test/b.py";
    char req2[] = "test/subdir/data";
    char req3[] = "test/subdir";
    char req4[] = "test";

    int slashCount = 0;
    
    // ZERO return value is Good

    /*      for root ospath '/'  
    slashCount = count_extra_slash(req, req1,1); 
    printf("return value = %d\n", slashCount);
    slashCount = count_extra_slash(req, req2,1); 
    printf("return value = %d\n", slashCount);
    slashCount = count_extra_slash(req, req3,1); 
    printf("return value = %d\n", slashCount);
    slashCount = count_extra_slash(req, req4,1); 
    printf("return value = %d\n", slashCount);
    */

    
    slashCount = count_extra_slash(req, req1,0); 
    printf("\nreturn value 1  = %d\n", slashCount);
    slashCount = count_extra_slash(req, req2,0); 
    printf("\nreturn value 2 = %d\n", slashCount);
    slashCount = count_extra_slash(req, req3,0); 
    printf("\nreturn value 3 = %d\n", slashCount);
    slashCount = count_extra_slash(req, req4,0); 
    printf("\nreturn value 4 = %d\n", slashCount);
    



    /*
    printf ("Splitting string \"%s\" into tokens:\n",str);
    pch = strtok (str,"/");
    while (pch != NULL)
    {
        printf ("%s\n",pch);
        pch = strtok (NULL, "/");
    }*/

    ///                     test READDIR
    /*
    DIR * dp;
    struct dirent * de;
    int count =0;
    
    dp = opendir("/u1/h3/hashmi/classes/os2Cs671/copyreadtar/readtar");
    if (dp == NULL)
        perror("Error at opendir\n");

    //struct stat sb;
    
    while ((de = readdir(dp)) != NULL){
    //if (de == NULL )
    //    perror("Error at readdir\n");
    printf("path : %s\n", de -> d_name);
        if (count == 5){
            break;
        }
    count ++;
    }
    */

    return 0;
}
