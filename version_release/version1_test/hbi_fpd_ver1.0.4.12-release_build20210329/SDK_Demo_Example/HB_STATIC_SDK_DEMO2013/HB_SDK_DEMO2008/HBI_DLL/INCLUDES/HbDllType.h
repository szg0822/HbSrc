/*---------------------------------------------------------------------------
* Copyright (c) 2019 上海昊博影像科技有限公司
* All rights reserved.
*
* 文件名称: HbDllType.h
* 文件标识:
* 摘    要: 平板探测器数据结构头文件
*
* 当前版本: 1.0
* 作    者: mhyang
* 完成日期: 2019/05/13
----------------------------------------------------------------------------*/
#ifndef __HB_DLL_TYPE_H_
#define __HB_DLL_TYPE_H_

#define FPD_DATA_BITS                  16
#define PACKET_MAX_SIZE                1050
#define JUMBO_PACKET_MAX_SIZE          8218
#define IMG_PACKET_DATA_MAX_LEN        1024
#define JUMBO_IMG_PACKET_DATA_MAX_LEN  8192
#define CUSTOMEFFECTRECVDATASIZE       1034
#define LIVE_ACQ_MAX_FRAME_NUM         20
#define FLOAT_PACKET_DATA_MAX_LEN      2048
#define UNINT_PACKET_DATA_MAX_LEN      2048
#define USHORT_PACKET_DATA_MAX_LEN     4096
#define PREVIEW_SPACE_NUMBER           4

// 自定义无线网卡类型
#ifndef MIB_IF_TYPE_ETHERNET_WIRELESS
#define MIB_IF_TYPE_ETHERNET_WIRELESS 71
#endif

#define IS_JUMBO 0

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Notice:Fpd Size struct
//#define BADPIXEL_MAX_NUM 3072*3072
//#define FPD_PIXEL_TBALE_MAX_NUM  2
struct FpdPixelMatrixTable
{
	unsigned int fpd_size;
	unsigned int fpd_width;
	unsigned int fpd_height;
	unsigned int fpd_packet_sum;
};
// Notice:Fpd Pixel Matrix Table,size is 14.
static const FpdPixelMatrixTable HB_FPD_SIZE[] = {
#if IS_JUMBO // 8192
	{ 1, 3072, 3072, 2304 },/*,"4343-140um" */
	{ 2, 2560, 3072, 1920 },/*,"3543" */
	{ 3, 1280, 1024, 320  },/*,"1613" */
	{ 4, 2048, 2048, 1024 },/*,"3030" */
	{ 5, 2816, 3584, 2464 },/*,"2530-85um" */
	{ 6, 2048, 1792, 896  },/*,"3025" */
	{ 7, 4302, 4302, 4519 },/*,"4343-100um" - 5240 2952*/
	{ 8, 3072, 3840, 2880 } /*,"2530-50um"*/
#else // 1024
	{ 1, 3072, 3072, 18432 },/*,"4343-140um" */
	{ 2, 2560, 3072, 15360 },/*,"3543" */
	{ 3, 1280, 1024, 2560  },/*,"1613" */
	{ 4, 2048, 2048, 8192  },/*,"3030" */
	{ 5, 2816, 3584, 19712 },/*,"2530-85um" */
	{ 6, 2048, 1792, 7168  },/*,"3025" */
	{ 7, 4302, 4302, 36147 },/*,"4343-100um" - 120 904*/
	{ 8, 3072, 3840, 23040 } /*,"2530-50um"*/
#endif
};

//Note:sofeware work mode
typedef enum
{
	WORKMODE_PRODUCTION_MODE = 0x00, // 软件生产模式
	WORKMODE_DEBUG_MODE = 0x01       // 软件调试模式
}eSOFTWARE_WORKMODE;

// Notice: Image Property
typedef struct ImageProperty
{
	ImageProperty() {
		nSize      = 0;
		nwidth     = 0;
		nheight    = 0;
		datatype   = 0;
		ndatabit   = 0;
		nendian    = 0;
		packet_sum = 0;
		tailPacketSize = 0;
		frame_size = 0;
	}
	unsigned int nSize;				    //fpd_size
	unsigned int nwidth;			    //image width
	unsigned int nheight;			    //image height
	unsigned int datatype;			    //data type：0-unsigned char,1-char,2-unsigned short,3-float,4-double
	unsigned int ndatabit;			    //data bit:0-16bit,1-14bit,2-12bit,3-8bit
	unsigned int nendian;			    //endian:0-little endian,1-biger endian
	unsigned int packet_sum;            //fpd send packet sum per frame
	unsigned int tailPacketSize;        //Tail packet size
	int frame_size;                     //data size per frame
}IMAGE_PROPERTY;

