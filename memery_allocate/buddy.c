#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct buddy2
{
  unsigned size;
  unsigned arr[1];
};

#define LEFT_LEAF(index) ((index)*2 + 1)
#define RIGHT_LEAF(index) ((index)*2 + 2)
#define PARENT(index) (((index) + 1) / 2 - 1)

#define IS_POWER_OF_2(x) (!((x) & ((x)-1)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define ALLOC malloc
#define FREE free

static unsigned fixsize(unsigned size)//使之变为2的幂,也可以一位一位移动，但是成倍的移动速率更快
{
  size |= size >> 1;
  size |= size >> 2;
  size |= size >> 4;
  size |= size >> 8;
  size |= size >> 16;
  return size + 1;
}

struct buddy2 *buddy2_new(int size)
{
  struct buddy2 *self;
  unsigned node_size;
  int i;

  if (size < 1 || !IS_POWER_OF_2(size))
    return NULL;

  self = (struct buddy2 *)ALLOC(2 * size * sizeof(unsigned));
  self->size = size;
  node_size = size * 2;

  for (i = 0; i < 2 * size - 1; ++i)//写如每个节点之后节点的个数
  {
    if (IS_POWER_OF_2(i + 1))//每块的大小不断减少
      node_size /= 2;
    self->arr[i] = node_size;
  }
  return self;
}

void buddy2_destroy(struct buddy2 *self)`
{
  FREE(self);
}

int buddy2_alloc(struct buddy2 *self, int size)
{
  unsigned index = 0;
  unsigned node_size;
  unsigned offset = 0;

  if (self == NULL)
    return -1;

  if (size <= 0)
    size = 1;
  else if (!IS_POWER_OF_2(size))//如果分配的内存不是2的幂函，就进行重新分配
    size = fixsize(size);

  if (self->arr[index] < size)//超出最大分配单元
    return -1;

  for (node_size = self->size; node_size != size; node_size /= 2)
  {
    if (self->arr[LEFT_LEAF(index)] >= size)
      index = LEFT_LEAF(index);
    else
      index = RIGHT_LEAF(index);
  }

  self->arr[index] = 0;//分配出来适当的块
  offset = (index + 1) * node_size - self->size;

  while (index)
  {
    index = PARENT(index);
    self->arr[index] =
        MAX(self->arr[LEFT_LEAF(index)], self->arr[RIGHT_LEAF(index)]);
  }

  return offset;
}

void buddy2_free(struct buddy2 *self, int offset)
{
  unsigned node_size, index = 0;
  unsigned left_arr, right_arr;

  assert(self && offset >= 0 && offset < self->size);

  node_size = 1;
  index = offset + self->size - 1;//找到当前位置的偏移地址

  for (; self->arr[index]; index = PARENT(index))//找到父节点，然后进行整合
  {
    node_size *= 2;
    if (index == 0)
      return;
  }

  self->arr[index] = node_size;

  while (index)
  {
    index = PARENT(index);
    node_size *= 2;

    left_arr = self->arr[LEFT_LEAF(index)];
    right_arr = self->arr[RIGHT_LEAF(index)];

    if (left_arr + right_arr == node_size)
      self->arr[index] = node_size;
    else
      self->arr[index] = MAX(left_arr, right_arr);
  }
}

int buddy2_size(struct buddy2 *self, int offset)//获得块的大小
{
  unsigned node_size, index = 0;

  assert(self && offset >= 0 && offset < self->size);
  printf("分配块的大小:%d\n",self->size);
  node_size = 1;
  for (index = offset + self->size - 1; self->arr[index]; index = PARENT(index))
    node_size *= 2;

  return node_size;
}

void buddy2_dump(struct buddy2 *self)
{
  char result[100];
  int i, j;
  unsigned node_size, offset;

  if (self == NULL)
  {
    printf("分配为空!");
    return;
  }

  if (self->size > 64)
  {
    printf("树过于大！");
    return;
  }

  memset(result, '-', sizeof(result));
  node_size = self->size * 2;

  for (i = 0; i < 2 * self->size - 1; ++i)
  {
    if (IS_POWER_OF_2(i + 1))//树的高度不断地减少
      node_size /= 2;

    if (self->arr[i] == 0)//本块被分配完
    {

      if (i >= self->size - 1)//没有子节点，直接字符替换
      {
        result[i - self->size + 1] = '*';
      }
      else if (self->arr[LEFT_LEAF(i)] && self->arr[RIGHT_LEAF(i)])
      {
        offset = (i + 1) * node_size - self->size;

        for (j = offset; j < offset + node_size; ++j)
          result[j] = '*';
      }
      
    }
  }
  result[self->size] = '\0';
  puts(result);
}