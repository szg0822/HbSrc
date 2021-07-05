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
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h> //set baud rate
#include <sys/select.h>
#include <sys/types.h>
#include<thread>

#include "UdpFunc.h"
#include "LinuxLog.h"
#include "MyUDP.h"
#include "Serial.h"

#define BuffSize 1024
#define TIMEOUT 2       //超时等待2秒

typedef unsigned int uint_32;
typedef unsigned short uint_16 ;  

static UCHAR *pSendBuf;				//组包后临时存放的地址
//update
static UCHAR *pUpdatedata = NULL; 			//更新固件的保存数据地址
static UCHAR *pTail;				//指向更新固件的保存数据地址
static UINT mUpPackNumFlag = 0;		//update数据包的个数只需要获取一次就行，这里上个锁
static UINT mUpSuccess = 0;			//update数据包成功的标志

//校正模板
static USHORT *pSaveRam = NULL;				//在offset固件校正时，bram C保存区缓存
static UCHAR *pTemplateBuf = NULL;	//Gain模板指针
static UCHAR *pGainTail;			//指向下载模板的保存数据地址
static UINT mDownloadFlag = 0;		//下载模板标志
static UCHAR *pSaveImage = NULL;	//保存的图像数据

//必须初始化，否则free会段错误
static USHORT *pTmpGainBuf = NULL;
static uint_16 * ipGainPtr = NULL;

//Flag
static UINT MGainBufFlag = 0;		//标志：申请一次地址，保存Gain校正后的数据
static UINT FileGainFlag = 0;		//Gain模板下载标志，获取一次就ok； 0：之前下载；1:刚下载;
static UINT MOffsetFlag = 0;

static UINT mOffset = 0;			//offset校正模式
static UINT mImageLen = 0;			//图像像素

static UINT mPackCount = 0;			//UpdateFirmware 包的数量

//图像存储EMMC
static UINT mFrameNum = 0;			//帧号，图片数量
static UINT mReadNum = 0;			//读取EMMC图像数量
static UINT mFrameImageFlag = 0;	//是否读取完成；1：完成
static UINT m_FrameCount = 0;		//存储在文件里的图像数量
static UINT m_FraCount = 0;			//图像开始编号
static UINT m_RNum = 1;
static UINT m_FrameNum = 1;				//接收下载图像序号，默认为1 ID1~ID4组成

// typedef struct defect {
	// unsigned de:8;
	// unsigned y:12;
	// unsigned x:12;	
// }defect_t;
// static defect_t * Pdefect = NULL;

static UINT ImageNum = 1;		//保存上一张图像编号

//低功耗
static struct timeval m_start;
static UCHAR PowerBuf[PC_SENDBUF_SIZE + 3] = { 0 };	//保存配置
static int m_time = 600;		//600秒=10m
static struct timeval m_end;
static UINT m_PowerDownFlag = 0;
static UINT m_PowerOnFlag = 0;
static UINT m_ConnectFlag = 0;		//当前如果处于休眠状态，上位机重新连接时需要直接唤醒

//串口变量
extern CSerial cSerial;

//大小端转换32位
#define BSWAP_32(x) \   
    (uint_32)((((uint_32)(x) & 0xff000000) >> 24) | \  
              (((uint_32)(x) & 0x00ff0000) >> 8) | \  
              (((uint_32)(x) & 0x0000ff00) << 8) | \  
              (((uint_32)(x) & 0x000000ff) << 24) \  
             )  

#define BSWAP_16(x) \   
    (uint_16)((((uint_16)(x) & 0x00ff) << 8) | \  
              (((uint_16)(x) & 0xff00) >> 8) \  
             )  
static uint_32 *Tdef = NULL;

extern parameter_t m_ParaInfo;

UdpFunc::UdpFunc()
{}
UdpFunc::~UdpFunc()
{}

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
 * * 函 数 名: MyDirExist
 * * 功能描述: 判断目录是否存在
 * * 参数说明: path:目录路径
 * * 返 回 值：0:目录存在
 * * 备    注:             
 * *********************************************************/
int MyDirExist(char * path)
{
	struct stat statBuf = {0};
	int pbExist = -1;
	if(NULL == path)
	{
		printf("[%s(),%d] path is NULL!!!", __FUNCTION__, __LINE__);
		return -1;
	}
	memset(&statBuf, 0, sizeof(statBuf));
	if(0 != stat(path, &statBuf))
	{
		return -2;
	}

	if(S_ISDIR(statBuf.st_mode))
		pbExist = 0;
	return pbExist;
}

/*********************************************************
 * * 函 数 名: MyMkdir
 * * 功能描述: 创建多级目录
 * * 参数说明: muldir:目录
 * * 返 回 值：
 * * 备    注:  
 * *********************************************************/
static void MyMkdir(char *muldir)
{
    int i,len;
    char str[512];
    strncpy(str, muldir, 512);
    len=strlen(str);
    for( i=0; i<len; i++ )
    {
        if( str[i]=='/' )
        {
            str[i] = '\0';
            if( access(str,0)!=0 )
            {
                mkdir( str, 0755 );
            }
            str[i]='/';
        }
    }
    if( len>0 && access(str,0)!=0 )
    {
        mkdir( str, 0777 );
    }
    return;
}


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

	memset(pSendBuf,0,PACKET_PL_SIZE + 1);
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
	CreateCmd(cmdID, NULL);
	MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], p_bram_parameter, TMP_BUFFER_SIZE);
	if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
		//LogError("[%s:%s %u]  UDP_SEND failed! \n", __FILE__, __func__, __LINE__);
		return -1;
	}
	return 0;
}

