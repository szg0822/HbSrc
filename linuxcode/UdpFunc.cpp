/**********************************************************
* 文件名: UdpFunc.cpp
* 功能描述: 接收上位机数据（CMD、Date）
* 作者: 孙志刚
* 日期: 2021/2/9
* 备注：
***********************************************************/

#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>

#include "UdpFunc.h"
#include "LinuxLog.h"
#include "MyUDP.h"

#define BuffSize 1024
#define TIMEOUT 2       //超时等待2秒

static char const *szdstIp = "192.168.0.20";    //目标主机IP
static char const *szsrcIp = "192.168.0.21";    //本机IP

// static UCHAR *pSendBuf;
// static UCHAR *pSaveRam;
//static UCHAR *pUpdatedata; 		//更新固件的保存数据地址
static UCHAR *pTail;				//指向更新固件的保存数据地址
static UINT mUpPackNumFlag = 0;		//update数据包的个数只需要获取一次就行，这里上个锁

static UINT mOffset = 0;			//offset校正模式
static UINT mImageLen = 0;			//图像像素

static UINT mPackCount = 0;			//UpdateFirmware 包的数量

UdpFunc::UdpFunc()
{}
UdpFunc::~UdpFunc()
{}


/*********************************************************
* 函 数 名: CreateCmd
* 功能描述: 进行数据组包（1049）
* 参数说明: cmdID：上位机命令
			pStatusCode：上位机运行状态代码
* 返 回 值： 
* 备    注:
*********************************************************/
void UdpFunc::CreateCmd(TCmdID cmdID, UCHAR * pStatusCode)
{	
	int i = 0;

	memset(pSendBuf,0,PACKET_MAX_SIZE + 1);
	pSendBuf[i++] = 0xaa;
	pSendBuf[i++] = 0xbb;
	pSendBuf[i++] = 0xcc;
	pSendBuf[i++] = 0xdd;
	pSendBuf[i++] = 0x00;
	pSendBuf[i++] = 0x00;
	pSendBuf[i++] = 0x00;
	pSendBuf[i++] = 0x00;
	pSendBuf[i++] = 0xab;
	//Control
	pSendBuf[i++] = (UCHAR )cmdID;

	// create parameter
	i += 9;

	if (NULL != pStatusCode)
	{
		pSendBuf[i + 4] = *pStatusCode;
	}
	i += 1024;
	//CRC
	pSendBuf[i++] = 0xff;
	
	// tail
	pSendBuf[i++] = 0xba;

	pSendBuf[i++] = 0xdd;
	pSendBuf[i++] = 0xcc;
	pSendBuf[i++] = 0xbb;
	pSendBuf[i++] = 0xaa;
}


/*********************************************************
* 函 数 名: UploadParameterData
* 功能描述: 上传参数数据，组包并UDP发送
* 参数说明: cmdID：上位机命令
* 返 回 值：0：成功；-1：失败	
* 备    注:
*********************************************************/
int UdpFunc::UploadParameterData(TCmdID cmdID)
{
	memset(pSendBuf, 0x00, PACKET_MAX_SIZE + 1);
#if 0
	for(int i=0; i<1024; i++) {
		printf("0x%2x\t", p_bram_parameter[i]);
		if(i % 10 == 0)
			printf("\n");
	}
#endif
	// printf("\nFPGAbuf:\n");
	// for(int i=310; i<=330; i++) {
		// printf("0x%2x\t", p_bram_parameter[i]);
		// if(i % 10 == 0)
			// printf("\n");
	// }
	CreateCmd(cmdID, NULL);
	memcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], p_bram_parameter, TMP_BUFFER_SIZE);

	if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
		LogError("[%s:%s %u]  UDP_SEND failed! \n", __FILE__, __func__, __LINE__);
		return -1;
	}
	return 0;
}


