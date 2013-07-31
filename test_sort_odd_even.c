#include <stdio.h>

/*
 *该程序是调整数组的左右两部分，左边的元素均为奇数
 *右边的元素均为偶数
 *@author:lin
 * */

int swap(int *a,int *b)
{
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

int AdjustSqlit(int *a,int length)
{
	int left,right;           //定义一个变量left，从左往右搜索数组，定义一个变量从右往左搜索数组
	left = 0 ;
	right = length-1;   
	while(left<right)
	{
		while(a[left]%2 != 0)
		{
			left++;
		}
		while(a[right]%2 == 0)
		{
			right--;
		}
		if(left<right)
		{
			swap(&a[left],&a[right]);
		}
	}
}

void main()
{
	int i; 
	int a[]={7,4,5,2,9,8};
	AdjustSqlit(a,6);
	for(i = 0 ; i < 6 ; i++)
	{
		printf("%d ",a[i]);
	}
	printf("\n");
}