//存储图片数量
void StoreImageCount(UINT ImageCount)
{
    FILE * fp = NULL;

    fp = fopen("ImageCount.txt","w");
    fprintf(fp, "%-10d", ImageCount);
    fclose(fp);
}
//获取图片数量
void GetImageCount(UINT * ImageCount)
{
    FILE * fp = NULL;   //文件指针，指向成功打开的文件

    //打开文件，以只读的方式r
    fp = fopen("ImageCount.txt","r");
    if (fp == NULL)
    {
        // fp = fopen("ImageCount.txt","w");
        // fprintf(fp, "%-10d", *ImageCount);
		LogError("[%s:%s %u]  fopen ImageCount.txt failed！ \n", __FILE__, __func__, __LINE__);
    }
    else
        fscanf(fp, "%10d", ImageCount);

    //关闭文件
    fclose(fp);
}


/*********************************************************
* 函 数 名: SaveImageData
* 功能描述: 保存图像数据
* 参数说明: pImage：图像数据	
*		   ImageSize:数据大小
*		   FrameNum：帧号
* 返 回 值：0：成功；!0：失败
* 备    注:
*********************************************************/
int UdpFunc::SaveImageData(UCHAR *pImage, int ImageSize)
{
	FILE *fp;
	char mBuf[50];
	char lCmdBuf[256];

	char Nbuf[50] = {'\0'};
	UINT *pSaveBuf = NULL;
	if (NULL == pImage) {
		LogError("[%s:%s %u]=== pImage NULL\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	pSaveBuf = (UINT *)pImage;
	
	//每次开机后需要存图时，先删除旧图
	if (0 == m_FraCount) {
		sprintf(lCmdBuf, "rm -f %s/*", RAW_IMAGE_PATH);
		system(lCmdBuf);
		StoreImageCount(0);
	}

	sprintf(Nbuf, RAW_IMAGE_PATH"Image%d.raw", m_FraCount);
	//把图像数量保存到文件
	StoreImageCount(m_FraCount + 1);

	if((fp=fopen(Nbuf,"wt+"))==NULL)
	{
		LogError("[%s:%s %u]  open <%s> file error \n", __FILE__, __func__, __LINE__, Nbuf);
		return -2;
	}
	fwrite(pSaveBuf, sizeof(UINT), ImageSize / 4, fp);			
	fclose(fp);
	LogDebug("[%s:%s %u]  SaveImageData[%d] Successfun!  \n", __FILE__, __func__, __LINE__, m_FraCount + 1);
	m_FraCount++;

	return 0;
}

/*********************************************************
* 函 数 名: ReadImageData
* 功能描述: 保存图像数据
* 参数说明: Output：[pImage:读取的图像数据]	
* 返 回 值：数据长度，失败返回-1
* 备    注:
*********************************************************/
int UdpFunc::ReadImageData(UCHAR *pImage)
{
	FILE *fp;
	char Nbuf[50];
	int lSize = 0;

	//轮询：提示图像读取完毕
	if (1 == mFrameImageFlag) {
		mFrameImageFlag = 0;
	}

	if (0 == m_FrameNum) {
		m_FrameNum = 1;
	}
	//从一个指定帧号(n-1)开始读取,默认必须为1
	if (ImageNum != m_FrameNum) {
		if (m_FrameNum > 0) {
			mReadNum = m_FrameNum - 1;
			ImageNum = m_FrameNum;
			m_RNum = mReadNum + 1; 
		}
	}

	sprintf(Nbuf, RAW_IMAGE_PATH"Image%d.raw", mReadNum);
	mReadNum++;
	if((fp=fopen(Nbuf,"r"))==NULL)
	{
		LogError("[%s:%s %u]  open <%s> file error \n", __FILE__, __func__, __LINE__, Nbuf);
		mReadNum = m_FrameNum - 1;
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	lSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);	
	fread(pImage, sizeof(UCHAR), lSize, fp);			
	fclose(fp);

	//从文件里获取图像数量
	GetImageCount(&m_FrameCount);
	if (mReadNum == m_FrameCount) {
		mFrameImageFlag = 1;
	}
	
	return lSize;
}

/*********************************************************
* 函 数 名: CorrectionOffset
* 功能描述: Offset固件校正
* 参数说明: ImageSize:图片大小
*		   SV:饱和值					
* 返 回 值：校正数据
* 备    注:
*********************************************************/
UCHAR *UdpFunc::CorrectionOffset(UINT ImageSize, UINT SV)
{
	//俩帧合为一帧
	int j = 0;
	USHORT C2;
	UCHAR *pImBuf = NULL;
	if (0 == MOffsetFlag) {
		MOffsetFlag = 1;
		pSaveRam = (USHORT *)malloc(ImageSize); //在offset固件校正时，bram C保存区缓存
	}
	memset(pSaveRam, 0x00, ImageSize);
	USHORT *TmpA = (USHORT *)p_bramA_image;
	USHORT *TmpB = (USHORT *)p_bramB_image;
	for (int i = 0; i < ImageSize / 2; i++) {
		//算法需要实现,俩个字节运算
		USHORT A2 = TmpA[i];
		USHORT B2 = TmpB[i];
		if (A2 < SV) {
			if ((A2 < B2) && (B2 - A2 <= 100)) 
				C2 = 100 - (B2 - A2);
			else if (A2 >= B2)
				C2 = A2 - B2 + 100;
			else if ((A2 < B2) && (B2 - A2 > 100))
				C2 = 100;		
		}
		else {
			C2 = SV;
		}
		pSaveRam[i] = C2;
	}
	pImBuf = (UCHAR*)pSaveRam;
	return pImBuf;
}

/*********************************************************
* 函 数 名: CorrectionGain
* 功能描述: Gain固件校正
* 参数说明: ImageSize:图片大小
*		   sv:饱和值					
* 返 回 值：校正数据
* 备    注:
*********************************************************/
UCHAR *UdpFunc::CorrectionGain(UCHAR * pImage, UINT ImageSize, UINT SV)
{
	FILE * fpFile;
	int lSize, lCount;
	UCHAR *pImBuf = NULL;
	USHORT *pDefeBuf = NULL;
	uint_16 TGain;
	USHORT TMP_Gain = 0;
	USHORT OffsetT = 0;

	fpFile = fopen(FILE_NAME_GAIN, "r");
	if (NULL == fpFile) {
		LogError("[%s:%s %u]  No template downloaded (file<%s> failed!) \n", __FILE__, __func__, __LINE__, FILE_NAME_GAIN);
		return NULL;
	}
	else {
	//申请Gain校正后的地址
		if (0 == MGainBufFlag) {
			MGainBufFlag = 1;
			if (NULL != pTmpGainBuf) {
				free(pTmpGainBuf);
				pTmpGainBuf = NULL;
			}
			pTmpGainBuf = (USHORT *)malloc(ImageSize);					
		}
		
		memset(pTmpGainBuf, 0, ImageSize);
		
	//拷贝Gain校正模板数据
		if (1 == FileGainFlag) {
			FileGainFlag = 0;
			if (NULL!= ipGainPtr) {
				free(ipGainPtr);
				ipGainPtr = NULL;
			}
			// fseek(fpFile, 0L, SEEK_END);
			// lSize = ftell(fpFile);
			// fseek(fpFile, 0L, SEEK_SET);	
			lSize = ImageSize;
			lCount = lSize / sizeof(uint_16);
			ipGainPtr = (uint_16*)malloc(lSize);
			memset(ipGainPtr, 0, lSize);
			fread(ipGainPtr,sizeof(uint_16),lCount,fpFile);				
		}
		fclose(fpFile);
		

		pDefeBuf = (USHORT *)pImage;
		for (int i = 0; i < ImageSize / 2; i++) {
			TGain = BSWAP_16(ipGainPtr[i]);
			//GainT = ((ipGainPtr[j] & 0xff) << 8) | (ipGainPtr[j+1] & 0xff);
			OffsetT = pDefeBuf[i];
			TMP_Gain = OffsetT * 1024 / TGain;
			if (TMP_Gain >= SV) {
				TMP_Gain = SV;
			}
			pTmpGainBuf[i] = TMP_Gain;
		}
		pImBuf = (UCHAR *)pTmpGainBuf;
	}
	return pImBuf;
}

/*********************************************************
* 函 数 名: CorrectionDefect
* 功能描述: Defect固件校正
* 参数说明: pImage:图片数据
*		   ImageLenBuf:长宽数据
*		   PanelSize:平板像素大小选择				
* 返 回 值：校正数据
* 备    注:
*********************************************************/
UCHAR *UdpFunc::CorrectionDefect(UCHAR * pImage, UINT *ImageLenBuf, UINT PanelSize)
{
	FILE * fpFile;	
	USHORT *pTempGainBuf = NULL;
	int lSize, lCount;
	UCHAR *pImaBuf = NULL;

	fpFile = fopen(FILE_NAME_DEFECT, "r");
	if (NULL == fpFile) {
		LogError("[%s:%s %u]  No template downloaded (file<%s> failed!) \n", __FILE__, __func__, __LINE__, FILE_NAME_DEFECT);
		return NULL;
	}
	else {
		//ImageLenBuf[2 * PanelSize - 2] * ImageLenBuf[2 * PanelSize - 1];
		UINT TmpLenX = ImageLenBuf[2 * PanelSize - 2];
		UINT TmpLenY = ImageLenBuf[2 * PanelSize - 1];
									
		UINT iTmp;	
		uint_32 bswap_ret;
		int iR = 0;
		pTempGainBuf = (USHORT *)pImage;
		//拷贝Gain校正模板数据	
				
		fseek(fpFile, 0L, SEEK_END);
		lSize = ftell(fpFile);
		fseek(fpFile, 0L, SEEK_SET);
		lCount = lSize / sizeof(uint_32);
		Tdef = (uint_32*)malloc(lSize);
		memset(Tdef, 0, lSize);
		fread(Tdef, sizeof(uint_32), lCount, fpFile);
		fclose(fpFile);						 
		//B5 B1 0B FF
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		for(int a = 0; a < lCount; a++) {
			UINT count = 0; //正常像素点个数
			UINT sum = 0;
			bswap_ret = BSWAP_32(Tdef[a]);
			//12+12+8位
			int x = ((bswap_ret & 0xfff00000) >> 20);
			int y = ((bswap_ret & 0x000fff00) >> 8);
			int de = (bswap_ret & 0x00000ff);
			// int x = ((Pdefect[0] & 0xff) << 4) | ((Pdefect[1] & 0xf0) >> 4);
			// int y = ((Pdefect[1] & 0x0f) << 8) | ((Pdefect[2]) & 0xff);
			/*排列顺序
				0==3==5
				1==X==6
				2==4==7	
			*/
			//获取正常点坐标
			int pos = 7;	//移位计数
			x1 = x + 1;
			x2 = x - 1;
			y1 = y + 1; 
			y2 = y - 1;
			//去除超界的点
			if (x1 > TmpLenX - 1) x1 =  TmpLenX - 1;
			if (x2 < 0) x2 = 0;
			if (y1 > TmpLenY - 1) y1 =  TmpLenY - 1;
			if (y2 < 0) y2 = 0;
			for (int i = x1; i >= x2; i--) {
				for (int j = y1; j >= y2; j--) {
					//中心不运算
					if (i == x && j == y)
						continue;
					//正常像素点计数
					if (0 != ((1 << pos) & de)) {
						count++;
						iR = j * TmpLenX + i;
						sum += pTempGainBuf[iR];
					}
					pos--;
				}
			}
			if (count > 0) {
				iR = y * TmpLenX + x;
				pTempGainBuf[iR] = sum / count;
			}
			
				//LogError("test1:sum=%d, count=%d, defect[%d][%d]=%d, de=0x%x\n", sum,count,x,y,pTempGainBuf[iR], de);
		}
		pImaBuf = (UCHAR *)pTempGainBuf;
		if (NULL != Tdef) {
			free(Tdef);
			Tdef = NULL;
		}
	}
	return pImaBuf;
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
	UINT packageNo = 0;		//包号
	UCHAR *pImage = NULL;
	UINT ImageLen = 0;			//像素值，2个字节一个像素点
	UINT offset = 0;
	UINT gain = 0;
	UINT defect = 0;
	UINT PanelSize = 1;
	UINT SV = 100;
	int m_ret = 0;
	UINT SaveEmmc = 0;

	UCHAR *pImageOffset = NULL;
	UCHAR *pImageGain = NULL;
	UCHAR *pImageDefect = NULL;

	offset = ParaInfo.offset;
	gain = ParaInfo.gain;
	defect = ParaInfo.defect;
	PanelSize = ParaInfo.PanelSize;
	SV = ParaInfo.SaturationV;
	SaveEmmc = ParaInfo.SaveEMMC;

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
	int ImageSize = ImageLen * 2;

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
	else if (2 == offset) {
		pImageOffset = CorrectionOffset(ImageSize, SV);
		pImage = pImageOffset;

	//Gain模板固件校正
		if (2 == gain) {
			pImageGain = CorrectionGain(pImageOffset, ImageSize, SV);
			if (NULL != pImageGain)
				pImage = pImageGain;
			else 
				pImage = pImageOffset;

			//Defect模板固件校正
			if (2 == defect) {
				pImageDefect = CorrectionDefect(pImageGain, ImageLenBuf, PanelSize);
				if (NULL != pImageDefect)	
					pImage = pImageDefect;
				else
					pImage = pImageGain;
			}	
		}				
	}
	//最终生成的数据
	pSaveImage = pImage;	

	if (1 == SaveEmmc) {
		UINT m_framCount = 0;
		m_ret = SaveImageData(pSaveImage, ImageSize);
		if (0 != m_ret) {
			LogError("[%s:%s %u]=== SaveImageData error! ret=%d\n", __FILE__, __func__, __LINE__, m_ret);
		}
		GetImageCount(&m_framCount);
		if (m_framCount >= 300) {
			UINT * pTmp = NULL;
			UINT *pPowerDown = NULL;
			CreateCmd(CMDD_WRITE_PARA, NULL);
			MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], p_bram_parameter, TMP_BUFFER_SIZE);
			MyMemcpy(PowerBuf, pSendBuf, PACKET_PL_SIZE);
			//发送给PL
			PowerBuf[19 + 522] = 0x01;		//修改配置开关；1：可以修改，0：不能修改
			PowerBuf[19 + 523] = 0x00;
			PowerBuf[19 + 524] = 0x00;
			pPowerDown = (UINT *)PowerBuf;		//保存之前的配置
			pTmp = (UINT *)p_bram_parameter;
			for (int i = 0; i < 1052 / 4; i++) {
				pTmp[i] = pPowerDown[i];
			}	
			UploadStateCmd(CMDU_REPORT, FPD_STATUS_TOPLIMIT);
			m_FraCount = 0;  //EMMC达上限后，如果再次使能，删除原数据，重新存储。
			LogDebug("[%s:%s %u]  EMMC：SaveImageEnd, Toplimit=300 \n", __FILE__, __func__, __LINE__);
		}
	}
	
	while (1)
	{	
		//一帧图像（3072*3072）像素点，然后除以512得出18432k，1K组一个包发送
		if (packageNo == (ImageLen / 512)) {
			break;
		}
		
		CreateCmd(cmdID, NULL);
		//帧号
		pSendBuf[HB_ID1] = (mFrameNum ) & 0xff;
		pSendBuf[HB_ID2] = (mFrameNum >> 8) & 0xff;
		pSendBuf[HB_ID3] = (mFrameNum >> 16) & 0xff;
		//包号
		pSendBuf[HB_ID7] = (packageNo ) & 0xff;
		pSendBuf[HB_ID8] = (packageNo >> 8) & 0xff;
		pSendBuf[HB_ID9] = (packageNo >> 16) & 0xff;

		MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], pImage, TMP_BUFFER_SIZE);
		
		if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
			LogError("[%s:%s %u]=== UDP_SEND failed!", __FILE__, __func__, __LINE__);
			return -1;
		}
		packageNo++;
		pImage += TMP_BUFFER_SIZE;
	}
	LogDebug("[%s:%s %u]  UploadImageData success! \n", __FILE__, __func__, __LINE__);
	mFrameNum++;		//帧号
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
	UINT FrameCount = 0;
	FrameCount = mFrameNum - 1;
	CreateCmd(cmdID, &chStatusCode);
	//帧号
	pSendBuf[HB_ID1] = (FrameCount ) & 0xff;
	pSendBuf[HB_ID2] = (FrameCount >> 8) & 0xff;
	pSendBuf[HB_ID3] = (FrameCount >> 16) & 0xff;
	if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
		//LogError("[%s:%s %u]=== UDP_SEND failed!", __FILE__, __func__, __LINE__);
		return -1;
	}
	return 0;
}

