#include <stdio.h>
#include "filesys.h"

static unsigned int block_buf[BLOCKSIZ];
/********************************************************** 
函数：balloc 
功能：维护超级块中的空闲数据栈，分配空闲数据块，并返回其块号 
***********************************************************/
unsigned int balloc(){
	unsigned int free_block;
	int i;

	//如果没有空闲盘块
	if (filsys.s_nfree == 0){
		printf("\nDisk Full!!!\n");
		return DISKFULL;
	}
	free_block = filsys.s_free[filsys.s_pfree]; //取堆栈中的盘块号
	if (filsys.s_pfree == NICFREE-1){		    //如果堆栈只剩一个块
		memcpy(block_buf,disk+DATASTART+(free_block)*BLOCKSIZ,BLOCKSIZ);
		//从中读取下一组块号
		for (i=0; i<NICFREE; i++)
			filsys.s_free[i] = block_buf[i];
		filsys.s_pfree = 0;				//设置堆栈指针
	}else{//如果堆栈中大于一个盘块
		filsys.s_pfree++;				//修改堆栈指针
	}
	filsys.s_nfree --;					//修改总块数
	filsys.s_fmod = SUPDATE;
	return free_block;	
}


void bfree(unsigned int block_num){
	int i;

	if (filsys.s_pfree == 0){//如果堆栈已满
		/*将当前堆栈内块号写入当前块号*/
		for(i=0; i<NICFREE; i++)
			block_buf[i] = filsys.s_free[NICFREE-1-i];
		memcpy(disk+DATASTART+block_num*BLOCKSIZ,block_buf,BLOCKSIZ);
		filsys.s_pfree = NICFREE;//清空堆栈
	}
	/*修改堆栈指针，并将当前块号压入堆栈*/
	filsys.s_pfree --;
	filsys.s_nfree++;
	filsys.s_free[filsys.s_pfree] = block_num;
	filsys.s_fmod = SUPDATE;
	return;
}