/*********************************************************
* 函 数 名: UploadImageData
* 功能描述: 上传图像数据，组包并UDP发送
* 参数说明: cmdID:上位机命令
*			offset：0：不做offset校正；1：软件校正；2：固件校正
*			SV：饱和值
*			PanelSize：1:3072*3072（4343）；2：2560*3072（3543）
* 返 回 值：0：成功；-1：失败	
* 备    注:
*********************************************************/
int UdpFunc::UploadImageData(TCmdID cmdID, UINT offset, UINT SV, UINT PanelSize)
{
	UINT len = 0;
	USHORT packageNo = 0;		//包号
	UCHAR *pImage = NULL;
	UINT ImageLen = 0;			//像素值，2个字节一个像素点

	memset(pSaveRam, 0x00, BRAM_SIZE_IMAGE);

//安全考虑
	if ((offset < 0) || (offset > 3)) 
		offset = 0;

	if ((PanelSize <= 0) || (PanelSize > 8))
		PanelSize = 1;
#if 1
	UINT ImageLenBuf[16] = {3072 * 3072, 2560 * 3072, 1280 * 1024, 2048 * 2048, 2816 * 3584, 2048 * 1792, 4302 * 4302, 3072 * 3840};
	ImageLen = ImageLenBuf[PanelSize - 1];

#else
//PannelSize选择
	switch(PanelSize) {
		case 1: 
			ImageLen = 3072 * 3072;
			break;
		case 2: 
			ImageLen = 2560 * 3072;
			break;
		case 3: 
			ImageLen = 1280 * 1024;
			break;
		case 4: 
			ImageLen = 2048 * 2048;
			break;
		case 5: 
			ImageLen = 2816 * 3584;
			break;
		case 6: 
			ImageLen = 2048 * 1792;
			break;
		case 7: 
			ImageLen = 4302 * 4302;
			break;
		case 8: 
			ImageLen = 3072 * 3840;
			break;		
	}
#endif
//给丢包使用校正模式
	mOffset = offset;	
//给丢包使用图像像素
	mImageLen = ImageLen;

//offset选择
//不做校正
	if (0 == offset) {
		pImage = p_bramA_image;
	}
//固件校正
	if (2 == offset) {
		//俩帧合为一帧
		int j = 0;
		USHORT C2;
		USHORT *TmpA = (USHORT *)p_bramA_image;
		USHORT *TmpB = (USHORT *)p_bramB_image;

		for (int i = 0; i < ImageLen; i++) {
			//算法需要实现,俩个字节运算

			USHORT A2 = TmpA[i];
			USHORT B2 = TmpB[i];

			if (A2 >= SV) {
				C2 = SV;
			}
			else {
				if (A2 >= B2)
					C2 = A2 - B2 + 100;
				else if ((A2 < B2) && (B2 - A2 <= 100))
					C2 = 100 - (B2 - A2);
				else if ((A2 < B2) && (B2 - A2 > 100))
					C2 = 100;
			}

			pSaveRam[j] = C2 & 0xff;
			pSaveRam[j + 1] = (C2 >> 8) & 0xff;
			j += 2;
		}
		pSaveRam[ImageLen * 2] = '\0';
		pImage = pSaveRam;
	}
	
	while (1)
	{	
		//一帧图像（3072*3072）像素点，然后除以512得出18432k，1K组一个包发送
		if (packageNo == (ImageLen / 512)) {
			break;
		}
		
		CreateCmd(cmdID, NULL);
		pSendBuf[HB_ID7] = (packageNo ) & 0xff;
		pSendBuf[HB_ID8] = (packageNo >> 8) & 0xff;
		pSendBuf[HB_ID9] = (packageNo >> 16) & 0xff;

		memcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], pImage, TMP_BUFFER_SIZE);
		
		if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
			LogError("[%s:%s %u]=== UDP_SEND failed!", __FILE__, __func__, __LINE__);
			return -1;
		}
		packageNo++;
		pImage += TMP_BUFFER_SIZE;
		usleep(20); //降低发送速度
		//printf("packageNo=%d,ImageLen/2=%d\n", packageNo, ImageLen / 512);
	}
	LogDebug("[%s:%s %u]  UploadImageData success! \n", __FILE__, __func__, __LINE__);
	return 0;
}

