#include"queue.h"
#include<malloc.h>
#include<stdio.h>
 
/*
#define TAILQ_ENTRY(type)						
struct {								
	struct type *tqe_next;	// next element 
	struct type **tqe_prev;	// address of previous next element
}
#define TAILQ_HEAD(name, type)						
struct name {								
	struct type *tqh_first;	// first element 			
	struct type **tqh_last;	// addr of last next element 		
}
*/
//声明节点类型
struct item_t
{
   int value;
   TAILQ_ENTRY(item_t) entry;
};
 
//声明头节点
TAILQ_HEAD(item_h,item_t);  //两个指针，一级fist指针，二级next指针
 
int main()
{
   struct item_h queue_head;
   struct item_t *node;
 
   //初始化头节点
   TAILQ_INIT(&queue_head);
/*
#define	TAILQ_INIT(head) do {						
	(head)->tqh_first = NULL;					
	(head)->tqh_last = &(head)->tqh_first;				
} while (0)
*/

   //插入几个节点
   int i;
   for(i=0; i<4; i++)
   {
       node = (struct item_t*)malloc(sizeof(struct item_t));
       node->value = i;
       TAILQ_INSERT_TAIL(&queue_head, node, entry);
/*
#define TAILQ_INSERT_TAIL(head, elm, field) do {			\
	(elm)->field.tqe_next = NULL;					\
	(elm)->field.tqe_prev = (head)->tqh_last;			\
	*(head)->tqh_last = (elm);					\
	(head)->tqh_last = &(elm)->field.tqe_next;			\
} while (0)
*/
   }
   
	//获取第一个元素
    node = TAILQ_FIRST(&queue_head);
    printf("the first entry is %d\n", node->value);
   
 	
	//获取下一个元素
    node = TAILQ_NEXT(node, entry);
    printf("the second entry is %d\n\n", node->value);
    
    //删除第二个元素, 但并没有释放s指向元素的内存
    TAILQ_REMOVE(&queue_head, node, entry);
    free(node);
 
/*
#define TAILQ_FOREACH(var, head, field)					
	for((var) = TAILQ_FIRST(head);				 //#define	TAILQ_FIRST(head)		((head)->tqh_first)	
	    (var) != TAILQ_END(head);					 //#define	TAILQ_END(head)			NULL
	    (var) = TAILQ_NEXT(var, field))    //#define	TAILQ_NEXT(elm, field)		((elm)->field.tqe_next)
*/
   //逐个访问 遍历
   TAILQ_FOREACH(node, &queue_head, entry)
   {
       printf("the value is %d\n",node->value);
   }
   return 0;
}