// Notice:Fpd Trigger Work Mode
typedef enum
{
	INVALID_TRIGGER_MODE         = 0x00,
	STATIC_SOFTWARE_TRIGGER_MODE = 0x01, // 
	STATIC_PREP_TRIGGER_MODE     = 0x02, //
	STATIC_HVG_TRIGGER_MODE      = 0x03, //
	STATIC_AED_TRIGGER_MODE      = 0x04,
	DYNAMIC_HVG_TRIGGER_MODE     = 0x05,
	DYNAMIC_FPD_TRIGGER_MODE     = 0x06
}EnumTRIGGER_MODE;

// Notice:fpd software calibrate enable info.
typedef struct software_calibrate_enable_st {
	software_calibrate_enable_st(){
		enableOffsetCalib = false;
		enableGainCalib   = false;
		enableDefectCalib = false;
		enableDummyCalib  = false;
	};
	bool enableOffsetCalib;
	bool enableGainCalib;
	bool enableDefectCalib;
	bool enableDummyCalib;
}SOFTWARE_CALIBRATE_ENABLE;

// Notice:fpd software calibrate enable info.
typedef struct image_correct_enable_st {
	image_correct_enable_st(){
		bFeedbackCfg = false;
		ucOffsetCorrection = 0;
		ucGainCorrection   = 0;
		ucDefectCorrection = 0;
		ucDummyCorrection  = 0;
	};
	bool bFeedbackCfg;                  //true-ECALLBACK_TYPE_ROM_UPLOAD Event,false-ECALLBACK_TYPE_SET_CFG_OK Event
	unsigned char ucOffsetCorrection;   //00-"Do nothing";01-"prepare Offset Correction";  02-"post Offset Correction";
	unsigned char ucGainCorrection;	    //00-"Do nothing";01-"Software Gain Correction";   02-"Hardware Gain Correction"
	unsigned char ucDefectCorrection;   //00-"Do nothing";01-"Software Defect Correction"; 02-"Software Defect Correction"
	unsigned char ucDummyCorrection;    //00-"Do nothing";01-"Software Dummy Correction";  02-"Software Dummy Correction"
}IMAGE_CORRECT_ENABLE;

// Notice: acq mode:static and dynamic
typedef enum
{
	STATIC_ACQ_DEFAULT_MODE = 0x00, // 默认单帧采集
	DYNAMIC_ACQ_DEFAULT_MODE,       // 默认连续采集
	DYNAMIC_ACQ_BARK_MODE,          // 创建Offset模板-连续采集暗场图
	DYNAMIC_ACQ_BRIGHT_MODE,        // 创建Gain模板-连续采集亮场图
	STATIC_ACQ_BRIGHT_MODE,         // 创建Gain模板-单帧采集亮场图
	STATIC_DEFECT_ACQ_MODE,         // 创建Defect模板-单帧采集亮场图
	DYNAMIC_DEFECT_ACQ_MODE,        // 创建Defect模板-连续采集亮场图
	// ADD BY MHYANG 20201013
	OFFSET_TEMPLATE_MODE,           // 连续采集一组暗场图 - offset 
	GAIN_TEMPLATE_MODE,             // 连续采集一组亮场图 - gain
	DEFECT_ACQ_GROUP1,              // 连续采集一组亮场图 - defect group1
	DEFECT_ACQ_GROUP2,              // 连续采集一组亮场图 - defect group2
	DEFECT_ACQ_AND_TEMPLATE         // 连续采集一组亮场图 - defect group3 and generate template
}EnumIMAGE_ACQ_MODE;

// Notice:fpd aqc mode
typedef struct fpd_aqc_mode_st {
	fpd_aqc_mode_st(){
		aqc_mode  = STATIC_ACQ_DEFAULT_MODE;
		ngroupno  = 0;
		nframesum = 0;
		ndiscard  = 0;
		nframeid  = 0;
	};
	EnumIMAGE_ACQ_MODE aqc_mode;
	int  ngroupno;
	int  nframesum;
	int  ndiscard;
	int  nframeid;
}FPD_AQC_MODE;

// Notice: upload template file
// template file type
typedef enum {
	GAIN_TMP   = 0x00,
	DEFECT_TMP = 0x01,
	OFFSET_TMP
}emUPLOAD_FILE_TYPE;
// template file attributes
typedef struct hbi_file_attr_st {
	hbi_file_attr_st() {
		emfiletype = GAIN_TMP;
		memset(filepath, 0x00, MAX_PATH);
	};
	emUPLOAD_FILE_TYPE emfiletype;
	char filepath[MAX_PATH];
}HBIFILE_ATTR;

