#include <stdio.h>
#include <stdlib.h>

#define NUM 8

int compare(const void *p, const void *q)
{
    return (*(int *)p - *(int *)q);
}

int main(int argc, char *argv[])
{
    int array[NUM] = {9, 2, 7, 11, 3, 87, 34, 6};
    int key = 3;
    int *p;

    
    qsort(array, NUM, sizeof(int), compare);
    p = (int *)bsearch(&key, array, NUM, sizeof(int), compare);

    (p == NULL) ? puts("not found") : printf("found:%d\n",*p);

    return 0;
}