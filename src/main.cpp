#include <stdio.h>
#include <string.h>
#include "Kernel.h"

void InitSystem()
{
	printf("Initialize System...");
	FileManager *fileManager = &Kernel::Instance().GetFileManager();
	fileManager->rootDirInode = g_InodeTable.IGet(FileSystem::ROOTINO);
	Kernel::Instance().GetFileSystem().LoadSuperBlock();
	User *u = &Kernel::Instance().GetUser();
	u->u_error = User::NOERROR;
	u->u_cdir = g_InodeTable.IGet(FileSystem::ROOTINO);
	u->u_pdir = NULL;
	strcpy(u->u_curdir, "/");
	u->u_dirp = "/";
	memset(u->u_arg, 0, sizeof(u->u_arg));
	printf("Done.\n");
}

void ExitSystem()
{
	BufferManager *bufferManager = &Kernel::Instance().GetBufferManager();
	bufferManager->Bflush();
	InodeTable *inodeTable = Kernel::Instance().GetFileManager().m_InodeTable;
	inodeTable->UpdateInodeTable();
}

int Fcreat(char *name, int mode)
{
	User *u = &Kernel::Instance().GetUser();
	u->u_ar0 = 0;
	u->u_dirp = name;
	u->u_arg[1] = mode;
	FileManager *fileManager = &Kernel::Instance().GetFileManager();
	fileManager->Creat();
	return u->u_ar0;
}

int Fopen(char *name, int mode)
{
	User *u = &Kernel::Instance().GetUser();
	u->u_ar0 = 0;
	u->u_dirp = name;
	u->u_arg[1] = mode;
	FileManager *fileManager = &Kernel::Instance().GetFileManager();
	fileManager->Open();
	return u->u_ar0;
}

int Fread(int fd, char *buffer, int length)
{
	User *u = &Kernel::Instance().GetUser();
	u->u_ar0 = 0;
	u->u_arg[0] = fd;
	u->u_arg[1] = int(buffer);
	u->u_arg[2] = length;
	FileManager *fileManager = &Kernel::Instance().GetFileManager();
	fileManager->Read();
	return u->u_ar0;
}

int Fwrite(int fd, char *buffer, int length)
{

	User *u = &Kernel::Instance().GetUser();
	u->u_ar0 = 0;
	u->u_arg[0] = fd;
	u->u_arg[1] = int(buffer);
	u->u_arg[2] = length;
	FileManager *fileManager = &Kernel::Instance().GetFileManager();
	fileManager->Write();
	return u->u_ar0;
}

void Fdelete(char *name)
{
	User *u = &Kernel::Instance().GetUser();
	u->u_ar0 = 0;
	u->u_dirp = name;
	FileManager *fileManager = &Kernel::Instance().GetFileManager();
	fileManager->UnLink();
}

int Flseek(int fd, int position, int whence)
{
	User *u = &Kernel::Instance().GetUser();
	u->u_ar0 = 0;
	u->u_arg[0] = fd;
	u->u_arg[1] = position;
	u->u_arg[2] = whence;
	FileManager *fileManager = &Kernel::Instance().GetFileManager();
	fileManager->Seek();
	return u->u_ar0;
}

void Fclose(int fd)
{
	User *u = &Kernel::Instance().GetUser();
	u->u_ar0 = 0;
	u->u_arg[0] = fd;
	FileManager *fileManager = &Kernel::Instance().GetFileManager();
	fileManager->Close();
}

void Ls()
{
	User *u = &Kernel::Instance().GetUser();
	u->u_ar0 = 0;
	int fd = Fopen(u->u_curdir, File::FREAD);
	char buffer[32] = {0};
	while (1)
	{
		if (Fread(fd, buffer, 32) == 0)
			break;
		else
		{
			DirectoryEntry *de = (DirectoryEntry *)buffer;
			if (de->m_ino == 0)
				continue;
			printf("%s\n", de->m_name);
			memset(buffer, 0, 32);
		}
	}
	Fclose(fd);
}

void Mkdir(char *name)
{
	User *u = &Kernel::Instance().GetUser();
	u->u_ar0 = 0;
	int defaultmode = 040755; //default newmode:- rwx r-w r-w
	u->u_dirp = name;
	u->u_arg[1] = defaultmode;
	u->u_arg[2] = 0;
	FileManager *fileManager = &Kernel::Instance().GetFileManager();
	fileManager->MkNod();
}

void Cd(char *name)
{
	User *u = &Kernel::Instance().GetUser();
	u->u_ar0 = 0;
	u->u_dirp = name;
	u->u_arg[0] = int(name);
	//printf("%s\n", (char *)u->u_arg[0]);
	FileManager *fileManager = &Kernel::Instance().GetFileManager();
	fileManager->ChDir();
}

void Fin(char *extername, char *intername)
{
	int defaultmode = 0777;
	int fd = Fcreat(intername, defaultmode);
	//printf("fd:%d\n", fd);
	char buffer[1024] = {0};
	FILE *fp = fopen(extername, "rb");
	int rbytes = fread(buffer, 1, 1024, fp);
	while (rbytes > 0)
	{
		//printf("%s", buffer);
		//printf("%d ", rbytes);
		int wbytes = Fwrite(fd, buffer, rbytes);
		//printf("%d\n", wbytes);
		rbytes = fread(buffer, 1, 1024, fp);
	}
	fclose(fp);
	Fclose(fd);
}

