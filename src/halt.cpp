#include <stdio.h>
#include <string.h>
#include "filesys.h"

void write_data_to_disk(const char* filename) {
    FILE* file = fopen(filename, "wb"); // 以二进制写入模式打开文件
    if (file == NULL) {
        printf("Failed to open file for writing: %s\n", filename);
        return;
    }
    // 写入disk
    fwrite(disk, sizeof(char), (DINODEBLK + FILEBLK + 2) * BLOCKSIZ, file);
    fclose(file);
}

void halt(){
	int i,j;
    /*1. write back the current dir */
    chdir("..");
    iput(cur_path_inode);

    /*2. free the u_ofile and sys_ofile and inode*/
    //是不是要写回filsys
    for(i=0; i<USERNUM; i++){
        if(user[i].u_uid != 0){
            for(j=0; j<NOFILE;j++){
                if(user[i].u_ofile[j] != SYSOPENFILE + 1){
                    close(i,j);
                    user[i].u_ofile[j] = SYSOPENFILE + 1;
                }//if
            }//for
        }//if
    }//for

    /*3. write back the filesys to the disk*/
    memcpy(disk+BLOCKSIZ, &filsys, sizeof(struct filsys));

    /*4. close the file system column*/
    write_data_to_disk( "../data/data.bin");

    /*5. say GOOD BYE to all the user*/
    printf("\nGood Bye. See You Next Time. Please turn off the switch\n");
    exit(0);
}






























