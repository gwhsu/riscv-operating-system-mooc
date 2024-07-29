#include "os.h"



// mem.S 中定义了以下全局变量
extern uint32_t TEXT_START;
extern uint32_t TEXT_END;

extern uint32_t DATA_START;
extern uint32_t DATA_END;

extern uint32_t RODATA_START;
extern uint32_t RODATA_END;

extern uint32_t BSS_START;
extern uint32_t BSS_END;

extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;


/*
 * _alloc_start 指向堆池的实际起始地址
 * _alloc_end   指向堆池的实际结束地址
 * _num_word   保存我们可以分配的实际最大页数。
 */
static uint32_t _alloc_start = 0;
static uint32_t _alloc_end = 0;
static uint32_t _num_word = 0;

#define WORD_SIZE 4
#define WORD_ORDER 2

#define MEM_TAKEN (uint8_t)(1 << 0)
#define MEM_LAST  (uint8_t)(1 << 1)
/*
 * Memory Descriptor 
 * flags:
 * - bit 0: 如果分配了此字，则标记
 * - bit 1: 如果此页是分配的内存块的最后1字，则标记
 */
struct memMan {
	uint8_t flags;
};

// 清空标志位
static inline void _clear(struct memMan *mam)
{
    mam->flags = 0;
}

// 判断是否被释放了
static inline int _is_free(struct memMan *mam)
{
	if (mam->flags & MEM_TAKEN) {
		return 0;
	} else {
		return 1;
	}
}

// 设置对应位的标志
static inline void _set_flag(struct memMan *mem, uint8_t flags)
{
	mem->flags |= flags;
}

// 判断此字是否是分配的内存块的最后一个字
static inline int _is_last(struct memMan *mem)
{
	if (mem->flags & MEM_LAST) {
		return 1;
	} else {
		return 0;
	}
}

// 将地址与字对齐（4Byte）
static inline uint32_t _align_word(uint32_t address)
{
	uint32_t order = (1 << WORD_ORDER) - 1;
	return (address + order) & (~order);
}

// 我们保留了 256字（256 x 4）来保存word结构。
// 最多可以管理 4KB（256*4*4)
void malloc_init()
{
    // 我们保留了 256字（256 x 4）来保存word结构。
    // 最多可以管理 4KB（8256*4*4)

    // 获取堆中可以分配的存储字个数
    _num_word = 1024;

    printf("HEAP_START = %x, ALLOCATE_SIZE = %x, num of words = %d\n", HEAP_START, 4096, _num_word);

    // 用结构体指针，访问堆的起始地址。
    struct memMan *word = (struct memMan *)HEAP_START;

    // 清空管理页
    for(int i = 0; i < _num_word; i++)
    {
        _clear(word);
        word++;
    }

    // 获取堆的存储区的起始地址
    _alloc_start = _align_word(HEAP_START + 256*WORD_SIZE);
    // 获取堆的存储区的结束地址
    _alloc_end = _alloc_start + (WORD_SIZE * _num_word);

	printf("TEXT:   0x%x -> 0x%x\n", TEXT_START, TEXT_END);
	printf("RODATA: 0x%x -> 0x%x\n", RODATA_START, RODATA_END);
	printf("DATA:   0x%x -> 0x%x\n", DATA_START, DATA_END);
	printf("BSS:    0x%x -> 0x%x\n", BSS_START, BSS_END);
	printf("HEAP:   0x%x -> 0x%x\n", _alloc_start, _alloc_end);
}


// 分配由连续物理页组成的内存块
// - nwords: 要分配的WORD_SIZE
void *malloc(int nwords)
{
    // 搜索页面描述符位图
    int found = 0;
    struct memMan *word_i = (struct memMan *)HEAP_START;

    for(int i=0; i<=(_num_word - nwords); i++)
    {
        if(_is_free(word_i))
        {
            found = 1;

            // 判断是否有足够分配的连续空闲字
            struct memMan* word_j = word_i;
            for (int j = i; j < (i+nwords); j++)
            {
                if(!_is_free(word_j))
                {
                    found = 0;
                    break;
                }
                word_j++;
            }

            // 有足够空间时，分配该空间
            if(found)
            {
                struct memMan* word_k = word_i;
                for(int k=i; k<(i+nwords); k++)
                {
                    _set_flag(word_k, MEM_TAKEN);
                    word_k++;
                }
                word_k--;
                _set_flag(word_k, MEM_LAST);
                return (void *)(_alloc_start + i*WORD_SIZE);
            }
        }
        word_i++;
    }
    return NULL;
}


// 释放这个内存块
// p: 这个内存块的起始地址
void free(void *p)
{
    if(!p || (uint32_t)p >= _alloc_end)
    {
        return;
    }

    // 获取堆的起始地址，也就是管理页的第一页
    struct memMan *word = (struct memMan *)HEAP_START;

    // 获取对于内存块对应的管理字
    word += ((uint32_t)p - _alloc_start)/WORD_SIZE;

    // 判断是否是未使用的页，是就退出，
    // 不是再判断是不是内存块的最后一页，
    // 是就清楚后退出，不是就清除后，到下一页。
    while(!_is_free(word))
    {
        if(_is_last(word))
        {
            _clear(word);
            break;
        }
        else
        {
            _clear(word);
            word++;
        }
    }
}


void word_test()
{
	void *p = malloc(2);
	printf("p = 0x%x\n", p);
	free(p);

	void *p2 = malloc(7);
	printf("p2 = 0x%x\n", p2);
	// free(p2);

	void *p3 = malloc(4);
	printf("p3 = 0x%x\n", p3);
}




