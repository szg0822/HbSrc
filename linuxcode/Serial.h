/*****************************************************************************
File name: Serial.h
Description: ARM和MCU之间通过串口进行通信
Author: sunzhigang
Date: 2021/1/8
*****************************************************************************/

#pragma once
#define uint16_t 	unsigned short 
#define uint8_t 	unsigned char  

#define SERIAL_BUF_SIZE    128

//#define TEST

struct SMessage_t
{
    char CHAdd;         //主机地址
    char CmdId;         //指令
    char CmdRegAdd;     //寄存器地址
    char CmdNum;        //数量
    char CmdData[20];   //数据
    char CRC_H; 		//CRC高
    char CRC_L; 		//CRC低
};

#define SERIAL_BUFF_MAX_SIZE (sizeof(SMessage_t))

class CSerial
{
public:
    CSerial();
    ~CSerial();
    
public:
    int openPort(int comport);      //打开串口（阻塞）
    int setOpt(int fd, int nSpeed, int nBits, char nEvent, int nStop);  //设置串口属性
    int readDataTty(int fd, uint8_t *rcv_buf, int TimeOut, int Len);   //从串口读取数据
    int writeDataTty(int fd, uint8_t *send_buf, int Len);  //写数据到串口
    int WriteMessage_57(int fd, uint8_t RegisterAdd, uint8_t Num, uint16_t *Data);  //ARM发送写指令
    int WriteMessage_52(int fd, uint8_t RegisterAdd, uint8_t Num);  //ARM发送读指令
    int ReadMessage(int fd, uint8_t *pData, int *BufLen);  //ARM接收数据

    SMessage_t *m_pSMessage;
    //串口
    int fdSerial = -1;

    int SerialOpen(void);
    int SerialSend(uint8_t RegisterAdd, uint8_t Num, uint16_t *Data);
    int SerialRecv();
    void SerialClose();

};