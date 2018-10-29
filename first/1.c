#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int main(void)
{
    int i;
    for (i = 0; i < 2; i++)
    {
        fork();
        // fprintf(stderr, "-");
        printf("-");
        //输出结果是八个，因为缓冲区没有清空，导致上一个进程的字符没有清空，所以是8个

        //             3 -   2
        //         /
        //      1 - 
        //         \   4 -   2
        //     /
        // 1       /  5 -   2 
        //     \2 -
        //            \ 6 -  2


        // 清空缓冲区的时候1,2 进程直接输出，就不会有残余
    }
   
    wait(NULL);
    wait(NULL);
    return 0;
}
