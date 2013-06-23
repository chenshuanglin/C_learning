#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

#define MAXSIZE 80;

void do_ls(char[]);
void dostat(char *);
void show_file_info(char *,struct stat *);
void mode_to_letters(int ,char []);
char * uid_to_name(uid_t);
char * gid_to_name(gid_t);

int screen_len = MAXSIZE;
int max;
int print_a = 0 ; //默认没有-a命令参数
int print_R = 0 ; //默认没有-R命令参数
int print_u = 0 ; //默认没有-u的命令参数
int print_l = 0 ; //默认没有-l的命令参数

main(int ac ,char *av[])
{
	char filename[255];
	strcpy(filename,"./");
	if(ac == 1)
		do_ls("./");
	else
	{
		while(--ac)
		{
			if((*++av)[0] == '-')
			{
				switch((*av)[1])
				{
					case 'l':
						print_l = 1;
						break;
					case 'a':
						print_a = 1;
						break;
					case 'u':
						print_u = 1;
						break;
					case 'R':
						print_R = 1;
						break;
					default:
						printf("参数不正确!!\n");
						return;
				}
			}
			else
				strcpy(filename,*av);
		}
		do_ls(filename);
	}
}

static int cmpstringp(const void *p1, const void *p2)
{
		    return strcmp((char * )p1, (char *)p2);
}

/*按最大的文件名决定排的列数*/
void display_single(char *name)
{
	int length;
	int i,len;
	if(screen_len < max)
	{
		printf("\n");
		screen_len = MAXSIZE;
	}

	length = strlen(name);
	len = max - length; //打印的空格数
	printf("%-s",name);
	/*为了适应汉字的情况下做的打印的调整*/
	for(i = 0 ; i < length ; i++)
	{
		if(name[i] < 0) //判断是否为汉字，当为汉字的时候多打印一个空格
		{
			len+=1;
			i = i+2;
		}
	}
	for(i = 0 ; i  < len ; i++)
	{
		printf(" ");
	}
	printf("  ");
	screen_len -= (max+2);
}

/*打印ls -a的内容*/
void print_ls(char dirname[][255],int number)
{
	int list_number;
	max = strlen(dirname[0]);
	for(list_number = 0 ; list_number < number ; list_number++)
	{
		if(strlen(dirname[list_number]) > max)
		max = strlen(dirname[list_number]);
	}
	for(list_number = 0 ; list_number < number ; list_number++)
	{
		display_single(dirname[list_number]);
	}
	printf("\n");
							
}

void print_ls_l(char dirname[][255],int number,char filename[])
{
	char file_name[512];
	int list_number;

	for(list_number = 0 ; list_number < number ; list_number++)
	{
		strcpy(file_name,filename);
		strcat(file_name,dirname[list_number]);
//		printf("%s\n", file_name);
		dostat(file_name);
	}
}

