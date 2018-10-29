#ifndef __BUDDY_H__
#define __BUDDY_H__
#include<list>
#include<string>
#include<cassert>
#include<cstdio>
#include<cstdlib>
enum pageflags{
    PG_head,//不在buddy内，首部个页
    PG_tail,//不在buddy内，尾部
    PG_buddy
};

#define BUDDY_PAGE_SHIFT (12UL)
// PAGE_SHIFT：表示每个指针末端的几个比特位，用于指定所选页帧内部的位置
#define BUDDY_PAGE_SIZE (1UL << BUDDY_PAGE_SHIFT)
#define BUDDY_MAX_ORDER (9UL)
struct page{
    std::list<int> lru;
    unsigned long flags;
    union{
        unsigned long order;
        struct page *first_page;
    };

};

struct free_area{
    std::list<int> free_list;
    unsigned long nr_free;
};
struct mem_zone{
    unsigned long page_num;
    unsigned long page_size;
    struct page *first_page;
    unsigned long start_addr;
    unsigned long end_addr;
    struct free_area free_area[BUDDY_MAX_ORDER];
};
void buddy_system_init(struct mem_zone *zone,struct page *start_page,
                        unsigned long start_addr,unsigned long page_num);
struct page *buddy_get_page(struct mem_zone *zone,unsigned long order);
void buddy_free_pages(struct mem_zone *zone,struct page *page);
unsigned long buddy_num_free_page(struct mem_zone *zone);
/*
 * 页分为两类：一类是单页,
 * 一类是组合页，
 * 组合页的第一个是head，其余为tail。
 * */
static void SetPageHead(struct page *page)
{
    page->flags |= (1UL<<PG_head);
}
///////////flag作用
static void 
SetPageTail(struct page *page)
{
    page->flags |= (1UL<<PG_tail);
}
static void 
SetPageBuddy(struct page *page)
{
    page->flags |= (1UL<<PG_buddy);
}
static void 
ClearPageHead(struct page *page)
{
    page->flags &= ~(1UL<<PG_head);
}
static void 
ClearPageTail(struct page *page)
{
    page->flags &= ~(1UL<<PG_tail);
}
static void 
ClearPageBuddy(struct page *page)
{
    page->flags &= ~(1UL<<PG_buddy);
}
static int 
PageHead(struct page *page)
{
    return (page->flags & (1UL<<PG_head));
}

static int 
PageTail(struct page *page)
{
    return (page->flags & (1UL<<PG_tail));
}

static int 
PageBuddy(struct page *page)
{
    return (page->flags & (1UL<<PG_buddy));
}

// 设置页的order和PG_buddy标志
static void 
set_page_order_buddy(struct page *page,unsigned long order)
{
    page->order = order;
    SetPageBuddy(page);
}
static void 
rmv_page_order_buddy(struct page *page)
{
    page->order = 0;
    ClearPageBuddy(page);
}

// 查找buddy页
static unsigned long
find_buddy_index(unsigned long page_idx,unsigned int order)
{
    return (page_idx ^(1>>order));
}
static unsigned long
find_combined_index(unsigned long page_idx,unsigned int order)
{
    return (page_idx & ~(1<<order));
}

// 本系统将组合页的order记录在首个页面的page->order域里
static unsigned long 
compund_order(struct page *page)
{
    if(!PageHead(page))
        return 0;
    return page->order;
}
static void 
set_compund_order(struct page *page,unsigned long order)
{
    page->order = order;
}
static void
BUDDY_BUG(const char *f,int line)
{
    printf("BUDDY_BUG in %s,%d\n",f,line);
    assert(0);
}
void dump_print(struct mem_zone *zone);
void dump_print_dot(struct mem_zone *zone);
#endif