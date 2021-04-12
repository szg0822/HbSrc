/*---------------------------------------------------------------------------
* Copyright (c) 2019 上海昊博影像科技有限公司
* All rights reserved.
*
* 文件名称: HbDllError.h
* 文件标识:
* 摘    要: 平板探测器错误信息头文件
*
* 当前版本: 1.0
* 作    者: mhyang
* 完成日期: 2019/05/13
----------------------------------------------------------------------------*/
#ifndef __HB_DLL_ERROR_H_
#define __HB_DLL_ERROR_H_

enum HBIRETCODE {
	HBI_SUCCSS = 0,
	HBI_ERR_OPEN_DETECTOR_FAILED  = 8001,
	HBI_ERR_INVALID_PARAMS        = 8002,
	HBI_ERR_CONNECT_FAILED        = 8003,
	HBI_ERR_MALLOC_FAILED         = 8004,
	HBI_ERR_RELIMGMEM_FAILED      = 8005,
	HBI_ERR_RETIMGMEM_FAILED      = 8006,
	HBI_ERR_NODEVICE              = 8007,
	HBI_ERR_NODEVICE_TRY_CONNECT  = 8008,
	HBI_ERR_DEVICE_BUSY           = 8009,
	HBI_ERR_SENDDATA_FAILED       = 8010,
	HBI_ERR_RECEIVE_DATA_FAILED   = 8011,
	HBI_ERR_COMMAND_DISMATCH      = 8012,
	HBI_ERR_NO_IMAGE_RAW          = 8013,
	HBI_ERR_PTHREAD_ACTIVE_FAILED = 8014,
	HBI_ERR_STOP_ACQUISITION      = 8015,
	HBI_ERR_INSERT_FAILED         = 8016,
	HBI_ERR_GET_CFG_FAILED        = 8017,
	HBI_NOT_SUPPORT               = 8018,
	HBI_REGISTER_CALLBACK_FAILED  = 8019,
	HBI_SEND_MESSAGE_FAILD        = 8020,
	HBI_ERR_WORKMODE              = 8021,
	HBI_FAILED                    = 8822,
	HBI_FILE_NOT_EXIST            = 8823,
	HBI_END
};

struct CodeStringTable
{
	int		     num;
	int        HBIRETCODE;
	const char *errString;
};
static const CodeStringTable CrErrStrList[] = {
	{ 0,  HBI_SUCCSS, "Success" },
	{ 1,  HBI_ERR_OPEN_DETECTOR_FAILED, "Open device driver failed" },
	{ 2,  HBI_ERR_INVALID_PARAMS, "Parameter error" },
	{ 3,  HBI_ERR_CONNECT_FAILED, "Connect failed" },
	{ 4,  HBI_ERR_MALLOC_FAILED, "Malloc memory failed" },
	{ 5,  HBI_ERR_RELIMGMEM_FAILED, "Releaseimagemem fail" },
	{ 6,  HBI_ERR_RETIMGMEM_FAILED, "ReturnImageMem fail" },
	{ 7,  HBI_ERR_NODEVICE, "No Device" },
	{ 8,  HBI_ERR_NODEVICE_TRY_CONNECT, "No Device,Try again" },
	{ 9,  HBI_ERR_DEVICE_BUSY, "Device busy" },
	{ 10, HBI_ERR_SENDDATA_FAILED, "SendData failed" },
	{ 11, HBI_ERR_RECEIVE_DATA_FAILED, "Receive Data failed" },
	{ 12, HBI_ERR_COMMAND_DISMATCH, "Command dismatch" },
	{ 13, HBI_ERR_NO_IMAGE_RAW, "No Image raw" },
	{ 14, HBI_ERR_PTHREAD_ACTIVE_FAILED, "Pthread active failed" },
	{ 15, HBI_ERR_STOP_ACQUISITION, "Pthread stop data acquisition failed" },
	{ 16, HBI_ERR_INSERT_FAILED, "insert calibrate mode failed" },
	{ 17, HBI_ERR_GET_CFG_FAILED, "get device config failed" },
	{ 18, HBI_NOT_SUPPORT, "not surport yet" },
	{ 19, HBI_REGISTER_CALLBACK_FAILED, "failed to register callback function" },
	{ 20, HBI_SEND_MESSAGE_FAILD, "send message failed" },
	{ 21, HBI_ERR_WORKMODE, "switch work mode failed" },
	{ 22, HBI_FAILED, "operation failed" },
    { 23, HBI_FILE_NOT_EXIST, "file does not exist" }
};
///*********************************************
//* 描  述：通过返回码获取当前错误信息
//* 函  数：GetHbiErrorString
//* 参  数：CodeStringTable* inTable 返回码表
//*         int count                返回码个数
//*         int recode               返回码
//* 返回值：
//*         const char * 返回字符串
//* 备  注: BY MH.YANG 2019/12/09
//*********************************************/
//const char *GetHbiErrorString(CodeStringTable* inTable, int count, int recode)
//{
//	const char *retString = "Unknown";
//	for(int i = 0; i < count; i++) {
//		if(inTable[i].HBIRETCODE == recode) {
//			retString = inTable[i].errString;
//			break;
//		}
//	}
//	return retString;
//}
//int main()
//{
//		static const CodeStringTable CrErrStrList[] = {
//			{ 0,  HBI_SUCCSS, "Success" },
//			{ 1,  HBI_ERR_OPEN_DETECTOR_FAILED, "Open device driver failed" },
//			{ 2,  HBI_ERR_INVALID_PARAMS, "Parameter error" },
//			{ 3,  HBI_ERR_CONNECT_FAILED, "Connect failed" },
//			{ 4,  HBI_ERR_MALLOC_FAILED, "Malloc memory failed" },
//			{ 5,  HBI_ERR_RELIMGMEM_FAILED, "Releaseimagemem fail" },
//			{ 6,  HBI_ERR_RETIMGMEM_FAILED, "ReturnImageMem fail" },
//			{ 7,  HBI_ERR_NODEVICE, "No Device" },
//			{ 8,  HBI_ERR_NODEVICE_TRY_CONNECT, "No Device,Try again" },
//			{ 9,  HBI_ERR_DEVICE_BUSY, "Device busy" },
//			{ 10, HBI_ERR_SENDDATA_FAILED, "SendData failed" },
//			{ 11, HBI_ERR_RECEIVE_DATA_FAILED, "Receive Data failed" },
//			{ 12, HBI_ERR_COMMAND_DISMATCH, "Command dismatch" },
//			{ 13, HBI_ERR_NO_IMAGE_RAW, "No Image raw" },
//			{ 14, HBI_ERR_PTHREAD_ACTIVE_FAILED, "Pthread active failed" },
//			{ 15, HBI_ERR_STOP_ACQUISITION, "Pthread stop data acquisition failed" },
//			{ 16, HBI_ERR_INSERT_FAILED, "insert calibrate mode failed" },
//			{ 17, HBI_ERR_GET_CFG_FAILED, "get device config failed" },
//			{ 16, HBI_NOT_SUPPORT, "not surport yet" },
//		};
//	int nCode = 8001;
//	const char *tmpString = GetErrString(CodeStringTable, sizeof(CrErrStrList)/sizeof(CodeStringTable), uValue);
//	printf("nCode=%d,tmpString=%s\n", nCode, tmpString);
//	return 0;
//}
#endif // __HB_DLL_ERROR_H_