// template file linked list
typedef struct upload_mode_st {
	upload_mode_st(){
		pFileAttr = NULL;
		next = NULL;
	}
	HBIFILE_ATTR *pFileAttr;
	struct upload_mode_st* next;
}UPLOAD_MODE;

// upload process status
typedef enum {
	UPLOAD_FILE_START  = 0x00,
	UPLOAD_FILE_DURING = 0x01,
	UPLOAD_FILE_STOP   = 0x02
}emUPLOAD_FILE_STATUS;

// Notice: After Each Member Variables, show Variables enum , 
// before '-' is variables' value, after '-' is the meaning of the value;
// each value departed by ';' symbol
enum eRequestCommType
{
	EDL_COMMON_TYPE_INVALVE               = 0x00,
	EDL_COMMON_TYPE_GLOBAL_RESET          = 0x01,
	EDL_COMMON_TYPE_PREPARE               = 0x02,
	EDL_COMMON_TYPE_SINGLE_SHORT          = 0x03,
	EDL_COMMON_TYPE_LIVE_ACQ              = 0x04,
	EDL_COMMON_TYPE_STOP_ACQ              = 0x05,
	EDL_COMMON_TYPE_PACKET_MISS           = 0x06,
	EDL_COMMON_TYPE_FRAME_MISS            = 0x07,
	EDL_COMMON_TYPE_DUMMPING              = 0x08,
	EDL_COMMON_TYPE_END_RESPONSE          = 0x0A, // End response packet
	EDL_COMMON_TYPE_SET_RAM_PARAM_CFG     = 0x10,
	EDL_COMMON_TYPE_GET_RAM_PARAM_CFG     = 0x11,
	EDL_COMMON_TYPE_SET_ROM_PARAM_CFG     = 0x12,
	EDL_COMMON_TYPE_GET_ROM_PARAM_CFG     = 0x13,
	EDL_COMMON_TYPE_SET_FACTORY_PARAM_CFG = 0x14,
	EDL_COMMON_TYPE_GET_FACTORY_PARAM_CFG = 0x15,
	EDL_COMMON_TYPE_RESET_FIRM_PARAM_CFG  = 0x16,
	EDL_COMMON_UPLOAD_GAIN_TEMPLATE       = 0x2F,// Upload gain template
	EDL_COMMON_UPLOAD_DEFECT_TEMPLATE     = 0x30,// Upload defect template
	EDL_COMMON_ERASE_FIRMWARE             = 0x4F,// Erase old firmware package request
	EDL_COMMON_UPDATE_FIRMWARE            = 0x50,// Update new firmware package request
	EDL_COMMON_TYPE_SET_AQC_MODE          = 0xFF
};

// Notice: After Each Member Variables, show Variables enum , 
// before '-' is variables' value, after '-' is the meaning of the value;
// each value departed by ';' symbol
enum eCallbackEventCommType
{
	ECALLBACK_TYPE_INVALVE           = 0X00,
	ECALLBACK_TYPE_COMM_RIGHT        = 0X01,
	ECALLBACK_TYPE_COMM_WRONG        = 0X02,
	ECALLBACK_TYPE_DUMMPLING         = 0X03,
	ECALLBACK_TYPE_ACQ_END           = 0X04,
	ECALLBACK_TYPE_UPDATE_FIRMWARE   = 0X06, // Update old firmware package answer 
	ECALLBACK_TYPE_ERASE_FIRMWARE    = 0X07, // Update new firmware package answer
	ECALLBACK_TYPE_FPD_STATUS        = 0X09, // Status package
	ECALLBACK_TYPE_ROM_UPLOAD        = 0X10,
	ECALLBACK_TYPE_RAM_UPLOAD        = 0X11,
	ECALLBACK_TYPE_FACTORY_UPLOAD    = 0X12,
	ECALLBACK_TYPE_SINGLE_IMAGE      = 0X51, 
	ECALLBACK_TYPE_MULTIPLE_IMAGE    = 0X52,
	ECALLBACK_TYPE_PREVIEW_IMAGE     = 0X53,
	ECALLBACK_TYPE_PACKET_MISS       = 0X5B,
	ECALLBACK_TYPE_SENDTO_WIZARD     = 0XA0,
	ECALLBACK_TYPE_ADMIN_CFG_UPDATA  = 0XA1,
	ECALLBACK_TYPE_USER_CFG_UPDATA   = 0XA2,
	ECALLBACK_TYPE_PACKET_MISS_MSG   = 0XA4,
	ECALLBACK_TYPE_THREAD_EVENT      = 0XA5,
	ECALLBACK_TYPE_CALIBRATE_ACQ_MSG = 0XA6,
	ECALLBACK_TYPE_OFFSET_ERR_MSG    = 0XA7,
	ECALLBACK_TYPE_GAIN_ERR_MSG      = 0XA8,
	ECALLBACK_TYPE_DEFECT_ERR_MSG    = 0XA9,
	ECALLBACK_TYPE_NET_ERR_MSG       = 0XAA,
	ECALLBACK_TYPE_SET_CFG_OK        = 0XAB, // 设置参数成功
	ECALLBACK_TYPE_ACQ_SUCCESS       = 0XAC, // 反馈采集到一帧图像成功
	ECALLBACK_TYPE_GENERATE_TEMPLATE = 0XAD, // 生成模板
	ECALLBACK_TYPE_FILE_NOTEXIST     = 0XAE  // 文件不存在
};

