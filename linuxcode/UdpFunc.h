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
#define PACKET_PL_SIZE           1052
#define UZID_SIZE 				 9
#define PC_SENDBUF_SIZE			 1049

//协议包
#define OFFSET_PACKAGE_CMD       				9
#define OFFSET_PACKAGE_IMAGESLICE       		19
//FPGA对应参数
#define OFFSET_PARAMETER_PANELSIZE      		2
#define OFFSET_PARAMETER_SATURATION_VALUE       311
#define OFFSET_PARAMETER_OFFSET	      			371
#define OFFSET_PARAMETER_GAIN	      			372
#define OFFSET_PARAMETER_DEFECT	      			373

//HB协议ID
#define HB_ID1		10
#define HB_ID2		11
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
#define FPD_STATUS_SLEEP         12  // Power Down	0x0C
#define FPD_STATUS_WAKEUP        13  // Power On	0x0D
#define FPD_STATUS_IMAGE_NO      14  // EMMC没有存图	0x0E
#define FPD_STATUS_IMAGE_LAST    15  // 下载的最后一张图像	0x0F
#define FPD_STATUS_TOPLIMIT      16  // EMMC存图数量已达上限，不能继续存图	0x10

// #define FPD_UPLOAD_GAIN_ANSWER   0X06  // Upload gain template answer
// #define FPD_UPLOAD_DEFECT_ANSWER 0X07  // Upload defect template answer
// #define SINGLE_FRAME_IMAGE_DATA  0X00  // Single frame image data
// #define MULTI_FRAME_IMAGE_DATA   0X01  // Multi frame image data


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
#define Template_Buf_SIZE		1024 * 1024 * 18	//模板缓冲区大小：18M

#define RECV_TYPE_PACKET_RETRANS        0x06     //数据包重传
#define RECV_TYPE_FRAME_RETRANS         0x07     //整个数据帧重传
#define RECV_TYPE_ERASE_FLASH         	0x4F     //升级软件时擦除软件的命令
#define RECV_TYPE_Firmware_Update       0x50     //下发的软件升级的数据包
#define RECV_TYPE_DOWNLOAD_GAIN       	0x2F     //下载Gain校正模板
#define RECV_TYPE_DOWNLOAD_DEFECT       0x30     //下载Defect校正模板
#define RECV_TYPE_DOWNLOAD_IAMGE		0x0C	 //下载保存在EMMC里的图像
#define RECV_TYPE_POWERDOWN				0x0D	 //进入低功耗模式
#define RECV_TYPE_POWERON				0x0E	 //唤醒低功耗模式
#define RECV_TYPE_READ_PARA				0x13	 //读取参数或连接   后面改0x0B

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
	CMDD_WRITE_PARA= 0x12,\
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

//获取的FPGA参数配置
typedef struct ParameterInfo
{
	UINT offset;				
	UINT gain;
	UINT defect;
	UINT SaturationV;			//饱和度
	UINT PanelSize;				//平板像素大小的选择
	UINT FrameNum;				//要读取探测器内部自动存储图像的编号，默认为1
	UINT SaveEMMC;				//是否把图片保存在EMMC里；1保存，0不保存
	UINT PowerDown;				//低功耗配置：FFBF；正常：0
}parameter_t;


#define FILE_NAME_GAIN 			"/home/root/szg/gain.raw"
#define FILE_NAME_DEFECT 		"/home/root/szg/defect.dld"
#define RAW_IMAGE_PATH			"/media/sd-mmcblk0p2/szg/raw/"

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
	int UploadImageData(TCmdID cmdID, parameter_t ParaInfo);


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

	/*********************************************************
	* 函 数 名: DownloadCurrencyTemplate
	* 功能描述: 下载通用校正模板
	* 参数说明:	recvbuf：接收的每包校正模板数据
			TemplateValue：1，Gain模板；2，Defect模板
	* 返 回 值：0：成功；    
	* 备    注: 
	*********************************************************/
	int DownloadCurrencyTemplate(UCHAR *recvbuf, UINT TemplateValue);

	void UdpFuncInit();

	/*********************************************************
	* 函 数 名: CorrectionOffset
	* 功能描述: Offset固件校正
	* 参数说明: ImageSize:图片大小
	*		   sv:饱和值					
	* 返 回 值：校正数据
	* 备    注:
	*********************************************************/
	UCHAR *CorrectionOffset(UINT ImageSize, UINT SV);

	/*********************************************************
	* 函 数 名: CorrectionGain
	* 功能描述: Gain固件校正
	* 参数说明: pImage:图像数据
	*		   ImageSize:图片大小
	*		   sv:饱和值					
	* 返 回 值：校正数据
	* 备    注:
	*********************************************************/
	UCHAR *CorrectionGain(UCHAR * pImage, UINT ImageSize, UINT SV);

	/*********************************************************
	* 函 数 名: CorrectionDefect
	* 功能描述: Defect固件校正
	* 参数说明: pImage:图片数据
	*		   ImageLenBuf:长宽数据
	*		   PanelSize:平板像素大小选择				
	* 返 回 值：校正数据
	* 备    注:
	*********************************************************/
	UCHAR *CorrectionDefect(UCHAR * pImage, UINT *ImageLenBuf, UINT PanelSize);

	/*********************************************************
	* 函 数 名: SaveImageData
	* 功能描述: 保存图像数据
	* 参数说明: pImage：图像数据	
	*		   ImageSize:数据大小
	*		   FrameNum：帧号
	* 返 回 值：0：成功；!0：失败
	* 备    注:
	*********************************************************/
	int SaveImageData(UCHAR *pImage, int ImageSize);
	/*********************************************************
	* 函 数 名: ReadImageData
	* 功能描述: 保存图像数据
	* 参数说明: Output：[pImage:读取的图像数据]	
	* 返 回 值：数据长度，失败返回-1
	* 备    注:
	*********************************************************/
	int ReadImageData(UCHAR *pImage);

	/*********************************************************
	* 函 数 名: UdpSendImage
	* 功能描述: 把保存图像数据发送给上位机
	* 参数说明: 	
	* 返 回 值：
	* 备    注: 利用文件存储图像数量
	*********************************************************/
	int UdpSendImage();

	//获取当前时间
	void GetStartTime();

	/*********************************************************
	* 函 数 名: InputLowerPower
	* 功能描述: 进入低功耗模式或休眠
	* 参数说明: 	
	* 返 回 值：
	* 备    注: 
	*********************************************************/
	void InputLowerPower();

	/*********************************************************
	* 函 数 名: AwakenLowerPower
	* 功能描述: 唤醒低功耗模式
	* 参数说明: 	
	* 返 回 值：
	* 备    注: 
	*********************************************************/
	void AwakenLowerPower();

	/*********************************************************
	* 函 数 名: MySwitch
	* 功能描述: switch太多，封装一个接口
	* 参数说明: RCmd：上位机下发的命令；	pRecvBuf：下发的数据
	* 返 回 值：
	* 备    注: 
	*********************************************************/
	void MySwitch(UCHAR RCmd, UCHAR *pRecvBuf);

	UCHAR *p_bram_state; 		//映射的状态数据地址
	UCHAR *p_bram_parameter; 	//映射的参数数据地址
	UCHAR *p_bramA_image;		//映射的图像数据A地址
	UCHAR *p_bramB_image;		//映射的图像数据B地址
	UCHAR *p_bram_tail;			//映射的临时地址

	
private:
	FILE *m_pFp;
};
