# SecondFileSystem, OS project

CC = g++
CFLAGS = -w

BIN = SecondFS.exe

$(BIN): main.o BufferManager.o DiskDriver.o File.o FileManager.o FileSystem.o INode.o OpenFileManager.o Utility.o Kernel.o
	$(CC) -o $@ $^
main.o: main.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
BufferManager.o: BufferManager.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
DiskDriver.o: DiskDriver.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
File.o: File.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
FileManager.o: FileManager.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
FileSystem.o: FileSystem.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
INode.o: INode.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
OpenFileManager.o: OpenFileManager.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
Utility.o: Utility.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
Kernel.o: Kernel.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
	
.PHONY:clean
clean:
	-del *.o $(BIN)
	-del *.img