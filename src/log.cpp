#include <stdio.h> 
#include <string.h>
#include "filesys.h" 

int login(unsigned short uid, char *passwd){
	int i,j;

	for (i=0; i<PWDNUM; i++){
		if ((uid == pwd[i].p_uid) && (!strcmp(passwd, pwd[i].password))){
			for (j=0; j<USERNUM; j++){
				if (user[j].u_uid == 0) 
					break;
			}
 
			if (j == USERNUM){
				printf("\ntoo much user in the systemm, waited to login\n");
				return -1;
			}else{
				user[j].u_uid = uid;
				user[j].u_gid = pwd[i].p_gid; 
				user[j].u_default_mode = DEFAULTMODE;
			}
			break;
		}//if
	}//for
	if (i == PWDNUM){
		printf("\nincorrect password\n");
		return -1;
	}else{
        //printf("您的user_id是%d",j);
		return j;
	}
}

int logout(unsigned short uid){
	int i,j,sys_no;
	struct inode *inode;

	for (i=0; i<USERNUM; i++){
		if (uid == user[i].u_uid)
			break;
	}

	if (i == USERNUM){
		printf("\nno such a file\n");
		return 0;
	}

	for (j=0; j<NOFILE; j++){
		if (user[i].u_ofile[j] != SYSOPENFILE+1){
			/* iput the inode free the sys_ofile and clear the user_ofile */
			sys_no = user[i].u_ofile[j];
			inode = sys_ofile[sys_no].f_inode;
			iput(inode);
			sys_ofile[sys_no].f_count--;
			user[i].u_ofile[j] = SYSOPENFILE+1;
		}
	}
	return 1;
}

void chpwd(char* new_pwd){
    unsigned short 	u_uid = user[user_id].u_uid;
    for(int i = 0;i<PWDNUM;i++){
        if(pwd[i].p_uid == u_uid){
            strcpy(pwd[i].password,new_pwd);
        }
    }
    return;

}

































