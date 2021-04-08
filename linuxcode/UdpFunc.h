/**********************************************************
* 文件名: UdpFunc.h
* 功能描述: 接收上位机数据（CMD、Date）
* 作者: 孙志刚
* 日期: 2021/2/9
* 备注：
***********************************************************/

#pragma once

#include "thread.h"
#include "MyUDP.h"

#define UINT 	unsigned int
#define UCHAR 	unsigned char 
#define USHORT 	unsigned short
#define ULONG	unsigned long

#define PACKET_MAX_SIZE          1050
#define UZID_SIZE 				 9
#define PC_SENDBUF_SIZE			 1049

//协议包
#define OFFSET_PACKAGE_CMD       				9
#define OFFSET_PACKAGE_IMAGESLICE       		19
//FPGA对应参数
#define OFFSET_PARAMETER_PANELSIZE      		2
#define OFFSET_PARAMETER_SATURATION_VALUE       311
#define OFFSET_PARAMETER_OFFSET	      			371

//HB协议ID
#define HB_ID3		12
#define HB_ID4		13
#define HB_ID5		14
#define HB_ID6		15
#define HB_ID7		16
#define HB_ID8		17
#define HB_ID9		18

//状态
#define FPD_STATUS_BUSY          0X01  // fpd busy status
#define FPD_STATUS_READY         0X02  // fpd ready status
#define FPD_STATUS_AED           0X03  // AED mode,avild image data
#define FPD_STATUS_PREPARE       0X04  // Software mode,prepare command feedback command
#define FPD_UPLOAD_GAIN_ANSWER   0X06  // Upload gain template answer
#define FPD_UPLOAD_DEFECT_ANSWER 0X07  // Upload defect template answer
#define SINGLE_FRAME_IMAGE_DATA  0X00  // Single frame image data
#define MULTI_FRAME_IMAGE_DATA   0X01  // Multi frame image data


//FPGA地址
#define FPGA_TMP_ADDRESS		0xA0000000   	//状态和参数的映射首地址
#define BRAM_TMP_SIZE 			0x4000			//状态和参数的映射长度
//#define FPGA_STATE    		0xA0000008      //FPGA_ADDRESS+8
//#define FPGA_PARAMETER		0xA0001000		//FPGA_ADDRESS+1000
//A和B图片数据首地址
#define FPGA_IMAGE_A_ADDRESS  	0x7C000000		//图片A数据映射首地址
#define FPGA_IMAGE_B_ADDRESS  	0x7E000000		//图片B数据映射首地址
//图像映射长度
#define BRAM_SIZE_IMAGE			0x2400000		//图片数据映射长度36M

//地址长度
#define BRAM_SIZE_STATE 		4		//状态数据长度
#define TMP_BUFFER_SIZE 		1024	//组包的数据长度（19+1024+‘\0’+6=1050）
#define UPDATE_DATA_BUF_SIZE	1024 * 1024 * 10  //更新软件的数据存放大小


#define RECV_TYPE_PACKET_RETRANS        0x06     //数据包重传
#define RECV_TYPE_FRAME_RETRANS         0x07     //整个数据帧重传
#define RECV_TYPE_ERASE_FLASH         	0x4F     //升级软件时擦除软件的命令
#define RECV_TYPE_Firmware_Update       0x50     //下发的软件升级的数据包
#define RECV_TYPE_DOWNLOAD_GAIN       	0x2F     //下发Gain模板的数据包

