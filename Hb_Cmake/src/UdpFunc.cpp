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

static UCHAR *pSendBuf;				//组包后临时存放的地址
//update
static UCHAR *pUpdatedata = NULL; 			//更新固件的保存数据地址
static UCHAR *pTail;				//指向更新固件的保存数据地址
static UINT mUpPackNumFlag = 0;		//update数据包的个数只需要获取一次就行，这里上个锁
static UINT mUpSuccess = 0;			//update数据包成功的标志

//校正模板
static UCHAR *pSaveRam;				//在offset固件校正时，bram C保存区缓存
static UCHAR *pTemplateBuf = NULL;	//Gain模板指针
static UCHAR *pGainTail;			//指向下载模板的保存数据地址

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
	CreateCmd(cmdID, NULL);
	MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], p_bram_parameter, TMP_BUFFER_SIZE);

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
int UdpFunc::UploadImageData(TCmdID cmdID, parameter_t ParaInfo)
{
	UINT len = 0;
	USHORT packageNo = 0;		//包号
	UCHAR *pImage = NULL;
	UINT ImageLen = 0;			//像素值，2个字节一个像素点
	UINT offset = 0;
	UINT gain = 0;
	UINT defect = 0;
	UINT PanelSize = 1;
	UINT SV = 100;

	USHORT * ipPtr;
	FILE * fpFile;
	long lSize,lCount;

	//必须初始化，否则free会段错误
	UCHAR *pTmpGainBuf = NULL;
	UCHAR *pTmpDefectBuf = NULL;

	memset(pSaveRam, 0x00, BRAM_SIZE_IMAGE);

	offset = ParaInfo.offset;
	gain = ParaInfo.gain;
	defect = ParaInfo.defect;
	PanelSize = ParaInfo.PanelSize;
	SV = ParaInfo.SaturationV;

//安全考虑
	if ((offset < 0) || (offset > 3)) 
		offset = 0;

	if ((PanelSize <= 0) || (PanelSize > 8))
		PanelSize = 1;

	//UINT ImageLenBuf[2 * 8] = {3072 * 3072, 2560 * 3072, 1280 * 1024, 2048 * 2048, 2816 * 3584, 2048 * 1792, 4302 * 4302, 3072 * 3840};
	UINT ImageLenBuf[2 * 8] = {3072 , 3072, 2560 , 3072, 1280 , 1024, 2048 , 2048, 2816 , 3584, 2048 , 1792, 4302 , 4302, 3072 , 3840};
	//公式：ImageLenBuf[2*(n-1)] * ImageLenBuf[2*(n-1) + 1]
	//x * y
	ImageLen = ImageLenBuf[2 * PanelSize - 2] * ImageLenBuf[2 * PanelSize - 1];

//给丢包使用校正模式
	mOffset = offset;	
//给丢包使用图像像素
	mImageLen = ImageLen;

//offset选择
	//修改：当offset为1或3时，没有实现，需要赋值为0，否则上不了图
	if ((1 == offset) || (3 == offset)) {
		offset = 0;
	}
//不做校正
	if (0 == offset) {
		pImage = p_bramA_image;
	}
//offset固件校正
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

	//Gain模板固件校正
		if (2 == gain) {
			fpFile = fopen(FILE_NAME_GAIN, "r");
			if (NULL == fpFile) {
				LogError("[%s:%s %u]  No template downloaded (file<%s> failed!) \n", __FILE__, __func__, __LINE__, FILE_NAME_GAIN);
			}
			else {
			//申请Gain校正后的地址
				pTmpGainBuf = (UCHAR *)malloc(ImageLen * 2);
				memset(pTmpGainBuf, 0, ImageLen * 2);

			//拷贝Gain校正模板数据
				fseek(fpFile, 0L, SEEK_END);
				lSize = ftell(fpFile);
				lCount = lSize / sizeof(USHORT);
				ipPtr = (USHORT*)malloc(lSize);
				memset(ipPtr, 0, lSize);
				fread(ipPtr,sizeof(USHORT),lCount,fpFile);
				fclose(fpFile);

				USHORT TMP_Gain = 0;
				int j = 0;
				for (int i = 0; i < ImageLen; i++) {
					TMP_Gain = (pSaveRam[i] / ipPtr[i]) * 1024;
					if (TMP_Gain >= SV) {
						TMP_Gain = SV;
					}
					pTmpGainBuf[j] = TMP_Gain & 0xff;
					pTmpGainBuf[j + 1] = (TMP_Gain >> 8) & 0xff;
					j += 2;
				}
				free(ipPtr);
				ipPtr = NULL;

				pTmpGainBuf[ImageLen * 2] = '\0';
				pImage = pTmpGainBuf;


			//Defect模板固件校正
				if (2 == defect) {
					fpFile = fopen(FILE_NAME_DEFECT, "r");
					if (NULL == fpFile) {
						LogError("[%s:%s %u]  No template downloaded (file<%s> failed!) \n", __FILE__, __func__, __LINE__, FILE_NAME_DEFECT);
					}
					else {
						//ImageLenBuf[2 * PanelSize - 2] * ImageLenBuf[2 * PanelSize - 1];
						UINT TmpLenX = ImageLenBuf[2 * PanelSize - 2];
						UINT TmpLenY = ImageLenBuf[2 * PanelSize - 1];

						USHORT pTGain[TmpLenX][TmpLenY] = {0};
						memcpy(&pTGain, (USHORT *)pTmpGainBuf, ImageLen);
						
						

						typedef struct defect {
							unsigned x:12;
							unsigned y:12;
							unsigned de:8;
						}defect_t;

						defect_t * Pdefect;
						UINT iTmp[8];
						UINT sum = 0;

						//拷贝Gain校正模板数据
						fseek(fpFile, 0L, SEEK_END);
						lSize = ftell(fpFile);
						lCount = lSize / sizeof(defect_t);
						Pdefect = (defect_t*)malloc(lSize);
						memset(Pdefect, 0, lSize);
						fread(Pdefect, sizeof(defect_t), lCount, fpFile);
						fclose(fpFile);

						for(int i = 0; i < lCount; i++) {
							int count = 0;
							int TmpJ[8];
							for(int j = 0; j < 8; j++) {
								iTmp[j] = ((Pdefect[i].de >> j) & 1);
								printf("tmp[%d]=%d", j , iTmp[j]);
								
								if (1 == iTmp[j]) {
									count++;	
									if (0 == j)
										sum += pTGain[Pdefect[i].x - 1][Pdefect[i].y - 1];	
									else if (1 == j)
										sum += pTGain[Pdefect[i].x][Pdefect[i].y - 1];
									else if (2 == j)
										sum += pTGain[Pdefect[i].x + 1][Pdefect[i].y - 1];
									else if (3 == j)
										sum += pTGain[Pdefect[i].x - 1][Pdefect[i].y];
									else if (4 == j)
										sum += pTGain[Pdefect[i].x + 1][Pdefect[i].y];
									else if (5 == j)
										sum += pTGain[Pdefect[i].x - 1][Pdefect[i].y + 1];
									else if (6 == j)
										sum += pTGain[Pdefect[i].x][Pdefect[i].y + 1];
									else if (7 == j)
										sum += pTGain[Pdefect[i].x + 1][Pdefect[i].y + 1];
								}
							}

							pTGain[Pdefect[i].x][Pdefect[i].y] = sum / count;	
						}

						//申请Defect校正后的地址
						pTmpDefectBuf = (UCHAR *)malloc(ImageLen * 2);
						memset(pTmpDefectBuf, 0, ImageLen * 2);
						memcpy(pTmpDefectBuf, (UCHAR *)pTGain, ImageLen * 2);

						pTmpDefectBuf[ImageLen * 2] = '\0';
						pImage = pTmpDefectBuf;

						free(Pdefect);
						Pdefect = NULL;
					}
				}
			}
			
		}
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

		MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], pImage, TMP_BUFFER_SIZE);
		
		if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
			LogError("[%s:%s %u]=== UDP_SEND failed!", __FILE__, __func__, __LINE__);
			if (NULL != pTmpGainBuf) {
				free(pTmpGainBuf);
				pTmpGainBuf = NULL;
			}
			if (NULL != pTmpDefectBuf) {
				free(pTmpDefectBuf);
				pTmpDefectBuf = NULL;
			}
			return -1;
		}
		packageNo++;
		pImage += TMP_BUFFER_SIZE;
		//usleep(5); //降低发送速度
		//printf("packageNo=%d,ImageLen/2=%d\n", packageNo, ImageLen / 512);
	}
	LogDebug("[%s:%s %u]  UploadImageData success! \n", __FILE__, __func__, __LINE__);

	if (NULL != pTmpGainBuf) {
		free(pTmpGainBuf);
		pTmpGainBuf = NULL;
	}

	if (NULL != pTmpDefectBuf) {
		free(pTmpDefectBuf);
		pTmpDefectBuf = NULL;
	}

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
	USHORT PackNum = 0;		//丢包的数量
	USHORT PackId;		//包号
	USHORT len = 0;
	LogDebug("[%s:%s %u]  Start Packet Retransmission \n", __FILE__, __func__, __LINE__);
	UINT count = 0;

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
	PackNum = (recvbuf[HB_ID8] & 0xff) | ((recvbuf[HB_ID9] & 0xff) << 8);		//低高

	if ((PackNum > 512) || (PackNum <= 0)) {
		LogError("[%s:%s %u]  failed：PackNum=%d \n", __FILE__, __func__, __LINE__, PackNum);
		return;
	}

	while(1) {	
		if (count == PackNum * 2) {
			if (PackId !=  (mImageLen / 512 - 1) ) {
				LogError("[%s:%s %u]  Package tail failed! PackId=%d \n", __FILE__, __func__, __LINE__, PackId);
			}
			break;
		}

		PackId = ((recvbuf[19 + count] & 0xff) << 8) | (recvbuf[19 + count + 1] & 0xff);				//两个字节组成一个包号
		if (PackId > (mImageLen / 512 - 1)) {
			LogError("[%s:%s %u]  Package failed! PackId=%d \n", __FILE__, __func__, __LINE__, PackId);
			break;
		}
		
		len = PackId * TMP_BUFFER_SIZE;
		TmpBram += len;

		CreateCmd(CMDU_UPLOAD_IMAGE_RETRANS,  NULL);		//修改丢包重传是0x5B
		pSendBuf[HB_ID7] = (PackId ) & 0xff;
		pSendBuf[HB_ID8] = (PackId >> 8) & 0xff;
		pSendBuf[HB_ID9] = (PackId >> 16) & 0xff;

		MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], TmpBram, TMP_BUFFER_SIZE);

		if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
			LogError("[%s:%s %u]  UDP_SEND Failed! \n", __FILE__, __func__, __LINE__);
		}
		//usleep(5); //降低发送速度

		count += 2;		//一步2个字节
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
		CreateCmd(CMDU_UPLOAD_IMAGE_RETRANS, NULL);
		pSendBuf[HB_ID7] = (packageNo ) & 0xff;
		pSendBuf[HB_ID8] = (packageNo >> 8) & 0xff;
		pSendBuf[HB_ID9] = (packageNo >> 16) & 0xff;
		MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], TmpBram, TMP_BUFFER_SIZE);
		if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
			LogError("[%s:%s %u]=== UDP_SEND failed!", __FILE__, __func__, __LINE__);
		}
		packageNo++;
		TmpBram += TMP_BUFFER_SIZE;
		//usleep(5); //降低发送速度
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

