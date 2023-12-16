#include <stdio.h>
#include <string.h>
#include "filesys.h" 

void format(){
	struct inode *inode;
	struct direct dir_buf[BLOCKSIZ/(DIRSIZ+4)];
	struct pwd passwd[32];
	unsigned int block_buf[BLOCKSIZ/sizeof(int)];
	int i,j;

	//��ʼ��Ӳ��
    FILE* file = fopen("../data/data.bin", "rb"); // �Զ����ƶ�ȡģʽ���ļ�
    if(file==NULL){
        memset(disk, 0x00, ((DINODEBLK+FILEBLK+2)*BLOCKSIZ));
    }
    else{
        size_t count = fread(disk, sizeof(char), (DINODEBLK + FILEBLK + 2) * BLOCKSIZ, file);
        printf("���ǴӴ������ȡ��%lu����\n",count);
    }


	/* 0.initialize the passwd */
    /*pwd ��������ˣ�ֱ��Ӳ�����ڴ������*/
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

    //��ʼ��ǰ����Ŀ¼��ֱ��ǿգ�.��..
	inode = iget(0);   /* 0 empty dinode id*/
	inode->di_number = 1;			//??��i�ڵ��к���????->�Ҳ�����
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

    //��disk�ж�ȡ
    if(memcmp(disk + DATASTART, "\x00", 1) == 0){
        //��diskΪ�գ���format���ʼ���ı�����������disk��
        memcpy(disk+DATASTART, &dir_buf, 3*(DIRSIZ+4));
    }else{
        //��disk��Ϊ�գ���disk������һ�δ���õ�ֵ
        memcpy(&dir_buf, disk+DATASTART, 3*(DIRSIZ+4));
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

	memcpy(disk+DATASTART+BLOCKSIZ*1, dir_buf, 3*(DIRSIZ+4));
	iput(inode);

	inode = iget(3);  /* 3  password id */
	inode->di_number = 1;
	inode->di_mode = DEFAULTMODE | DIFILE;
	inode->di_size = BLOCKSIZ;
	inode->di_addr[0] = 2; /*block 2# is used by the password file*/

	for (i=5; i<PWDNUM; i++){
		passwd[i].p_uid = 0;
		passwd[i].p_gid = 0;
		strcpy(passwd[i].password, "            ");  // PWDSIZ " "
	}
    memcpy(pwd, passwd, 32*sizeof(struct pwd));

    if(memcmp(disk + DATASTART+BLOCKSIZ*2, "\x00", 1) == 0){
        memcpy(disk+DATASTART+BLOCKSIZ*2, passwd, BLOCKSIZ);
    }
    else{
        memcpy(passwd, disk+DATASTART+BLOCKSIZ*2, BLOCKSIZ);
    }
	iput(inode);

	/*2. initialize the superblock */
    //��ʼ������д�õ��ļ�ϵͳ����
    if(memcmp(disk+BLOCKSIZ, "\x00", 1) == 0){
        //��û�г�����
        printf("===init filsys superblock===\n");
        filsys.s_isize = DINODEBLK;
        filsys.s_fsize = FILEBLK;
        filsys.s_ninode = DINODEBLK * BLOCKSIZ/DINODESIZ - 4;
        filsys.s_nfree = FILEBLK - 3;
        for (i=0; i < NICINOD; i++){
            /* begin with 4,    0,1,2,3, is used by main,etc,password */
            filsys.s_inode[i] = 4+i;
        }
        filsys.s_pinode = 0;
        filsys.s_rinode = NICINOD + 4;
        block_buf[NICFREE-1] = FILEBLK+1;  /*FILEBLK+1 is a flag of end*/
        for (i=0; i<NICFREE-1; i++)
            block_buf[NICFREE-2-i] = FILEBLK-i-1;			//�����һ�����ݿ鿪ʼ����??????

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
        memcpy(&filsys,disk+BLOCKSIZ , sizeof(struct filsys));
    }

	return;
	
}