void UdpFunc::PacketRetransmission(UCHAR *recvbuf)
{
	UCHAR *TmpBram;
	USHORT PackNum = 0;		//丢包的数量
	UINT PackId;		//包号
	USHORT len = 0;
	LogDebug("[%s:%s %u]  Start Packet Retransmission \n", __FILE__, __func__, __LINE__);
	UINT count = 0;

	if (recvbuf == NULL) {
		LogError("[%s:%s %u]  recvbuf NULL \n", __FILE__, __func__, __LINE__);
		return;
	}
	//丢包数据
	TmpBram = pSaveImage;

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
		//帧号
		pSendBuf[HB_ID1] = (mFrameNum ) & 0xff;
		pSendBuf[HB_ID2] = (mFrameNum >> 8) & 0xff;
		pSendBuf[HB_ID3] = (mFrameNum >> 16) & 0xff;
		//包号		
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
	UINT packageNo = 0;
	UCHAR *pImage;

	LogDebug("[%s:%s %u]  Start Frame Retransmission \n", __FILE__, __func__, __LINE__);
	//丢帧数据
	TmpBram = pSaveImage;

	while (1)
	{	
		//一帧图像（3072*3072）像素点，然后除以512得出18432k，1K组一个包发送
		if (packageNo == (mImageLen / 512)) {
			break;
		}
		CreateCmd(CMDU_UPLOAD_IMAGE_RETRANS, NULL);
		//帧号
		pSendBuf[HB_ID1] = (mFrameNum ) & 0xff;
		pSendBuf[HB_ID2] = (mFrameNum >> 8) & 0xff;
		pSendBuf[HB_ID3] = (mFrameNum >> 16) & 0xff;
		//包号
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
		LogDebug("[%s:%s %u]  Recv XDStatic [Cmd=0x%.2x] \n", __FILE__, __func__, __LINE__,  *(pRecvCmd));
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

//暂时放着，后续考虑
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
	static UINT PackNum = 1;		//包数量
	FILE *fp;
	UCHAR *pRecvCmd;
	UINT LastPackLen = 0;
	static UINT Num = 0;			//包号
	
	if (NULL == recvbuf) {
		LogError("[%s:%s %u]  recvbuf NULL \n", __FILE__, __func__, __LINE__);
		return -1;
	}

	//用包号来判断是否第一次下载模板（重新下载模板时包号为0）
	Num = (recvbuf[HB_ID8] & 0xff) | ((recvbuf[HB_ID9] & 0xff) << 8);
	if (0 == Num) {
		mDownloadFlag = 0;
	}

	//下载通用模板只进来一次
	if (mDownloadFlag == 0) {
		if (NULL != pTemplateBuf) {
			free(pTemplateBuf);
			pTemplateBuf = NULL;
		}
			
		pTemplateBuf = (UCHAR *)malloc(Template_Buf_SIZE);
		if (NULL == pTemplateBuf) {
			LogError("[%s:%s %u]  malloc pTemplateBuf NULL \n", __FILE__, __func__, __LINE__);
			return -2;
		}

		memset(pTemplateBuf, 0, Template_Buf_SIZE);
		pGainTail = pTemplateBuf;
		mPackCount = 0;
		pRecvCmd = (UCHAR *)(recvbuf + OFFSET_PACKAGE_CMD);
		LogDebug("[%s:%s %u]  Recv XDStatic [Cmd=0x%.2x] \n", __FILE__, __func__, __LINE__,  *(pRecvCmd));
		PackNum = (recvbuf[HB_ID6] & 0xff) | ((recvbuf[HB_ID7] & 0xff) << 8);
		LogDebug("[%s:%s %u]  Total_PackNum = %d\n", __FILE__, __func__, __LINE__, PackNum);
		mDownloadFlag = 1;
	}

	if (mPackCount == PackNum - 1) {
		LastPackLen = (recvbuf[HB_ID4] & 0xff) | ((recvbuf[HB_ID5] & 0xff) << 8);
		MyMemcpy(pGainTail, recvbuf + OFFSET_PACKAGE_IMAGESLICE, LastPackLen);
		pGainTail += LastPackLen;
	}	
	else {
		MyMemcpy(pGainTail, recvbuf + OFFSET_PACKAGE_IMAGESLICE, TMP_BUFFER_SIZE);
		pGainTail += TMP_BUFFER_SIZE;
	}

	mPackCount++;
	
	//printf("packnum=%d, count=%d\n", PackNum, mPackCount);
	if (mPackCount == PackNum) {
		pGainTail = pTemplateBuf;
		if (1 == TemplateValue) {
			if((fp=fopen(FILE_NAME_GAIN,"wt+"))==NULL)
			{
				LogError("[%s:%s %u]  open <%s> file error \n", __FILE__, __func__, __LINE__, FILE_NAME_GAIN);
				free(pTemplateBuf);
				return -1;
			}
			fwrite(pTemplateBuf, sizeof(UCHAR), (PackNum - 1) * TMP_BUFFER_SIZE + LastPackLen, fp);			
			fclose(fp);

			LogDebug("[%s:%s %u]  save file[%s] successful!\n", __FILE__, __func__, __LINE__, FILE_NAME_GAIN);
			FileGainFlag = 1;
		}
		else if (2 == TemplateValue) {
			if((fp=fopen(FILE_NAME_DEFECT,"wt+"))==NULL)
			{
				LogError("[%s:%s %u]  open <%s> file error \n", __FILE__, __func__, __LINE__, FILE_NAME_DEFECT);
				free(pTemplateBuf);
				return -1;
			}
			fwrite(pTemplateBuf, sizeof(UCHAR), (PackNum - 1) * TMP_BUFFER_SIZE + LastPackLen, fp);			
			fclose(fp);
			LogDebug("[%s:%s %u]  save file[%s] successful!\n", __FILE__, __func__, __LINE__, FILE_NAME_DEFECT);
		}

		mPackCount = 0;
		mDownloadFlag = 0;
		free(pTemplateBuf);
		pTemplateBuf = NULL;
	}
	return 0;
}

/*********************************************************
* 函 数 名: UdpSendImage
* 功能描述: 把保存图像数据发送给上位机
* 参数说明: 	
* 返 回 值：
* 备    注: 利用文件存储图像数量
*********************************************************/
int UdpFunc::UdpSendImage(UCHAR * pRecvBuf)
{
	UCHAR * pImage = NULL;
	UINT packageNo = 0;
	int lImageSize = 0;
	int ImageLen = 0;
	LogDebug("[%s:%s %u]  UdpSendImage Start! \n", __FILE__, __func__, __LINE__);

	m_FrameNum = (pRecvBuf[HB_ID1] & 0xff) | ((pRecvBuf[HB_ID2] & 0xff) << 8) \
				| ((pRecvBuf[HB_ID3] & 0xff) << 16) | ((pRecvBuf[HB_ID4] & 0xff) << 24);		//低高

	lImageSize = ReadImageData(pSaveImage);
	if (-1 == lImageSize) {
		UploadStateCmd(CMDU_REPORT, FPD_STATUS_IMAGE_NO);
		return -1;
	}

	pImage = pSaveImage;
	ImageLen = lImageSize / 2;

	//帧号+1
	m_RNum = mReadNum;
	if (1 == mFrameImageFlag) {
		m_RNum = m_FrameCount;
	}	

	while(1)
	{	
		//一帧图像（3072*3072）像素点，然后除以512得出18432k，1K组一个包发送
		if (packageNo == (ImageLen / 512)) {
			break;
		}
		
		CreateCmd(CMDU_UPLOAD_IMAGE_SINGLE_SHOT, NULL);	
		//序号
		pSendBuf[HB_ID1] = (m_RNum ) & 0xff;
		pSendBuf[HB_ID2] = (m_RNum >> 8) & 0xff;
		pSendBuf[HB_ID3] = (m_RNum >> 16) & 0xff;
		//图像总数量
		pSendBuf[HB_ID4] = (m_FrameCount ) & 0xff;
		pSendBuf[HB_ID5] = (m_FrameCount >> 8) & 0xff;
		pSendBuf[HB_ID6] = (m_FrameCount >> 16) & 0xff;
		//包号
		pSendBuf[HB_ID7] = (packageNo ) & 0xff;
		pSendBuf[HB_ID8] = (packageNo >> 8) & 0xff;
		pSendBuf[HB_ID9] = (packageNo >> 16) & 0xff;

		MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], pImage, TMP_BUFFER_SIZE);
		
		if (0 != UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE)) {
			LogError("[%s:%s %u]=== UDP_SEND failed!", __FILE__, __func__, __LINE__);
			return -2;
		}
		packageNo++;
		pImage += TMP_BUFFER_SIZE;
	}
	LogDebug("[%s:%s %u]  UdpSendImage[%d] success! total=%d\n", __FILE__, __func__, __LINE__, mReadNum, m_FrameCount);

	//判断保存的图像是否读取完毕，发个消息给上位机
	if (1 == mFrameImageFlag) {
		if (m_FrameNum > 0)
			mReadNum = m_FrameNum - 1;
		LogDebug("[%s:%s %u]  ===Read Complete!!!! \n", __FILE__, __func__, __LINE__);
		UploadStateCmd(CMDU_REPORT, FPD_STATUS_IMAGE_LAST);
	}
	return 0;
}

