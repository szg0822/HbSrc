
#ifndef MYIPCSOCKET_H
#define MYIPCSOCKET_H

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
typedef enum MysocketType_e {
	MySOCKTYPE_TCP = 0,
	MySOCKTYPE_UDP ,
	MySOCKTYPE_COUNT
} MysocketType_e;

typedef enum
{
    My_SOCKET_ATRRID_FREE_PORT_E=0,
        
}MysocketAttrId_e;


#define UINT unsigned int
#define UCHAR unsigned char
#define USHORT   unsigned short
#define Bool    int 
#define TRUE (0==0)
#define FALSE (!TRUE)


typedef struct Mysocket_t {
    int fd;
    MysocketType_e type;
    struct sockaddr_in dest;
    Bool block;
} mysock_t;

/////////////////////////////////////////////////////可用//////////////////////////////////////////////////////
//socket UDP
int MyUDP_CREATE(MysocketType_e type, Bool bBlock);
int MyUDP_RECV(unsigned char *szBuff, int nSize, int timeout);
int MyUDP_SEND(unsigned char *szBuff, int nSize);



/////////////////////////////////////////////////////测试//////////////////////////////////////////////////////
/*
 * 函数名: MyServerSocketOpen
 * 函数说明: 建立一个新的socket，即向系统注册一个新的通信端口，作为服务端。
 * 输入参数:
 *      port: 端口号。
 *      type: 网络协议类型。
 *      bBlock： 是否为阻塞。
 * 输出参数:
 *      phSocket: 如果建立成功，socket（通信端口）的句柄被写入 phSocket的内存。
 * 返回值: 查看枚举型 int 。
 */
int MyServerSocketOpen(mysock_t* pSocket, USHORT port, MysocketType_e type, Bool bBlock);


/*
 * 函数名: MyServerSocketClose
 * 函数说明: 关闭一个socket，即向系统注销一个通信端口。
 * 输入参数:
 *      handle: 要关闭的socket（通信端口）的文件句柄。
 * 输出参数:
 *      无。
 * 返回值: 查看枚举型 int 。
 */
int MyServerSocketClose(UINT handle);


/*
 * 函数名: MyClientSocketOpen
 * 函数说明: 建立一个新的socket，即向系统注册一个新的通信端口，作为客户端。
 * 输入参数:
 * 		host: 用来建立socket通信的主机域名。
 * 		port: 端口号。
 *      type: 网络协议类型。
 *      bBlock： 是否为阻塞。
 * 输出参数:
 * 		phSocket: 如果建立成功，socket（通信端口）的句柄被写入 phSocket的内存。
 * 返回值: 查看枚举型 int 。
 */
int MyClientSocketOpen(mysock_t* pSocket, char const* host, USHORT port, MysocketType_e type, Bool bBblock);


/*
 * 函数名: MyClientSocketClose
 * 函数说明: 关闭一个socket，即向系统注销一个通信端口。
 * 输入参数:
 * 		handle: 要关闭的socket（通信端口）的文件句柄。
 * 输出参数:
 * 		无。
 * 返回值: 查看枚举型 int 。
 */
int MyClientSocketClose(UINT handle);


/*
 * 函数名: MySocketRecv
 * 函数说明: 从一个socket（通信端口）中接收数据。
 * 输入参数:
 * 		handle: 要接收数据的socket（通信端口）的文件句柄。
 * 		size: 接收数据的大小。
 * 输出参数:
 * 		data: 把从socket（通信端口）接受到的数据写入data的内存中。
 * 返回值: 返回实际接收的数据的长度，不成功返回-1。
 * 注意：这里TCP和UDP可以选择
 */
int MySocketRecv(mysock_t *pSocket, char* data, int size);

/*
 * 函数名: MySocketSend
 * 函数说明: 向建立好的socket（通信端口）中写入size大小的数据。
 * 输入参数:
 * 		data: 要写入数据的socket（通信端口）的数据的内存地址。
 * 		size: 写入数据的大小。
 * 输出参数:
 * 		handle: 数据写入socket（通信端口）的文件句柄。
 * 返回值: 返回实际写入的数据的长度，不成功返回-1。
 * 注意：这里TCP和UDP可以选择
 */
int MySocketSend(mysock_t *pSocket, char* data, int size);


/*
 * 函数名: MySocketGetAttr
 * 函数说明: 获取端口
 * 输入参数:
 * 		data: 要写入数据的socket（通信端口）的数据的内存地址。
 * 		size: 写入数据的大小。
 * 输出参数:
 * 		handle: 数据写入socket（通信端口）的文件句柄。
 * 返回值: 返回实际写入的数据的长度，不成功返回-1。
 * 注意：这里TCP和UDP可以选择
 */
int MySocketGetAttr(MysocketAttrId_e attrId, UINT port, UINT param2);


#endif /* EVM_PORTING_SOCKET_H_ */
