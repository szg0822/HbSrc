/**********************************************************
* 文件名: MyUDP.h
* 功能描述: UDP传输数据
* 作者: 孙志刚
* 日期: 2021/1/15
* 备注：
***********************************************************/


#ifndef __MYUDP_H
#define __MYUDP_H
#include <stdio.h>   
#include <stdlib.h>   
#include <unistd.h>   
#include <errno.h>   
#include <string.h>   
#include <sys/types.h>   
#include <netinet/in.h>   
#include <sys/socket.h>   
#include <sys/wait.h>   
#include <arpa/inet.h>  
#include <fcntl.h>
#include <sys/time.h>

#define BOOL bool
//extern int errno;

#define ERR_SUCCESS  0x00000000	// Successful
#define ERR_BADPARAM 0x80000001	// Bad argument passed
#define ERR_WSAERROR 0x80000002	// Need to get_LastError()
#define ERR_MAXLENGTH 512

#define MAX_IPADDR_LEN 16
#define MAX_BUFF_SIZE  1024

#define ETHERTYPE_IP            0x0800          /* IP */  
#define ETHER_ADDR_LEN	6
#define PACKAGE_PER_SEND_BYTE 1091

#define MAX_IP_LEN 16
#define DEFAULT_LOCAL_PORT  32897
#define DEFAULT_REMOTE_PORT 32896
#define DEFAULT_CONN_TIMEOUT 2
#define DEFAULT_RECV_TIMEOUT 6
#define DEFAULT_SEND_TIMEOUT 5

class MyUDP
{
public:
	MyUDP();
	virtual ~MyUDP();
public:
	int UDP_CREATE(void);
	int UDP_CLOSE(void);
	int UDP_SEND(unsigned char *szBuff, int nSize);
	int UDP_RECV(unsigned char *szBuff, int nSize);

	int UDP_SET_SEND_TIMEOUT();
	int UDP_SET_RECV_TIMEOUT();
	void UDP_GETERR(char* strBuffer, int* iErrNum);
	bool IsOpened() { return bIsOpen; };

private:
	void UDP_SETERR(const char* newError, int errNum);
	int  errcode;
	char errmsg[ERR_MAXLENGTH + 1];
	fd_set rfd;                 // 读描述符集
/*
    // timeout
	struct timeval {
	   __time_t tv_sec;        // Seconds.
	   __suseconds_t tv_usec;  // Microseconds.
	};
*/

public:
	sockaddr_in srcAddr;
	sockaddr_in dstAddr;
	//SOCKET m_udpfd;
	int m_udpfd;
	char remoteip[MAX_IP_LEN];
	char localip[MAX_IP_LEN];
	unsigned short loacalPort;
	unsigned short remotePort;
	int timeout;
	int overtime;
	bool bIsOpen;
	// struct timeval udptmvl;    //超时
};

#endif