/*********************************************************
* 函 数 名: Update_SaveFile
* 功能描述: 软件包下载后保存文件，如果失败，不会更新软件
* 参数说明:	recvbuf：接收的每包数据
* 返 回 值：0：成功；    
* 备    注: 接收的包的数量采用大端
*********************************************************/
int UdpFunc::Update_SaveFile(UCHAR *recvbuf)
{
	static UINT PackNum;
	FILE *fp;
	UCHAR *pRecvCmd;
	UINT LastPackLen = 0;
	
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
		PackNum = ((recvbuf[HB_ID8] & 0xff) << 8) | (recvbuf[HB_ID9] & 0xff);		//大端，高低

		LogDebug("[%s:%s %u]  Total_PackNum = %d\n", __FILE__, __func__, __LINE__, PackNum);
		mUpPackNumFlag = 1;
	}
	if (mPackCount == PackNum - 1) {
		LastPackLen = ((recvbuf[HB_ID6] & 0xff) << 8) | (recvbuf[HB_ID7] & 0xff);
		MyMemcpy(pTail, recvbuf + OFFSET_PACKAGE_IMAGESLICE, LastPackLen);
		pTail += LastPackLen;
	}	
	else {
		MyMemcpy(pTail, recvbuf + OFFSET_PACKAGE_IMAGESLICE, TMP_BUFFER_SIZE);
		pTail += TMP_BUFFER_SIZE;
	}

	mPackCount++;
	
	//printf("packnum=%d, count=%d\n", PackNum, mPackCount);
	if (mPackCount == PackNum) {
		pTail = pUpdatedata;
		if((fp=fopen("/home/root/UpFile","wt+"))==NULL)
		{
			LogError("[%s:%s %u]  open </home/root/UpFile> file error \n", __FILE__, __func__, __LINE__);
			return -1;
		}
		fwrite(pUpdatedata, sizeof(UCHAR), (PackNum - 1) * TMP_BUFFER_SIZE + LastPackLen, fp);			
		fclose(fp);
		
		LogDebug("[%s:%s %u]  save file[/home/root/UpFile] successful!\n", __FILE__, __func__, __LINE__);
		mUpSuccess = 1;
		mPackCount = 0;

		free(pUpdatedata);
		pUpdatedata = NULL;
	}
	return 0;
}