void Fout(char *intername, char *extername)
{
	int defaultmode = File::FREAD | File::FWRITE;
	int fd = Fopen(intername, defaultmode);
	char buffer[1024] = {0};
	FILE *fp = fopen(extername, "wb");
	int rbytes = Fread(fd, buffer, 1024);
	while (rbytes > 0)
	{
		//printf("%s", buffer);
		//printf("%d\n", rbytes);
		fwrite(buffer, 1, rbytes, fp);
		rbytes = Fread(fd, buffer, 1024);
	}
	Fclose(fd);
	fclose(fp);
}

void Help()
{
	printf("1.ls\n\tUsage:ls\n\tDescription:显示当前目录列表\n");
	printf("2.fopen\n\tUsage:fopen [name]\n\tDescription:打开名为name的文件,返回fd\n");
	printf("3.fclose\n\tUsage:fclose [fd]\n\tDescription:关闭文件描述符为fd的文件\n");
	printf("4.fread\n\tUsage:fread [fd] [length]\n\tDescription:从文件描述符为fd的文件读取length个字节，输出读取的内容\n");
	printf("5.fwrite\n\tUsage:fwrite [fd] [string] [length]\n\tDescription:向文件描述符为fd的文件写入内容为string的length个字节(不足截断,超过补0)\n");
	printf("6.flseek\n\tUsage:flseek [fd] [offset]\n\tDescription:将文件描述符为fd的文件的读写指针调整到据文件开头偏移量为offset的位置\n");
	printf("7.fcreat\n\tUsage:fcreat [name]\n\tDescription:创建名为name的普通文件\n");
	printf("8.mkdir\n\tUsage:mkdir [name]\n\tDescription:创建名为name的目录文件\n");
	printf("9.fdelete\n\tUsage:fdelete [name]\n\tDescription:删除名为name的文件\n");
	printf("10.cd\n\tUsage:cd [name]\n\tDescription:改变工作目录为name\n");
	printf("11.fin\n\tUsage:fin [extername] [intername]\n\tDescription:将外部名为extername的文件内容存入内部名为intername的文件\n");
	printf("12.fout\n\tUsage:fout [intername] [extername]\n\tDescription:将内部名为intername的文件内容存入外部名为extername的文件\n");
	printf("13.exit\n\tUsage:exit\n\tDescription:退出系统\n");
}

int main()
{
	/* 系统初始化 */
	Kernel::Instance().Initialize();
	InitSystem();

	User *u = &Kernel::Instance().GetUser();

	printf("\nWelcome to SecondFileSystem! Input \"help\" to get information.\n");
	while (1)
	{
		printf("[root@SecondFS %s]# ", u->u_curdir);
		u->u_error = User::NOERROR;

		char cmd[200];
		memset(cmd, 0, sizeof(cmd));
		scanf("%s", cmd);

		if (strncmp(cmd, "exit", 4) == 0)
		{
			ExitSystem();
			break;
		}
		else if (strncmp(cmd, "mkdir", 5) == 0)
		{
			char name[50];
			scanf("%s", name);
			Mkdir(name);
		}
		else if (strncmp(cmd, "ls", 2) == 0)
		{
			Ls();
		}
		else if (strncmp(cmd, "cd", 2) == 0)
		{
			char name[50];
			scanf("%s", name);
			Cd(name);
		}
		else if (strncmp(cmd, "fcreat", 6) == 0)
		{
			char name[50];
			int defaultmode = 0777;
			scanf("%s", name);
			printf("fcreat return fd:%d\n", Fcreat(name, defaultmode));
		}
		else if (strncmp(cmd, "fopen", 5) == 0)
		{
			char name[50];
			int defaultmode = File::FREAD | File::FWRITE;
			scanf("%s", name);
			int ret = Fopen(name, defaultmode);
			if (ret < 0)
				printf("fopen error\n");
			else
				printf("fopen return fd:%d\n", ret);
		}
		else if (strncmp(cmd, "fdelete", 7) == 0)
		{
			char name[50];
			scanf("%s", name);
			Fdelete(name);
		}
		else if (strncmp(cmd, "fwrite", 6) == 0)
		{
			int fd;
			char buffer[1024] = {0};
			int length;
			scanf("%d", &fd);
			scanf("%s", buffer);
			scanf("%d", &length);
			printf("fwrite return %d\n", Fwrite(fd, buffer, length));
		}
		else if (strncmp(cmd, "fread", 5) == 0)
		{
			int fd;
			char buffer[1024] = {0};
			int length;
			scanf("%d", &fd);
			scanf("%d", &length);
			Fread(fd, buffer, length);
			printf("fread:%s\n", buffer);
		}
		else if (strncmp(cmd, "fclose", 6) == 0)
		{
			int fd;
			scanf("%d", &fd);
			Fclose(fd);
		}
		else if (strncmp(cmd, "flseek", 6) == 0)
		{
			int fd;
			int position;
			int whence = 0;
			scanf("%d", &fd);
			scanf("%d", &position);
			//scanf("%d", &whence);
			Flseek(fd, position, whence);
		}
		else if (strncmp(cmd, "fin", 3) == 0)
		{
			char intername[50];
			char extername[50];
			scanf("%s", extername);
			scanf("%s", intername);
			Fin(extername, intername);
		}
		else if (strncmp(cmd, "fout", 4) == 0)
		{
			char intername[50];
			char extername[50];
			scanf("%s", intername);
			scanf("%s", extername);
			Fout(intername, extername);
		}
		else if (strncmp(cmd, "help", 4) == 0)
		{
			Help();
		}
	}

	return 0;
}
