#include <stdio.h>
#include "filesys.h"
/*********************************************************************
函数：creat
功能：创建文件，存在且可写则覆盖，否则申请i节点，并打开该文件，返回文件指针
**********************************************************************/
int creat(unsigned int user_id, char *filename, unsigned short mode){
    struct inode *inode;
    int dirid,di_ith;
    int i,j;

    dirid = namei(filename);
    if (dirid != -1 ){//如果存在同名文件/目录
        inode = iget(dir.direct[dirid].d_ino);
        if(!(inode->di_mode&DIFILE)){//如果不是文件
            iput(inode);
            printf("存在同名目录！\n");
            return -1;
        }
        if (access(user_id,inode,WRITE) == 0){
            iput(inode);
            printf("\n creat access not allowed \n");
            return -1;
        }
        //清除原有inode对应的东西
        j = inode->di_size%512?1:0;
        for (i=0; i<inode->di_size/BLOCKSIZ+j; i++)
            bfree(inode->di_addr[i]);

        for (i=0; i<SYSOPENFILE; i++){
            if (sys_ofile[i].f_count != 0 && sys_ofile[i].f_inode == inode){
                sys_ofile[i].f_off = 0;
            }
        }
        return open(user_id,filename,WRITE);
    }else{//若不存在同名文件或目录
        inode = ialloc();
        di_ith = iname(filename);

        dir.size++;
        dir.direct[di_ith].d_ino = inode->i_ino;
        inode->di_mode = mode;
        inode->di_uid = user[user_id].u_uid;
        inode->di_gid = user[user_id].u_gid;
        inode->di_size = 0;
        inode->di_number = 1;			//liwen change to 1
        iput(inode);
        return open(user_id,filename,WRITE);
    }
    return 0;
}

int chname(char *filename, char *newname) {
    struct inode *inode;
    int dirid;
    dirid = namei(filename);
    if(dirid==-1){
        printf("该文件不存在");
        return -1;
    }
    inode = iget(dir.direct[dirid].d_ino);
    if (access(user_id,inode,WRITE) == 0){
        iput(inode);
        printf("\n creat access not allowed \n");
        return -1;
    }
    strcpy(dir.direct[dirid].d_name,newname);
    printf("成功将源文件%s更改为%s\n",filename,newname);
    return 0;

}

int copy_file(char* srcfile,char* dstfile) {
    struct inode *src_inode, *dst_inode;;
    int src_dirid, dst_dirid;
    int i,j;
    src_dirid = namei(srcfile);
    dst_dirid = namei(dstfile);

    if (src_dirid == -1) {
        printf("源文件不存在");
        return -1;
    }

    src_inode = iget(dir.direct[src_dirid].d_ino);
    dst_inode = iget(dir.direct[dst_dirid].d_ino);

    if (dst_dirid != -1) {
        printf("目标文件存在，将被全部删除，以源文件的内容取代，你确定要继续更改吗，按y继续，按n取消本操作\n>");
        char choice[2];
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = '\0';  // 去除换行符
        if (strcmp(choice, "y") == 0 || strcmp(choice, "Y") == 0) {
            printf("++++继续执行++++\n");
            dst_inode = iget(dir.direct[dst_dirid].d_ino);
            j = dst_inode->di_size%512?1:0;
            for (i=0; i<dst_inode->di_size/BLOCKSIZ+j; i++)
                bfree(dst_inode->di_addr[i]);

            for (i=0; i<SYSOPENFILE; i++){
                if (sys_ofile[i].f_count != 0 && sys_ofile[i].f_inode == dst_inode){
                    sys_ofile[i].f_off = 0;
                }
            }
        } else {
            printf("操作被取消\n");
            return -1;
        }
    }
    int di_ith;

    // 复制文件内容 todo
    dst_inode = ialloc();
    di_ith = iname(dstfile);
    dir.size++;
    dir.direct[di_ith].d_ino = dst_inode->i_ino;
    dst_inode->di_mode = src_inode->di_mode;
    dst_inode->di_uid = user[user_id].u_uid;
    dst_inode->di_gid = user[user_id].u_gid;
    dst_inode->di_size = 0;
    dst_inode->di_number = 1;
    iput(dst_inode);

    return 0;
}




