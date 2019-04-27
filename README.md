# SecondFileSystem
Second file system based on Unix V6++ for OS course
## 环境
- 操作系统：win10 64bit
- 编译器：gcc version 3.4.5 (mingw-vista special r3)
## 使用说明
1. 命令行下make完成编译
2. 打开可执行文件，输入help获取命令使用帮助
3. 指令说明
```
1.ls
        Usage:ls
        Description:显示当前目录列表
2.fopen
        Usage:fopen [name]
        Description:打开名为name的文件,返回fd
3.fclose
        Usage:fclose [fd]
        Description:关闭文件描述符为fd的文件
4.fread
        Usage:fread [fd] [length]
        Description:从文件描述符为fd的文件读取length个字节，输出读取的内容
5.fwrite
        Usage:fwrite [fd] [string] [length]
        Description:向文件描述符为fd的文件写入内容为string的length个字节(不足截断,超过补0)
6.flseek
        Usage:flseek [fd] [offset]
        Description:将文件描述符为fd的文件的读写指针调整到据文件开头偏移量为offset的位置
7.fcreat
        Usage:fcreat [name]
        Description:创建名为name的普通文件
8.mkdir
        Usage:mkdir [name]
        Description:创建名为name的目录文件
9.fdelete
        Usage:fdelete [name]
        Description:删除名为name的文件
10.cd
        Usage:cd [name]
        Description:改变工作目录为name
11.fin
        Usage:fin [extername] [intername]
        Description:将外部名为extername的文件内容存入内部名为intername的文件
12.fout
        Usage:fout [intername] [extername]
        Description:将内部名为intername的文件内容存入外部名为extername的文件
13.exit
        Usage:exit
        Description:退出系统
```
4. exit退出系统
5. 命令行下make clean清除编译生成文件
## 示例
初始界面
![example](/example/1.jpg)