void do_ls(char dirname[])
{
	int i,j;
	DIR * dir_ptr;
	char dir_name[255][255];
	char dir_name_R[255][255];
	struct dirent *direntp;
	struct stat info;
	char filename[512];
	char filename1[255];
	i = 0;
	j = 0;
	if(strcmp(dirname,"./") != 0)
	{
		strcat(dirname,"/");  //得到当前目录的绝对路径
	}
	if((dir_ptr = opendir(dirname)) == NULL)
		fprintf(stderr,"ls3: cannot open %s\n",dirname);
	else
	{
//		strcpy(filename,dirname);
//		printf("--->%s\n",filename);     
		while((direntp = readdir(dir_ptr))!=NULL) {
			strcpy(filename,dirname);
			strcat(filename,direntp->d_name);
//			printf("--->%s\n",filename);
			if((direntp->d_name[0]-'.')== 0){
				if(print_a == 1)
				{
					if(strcmp(direntp->d_name,".") != 0 &strcmp(direntp->d_name,"..")!= 0 )
					{
						if(stat(filename,&info)!=-1)
						{
							if(S_ISDIR(info.st_mode))
							{
//								printf("wo men yi jing jin ru!!!\n");
								strcpy(dir_name_R[j++],direntp->d_name);
							}
						}
					}
					strcpy(dir_name[i++], direntp->d_name);
				}
			}
			else
			{
//				printf(" wo shi \n");
				if(stat(filename,&info) != -1)
				{
					if(S_ISDIR(info.st_mode))
				   	{
//						printf("wo men yijing jin ru!!!!\n");
//						printf("--->%s\n",direntp->d_name);
					   	strcpy(dir_name_R[j++],direntp->d_name);
					}
	   			}
				strcpy(dir_name[i++],direntp->d_name);
			}
		}
		closedir(dir_ptr);
	}
	qsort(&dir_name[0], i-1, sizeof(dir_name[1]), cmpstringp);
	if(print_l == 1){
		print_ls_l(dir_name,i,dirname);
	}
	else
		print_ls(dir_name,i);
	if(print_R == 1)
	{
//		strcpy(filename1,dirname);
		for(i = 0 ; i < j ; i++)
		{
			strcpy(filename1,dirname);
			strcat(filename1,dir_name_R[i]);
			printf("\n%s:\n",filename1);
			do_ls(filename1);
		}
	}
}
/*截取文件的绝对路径得到相对路径*/
char * strsub(char *filename){
	int i;
	int judge;
	char *str;
	for(i = 0 ; filename[i] != '\0' ; i++)
	{
		if(filename[i] == '/')
			judge = i;
	}
	str = &filename[judge+1];
}

void dostat(char *filename)
{
	struct stat info;
	if(stat(filename,&info) == -1)
		perror(filename);
	else
		show_file_info(filename,&info);
}

void show_file_info(char * filename, struct stat * info_p)
{
	char *uid_to_name(),*ctime(),*gid_to_name(),*filemode();
	void mode_to_letters();
	char modestr[11];
	mode_to_letters(info_p->st_mode,modestr);
	printf("%s",modestr);
	printf("%4d",(int)info_p->st_nlink);
	printf("%8s",uid_to_name(info_p->st_uid));
	printf("%8s",gid_to_name(info_p->st_gid));
	printf("%8ld",(long)info_p->st_size);
	printf("\t");
	if(print_u == 1)
		printf("%.12s",4+ctime(&info_p->st_atime));
	else
		printf("%.12s",4+ctime(&info_p->st_mtime));
	printf("\t");
//	printf("%s\n",filename);
	printf("%s\n",strsub(filename));
}

void mode_to_letters(int mode,char str[])
{
	strcpy(str,"----------");
	if(S_ISDIR(mode)) str[0] = 'd'; //directory
	if(S_ISCHR(mode)) str[0] = 'c'; //char devices
	if(S_ISBLK(mode)) str[0] = 'b'; //block device

	if(mode & S_IRUSR) str[1] = 'r'; /*3 bits for user*/
	if(mode & S_IWUSR) str[2] = 'w';
	if(mode & S_IXUSR){
		str[3] = 'x';
		if(mode & S_ISUID)
			str[3] = 's';
	}
	
	if(mode & S_IRGRP) str[4] = 'r';/*3 bits for group*/
	if(mode & S_IWGRP) str[5] = 'w';
	if(mode & S_IXGRP){
		str[6] = 'x';
		if(mode & S_ISGID)
			str[6] = 's';
	}

	if(mode & S_IROTH) str[7] = 'r'; /*3 bits for other*/
	if(mode & S_IWOTH) str[8] = 'w';
	if(mode & S_IXOTH) {
		str[9] = 'x';
		if(mode & S_ISVTX)
			str[9] = 't';
	}
}

char * uid_to_name(uid_t uid)
{
	struct passwd * getpwuid(),*pw_ptr;
	static char numstr[10];

	if((pw_ptr = getpwuid(uid)) == NULL){
		sprintf(numstr,"%d",uid);
		return numstr;
	}
	else
		return pw_ptr->pw_name;
}

char *gid_to_name(gid_t gid)
{
	struct group * getgrpid(),*grp_ptr;
	static char numstr[10];
	if((grp_ptr = getgrgid(gid)) == NULL){
		sprintf(numstr,"%d",gid);
		return numstr;
	}
	else
		return grp_ptr->gr_name;
}