/*********************************************************
* 函 数 名: UploadResponseCmd
* 功能描述: 组包发送状态命令
* 参数说明:	cmdID：响应命令
* 返 回 值：0：成功；-1：失败	    
* 备    注:
*********************************************************/
int UdpFunc::UploadResponseCmd(TCmdID cmdID)
{
	CreateCmd(cmdID, NULL);
	if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
		LogError("[%s:%s %u]=== UDP_SEND failed!", __FILE__, __func__, __LINE__);
		return -1;
	}
	return 0;
}

/*********************************************************
* 函 数 名: UploadStateCmd
* 功能描述: 组包发送状态命令
* 参数说明:	cmdID：上位机命令
*			chStatusCode：状态
* 返 回 值：0：成功；-1：失败	    
* 备    注:
*********************************************************/
int UdpFunc::UploadStateCmd(TCmdID cmdID, UCHAR chStatusCode)
{
	CreateCmd(cmdID, &chStatusCode);
	if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
		LogError("[%s:%s %u]=== UDP_SEND failed!", __FILE__, __func__, __LINE__);
		return -1;
	}
	return 0;
}

void UdpFunc::PacketRetransmission(UCHAR *recvbuf)
{
	UCHAR *TmpBram;
	UINT PackNum = 0;		//丢包的数量
	UINT PackId = 0;		//包号
	ULONG len = 0;
	LogDebug("[%s:%s %u]  Start Packet Retransmission \n", __FILE__, __func__, __LINE__);

	if (recvbuf == NULL) {
		LogError("[%s:%s %u]  recvbuf NULL \n", __FILE__, __func__, __LINE__);
		return;
	}
	if (0 == mOffset) {
		TmpBram = p_bramA_image;
	}
	else if (2 == mOffset) {
		TmpBram = pSaveRam;
	}
	else
		TmpBram = p_bramA_image;

	//丢包的数量
	PackNum = (recvbuf[17] << 8) | recvbuf[18];
	//包号
	PackId = 0;
	for (int i = 0; i < PackNum; i++) {
		PackId = recvbuf[19 + i];
		len = PackId * TMP_BUFFER_SIZE;
		TmpBram += len;
		CreateCmd(CMDU_UPLOAD_IMAGE_SINGLE_SHOT,  NULL);
		pSendBuf[HB_ID7] = (PackId ) & 0xff;
		pSendBuf[HB_ID8] = (PackId >> 8) & 0xff;
		pSendBuf[HB_ID9] = (PackId >> 16) & 0xff;
		memcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], TmpBram, TMP_BUFFER_SIZE);
		if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
			LogError("[%s:%s %u]  UDP_SEND Failed! \n", __FILE__, __func__, __LINE__);
		}
	}
	UploadStateCmd(CMDU_REPORT, FPD_STATUS_READY);
	LogDebug("[%s:%s %u]  Packet Retransmission Success! Packet_num=%d \n", __FILE__, __func__, __LINE__, PackNum);	
}


void UdpFunc::FrameRetransmission()
{
	UCHAR *TmpBram;
	USHORT packageNo = 0;
	UCHAR *pImage;

	LogDebug("[%s:%s %u]  Start Frame Retransmission \n", __FILE__, __func__, __LINE__);
	if (0 == mOffset) {
		TmpBram = p_bramA_image;
	}
	else if ( 2 == mOffset) {
		TmpBram = pSaveRam;
	}
	else
		TmpBram = p_bramA_image;

	while (1)
	{	
		//一帧图像（3072*3072）像素点，然后除以512得出18432k，1K组一个包发送
		if (packageNo == (mImageLen / 512)) {
			break;
		}
		CreateCmd(CMDU_UPLOAD_IMAGE_SINGLE_SHOT, NULL);
		pSendBuf[HB_ID7] = (packageNo ) & 0xff;
		pSendBuf[HB_ID8] = (packageNo >> 8) & 0xff;
		pSendBuf[HB_ID9] = (packageNo >> 16) & 0xff;
		memcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], TmpBram, TMP_BUFFER_SIZE);
		if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
			LogError("[%s:%s %u]=== UDP_SEND failed!", __FILE__, __func__, __LINE__);
		}
		packageNo++;
		TmpBram += TMP_BUFFER_SIZE;
		usleep(20); //降低发送速度
	}
	UploadStateCmd(CMDU_REPORT, FPD_STATUS_READY);
	LogDebug("[%s:%s %u]  Frame Retransmission Success! Packet_num=%d \n", __FILE__, __func__, __LINE__, packageNo);
}

