#include <stdio.h>
#include "filesys.h" 

void delete(char *filename){
	unsigned int dirid;
	struct inode *inode;

	dirid = namei(filename);
	if (dirid == -1){
		printf("文件不存在，请检查!\n"); 
		return;
	}
	inode = iget(dir.direct[dirid].d_ino);
	if(!(inode->di_mode &DIFILE)){
		printf("对象不是文件，请检查！\n");
		iput(inode);
		return;
	}
	dir.direct[dirid].d_ino = DIEMPTY;
	dir.size --;
	inode->di_number--;
	iput(inode);
	return;
}











