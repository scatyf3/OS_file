#include "filesys.h" 

/**********************************************************************************
函数：access
参数：用户ID号；内存节点；要判断的权限 
功能：通过用户权限判断用户是否能对该文件进行相应的操作；权限够返回1，权限不够返回0。
***********************************************************************************/
unsigned int access(unsigned int user_id,  struct inode *inode,  unsigned short mode){
	switch (mode){
		case READ:
			if (inode->di_mode & ODIREAD)												//文件是所有用户可读行通过；
				return 1;
			if ((inode->di_mode & GDIREAD) && (user[user_id].u_gid == inode->di_gid))	//文件本组可读，用户组和文件组一致通过；
				return 1;
			if ((inode->di_mode & UDIREAD) && (user[user_id].u_uid == inode->di_uid))	//文件指定用户可读，是该用户通过；
				return 1;
			return 0;				//其他不通过；
		case WRITE:
			if (inode->di_mode & ODIWRITE)
				return 1;
			if ((inode->di_mode & GDIWRITE) && (user[user_id].u_gid == inode->di_gid))
				return 1;
			if ((inode->di_mode & UDIWRITE) && (user[user_id].u_uid == inode->di_uid))
				return 1;
			return 0;
		case EXICUTE:
			if (inode->di_mode & ODIEXICUTE)
				return 1;
			if ((inode->di_mode & GDIEXICUTE) && (user[user_id].u_gid == inode->di_gid))
				return 1;
			if ((inode->di_mode & UDIEXICUTE) && (user[user_id].u_uid == inode->di_uid))
				return 1;
			return 0;		
	}//swith
	return 0;
}




































