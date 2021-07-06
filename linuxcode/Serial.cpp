/*****************************************************************************
File name: Serial.cpp
Description: ARM和MCU之间通过串口进行通信
Author: sunzhigang
Date: 2021/1/8
*****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
#include <fcntl.h>
#include <unistd.h>
 
#include <termios.h> //set baud rate
 
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include "Serial.h"
#include "LinuxLog.h"
 
#define rcvTimeOut 2

CSerial::CSerial()
{}

CSerial::~CSerial()
{}


//CRC16/IBM
uint16_t crc16(uint8_t *data,uint16_t size)
{
	uint16_t crc = 0;
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t data_t ;
	if(data == NULL){
		return 0;
	}
	for(j=0;j<size;j++){
		data_t = *data++;
		crc = (data_t ^ (crc));
		for(i=0;i<8;i++){
			if((crc&0x01)==1){
				crc = (crc>>1) ^ 0xA001;
			} else {
				crc >>= 1;
			}
		}
	}
	return crc;
}


/*************************************************
Function: // int openPort(int fd, int comport)
Description: // 打开串口（阻塞）
Input: // df:设备描述符；comport：设备选择。
Output: // 无
Return: // 小于0错误，相反正确
Others: // 其它说明
*************************************************/
int CSerial::openPort(int comport)
{
	int fd = -1;
	if (comport == 1)
	{
		fd = open("/dev/ttyPS1", O_RDWR | O_NOCTTY | O_NDELAY);
		if (-1 == fd)
		{
			return(-1);
		}
	}
	else if (comport == 2)
	{
		fd = open("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NDELAY);
		if (-1 == fd)
		{
			return(-1);
		}
	}
	else if (comport == 3)
	{
		fd = open("/dev/ttyS2", O_RDWR | O_NOCTTY | O_NDELAY);
		if (-1 == fd)
		{
			return(-1);
		}
	}
	
	else if (comport == 4)
	{
		fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
		if (-1 == fd)
		{
			return(-1);
		}
	}
	//test
	else if (comport == 5)
	{
		fd = open("./test.txt", O_RDWR | O_NOCTTY | O_NDELAY);
		if (-1 == fd)
		{
			return(-1);
		}
	}
 
	if (fcntl(fd, F_SETFL, 0)<0)
	{
		LogError("[%s:%s %u]  fcntl failed! \n", __FILE__, __func__, __LINE__);
	}
	// if (isatty(STDIN_FILENO) == 0)
	// {
		// LogError("[%s:%s %u]  standard input is not a terminal device \n", __FILE__, __func__, __LINE__);
	// }

	return fd;
}
 
/*************************************************
Function: // int setOpt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
Description: // 设置串口属性
Input: // df:设备描述符；nSpeed：波特率； nBits：数据位； nEvent：校验位； nStop：停止位。
Output: // 无
Return: // 小于0错误，相反正确
Others: // 其它说明
*************************************************/
int CSerial::setOpt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio, oldtio;
	if (tcgetattr(fd, &oldtio) != 0)
	{
		LogError("[%s:%s %u]  SetupSerial 1 \n", __FILE__, __func__, __LINE__);
		return -1;
	}
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;
 
	switch (nBits)
	{
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
	}
 
	switch (nEvent)
	{
		case 'O':                     //奇校验
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |= PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'E':                     //偶校验
			newtio.c_iflag |= (INPCK | ISTRIP);
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			break;
		case 'N':                    //无校验
			newtio.c_cflag &= ~PARENB;
			break;
	}
 
	switch (nSpeed)
	{
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		default:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
	}
	if (nStop == 1)
	{
		newtio.c_cflag &= ~CSTOPB;
	}
	else if (nStop == 2)
	{
		newtio.c_cflag |= CSTOPB;
	}
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;
	tcflush(fd, TCIFLUSH);
	if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
	{
		LogError("[%s:%s %u]  com set error \n", __FILE__, __func__, __LINE__);
		return -1;
	}

	return 0;
}
 
