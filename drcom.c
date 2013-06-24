#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

//#define NO_ERRLOG 1

typedef struct {
    char    bSignture;
    char    bPacketID;
    short   wLength;
    short   wSeq;
    short   wReserved;
}DRCOM_CODE1,*PDRCOM_CODE1;

typedef struct{
    char    bSignture;
    char    bPacketID;
    short   wLength;
    short   wSeq;
    short   wReserved;
    int     dwCode0;
    int     dwCode1;
}DRCOM_CODE2,*PDRCOM_CODE2;

typedef struct{
    char    szAddress[6];
    char    bDhcp;
    char    bDevType;
    int     dwIpAddr;
    int     dwIpMask;
}NIFS,*PNIFS;

typedef struct{
    char    bSignture;
    char    bPacketID;
    short   wLength;
    short   wSeq;
    int     dwUnk0;
    short   wReserved0;
    int     dwCode1;
    int     dwMode;
    int     dwCode0;
    int     dwChecksum;
    int     dwUnk1;
    NIFS    NetIfs[4];
}DRCOM_CODE3,*PDRCOM_CODE3;

typedef struct{
    char    uninterested_xx;
}DRCOM_CODE4,*PDRCOM_CODE4;

typedef struct{
    char    bSignture;
    char    bPacketID;
    short   wLength;
    short   wSeq;
    short   wCurrentVer;
    short   wRand;
    short   wReserved0;
    int     dwReserved0;
    int     dwKey0;
    int     dwKey1;
    int     dwChecksum;
    int     dwCode1;
    int     dwReserved1;
    int     dwReserved2;
}DRCOM_UPDATEINFO,*PDRCOM_UPDATEINFO;

#define LOGFILE_ERR     "drcom.err"

int log_err(char *fmt, ...)
{
    va_list args;
    struct tm *tm;
    time_t cur_time;
    char timebuf[256];
    char buf[1024];
    int fd_log;

#ifdef NO_ERRLOG
    return 0;
#endif

    va_start(args,fmt);
    vsnprintf(buf,1024,fmt,args);
    va_end(args);

    time(&cur_time);
    tm = localtime(&cur_time);
    sprintf(timebuf,"%04d/%02d/%02d %02d:%02d:%02d ",
        tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);

    fd_log = open(LOGFILE_ERR,O_WRONLY|O_CREAT|O_APPEND,0644);
    if(fd_log)
    {
        write(fd_log,timebuf,strlen(timebuf));
        write(fd_log,buf,strlen(buf));
        close(fd_log);
        return 1;
    }
    return 0;
}

int WaitForSingleSocket(int socket, int usec)
{
    fd_set fds;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(socket,&fds);
    tv.tv_usec = usec;
    tv.tv_sec = 0;
    return select(socket+1,&fds,NULL,NULL,&tv);
}

int ClearRecvBuffer(int socket)
{
    struct sockaddr_in sin;
    struct sockaddr *sa = (struct sockaddr *) &sin;
    int fromlen;
    int recvlen;
    char buf[512];

    while(1)
    {
        fromlen = sizeof(sin);
        recvlen = recvfrom(socket,buf,sizeof(buf),0,sa,&fromlen);
        if(recvlen < 0)
        {
            if(errno == EWOULDBLOCK)
                return 1;
            else
                return 0;
        }
        //throw about it
    }
}

int init_daemon(void)
{
    pid_t pid;
    int i;

    pid = fork();
    if(pid < 0)
    {
        printf("fork failed, what the fuck?!\n");
        exit(0);
    }
    else if(pid != 0)
    {
        exit(0);    //parent exits
    }

    setsid();       //child becomes session leader

    for(i = 0; i < 3; i++)
    {
        close(i);   //close STDIN STDOUT STDERR
    }

    umask(0);       //clear file mode creation mask

    return 0;
}

