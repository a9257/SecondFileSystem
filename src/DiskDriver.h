#ifndef DISK_DRIVER_H
#define DISK_DRIVER_H

#include <stdio.h>
#include "Buf.h"

class DiskDriver
{
private:
  static const char *DISK_FILE_NAME; /* 磁盘镜像文件名 */
  static const int BLOCK_SIZE = 512; /* 数据块大小为512字节 */

  FILE *fp; /* 磁盘镜像文件指针 */
public:
  DiskDriver();
  ~DiskDriver();

  void Initialize(); /* 初始化磁盘镜像 */

  /* 
     * 从磁盘镜像文件指定数据块开始读取
     * blkno为-1表明接着当前指针处继续读
     * 成功返回读取的字节数
     * 失败返回-1
     */
  //int Read(void *buffer, int size, int blkno = -1, int offset = 0);

  /* 向磁盘镜像文件写入 */
  //int Write(const void *buffer, int size, int blkno = -1, int offset = 0);

  void IO(Buf *bp); /* 根据IO请求块进行读写 */
};

#endif
