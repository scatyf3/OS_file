#include <stdio.h>
#include <string.h>
#include "filesys.h"
#include <string>
#include "VariadicTable.h"


void _dir(){
	unsigned int di_mode;
	int i,j,k;          //xiao   
	struct inode *temp_inode;
    printf("当前共有%d个文件/目录\n",dir.size);
    printf("\n CURRENT DIRECTORY :%s\n",dir.direct[0].d_name);
    VariadicTable<std::string, std::string, std::string, std::string> vt({"Name", "Access", "inode", "Category"}, 1000);

    for (int i = 0; i < DIRNUM; i++) {
        if (dir.direct[i].d_ino != DIEMPTY) {
            std::string name = dir.direct[i].d_name;
            std::string access = ""; // 存储文件权限字符串
            std::string inode = ""; // 存储 inode 号
            std::string category = ""; // 存储文件类别

            // 计算文件权限字符串
            int di_mode = iget(dir.direct[i].d_ino)->di_mode & 00777;
            for (int j = 0; j < 9; j++) {
                if (di_mode % 2) {
                    access = "x" + access;
                } else {
                    access = "-" + access;
                }
                di_mode = di_mode / 2;
            }

            // 设置 inode 号
            inode = "i_ino->" + std::to_string(iget(dir.direct[i].d_ino)->i_ino);

            // 根据文件类型设置类别
            if (iget(dir.direct[i].d_ino)->di_mode & DIFILE) {
                category = "<file>";
            } else {
                category = "<dir>";
            }

            vt.addRow(name, access, inode, category);
        }
    }
    vt.print(std::cout);
}

void mkdir(char *dirname){
	int dirid, dirpos;
	struct inode *inode;
	struct direct buf[BLOCKSIZ/(DIRSIZ+4)];
	unsigned int block;

	dirid= namei(dirname);
	if (dirid != -1){
		inode = iget(dirid);
		if (inode->di_mode & DIDIR)
			printf("目录%s已存在！\n", dirname); //xiao
		else
			printf("%s是一个文件！\n", dirname);
		iput(inode);
		return;
	}
	dirpos = iname(dirname);					//取得在addr中的空闲项位置,并将目录名写到此项里
	inode = ialloc();							//分配i节点
	dir.direct[dirpos].d_ino = inode->i_ino;	//设置该目录的磁盘i节点号
	dir.size++;									//目录数++		
	
	strcpy(buf[0].d_name,".");					//当前目录
	buf[0].d_ino = cur_path_inode->i_ino;
	strcpy(buf[1].d_name, "..");
	buf[1].d_ino = inode->i_ino;				//上一级目录

	block = balloc();
	memcpy(disk+DATASTART+block*BLOCKSIZ, buf, BLOCKSIZ);

	inode->di_size = 2*(DIRSIZ+4);
	inode->di_number = 1; 
	inode->di_mode = user[user_id].u_default_mode|DIDIR;
	inode->di_uid = user[user_id].u_uid;
	inode->di_gid = user[user_id].u_gid;
	inode->di_addr[0] = block;

	iput(inode);
	return;
 }


void chdir(char *dirname){
	unsigned int dirid;
	struct inode *inode;
	unsigned short block;
	int i,j,low=0, high=0;

    //通过目录名，获取目录索引
	dirid = namei(dirname);
	if (dirid == -1){
		printf("不存在目录%s！\n", dirname);
		return;
	}
    //判断是否为文件
	inode =iget(dir.direct[dirid].d_ino);
	if(!(inode->di_mode&DIDIR)){
		printf("%s不是一个目录！\n");
		return;
	}
	for (i=0; i<dir.size; i++){
		if(dir.direct[i].d_ino == 0){
			for(j=DIRNUM-1;j>=0&&dir.direct[j].d_ino == 0;j--);
			memcpy(&dir.direct[i], &dir.direct[j], DIRSIZ+4);  //xiao
			dir.direct[j].d_ino = 0;
		}		
	}
	j = cur_path_inode->di_size%BLOCKSIZ?1:0;
	for (i=0; i<cur_path_inode->di_size/BLOCKSIZ+j; i++){
		bfree(cur_path_inode->di_addr[i]);
	}
	for (i=0; i<dir.size; i+=BLOCKSIZ/(DIRSIZ+4)){
		block = balloc();
		cur_path_inode->di_addr[i] = block;
		memcpy(disk+DATASTART+block*BLOCKSIZ, &dir.direct[i], BLOCKSIZ);
	}
	cur_path_inode->di_size = dir.size*(DIRSIZ+4);
	iput(cur_path_inode);
	cur_path_inode = inode;

	j=0;
	for (i=0; i<inode->di_size/BLOCKSIZ+1; i++){
		memcpy(&dir.direct[j],disk+DATASTART+inode->di_addr[i]*BLOCKSIZ, BLOCKSIZ);
		j+=BLOCKSIZ/(DIRSIZ+4);
	}
	dir.size = cur_path_inode->di_size/(DIRSIZ+4);
	for (i=dir.size; i<DIRNUM; i++){ 
		dir.direct[i].d_ino = 0;
	}
	
	//end by xiao

	return;  
}



































