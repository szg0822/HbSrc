/**********************************************************
* 文件名: main.cpp
* 功能描述: 程序入口文件
* 作者: 孙志刚
* 日期: 2021/2/9
* 备注：
***********************************************************/


#include <sys/types.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>   
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "LinuxLog.h"
#include "MyUDP.h"
#include "UdpFunc.h"


using namespace std; 
//#define _debug

//#define FILE_NAME "phon.raw"
#define BuffSize 1024


char const *szdstIp = "192.168.0.20";
char const *szsrcIp = "192.168.0.21";
UdpFunc udpfunc;//线程对象


//FPGA地址
int	fd_bram;
UCHAR *pBramState = NULL;
UCHAR *pBramParameter;
UCHAR *pBramAImage;
UCHAR *pBramBImage;

static int pip_fd1[2], pip_fd2[2];
static int fd1_pip, fd2_pip;

//udp准备
static int UdpConnect()
{	
	//udpfunc.remotePort = 1234;
	//udpfunc.loacalPort = 1235;
	#if 1
	memcpy(udpfunc.remoteip,szdstIp,strlen(szdstIp));
	memcpy(udpfunc.localip,szsrcIp,strlen(szsrcIp));
	if (ERR_SUCCESS != udpfunc.UDP_CREATE()) { 
		LogError("[%s:%s %u]  UDP_CREATE failed! \n", __FILE__, __func__, __LINE__);
	}
	else{
		LogDebug("[%s:%s %u]  Create udp server ok \n", __FILE__, __func__, __LINE__);
	}
	#else

	udpfunc.Open("192.168.0.21","192.168.0.20");
	#endif 
	
	return 0;
}        

/*********************************************************
* 函 数 名: UdpSend()
* 功能描述: 阻塞等待外触发（AED等）被动接收信号，然后发送图像数据到上位机
* 参数说明:
* 返 回 值：   
* 备    注:
*********************************************************/
static void UdpSend()
{
	LogDebug("[%s:%s %u]  Wait Fpga .... \n", __FILE__, __func__, __LINE__);

	UINT m_offset = 0;				//offset参数值, 0:不做offset；1：软件offset；2:固件offset
	UINT m_SaturationV = 0;					//固件offset校正算法用到	
	UCHAR tmpSVBuf[5] = {0};
	UINT m_PanelSize = 0;
	while(1)
	{
		usleep(50 * 1000); //20ms
		//usleep(100); //20ms
		
			//printf("pBramState[0]=0x%x\n", pBramState[0]);
		switch (pBramState[0]) {
			case 3: {
				LogDebug("[%s:%s %u]  Recv Fpga 0x03, Parameter \n", __FILE__, __func__, __LINE__);
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				if (0 == udpfunc.UploadParameterData(CMDU_SYSPARA_REPORT))
					LogDebug("[%s:%s %u]  UploadParameterData success!  \n", __FILE__, __func__, __LINE__);
			//offset(1byte)		
				m_offset = *(pBramParameter + OFFSET_PARAMETER_OFFSET); 					//取offset参数值

			//Saturation Value(4byte)
				memcpy(tmpSVBuf, (pBramParameter + OFFSET_PARAMETER_SATURATION_VALUE), 4);
				m_SaturationV = tmpSVBuf[0] << 24 | tmpSVBuf[1] << 16 | tmpSVBuf[2] << 8 | tmpSVBuf[3];

			//Panel_Size(平板像素大小的选择)
				m_PanelSize = *(pBramParameter + OFFSET_PARAMETER_PANELSIZE);

				break;
			}
			case 4: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				LogDebug("[%s:%s %u]  Recv Fpga 0x04,  \n", __FILE__, __func__, __LINE__);
				break;
			}
			case 5: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				LogDebug("[%s:%s %u]  Recv Fpga 0x05, SINGLE_SHORT, m_offset=%d, m_PanelSize=%d \n", __FILE__, __func__, __LINE__, m_offset, m_PanelSize);
				//UCHAR readbuf[2] = {0};
				//UCHAR writebuf[2] = {0};
			//从管道里读取cmd
				// if (read(fd1_pip, readbuf, 1) <= 0) {
					// LogError("[%s:%s %u]  Read Pip Faild, buf=0x%x \n", __FILE__, __func__, __LINE__, readbuf[0]);
					// break;
				// }
				if (0 == udpfunc.UploadImageData(CMDU_UPLOAD_IMAGE_SINGLE_SHOT, m_offset, m_SaturationV, m_PanelSize)) {
					udpfunc.UploadStateCmd(CMDU_REPORT, FPD_STATUS_READY);
				}
				break;
			}
			case 6: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				LogDebug("[%s:%s %u]  Recv Fpga 0x06, PREPARE \n", __FILE__, __func__, __LINE__);
				udpfunc.UploadStateCmd(CMDU_REPORT, FPD_STATUS_PREPARE);
				break;
			}
			case 7: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				LogDebug("[%s:%s %u]  Recv Fpga 0x07, BUSY \n", __FILE__, __func__, __LINE__);
				udpfunc.UploadStateCmd(CMDU_REPORT, FPD_STATUS_BUSY);
				break;
			}
			case 8: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				LogDebug("[%s:%s %u]  Recv Fpga 0x08,  \n", __FILE__, __func__, __LINE__);
				//udpfunc.UploadStateCmd(CMDU_REPORT, FPD_STATUS_READY);
				break;
			}
			
			
		}
	}
	LogError("[%s:%s %u]=== UdpSend END ", __FILE__, __func__, __LINE__);
}