//上位机下发的命令
typedef enum tagTCmdID {
	CMDD_GLOBAL_RESET = 1, \
	CMDD_PREPARE = 2, \
	CMDD_SINGLE_SHOT = 3, \
	CMDD_LIVE_ACQ = 4, \
	CMDD_STOP_ACQ = 5, \
	CMDD_PACKET_RETRANS = 6, \
	CMDD_FRAME_RETRANS = 7, \
	CMDD_DUMMPLING = 8, \
	CMDD_READ_DEVSTATUS = 9, \
	CMDD_ACK_REPORT = 0x0a, \
	CMDD_CONFIG_PARA = 0x10, \
	CMDD_READ_PARA= 0x13,\
	CMDU_ACK_WRONG = 1, \
	CMDU_ACK_OK = 2, \
	CMDU_DUMMPLING_END = 3, \
	CMDU_ACQUISITION_END = 4, \
	CMDU_REPORT = 9, \
	CMDU_SYSPARA_REPORT=0x10,\
	CMDU_UPLOAD_IMAGE_SINGLE_SHOT = 0x51, \
	CMDU_UPLOAD_IMAGE_LIVE_ACQ = 0x52, \
	CMDU_UPLOAD_IMAGE_RETRANS = 0x5B
}TCmdID;


class UdpFunc:public Thread,public MyUDP
{
public:
	UdpFunc();
	~UdpFunc();

	//Function
	//接收上位机数据，然后透传fpga；当出现丢包时，这里需要处理
	void run();	

	/*********************************************************
	* 函 数 名: UploadResponseCmd
	* 功能描述: 组包发送状态命令
	* 参数说明:	cmdID：响应命令
	* 返 回 值：0：成功；-1：失败	    
	* 备    注:
	*********************************************************/
	int UploadResponseCmd(TCmdID cmdID);


	/*********************************************************
	* 函 数 名: UploadStateCmd
	* 功能描述: 组包发送状态命令
	* 参数说明:	cmdID：上位机命令
	*			chStatusCode：状态
	* 返 回 值：0：成功；-1：失败	    
	* 备    注:
	*********************************************************/
	int UploadStateCmd(TCmdID cmdID, UCHAR chStatusCode);


	/*********************************************************
	* 函 数 名: UploadParameterData
	* 功能描述: 上传参数数据，组包并UDP发送
	* 参数说明: cmdID：上位机命令
	* 返 回 值：0：成功；-1：失败	
	* 备    注:
	*********************************************************/
	int UploadParameterData(TCmdID cmdID);

	/*********************************************************
	* 函 数 名: UploadImageData
	* 功能描述: 上传图像数据，组包并UDP发送
	* 参数说明: cmdID:上位机命令
	*			offset：0：一帧；1：二帧
	* 返 回 值：0：成功；-1：失败	
	* 备    注:
	*********************************************************/
	int UploadImageData(TCmdID cmdID, UINT offset, UINT SV, UINT PanelSize);


	/*********************************************************
	* 函 数 名: CreateCmd
	* 功能描述: 进行数据组包（1049）
	* 参数说明: cmdID：上位机命令
	*			pStatusCode：上位机运行状态代码
	* 返 回 值： 
	* 备    注:
	*********************************************************/
	void CreateCmd(TCmdID cmdID, UCHAR * pStatusCode);

	//丢包重传
	void PacketRetransmission(UCHAR *recvbuf);
	//丢帧重传
	void FrameRetransmission();

	//软件包下载后保存文件，如果失败，不会更新软件
	int Update_SaveFile(UCHAR *recvbuf);
	//更新Fpga固件
	void Update_FpgaFile();
	//更新嵌入式固件
	void Update_LinuxFile();

	//因固件不支持memcpy（会出现Bus error），自己封装函数
	void * MyMemcpy(void *dest, const void *src, size_t count);

	UCHAR *p_bram_state; 		//映射的状态数据地址
	UCHAR *p_bram_parameter; 	//映射的参数数据地址
	UCHAR *p_bramA_image;		//映射的图像数据A地址
	UCHAR *p_bramB_image;		//映射的图像数据B地址
	UCHAR *p_bram_tail;

	//malloc
	UCHAR *pUpdatedata;           //更新固件的保存数据地址
	UCHAR *pSendBuf;			  //组包后临时存放的地址
	UCHAR *pSaveRam;			  //在offset固件校正时，bram C保存区缓


	int *m_pPipFd1;   			//管道（写）
	int *m_pPipFd2;   			//管道（读）

	
private:
	FILE *m_pFp;
};
