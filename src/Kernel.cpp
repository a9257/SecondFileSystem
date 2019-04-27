#include "Kernel.h"
#include <stdio.h>

Kernel Kernel::instance;

/*
 * 磁盘驱动全局变量
 */
DiskDriver g_DiskDriver;

/* 
 * 高速缓存管理全局manager 
 */
BufferManager g_BufferManager;

/*
 * 文件系统相关全局manager
 */
FileSystem g_FileSystem;
FileManager g_FileManager;

User g_User;

Kernel::Kernel()
{
}

Kernel::~Kernel()
{
}

Kernel &Kernel::Instance()
{
    return Kernel::instance;
}

void Kernel::InitDiskDriver()
{
    this->m_DiskDriver = &g_DiskDriver;

    printf("Initialize Disk Driver...");
    this->GetDiskDriver().Initialize();
    printf("Done.\n");
}

void Kernel::InitBuffer()
{
    this->m_BufferManager = &g_BufferManager;

    printf("Initialize Buffer...");
    this->GetBufferManager().Initialize();
    printf("Done.\n");
}

void Kernel::InitFileSystem()
{
    this->m_FileSystem = &g_FileSystem;
    this->m_FileManager = &g_FileManager;
    this->m_User = &g_User;

    printf("Initialize File System...");
    this->GetFileSystem().Initialize();
    printf("Done.\n");

    printf("Initialize File Manager...");
    this->GetFileManager().Initialize();
    printf("Done.\n");
}

void Kernel::Initialize()
{
    InitDiskDriver();
    InitBuffer();
    InitFileSystem();
}

BufferManager &Kernel::GetBufferManager()
{
    return *(this->m_BufferManager);
}

FileSystem &Kernel::GetFileSystem()
{
    return *(this->m_FileSystem);
}

FileManager &Kernel::GetFileManager()
{
    return *(this->m_FileManager);
}

DiskDriver &Kernel::GetDiskDriver()
{
    return *(this->m_DiskDriver);
}

User &Kernel::GetUser()
{
    return *(this->m_User);
}
