#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 100
#define OK 1
#define ERROR 0

typedef struct{
	int elem[MAXSIZE];               //线性表占用的数组空间
	int last;               //记录线性表中最后一个元素在数组elem[]中的位置，空表置为1
}SeqList;

void init(SeqList *L);                 //初始化线性顺序表
int Locate(SeqList L,int find_number); //查找线性顺序表
int InsList(SeqList *L,int insert_number);      //对线性顺序表插入数据
int delList(SeqList * L , int delete_number);   //删除线性顺序表中的数据
void Merge(SeqList * LA, SeqList *LB ,SeqList *LC);       //合并线性顺序表
void create(SeqList * L,int number);           //创建线性顺序表
void print_SeqList(SeqList L);              //打印表

void main()
{
	printf("######################练习的小程序#########################\n");
	SeqList LA,LB,LC;
	init(&LA);init(&LB);init(&LC);
	printf("构造线性表LA：\n");
	create(&LA,3);	
	printf("构造线性表LB：\n");
	create(&LB,4);
	int select;
	while(1){
		printf("————————————————————————————————————————————————————————————\n");
		printf("其中的查找、插入、删除是针对表LB的操作，合并是针对LA，LB的操作\n");
		printf("\n");
		printf("------------------1.查找-----------------------------------\n");
		printf("------------------2.插入-----------------------------------\n");
		printf("------------------3.删除-----------------------------------\n");
		printf("------------------4.合并-----------------------------------\n");
		printf("------------------5.退出-----------------------------------\n");
		printf("\n");
		printf("————————————————————————————————————————————————————————————\n");
		printf("请输入你要进行的操作: ");
		scanf("%d",&select);
		switch(select){
				int find_number;
				int insert_number;
				int del_number;
			case 1:
				printf("请输入要查找的数： ");
				scanf("%d",&find_number);
				printf("查找数的位置是：%d",Locate(LB,find_number));
				break;
			case 2:
				printf("请输入要插入的数：");
				scanf("%d",&insert_number);
				InsList(&LB,insert_number);
				printf("插入后数组的打印：\n");
				print_SeqList(LB);
				break;
			case 3:
				printf("请输入要删除的数：");
				scanf("%d",&del_number);
				delList(&LB , del_number);
				printf("删除后数组的打印：\n");
				print_SeqList(LB);
				break;
			case 4:
				Merge(&LA, &LB ,&LC);
				printf("合并后的数组打印如下：\n");
				print_SeqList(LC);
				break;
			case 5:
				exit(1);
		}
	}

}

/*初始化操作*/
void init(SeqList * L)
{
	L->last = -1 ;
}

//打印线性表
void print_SeqList(SeqList L)
{
	int i;
	for(i = 0 ; i <= L.last ; i++)
	{
		printf("%d ",L.elem[i]);
	}
	printf("\n");
}

//查找操作
int Locate(SeqList L , int find_number)
{
	int i = 0 ; 
	while((i <= L.last)&&(L.elem[i] != find_number))
	  i++;
	if(i<=L.last)
	  return(i+1);
	else
	  return(-1);
}

/*以下是针对顺序表的插入操作*/
int InsList(SeqList * L ,int insert_number)
{
	int k;
	int insert_index = 0 ;  
	while((insert_index <= L->last)&& (L->elem[insert_index]<=insert_number))
	{
		insert_index++;
	}
	if(L->last >= MAXSIZE-1)
	{
		printf("表已经满了不能插入数据！\n");
		return ERROR;
	}
	for(k = L->last ; k>=insert_index ; k--)
	{
		L->elem[k+1] = L->elem[k];
	}
	L->elem[insert_index] = insert_number;
	L->last++;
	return OK;
}

/*以下是线性表的删除操作*/
int delList(SeqList * L , int delete_number)
{
	int k;
	int delete_index = 0 ;
	while((delete_index <= L->last) && (L->elem[delete_index] != delete_number))
	{
		delete_index++;
	}
	if(delete_index>L->last){
		printf("你要删除的数据并不存在\n");
		return ERROR;
	}
	for(k = delete_index ;  k <L->last ; k++)
	{
		L->elem[k] = L->elem[k+1];
	}
	L->last--;
	return OK;
}

/*线性表的合并运算*/
void Merge(SeqList * LA, SeqList *LB ,SeqList *LC)
{
	int i , j , k;
	i = 0 ;                       //LA表的下标
	j = 0 ;                       //LB表的下标
	k = 0 ;                      //LC表的下标
	while(i <= LA->last && j <= LB->last)
	{
		if(LA->elem[i] <= LB->elem[j])
		{
			LC->elem[k] = LA->elem[i];
			i++;
			k++;
		}
		else
		{
			LC->elem[k] = LB->elem[j];
			j++;
			k++;
		}
	}
	while(i<=LA->last)               //当表A还有剩余时，直接加入到C线性表中
	{
		LC->elem[k] = LA->elem[i];
		i++;
		k++;
	}
	while(j <= LB->last)                 //当表B还有剩余时，直接加入到C线性表中
	{
		LC->elem[k] = LB->elem[j];
		j++;
		k++;
	}
	LC->last = LA->last + LB->last+1;
}

//顺序线性表的构造
void create(SeqList * L,int number)
{
	int i;
	int data;
	printf("输入要存入的数据：");
	for(i = 0 ; i < number ; i++)
	{
		scanf("%d",&L->elem[i]);
	}
	L->last = number-1;
}