void UdpFunc::UdpFuncInit()
{
	UINT * pTmp = NULL;
	UINT *pPowerDown = NULL;
	int m_mkdir = -1;
#if 1
	//UDP连接有线
	char SrcWriedIp[MAX_IP_LEN] = "192.168.0.40";
	if (ERR_SUCCESS != UDP_CREATE(SrcWriedIp, 1)) { 
		LogError("[%s:%s %u]  UDP Connect Wired Failed! \n", __FILE__, __func__, __LINE__);
		UDP_CLOSE();
	}
	else{
		LogDebug("[%s:%s %u]  UDP Connect Wired Success! \n", __FILE__, __func__, __LINE__);
	}
#endif	
	//组包后临时存放的地址
	pSendBuf= (UCHAR *)malloc(PACKET_PL_SIZE + 1);

	pSaveImage = (UCHAR *)malloc(BRAM_SIZE_IMAGE);	//最终保存的图片数据
	memset(pSaveImage, 0x00, BRAM_SIZE_IMAGE);

	//开机第一次fread时间长，需提前获取
	FILE *fpFile; 
	int lCount;
	fpFile = fopen(FILE_NAME_GAIN, "r");
	if (NULL == fpFile) {
		LogError("[%s:%s %u]  No template downloaded (file<%s> failed!) \n", __FILE__, __func__, __LINE__, FILE_NAME_GAIN);
	}else {
		fseek(fpFile, 0L, SEEK_END);
		int lSize = ftell(fpFile);
		fseek(fpFile, 0L, SEEK_SET);
		lCount = lSize / sizeof(uint_16);
		ipGainPtr = (uint_16*)malloc(lSize);
		memset(ipGainPtr, 0, lSize);
		fread(ipGainPtr,sizeof(uint_16),lCount,fpFile);
		fclose(fpFile);
		FileGainFlag = 0;
	}

	//INIT低功耗配置为0
	CreateCmd(CMDD_WRITE_PARA, NULL);
	MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], p_bram_parameter, TMP_BUFFER_SIZE);
	MyMemcpy(PowerBuf, pSendBuf, PACKET_PL_SIZE);
	//LogError("[%s:%s %u]  recv:buf7=%x,buf8=%x \n", __FILE__, __func__, __LINE__, pPowerDown[19+447], pPowerDown[19+448]);
	//发送给PL
	PowerBuf[19 + 438] = 0x01;		//修改配置开关；1：可以修改，0：不能修改
	PowerBuf[19 + 447] = 0x00;
	PowerBuf[19 + 448] = 0x00;
	pPowerDown = (UINT *)PowerBuf;		//保存之前的配置
	pTmp = (UINT *)p_bram_parameter;
	for (int i = 0; i < 1052 / 4; i++) {
		pTmp[i] = pPowerDown[i];
	}	

	//创建EMMC图像目录
	m_mkdir = MyDirExist(RAW_IMAGE_PATH);
	if (0 != m_mkdir) {
		MyMkdir(RAW_IMAGE_PATH);
	}
}