enum eCallbackUpdateFirmwareStatus
{
	ECALLBACK_UPDATE_STATUS_ERASE    = 0XB0,
	ECALLBACK_UPDATE_STATUS_PROGRESS = 0XB1,
	ECALLBACK_UPDATE_STATUS_RESULT   = 0XB2
};

enum eCallbackTemplateStatus
{
	ECALLBACK_TEMPLATE_BEGIN          = 0X00,
	ECALLBACK_TEMPLATE_INVALVE_PARAM  = 0X01,
	ECALLBACK_TEMPLATE_MALLOC_FAILED  = 0X02,
	ECALLBACK_TEMPLATE_SEND_FAILED    = 0X03,
	ECALLBACK_TEMPLATE_STATUS_ABORMAL = 0X04,
	ECALLBACK_TEMPLATE_FRAME_NUM      = 0X05,
	ECALLBACK_TEMPLATE_TIMEOUT        = 0X06,
	ECALLBACK_TEMPLATE_MEAN           = 0X07,
	ECALLBACK_TEMPLATE_GENERATE       = 0X08,
	ECALLBACK_TEMPLATE_RESULT         = 0X09
};

// Notice: call back function
// @USER_CALLBACK_HANDLE_ENVENT
// @cmd:enum eEventCallbackCommType
// @buff
// @len
// @nID
typedef int(*USER_CALLBACK_HANDLE_ENVENT)(unsigned char cmd, void *buff, int len, int nID);

// Notice: call back function
// @USER_CALLBACK_HANDLE_PROCESS
// @cmd:enum eAnswerCallbackCommType
// @buff
// @len
typedef int(*USER_CALLBACK_HANDLE_PROCESS)(unsigned char cmd, int retcode, void *inContext);

// Notice: calibrate template raw file call back info
// 2019/09/03
typedef struct callback_raw_info_st
{
	callback_raw_info_st() {
		memset(szRawName, 0x00, MAX_PATH);
		dMean = 0.0;
	};
	char   szRawName[MAX_PATH];	// 返回数据文件名称
	double dMean;               // 灰度均值
}ECALLBACK_RAW_INFO;

// Notice: calibrate
const unsigned short BADPIX = 65535;
const unsigned short GOODPIX = 0;
enum EnumTempType
{
	OFFSETFILE=0x00,
	GAINA,
	GAINB,
	PREVIEWOFF,
	PREVIEWGNA,
	PREVIEWGNB
};

struct calibrateModeItem
{
	int id;                    //ID标识
	int dosvalue;              //剂量值
	int groupnum;              //获取测试组总数
	char offsetpath[MAX_PATH]; //亮场offset文件路径
	int averagegray;           //该剂量下所有帧的平均灰度
};

// Notice:generate calibrate template input param
typedef struct calibrate_input_param_st {
	calibrate_input_param_st()
	{
		image_w       = 0;
		image_h       = 0;
		roi_x         = 0;
		roi_y         = 0;
		roi_w         = 0;
		roi_h         = 0;
		group_sum     = 0;
		per_group_num = 0;
	}
	int image_w;       // image width
	int image_h;       // image height
	int roi_x;         // ROI left
	int roi_y;         // ROI top
	int roi_w;         // ROI width
	int roi_h;         // ROI height
	int group_sum;     // group sum
	int per_group_num; // num per group
}CALIBRATE_INPUT_PARAM;

