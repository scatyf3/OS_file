#include <stdio.h>
#include "filesys.h"

/****************************************************** 
函数：close
功能：关闭打开文件时，修改用户打开表和系统打开表中的纪录信息 
*******************************************************/
void close(unsigned int user_id, unsigned short cfd){
	struct inode *inode;
	inode = sys_ofile[user[user_id].u_ofile[cfd]].f_inode;
	iput(inode);
	sys_ofile[user[user_id].u_ofile[cfd]].f_count -- ;
	user[user_id].u_ofile[cfd] = SYSOPENFILE+1;
	return;
}
