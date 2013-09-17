/*
 * description: This program is test how to use
 * the list.h
 *
 * author:chenshuanglin
 * Date:2013/9/17
 */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#define N 10                              //链表节点数

typedef struct numlist{
	int num;                      //数据
	struct list_head list;           //指向双链表前后结点的指针
}NumNode;

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
 * author:chenshuanglin
 * description: delete number we want
 *
 */
void deleteNum(NumNode * head,int number)
{
	struct list_head *pos,*n;
	NumNode *p;
	int i=1;
	int judge = 0;
	list_for_each_safe(pos,n,&head->list)
	{
		p = list_entry(pos,NumNode,list);
		if(p->num == number)
		{
			judge = 1 ;
			list_del(pos);
			free(p);
			printf("this node %d has removed from the doublelist...\n",i);
		}
		i++;
	}
	if(judge == 0 )
	{
		printf("NO FOUND!\n");
	}

}
/*
 * test Main()
 */
int main(void)
{
	NumNode head;
	int number;
	INIT_LIST_HEAD(&head.list);
	createList(&head);
	printDoubleList(&head);
	printf("Please input the number you want to delete: ");
	scanf("%d",&number);
	deleteNum(&head,number);
	printDoubleList(&head);
	return 1;
}
