#include "MyIpcSocket.h"
#include "LinuxLog.h"
#include "MyThread.h"


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>



int MyServerSocketOpen(mysock_t* pSocket, USHORT port, MysocketType_e type, Bool bBlock)
{
    int s32Ret = -1;
    int s32Type, s32Proto;
    
    int fd = 0xffffffff;
    
    printf("if you need to use this, plsese sure this is ok ........ @mm\n");

    if (NULL == pSocket)
    {
        s32Ret = -3;
        goto FAIL;
    }

    if (MySOCKTYPE_TCP == type)
    {
        s32Type = SOCK_STREAM;
        s32Proto = IPPROTO_TCP;
        if (!port)
            goto FAIL;
    }
    else if (MySOCKTYPE_UDP == type)
    {
        s32Type = SOCK_DGRAM;
        s32Proto = IPPROTO_UDP;
        if (!port)
            goto FAIL;
    }
    else
        goto FAIL;

    fd = socket(AF_INET, s32Type, s32Proto);
    if (fd < 0)
        goto FAIL;

    // set block or non-block
    if (TRUE == bBlock)
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
    else
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    // server
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    dest.sin_addr.s_addr = htonl(INADDR_ANY);

    if (MySOCKTYPE_TCP == type)
    {
        // do after
    }
    else if (MySOCKTYPE_UDP == type)
    {
        int ret = bind(fd, (struct sockaddr*)&dest, sizeof(dest));
        if (ret < 0)
        {
            goto FAIL;
        }
    }
    else
    {
        goto FAIL;
    }

    pSocket->fd = fd;
    pSocket->block = bBlock;
    pSocket->type = type;
    memcpy(&pSocket->dest, &dest, sizeof(dest));

    


    s32Ret = 0;
    return s32Ret;
    
FAIL:
    if(fd>=0)
    {
        close(fd);
    }
   return s32Ret;
}

int MyServerSocketClose(UINT handle)
{
    int s32Ret = -1;
    mysock_t* hSocket = NULL;

    if(0 == handle)
    {
        s32Ret = -3;
        goto end;
    }

    hSocket = (mysock_t*)handle;

    if (hSocket->fd >= 0)
    {
        close(hSocket->fd);
        free(hSocket);
    }

    s32Ret = 0;

end:
    return s32Ret;
}

