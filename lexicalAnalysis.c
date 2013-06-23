#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 255   //设置字符串的最长的长度
#define $ID 1         //已插入符号表的返回值
#define $code 2        //返回该值表示是保留字
#define $ASSIGN 3     //返回该值表示是一个 =
#define $PLUS 4       //返回该值表示是一个 +
#define $POWER 5      //返回该值表示是一个 非*
#define $STAR 6      //返回该值表示是一个 *
#define $SEMICOLON 7  //返回该值表示是一个 ;
#define $LPAR 8       //返回该值表示是一个 (
#define $RPAR 9       //返回该值表示是一个 )
#define $LBRACE 10    //返回该值表示是一个 {
#define $RBRACE 11    //返回该值表示是一个 }
#define $INT 12       //返回该值表示是一个int型


char ch;                      //字符变量，存放最新读进的源程序字符
char strToken[MAXSIZE];       //字符数组，存放构成单词符号的字符串
FILE *fp;                     //读取源文件内容的指针
FILE *fReserve;               //读取保留字表文件的指针
FILE *fId;                    //读取含有标识符的文件的指针
FILE *fConst;                 //读取含有常数表文件的指针 
int index1;                    //字符数组的下标


int judgeString();   //判断当前读入的字符串
void GetChar();   //读入一个字符到ch中，搜索指示器前移 
void GetBC();     //检查ch是否为一个空白。若是，则调用GetChar()直至ch中读入一个非空字符
void Concat();    //链接字符串，对strToken操作
int IsLetter();  //是否是字母
int IsDigit();   //是否是数字
int Reserve();    //是否是保留字
void Retract();   //搜索指示器回调，ch置空
int InsertId();  //插入符号栈
int InsertConst();      //插入常数栈
void ProcError();          //错误处理


int main()
{
	char fidName[MAXSIZE];
	char filename[MAXSIZE];
	char fconstName[MAXSIZE];
	printf("请输入要分析的源文件的文件名：");
	gets(filename);
	fp = fopen(filename,"rt");
	if(fp == NULL)
	{
		printf("\n打开文件失败，%s可能不存在\n",filename);
		return 0;
	}
	strcpy(fidName,filename);
	strcpy(fconstName,filename);
	strcat(fidName,"id.txt");
	strcat(fconstName,"const.txt");
	fId = fopen(fidName,"at+");
	if(fId == NULL)
	{
		printf("无法操作标识符文件\n");
		return 0;
	}
	fConst = fopen(fconstName,"at+");
	if(fConst == NULL)
	{
		printf("无法操作常量文件!\n");
		return 0;
	}
	while(fgetc(fp) != -1)   //从文件中取中一个字符 判断是否读取文件结束了
	{
		Retract();           //读取文件指针前移一位
		judgeString();       

	}
	fclose(fp);
	fclose(fId);
	fclose(fConst);
	return 1;
}

int judgeString()
{
	int code;
	int value;
	memset(strToken,0,sizeof(strToken));
	index1 = 0;
	GetChar();
	GetBC();
	if(IsLetter()== 1)
	{
		while(IsLetter() == 1|| IsDigit()== 1)
		{
			Concat();
			GetChar();
		}
		Retract();
		code=Reserve();
		if(code == 0)
		{
			value = InsertId(strToken);
			return $ID;
		}
		else{
			return code;
		}
	}else if(IsDigit() == 1){
		while(IsDigit() == 1){
			Concat();
			GetChar();
		}
		Retract();
		value = InsertConst(strToken);
		return $INT;
	}else if(ch == '='){
		return $ASSIGN;
	}else if(ch == '+'){
		return $PLUS;
	}else if(ch == '*'){
		GetChar();
		if(ch = '*') return $POWER;
		Retract();
		return $STAR;
	}else if(ch == ';'){
		return $SEMICOLON;
	}else if(ch =='('){
		return $LPAR;
	}else if(ch == '{'){
		return $LBRACE;
	}else if(ch == '}'){
		return $RBRACE;
	}else
		ProcError();
}


/*
 *读取一个字符，指示器前移
 * */
void GetChar()
{
	ch = fgetc(fp);
//	printf("%c",ch);
}

/*
 *检查ch是否为空，为空就调用GetChar()，直到取到非空字符
 * */
void GetBC()
{
	while(ch == ' ')
		GetChar();
}

/*
 *连接字符
 * */
void Concat()
{
	strToken[index1] = ch;
	index1++;
}	

/*
 *判断是否是字母
 * */
int IsLetter()
{
	if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		return 1;
	else
		return 0;
}

/*
 *判断是否是数字
 * */
int IsDigit()
{
	if(ch >= '0' && ch <='9')
		return 1;
	else
		return 0;
}


int Reserve()
{
	char data[MAXSIZE];
	fReserve = fopen("Reserve.txt","rt");
	if(fReserve == NULL)
	{
		printf("打开文件失败!!!!!!!!!!!!!!！\n");
		return -1;
	}
	while(fscanf(fReserve,"%s ",data)!=EOF)
	{
	//	printf("%s\n",data);
		if(strcmp(strToken,data) == 0)
		{
			printf("%s是保留字\n",data);
			return -2;
		}
	}
	return 0;
}

void Retract()
{
	fseek(fp,-1,SEEK_CUR);
	ch = ' ';
}

int InsertId()
{
	fprintf(fId,"%s\n",strToken);
	return 1;
}

int InsertConst()
{
	fprintf(fConst,"%s\n",strToken);
	return 1;
}

void ProcError()
{
	printf("%c是未知词\n",ch);
}

