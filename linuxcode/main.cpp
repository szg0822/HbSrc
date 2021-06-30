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

//static char const *szdstIp = "192.168.10.23";    //目标主机IP
//static char const *szsrcIp = "192.168.10.80";    //本机IP
UdpFunc udpfunc;//线程对象

parameter_t m_ParaInfo;

//FPGA地址
int	fd_bram;
UCHAR *pBramState = NULL;
UCHAR *pBramParameter;
UCHAR *pBramAImage;
UCHAR *pBramBImage;

//udp准备
static int UdpConnect(char *SrcIp)
{	
	//memcpy(udpfunc.remoteip,szdstIp,strlen(szdstIp));
	//memcpy(udpfunc.localip,szsrcIp,strlen(szsrcIp));

	if (ERR_SUCCESS != udpfunc.UDP_CREATE(SrcIp, 2)) { 
		LogError("[%s:%s %u]  UDP Connect Wireless Failed! \n", __FILE__, __func__, __LINE__);
		udpfunc.UDP_CLOSE();
		return -1;
	}
	else{
		LogDebug("[%s:%s %u]  UDP Connect Wireless Success! \n", __FILE__, __func__, __LINE__);
	}
	return 0;
}        

//获取参数
static void GetBramParameter()
{
	UCHAR tmpSVBuf[5] = {0};

//offset(1byte)		offset参数值, 0:不做offset；1：软件offset；2:固件offset
	m_ParaInfo.offset = *(pBramParameter + OFFSET_PARAMETER_OFFSET); 					//取offset参数值
				
//gain(1byte)		
	m_ParaInfo.gain = *(pBramParameter + OFFSET_PARAMETER_GAIN); 					//取gain参数值
				
//defect(1byte)		
	m_ParaInfo.defect = *(pBramParameter + OFFSET_PARAMETER_DEFECT); 					//取defect参数值

//Saturation Value(4byte)
	udpfunc.MyMemcpy(tmpSVBuf, (pBramParameter + OFFSET_PARAMETER_SATURATION_VALUE), 4);
	m_ParaInfo.SaturationV = tmpSVBuf[0] << 24 | tmpSVBuf[1] << 16 | tmpSVBuf[2] << 8 | tmpSVBuf[3];

//Panel_Size(平板像素大小的选择)
	m_ParaInfo.PanelSize = *(pBramParameter + OFFSET_PARAMETER_PANELSIZE);

//要读取探测器内部自动存储图像的编号，默认为1
	udpfunc.MyMemcpy(tmpSVBuf, (pBramParameter + 523), 2);	
	m_ParaInfo.SaveEMMC = tmpSVBuf[0] << 8 | tmpSVBuf[1];

//低功耗参数
	udpfunc.MyMemcpy(tmpSVBuf, (pBramParameter + 447), 2);
	m_ParaInfo.PowerDown = tmpSVBuf[0] << 8 | tmpSVBuf[1];

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
	
	memset(&m_ParaInfo, 0, sizeof(parameter_t));

	while(1)
	{
		usleep(20 * 1000); //20ms
		switch (pBramState[0]) {
			case 3: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				udpfunc.GetStartTime();			//任何用户操作，重新开始计时
				LogDebug("[%s:%s %u]  Recv Fpga 0x03, Parameter \n", __FILE__, __func__, __LINE__);
				udpfunc.UploadParameterData(CMDU_SYSPARA_REPORT);
					//LogDebug("[%s:%s %u]  UploadParameterData success!  \n", __FILE__, __func__, __LINE__);
				GetBramParameter();  //获取参数
				break;
			}
			case 4: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				LogDebug("[%s:%s %u]  Recv Fpga 0x04,  \n", __FILE__, __func__, __LINE__);
				udpfunc.GetStartTime();			//任何用户操作，重新开始计时
				break;
			}
			case 5: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				udpfunc.GetStartTime();			//任何用户操作，重新开始计时
				LogDebug("[%s:%s %u]  Recv Fpga 0x05, SINGLE_SHORT: Offset=%d, Gain=%d, Defect=%d, PSize=%d, SV=%d, Emmc=%d\n", \
						__FILE__, __func__, __LINE__, m_ParaInfo.offset, m_ParaInfo.gain, m_ParaInfo.defect, m_ParaInfo.PanelSize, \
						m_ParaInfo.SaturationV, m_ParaInfo.SaveEMMC);
				if (0 == udpfunc.UploadImageData(CMDU_UPLOAD_IMAGE_SINGLE_SHOT, m_ParaInfo)) {
					udpfunc.UploadStateCmd(CMDU_REPORT, FPD_STATUS_READY);
				}
				break;
			}
			case 6: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				udpfunc.GetStartTime();			//任何用户操作，重新开始计时
				LogDebug("[%s:%s %u]  Recv Fpga 0x06, PREPARE \n", __FILE__, __func__, __LINE__);
				udpfunc.UploadStateCmd(CMDU_REPORT, FPD_STATUS_PREPARE);
				break;
			}
			case 7: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				udpfunc.GetStartTime();			//任何用户操作，重新开始计时
				LogDebug("[%s:%s %u]  Recv Fpga 0x07, BUSY \n", __FILE__, __func__, __LINE__);
				udpfunc.UploadStateCmd(CMDU_REPORT, FPD_STATUS_BUSY);
				break;
			}
			case 8: {
				memset(pBramState, 0xff, BRAM_SIZE_STATE);
				udpfunc.GetStartTime();			//任何用户操作，重新开始计时
				LogDebug("[%s:%s %u]  Recv Fpga 0x08,  \n", __FILE__, __func__, __LINE__);
				//udpfunc.UploadStateCmd(CMDU_REPORT, FPD_STATUS_READY);
				break;
			}
			
			
		}
	}
	LogError("[%s:%s %u]=== UdpSend END ", __FILE__, __func__, __LINE__);
}

int InitDevMem()
{
	UCHAR *pBramTmp;
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
	return 0;
}

int main(int argc, char* argv[])
{
	int ret = 0;
//日志初始化
	InitLinuxLog();
	LogDebug("[%s:%s %u]  ************[[[START RUN LINUX]]]*********** \n", __FILE__, __func__, __LINE__);

//将文件映射到内存	
	ret = InitDevMem();
	if (0 != ret) {
		LogError("[%s:%s %u]  InitDevMem error! ret=%d \n", __FILE__, __func__, __LINE__, ret);
		return -1;
	}

//udp连接
	//UDP无线方式
	char SrcWirelessIp[MAX_IP_LEN] = "192.168.10.80";
	UdpConnect(SrcWirelessIp);


	//udp连接，接收线程
	udpfunc.start();

	//接收FPGA数据，然后发送数据到上位机
	UdpSend();

	//等待线程
	udpfunc.join();

	LogDebug("[%s:%s %u]  ************[[[RUN LINUX END]]]*********** \n", __FILE__, __func__, __LINE__);
	return 0;

}
