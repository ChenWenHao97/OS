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
      printf("allocated:%d\n", buddy2_alloc(buddy, arg));
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
  }
}
void display(void)
{
  printf("1.alloc\n");
  printf("2.free\n");
  printf("3.size\n");
}