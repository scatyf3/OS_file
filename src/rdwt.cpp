#include <stdio.h>
#include "filesys.h"  

unsigned int read(int fd, char *buf, unsigned int size){
	unsigned long off;
	int block, block_off, i, j;
	struct inode *inode;
	char *temp_buf;

	inode = sys_ofile[user[user_id].u_ofile[fd]].f_inode;
	if (!(sys_ofile[user[user_id].u_ofile[fd]].f_flag & FREAD)){
		printf("\nthe file is not opened for read\n");
		return 0;
	}
	temp_buf = buf;
	off = sys_ofile[user[user_id].u_ofile[fd]].f_off;
	if ((off+size) > inode->di_size){
		size = inode->di_size - off;
	}
	block_off = off % BLOCKSIZ;
	block = off / BLOCKSIZ;
	if (block_off+size<BLOCKSIZ){
		memcpy(buf, disk+DATASTART+inode->di_addr[block]*BLOCKSIZ+block_off, size);
		return size;
	}
	memcpy(temp_buf, disk+DATASTART+inode->di_addr[block]*BLOCKSIZ+block_off,BLOCKSIZ-block_off);
	temp_buf += BLOCKSIZ - block_off;
	j = (inode->di_size-off-block_off) / BLOCKSIZ;
	for (i=0; i<(size-(BLOCKSIZ-block_off))/BLOCKSIZ; i++){
		memcpy(temp_buf, disk+DATASTART+inode->di_addr[j+i]*BLOCKSIZ, BLOCKSIZ);
		temp_buf += BLOCKSIZ;
	}

	block_off = (size-(BLOCKSIZ-block_off))% BLOCKSIZ;
	memcpy(temp_buf, disk+DATASTART+i*BLOCKSIZ, block_off);
	sys_ofile[user[user_id].u_ofile[fd]].f_off += size;
	return size;
}


unsigned int write(int fd, char *buf, unsigned int size){
	unsigned long off;
	int block, block_off, i, j;
	struct inode *inode;
	char *temp_buf;

	inode = sys_ofile[user[user_id].u_ofile[fd]].f_inode;
	if  (!(sys_ofile[user[user_id].u_ofile[fd]].f_flag & FWRITE)){
		printf("\nthe file is not opened for write\n");
		return 0;
	}
	//add by liwen to check the filesize and alloc the BLOCK
	off = sys_ofile[user[user_id].u_ofile[fd]].f_off;
	block = ((off+size)-inode->di_size)/BLOCKSIZ;//ÉÐÐè¸öÊý 
	if(((off+size)-inode->di_size)%BLOCKSIZ)
		block++;
	if(filsys.s_nfree < block){
		printf("Not enough space to write so much bytes!\n");
		return 0;
	}
	j = inode->di_size/BLOCKSIZ;
	if(inode->di_size % BLOCKSIZ){
		j ++;
	}
	if(j+block>NADDR){
		printf("To write so much bytes will exceed the file limit!!\n");
		return 0;
	}
	for(i=j;i<j+block;i++){
		inode->di_addr[i]=balloc();
	}
	inode->di_size += size;
	//end add
	temp_buf = buf;

	off = sys_ofile[user[user_id].u_ofile[fd]].f_off;
	block_off = off % BLOCKSIZ;
	block = off/BLOCKSIZ;

	if (block_off+size<BLOCKSIZ){
		memcpy(disk+DATASTART+inode->di_addr[block]*BLOCKSIZ+block_off, buf, size);
		return size;
	}
	memcpy(disk+DATASTART+inode->di_addr[block]*BLOCKSIZ+block_off,temp_buf,BLOCKSIZ-block_off);

	temp_buf += BLOCKSIZ-block_off;
	for (i=0; i<(size-(BLOCKSIZ-block_off))/BLOCKSIZ; i++){
		memcpy(disk+DATASTART+inode->di_addr[block+1+i]*BLOCKSIZ, temp_buf, BLOCKSIZ);
		temp_buf += BLOCKSIZ;
	}
	block_off = (size-(BLOCKSIZ-block_off)) % BLOCKSIZ;
	memcpy(disk+DATASTART+block*BLOCKSIZ, temp_buf, block_off);
	sys_ofile[user[user_id].u_ofile[fd]].f_off += size;
	return size;
}