void UdpFunc::Update_FpgaFile()
{
	//update System_Top.bit
	int ret = -1;
	HBExecShell("cp -f /home/root/System_Top.bit /home/root/System_Top.bit_save");
	ret = HBExecShell("cp -f /home/root/UpFile /home/root/System_Top.bit");
	if (ret == 0) {
		LogDebug("[%s:%s %u]  Update_FpgaFile </home/root/System_Top.bit> successfun! Restarting...\n", __FILE__, __func__, __LINE__);
		HBExecShell("reboot");
	}else {
		HBExecShell("cp -f /home/root/System_Top.bit_save /home/root/System_Top.bit");
	}
}

void UdpFunc::Update_LinuxFile()
{
	//update demo
	int ret = -1;
	ret = HBExecShell("kill $(pidof demo)");
	if (0 == ret) {
		HBExecShell("cp -f /home/root/UpFile /home/root/demo");
		LogDebug("[%s:%s %u]  Update_LinuxFile </home/root/demo> successfun! Restarting...\n", __FILE__, __func__, __LINE__);
		HBExecShell("reboot");
	}
}

void *UdpFunc::MyMemcpy(void *dest, const void *src, size_t count)
{
	char *d;
	const char *s;

	if (dest == NULL || src == NULL) 
        return NULL;

	if ((dest > (src+count)) || (dest < src))
	{
		d = (char *)dest;
		s = (char *)src;
		while (count--)
			*d++ = *s++;        
	}
	else /* overlap */
	{
		d = (char *)(dest + count - 1); /* offset of pointer is from 0 */
		s = (char *)(src + count -1);
		while (count --)
			*d-- = *s--;
	}

	return dest;
}


