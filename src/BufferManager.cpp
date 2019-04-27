#include "BufferManager.h"
#include "Kernel.h"

BufferManager::BufferManager()
{
}
BufferManager::~BufferManager()
{
}

void BufferManager::Initialize()
{
    int i;
    Buf *bp;

    this->bFreeList.b_forw = this->bFreeList.b_back = &(this->bFreeList);
    //this->bFreeList.av_forw = this->bFreeList.av_back = &(this->bFreeList);

    for (i = 0; i < NBUF; i++)
    {
        bp = &(this->m_Buf[i]);
        //bp->b_dev = -1;
        bp->b_addr = this->Buffer[i];
        /* 初始化NODEV队列 */
        bp->b_back = &(this->bFreeList);
        bp->b_forw = this->bFreeList.b_forw;
        this->bFreeList.b_forw->b_back = bp;
        this->bFreeList.b_forw = bp;
        /* 初始化自由队列 */
        bp->b_flags = Buf::B_BUSY;
        Brelse(bp);
    }
    this->m_DiskDriver = &Kernel::Instance().GetDiskDriver();
    return;
}

Buf *BufferManager::GetBlk(int blkno)
{
    Buf *dp = &(this->bFreeList); //此处将NODEV作为磁盘设备队列
    Buf *bp;

    /* 去除设备相关操作 */
    for (bp = dp->b_forw; bp != dp; bp = bp->b_forw)
    {
        /* 不是要找的缓存，则继续 */
        if (bp->b_blkno != blkno)
            continue;
        /* 从自由队列中抽取出来 */
        //this->NotAvail(bp);
        bp->b_flags |= Buf::B_BUSY;
        return bp;
    }

    /* 取自由队列第一个空闲块 */
    bp = this->bFreeList.b_forw;
    //this->NotAvail(bp);

    /* 如果该字符块是延迟写，将其异步写到磁盘上 */
    if (bp->b_flags & Buf::B_DELWRI)
    {
        //bp->b_flags |= Buf::B_ASYNC;
        this->Bwrite(bp);
    }
    /* 注意: 这里清除了所有其他位，只设了B_BUSY */
    bp->b_flags = Buf::B_BUSY;

    // 头变尾
    bp->b_back->b_forw = bp->b_forw;
    bp->b_forw->b_back = bp->b_back;

    bp->b_back = this->bFreeList.b_back;
    this->bFreeList.b_back->b_forw = bp;
    bp->b_forw = &this->bFreeList;
    this->bFreeList.b_back = bp;

    bp->b_blkno = blkno;
    return bp;
}

void BufferManager::Brelse(Buf *bp)
{
    /* 注意以下操作并没有清除B_DELWRI、B_WRITE、B_READ、B_DONE标志
	 * B_DELWRI表示虽然将该控制块释放到自由队列里面，但是有可能还没有些到磁盘上。
	 * B_DONE则是指该缓存的内容正确地反映了存储在或应存储在磁盘上的信息 
	 */
    bp->b_flags &= ~(Buf::B_WANTED | Buf::B_BUSY | Buf::B_ASYNC);
    /*(this->bFreeList.av_back)->av_forw = bp;
    bp->av_back = this->bFreeList.av_back;
    bp->av_forw = &(this->bFreeList);
    this->bFreeList.av_back = bp;*/
    //printf("Brelse\n");
    return;
}

void BufferManager::NotAvail(Buf *bp)
{
    /* 从自由队列中取出 */
    //bp->av_back->av_forw = bp->av_forw;
    //bp->av_forw->av_back = bp->av_back;
    /* 设置B_BUSY标志 */
    bp->b_flags |= Buf::B_BUSY;
    return;
}

Buf *BufferManager::Bread(int blkno)
{
    Buf *bp;
    /* 根据字符块号申请缓存 */
    bp = this->GetBlk(blkno);
    /* 如果在设备队列中找到所需缓存，即B_DONE已设置，就不需进行I/O操作 */
    if (bp->b_flags & Buf::B_DONE)
    {
        return bp;
    }
    /* 没有找到相应缓存，构成I/O读请求块 */
    bp->b_flags |= Buf::B_READ;
    bp->b_wcount = BufferManager::BUFFER_SIZE;

    /* 这里直接通过磁盘驱动进行读操作即可 */
    this->m_DiskDriver->IO(bp);
    return bp;
}

void BufferManager::Bwrite(Buf *bp)
{
    unsigned int flags;

    flags = bp->b_flags;
    bp->b_flags &= ~(Buf::B_READ | Buf::B_DONE | Buf::B_ERROR | Buf::B_DELWRI);
    bp->b_wcount = BufferManager::BUFFER_SIZE; /* 512字节 */

    this->m_DiskDriver->IO(bp);
    this->Brelse(bp);

    bp->b_back->b_forw = bp->b_forw;
    bp->b_forw->b_back = bp->b_back;

    bp->b_back = this->bFreeList.b_back;
    this->bFreeList.b_back->b_forw = bp;
    bp->b_forw = &this->bFreeList;
    this->bFreeList.b_back = bp;

    return;
}

void BufferManager::Bdwrite(Buf *bp)
{
    /* 置上B_DONE允许其它进程使用该磁盘块内容 */
    bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
    bp->b_wcount = BufferManager::BUFFER_SIZE;
    this->Brelse(bp);

    bp->b_back->b_forw = bp->b_forw;
    bp->b_forw->b_back = bp->b_back;

    bp->b_back = this->bFreeList.b_back;
    this->bFreeList.b_back->b_forw = bp;
    bp->b_forw = &this->bFreeList;
    this->bFreeList.b_back = bp;

    return;
}

void BufferManager::ClrBuf(Buf *bp)
{
    int *pInt = (int *)bp->b_addr;

    /* 将缓冲区中数据清零 */
    for (unsigned int i = 0; i < BufferManager::BUFFER_SIZE / sizeof(int); i++)
    {
        pInt[i] = 0;
    }
    return;
}

void BufferManager::Bflush()
{
    Buf *bp;
    /*for (bp = this->bFreeList.av_forw; bp != &(this->bFreeList); bp = bp->av_forw)
    {
        // 找出自由队列中所有延迟写的块
        if ((bp->b_flags & Buf::B_DELWRI))
        {
            //bp->b_flags |= Buf::B_ASYNC;
            this->NotAvail(bp);
            this->Bwrite(bp);
        }
    }*/

    /*for (bp = this->bFreeList.b_forw; bp != &(this->bFreeList); bp = bp->b_forw)
    {
        // 找出自由队列中所有延迟写的块 
        if ((bp->b_flags & Buf::B_DELWRI))
        {
            bp->b_back->b_forw = bp->b_forw;
            bp->b_forw->b_back = bp->b_back;

            bp->b_back = this->bFreeList.b_back;
            this->bFreeList.b_back->b_forw = bp;
            bp->b_forw = &this->bFreeList;
            this->bFreeList.b_back = bp;

            this->Bwrite(bp);
        }
    }*/

    for (int i = 0; i < NBUF; i++)
    {
        bp = this->m_Buf + i;
        if (bp->b_flags & Buf::B_DELWRI)
        {
            this->Bwrite(bp);
        }
    }
    return;
}

Buf *BufferManager::InCore(int blkno)
{
    Buf *bp;
    Buf *dp = &(this->bFreeList);
    for (bp = dp->b_forw; bp != dp; bp = bp->b_forw)
    {
        if (bp->b_blkno == blkno)
            return bp;
    }
    return NULL;
}