int MyClientSocketOpen(mysock_t* pSocket, char const* host, USHORT port, MysocketType_e type, Bool bBlock)
{
    int s32Ret = -1;
    int s32Type, s32Proto;

    if (NULL == pSocket)
    {
        s32Ret = -3;
        goto end;
    }

    if (MySOCKTYPE_TCP == type)
    {
        s32Type = SOCK_STREAM;
        s32Proto = IPPROTO_TCP;
        if (!host || !port)
            goto end;
    }
    else if (MySOCKTYPE_UDP == type)
    {
        s32Type = SOCK_DGRAM;
        s32Proto = IPPROTO_UDP;
    }
    else
        goto end;

    int fd = socket(AF_INET, s32Type, s32Proto);
    if (fd < 0)
        goto end;

    // set block or non-block
    if (TRUE == bBlock)
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
    else
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    // client
    struct sockaddr_in dest;


    if (MySOCKTYPE_TCP == type)
    {
        dest.sin_family = AF_INET;
        dest.sin_port = htons(port);
        if (!inet_aton(host, &(dest.sin_addr)))
        {
            struct hostent* h = gethostbyname(host);
            if (h)
                memcpy(&dest.sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
            else
            {
                goto FAIL;
            }
        }

        // connect host
        while (1)
        {
            s32Ret = connect(fd, (struct sockaddr *)&dest, sizeof(dest));

            // if non-block
            if ((s32Ret < 0) && (TRUE != bBlock))
            {
                if (EINTR == errno)
                    continue;

                if ((EINPROGRESS != errno) && (EAGAIN != errno))
                {
                    if (fd >= 0)
                        close(fd);
                    goto end;
                }

                // wait until we are connected or until abort
                int try = 100;
                fd_set wfds;
                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 100 * 1000;
                while (1)
                {
                    FD_ZERO(&wfds);
                    FD_SET(fd, &wfds);

                    s32Ret = select(fd + 1, NULL, &wfds, NULL, &tv);
                    if (s32Ret > 0 && FD_ISSET(fd, &wfds))
                        break;
                    else
                        usleep(50 * 1000);

                    // timeout?
                    if (try-- < 0)
                    {
                        if (fd >= 0)
                            close(fd);

                        s32Ret = -1;
                        goto end;
                    }
                }
            }
            else if (s32Ret >= 0)
            {
                break;
            }
            else
            {
                if (fd >= 0)
                    close(fd);
                goto end;
            }
        }
    }
    else if (MySOCKTYPE_UDP == type)
    {
        int so_broadcast = 1;
        setsockopt(fd,SOL_SOCKET,SO_BROADCAST,&so_broadcast,sizeof(so_broadcast));

        dest.sin_family = AF_INET;
        dest.sin_port = htons(port);
        if (!host)
        {
            dest.sin_addr.s_addr = htonl(INADDR_ANY);

            int ret = bind(fd, (struct sockaddr*)&dest, sizeof(dest));
            if (ret < 0)
            {
                goto FAIL;
            }
        }
        else
        {
            if (!inet_aton(host, &(dest.sin_addr)))
            {
                struct hostent* h = gethostbyname(host);
                if (h)
                    memcpy(&dest.sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
                else
                {
                    goto FAIL;
                }
            }

        }
    }
    else
    {
        goto FAIL;
    }

    pSocket->fd = fd;
    pSocket->block = bBlock;
    pSocket->type = type;
    memcpy(&pSocket->dest, &dest, sizeof(dest));


    s32Ret = 0;
    return s32Ret;
FAIL:
    if(fd >=0)
    {
        close(fd);
    }
end:
   printf("socket error s32Ret=0x%x error no = %d\n",s32Ret, errno);
    return s32Ret;
}

int MyClientSocketClose(UINT handle)
{
    int s32Ret = -1;
    mysock_t* hSocket = NULL;

    if(0 == handle)
    {
        s32Ret = -3;
        goto end;
    }

    hSocket = (mysock_t*)handle;

    if (hSocket->fd >= 0)
    {
        close(hSocket->fd);
        free(hSocket);
    }

    s32Ret = 0;

end:
    return s32Ret;
}

int MySocketRecv(mysock_t *pSocket, char* data, int size)
{
    int s32Ret = -1;
    mysock_t* hSocket = NULL;

    if(NULL == pSocket)
    {
        goto end;
    }
    hSocket = (mysock_t*)pSocket;
printf("===fd=%d, block=%d, type=%d, dest=%s\n", hSocket->fd, hSocket->block,hSocket->type, hSocket->dest);
    // non-block
    if (FALSE == hSocket->block)
    {
        struct timeval tv;
        fd_set readfds;

        FD_ZERO(&readfds);
        FD_SET(hSocket->fd, &readfds);

        tv.tv_sec = 0;
        tv.tv_usec = 20 * 1000;
        s32Ret = select(hSocket->fd + 1, &readfds, NULL, NULL, &tv);

        if (s32Ret > 0)
        {
            if (MySOCKTYPE_TCP == hSocket->type)
                s32Ret = recv(hSocket->fd, data, size, 0);
            else
            {
#if 1
                struct sockaddr_in src;
                unsigned int srcLen = sizeof(src);
                printf("11\n");
                s32Ret = recvfrom(hSocket->fd, data, size, 0, (struct sockaddr*)&src, &srcLen);
                printf("22\n");
                memset(&hSocket->dest, 0, sizeof(hSocket->dest));
                memcpy(&hSocket->dest, &src, sizeof(hSocket->dest));
                printf("33\n");
#else
                s32Ret = read(hSocket->fd, data, size);
#endif

            }

            if (s32Ret < 0)
            {
                if (errno != EINTR && errno != EAGAIN)
                    s32Ret = -1;
            }
        }
        else if (s32Ret < 0)
            s32Ret = -1;
        else
            s32Ret = 0;
    }
    // block
    else
    {
        if (MySOCKTYPE_TCP == hSocket->type)
            s32Ret = recv(hSocket->fd, data, size, 0);
        else
        {
            struct sockaddr_in src;
            unsigned int srcLen = sizeof(src);

            s32Ret = recvfrom(hSocket->fd, data, size, 0, (struct sockaddr*)&src, &srcLen);
            memset(&hSocket->dest, 0, sizeof(hSocket->dest));
            memcpy(&hSocket->dest, &src, sizeof(hSocket->dest));
        }
    }


end:
    return s32Ret;
}

int MySocketSend(mysock_t *pSocket, char* data, int size)
{
    int s32Ret = -1;
    mysock_t* hSocket = NULL;

    if(NULL == pSocket)
    {
        goto end;
    }
    hSocket = (mysock_t*)pSocket;

    if (FALSE == hSocket->block)
    {
        struct timeval tv;
        fd_set writefds;

        FD_ZERO(&writefds);
        FD_SET(hSocket->fd, &writefds);

        tv.tv_sec = 0;
        tv.tv_usec = 100 * 1000;
        s32Ret = select(hSocket->fd + 1, NULL, &writefds, NULL, &tv);

        if (s32Ret > 0)
        {
            if (MySOCKTYPE_TCP == hSocket->type)
                s32Ret = send(hSocket->fd, data, size, 0);
            else
                s32Ret = sendto(hSocket->fd, data, size, 0, (struct sockaddr*)&hSocket->dest, sizeof(hSocket->dest));

            if (s32Ret < 0)
            {
                perror(" send -1\n");
                if (errno != EINTR && errno != EAGAIN)
                    s32Ret = -1;
            }
        }
        else if (s32Ret < 0)
            s32Ret = -1;
        else
            s32Ret = 0;
    }
    else
    {
        if (MySOCKTYPE_TCP == hSocket->type)
            s32Ret = send(hSocket->fd, data, size, 0);
        else
            s32Ret = sendto(hSocket->fd, data, size, 0, (struct sockaddr*)&hSocket->dest, sizeof(hSocket->dest));
    }

end:
    return s32Ret;
}

int MySocketGetAttr(MysocketAttrId_e attrId, UINT port, UINT param2)
{
    int ret = -1;
    struct sockaddr_in connAddr;
    int len = sizeof (connAddr);
    int  fd  = 0xffffffff;
    struct sockaddr_in addr;
     
    switch(attrId)
    {
        case My_SOCKET_ATRRID_FREE_PORT_E:
            {
                memset(&addr,0,sizeof(struct sockaddr_in));
                UINT *port = (UINT *)port;
                fd= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if(fd <0)
                {
                    LogError("error open socket \n");
                    return -1;
                }
                 addr.sin_family = AF_INET;
                 addr.sin_addr.s_addr = htonl(INADDR_ANY);
                 addr.sin_port = 0;             
                 int ret = bind(fd, (struct sockaddr*)&addr, sizeof addr);
                 if (0 != ret) 
                 {
                    LogError("bind error ");
                    goto end;
                 }
                ret = getsockname(fd, (struct sockaddr*)&connAddr, (socklen_t *)&len);
                if (0 != ret)
                {
                    LogError("getsockname  error ");
                     goto end;
                }
                *port = ntohs(connAddr.sin_port); 
end:

                if(fd != 0xffffffff)
                {
                    close(fd);
                }
                ret = 0;
                LogError("got free port:%d \n",*port );
            }
            break;
        default:
            LogError("unsupport attrid:%d \n",attrId);
            break;
    }

    return ret;
}

static char remoteip[128] = "127.0.0.1";
static char localip[128] = "127.0.0.1";
#define REMOTEPORT 40000
#define LOCALPORT 40000
static Bool bIsOpen;
static int fd = 0xffffffff;
static struct sockaddr_in srcAddr;
static struct sockaddr_in dstAddr;


int MyUDP_CREATE(MysocketType_e type, Bool bBlock)
{
    int s32Ret = -1;
    int s32Type, s32Proto;
    
    
    
    printf("MyUDP_CREATE run\n");

    if (MySOCKTYPE_TCP == type)
    {
        s32Type = SOCK_STREAM;
        s32Proto = IPPROTO_TCP;
    }
    else if (MySOCKTYPE_UDP == type)
    {
        s32Type = SOCK_DGRAM;
        s32Proto = IPPROTO_UDP;
    }
    else
        goto FAIL;

    fd = socket(AF_INET, s32Type, s32Proto);
	if(fd < 0){
		LogError("[%s:%s %u]=== UDP_CREATE Err:create socket failed", __FILE__, __func__, __LINE__);
		return -1;
	}
    // set block or non-block
    if (TRUE == bBlock)
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
    else
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

	memset(&dstAddr, 0, sizeof(dstAddr));
	dstAddr.sin_family = AF_INET;
	dstAddr.sin_addr.s_addr = inet_addr(remoteip);
	dstAddr.sin_port = htons(REMOTEPORT);

	memset(&srcAddr, 0, sizeof(srcAddr));//初始化addr
	srcAddr.sin_family = AF_INET;
	srcAddr.sin_port = htons(LOCALPORT);

    if (strlen(localip) == 0) {
        srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else{
		srcAddr.sin_addr.s_addr = inet_addr(localip);
	}

    // 设置接收缓冲区
	int optVal = 2000 * 2048;// 4M socket buffer
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, sizeof(optVal));


    if (MySOCKTYPE_TCP == type)
    {
        // do after
    }
    else if (MySOCKTYPE_UDP == type)
    {
        int ret = bind(fd, (struct sockaddr*)&srcAddr, sizeof(srcAddr));
        if (ret < 0)
        {
            goto FAIL;
        }
    }
    else
    {
        goto FAIL;
    }
    printf("MyUDP_CREATE end\n");
    bIsOpen = TRUE;
    return 0;
FAIL:
    if(fd>=0)
    {
        close(fd);
    }
   return -1;

}

int MyUDP_RECV(unsigned char *szBuff, int nSize, int timeout)
{
	if (!bIsOpen) {
		LogError("[%s:%s %u]=== bIsOpen is FALSE", __FILE__, __func__, __LINE__);
		return 0;
	}

	if (szBuff == NULL || nSize == 0) {
		LogError("[%s:%s %u]=== Err:szBuff is NULL; nSize=%d", __FILE__, __func__, __LINE__, nSize);
		return -1;
	}

	int getlen = sizeof(struct sockaddr_in);
	int nlen = 0;

    struct timeval tv;
    fd_set rfd;

    FD_ZERO(&rfd);
    FD_SET(fd, &rfd);

    tv.tv_sec = timeout;
    tv.tv_usec = 20 * 1000;

	errno = 0;
	
	int ret = select((fd + 1), &rfd, NULL, NULL, &tv);// 检测是否有套接口是否可读 准备就绪的描述符数
	if (ret < 0) { // 出错则返回-1。
		LogError("[%s:%s %u]=== UDP_RECV:select() failed", __FILE__, __func__, __LINE__);
		return -2;
	}
	else if (ret == 0) { // 超时则返回0
		//ErrLog("[%s][%d] UDP_RECV:%s", __FILE__, __LINE__, "timeout");
		return -3;
	}
	else { // 检测到有套接口可读struct sockaddr_in srcAddr
		if (FD_ISSET(fd, &rfd))  { // 可读
			nlen = recvfrom(fd, szBuff, nSize, 0, (struct sockaddr_in *)&srcAddr,(socklen_t*) &getlen);
			if (nlen < 0) {
				LogError("[%s:%s %u]=== recvfrom failed,because of %d", __FILE__, __func__, __LINE__, errno);
				return -4;
			}
		}
		else
			return -5;
	}
	return nlen;
}

int MyUDP_SEND(unsigned char *szBuff, int nSize)
{
	if (!bIsOpen) {
		LogError("[%s:%s %u]=== UDP_SEND Err:bIsOpen is FALSE", __FILE__, __func__, __LINE__);
		return -1;
	}

	if (szBuff == NULL || nSize == 0) {
		LogError("[%s:%s %u]=== UDP_SEND Err:szBuff is NULL; nSize=%d", __FILE__, __func__, __LINE__, nSize);
		return -2;
	}

	if (sendto(fd, szBuff, nSize, 0, (struct sockaddr_in *)&dstAddr, sizeof(struct sockaddr)) < 0) {
		int lasterror = errno;
		if (lasterror == EWOULDBLOCK) {
			struct timeval timeout;
			fd_set r;
			FD_ZERO(&r);
			FD_SET(fd, &r);
			timeout.tv_sec = 2; //连接超时2秒 
			timeout.tv_usec = 0;
			if (select(0, 0, &r, 0, &timeout) <= 0)
				return -3;
			else
				return 0;
		}
		else
			LogError("[%s:%s %u]=== lasterror=%d", __FILE__, __func__, __LINE__,lasterror);
		return -4;
	}

	return 0;
}

#ifndef _TEST
USHORT port = 5000;

void TServer()
{
    printf("TServer run\n");
    UINT SHandle = 0;  
    char data[1024] = {0};
    mysock_t *pSocket;
    // create socket
    pSocket = malloc(sizeof(mysock_t));
    if (NULL == pSocket)
    {
        return;
    }
    memset(pSocket, 0, sizeof(mysock_t));
    MyServerSocketOpen(pSocket, port, MySOCKTYPE_UDP, FALSE);
    while(1) {
        memset(data, 0, 1024);
        printf("server\n");
        MySocketRecv(pSocket, data, 1024);
               
        sleep(1);
    }

    //MyServerSocketClose(SHandle);

}
void TClient()
{
    printf("TClient ok\n");
    UINT SHandle = 0;  
    mysock_t *pSocket;
    // create socket
    pSocket = malloc(sizeof(mysock_t));
    if (NULL == pSocket)
    {
        return;
    }
    memset(pSocket, 0, sizeof(mysock_t));
    char data[1024] = "socket ok";
    printf("122\n");
    MyClientSocketOpen(pSocket, "127.0.0.1", port, MySOCKTYPE_UDP, FALSE);
    printf("122\n");
    while(1) {
         printf("client\n");
        MySocketSend(pSocket, data, 1024);
       
        printf("data=%s\n", data);
        sleep(1);
    }
    


    //MyClientSocketClose(SHandle);


}

static void ThreadHandle_server(void *arg)
{
    MyThread_t *thread = (MyThread_t*)arg;

    //while (MyThreadGetState(thread) == THREAD_RUNNING)
    {
        TServer();
        //MyThreadSleep(thread);
    }

}

static void ThreadHandle_client(void *arg)
{
    MyThread_t *thread = (MyThread_t*)arg;

    //while (MyThreadGetState(thread) == THREAD_RUNNING)
    {
        TClient();
        //MyThreadSleep(thread);
    }

}

int main()
{
    MyThread_t * sigThread_read;
    MyThread_t * sigThread_write;
    printf("thread run\n");
    //日志初始化
    InitLinuxLog();
    LogDebug("[%s:%s %u]=== pthread run", __FILE__, __func__, __LINE__);
    
    char sbuf[1024] = "socket okd";
    char rbuf[1024];
    if (0 == MyUDP_CREATE(MySOCKTYPE_UDP, FALSE)) {
        printf("connect success!\n");
        while(1) {
            if (0 == MyUDP_SEND(sbuf, 1024)) {
                memset(rbuf, 0, 1024);
                int len = MyUDP_RECV(rbuf, 1024, 2);
                printf("buf=[%s], len=%d\n", rbuf, len);
            }
            sleep(1);
        }
        
    }
    return 0;
#if 0
    sigThread_write = MyThreadCreate(1000*100, NULL, ThreadHandle_client);
    if (sigThread_write)
    {
        MyThreadStart(sigThread_write);
    }
    else
    {
        printf("register: Create SIG collect thread failed");
    }
    sigThread_read = MyThreadCreate(1000*100, NULL, ThreadHandle_server);
    if (sigThread_read)
    {
        MyThreadStart(sigThread_read);
    }
    else
    {
        printf("register: Create SIG collect thread failed");
    }
    printf("thread end\n");
    while(1); //保存主程序活性，线程才能一直跑，当主程序结束时，线程资源回收
    MyThreadRelease(sigThread_read);
    MyThreadRelease(sigThread_write);
#endif 
}

#endif