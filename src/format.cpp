#include <stdio.h>
#include <string.h>
#include "filesys.h"

void format(){
	struct inode *inode;
	struct direct dir_buf[BLOCKSIZ/(DIRSIZ+4)];
	struct pwd passwd[32];
	unsigned int block_buf[BLOCKSIZ/sizeof(int)];
	int i,j;

	//初始化硬盘
    FILE* disk = fopen("../data/data.bin", "rb"); // 以二进制读取模式打开文件
    if(disk==NULL){
        memset(disk, 0x00, ((DINODEBLK+FILEBLK+2)*BLOCKSIZ));
    }

	/* 0.initialize the passwd */
    /*pwd 不存外存了，直接硬编码在代码里吧*/
	passwd[0].p_uid = 2116;
	passwd[0].p_gid = 03;
	strcpy(passwd[0].password, "dddd");

	passwd[1].p_uid = 2117;
	passwd[1].p_gid = 03;
	strcpy(passwd[1].password, "bbbb");

	passwd[2].p_uid = 2118;
	passwd[2].p_gid = 04;
	strcpy(passwd[2].password, "abcd");  

	passwd[3].p_uid = 2119;
	passwd[3].p_gid = 04;
	strcpy(passwd[3].password, "cccc");

	passwd[4].p_uid = 2120;
	passwd[4].p_gid = 05;
	strcpy(passwd[4].password, "eeee");

	/* 1.creat the main directory and its sub dir etc and the file password */

    //初始化前三个目录项，分别是空，.和..
	inode = iget(0);   /* 0 empty dinode id*/
	inode->di_number = 1;			//??空i节点有何用????->我不到啊
	inode->di_mode = DIEMPTY;
	iput(inode);

	inode = iget(1);   /* 1 main dir id*/
	inode->di_number = 1;
	inode->di_mode = DEFAULTMODE | DIDIR;
	inode->di_size = 3*(DIRSIZ + 4);
	inode->di_addr[0] = 0; /*block 0# is used by the main directory*/

	strcpy(dir_buf[0].d_name,".");
	dir_buf[0].d_ino = 1;
	strcpy(dir_buf[1].d_name,"..");
	dir_buf[1].d_ino = 1;
	strcpy(dir_buf[2].d_name,"etc");
	dir_buf[2].d_ino = 2;

    //disk存取之1：放入DATASTART部分，即第一个dinode块
    if(memcmp(disk + DATASTART, "\x00", 1) == 0){
        //若disk为空，从format里初始化的变量，拷贝到disk里
        memcpy(disk+DATASTART, &dir_buf, 3*(DIRSIZ+4));
    }else{
        // 若disk不为空，从disk拷贝上一次储存好的值
        // memcpy(&dir_buf, disk+DATASTART, 3*(DIRSIZ+4));
        // ssize_t pread(int fd, void buf[.count], size_t count,
        //                     off_t offset);
        if(fseek(disk,DATASTART,SEEK_DATA)!=0){
            perror("设定磁盘读取offset错误");
        }
        fread(dir_buf,3*(DIRSIZ+4),1,disk);
        rewind(disk);
    }
	iput(inode);


	inode = iget(2);  /* 2  etc dir id */ 
	inode->di_number = 1;
	inode->di_mode = DEFAULTMODE | DIDIR;
	inode->di_size = 3*(DIRSIZ + 4);
	inode->di_addr[0] = 1; /*block 1# is used by the etc directory*/
	
	strcpy(dir_buf[0].d_name,".");
	dir_buf[0].d_ino = 1;
	strcpy(dir_buf[1].d_name,"..");
	dir_buf[1].d_ino = 2;
	strcpy(dir_buf[2].d_name,"password");
	dir_buf[2].d_ino = 3;

    //disk存取之2：放入第二个dinode块
	if(memcmp(disk+DATASTART+BLOCKSIZ*1,"\x00", 1)==0){
        memcpy(disk+DATASTART+BLOCKSIZ*1, dir_buf, 3*(DIRSIZ+4));
    }else{
        //memcpy(dir_buf,disk+DATASTART+BLOCKSIZ*1,3*(DIRSIZ+4));
        if(fseek(disk,DATASTART+BLOCKSIZ*1,SEEK_DATA)!=0){
            perror("设定磁盘读取offset错误");
        }
        fread(dir_buf,3*(DIRSIZ+4),1,disk);
        rewind(disk);
    }

    iput(inode);

	inode = iget(3);  /* 3  password id */
	inode->di_number = 1;
	inode->di_mode = DEFAULTMODE | DIFILE;
	inode->di_size = BLOCKSIZ;
	inode->di_addr[0] = 2; /*block 2# is used by the password file*/

    //TODO: put password here
	for (i=5; i<PWDNUM; i++){
		passwd[i].p_uid = 0;
		passwd[i].p_gid = 0;
		strcpy(passwd[i].password, "            ");  // PWDSIZ " "
	}
    memcpy(pwd, passwd, 32*sizeof(struct pwd));

    //disk存取之3:密码，放入第三个dinode块
    if(memcmp(disk + DATASTART+BLOCKSIZ*2, "\x00", 1) == 0){
        memcpy(disk+DATASTART+BLOCKSIZ*2, passwd, BLOCKSIZ);
    }
    else{
        //memcpy(passwd, disk+DATASTART+BLOCKSIZ*2, BLOCKSIZ);
        if(fseek(disk,DATASTART+BLOCKSIZ*2,SEEK_DATA)!=0){
            perror("设定磁盘读取offset错误");
        }
        fread(dir_buf,BLOCKSIZ,1,disk);
        rewind(disk);
    }
	iput(inode);

	/*2. initialize the superblock */
    //初始化若干写好的文件系统参数
    //unsigned int block_buf[BLOCKSIZ/sizeof(int)];
    if(memcmp(disk+BLOCKSIZ, "\x00", 1) == 0){
        //若没有超级块
        printf("===init filsys superblock from void===\n");
        filsys.s_isize = DINODEBLK; //dinode的数目
        filsys.s_fsize = FILEBLK; //文件块的数目
        filsys.s_ninode = DINODEBLK * BLOCKSIZ/DINODESIZ - 4;
        //表示文件系统中可用的 inode 的个数。
        // 根据给定的 DINODEBLK（inode 区域的块数）、BLOCKSIZ（块大小）和 DINODESIZ（inode 的大小），
        // 减去 4 个已经被使用的 inode（由 main、etc、password 等占用）。

        filsys.s_nfree = FILEBLK - 3;
        for (i=0; i < NICINOD; i++){
            /* begin with 4,    0,1,2,3, is used by main,etc,password */
            filsys.s_inode[i] = 4+i;
        }
        filsys.s_pinode = 0;
        filsys.s_rinode = NICINOD + 4;
        //block_buf 数组只是这段代码中的一种临时数据结构，用于初始化文件系统的空闲块列表 @chatGPT
        block_buf[NICFREE-1] = FILEBLK+1;  /*FILEBLK+1 is a flag of end*/
        for (i=0; i<NICFREE-1; i++)
            block_buf[NICFREE-2-i] = FILEBLK-i-1;			//从最后一个数据块开始分配??????
        memcpy(disk+DATASTART+BLOCKSIZ*(FILEBLK-NICFREE), block_buf, BLOCKSIZ);

        for (i=FILEBLK-2*NICFREE+1; i>2; i-=NICFREE){
            for (j=0; j<NICFREE;j++){
                block_buf[j] = i+j;
            }
            memcpy(disk+DATASTART+BLOCKSIZ*(i-1), block_buf, BLOCKSIZ);
        }
        i += NICFREE;
        j = 1;
        for (; i>3; i--)
        {
            filsys.s_free[NICFREE-j] = i-1;
            j ++;
        }

        filsys.s_pfree = NICFREE - j+1;
        memcpy(disk+BLOCKSIZ, &filsys, sizeof(struct filsys));
    }else{
        printf("===loading filsys superblock from disk===\n");

        memcpy(disk+DATASTART+BLOCKSIZ*(FILEBLK-NICFREE), block_buf, BLOCKSIZ);

        //memcpy(&filsys,disk+BLOCKSIZ , sizeof(struct filsys));
        if(fseek(disk,BLOCKSIZ,SEEK_DATA)!=0){
            perror("设定磁盘读取offset错误");
        }
        fread(&filsys,sizeof(struct filsys),1,disk);
        rewind(disk);
    }

	return;
	
}
