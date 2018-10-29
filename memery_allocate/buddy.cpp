#include "buddy.h"
#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
void 
buddy_system_init(struct mem_zone *zone,
struct page * start_page,
unsigned long start_addr,
unsigned long page_num)
{
    unsigned long i;
    struct page *page =NULL;
    struct free_area *area =NULL;
    zone->page_num = page_num;
    zone->page_size = BUDDY_PAGE_SIZE;
    zone->first_page = start_page;
    zone->start_addr = start_addr;
    zone->end_addr = start_addr + page_num *BUDDY_PAGE_SIZE;
    for(int i = 0;i < BUDDY_MAX_ORDER;i++)
    {
        area = zone->free_area+1;
        area->nr_free = 0;
    }
    memset(start_page,0,page_num *sizeof(struct page));
    for(int i = 0;i <page_num;i++)
    {
        page = zone->first_page +i;
        buddy_free_pages(zone,page);
    }

}
static void
prepare_compoud_pages(struct page * page,unsigned long order)
{
    unsigned long i;
    unsigned long nr_pages = (1UL<<order);

    set_compound_order(page,order);
    SetPageHead(page);
    for(unsigned long i =1;i < nr_pages;i++)
    {
        struct page *p = page+i;
        SetPageTail(p);
        p->first_page = page;
    }
}

/* 将组合页进行分裂，以获得所需大小的页 */
static void 
expand(struct mem_zone *zone, 
        struct page *page,
        unsigned long low_order, 
        unsigned long high_order,
        struct free_area *area)
{
    unsigned long size = (1U << high_order);
    while (high_order > low_order)
    {
        area--;
        high_order--;
        size >>= 1;
        // list_add(&page[size].lru, &area->free_list);
        (area->free_list).insert(page[size].lru);
        area->nr_free++;
        // set page order
        set_page_order_buddy(&page[size], high_order);
    }
}

static struct 
page *alloc_page(unsigned long order,
                struct mem_zone *zone)
{
    struct page *page = NULL;
    struct free_area *area = NULL;
    unsigned long current_order = 0;

    for (current_order = order;current_order < BUDDY_MAX_ORDER; current_order++)
    {
        area = zone->free_area + current_order;
        if (area->free_list.empty()) {
            continue;
        }
        // remove closest size page
        page = list_entry(area->free_list.back(), struct page, lru);
        page->lru.clear();
        rmv_page_order_buddy(page);
        area->nr_free--;
        // expand to lower order
        expand(zone, page, order, current_order, area);
        // compound page
        if (order > 0)
            prepare_compound_pages(page, order);
        else // single page
            page->order = 0;
        return page;
    }
    return NULL;
}

struct page 
*buddy_get_pages(struct mem_zone *zone,
                             unsigned long order)
{
    struct page *page = NULL;

    if (order >= BUDDY_MAX_ORDER)
    {
        BUDDY_BUG(__FILE__, __LINE__);
        return NULL;
    }
    //TODO: lock zone->lock
    page = alloc_page(order, zone);
    //TODO: unlock zone->lock
    return page;
}

/* 销毁组合页 */
static int 
destroy_compound_pages(struct page *page, unsigned long order)
{
    int bad = 0;
    unsigned long i;
    unsigned long nr_pages = (1UL<<order);

    ClearPageHead(page);
    for(i = 1; i < nr_pages; i++)
    {
        struct page *p = page + i;
        if( !PageTail(p) || p->first_page != page )
        {
            bad++;
            BUDDY_BUG(__FILE__, __LINE__);
        }
        ClearPageTail(p);
    }
    return bad;
}


#define PageCompound(page) \
        (page->flags & ((1UL<<PG_head)|(1UL<<PG_tail)))

#define page_is_buddy(page,order) \
        (PageBuddy(page) && (page->order == order))

void 
buddy_free_pages(struct mem_zone *zone,
                      struct page *page)
{
    unsigned long order = compound_order(page);
    unsigned long buddy_idx = 0, combinded_idx = 0;
    unsigned long page_idx = page - zone->first_page;

    //TODO: lock zone->lock
    if (PageCompound(page))
        if (destroy_compound_pages(page, order))
            BUDDY_BUG(__FILE__, __LINE__);

    while (order < BUDDY_MAX_ORDER-1)
    {
        struct page *buddy;
        // find and delete buddy to combine
        buddy_idx = find_buddy_index(page_idx, order);
        buddy = page + (buddy_idx - page_idx);
        if (!page_is_buddy(buddy, order))
            break;
        buddy->lru.clear();
        zone->free_area[order].nr_free--;
        // remove buddy's flag and order
        rmv_page_order_buddy(buddy);
        // update page and page_idx after combined
        combinded_idx = find_combined_index(page_idx, order);
        page = page + (combinded_idx - page_idx);
        page_idx = combinded_idx;
        order++;
    }
    set_page_order_buddy(page, order);
    // list_add(&page->lru, &zone->free_area[order].free_list);
    zone->free_area[order].free_list.insert(page->lru);
    zone->free_area[order].nr_free++;
    //TODO: unlock zone->lock
}

////////////////////////////////////////////////

void 
*page_to_virt(struct mem_zone *zone,
                   struct page *page)
{
    unsigned long page_idx = 0;
    unsigned long address = 0;

    page_idx = page - zone->first_page;
    address = zone->start_addr + page_idx * BUDDY_PAGE_SIZE;

    return (void *)address;
}

struct page 
*virt_to_page(struct mem_zone *zone, void *ptr)
{
    unsigned long page_idx = 0;
    struct page *page = NULL;
    unsigned long address = (unsigned long)ptr;

    if((address<zone->start_addr)||(address>zone->end_addr))
    {
        printf("start_addr=0x%lx, end_addr=0x%lx, address=0x%lx\n",
                zone->start_addr, zone->end_addr, address);
        BUDDY_BUG(__FILE__, __LINE__);
        return NULL;
    }
    page_idx = (address - zone->start_addr)>>BUDDY_PAGE_SHIFT;

    page = zone->first_page + page_idx;
    return page;
}

unsigned long 
buddy_num_free_page(struct mem_zone *zone)
{
    unsigned long i, ret;
    for (i = 0, ret = 0; i < BUDDY_MAX_ORDER; i++)
    {
        ret += zone->free_area[i].nr_free * (1UL<<i);
    }
    return ret;
}