int main(int argc, char* argv[])
{
	UCHAR *pBramTmp;

//日志初始化
	InitLinuxLog();
	LogDebug("[%s:%s %u]  ************[[[START RUN LINUX]]]*********** \n", __FILE__, __func__, __LINE__);

//将文件映射到内存	
	if ((fd_bram = open("/dev/mem", O_RDWR | O_SYNC)) != -1) {
		pBramTmp = (UCHAR *)mmap(NULL, BRAM_TMP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED , fd_bram, FPGA_TMP_ADDRESS);
		//pBramParameter = (UCHAR *)mmap(NULL, BRAM_SIZE_PARAMETER, PROT_READ|PROT_WRITE, MAP_SHARED, fd_bram, FPGA_PARAMETER);
		pBramAImage = (UCHAR *)mmap(NULL, BRAM_SIZE_IMAGE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_bram, FPGA_IMAGE_A_ADDRESS);
		pBramBImage = (UCHAR *)mmap(NULL, BRAM_SIZE_IMAGE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_bram, FPGA_IMAGE_B_ADDRESS);

		if (MAP_FAILED == pBramTmp) {
			LogError("[%s:%s %u]  mmap pBramTmp err=[%s] \n", __FILE__, __func__, __LINE__, strerror(errno));
			return -1;
		}
		if (MAP_FAILED == pBramAImage) {
			LogError("[%s:%s %u]  mmap pBramAImage err=[%s] \n", __FILE__, __func__, __LINE__, strerror(errno));
			return -2;
		}
		pBramState = pBramTmp + 0x8;			//FPGA_ADDRESS+8
		pBramParameter = pBramTmp + 0x1000;		//FPGA_ADDRESS+1000

		udpfunc.p_bram_tail = pBramTmp;
		udpfunc.p_bram_state = pBramState;
		udpfunc.p_bram_parameter = pBramParameter;
		udpfunc.p_bramA_image = pBramAImage;
		udpfunc.p_bramB_image = pBramBImage;

		LogDebug("[%s:%s %u]  open mem ok,fd = %d \n", __FILE__, __func__, __LINE__, fd_bram);
    }
    else{
		LogError("[%s:%s %u]  open mem error \n", __FILE__, __func__, __LINE__);
		close(fd_bram);
		return -3;
    }  

	UCHAR *pSendTmpbuf;
	pSendTmpbuf= (UCHAR *)malloc(PACKET_MAX_SIZE + 1);
	udpfunc.pSendBuf = pSendTmpbuf;

	UCHAR *pSaveRam_C;
	pSaveRam_C = (UCHAR *)malloc(BRAM_SIZE_IMAGE); //在offset固件校正时，bram C保存区缓存
	udpfunc.pSaveRam = pSaveRam_C;


	UCHAR *pUpdateTmpbuf;
	pUpdateTmpbuf = (UCHAR *)malloc(UPDATE_DATA_BUF_SIZE);
	udpfunc.pUpdatedata = pUpdateTmpbuf;
	
#if 0
	//创建读写管道，fd[0]在管道读取，fd[1]在管道写入 
	if(pipe(pip_fd1) < 0)
	{
		LogError("[%s:%s %u]  pip_fd1 error \n", __FILE__, __func__, __LINE__);
	}
	// if(pipe(pip_fd2) < 0)
	// {
		// TERR("pip_fd2 error\n");
	// }

	fd1_pip = pip_fd1[0];
	fd2_pip = pip_fd2[1];
	udpfunc.m_pPipFd1 = &pip_fd1[1];
	udpfunc.m_pPipFd2 = &pip_fd1[0];
#endif

	//udp连接，接收线程
	udpfunc.start();

	//接收FPGA数据，然后发送数据到上位机
	UdpSend();

	//等待线程
	udpfunc.join();

	LogDebug("[%s:%s %u]  ************[[[RUN LINUX END]]]*********** \n", __FILE__, __func__, __LINE__);
	return 0;

}
