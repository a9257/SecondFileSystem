#include <time.h>
#include <string.h>
#include "DiskDriver.h"
#include "FileSystem.h"
#include "INode.h"

const char *DiskDriver::DISK_FILE_NAME = "myDisk.img";

DiskDriver::DiskDriver()
{
	/*
	 * 尝试打开磁盘镜像文件
	 * 若不存在文件返回NULL，则需创建并格式化
	 * 存在则直接使用
	 */
	fp = fopen(DISK_FILE_NAME, "rb+");
}
DiskDriver::~DiskDriver()
{
	if (fp)
		fclose(fp);
}
void DiskDriver::Initialize()
{
	if (!fp)
	{
		/* 创建磁盘镜像文件 */
		fp = fopen(DISK_FILE_NAME, "wb+");
	}
	else
	{
		//printf("don't create disk.\n");
		return;
	}
	if (!fp)
		printf("Failed.\n");
	else
	{
		// 初始化相关数据结构、分配存储空间
		// 初始化超级块
		SuperBlock spb;
		spb.s_isize = FileSystem::INODE_ZONE_SIZE;
		spb.s_fsize = FileSystem::DATA_ZONE_END_SECTOR + 1;

		spb.s_ninode = 100;
		for (int i = 1; i <= spb.s_ninode; i++)
			spb.s_inode[i] = i;

		//spb.s_nfree = (FileSystem::DATA_ZONE_SIZE - 99) % 100;
		//for (int i = spb.s_nfree - 1; i >= 0; i--)
		//spb.s_free[i] = FileSystem::DATA_ZONE_END_SECTOR - (spb.s_nfree - 1 - i);

		int s_flock = 0;
		int s_ilock = 0;
		spb.s_fmod = 0;
		spb.s_ronly = 0;
		spb.s_time = time(0);
		fwrite(&spb, sizeof(spb), 1, fp);

		// 初始化inode区
		for (int i = 0; i < FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR; i++)
		{
			DiskInode dinode;
			if (i == FileSystem::ROOTINO)
			{
				dinode.d_mode |= Inode::IFDIR | Inode::IALLOC | Inode::IEXEC;
				dinode.d_nlink = 1;
			}
			fwrite(&dinode, sizeof(dinode), 1, fp);
		}

		// 初始化数据区
		unsigned char hBlock[BLOCK_SIZE] = {0};
		unsigned char fBlock[BLOCK_SIZE] = {0};

		spb.s_nfree = 99;
		spb.s_free[0] = 0;
		for (int i = 0; i <= 98; i++)
		{
			spb.s_free[i + 1] = FileSystem::DATA_ZONE_START_SECTOR + i;
			fwrite(fBlock, BLOCK_SIZE, 1, fp);
		}
		memcpy(hBlock, &(spb.s_nfree), sizeof(int) * 101);
		fwrite(hBlock, BLOCK_SIZE, 1, fp);
		spb.s_nfree = 1;
		spb.s_free[0] = FileSystem::DATA_ZONE_START_SECTOR + 99;

		for (int i = 100; i < FileSystem::DATA_ZONE_SIZE; i++)
		{
			if (spb.s_nfree >= 100)
			{
				memcpy(hBlock, &(spb.s_nfree), sizeof(int) * 101);
				fwrite(hBlock, BLOCK_SIZE, 1, fp);
				spb.s_nfree = 0;
			}
			else
				fwrite(fBlock, BLOCK_SIZE, 1, fp);
			spb.s_free[spb.s_nfree++] = FileSystem::DATA_ZONE_START_SECTOR + i;
		}

		// 重新写入超级块
		fseek(fp, 0, SEEK_SET);
		fwrite(&spb, sizeof(spb), 1, fp);

		//for (int i = 0; i < 10; i++)
		//	printf("%d:%d", i, spb.s_free[i]);
	}
}

/*int DiskDriver::Read(void *buffer, int size, int blkno, int offset)
{
	// 当blkno>=0移动文件读写指针
	if (blkno >= 0)
		fseek(fp, blkno * BLOCK_SIZE + offset, SEEK_SET);
	int n = fread(buffer, size, 1, fp);
	if (n >= 0)
		return n * size;
	else
		return -1;
}

int DiskDriver::Write(const void *buffer, int size, int blkno, int offset)
{
	if (blkno >= 0)
		fseek(fp, blkno * BLOCK_SIZE + offset, SEEK_SET);
	int n = fwrite(buffer, size, 1, fp);
	if (n >= 0)
		return n * size;
	else
		return -1;
}*/

void DiskDriver::IO(Buf *bp)
{

	/* 如果是读操作 */
	if (bp->b_flags & Buf::B_READ)
	{
		fseek(fp, bp->b_blkno * BLOCK_SIZE, SEEK_SET);
		fread(bp->b_addr, bp->b_wcount, 1, fp);
		bp->b_flags &= ~(Buf::B_READ);
		/* 置上I/O完成标志 */
		bp->b_flags |= Buf::B_DONE;
		//printf("read from no.%d block\n", bp->b_blkno);
	}
	/* 如果是写操作 */
	else
	{
		fseek(fp, bp->b_blkno * BLOCK_SIZE, SEEK_SET);
		fwrite(bp->b_addr, bp->b_wcount, 1, fp);
		bp->b_flags &= ~(Buf::B_WRITE);
		/* 置上I/O完成标志 */
		bp->b_flags |= Buf::B_DONE;
		//printf("write to no.%d block\n", bp->b_blkno);
	}
}