//获取当前时间
void UdpFunc::GetStartTime()
{
	gettimeofday(&m_start, NULL);
}

/*********************************************************
* 函 数 名: InputLowerPower
* 功能描述: 进入低功耗模式或休眠
* 参数说明: 	
* 返 回 值：
* 备    注: 
*********************************************************/
void UdpFunc::InputLowerPower()
{
	UINT * pTmp = NULL;
	UINT *pPowerDown = NULL;
	ULONG diff = 0;

	gettimeofday(&m_end, NULL);
	diff = 1000000 * (m_end.tv_sec-m_start.tv_sec) + m_end.tv_usec - m_start.tv_usec;

	//Power Down
	if (1 == m_PowerDownFlag) {
		m_PowerDownFlag = 0;
		diff = m_time * CLOCKS_PER_SEC;
	}

	if (diff >= (m_time * CLOCKS_PER_SEC)) {
		//LogError("[%s:%s %u]  ==========================Test:time=%ld \n", __FILE__, __func__, __LINE__, diff);
		m_PowerOnFlag = 0;		//如果先按了唤醒，再去休眠，就会休眠失败；点击PowerOn是会记录标识的，会一直为1
		//读取Fpga数据，组包发送Fpga
		CreateCmd(CMDD_WRITE_PARA, NULL);
		MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], p_bram_parameter, TMP_BUFFER_SIZE);
		MyMemcpy(PowerBuf, pSendBuf, PACKET_PL_SIZE);				
		//发送给PL
		PowerBuf[19 + 438] = 0x01;		//修改配置开关；1：可以修改，0：不能修改
		PowerBuf[19 + 447] = 0xFF;
		PowerBuf[19 + 448] = 0xBF;
		pPowerDown = (UINT *)PowerBuf;		//保存之前的配置
		pTmp = (UINT *)p_bram_parameter;
		for (int i = 0; i < 1052 / 4; i++) {
			pTmp[i] = pPowerDown[i];
		}
		UploadStateCmd(CMDU_REPORT, FPD_STATUS_SLEEP);	

		int retS = -1;
		uint8_t RegisterAdd = 0x08;		//寄存器地址
		uint8_t Num = 0x01;				//数量
		uint16_t Data[Num] = {0x00FB};	//关闭bit2：（11111011=0xFB）
		retS = cSerial.SerialSend(RegisterAdd, Num, Data);
	}	
}

