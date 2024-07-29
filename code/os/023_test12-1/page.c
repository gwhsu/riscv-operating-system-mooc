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
 * _num_pages   保存我们可以分配的实际最大页数。
 */
static uint32_t _alloc_start = 0;
static uint32_t _alloc_end = 0;
static uint32_t _num_pages = 0;

#define PAGE_SIZE 4096
#define PAGE_ORDER 12

#define PAGE_TAKEN (uint8_t)(1 << 0)
#define PAGE_LAST  (uint8_t)(1 << 1)
/*
 * Page Descriptor 
 * flags:
 * - bit 0: 如果分配了此页，则标记
 * - bit 1: 如果此页是分配的内存块的最后一页，则标记
 */
struct Page {
	uint8_t flags;
};

// 清空标志位
static inline void _clear(struct Page *page)
{
    page->flags = 0;
}

// 判断是否被释放了
static inline int _is_free(struct Page *page)
{
	if (page->flags & PAGE_TAKEN) {
		return 0;
	} else {
		return 1;
	}
}

// 设置对应位的标志
static inline void _set_flag(struct Page *page, uint8_t flags)
{
	page->flags |= flags;
}

// 判断此页是否是分配的内存块的最后一页
static inline int _is_last(struct Page *page)
{
	if (page->flags & PAGE_LAST) {
		return 1;
	} else {
		return 0;
	}
}

// 将地址与页面边框对齐（4K）
static inline uint32_t _align_page(uint32_t address)
{
	uint32_t order = (1 << PAGE_ORDER) - 1;
	return (address + order) & (~order);
}

// 我们保留了8页（8 x 4096）来保存Page结构。
// 最多可以管理128 MB（8 x 4096 x 4096）
void page_init()
{
    // 我们保留了8页（8 x 4096）来保存Page结构。
    // 最多可以管理128 MB（8 x 4096 x 4096）

    // 获取堆中可以分配的存储页个数
    _num_pages = (HEAP_SIZE / PAGE_SIZE) - 8;

    printf("HEAP_START = %x, HEAP_SIZE = %x, num of pages = %d\n", HEAP_START, HEAP_SIZE, _num_pages);

    // 用结构体指针，访问堆的起始地址。
    struct Page *page = (struct Page *)HEAP_START;

    // 清空管理页
    for(int i = 0; i < _num_pages; i++)
    {
        _clear(page);
        page++;
    }

    // 获取堆的存储区的起始地址
    _alloc_start = _align_page(HEAP_START + 8*PAGE_SIZE);
    // 获取堆的存储区的结束地址
    _alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);

	printf("TEXT:   0x%x -> 0x%x\n", TEXT_START, TEXT_END);
	printf("RODATA: 0x%x -> 0x%x\n", RODATA_START, RODATA_END);
	printf("DATA:   0x%x -> 0x%x\n", DATA_START, DATA_END);
	printf("BSS:    0x%x -> 0x%x\n", BSS_START, BSS_END);
	printf("HEAP:   0x%x -> 0x%x\n", _alloc_start, _alloc_end);
}


// 分配由连续物理页组成的内存块
// - npages: 要分配的PAGE_SIZE页数
void *page_alloc(int npages)
{
    // 搜索页面描述符位图
    int found = 0;
    struct Page *page_i = (struct Page *)HEAP_START;

    for(int i=0; i<=(_num_pages - npages); i++)
    {
        if(_is_free(page_i))
        {
            found = 1;

            // 判断是否有足够分配的连续空闲页
            struct Page* page_j = page_i;
            for (int j = i; j < (i+npages); j++)
            {
                if(!_is_free(page_j))
                {
                    found = 0;
                    break;
                }
                page_j++;
            }

            // 有足够空间时，分配该空间
            if(found)
            {
                struct Page* page_k = page_i;
                for(int k=i; k<(i+npages); k++)
                {
                    _set_flag(page_k, PAGE_TAKEN);
                    page_k++;
                }
                page_k--;
                _set_flag(page_k, PAGE_LAST);
                return (void *)(_alloc_start + i*PAGE_SIZE);
            }
        }
        page_i++;
    }
    return NULL;
}


// 释放这个内存块
// p: 这个内存块的起始地址
void page_free(void *p)
{
    if(!p || (uint32_t)p >= _alloc_end)
    {
        return;
    }

    // 获取堆的起始地址，也就是管理页的第一页
    struct Page *page = (struct Page *)HEAP_START;

    // 获取对于内存块对应的管理页
    page += ((uint32_t)p - _alloc_start)/PAGE_SIZE;

    // 判断是否是未使用的页，是就退出，
    // 不是再判断是不是内存块的最后一页，
    // 是就清楚后退出，不是就清除后，到下一页。
    while(!_is_free(page))
    {
        if(_is_last(page))
        {
            _clear(page);
            break;
        }
        else
        {
            _clear(page);
            page++;
        }
    }
}


void page_test()
{
	void *p = page_alloc(2);
	printf("p = 0x%x\n", p);
	//page_free(p);

	void *p2 = page_alloc(7);
	printf("p2 = 0x%x\n", p2);
	page_free(p2);

	void *p3 = page_alloc(4);
	printf("p3 = 0x%x\n", p3);
}




