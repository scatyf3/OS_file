#include <stdio.h>
#include <string.h>
#include "filesys.h" 

void install(){
	int i,j;

	/* 1. read the filsys from the superblock*/   //xiao 
	memcpy(&filsys,disk+BLOCKSIZ,sizeof(struct filsys));

	/* 2. initialize the inode hash chain*/
	for (i=0; i<NHINO; i++){
		hinode[i].i_forw = NULL;
	}

	/*3. initialize the sys_ofile*/
	for (i=0; i<SYSOPENFILE; i++){
		sys_ofile[i].f_count = 0;
		sys_ofile[i].f_inode = NULL;
	}

	/*4. initialize the user*/
	for (i=0; i<USERNUM; i++){
		user[i].u_uid = 0;
		user[i].u_gid = 0;
		for (j=0; j<NOFILE; j++)
			user[i].u_ofile[j] = SYSOPENFILE+1;
	}

	/*5. read the main directory to initialize the dir*/
	cur_path_inode = iget(1);
	dir.size = cur_path_inode->di_size/(DIRSIZ+4);//xiao 2-->4

	for (i=0; i<DIRNUM; i++){
		strcpy(dir.direct[i].d_name,"             ");
		dir.direct[i].d_ino = 0;
	}
 
	for (i=0; i<dir.size/(BLOCKSIZ/(DIRSIZ+4)); i++){ 
		memcpy(&dir.direct[(BLOCKSIZ/(DIRSIZ+4))*i], 
		disk+DATASTART+BLOCKSIZ*cur_path_inode->di_addr[i], DINODESIZ);
	}

	memcpy(&dir.direct[(BLOCKSIZ)/(DIRSIZ+4)*i], 
	disk+DATASTART+BLOCKSIZ*cur_path_inode->di_addr[i], DINODESIZ);
	return;	
}