/*********************************************************
* 函 数 名: AwakenLowerPower
* 功能描述: 唤醒低功耗模式
* 参数说明: 	
* 返 回 值：
* 备    注: 
*********************************************************/
void UdpFunc::AwakenLowerPower()
{
	FILE *fp = NULL;
	int value = 1;
	UINT *pPowerDown = NULL;
	UINT *pTmp = NULL;

	fp = fopen("/sys/class/gpio/gpio344/value", "r");
	if (NULL == fp) {
		LogError("[%s:%s %u]  fopen</sys/class/gpio/gpio344/value> fail! \n", __FILE__, __func__, __LINE__);
	}else {
		fscanf(fp, "%d", &value);
		//LogError("[%s:%s %u]  value=%d \n", __FILE__, __func__, __LINE__, value);
		fclose(fp);

		//Power On
		if (1 == m_PowerOnFlag) {
			m_PowerOnFlag = 0;
			value = 0;
		}
		if (0 == value) {
			value = 1;
			//读取Fpga数据，组包发送Fpga
			CreateCmd(CMDD_WRITE_PARA, NULL);
			MyMemcpy(&pSendBuf[OFFSET_PACKAGE_IMAGESLICE], p_bram_parameter, TMP_BUFFER_SIZE);
			MyMemcpy(PowerBuf, pSendBuf, PACKET_PL_SIZE);
			//发送给PL
			PowerBuf[19 + 438] = 0x01;		//修改配置开关；1：可以修改，0：不能修改
			PowerBuf[19 + 447] = 0x00;
			PowerBuf[19 + 448] = 0x00;
			pPowerDown = (UINT *)PowerBuf;		//保存之前的配置
			pTmp = (UINT *)p_bram_parameter;
			for (int i = 0; i < 1052 / 4; i++) {
				pTmp[i] = pPowerDown[i];
			}	
			if (0 == m_ConnectFlag)	
				UploadStateCmd(CMDU_REPORT, FPD_STATUS_WAKEUP);	
			else			
				m_ConnectFlag = 0;
			
			uint8_t RegisterAdd = 0x08;		//寄存器地址
			uint8_t Num = 0x01;				//数量
			uint16_t Data[Num] = {0xFFFF};
			cSerial.SerialSend(RegisterAdd, Num, Data);
		}
	}
}

