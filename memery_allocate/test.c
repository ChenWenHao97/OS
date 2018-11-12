#include "buddy.h"
#include <stdio.h>
void display(void);
int main()
{
  int num;
  int arg;
  struct buddy2 *buddy = buddy2_new(32);
  buddy2_dump(buddy);
  display();
  for (;;)
  {
    scanf("%d %d", &num, &arg);
    if (num == 1)
    {
      int index = 0;
      if ((index = buddy2_alloc(buddy, arg)) != -1)
        printf("分配的起始下标:%d\n", index);
      else
      {
        printf("内存不足!\n");
      }
      buddy2_dump(buddy);
    }
    else if (num == 2)
    {
      buddy2_free(buddy, arg);
      buddy2_dump(buddy);
    }
    else if (num == 3)
    {
      printf("size: %d\n", buddy2_size(buddy, arg));
      buddy2_dump(buddy);
    }
    else
      buddy2_dump(buddy);
    printf("\n\n");
    display();
  }
  
}
void display(void)
{
  printf("1.alloc\n");
  printf("2.free\n");
  printf("3.size\n");
}