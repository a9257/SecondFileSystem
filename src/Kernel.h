#ifndef KERNEL_H
#define KERNEL_H

#include "User.h"
#include "BufferManager.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "DiskDriver.h"

/*
 * Kernel类用于封装所有内核相关的全局类实例对象，
 * 
 * Kernel类在内存中为单体模式，保证内核中封装各内核
 * 模块的对象都只有一个副本。
 */

class Kernel
{

public:
  Kernel();
  ~Kernel();
  static Kernel &Instance();
  void Initialize(); /* 该函数完成初始化内核大部分数据结构的初始化 */

  BufferManager &GetBufferManager();
  FileSystem &GetFileSystem();
  FileManager &GetFileManager();
  DiskDriver &GetDiskDriver();
  User &GetUser(); /* 获取当前进程的User结构 */

private:
  void InitDiskDriver();
  void InitBuffer();
  void InitFileSystem();

private:
  static Kernel instance; /* Kernel单体类实例 */

  BufferManager *m_BufferManager;
  FileSystem *m_FileSystem;
  FileManager *m_FileManager;
  DiskDriver *m_DiskDriver;
  User *m_User;
};

#endif
