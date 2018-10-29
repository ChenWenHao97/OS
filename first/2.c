#include <stdio.h>

#include <sys/types.h>

#include <unistd.h>

#include <signal.h>

#include <ctype.h>

/* 允许建立的子进程个数最大值 */

#define MAX_CHILD_NUMBER 10

/* 子进程睡眠时间 */

#define SLEEP_INTERVAL 2

    int proc_number = 0; /* 子进程的自编号，从0开始 */

void do_something();

int main(int argc, char *argv[])

{

    int child_proc_number = MAX_CHILD_NUMBER; /* 子进程个数 */

    int i, ch;

    pid_t child_pid;

    pid_t pid[10] = {0}; /* 存放每个子进程的id */

    if (argc > 1)

    {

        /* 命令行参数中的第一个参数表示建立几个子进程，最多10个 */

        child_proc_number = atoi(argv[1]);

        child_proc_number

            = (child_proc_number > 10) ? 10 : child_proc_number;
    }

    for (i = 0; i < child_proc_number; i++)

    {

        child_pid = fork();

        if (child_pid > 0)

        {

            pid[i] = child_pid;
        }

        else if (child_pid == 0)

        {

            proc_number = i;

            do_something();
        }

        else

        {

            perror("fail to fork!\n");
        }

        /* 在这里填写代码，建立child_proc_number个子进程  

        * 子进程要执行       

        *    proc_number = i;  

        *    do_something();  

        * 父进程把子进程的id保存到pid[i] */
    }

    /* 让用户选择杀死哪个进程。输入数字(自编号)表示杀死该进程  

    * 输入q退出 */

    while ((ch = getchar()) != 'q')

    {

        if (isdigit(ch))

        {

            kill(pid[ch - '0'], SIGTERM);

            /* 在这里填写代码，向pid[ch-'0']发信号SIGTERM，  

            * 杀死该子进程 */
        }
    }

    /* 在这里填写代码，杀死本组的所有进程 */

    kill(0, SIGTERM);

    return 0;
}

void do_something()

{

    for (;;)

    {

        printf("This is process No.%d and its pid is %d\n", proc_number, getpid());

        sleep(2); /* 主动阻塞两秒钟 */
    }
}
// 实验结果与预期结不相同，产生的子进程数最多为10个，当产生小于10个， 则产生10个，大于10个，直接产生10个，随机输出0~9号进程，循环输 出。

// 输入数字键回车后，杀死该数字所对应的进程，输入q回车后，杀死所有进程，退出程序。


// 3. proc_number 这个全局变量在各个子进程里的值相同吗？为什么？
 

// 答，相同，子进程的资源相互独立，互不影响。
 

// 4. kill 命令在程序中使用了几次？每次的作用是什么？执行后的现象是什么？
// 答：kill命令在程序中使用了2次，kill(pid[ch-'0'],SIGTERM)和kill(0,SIGTERM); 第一次是杀死该进程号pid[ch-‘0’]，输入进程号回车后，执行后接下来的结果中不会有该进程号，

// 第二次是杀死本组所有进程。即主进程以及它创建的所有子进程。执行后程序退出，进程结束。用另一个终端打开，使用命令ps -a，查看终端下运行的进程。
 

// 5. 使用kill 命令可以在进程的外部杀死进程。进程怎样能主动退出？这两种退出方式哪种更好一些？
 

// 答：进程在main函数中return，或调用exit()函数都可以正常退出，而使用kill命令则是异常退出。正常退出比较好，若在子进程退出前使用kill命令杀死其父进程，则系统会让init进程接管子进程。当用kill命令使得子进程先于父进程退出时，而父进程又没有调用wait函数等待子进程结束，子进程处于僵死状态，并且会一直保持下去，直到系统重启。子进程处于僵死状态时，内核只保存该进程的必要信息以被父进程所需，此时子进程始终占着资源，同时减少了系统可以创建的最大进程数，使用return正常退出更好一些