// Notice: After Each Member Variables, show Variables enum , 
// before '-' is variables' value, after '-' is the meaning of the value;
// each value departed by ';' symbol
#pragma pack(1)
// System Manufacture Information：50
typedef struct SysBaseInfo_st {
	// Register_1
	unsigned char   	m_byProductType;      				//01	//01-Flat Panel Detector;  02-Linear Array Detector
	// Register_2
	unsigned char    	m_byXRaySensorType;   				//01	//01-Amorphous Silicon; 02-CMOS; 03-IGZO; 04-LTPS; 05-Amorphous Selenium
	// Register_3
	unsigned char    	m_byPanelSize;        				//01	//01-1717 inch Panel Size; 02-1417; 03-1414; 04-1212; 
	//05-1012; 06-0912; 07-0910; 08-0909; 09-0506; 10-0505; 11-0503
	// Register_4
	unsigned char   	m_byPixelPitch;						//01	//01-27 um; 02-50; 03-75; 04-85; 05-100; 06-127; 07-140; 08-150; 09-180; 10-200 11-205; 12-400; 13-1000
	// Register_5
	unsigned char       m_byPixelMatrix;     				//01	//01-"3072 x 3072"; 02-"2536 x 3528"; 03-"2800 x 2304"; 04-"2304 x 2304"; 05-"2048 x 2048"; 06-"1536 x 1536"; 
	//07-"1024 x 1536"; 08-"1024 x 1024"; 09-"1024 x 0768"; 10-"1024 x 0512"; 11-"0768 x 0768"; 
	//12-"0512 x 0512"; 13-"0512 x 0256"; 14-"0256 x 0256"
	// Register_6
	unsigned char       m_byScintillatorType;				//01	//01-DRZ Standard; 02-DRZ High; 03-DRZ Plus; 04-PI-200; 05-Structured GOS; 06-CSI Evaporation; 07-CSI Attach;
	// Register_7
	unsigned char       m_byScanLineFanoutMode;				//01	//01-Single Side Single Gate; 02-Dual Side Single Gate; 03-Single Side Dual Gate; 04-Dual Side Dual Gate;
	// Register_8
	unsigned char       m_byReadoutLineFanoutMode;  		//01	//01-Single Side Single Read; 02-Dual Side Single Read; 03-Single Side Dual Read; 04-Dual Side Dual Read;
	// Register_9
	unsigned char       m_byFPDMode;						//01	//01-Static; 02-Dynamic;
	// Register_10
	unsigned char       m_byFPDStyle;						//01	//01-Fixed; 02-Wireless; 03-Portable;
	// Register_11
	unsigned char       m_byApplicationMode;        		//01	//01-Medical; 02-Industry;
	// Register_12
	unsigned char       m_byGateChannel;					//01	//01-"600 Channel"; 02-"512 Channel"; 03-"384 Channel"; 04-"256 Channel"; 05-"128 Channel"; 06-"064 Channel"
	// Register_13
	unsigned char		m_byGateType;						//01	//01-"HX8677"; 02-"HX8698D"; 03-"NT39565D"
	// Register_14
	unsigned char		m_byAFEChannel;						//01	//01-"64 Channel"; 02-"128 Channel"; 03-"256 Channel"; 04-"512 Channel";
	// Register_15
	unsigned char		m_byAFEType;						//01	//01-"AFE0064"; 02-"TI COF 2256"; 03-"AD8488"; 04-"ADI COF 2256";
	// Register_16~Register_17
	unsigned short		m_sGateNumber;						//02	//Gate Number
	// Register_18~Register_19
	unsigned short		m_sAFENumber;						//02	//AFE Number	
	// Register_20~Register_21
	unsigned short		m_sImageWidth;						//02	//Image Width // modified by mhyang 20191220
	// Register_22~Register_23
	unsigned short		m_sImageHeight;						//02	//Image Height
	// Register_24~Register_37
	unsigned char		m_cSnNumber[14];				    //14	//sn number   // modified by mhyang 20200401
	// Register_38~Register_100.
	unsigned char		m_abyReservedFrom38To100[63];	    //63//////Registers 38To100(include) Are Reserved;
}SysBaseInfo;

// System Manufacture Information：50
typedef struct SysManuInfo_st {
	// Register_1~Register_4
	unsigned char       m_byHead[4];
	// Register_5~Register_16
	unsigned char       m_abyFPDSerialNumber[12];   		//12	//FPD SN Byte 01-12;
	// Register_17~Register_18
	unsigned short      m_sYear;							//02	//Year
	// Register_19~Register_20
	unsigned short      m_sMouth;							//02	//Month
	// Register_21~Register_22
	unsigned short      m_sDay;								//02	//Day
	// Register_23~Register_24
	unsigned short      m_sEOSFirmwareVersion;			    //02	//EOS Firmware Version
	// Register_25~Register_26
	unsigned short      m_sEOSFirmwareBuildingTime;		    //02	//EOS Firmware Building Time
	// Register_27~Register_28
	unsigned short      m_sMasterFPGAFirmwareVersion;		//02	//Master FPGA Firmware Version
	// Register_29~Register_31
	unsigned char       m_byMasterFPGAFirmwareBuildingTime1;//01	//Master FPGA Firmware Building Time1
	unsigned char       m_byMasterFPGAFirmwareBuildingTime2;//01	//Master FPGA Firmware Building Time2
	unsigned char       m_byMasterFPGAFirmwareBuildingTime3;//01	//Master FPGA Firmware Building Time3
	// Register_32~Register_34
	unsigned char       m_byMasterFPGAFirmwareStatus1;		//01	//Master FPGA Firmware status1
	unsigned char       m_byMasterFPGAFirmwareStatus2;		//01	//Master FPGA Firmware status2
	unsigned char       m_byMasterFPGAFirmwareStatus3;      //01	//Master FPGA Firmware status3
	// Register_35~Register_36
	unsigned short       m_sMCUFirmwareVersion;			    //02	//MCU Firmware Version
	// Register_37~Register_38
	unsigned short       m_sMCUFirmwareBuildingTime;		//02	//MCU Firmware Building Time
	// Register_39~Register_50
	unsigned char		m_abySysManuInfoReserved[12];
}SysManuInfo;

