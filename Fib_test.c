#include <stdio.h>

/*斐波那契数的递归实现*/

int Fib_test1(int n)
{
	if(n == 0 || n == 1)
	  return n;
	else
	  return Fib_test1(n-1)+Fib_test1(n-2);
}

/*斐波那契数的非递归实现*/

int Fib_test2(int n)
{
	int i;
	int x,y,z;
	if(n == 0 || n==1)
	  return n;
	else
	{
		x=0,y=1;
		for(i = 2 ; i <= n ;i++)
		{
			z = y;
			y = x+y;
			x = z;
		}
		return y;
	}
		
}

void main()
{
	int n;
	printf("请输入N：");
	scanf("%d",&n);
	printf("递归求解结果：%d\n",Fib_test1(n));
	printf("非递归求解结果：%d\n",Fib_test2(n));
}
