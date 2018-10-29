#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#define PROCESS_NAME_LEN 32   /*进程名长度*/
#define MIN_SLICE 10          /*最小碎片的大小*/
#define DEFAULT_MEM_SIZE 1024 /*内存大小*/
#define DEFAULT_MEM_START 0   /*起始位置*/
/* 内存分配算法 */
#define MA_FF 1
#define MA_BF 2
#define MA_WF 3
int mem_size = DEFAULT_MEM_SIZE; /*内存大小*/
int ma_algorithm = MA_FF;        /*当前分配算法*/
static int pid = 0;              /*初始pid*/
int flag = 0;                    /*设置内存大小标志*/

/*描述每一个空闲块的数据结构*/
struct free_block_type
{
    int size;
    int start_addr;
    struct free_block_type *next;
};
/*指向内存中空闲块链表的首指针*/
struct free_block_type *free_block;
/*每个进程分配到的内存块的描述*/
struct allocated_block
{
    int pid;
    int size;
    int start_addr;
    char process_name[PROCESS_NAME_LEN];
    struct allocated_block *next;
};
/*进程分配内存块链表的首指针*/
struct allocated_block *allocated_block_head = NULL;

//函数声明
void display_menu();
int set_mem_size();
void set_algorithm();
void rearrange(int algorithm);
int new_process();
int allocate_mem(struct allocated_block *ab);
void kill_process();
int free_mem(struct allocated_block *ab);
int dispose(struct allocated_block *free_ab);
int display_mem_usage();
//allocated_block* find_process(int pid);
void rearrange_FF();
void rearrange_BF();
void rearrange_WF();

/*按FF算法重新整理内存空闲块链表*/
void rearrange_FF()
{ 
    //请自行补充 m代表头，p。q分别代表交换的两个节点 n代表交换截至位置
    struct free_block_type *p, *q, *m, *n;

    if (free_block->next->next != NULL)
    {
        n = NULL;
        while (n != free_block->next->next)
        {
            m = free_block;
            p = free_block->next;
            q = p->next;
            while (q != n)
            {
                if (p->start_addr > q->start_addr)
                {
                    p->next = q->next;
                    q->next = p;
                    m->next = q;
                }
                m = m->next;
                p = m->next;
                q = p->next;
            }
            n = p;
        }
    }
}
/*按BF算法重新整理内存空闲块链表*/
void rearrange_BF()
{
    //请自行补充

    //请自行补充 m代表头，p。q分别代表交换的两个节点 n代表交换截至位置
    struct free_block_type *p, *q, *m, *n;

    if (free_block->next->next != NULL)
    {
        n = NULL;
        while (n != free_block->next->next)
        {
            m = free_block;
            p = free_block->next;
            q = p->next;
            while (q != n)
            {
                if (p->size > q->size)
                {
                    p->next = q->next;
                    q->next = p;
                    m->next = q;
                }
                m = m->next;
                p = m->next;
                q = p->next;
            }
            n = p;
        }
    }
}
/*按WF算法重新整理内存空闲块链表*/
void rearrange_WF()
{
    //请自行补充
    //请自行补充 m代表头，p。q分别代表交换的两个节点 n代表交换截至位置
    struct free_block_type *p, *q, *m, *n;

    if (free_block->next->next != NULL)
    {
        n = NULL;
        while (n != free_block->next->next)
        {
            m = free_block;
            p = free_block->next;
            q = p->next;
            while (q != n)
            {
                if (p->size < q->size)
                {
                    p->next = q->next;
                    q->next = p;
                    m->next = q;
                }
                m = m->next;
                p = m->next;
                q = p->next;
            }
            n = p;
        }
    }
}

//查找进程
struct allocated_block *find_process(int pid)
{
    struct allocated_block *abb;
    for (abb = allocated_block_head->next; abb && abb->pid != pid; abb = abb->next)
        ;
    return abb;
}

//释放内存
int free_mem(struct allocated_block *ab)
{
    struct free_block_type *p;
    //分配空闲分区表的中的一个节点  每一次选择头节点后后面插入，并且完成之后查看当前算法，重新整理链表
    p = (struct free_block_type *)malloc(sizeof(struct free_block_type));
    p->next = free_block->next;//头插
    free_block->next = p;
    p->size = ab->size;
    p->start_addr = ab->start_addr;

    return 1;
}

//释放要杀死进程的节点
int dispose(struct allocated_block *ab)
{
    struct allocated_block *p;
    for (p = allocated_block_head; p && p->next != ab; p = p->next)
        ;
    p->next = ab->next;
    free(ab);
    return 1;
}

//杀死进程
void kill_process()
{
    struct allocated_block *ab;
    int pid;
    printf("Kill Process, pid=");
    scanf("%d", &pid);
    getchar();
    ab = find_process(pid);
    if (ab != NULL)
    {
        free_mem(ab);
        dispose(ab);
    }
}