//switch太多，封装一个接口
void UdpFunc::MySwitch(UCHAR RCmd, UCHAR *pRecvBuf)
{
	UCHAR m_SCode = FPD_STATUS_READY;
	uint8_t RegisterAdd1 = 0x00;		//寄存器地址
	uint8_t Num1 = 0x01;				//数量
	uint8_t RegisterAdd2 = 0x03;		//寄存器地址
	uint8_t Num2 = 0x01;				//数量
	
	switch (RCmd) {
		case RECV_TYPE_ERASE_FLASH:
			//擦除frame（每次update，都会先发一次0x4f；我这里只做回应）
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
			break;
		case RECV_TYPE_Firmware_Update:
			//Update FPGA File
			if ( 0 == Update_SaveFile(pRecvBuf)) {
				UploadResponseCmd(CMDD_PACKET_RETRANS);
				if (1 == mUpSuccess) {
					Update_FpgaFile();
					if (NULL != pUpdatedata) {
						free(pUpdatedata);
						pUpdatedata = NULL;
					}
				}
			}		
			break;
		case RECV_TYPE_PACKET_RETRANS:
			//丢包重传
			PacketRetransmission(pRecvBuf);
			break;
		case RECV_TYPE_FRAME_RETRANS:
			//整帧重传
			FrameRetransmission();
			break;
		case RECV_TYPE_DOWNLOAD_GAIN:
			//下载Gain校正模板
			if (0 == DownloadCurrencyTemplate(pRecvBuf, 1)) {
				UploadResponseCmd(CMDD_PACKET_RETRANS);			
			}
			break;
		case RECV_TYPE_DOWNLOAD_DEFECT:
			//下载Defect校正模板
			if (0 == DownloadCurrencyTemplate(pRecvBuf, 2)) {
				UploadResponseCmd(CMDD_PACKET_RETRANS);			
			}
			break;
		case RECV_TYPE_DOWNLOAD_IAMGE: 
			//读取EMMC图像 
			if (0 == UdpSendImage(pRecvBuf)) {
				CreateCmd(CMDU_REPORT, &m_SCode);																			
				m_RNum = mReadNum;
				if (1 == mFrameImageFlag) {
					m_RNum = m_FrameCount;
				}	
				//帧号+1					
				pSendBuf[HB_ID1] = (m_RNum ) & 0xff;
				pSendBuf[HB_ID2] = (m_RNum >> 8) & 0xff;
				pSendBuf[HB_ID3] = (m_RNum >> 16) & 0xff;
				//图像总数量
				pSendBuf[HB_ID4] = (m_FrameCount ) & 0xff;
				pSendBuf[HB_ID5] = (m_FrameCount >> 8) & 0xff;
				pSendBuf[HB_ID6] = (m_FrameCount >> 16) & 0xff;
				UDP_SEND((UCHAR *)pSendBuf, PACKET_MAX_SIZE);
			}
			break;
		case RECV_TYPE_POWERDOWN:
			//power down					
			m_PowerDownFlag = 1;					
			break;
		case RECV_TYPE_POWERON:	
			//power on
			m_PowerOnFlag = 1;
			break;
		case RECV_TYPE_READ_PARA:
			if (0 != p_bram_parameter[447]) {
				m_PowerOnFlag = 1;
				m_ConnectFlag = 1;		//Connect 唤醒低功耗
			}
			break;
		// case TEST1:
			// cSerial.SerialSend(RegisterAdd1, Num1, NULL);
			// break;
		// case TEST2:
			// cSerial.SerialSend(RegisterAdd2, Num2, NULL);
			// break;
		default:
			break;
	}

}

