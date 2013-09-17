/*
 * description: This program is test how to use
 * the list.h
 *
 * author:chenshuanglin
 * Date:2013/9/17
 */

#include <stdio.h>
#include <stdlib.h>

#define N 10                              //链表的节点数


/*
 * 下面这些代码除了结构体numlist不是使用inux/list.h中的内容
 * 其他都是复制粘帖过来的
 *
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)


#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member))) 

struct list_head{
	struct list_head *next,*prev;
};

typedef struct numlist{
	int num;                      //数据
	struct list_head list;           //指向双链表前后结点的指针
}NumNode;

void __list_add(struct list_head *new,  struct list_head *prev, struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
 }


 void INIT_LIST_HEAD(struct list_head *list)
 {
	 list->next = list;
	 list->prev = list;
 }


void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/*
 * author:chenshuanglin
 * description:This function is create a doubleList
 *
 */
void createList(NumNode * head)
{
	int i;
	NumNode * newNode;             //每次申请链表结点时所使用的指针
	printf("Please input 10 number: ");
	for(i = 0 ; i < N ; i++)
	{
		newNode = (NumNode *)malloc(sizeof(NumNode));
		scanf("%d",&newNode->num);
		list_add_tail(&newNode->list,&head->list);      //调用list.h中的添加节点的函数list_add_tail
	}	  
}

/*
 * author:chenshuanglin
 * description: Print this doubleList
 */
void printDoubleList(NumNode *head)
{
	int	i=1;
	struct list_head * pos;
	NumNode * num;
	list_for_each(pos,&head->list)                              //调用list.h中的list_for_each函数进行遍历
	{
		num = list_entry(pos,NumNode,list);                     //调用list_entry函数得到相对应的节点
		printf("Node %d's data: %d\n", i , num->num);
		i++;
	}
}

/*
 * test Main()
 */
int main(void)
{
	NumNode head;
	INIT_LIST_HEAD(&head.list);
	createList(&head);
	printDoubleList(&head);
	return 1;
}