int allocate_mem(struct allocated_block *ab)
{
    struct free_block_type *p, *q, *temp;
    int sum = 0;
    if (free_block->next == NULL)
    {
        printf("no  memory");
        return -1;
    }
    //当前算法进行整理内存
    rearrange(ma_algorithm);

    //根据当前算法在空闲分区链表中搜索合适空闲分区进行分配，分配时注意以下情况：
    // 1. 找到可满足空闲分区且分配后剩余空间足够大，则分割
    // 2. 找到可满足空闲分区且但分配后剩余空间（最小碎片化）比较小，则一起分配
    // 3. 找不可满足需要的空闲分区但空闲分区之和能满足需要，则采用内存紧缩技术，进行空闲分区的合并，然后再分配
    // 4. 在成功分配内存后，应保持空闲分区按照相应算法有序
    // 5. 分配成功则返回1，否则返回-1

    //如果首次适应 和最佳适应差不多 从链首开始查找，如果找到，则开始分配，如果没有找到，采用分区紧缩技术，如果可以分配，则分配，不能分配，返回-1
    if (ma_algorithm == MA_FF || ma_algorithm == MA_BF)
    {
        //开始寻找符合条件的节点
        for (p = free_block, q = p->next; q && q->size < ab->size; p = p->next, q = p->next)
            ;

        //找到
        if (q)
        {
            //判断是否达到最小碎片程度

            if ((q->size - ab->size) <= MIN_SLICE)
            {
                //全部分配
                ab->start_addr = q->start_addr;
                ab->size = q->size;
                //释放q节点
                p->next = q->next;
                free(q);
            }
            else
            {
                ab->start_addr = q->start_addr;
                q->start_addr = q->start_addr + ab->size;
                q->size = q->size - ab->size;
            }
        }
        //开始内存紧缩
        else
        {
            for (q = free_block->next; q; q = q->next)
            {
                sum += q->size;
                if (sum >= ab->size)
                {
                    break;
                }
            }
            if (q)
            {
                //第一个空闲分区大小更改为sum，并且除了第一个，其他结点全部删除
                free_block->next->size = sum;
                p = free_block->next;
                for (temp = p->next; temp != q; temp = p->next)
                {
                    p->next = temp->next;
                    free(temp);
                }
                p->next = q->next;
                free(q);

                p->start_addr = p->start_addr + ab->size;
                p->size = p->size - ab->size;
                //如果剩余空间为0 则直接释放当前的空间
            }
            else

            {
                return -1;
            }
        }
    }
    //最差适应
    else if (ma_algorithm == MA_WF)
    {
        //只需要找到第一个最大的空闲分区  如果满足，则分配，不满足则判断能否进行合并，如果可以，分配
        if (free_block->next->size >= ab->size)
        {
            if (free_block->next->size - ab->size >= 10)
            {
                ab->start_addr = free_block->next->start_addr;
                free_block->next->start_addr += ab->size;
                free_block->next->size -= ab->size;
            }
            else

            {
                p = free_block->next;
                free_block->next = p->next;
                free(p);
            }
        }
        else
        {
            for (q = free_block->next; q; q = q->next)
            {
                sum += q->size;
                if (sum >= ab->size)
                {
                    break;
                }
            }

            if (q)
            {
                //第一个空闲分区大小更改为sum，并且除了第一个，其他结点全部删除
                free_block->next->size = sum;
                p = free_block->next;
                for (temp = p->next; temp != q; temp = p->next)
                {
                    p->next = temp->next;
                    free(temp);
                }
                p->next = q->next;
                free(q);

                ab->start_addr = free_block->next->start_addr;
                p->start_addr = p->start_addr + ab->size;
                p->size = p->size - ab->size;
                //如果剩余空间为0 则直接释放当前的空间
            }
            else

            {
                return -1;
            }
        }
    }

    return 1;
}

//创建一个新的进程
int new_process()
{
    struct allocated_block *ab;
    int size;
    int ret;
    ab = (struct allocated_block *)malloc(sizeof(struct allocated_block));
    if (!ab)
        exit(-5);
    ab->next = NULL;
    pid++;
    sprintf(ab->process_name, "PROCESS-%02d", pid);
    ab->pid = pid;
    printf("Memory for %s:", ab->process_name);
    scanf("%d", &size);
    getchar();
    if (size > 0)
        ab->size = size;
    ret = allocate_mem(ab); /* 从空闲区分配内存，ret==1表示分配ok*/
    /*如果此时allocated_block_head尚未赋值，则赋值*/
    if ((ret == 1) && (allocated_block_head->next == NULL))
    {
        allocated_block_head->next = ab;
        return 1;
    }
    /*分配成功，将该已分配块的描述插入已分配链表*/
    else if (ret == 1)
    {
        ab->next = allocated_block_head->next;
        allocated_block_head->next = ab;
        return 2;
    }
    else if (ret == -1)
    {
        /*分配不成功*/
        printf("Allocation fail\n");
        free(ab);
        return -1;
    }
    return 3;
}

