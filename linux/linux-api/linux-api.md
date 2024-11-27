### bsearch()
```
void *bsearch(const void *key, const void *base, size_t nmem, size_t size, int (*comp)(const void *, const void *));
```
**参数**
 - key指向所要查找的元素
 - base指向进行查找的数组
 - nmem为查找长度，一般为数组长度
 - size为每个元素所占的字节数，一般用sizeof(...)表示
 - comp指向比较子函数，它定义比较的规则。

**返回值**
 - 如果查找成功则返回数组中匹配元素的地址，反之则返回空。

####例子
```
#include <stdio.h>
#include <stdlib.h>
#define NUM 8
int compare(const void *p, const void *q){
    return (*(int *)p - *(int *)q);
}

int main(int argc, char *argv[]){
    int array[NUM] = {9, 2, 7, 11, 3, 87, 34, 6};
    int key = 3;
    int *p;
    qsort(array, NUM, sizeof(int), compare);
    p = (int *)bsearch(&key, array, NUM, sizeof(int), compare);
    (p == NULL) ? puts("not found") : printf("found:%d",*p);
    return 0;
}
```
