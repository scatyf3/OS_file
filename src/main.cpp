#include <stdio.h>
#include "filesys.h" 

//global vars
struct hinode 	hinode[NHINO];//HASG表，增加文件访问速度？

struct dir 		dir;
struct file 	sys_ofile[SYSOPENFILE];//打开的文件列表->写入内存增加访问速度
struct filsys 	filsys;
struct pwd 		pwd[PWDNUM];
struct user 	user[USERNUM];
struct inode* 	cur_path_inode;
int 			user_id;
char   			disk[(DINODEBLK+FILEBLK+2)*BLOCKSIZ];
char 			str[100];


int main(){ 
	int username;
	char password[16];
	user_id = -1;
    format();
	install();
	printf("Welcome to mini filesystem!\n");
	while(user_id == -1){
		printf("Login:");
		scanf("%d",&username);
		printf("Password:");
		scanf("%s",password);
		user_id = login(username,password);
	}
	do{
		printf("> ");
		fflush(stdin);
		fgets(str, sizeof(str), stdin);
	}while(shell(user_id,str));
	logout(2118);
	halt();
	return 0;
}