/*************************************************
Function: // int readDataTty(int fd, char *rcv_buf, int TimeOut, int Len)
Description: // 从串口读取数据
Input: // fd:设备描述符； TimeOut：超时；Len：数据长度
Output: // rcv_buf：读取的数据
Return: // 读取数据的实际大小
Others: // 其它说明
*************************************************/
int CSerial::readDataTty(int fd, uint8_t *rcv_buf, int TimeOut, int Len)
{
	int retval;
	fd_set rfds;
	struct timeval tv;
	int ret, pos;
	tv.tv_sec = TimeOut / 1000;  //set the rcv wait time  
	tv.tv_usec = TimeOut % 1000 * 1000;  //100000us = 0.1s  
 
	pos = 0;
	while (1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		retval = select(fd + 1, &rfds, NULL, NULL, &tv);
		if (retval == -1)
		{
			LogError("[%s:%s %u]  select() \n", __FILE__, __func__, __LINE__);
			break;
		}
		else if (retval)
		{
			ret = read(fd, rcv_buf + pos, 1);
			if (-1 == ret)
			{
				break;
			}
 
			pos++;
			if (Len <= pos)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
 
	return pos;
}
 
/*************************************************
Function: // int writeDataTty(int fd, char *send_buf, int Len)
Description: // 写数据到串口
Input: // fd:设备描述符； send_buf：写的数据； Len:数据长度
Output: // 无
Return: // 正确：返回写入数据的实际大小，错误返回-1
Others: // 其它说明
*************************************************/
int CSerial::writeDataTty(int fd, uint8_t *send_buf, int Len)
{
	ssize_t ret;
 
	ret = write(fd, send_buf, Len);
	if (ret == -1)
	{
		LogError("[%s:%s %u]  write device error \n", __FILE__, __func__, __LINE__);
		tcflush(fd, TCOFLUSH);
		return -1;
	}
 
	return 0;
}

/*************************************************
Function: // WriteMessage_57
Description: // ARM发送写指令
Input: // fd:设备描述符； RegisterAdd:寄存器地址； Num：数据数量N； Data：N个16bit数据
Output: // 无
Return: // 正确：0，错误返回-1
Others: // 其它说明
*************************************************/
//01 57 08 01 12 34 CA 2A
//ARM发送写指令
int CSerial::WriteMessage_57(int fd, uint8_t RegisterAdd, uint8_t Num, uint16_t *Data)
{
	LogDebug("[%s:%s %u]  WriteMessage_57 \n", __FILE__, __func__, __LINE__);
	uint8_t sndbuf[SERIAL_BUF_SIZE] = {0};
	memset(sndbuf, 0x00, SERIAL_BUF_SIZE);
	// head 0~5
	int i = 0;
	int j = 0;
	int k = 0;
	uint16_t crc_len = 0;

	sndbuf[i++] = 0x01;
	sndbuf[i++] = 0x57;
	sndbuf[i++] = RegisterAdd;
	sndbuf[i++] = Num;
	//printf("num=%d\n", Num);
	for (j = i, k = 0; k < Num; k++) {
		sndbuf[j] = (uint8_t)((Data[k] >> 8) & 0xff);
		sndbuf[j+1] = (uint8_t)(Data[k] & 0xff);
		j += 2;
	}
	i = j;
	crc_len = i;
	uint16_t ret_crc = crc16(sndbuf, crc_len);
	uint8_t crcH, crcL;
	crcH = (ret_crc >> 8) & 0xff;
	crcL = ret_crc & 0xff;
	//printf("write:buf[0]=0x%x, buf[1]=0x%x\n", crcH, crcL);

	sndbuf[i++] = crcH;
	sndbuf[i++] = crcL;
	// for(int j = 0; j < i; j++) 
	// {
		// LogDebug("[%s:%s %u]  send[%d]=0x%x \n", __FILE__, __func__, __LINE__, j, sndbuf[j]);
	// }

	if (0 != writeDataTty(fd, sndbuf, i)) {
		LogError("[%s:%s %u]  send error \n", __FILE__, __func__, __LINE__);
		return -1;
	}
		
	return 0; 
}

/*************************************************
Function: // WriteMessage_52
Description: // ARM发送读指令
Input: // fd:设备描述符； RegisterAdd:寄存器地址； Num：数据数量N； Data：N个16bit数据
Output: // 无
Return: // 正确：0，错误返回-1
Others: // 其它说明
*************************************************/
//01 52 00 01 ED 61
//ARM发送读指令
int CSerial::WriteMessage_52(int fd, uint8_t RegisterAdd, uint8_t Num)
{
	LogDebug("[%s:%s %u]  WriteMessage_52 \n", __FILE__, __func__, __LINE__);
	uint8_t sndbuf[SERIAL_BUF_SIZE] = {0};
	memset(sndbuf, 0x00, SERIAL_BUF_SIZE);
	// head 0~5
	int i = 0;
	uint16_t crc_len = 0;

	sndbuf[i++] = 0x01;
	sndbuf[i++] = 0x52;
	sndbuf[i++] = RegisterAdd;
	sndbuf[i++] = Num;

	//LogDebug("[%s:%s %u]  num=%d \n", __FILE__, __func__, __LINE__, Num);

	crc_len = i;
	uint16_t ret_crc = crc16(sndbuf, crc_len);
	uint8_t crcH, crcL;
	crcH = (ret_crc >> 8) & 0xff;
	crcL = ret_crc & 0xff;
	//printf("write:buf[0]=0x%x, buf[1]=0x%x\n", crcH, crcL);

	sndbuf[i++] = crcH;
	sndbuf[i++] = crcL;
	// for(int j = 0; j < i; j++) 
	// {
		// LogDebug("[%s:%s %u]  send[%d]=0x%x \n", __FILE__, __func__, __LINE__, j, sndbuf[j]);
	// }

	if (0 != writeDataTty(fd, sndbuf, i)) {
		LogError("[%s:%s %u]  send error \n", __FILE__, __func__, __LINE__);
		return -1;
	}
		
	return 0; 
}

//ARM接收数据
int CSerial::ReadMessage(int fd, uint8_t *pData, int *BufLen)
{
	uint8_t readbuf[SERIAL_BUF_SIZE];
	uint16_t readDataLen = 0;
	uint8_t crcH, crcL;
	uint8_t crcHTmp, crcLTmp;
	int count = 0;
	int ret = -1;

	if (NULL == pData) {
		LogError("[%s:%s %u]  pData NULL \n", __FILE__, __func__, __LINE__);
		return ret;
	}

	memset(readbuf, 0x00, SERIAL_BUF_SIZE);
	memset(pData, 0x00, SERIAL_BUF_SIZE);
	
	readDataLen = readDataTty(fd, readbuf, rcvTimeOut, SERIAL_BUF_SIZE);
	// for (int i = 0; i < readDataLen; i++)
	// {
		// printf("buf[%d]=0x%x\t", i, readbuf[i]);
	// }
	//printf("buf[3]=0x%x\n", readbuf[3]);
	//没有数据
	if (0 == readDataLen) {
		return -2;
	}
//CRC校验
	uint16_t ret_crc = crc16(readbuf, readDataLen - 2);
	crcH = (ret_crc >> 8) & 0xff;
	crcL = ret_crc & 0xff;
	// printf("\n");
	// printf("read1:buf[0]=0x%x, buf[1]=0x%x\n", crcH, crcL);

	crcHTmp = readbuf[readDataLen - 2];
	crcLTmp = readbuf[readDataLen - 1];
	// printf("read2:buf[0]=0x%x, buf[1]=0x%x\n", crcHTmp, crcLTmp);

	if ((crcHTmp == crcH) && (crcLTmp == crcL)) {
		// count = readbuf[3];
		// *BufLen = count * 2;
		// if (0x10 != readbuf[3])
			// printf("=====err：readbuf=%x, count=%d, len=%d\n", readbuf[3], count, *BufLen);

		//memcpy(pData, readbuf + 4, *BufLen);
		*BufLen = readDataLen;
		memcpy(pData, readbuf, readDataLen);
		ret = 0;
	}
	else {
		uint8_t retErr = readbuf[1];
		//校验错误
		if (0x45 == retErr) {
			printf("接收到CRC校验错误\n");
			ret = -1;
		}
		//非法命令
		else if (0x49 == retErr) {
			printf("接收到非法命令\n");
			ret = -1;
		}
	}
	
	return ret;
}

int CSerial::SerialOpen(void)
{
	int iSetOpt = 0;//SetOpt 的增量i
	//openPort
	if ((fdSerial = openPort(1))<0)//1--"/dev/ttyPS1",2--"/dev/ttyS1",3--"/dev/ttyS2",4--"/dev/ttyUSB0" 测试：5--"./test.txt"
	{
		LogError("[%s:%s %u]  serial open_port error \n", __FILE__, __func__, __LINE__);
		return -1;
	}
		
	//setOpt(fdSerial, 115200, 8, 'N', 1)
	if ((iSetOpt = setOpt(fdSerial, 115200, 8, 'N', 1))<0)
	{
		LogError("[%s:%s %u]  serial set_opt error \n", __FILE__, __func__, __LINE__);
		return -1;
	}
	//printf("Serial fdSerial=%d\n", fdSerial);
	tcflush(fdSerial, TCIOFLUSH);//清掉串口缓存
	fcntl(fdSerial, F_SETFL, 0);
	return 0;
}

int CSerial::SerialSend(uint8_t RegisterAdd, uint8_t Num, uint16_t *Data)
{
	int ret = -1;

	if (fdSerial >= 0) {
		if (NULL == Data) {
			//ARM发送读指令
			ret = WriteMessage_52(fdSerial, RegisterAdd, Num);
		}
		else {
			//ARM发送写指令
			ret = WriteMessage_57(fdSerial, RegisterAdd, Num, Data);
			if (0 != ret) {
				LogError("[%s:%s %u]  write Serial error \n", __FILE__, __func__, __LINE__);
				return -2;
			}
		}
	}
	return ret;
}

int CSerial::SerialRecv()
{
	int RetRecv = -1;
	uint8_t Databuf[SERIAL_BUF_SIZE] = {'\0'};
	int BufLen;

	if (fdSerial >= 0) {
		RetRecv = ReadMessage(fdSerial, Databuf, &BufLen);
		if (0 == RetRecv) {
			int n = 1;
			//printf("len=%d\n", BufLen);
			LogDebug("[%s:%s %u]  \n", __FILE__, __func__, __LINE__);
			for (int i = 0; i < BufLen; i++) {
				//printf("Rbuf[%d]=0x%x\t", i, Databuf[i]);
				LogDebug("[%s:%s %u]  Rbuf[%d]=0x%x \n", __FILE__, __func__, __LINE__, i, Databuf[i]);
				// if (n % 10 == 0 )
					// printf("\n");
				// n++;
			}

			memset(Databuf, 0x00, SERIAL_BUF_SIZE);
			RetRecv = -1;
		}
	}
	return 0;
}

void CSerial::SerialClose()
{
	if (fdSerial >= 0)
		close(fdSerial);
}

 