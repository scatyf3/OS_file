#include <stdio.h>
#include <string.h>
#include "filesys.h"
#include <string>
#include "VariadicTable.h"


void _dir(){
	unsigned int di_mode;
	int i,j,k;          //xiao   
	struct inode *temp_inode;
    printf("��ǰ����%d���ļ�/Ŀ¼\n",dir.size);
    printf("\n CURRENT DIRECTORY :%s\n",dir.direct[0].d_name);
    VariadicTable<std::string, std::string, std::string, std::string> vt({"Name", "Access", "inode", "Category"}, 1000);

    for (int i = 0; i < DIRNUM; i++) {
        if (dir.direct[i].d_ino != DIEMPTY) {
            std::string name = dir.direct[i].d_name;
            std::string access = ""; // �洢�ļ�Ȩ���ַ���
            std::string inode = ""; // �洢 inode ��
            std::string category = ""; // �洢�ļ����

            // �����ļ�Ȩ���ַ���
            int di_mode = iget(dir.direct[i].d_ino)->di_mode & 00777;
            for (int j = 0; j < 9; j++) {
                if (di_mode % 2) {
                    access = "x" + access;
                } else {
                    access = "-" + access;
                }
                di_mode = di_mode / 2;
            }

            // ���� inode ��
            inode = "i_ino->" + std::to_string(iget(dir.direct[i].d_ino)->i_ino);

            // �����ļ������������
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
			printf("Ŀ¼%s�Ѵ��ڣ�\n", dirname); //xiao
		else
			printf("%s��һ���ļ���\n", dirname);
		iput(inode);
		return;
	}
	dirpos = iname(dirname);					//ȡ����addr�еĿ�����λ��,����Ŀ¼��д��������
	inode = ialloc();							//����i�ڵ�
	dir.direct[dirpos].d_ino = inode->i_ino;	//���ø�Ŀ¼�Ĵ���i�ڵ��
	dir.size++;									//Ŀ¼��++		
	
	strcpy(buf[0].d_name,".");					//��ǰĿ¼
	buf[0].d_ino = cur_path_inode->i_ino;
	strcpy(buf[1].d_name, "..");
	buf[1].d_ino = inode->i_ino;				//��һ��Ŀ¼

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

    //ͨ��Ŀ¼������ȡĿ¼����
	dirid = namei(dirname);
	if (dirid == -1){
		printf("������Ŀ¼%s��\n", dirname);
		return;
	}
    //�ж��Ƿ�Ϊ�ļ�
	inode =iget(dir.direct[dirid].d_ino);
	if(!(inode->di_mode&DIDIR)){
		printf("%s����һ��Ŀ¼��\n");
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



