// System Status Information：28
typedef struct SysStatusInfo_st {
	// Register_1~Register_4
	unsigned char		m_byHead[4];
	// Register_5~Register_8
	unsigned int		m_unTemperature;					//04	//Temperature
	// Register_9~Register_12
	unsigned int 		m_unHumidity;						//04	//Humidity
	// Register_13~Register_16
	unsigned int 		m_unDose;							//04	//Dose
	// Register_17~Register_28
	unsigned char		m_abySysStatusInfoReserved[12];
}SysStatusInfo;

// Gigabit Ethernet Information：40
typedef struct EtherInfo_st {
	// Register_1~Register_4
	unsigned char       m_byHead[4];
	// Register_9~Register_14
	unsigned char       m_bySourceMAC[6];                   //06	//Source MAC
	// Register_15~Register_18
	unsigned char       m_bySourceIP[4];                    //04	//Source IP
	// Register_5~Register_6
	unsigned short		m_sSourceUDPPort;					//02	//Source UDP Port
	// Register_19~Register_24
	unsigned char       m_byDestMAC[6];                     //06	//Dest MAC
	// Register_25~Register_28
	unsigned char       m_byDestIP[4];                      //04	//Dest IP
	// Register_7~Register_8
	unsigned short		m_sDestUDPPort;						//02	//Dest UDP Port	
	// Register_29~Register_40
	unsigned char		m_abyEtherInfoReserved[12];
}EtherInfo;

// High Voltage Generator Interface Trigger Mode Information：21
typedef struct HiVolTriggerModeInfo_st {
	// Register_1~Register_4
	unsigned char   m_byHead[4];
	// Register_5
	unsigned char		m_byPrepSignalValidTrigger;			//01	//01-"High Level Trigger, Low Level Invalid"; 02-"Low Level Trigger, High Level Invalid"
	//03-"Positive Edge Trigger, Negative Edge Invalid"; 04-"Negative Edge Trigger, Positive Edge Invalid"
	// Register_6
	unsigned char		m_byExposureEnableSignal;			//01	//01-"High Level Trigger, Low Level Invalid"; 02-"Low Level Trigger, High Level Invalid"
	//03-"Positive Edge Trigger, Negative Edge Invalid"; 04-"Negative Edge Trigger, Positive Edge Invalid"
	// Register_7
	unsigned char		m_byXRayExposureSignalValid;		//01	//01-"High Level Trigger, Low Level Invalid"; 02-"Low Level Trigger, High Level Invalid"
	//03-"Positive Edge Trigger, Negative Edge Invalid"; 04-"Negative Edge Trigger, Positive Edge Invalid"
	// Register_8
	unsigned char		m_bySyncInSignalValidTrigger;		//01	//01-"High Level Trigger, Low Level Invalid"; 02-"Low Level Trigger, High Level Invalid"
	//03-"Positive Edge Trigger, Negative Edge Invalid"; 04-"Negative Edge Trigger, Positive Edge Invalid";
	// Register_9
	unsigned char		m_bySyncOutSignalValidTrigger;		//01	//01-"High Level Trigger, Low Level Invalid"; 02-"Low Level Trigger, High Level Invalid";

	// Register_10~Register_21
	unsigned char		m_abyEtherInfoReserved[12];
}HiVolTriggerModeInfo;

