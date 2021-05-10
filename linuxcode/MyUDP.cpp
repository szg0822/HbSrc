/**********************************************************
* 文件名: MyUDP.cpp
* 功能描述: UDP传输数据
* 作者: 孙志刚
* 日期: 2021/1/15
* 备注：
***********************************************************/

#include "MyUDP.h"
#include "LinuxLog.h"



MyUDP::MyUDP()
{
	loacalPort = DEFAULT_LOCAL_PORT;
	remotePort = DEFAULT_REMOTE_PORT;
	overtime   = DEFAULT_CONN_TIMEOUT;
	timeout    = DEFAULT_RECV_TIMEOUT;
	m_udpfd    = -1;
	errcode    = 0;
	bIsOpen    = false;
	memset(remoteip, 0x00, MAX_IP_LEN);
	memset(localip, 0x00, MAX_IP_LEN);
	memset(errmsg, 0x00, ERR_MAXLENGTH + 1);

	// 设置超时时间为2s
	// udptmvl.tv_sec  = 2;
	// udptmvl.tv_usec = 0;

#if _DEBUG
	ErrLog("[%s][%d]", __FILE__, __LINE__);
#endif
}

MyUDP::~MyUDP()
{
#if _DEBUG
	ErrLog("[%s][%d]", __FILE__, __LINE__ );
#endif
}

/*********************************************************
* 函 数 名: UDP_CREATE
* 功能描述: 创建UDP连接
* 参数说明:
* 返 回 值：0：创建成功；非0：创建失败	    
* 备    注:
*********************************************************/
int MyUDP::UDP_CREATE(void)
{
	//创建socket套接字
	//if ((m_udpfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
	m_udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_udpfd < 0){
		LogError("[%s:%s %u]  create socket failed \n", __FILE__, __func__, __LINE__);
		return ERR_WSAERROR;
	}

	memset(&srcAddr, 0, sizeof(srcAddr));//初始化addr
	srcAddr.sin_family = AF_INET;
	srcAddr.sin_port = htons(loacalPort);
	
	if (strlen(localip) == 0) {
		srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		LogError("[%s:%s %u]  WIFI no connection \n", __FILE__, __func__, __LINE__);
	}
	else{
		srcAddr.sin_addr.s_addr = inet_addr(localip);
	}

#if 0
	bool bReuseaddr = true;
	setsockopt(m_udpfd, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuseaddr, sizeof(bReuseaddr));
#endif

	LogDebug("[%s:%s %u]  localip=[%s],locaport=[%d] \n", __FILE__, __func__, __LINE__, localip, loacalPort);
	// 设置接收缓冲区
	int optVal = 2000 * 2048;// 4M socket buffer
	//int optVal = 20 * 1024 * 1024;// 20M socket buffer
	setsockopt(m_udpfd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, sizeof(optVal));

	//绑定
	if (bind(m_udpfd, (struct sockaddr*)&srcAddr, sizeof(srcAddr)) < 0) {
		LogError("[%s:%s %u]  bind local port failed,err = %s,errno=%d \n", __FILE__, __func__, __LINE__,strerror(errno),errno);
		close(m_udpfd);//关闭套接字 
		return ERR_WSAERROR;
	}
	
	int flags  = fcntl(m_udpfd, F_GETFL, 0);
	fcntl(m_udpfd, F_SETFL, flags&~O_NONBLOCK);    //设置成阻塞模式；

	bIsOpen = true;
	return ERR_SUCCESS;
}

/*********************************************************
* 函 数 名: UDP_CLOSE
* 功能描述: 关闭UDP连接
* 参数说明:
* 返 回 值：0：成功；非0：失败	    
* 备    注:
*********************************************************/
int MyUDP::UDP_CLOSE(void)
{
	//关闭资源  
	if (bIsOpen) {
		if (close(m_udpfd)< 0)	{//关闭套接字 
			LogError("[%s:%s %u]  closesocket failed \n", __FILE__, __func__, __LINE__);
			return ERR_WSAERROR;
		}
	}
	sleep(2);
	bIsOpen = false;
	return ERR_SUCCESS;
}