/*********************************************************
* 函 数 名: DownloadCurrencyTemplate
* 功能描述: 下载通用校正模板
* 参数说明:	recvbuf：接收的每包校正模板数据
		   TemplateValue：1，Gain模板；2，Defect模板
* 返 回 值：0：成功；    
* 备    注: 
*********************************************************/
int UdpFunc::DownloadCurrencyTemplate(UCHAR *recvbuf, UINT TemplateValue)
{
	static UINT PackNum;
	FILE *fp;
	UCHAR *pRecvCmd;
	UINT LastPackLen = 0;
	
	if (NULL == recvbuf) {
		LogError("[%s:%s %u]  recvbuf NULL \n", __FILE__, __func__, __LINE__);
		return -1;
	}

	//下载通用模板只进来一次
	if (mUpPackNumFlag == 0) {
		if (NULL != pTemplateBuf) {
			free(pTemplateBuf);
			pTemplateBuf = NULL;
		}
			
		pTemplateBuf = (UCHAR *)malloc(Gain_Buf_SIZE);
		if (NULL == pTemplateBuf) {
			LogError("[%s:%s %u]  malloc pTemplateBuf NULL \n", __FILE__, __func__, __LINE__);
			return -2;
		}

		memset(pTemplateBuf, 0, Gain_Buf_SIZE);
		pGainTail = pTemplateBuf;
		mPackCount = 0;
		pRecvCmd = (UCHAR *)(recvbuf + OFFSET_PACKAGE_CMD);
		LogDebug("[%s:%s %u]  Recv XDStatic Cmd=0x%.2x \n", __FILE__, __func__, __LINE__,  *(pRecvCmd));
		PackNum = mImageLen / 512;

		LogDebug("[%s:%s %u]  Total_PackNum = %d\n", __FILE__, __func__, __LINE__, PackNum);
		mUpPackNumFlag = 1;
	}

	if (mPackCount == PackNum - 1) {
		LastPackLen = ((recvbuf[HB_ID4] & 0xff) << 8) | (recvbuf[HB_ID5] & 0xff);
		printf("end packlen=%d\n", LastPackLen);
		memcpy(pGainTail, recvbuf + OFFSET_PACKAGE_IMAGESLICE, LastPackLen);
		pGainTail += LastPackLen;
	}	
	else {
		memcpy(pGainTail, recvbuf + OFFSET_PACKAGE_IMAGESLICE, TMP_BUFFER_SIZE);
		pGainTail += TMP_BUFFER_SIZE;
	}

	mPackCount++;
	
	//printf("packnum=%d, count=%d\n", PackNum, mPackCount);
	if (mPackCount == PackNum) {
		pGainTail = pTemplateBuf;
		if (1 == TemplateValue) {
			if((fp=fopen(FILE_NAME_GAIN,"wt+"))==NULL)
			{
				LogError("[%s:%s %u]  open </home/root/Gain.raw> file error \n", __FILE__, __func__, __LINE__);
				free(pTemplateBuf);
				return -1;
			}
			fwrite(pTemplateBuf, sizeof(UCHAR), (PackNum - 1) * TMP_BUFFER_SIZE + LastPackLen, fp);			
			fclose(fp);

			LogDebug("[%s:%s %u]  save file[/home/root/Gain.raw] successful!\n", __FILE__, __func__, __LINE__);
		}
		else if (2 == TemplateValue) {
			if((fp=fopen(FILE_NAME_DEFECT,"wt+"))==NULL)
			{
				LogError("[%s:%s %u]  open </home/root/Defect.raw> file error \n", __FILE__, __func__, __LINE__);
				free(pTemplateBuf);
				return -1;
			}
			fwrite(pTemplateBuf, sizeof(UCHAR), (PackNum - 1) * TMP_BUFFER_SIZE + LastPackLen, fp);			
			fclose(fp);
			LogDebug("[%s:%s %u]  save file[/home/root/Defect.raw] successful!\n", __FILE__, __func__, __LINE__);
		}

		mPackCount = 0;
		free(pTemplateBuf);
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
	FILE *fp;
	
	LogDebug("[%s:%s %u]  UdpFunc RUN \n", __FILE__, __func__, __LINE__);
	memset(recvbuf, 0x00, PC_SENDBUF_SIZE + 3);

	//组包后临时存放的地址
	pSendBuf= (UCHAR *)malloc(PACKET_MAX_SIZE + 1);

	pSaveRam = (UCHAR *)malloc(BRAM_SIZE_IMAGE); //在offset固件校正时，bram C保存区缓存
	memset(pSaveRam, 0x00, BRAM_SIZE_IMAGE);

	UINT *pTmpPara = NULL;
	UINT *pTmprecvbuf = NULL;

	while(1){
		length = UDP_RECV((UCHAR *)recvbuf, PACKET_MAX_SIZE);
		if (length == PC_SENDBUF_SIZE) {
			p_bram_parameter = p_bram_tail + 0x1000;		//FPGA_ADDRESS+1000

			//因固件不支持memcpy（会出现Bus error），自己封装函数
			//memcpy(p_bram_parameter, recvbuf, PC_SENDBUF_SIZE + 3); //透传1049+3个0x00,共1052;
			//MyMemcpy(p_bram_parameter, recvbuf, PC_SENDBUF_SIZE + 3); //不能配置参数，需要每次拷贝4个字节

			pTmpPara = (UINT *)p_bram_parameter;
			pTmprecvbuf = (UINT *)recvbuf;
			for(int i = 0; i < 1052 / 4; i++) {
				pTmpPara[i] = pTmprecvbuf[i];
			}

			m_pRecvCmd = (UCHAR *)(recvbuf + OFFSET_PACKAGE_CMD); //CMD

			if ((*(m_pRecvCmd) == RECV_TYPE_Firmware_Update) || (*(m_pRecvCmd) == RECV_TYPE_DOWNLOAD_GAIN)) {
				//屏蔽不必要的打印
			}else {
				LogDebug("[%s:%s %u]  Recv XDStatic Cmd=0x%.2x \n", __FILE__, __func__, __LINE__,  *(m_pRecvCmd));
			}
			
				

		//擦除frame（每次update，都会先发一次0x4f；我这里只做回应）
			if (*(m_pRecvCmd) == RECV_TYPE_ERASE_FLASH) {
				UploadResponseCmd(CMDD_FRAME_RETRANS);
				//上一次update失败后，下一次需要恢复参数数据
				mPackCount = 0;
				if (NULL != pUpdatedata) {
					free(pUpdatedata);
					pUpdatedata = NULL;
				}
				pUpdatedata = (UCHAR *)malloc(UPDATE_DATA_BUF_SIZE);
				memset(pUpdatedata, 0xff, UPDATE_DATA_BUF_SIZE);
				pTail = pUpdatedata;
			//flag	
				mUpPackNumFlag = 0;
				mUpSuccess = 0;	
			}	
		//Update FPGA File
			if (*(m_pRecvCmd) == RECV_TYPE_Firmware_Update) {
				if ( 0 == Update_SaveFile(recvbuf)) {
					UploadResponseCmd(CMDD_PACKET_RETRANS);
					if (1 == mUpSuccess) {
						Update_FpgaFile();
						if (NULL != pUpdatedata) {
							free(pUpdatedata);
							pUpdatedata = NULL;
						}
					}
				}		
			}
		//暂时放着，后续考虑
		//Update Linux File
			if (*(m_pRecvCmd) == 0x66) {
				if ( 0 == Update_SaveFile(recvbuf)) {
					UploadResponseCmd(CMDD_PACKET_RETRANS);
					if (1 == mUpSuccess) {
						Update_LinuxFile();
					}
				}		
			}

		//丢包重传
			if (*(m_pRecvCmd) == RECV_TYPE_PACKET_RETRANS) {
				PacketRetransmission(recvbuf);
			}

		//整帧重传
			else if (*(m_pRecvCmd) == RECV_TYPE_FRAME_RETRANS) {
				FrameRetransmission();
			}			
		
		//下载Gain校正模板
			if (*(m_pRecvCmd) == RECV_TYPE_DOWNLOAD_GAIN) {
				if((fp=fopen(FILE_NAME_GAIN,"wt+")) == NULL)
				{
					LogError("[%s:%s %u]  open <%s> file error \n", __FILE__, __func__, __LINE__, FILE_NAME_GAIN);
				}
				fwrite(recvbuf, sizeof(UCHAR), PC_SENDBUF_SIZE, fp);			
				fclose(fp);
				LogDebug("[%s:%s %u]  Download <%s> Successful! \n", __FILE__, __func__, __LINE__, FILE_NAME_GAIN);
			}

		//下载Defect校正模板
			if (*(m_pRecvCmd) == RECV_TYPE_DOWNLOAD_DEFECT) {
				if((fp=fopen(FILE_NAME_DEFECT,"wt+")) == NULL)
				{
					LogError("[%s:%s %u]  open <%s> file error \n", __FILE__, __func__, __LINE__, FILE_NAME_DEFECT);
				}
				fwrite(recvbuf, sizeof(UCHAR), PC_SENDBUF_SIZE, fp);			
				fclose(fp);
				LogDebug("[%s:%s %u]  Download <%s> Successful! \n", __FILE__, __func__, __LINE__, FILE_NAME_DEFECT);
			}


			memset(recvbuf, 0x00, PC_SENDBUF_SIZE + 3);		//用完清空
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