int set_mem_size()
{
    int size;
    if (flag != 0)
    { //防止重复设置
        printf("Cannot set memory size again\n");
        return 0;
    }
    printf("Total memory size =");
    scanf("%d", &size);
    getchar();
    if (size > 0)
    {
        mem_size = size;
        free_block->next->size = mem_size;
    }
    flag = 1;
    return 1;
}
void rearrange(int algorithm)
{
    switch (algorithm)
    {
    case MA_FF:
        rearrange_FF();
        break;
    case MA_BF:
        rearrange_BF();
        break;
    case MA_WF:
        rearrange_WF();
        break;
    }
}

/* 设置当前的分配算法 */
void set_algorithm()
{
    int algorithm;
    printf("----------------------------------\n");
    printf("\t1 - First Fit\n");
    printf("\t2 - Best Fit \n");
    printf("\t3 - Worst Fit \n");
    scanf("%d", &algorithm);
    getchar();
    if (algorithm >= 1 && algorithm <= 3)
        ma_algorithm = algorithm;
    //按指定算法重新排列空闲区链表
    rearrange(ma_algorithm);
}

//释放所有链表
void do_exit()
{
    struct free_block_type *p;
    struct allocated_block *q;
    p = free_block->next;
    q = allocated_block_head->next;
    while (free_block->next != NULL)
    {
        free_block->next = p->next;
        free(p);
        p = free_block->next;
    }
    free(free_block);

    while (allocated_block_head->next)
    {
        allocated_block_head->next = q->next;
        free(q);
        q = allocated_block_head->next;
    }
    free(allocated_block_head);
}

/*初始化空闲块，默认为一块，可以指定大小及起始地址*/
struct free_block_type *init_free_block(int mem_size)
{
    struct free_block_type *fb, *p;
    struct allocated_block *q;
    fb = (struct free_block_type *)malloc(sizeof(struct free_block_type));
    p = (struct free_block_type *)malloc(sizeof(struct free_block_type));
    q = (struct allocated_block *)malloc(sizeof(struct allocated_block));

    if (fb == NULL || p == NULL || q == NULL)
    {
        printf("No mem\n");
        return NULL;
    }
    q->next = NULL;
    allocated_block_head = q;
    p->size = mem_size;
    p->start_addr = DEFAULT_MEM_START;
    p->next = NULL;

    fb->next = p;
    return fb;
}

int display_mem_usage()
{
    struct free_block_type *fbt = free_block->next;
    struct allocated_block *ab = allocated_block_head->next;

    if (fbt == NULL)
        return (-1);
    printf("----------------------------------------------------------\n");

    /* 显示空闲区 */
    printf("Free Memory:\n");
    printf("%20s %20s\n", "      start_addr", "       size");
    while (fbt != NULL)
    {
        printf("%20d %20d\n", fbt->start_addr, fbt->size);
        fbt = fbt->next;
    }
    /* 显示已分配区 */
    printf("\nUsed Memory:\n");
    printf("%10s %20s %10s %10s\n", "PID", "ProcessName", "start_addr", " size");
    while (ab != NULL)
    {
        printf("%10d %20s %10d %10d\n", ab->pid, ab->process_name, ab->start_addr, ab->size);
        ab = ab->next;
    }
    printf("----------------------------------------------------------\n");
    return 0;
}

/*显示菜单*/
void display_menu()
{
    printf("\n");
    printf("1 - Set memory size (default=%d)\n", DEFAULT_MEM_SIZE);
    printf("2 - Select memory allocation algorithm\n");
    printf("3 - New process \n");
    printf("4 - Terminate a process \n");
    printf("5 - Display memory usage \n");
    printf("0 - Exit\n");
}

int main(void)
{
    char choice;
    pid = 0;
    free_block = init_free_block(mem_size); //初始化空闲区

    while (1)
    {
        display_menu(); //显示菜单
        fflush(stdin);
        choice = getchar();
        fflush(stdin);
        //获取用户输入
        switch (choice)
        {
        case '1':
            set_mem_size();
            break; //设置内存大小
        case '2':
            set_algorithm();
            flag = 1;
            break; //设置算法
        case '3':
            new_process();
            flag = 1;
            break; //创建新进程
        case '4':
            kill_process();
            flag = 1;
            break; //删除进程
        case '5':
            display_mem_usage();
            flag = 1;
            break; //显示内存使用
        case '0':
            do_exit();
            exit(0); //释放链表并退出
        default:
            break;
        }
    }
    return 0;
}