// System Configuration Information：127
typedef struct SysCfgInfo_st {
	// Register_1~Register_4
	unsigned char      m_byHead[4];
	// Register_5
	unsigned char      m_byWorkMode;						//01	// 01-"Static: Software Mode" 02-"Static: Prep Mode"; 03-"Static Hvg Trigger Mode"; 
	                                                                // 04-Static AED Trigger Mode, 05-Dynamic Hvg In Mode,06-Dynamic Fpd Out Mode.
	// Register_6
	unsigned char 	   m_byPreviewModeTransmission;		    //01	//00-"Just transmmit full resolution image matrix"; 01-"Just transmmit the preview image"; 
	//02-"Transmmit the preview image and full resolution image"
	// Register_7
	unsigned char 	   m_byPositiveVsNegativeIntegrate;	    //01	//01-"Electron"; 02-"Hole" //04-"Static: Inner Trigger Mode"; 05-"Dynamic: Sync In Mode"; 06-"Dynamic: Sync Out Mode"

	// Register_8
	unsigned char 	   m_byBinning;						    //01	//01-"1x1"; 02-"2x2 Binning"; 03-"3x3 Binning"; 04-"4x4 Binning";

	// Register_9
	unsigned char      m_byIntegratorCapacitorSelection;	//01	//01-"Integrator Capacitor PGA0"; 02-"Integrator Capacitor PGA1"; 03-"PGA2";......;08-"Integrator Capacitor PGA7"

	// Register_10
	unsigned char       m_byAmplifierGainSelection;			//01	//01-"Amplifier Gain AMP0"; 02-"Amplifier Gain AMP1"; 03-"Amplifier Gain AMP2"; 04-"Amplifier Gain AMP3";
	//05-"Amplifier Gain AMP4"; 06-"Amplifier Gain AMP5"; 07-"Amplifier Gain AMP6"; 08-"Amplifier Gain AMP7";
	// Register_11
	unsigned char		m_bySelfDumpingEnable;				//01	//01-"Enable Dumping Process"; 02-"Disable Dumping Process";
	// Register12
	unsigned char		m_bySelfDumpingProcessing;			//01	//01-"Clear Process for Dumping"; 02-"Acquisition Process for Dumping";
	
	// add 20190705
	unsigned char       m_byClearFrameNumber;               //01	//01-"Clear Frame"; 02-"Clear Frame";03-"Clear Frame";04-"Clear Frame";
	
	// Register_13
	unsigned char		m_byPDZ;							//01	//01-"Disable PDZ"; 02-"Enable PDZ"
	// Register_14
	unsigned char		m_byNAPZ;							//01	//01-"Disable NAPZ"; 02-"Enable NAPZ"

	// Register15
	unsigned char		m_byInnerTriggerSensorSelection;	//01	//00-"No Selection"; 01-"I Sensor Selection"; 02-"II Sensor Selection"; 
	//03-"III Sensor Selection"; 04-"IV Sensor Selection"; 05-"V Sensor Selection"                                                                
	// Register_16~Register_17
	unsigned short		m_sZoomBeginRowNumber;				//02	//Zoom Begin Row Number
	// Register_18~Register_19
	unsigned short		m_sZoomEndRowNumber;				//02	//Zoom End Row Number
	// Register_20~Register_21
	unsigned short		m_sZoomBeginColumnNumber;			//02	//Zoom Begin Column Number
	// Register_22~Register_23
	unsigned short		m_sZoomEndColumnNumber;				//02	//Zoom End Column Number

	// Register24~Register27
	unsigned int 		m_unSelfDumpingSpanTime;			//04	//unit: ms
	// Register28~Register31
	unsigned int 		m_unContinuousAcquisitionSpanTime;	//04	//unit: ms
	// Register32~Register35
	unsigned int 		m_unPreDumpingDelayTime;			//04	//unit: ms
	// Register36~Register39
	unsigned int 		m_unPreAcquisitionDelayTime;		//04	//unit: ms
	// Register40~Register43
	unsigned int 		m_unPostDumpingDelayTime;			//04	//unit: ms
	// Register44~Register47
	unsigned int 		m_unPostAcquisitionDelayTime;		//04	//unit: ms
	// Register48~Register51
	unsigned int 		m_unSyncInExposureWindowTime;		//04	//unit: ms
	// Register52~Register55
	unsigned int 		m_unSyncOutExposureWindowTime;		//04	//unit: ms
	// Register56~Register59
	unsigned int 		m_unTFTOffToIntegrateOffTimeSpan;	//04	//unit: ms
	// Register_60~Register_63
	unsigned int 		m_unIntegrateTime;					//04	//unit: ms
	// Register_64~Register_67
	unsigned int 		m_unPreFrameDelay;					//04	//unit: ms
	// Register_68~Register_71
	unsigned int 		m_unPostFrameDelay;					//04	//unit: ms
	// Register_72~Register_75
	unsigned int 		m_unPreRowDelay;					//04	//unit: ms
	// Register_76~Register_79
	unsigned int 		m_unPostRowDelay;					//04	//unit: ms
	// Register_80
	unsigned short		m_sHvgRdyMode;				        //02	//0-"Edge Trigger"; 非0-"[1~255]:Config Delay,unit:n*128 ms"
	unsigned short		m_sSaecPreRdyTm;				    //02	//SAEC pre ready time
	unsigned short		m_sSaecPostRdyTm;				    //02	//SAEC post ready time
	unsigned char       m_byDefectThreshold;                //01	//Defect correction:Calculating the threshold of bad points
	unsigned char		m_abySysCfgInfoReserved[41];
}SysCfgInfo;

