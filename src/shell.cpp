#include "filesys.h"
#define CLEN 10
#define CNUM 20
//enum ctype
void printDiskStructures();
char commands[CNUM][CLEN]={
        "exit",
        "dir",
        "mkdir",
        "cd",
        "mkfile",
        "del",
        "write",
        "read",
        "password",
        "who",
        "debug"
};
int getcid(char *command){
    int i;
    if(command == NULL)
        return -1;
    for(i=0;i<CNUM;i++){
        if(strcmp(command,commands[i]) == 0)
            return i;
    }
    return -1;
}
int shell(int user_id,char *str){
    char seps[] =" \t\n\0";
    char* token,*tstr,*buf;
    unsigned short mode;
    int fd;//只有fd取int才能使返回-1的错误码正确显示
    int cid,size;
    token = strtok(str,seps);
    if(token == NULL)
        return 1;
    cid = getcid(token);
    switch(cid){
        case 1:
            _dir();
            break;
        case 2:
            token = strtok(NULL,seps);
            if(token == NULL){
                printf("mkdir命令的正确格式为mkdir dirname，请检查命令!\n");
                break;
            }
            mkdir(token);
            break;
        case 3:
            token = strtok(NULL,seps);
            if(token == NULL){
                printf("cd命令的正确格式为cd dirname，请检查命令!\n");
                break;
            }
            chdir(token);
            break;
        case 4:
            token = strtok(NULL,seps);
            if(token == NULL){
                printf("mkfile 命令的正确格式为mkfile filename [mode]，请检查命令!\n");
                break;
            }
            tstr =token;
            mode = DEFAULTMODE;
            token = strtok(NULL,seps);
            if(token != NULL){
                sscanf(token,"%o",&mode);
            }
            mode = mode|DIFILE|0700;
            fd = creat(user_id,tstr,mode);
            if(fd == -1){
                printf("创建文件失败！\n");
                break;
            }
            close(user_id,fd);
            break;
        case 5:
            token = strtok(NULL,seps);
            if(token == NULL){
                printf("del 命令的正确格式为del filename，请检查命令!\n");
                break;
            }
            delete(token);
            break;
        case 6:
            mode = WRITE;
            token = strtok(NULL,seps);
            tstr = token;
            token = strtok(NULL,seps);
            if(token == NULL){
                printf("write 命令的正确格式为write filename bytes，请检查命令!\n");
                break;
            }
            if(token[0] == '-'){
                if(token[1] == 'a')
                    mode = FAPPEND;
            }else{
                sscanf(token,"%d",&size);
            }
            fd = open(user_id,tstr,char(mode));
            buf = (char*)malloc(size);
            size = write(fd,buf,size);
            printf("%d bytes have been writed in file %s.\n",size,tstr);
            free(buf);
            close(user_id,fd);
            break;
        case 7:
            token = strtok(NULL,seps);
            tstr = token;
            token = strtok(NULL,seps);
            if(token == NULL){
                printf("read �������ȷ��ʽΪread filename bytes����������!\n");
                break;
            }
            sscanf(token,"%d",&size);
            fd = open(user_id,tstr,READ);
            buf = (char*)malloc(size+1);
            size = read(fd,buf,size);
            printf("%d bytes have been read in buf from file %s.\n",size,tstr);
            free(buf);
            close(user_id,fd);
            break;
        case 8:
            token = strtok(NULL,seps);
            chpwd(token);
            break;
        case 9:
            userinfo();
            break;
        case 0:
            halt();
            return 0;
        case 10:
            printDiskStructures();
            break;
        default:
            printf("����:û������%s��\n",token);
            break;
    }
    return 1;
}

void printDiskStructures() {
    struct filsys* superblock = (struct filsys*)&disk[BLOCKSIZ];
    struct inode* d_inode = (struct inode*)&disk[BLOCKSIZ * 3];
    struct file* files = (struct file*)&disk[BLOCKSIZ * (3 + DINODEBLK)];

    printf("Superblock:\n");
    printf("s_isize: %hu\n", superblock->s_isize);
    printf("s_fsize: %lu\n", superblock->s_fsize);
    printf("s_nfree: %u\n", superblock->s_nfree);
    printf("s_pfree: %hu\n", superblock->s_pfree);
    printf("s_ninode: %u\n", superblock->s_ninode);
    printf("s_pinode: %hd\n", superblock->s_pinode);
    printf("s_rinode: %u\n", superblock->s_rinode);
    printf("s_fmod: %c\n\n", superblock->s_fmod);

    //把这里改成打印inode信息
    printf("Inodes:\n");
    for (int i = 0; i < superblock->s_ninode; i++) {
        printf("Inode %d:\n", i + 1);
        printf("i_flag: %c\n", d_inode[i].i_flag);
        printf("i_ino: %u\n", d_inode[i].i_ino);
        printf("i_count: %u\n", d_inode[i].i_count);
        printf("di_number: %hu\n", d_inode[i].di_number);
        printf("di_mode: %hu\n", d_inode[i].di_mode);
        printf("di_uid: %hu\n", d_inode[i].di_uid);
        printf("di_gid: %hu\n", d_inode[i].di_gid);
        printf("di_size: %hu\n", d_inode[i].di_size);
        printf("di_addr: ");
        for (int j = 0; j < NADDR; j++) {
            printf("%u ", d_inode[i].di_addr[j]);
        }
        printf("\n\n");
    }


    printf("Files:\n");
    for (int i = 0; i < FILEBLK; i++) {
        printf("File %d:\n", i + 1);
        printf("f_flag: %c\n", files[i].f_flag);
        printf("f_count: %u\n", files[i].f_count);
        printf("f_off: %lu\n\n", files[i].f_off);
    }
}