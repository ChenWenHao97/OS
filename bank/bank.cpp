#include <iostream>
using namespace std;
class Bank
{
  public:
    int available[100];      //可利用资源
    int max[50][100];        //最大分配数组
    int need[50][100];       //需求的数组
    int allocation[50][100]; //分配矩阵
    int request[50][100];    //需求的数组
    bool finish[50];         //完成分配的数组
    int sequence[50];        //安全的序列
    int safe(void);
    void init(void);
    void input(void);

  private:
    int process;
    int kind;
};
int Bank::safe(void)
{
    int l = 0;
    int work[100]; //可利用资源数组,为了保存原来状态

    for (int i = 0; i < kind; i++)
        work[i] = available[i];

    for (int i = 0; i < process; i++)
        finish[i] = false;

    for (int i = 0; i < process; i++)
    {
        if (finish[i])
            continue;
        else
        {
            int j = 0;
            for (j = 0; j < kind; j++)
            {
                if (need[i][j] > work[j]) //需求大于剩余的,错误
                    break;
            }
            if (j == kind) //每种资源都分配了一遍
            {
                finish[i] = true;
                for (int k = 0; k < kind; k++) //每次彻底分配完一个进程之后，进行回收
                    work[k] += allocation[i][k];
                sequence[l++] = i;
                i = -1; //分配资源不是按顺序的，所有要重新从第一个开始
            }
            else
                continue;
        }
        if (l == process) //证明进程都分配完了
        {
            cout << "系统是安全的" << endl;
            cout << "系统安全序列是:" << endl;
            for (i = 0; i < l; i++)
            {
                cout << sequence[i];
                if (i != l - 1)
                    cout << "->";
            }
            cout << endl;
            return 1;
        }
    }
    return 0;
}
void Bank::init(void)
{
    cout << "输入进程的数目:" << endl;
    cin >> process;
    cout << "输入资源的种类:" << endl;
    cin >> kind;
    cout << "输入每个进程最多所需的各类资源数,按照" << process << "x" << kind << "矩阵输入\n";
    for (int i = 0; i < process; i++)
        for (int j = 0; j < kind; j++)
            cin >> max[i][j];
    cout << "输入每个进程已经分配的各类资源数,按照" << process << "x" << kind << "矩阵输入\n";
    for (int i = 0; i < process; i++)
    {
        for (int j = 0; j < kind; j++)
        {
            cin >> allocation[i][j];
            need[i][j] = max[i][j] - allocation[i][j];
            if (need[i][j] < 0)
            {
                cout << "你输入的第" << i + 1 << "个进程所拥有的第" << j + 1 << "个资源错误，请重新输入:\n";
                j--;
                continue;
            }
        }
    }
    cout << "请输入各个资源现有的数目:\n";
    for (int i = 0; i < kind; i++)
        cin >> available[i];
}
void Bank::input(void)
{
    while (true)
    {
        int num;
        cout << "输入要申请的资源的进程号:" << endl;
        cin >> num;

        cout << "输入进程所请求的各个资源的数量:" << endl;
        for (int i = 0; i < kind; i++)
            cin >> request[num][i];

        for (int i = 0; i < kind; i++)
        {

            if (request[num][i] > need[num][i])
            {
                cout << "所请求资源数超过进程的需求量！" << endl;
                return;
            }
            if (request[num][i] > available[i])
            {
                cout << "所请求资源数超过系统所有的资源数！" << endl;
                return;
            }
        }
        for (int i = 0; i < kind; i++)
        {
            available[i] -= request[num][i];
            allocation[num][i] += request[num][i];
            need[num][i] -= request[num][i];
        }
        if (safe())
            cout << "同意分配请求\n";
        else
        {
            cout << "请求被拒绝\n";
            for (int i = 0; i < kind; i++)
            {
                available[i] += request[num][i];
                allocation[num][i] -= request[num][i];
                need[num][i] += request[num][i];
            }
        }
        for (int i = 0; i < process; i++)
            finish[i] = false;

        char flag; //标志位
        cout << "是否再次请求分配？是请按Y/y，否请按N/n";
        while (true)
        {
            cin >> flag;
            if (flag == 'Y' || flag == 'y' || flag == 'N' || flag == 'n')
                break;
            else
            {
                cout << "请按要求重新输入:\n";
                continue;
            }
        }
        if (flag == 'Y' || flag == 'y')
            continue;
        else
            break;
    }
}
int main()
{
    Bank bank;
    bank.init();
    bank.safe();
    bank.input();
}