int HBExecShell(const char *shell) 
{
	int ret;
	if (shell == NULL)
		return -1;

	ret = system(shell);
	if (ret == -1 || ret == 127) {
		return -1;
	}
	return 0;
}

int UdpFunc::UpdateFirmware(UCHAR *recvbuf)
{
	static UINT PackNum;
	FILE *fp;
	UCHAR *pRecvCmd;
	
	if (NULL == recvbuf) {
		LogError("[%s:%s %u]  recvbuf NULL \n", __FILE__, __func__, __LINE__);
		return -1;
	}
	if (NULL == pTail) {
		LogError("[%s:%s %u]  pTail NULL \n", __FILE__, __func__, __LINE__);
		return -2;
	}
	//ID8+ID9=包的数量
	//每次升级只进来一次
	if (mUpPackNumFlag == 0) {
		pRecvCmd = (UCHAR *)(recvbuf + OFFSET_PACKAGE_CMD);
		LogDebug("[%s:%s %u]  Recv XDStatic Cmd=0x%.2x \n", __FILE__, __func__, __LINE__,  *(pRecvCmd));
		PackNum = (recvbuf[HB_ID8] << 8) | recvbuf[HB_ID9];
		LogDebug("[%s:%s %u]  Total_PackNum = %d\n", __FILE__, __func__, __LINE__, PackNum);
		mUpPackNumFlag = 1;
	}
		
	memcpy(pTail, recvbuf + OFFSET_PACKAGE_IMAGESLICE, TMP_BUFFER_SIZE);
	pTail += TMP_BUFFER_SIZE;

	mPackCount++;
	
	//printf("packnum=%d, count=%d\n", PackNum, mPackCount);
	if (mPackCount == PackNum) {
		pTail = pUpdatedata;
		if((fp=fopen("/home/root/UpFire.bit","wt+"))==NULL)
		{
			LogError("[%s:%s %u]  open </home/root/UpFire.bit> file error \n", __FILE__, __func__, __LINE__);
			return -1;
		}
		fwrite(pUpdatedata, sizeof(UCHAR), PackNum * TMP_BUFFER_SIZE, fp);			
		fclose(fp);
	#if 1 //update System_Top.bit
		HBExecShell("mv -f /home/root/System_Top.bit /home/root/System_Top.bit_save");
		int ret = HBExecShell("mv -f /home/root/UpFire.bit /home/root/System_Top.bit");
		if (ret == 0) {
			LogDebug("[%s:%s %u]  Recv_PackCount=%d\tUpdateFirmware </home/root/System_Top.bit> successfun!\n", __FILE__, __func__, __LINE__, mPackCount);
			HBExecShell("reboot");
		}else {
			HBExecShell("mv -f /home/root/System_Top.bit_save /home/root/System_Top.bit");
		}
	#else //update demo
		int ret = HBExecShell("kill $(pidof demo)");
		if (0 == ret) {
			HBExecShell("mv -f /home/root/UpFire.bit /home/root/demo");
			LogDebug("[%s:%s %u]  UpdateFirmware </home/root/demo> successfun!\n", __FILE__, __func__, __LINE__);
		}
	#endif
		
		//LogDebug("[%s:%s %u]  Recv_PackCount = %d\n", __FILE__, __func__, __LINE__, mPackCount);
	}
	return 0;
}

