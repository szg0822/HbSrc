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




int main(int argc, char** argv)
{
	int iSetOpt = 0;//SetOpt 的增量i
	int fdSerial = 0;
	CSerial cSerial;

	int RetSend = -1;
	int RetRecv = -1;

	uint8_t SlaveAdd = 0x01;		//从机地址
	uint8_t RegisterAdd = 0x00;		//寄存器地址
	uint8_t Num = 16;				//数量(十进制)
	uint8_t Databuf[BUF_SIZE];
	int BufLen;

	//openPort
	if ((fdSerial = cSerial.openPort(fdSerial, 4))<0)//1--"/dev/ttyS0",2--"/dev/ttyS1",3--"/dev/ttyS2",4--"/dev/ttyUSB0" 测试：5--"./test.txt"
	{
		perror("open_port error");
		return -1;
	}
    
	//setOpt(fdSerial, 115200, 8, 'N', 1)
	if ((iSetOpt = cSerial.setOpt(fdSerial, 9600, 8, 'N', 1))<0)
	{
		perror("set_opt error");
		return -1;
	}
	printf("Serial fdSerial=%d\n", fdSerial);

	tcflush(fdSerial, TCIOFLUSH);//清掉串口缓存
	fcntl(fdSerial, F_SETFL, 0);

	while(1)
	{
		RetSend = cSerial.WriteMessage(fdSerial, SlaveAdd, RegisterAdd, Num);
		if (0 == RetSend) {
			usleep(50 * 1000); //延时50ms等待下位机发送数据
			RetRecv = cSerial.ReadMessage(fdSerial, Databuf, &BufLen);
			if (0 == RetRecv) {
				// int n = 1;
				// printf("len=%d\n", BufLen);
				// for (int i = 0; i < BufLen; i++) {
					// printf("buf[%d]=0x%x\t", i, Databuf[i]);
					// if (n % 10 == 0 )
						// printf("\n");
					// n++;
				// }
				memset(Databuf, 0x00, BUF_SIZE);
				RetRecv = -1;
			}

			RetSend = -1;
		}
		
		sleep(1);
	}

	close(fdSerial);

	return 0;
}