/*********************************************************
* 函 数 名: UDP_SEND
* 功能描述: UDP发送数据
* 参数说明: szBuff：要发送的数据
*          nSize：发送的长度
* 返 回 值：0：成功；非0：失败	    
* 备    注:
*********************************************************/
int MyUDP::UDP_SEND(unsigned char *szBuff, int nSize)
{
	if (!bIsOpen) {
		LogError("[%s:%s %u]  bIsOpen is FALSE \n", __FILE__, __func__, __LINE__);
		return ERR_WSAERROR;
	}

	if (szBuff == NULL || nSize == 0) {
		LogError("[%s:%s %u]  szBuff is NULL; nSize=%d \n", __FILE__, __func__, __LINE__, nSize);
		return ERR_BADPARAM;
	}

	if (sendto(m_udpfd, szBuff, nSize, 0, (sockaddr *)&dstAddr, sizeof(struct sockaddr)) < 0) {
		int lasterror = errno;
		if (lasterror == EWOULDBLOCK) {
			struct timeval timeout;
			fd_set r;
			FD_ZERO(&r);
			FD_SET(m_udpfd, &r);
			timeout.tv_sec = overtime; //连接超时2秒 
			timeout.tv_usec = 0;
			if (select(0, 0, &r, 0, &timeout) <= 0)
				return ERR_WSAERROR;
			else
				return ERR_SUCCESS;
		}
		else
			LogError("[%s:%s %u]  lasterror=%d \n", __FILE__, __func__, __LINE__, lasterror);
		return ERR_WSAERROR;
	}

	return ERR_SUCCESS;
}

/*********************************************************
* 函 数 名: UDP_RECV
* 功能描述: UDP接收数据
* 参数说明: szBuff：要接收的数据
*          nSize：接收的长度
* 返 回 值：正确：返回的是接收数据长度；错误：小于等于0    
* 备    注:
* select 有三种返回值
* -1：出错
* 0：如果设置了超时，在指定时间内没有fd可读写，则返回0， 可在此指定的相应超时处理操作
* >0:返回可读写的fd数
*********************************************************/
int MyUDP::UDP_RECV(unsigned char *szBuff, int nSize)
{
	if (!bIsOpen) {
		LogError("[%s:%s %u]  bIsOpen is FALSE \n", __FILE__, __func__, __LINE__);
		return 0;
	}

	if (szBuff == NULL || nSize == 0) {
		LogError("[%s:%s %u]  szBuff is NULL; nSize=%d \n", __FILE__, __func__, __LINE__, nSize);
		return -1;
	}

	int getlen = sizeof(dstAddr);
	int nlen = 0;

	struct timeval udp_tmvl;
	udp_tmvl.tv_sec = 0;
	udp_tmvl.tv_usec = 100 * 1000;

	errno = 0;
	FD_ZERO(&rfd);         // 在使用之前总是要清空
	FD_SET(m_udpfd, &rfd);
	
	int ret = select((m_udpfd + 1), &rfd, NULL, NULL, &udp_tmvl);// 检测是否有套接口是否可读 准备就绪的描述符数
	if (ret < 0) { // 出错则返回-1。
		LogError("[%s:%s %u]  Select Failed \n", __FILE__, __func__, __LINE__);
		return -2;
	}
	else if (ret == 0) { // 超时则返回0
		//ErrLog("[%s][%d] UDP_RECV:%s", __FILE__, __LINE__, "timeout");
		return -3;
	}
	else { // 检测到有套接口可读
		if (FD_ISSET(m_udpfd, &rfd))  { // 可读
			nlen = recvfrom(m_udpfd, szBuff, nSize, 0, (sockaddr *)&dstAddr,(socklen_t*) &getlen);
			if (nlen < 0) {
				LogError("[%s:%s %u]  recvfrom failed,because of %s \n", __FILE__, __func__, __LINE__, strerror(errno));
				return -4;
			}
		}
		else
			return -5;
	}
	return nlen;
}