void UdpFunc::run()
{
	int iRet;
	int length = 0;
	fd_set m_fsRead;
	UCHAR *m_pRecvCmd;  //接收的CMD
	UCHAR recvbuf[PC_SENDBUF_SIZE + 3] = { 0 };
	int TmpPrint = 0;
	FILE *fp = NULL;
	UINT *pTmpPara = NULL;
	UINT *pTmprecvbuf = NULL;

	gettimeofday(&m_start, NULL);

	UdpFuncInit();
	
	LogDebug("[%s:%s %u]  UdpFunc RUN \n", __FILE__, __func__, __LINE__);
	memset(recvbuf, 0x00, PC_SENDBUF_SIZE + 3);
	
	while(1){	
		length = UDP_RECV((UCHAR *)recvbuf, PACKET_MAX_SIZE);
		if (length == PC_SENDBUF_SIZE) {
			gettimeofday(&m_start, NULL);

			m_pRecvCmd = (UCHAR *)(recvbuf + OFFSET_PACKAGE_CMD); //CMD
			if ((*(m_pRecvCmd) == RECV_TYPE_Firmware_Update) || (*(m_pRecvCmd) == RECV_TYPE_DOWNLOAD_GAIN) || (*(m_pRecvCmd) == RECV_TYPE_DOWNLOAD_DEFECT)) {
				//屏蔽不必要的打印
			}else {
				LogDebug("[%s:%s %u]  Recv XDStatic [Cmd=0x%.2x] \n", __FILE__, __func__, __LINE__,  *(m_pRecvCmd));
			}
			//如果处于休眠模式，只能通过3种唤醒方式，屏蔽其它操作
			if (0xFF == p_bram_parameter[447]) {
				if ((*(m_pRecvCmd) == RECV_TYPE_POWERON) || (*(m_pRecvCmd) == RECV_TYPE_READ_PARA)) {}
				else 
					continue;
			}

			p_bram_parameter = p_bram_tail + 0x1000;		//FPGA_ADDRESS+1000
			//因固件不支持memcpy（会出现Bus error），自己封装函数
			//memcpy(p_bram_parameter, recvbuf, PC_SENDBUF_SIZE + 3); //透传1049+3个0x00,共1052;
			//MyMemcpy(p_bram_parameter, recvbuf, PC_SENDBUF_SIZE + 3); //不能配置参数，需要每次拷贝4个字节

			if ((*(m_pRecvCmd) == RECV_TYPE_ERASE_FLASH) || (*(m_pRecvCmd) == RECV_TYPE_Firmware_Update) || (*(m_pRecvCmd) == RECV_TYPE_PACKET_RETRANS) \
				|| (*(m_pRecvCmd) == RECV_TYPE_FRAME_RETRANS) || (*(m_pRecvCmd) == RECV_TYPE_DOWNLOAD_GAIN) || (*(m_pRecvCmd) == RECV_TYPE_DOWNLOAD_DEFECT) \
				|| (*(m_pRecvCmd) == RECV_TYPE_DOWNLOAD_IAMGE) || (*(m_pRecvCmd) == RECV_TYPE_POWERDOWN) || (*(m_pRecvCmd) == RECV_TYPE_POWERON) \
				) {
				//不需要透传的命令
			}else {
				pTmpPara = (UINT *)p_bram_parameter;
				pTmprecvbuf = (UINT *)recvbuf;
				for(int i = 0; i < 1052 / 4; i++) {
					pTmpPara[i] = pTmprecvbuf[i]; //每次4个字节透传
				}
			}
			
			MySwitch(*(m_pRecvCmd), recvbuf);				
			memset(recvbuf, 0x00, PC_SENDBUF_SIZE + 3);             //用完清空		
		}
		else {
			if (0 == p_bram_parameter[447]) {		//读取探测器参数 
				//进入低功耗模式
				InputLowerPower();
			}
			else {
				//唤醒低功耗
				gettimeofday(&m_start, NULL);
				AwakenLowerPower();
			}
		}
	}
                          
	LogError("[%s:%s %u]  UdpFunc END \n", __FILE__, __func__, __LINE__);
	if (NULL != pSendBuf) {
		free(pSendBuf);
		pSendBuf = NULL;
	}

	if (NULL != pUpdatedata) {
		free(pUpdatedata);
		pUpdatedata = NULL;
	}

}