// Image Calibration Configuration：20
typedef struct ImgCaliCfg_st {
	// Register_1~Register_4
	unsigned char		m_byHead[4];                        //04    //head 
	// Register_5
	unsigned char		m_byOffsetCorrection;				//01	//00-"Do nothing";01-"Software Offset Correction"; 02-"Hardware Offset Correction";
	// Register_6
	unsigned char		m_byGainCorrection;					//01	//00-"Do nothing";01-"Software Gain Correction";   02-"Hardware Gain Correction"
	// Register_7
	unsigned char		m_byDefectCorrection;				//01	//00-"Do nothing";01-"Software Defect Correction"; 02-"Software Defect Correction"
	// Register_8
	unsigned char		m_byDummyCorrection;				//01	//00-"Do nothing";01-"Software Dummy Correction";  02-"Software Dummy Correction"
	// Register_9~Register_20
	unsigned char		m_abyImgCaliCfgReserved[12];		//12//////Registers 114(include) Are Reserved;
}ImgCaliCfg;

// Voltage Adjust Configuration：48
typedef struct VolAdjustCfg_st {
	// Register_1~Register_4
	unsigned char		m_byHead[4];
	// Register_5~Register_8
	unsigned int 		m_unVCOM;							//04	//VCOM
	// Register_9~Register_12
	unsigned int 		m_unVGG;							//04	//VGG
	// Register_13~Register_16
	unsigned int 		m_unVEE;							//04	//VEE
	// Register_17~Register_20
	unsigned int 		m_unVbias;							//04	//Vbias	
	// Register_21~Register_36
	unsigned int 		m_unComp[4];						//04	//Comp1	// Register114
	// Register_37~Register_48
	unsigned char		m_abyVolAdjustCfgReserved[12];		//12//////Registers 114(include) Are Reserved;
}VolAdjustCfg;

// TI COF Parameter Configuration：84
typedef struct TICOFCfg_st {
	// Register_1~Register_4
	unsigned char		m_byHead[4];
	// Register_5~Register_64
	unsigned short		m_sTICOFRegister[30];
	// Register_65~Register_84
	unsigned char		m_abyTICOFCfgReserved[20];
}TICOFCfg;

//CMOS Parameter Configuration：116
typedef struct CMOSCfg_st {
	// Register_1~Register_4
	unsigned char		m_byHead[4];
	// Register_5~Register_68
	unsigned short		m_sCMOSRegister[32];
	// Register_69~Register_116
	unsigned char		m_abyCMOSCfgReserved[48];
}CMOSCfg;

// ADI COF Parameter Configuration：390
typedef struct ADICOFCfg_st {
	// Register_1~Register_4
	unsigned char		m_byHead[4];
	// Register_5~Register_19
	unsigned short		m_sADICOFRegister[15];
	// Register_20~Register_375
	unsigned char		m_abyADICOFCfgReserved[355];
}ADICOFCfg;

// 1024 byte regedit
typedef struct RegCfgInfo_s
{
	// System base Information：100,Register_1~Register_100
	SysBaseInfo m_SysBaseInfo;
	// System Manufacture Information：50,Register_101~Register_150
	SysManuInfo m_SysManuInfo;
	// System Status Information：28,Register_151~Register_178
	SysStatusInfo m_SysStatusInfo;
	// Gigabit Ethernet Information：40,Register_179~Register_218
	EtherInfo m_EtherInfo;
	// High Voltage Generator Interface Trigger Mode Information：21,Register_219~Register_239
	HiVolTriggerModeInfo m_HiVolTriggerModeInfo;
	// System Configuration Information：127,Register_240~Register_366
	SysCfgInfo m_SysCfgInfo;
	// Image Calibration Configuration：20,Register_367~Register_386
	ImgCaliCfg m_ImgCaliCfg;
	// Voltage Adjust Configuration：48,Register_387~Register_434
	VolAdjustCfg m_VolAdjustCfg;
	// TI COF Parameter Configuration：84,Register_435~Register_518
	TICOFCfg m_TICOFCfg;
	// CMOS Parameter Configuration：116,Register_519~Register_634
	CMOSCfg m_CMOSCfg;
	// ADI COF Parameter Configuration：390,Register_635~Register_1024
	ADICOFCfg m_ADICOFCfg;
	///////////////////////
}RegCfgInfo;
#pragma pack()

#endif // __HB_DLL_TYPE_H_
