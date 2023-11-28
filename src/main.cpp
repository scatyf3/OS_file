#include <stdio.h>
#include "filesys.h" 

struct hinode 	hinode[NHINO];

struct dir 		dir;
struct file 	sys_ofile[SYSOPENFILE];
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




