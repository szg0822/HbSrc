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

#define BuffSize 1024
#define CONNECT_IP 0

static char const *szdstIp = "192.168.10.20";    //目标主机IP
static char const *szsrcIp = "192.168.10.80";    //本机IP
UdpFunc udpfunc;//线程对象


//FPGA地址
int	fd_bram;
UCHAR *pBramState = NULL;
UCHAR *pBramParameter;
UCHAR *pBramAImage;
UCHAR *pBramBImage;

#if CONNECT_IP
//udp准备
static int UdpConnect(const char *pDstIp)
{	
	//udpfunc.remotePort = 1234;
	//udpfunc.loacalPort = 1235;
	if (NULL == pDstIp) {
		LogError("[%s:%s %u]  pDstIp NULL \n", __FILE__, __func__, __LINE__);
		return -1;
	}
	//printf("dstip=%s, len=%d\n", pDstIp, strlen(pDstIp));
	memcpy(udpfunc.remoteip,pDstIp,strlen(pDstIp));
	memcpy(udpfunc.localip,szsrcIp,strlen(szsrcIp));
	if (ERR_SUCCESS != udpfunc.UDP_CREATE()) { 
		LogError("[%s:%s %u]  UDP Connect Failed! \n", __FILE__, __func__, __LINE__);
		udpfunc.UDP_CLOSE();
		return -1;
	}
	else{
		LogDebug("[%s:%s %u]  UDP Connect Success! \n", __FILE__, __func__, __LINE__);
	}
	return 0;
}        
#endif
//udp准备
static int UdpConnect()
{	
	//memcpy(udpfunc.remoteip,szdstIp,strlen(szdstIp));
	memcpy(udpfunc.localip,szsrcIp,strlen(szsrcIp));

	if (ERR_SUCCESS != udpfunc.UDP_CREATE()) { 
		LogError("[%s:%s %u]  UDP Connect Failed! \n", __FILE__, __func__, __LINE__);
		udpfunc.UDP_CLOSE();
		return -1;
	}
	else{
		LogDebug("[%s:%s %u]  UDP Connect Success! \n", __FILE__, __func__, __LINE__);
	}
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

	UCHAR tmpSVBuf[5] = {0};
	parameter_t ParaInfo;
	
	memset(&ParaInfo, 0, sizeof(parameter_t));

	while(1)
	{
		usleep(20 * 1000); //20ms
		switch (pBramState[0]) {
			case 3: {
				LogDebug("[%s:%s %u]  Recv Fpga 0x03, Parameter \n", __FILE__, __func__, __LINE__);
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				if (0 == udpfunc.UploadParameterData(CMDU_SYSPARA_REPORT))
					LogDebug("[%s:%s %u]  UploadParameterData success!  \n", __FILE__, __func__, __LINE__);

			//offset(1byte)		offset参数值, 0:不做offset；1：软件offset；2:固件offset
				ParaInfo.offset = *(pBramParameter + OFFSET_PARAMETER_OFFSET); 					//取offset参数值
			
			//gain(1byte)		
				ParaInfo.gain = *(pBramParameter + OFFSET_PARAMETER_GAIN); 					//取gain参数值
			
			//defect(1byte)		
				ParaInfo.defect = *(pBramParameter + OFFSET_PARAMETER_DEFECT); 					//取defect参数值

			//Saturation Value(4byte)
				udpfunc.MyMemcpy(tmpSVBuf, (pBramParameter + OFFSET_PARAMETER_SATURATION_VALUE), 4);
				ParaInfo.SaturationV = tmpSVBuf[0] << 24 | tmpSVBuf[1] << 16 | tmpSVBuf[2] << 8 | tmpSVBuf[3];

			//Panel_Size(平板像素大小的选择)
				ParaInfo.PanelSize = *(pBramParameter + OFFSET_PARAMETER_PANELSIZE);

				break;
			}
			case 4: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				LogDebug("[%s:%s %u]  Recv Fpga 0x04,  \n", __FILE__, __func__, __LINE__);
				break;
			}
			case 5: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				LogDebug("[%s:%s %u]  Recv Fpga 0x05, SINGLE_SHORT, Offset=%d, Gain=%d, Defect=%d, PanelSize=%d, SaturationValue=%d \n", \
						__FILE__, __func__, __LINE__, ParaInfo.offset, ParaInfo.gain, ParaInfo.defect, ParaInfo.PanelSize, ParaInfo.SaturationV);
				if (0 == udpfunc.UploadImageData(CMDU_UPLOAD_IMAGE_SINGLE_SHOT, ParaInfo)) {
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

//udp连接
#if CONNECT_IP
	//远程ip可能会发生变化
	if (2 != argc) {
		LogError("[%s:%s %u]  run failed! <%s \"DstIp\"> \n", __FILE__, __func__, __LINE__, argv[0]);
		printf("run failed! <%s \"DstIp\"> \n", argv[0]);
		return -1;
	}
	if (-1 == UdpConnect((const char *)argv[1])) {
		return -1;
	}
#else
	if (-1 == UdpConnect()) {
		return -1;
	}
#endif 

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

		//状态、参数数据地址
		udpfunc.p_bram_tail = pBramTmp;
		udpfunc.p_bram_state = pBramState;
		udpfunc.p_bram_parameter = pBramParameter;

		//图像数据地址
		udpfunc.p_bramA_image = pBramAImage;
		udpfunc.p_bramB_image = pBramBImage;

		LogDebug("[%s:%s %u]  open mem ok,fd = %d \n", __FILE__, __func__, __LINE__, fd_bram);
    }
    else{
		LogError("[%s:%s %u]  open mem error \n", __FILE__, __func__, __LINE__);
		close(fd_bram);
		return -3;
    }  

	//udp连接，接收线程
	udpfunc.start();

	//接收FPGA数据，然后发送数据到上位机
	UdpSend();

	//等待线程
	udpfunc.join();

	LogDebug("[%s:%s %u]  ************[[[RUN LINUX END]]]*********** \n", __FILE__, __func__, __LINE__);
	return 0;

}