void UdpFunc::run()
{
	int iRet;
	int length = 0;
	fd_set m_fsRead;
	UCHAR *m_pRecvCmd;  //接收的CMD
	UCHAR recvbuf[PC_SENDBUF_SIZE + 3] = { 0 };
	int TmpPrint = 0;
	
	LogDebug("[%s:%s %u]  UdpFunc RUN \n", __FILE__, __func__, __LINE__);

	memcpy(remoteip,szdstIp,strlen(szdstIp));
	memcpy(localip,szsrcIp,strlen(szsrcIp));
	if (ERR_SUCCESS != UDP_CREATE()) { 
		LogError("[%s:%s %u]  UDP Connect Failed! \n", __FILE__, __func__, __LINE__);
		UDP_CLOSE();
		return;
	}
	else{
		LogDebug("[%s:%s %u]  UDP Connect Success! \n", __FILE__, __func__, __LINE__);
	}

	pTail = pUpdatedata;

	while(1){
		length = UDP_RECV((UCHAR *)recvbuf, PACKET_MAX_SIZE);
		if (length == PC_SENDBUF_SIZE) {
			p_bram_parameter = p_bram_tail + 0x1000;		//FPGA_ADDRESS+1000

			memcpy(p_bram_parameter, recvbuf, PC_SENDBUF_SIZE + 3); //透传1049+3个0x00,共1052;

			m_pRecvCmd = (UCHAR *)(recvbuf + OFFSET_PACKAGE_CMD); //CMD
			if (*(m_pRecvCmd) != RECV_TYPE_Firmware_Update) 
				LogDebug("[%s:%s %u]  Recv XDStatic Cmd=0x%.2x \n", __FILE__, __func__, __LINE__,  *(m_pRecvCmd));

		//擦除frame（每次update，都会先发一次0x4f；我这里只做回应）
			if (*(m_pRecvCmd) == RECV_TYPE_ERASE_FLASH) {
				UploadResponseCmd(CMDD_FRAME_RETRANS);
				//上一次update失败后，下一次需要恢复参数数据
				mPackCount = 0;
				memset(pUpdatedata, 0xff, UPDATE_DATA_BUF_SIZE);
				pTail = pUpdatedata;
				if (mUpPackNumFlag == 1) {
					mUpPackNumFlag = 0;
				}
			}	
		//update firmware
			if (*(m_pRecvCmd) == RECV_TYPE_Firmware_Update) {
				if ( 0 == UpdateFirmware(recvbuf)) {
					UploadResponseCmd(CMDD_PACKET_RETRANS);
				}		
			}

		//将接收到的cmd写入管道，以便UdpSend()函数使用。过滤下面3个cmd	
			// if ((*(m_pRecvCmd) == PC_COMMON_TYPE_SINGLE_SHORT) || (*(m_pRecvCmd) == PC_COMMON_TYPE_LIVE_ACQ) || (*(m_pRecvCmd) == PC_COMMON_TYPE_STOP_ACQ))
				// write( *(int*)m_pPipFd1, m_pRecvCmd, 1);  

		//丢包重传
			if (*(m_pRecvCmd) == RECV_TYPE_PACKET_RETRANS) {
				PacketRetransmission(recvbuf);
			}

		//整帧重传
			else if (*(m_pRecvCmd) == RECV_TYPE_FRAME_RETRANS) {
				FrameRetransmission();
			}

			memset(recvbuf, 0x00, PC_SENDBUF_SIZE + 3);

		}
	}
                          
	LogError("[%s:%s %u]  UdpFunc END \n", __FILE__, __func__, __LINE__);
	if (NULL != pSendBuf) {
		free(pSendBuf);
		pSendBuf = NULL;
	}

	if (NULL != pSaveRam) {
		free(pSaveRam);
		pSaveRam = NULL;
	}

	if (NULL != pUpdatedata) {
		free(pUpdatedata);
		pUpdatedata = NULL;
	}

}
