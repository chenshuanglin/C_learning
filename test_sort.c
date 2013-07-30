#include <stdio.h>

void sort(int a[],int length)
{
	int i = 0 , j,temp;
	int change;
	do{
		change = 0;
		for(j = 0; j<length-i; j++)
		{
			if(a[j]>a[j+1])
			{
				temp=a[j];
				a[j] = a[j+1];
				a[j+1] = temp;
				change = 1;
			}
		}
		i++;
	}while(i<length && change==1);
}

int main()
{
	int i=0;
	int a[5] = {4,7,8,3,1};
	sort(a,5);
	for(; i < 5 ; i++)
	{
		printf("%d ",a[i]);
	}
	printf("\n");
	return 0;
}