int main(int argc,char **argv)
{
    char szIfName[IFNAMSIZ];
    char szServerIp[16];
    struct ifreq ifr;
    int hSocket;
    int dwFlags;
    int dwRet;
    struct sockaddr_in sin;
    struct sockaddr *sa = (struct sockaddr *)&sin;
    int sinlen;
    char szBuf[512];
    char bId0,bId1;
    DRCOM_CODE1 c1;
    PDRCOM_CODE2 c2 = (PDRCOM_CODE2)szBuf;
    DRCOM_CODE3 c3;
    int bLogon;
    DRCOM_UPDATEINFO up;
    PDRCOM_UPDATEINFO pup = (PDRCOM_UPDATEINFO)szBuf;
    short wCurrentVer;
    short wRand;
    int dwUpdateKey0,dwUpdateKey1;
    int n,dwChecksum;
    char *ptr_char;
    short *ptr_short;
    int *ptr_int;

    memset(szIfName,0,sizeof(szIfName));
    strcpy(szServerIp,"1.1.1.1");

    if(argc == 1)
    {
        printf("Free Drcom Client v0.7 Beta\n  MengXP @ XUPT,2009-2012\n\n");
        printf("    Usage: drcom [interface] [serverip]\n\n    Example: drcom ppp0\n             drcom eth0 221.11.17.138\n             drcom eth0 123.138.72.114\n\n");
        return 0;
    }
    else
    {
        if(argc >= 2 && strlen(argv[1]) < IFNAMSIZ)
            strcpy(szIfName,argv[1]);

        if(argc == 3 && strlen(argv[2]) < 16)
            strcpy(szServerIp,argv[2]);

        //run in background
        init_daemon();
    }

Init:
    bId0 = 0;
    bId1 = 0;
    bLogon = 0;
    dwUpdateKey0 = 0;
    dwUpdateKey1 = 0;
    wCurrentVer = 9999;

    hSocket = socket(AF_INET,SOCK_DGRAM,0);
    if (hSocket < 0)
    {
        log_err("Socket initialization failed!\n");
        sleep(1);
        goto Init;
    }

    dwFlags = fcntl(hSocket,F_GETFL,0);
    fcntl(hSocket,F_SETFL,dwFlags | O_NONBLOCK);

    strcpy(ifr.ifr_name,szIfName);
    dwRet = ioctl(hSocket,SIOCGIFADDR, &ifr);
    if(dwRet < 0)
    {
        log_err("Get net interface [%s] failed!\n",szIfName);
        close(hSocket);
        sleep(1);
        goto Init;
    }

    sin.sin_addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
    sin.sin_port = htons(61440);
    sin.sin_family = AF_INET;
    dwRet = bind(hSocket,sa,sizeof(sin));
    if(dwRet < 0)
    {
        close(hSocket);
        log_err("Can't bind %s:61440\n",inet_ntoa(sin.sin_addr));
        sleep(1);
        goto Init;
    }
    sin.sin_addr.s_addr = inet_addr(szServerIp);

Activation:

    //Find DrCOM Server
    memset(&c1,0,sizeof(c1));
    c1.bSignture = 7;
    c1.bPacketID = bId0;
    bId0++;
    c1.wLength = 8;
    c1.wSeq = 1;

    ClearRecvBuffer(hSocket);
    dwRet = sendto(hSocket,&c1,8,MSG_NOSIGNAL,sa,sizeof(sin));
    if(dwRet < 0)
    {
        log_err("Send c1 failed,reinit...\n");
        close(hSocket);
        sleep(1);
        goto Init;
    }
    //Server response
    dwRet = WaitForSingleSocket(hSocket,1000*1000);
    if(dwRet < 0)
    {
        log_err("Wait c2 timed out,reinit...\n");
        close(hSocket);
        sleep(1);
        goto Init;
    }
    sinlen = sizeof(sin);
    recvfrom(hSocket,szBuf,sizeof(szBuf),0,sa,&sinlen);

    //Send Login CMD
    memset(&c3,0,sizeof(c3));
    c3.bSignture = 7;
    c3.bPacketID = bId0;
    bId0++;
    c3.wLength = 96;
    c3.wSeq = 3;
    c3.dwUnk0 = 0x455300;
    c3.dwCode1 = c2->dwCode1;
    if(!bLogon)
        c3.dwMode = 0x6200;     //Login
    else
        c3.dwMode = 0x6300;     //Keep alive
    c3.dwCode0 = c2->dwCode0;
    c3.dwChecksum = 0x1312FC7;
    c3.dwUnk1 = 0x7E;

    ptr_int = (int *)&c3;
    dwChecksum = 0;
    for(n = 0; n < 96/4; n++)
        dwChecksum ^= *(ptr_int + n);
    c3.dwChecksum = dwChecksum * 0x12C4B7E;
    c3.dwUnk1 = 0;

    ptr_char = (char *)&c3.NetIfs;  //NetIfs is unnecessary field..all zero..
    //um..this fucking encryption algorithm...
    for(n = 0; n < 64; n++)
        *(ptr_char + n) = (*(ptr_char + n) >> 8 - (n & 7)) + (*(ptr_char + n) << (n & 7));

    dwRet = sendto(hSocket,&c3,96,MSG_NOSIGNAL,sa,sizeof(sin));
    if(dwRet < 0)
    {
        log_err("Send c3 failed,reinit...\n");
        close(hSocket);
        sleep(1);
        goto Init;
    }

    //Server Response & the fuck ad
    dwRet = WaitForSingleSocket(hSocket,1000*1000);
    if(dwRet < 0)
    {
        log_err("Wait c4 timed out,reinit...\n");
        close(hSocket);
        sleep(1);
        goto Init;
    }
    sinlen = sizeof(sin);
    recvfrom(hSocket,szBuf,sizeof(szBuf),0,sa,&sinlen);

    bLogon = 1;
    wRand = (short)time(NULL);

SendCurVer:
    //Send module version
    memset(&up,0,sizeof(up));
    up.bSignture = 7;
    up.bPacketID = bId1;
    bId1++;
    up.wLength = 40;
    up.wSeq = 0x10B;
    up.wCurrentVer = wCurrentVer;
    up.wRand = wRand;
    ClearRecvBuffer(hSocket);
    dwRet = sendto(hSocket,&up,40,MSG_NOSIGNAL,sa,sizeof(sin));
    if(dwRet < 0)
    {
        log_err("Send up1 failed,reinit...\n");
        close(hSocket);
        sleep(1);
        goto Init;
    }

    //Server Response
    dwRet = WaitForSingleSocket(hSocket,1000*1000);
    if(dwRet < 0)
    {
        log_err("Wait up2 timed out,CurrentVer = %d,reinit...\n",wCurrentVer);
        close(hSocket);
        sleep(1);
        goto Init;
    }
    sinlen = sizeof(sin);
    recvfrom(hSocket,szBuf,sizeof(szBuf),0,sa,&sinlen);

    if(wCurrentVer != pup->wCurrentVer)
    {
        wCurrentVer = pup->wCurrentVer;
        goto SendCurVer;
    }

    //Send module version & key
    memset(&up,0,sizeof(up));
    up.bSignture = 7;
    up.bPacketID = bId1;
    bId1++;
    up.wLength = 40;
    up.wSeq = 0x30B;
    up.wCurrentVer = wCurrentVer;
    up.wRand = wRand;
    up.dwKey0 = pup->dwKey0;
    up.dwKey1 = pup->dwKey1;
    up.dwCode1 = c3.dwCode1;
    ptr_short = (short *)&up;
	dwChecksum = 0 ;
    for(n = 0; n < 40/2; n++)
        dwChecksum ^= *(ptr_short + n);
    dwChecksum &= 0xFFFF;
    up.dwChecksum = dwChecksum * 0x2C7;

    ClearRecvBuffer(hSocket);
    dwRet = sendto(hSocket,&up,40,MSG_NOSIGNAL,sa,sizeof(sin));
    if(dwRet < 0)
    {
        log_err("Send up3 failed,reinit...\n");
        close(hSocket);
        sleep(1);
        goto Init;
    }

    //Server Response
    dwRet = WaitForSingleSocket(hSocket,1000*1000);
    if(dwRet < 0)
    {
        log_err("Wait up4 timed out,reinit...\n");
        close(hSocket);
        sleep(1);
        goto Init;
    }
    sinlen = sizeof(sin);
    recvfrom(hSocket,szBuf,sizeof(szBuf),0,sa,&sinlen);

    sleep(15);
    goto Activation;

    return 0;
}
