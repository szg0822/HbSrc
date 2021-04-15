/*---------------------------------------------------------------------------
* Copyright (c) 2019 上海昊博影像科技有限公司
* All rights reserved.
*
* 文件名称: HbFpdDll.h
* 文件标识:
* 摘    要: 平板探测器导出函数头文件
*
* 当前版本: 1.0
* 作    者: mhyang
* 创建日期: 2019/04/18
* 修改日期: 2020/06/29
----------------------------------------------------------------------------*/
#pragma once
#ifndef __HB_FPD_DLL_H_
#define __HB_FPD_DLL_H_

#include "HbDllType.h"
#include "HbDllError.h"

#ifdef _DLL_EX_IM
#define HB_PDF_DLL __declspec(dllexport)
#else
#define HB_PDF_DLL __declspec(dllimport)
#endif

#ifdef _cplusplus
extern "C" {
#endif
/*********************************************************
* 编    号: No001
* 函 数 名: HBI_Init
* 功能描述: 初始化动态库
* 参数说明:
* 返 回 值：void *
		    失败：NULL,成功：非空
* 备    注:
*********************************************************/
HB_PDF_DLL void* __stdcall HBI_Init();

/*********************************************************
* 编    号: No002
* 函 数 名: HBI_Destroy
* 功能描述: 释放动态库资源
* 参数说明:
        In: void *handle - 句柄
		Out: 无
* 返 回 值：void
* 备    注:
*********************************************************/
HB_PDF_DLL void __stdcall HBI_Destroy(void *handle);

/*********************************************************
* 编    号: No003
* 函 数 名: HBI_ConnectDetector
* 功能描述: 建立连接
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			char *szRemoteIp - 平板IP地址,如192.168.10.40
			unsigned short remotePort - 平板端口,如32897(0x8081)
			char *szlocalIp - 上位机地址,如192.168.10.20
			unsigned short localPort -上位机端口,如192.168.10.40
			int timeout - 上位机接收超时,如20ms
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ConnectDetector(void *handle, char *szRemoteIp, unsigned short remotePort, char *szlocalIp, unsigned short localPort, int timeout=20);

/*********************************************************
* 编    号: No004
* 函 数 名: HBI_DisConnectDetector
* 功能描述: 断开连接
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
			0   - 成功
			非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_DisConnectDetector(void *handle);

/*********************************************************
* 编    号: No005
* 函 数 名: HBI_RegEventCallBackFun
* 功能描述: 注册回调函数
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		USER_CALLBACK_HANDLE_ENVENT handleEventfun - 注册回调函数
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_RegEventCallBackFun(void *handle, USER_CALLBACK_HANDLE_ENVENT handleEventfun);

/*********************************************************
* 编    号: No006
* 函 数 名: HBI_RegProgressCallBack
* 功能描述: 注册回调函数
* 参数说明: 处理固件升级反馈信息
		In: void *handle - 句柄(无符号指针)
			USER_CALLBACK_HANDLE_ENVENT handleStatusfun - 注册回调函数
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_RegProgressCallBack(void *handle, USER_CALLBACK_HANDLE_PROCESS handleStatusfun, void* _Object);

/*********************************************************
* 编    号: No007
* 函 数 名: HBI_IsConnect
* 功能描述: 判断平板是为连接状态
* 参数说明:
		In: void *handle - 句柄(无符号指针)
* 返 回 值：int
		1 - 连接
		0 - 断开
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_IsConnect(void *handle);

/*********************************************************
* 编    号: No008
* 函 数 名: HBI_GetSDKVerion
* 功能描述: 获取SDK版本号
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: char *szVer
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSDKVerion(void *handle, char *szVer);

/*********************************************************
* 编    号: No009
* 函 数 名: HBI_GetFirmareVerion
* 功能描述: 获取固件版本号
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: char *szVer
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetFirmareVerion(void *handle, char *szFirmwareVer);

/*********************************************************
* 编    号: No010
* 函 数 名: HBI_Prepare
* 功能描述: 软触发，清空准备指令
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_Prepare(void *handle);

/*********************************************************
* 编    号: No011
* 函 数 名: HBI_SingleAcquisition
* 功能描述: 单帧采集
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		FPD_AQC_MODE _mode - 采集模式以及参数
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SingleAcquisition(void *handle, FPD_AQC_MODE _mode);

/*********************************************************
* 编    号: No012
* 函 数 名: HBI_LiveAcquisition
* 功能描述: 连续采集
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		FPD_AQC_MODE _mode - 采集模式以及参数
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_LiveAcquisition(void *handle, FPD_AQC_MODE _mode);

/*********************************************************
* 编    号: No013
* 函 数 名: HBI_StopAcquisition
* 功能描述: 停止连续采集
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备     注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_StopAcquisition(void *handle);

/*********************************************************
* 编    号: No014
* 函 数 名: HBI_ForceStopAcquisition
* 功能描述: 强制停止采集
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ForceStopAcquisition(void *handle);

/*********************************************************
* 编    号: No015
* 函 数 名: HBI_ResetDetector
* 功能描述: 重置探测器
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ResetDetector(void *handle);

/*********************************************************
* 编    号: No016
* 函 数 名: HBI_ResetFirmware
* 功能描述: 重置固件出厂设置
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ResetFirmware(void *handle);

/*********************************************************
* 编    号: No017
* 函 数 名: HBI_Dumping
* 功能描述: 清空指令
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_Dumping(void *handle);

/*********************************************************
* 编    号: No018
* 函 数 名: HBI_RetransMissPacket
* 功能描述: 单包丢包重传
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_RetransMissPacket(void *handle);

/*********************************************************
* 编    号: No019
* 函 数 名: HBI_RetransMissFrame
* 功能描述: 整帧重传
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_RetransMissFrame(void *handle);

/*********************************************************
* 编    号: No020
* 函 数 名: HBI_GetLastestConfig
* 功能描述: 回读平板ROM参数,异步事件，在参数在回调函数中反馈
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetLastestConfig(void *handle);

/*********************************************************
* 编    号: No021
* 函 数 名: HBI_GetFirmwareConfig
* 功能描述: 获取当前SDK中保存的最新ROM参数，同步事件
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out:RegCfgInfo* pRegCfg,见OEMFPDSDK_TYPE.H。
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:Latest configuration
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetFirmwareConfig(void *handle, RegCfgInfo* pRegCfg);

/*********************************************************
* 编    号: No022
* 函 数 名: HBI_SwitchSDKMode
* 功能描述: 切换SDK运行模式，暂不支持
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		int mode - 切换不同的运行模式，例如调试模式、生产模式以及升级模式和下载模式等
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SwitchSDKMode(void *handle, int mode);

/*********************************************************
* 编    号: No023
* 函 数 名: HBI_SetRawStyle
* 功能描述: 设置是否保存图像以及图像文件形式
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			bool bsave - 保存拟或显示,false:显示不保存，true:保存不显示
			bool bsingleraw - 报存在单个文件或多个文件,false:1帧数据保存1个文件，true:多帧数据可保存在一个文件中
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetRawStyle(void *handle, bool bsave, bool bsingleraw = false);

/*********************************************************
* 编    号: No024
* 函 数 名: HBI_SetAqcProperty
* 功能描述: 设置采集属性
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		    FPD_AQC_MODE _mode - 采集模式以及参数
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetAqcProperty(void *handle, FPD_AQC_MODE _mode);

/*********************************************************
* 编    号: No025
* 函 数 名: HBI_GetError
* 功能描述: 获取错误信息
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			CodeStringTable* inTable - 错误信息信息列表
			int count - 信息列表个数
			int recode - 错误码
		Out:无
* 返 回 值：const char *
		非NULL - 成功，错误信息
		NULL   - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL const char * __stdcall HBI_GetError(void *handle, CodeStringTable* inTable, int count, int recode);

/*********************************************************
* 编    号: No026
* 函 数 名: HBI_SetAcqSpanTm
* 功能描述: 设置采集时间间隔
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			int time - 间隔时间,单位是毫秒ms，>= 1000ms
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetAcqSpanTm(void *handle, int time);

/*********************************************************
* 编    号: No027
* 函 数 名: HBI_GetAcqSpanTm
* 功能描述: 获取采集时间间隔
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		out:int *out_time - 时间,单位是毫秒ms
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetAcqSpanTm(void *handle, int *out_time);

/*********************************************************
* 编    号: No028
* 函 数 名: HBI_SetAcqSpanTm
* 功能描述: 设置AED积分时间
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			int time - 间隔时间,单位是毫秒ms，>= 500ms
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetAedIntegrateTm(void *handle, int time);

/*********************************************************
* 编    号: No029
* 函 数 名: HBI_GetAedIntegrateTm
* 功能描述: 获取AED积分时间
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		out:int *out_time - 时间,单位是毫秒ms
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetAedIntegrateTm(void *handle, int *out_time);

/*********************************************************
* 编    号: No030
* 函 数 名: HBI_SetGainMode
* 功能描述: 设置增益模式,即设置可编程积分电容档位
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			int mode - 模式
			[n]-Invalid
			[1]-0.6pC
			[2]-1.2pC
			[3]-2.4pC
			[4]-3.6pC
			[5]-4.8pC
			[6]-7.2pC,默认7.2pC
			[7]-9.6pC	
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetGainMode(void *handle, int nGainMode);

/*********************************************************
* 编    号: No031
* 函 数 名: HBI_GetGainMode
* 功能描述: 获取增益模式,即获取可编程积分电容档位
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
        [n]-Invalid
		[1]-0.6pC
		[2]-1.2pC
		[3]-2.4pC
		[4]-3.6pC
		[5]-4.8pC
		[6]-7.2pC
		[7]-9.6pC
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetGainMode(void *handle);

/*********************************************************
* 编    号: No032
* 函 数 名: HBI_TriggerAndCorrectApplay
* 功能描述: 设置触发模式和图像校正使能（工作站）新版本
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			int _triggerMode,1-软触发，3-高压触发，4-FreeAED。
			IMAGE_CORRECT_ENABLE* pCorrect,见HbDllType.h。
		Out:无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:替换HBI_WorkStationInitDllCfg和HBI_QuckInitDllCfg接口
*********************************************************/
HB_PDF_DLL int __stdcall HBI_TriggerAndCorrectApplay(void *handle, int _triggerMode, IMAGE_CORRECT_ENABLE* pCorrect);

/*********************************************************
* 编    号: No033
* 函 数 名: HBI_UpdateTriggerMode
* 功能描述: 设置触发模式
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			int _triggerMode,1-软触发，3-高压触发，4-FreeAED。
		Out:无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_UpdateTriggerMode(void *handle, int _triggerMode);

/*********************************************************
* 编    号: No034
* 函 数 名: HBI_UpdateCorrectEnable
* 功能描述: 更新图像固件校正使能
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			IMAGE_CORRECT_ENABLE* pCorrect,见HbDllType.h。
		Out:无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_UpdateCorrectEnable(void *handle, IMAGE_CORRECT_ENABLE* pCorrect);

/*********************************************************
* 编    号: No035
* 函 数 名: HBI_GetCorrectEnable
* 功能描述: 获取图像固件校正使能
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			IMAGE_CORRECT_ENABLE* pCorrect,见HbDllType.h。
		Out:无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetCorrectEnable(void *handle, IMAGE_CORRECT_ENABLE *pCorrect);

/*********************************************************
* 编    号: No036
* 函 数 名: HBI_GetImageProperty
* 功能描述: 获取图像属性
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: IMAGE_PROPERTY *img_pro,见HbDllType.h。
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetImageProperty(void *handle, IMAGE_PROPERTY *img_pro);

/*********************************************************
* 编    号: No037
* 函 数 名: HBI_WorkStationInitDllCfg
* 功能描述: 初始化参数（工作站）旧版本
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	     SysCfgInfo* pSysCfg,见HbDllType.h。
	     ImgCaliCfg* pFirmwareCaliCfg,见HbDllType.h。
		 SOFTWARE_CALIBRATE_ENABLE* pSoftwareCaliCfg,见HbDllType.h。
	Out:无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:因部分老用户仍在使用,暂且保留,建议用户使用HBI_QuckInitDllCfg替换。
*********************************************************/
HB_PDF_DLL int __stdcall HBI_WorkStationInitDllCfg(void *handle, SysCfgInfo* pSysCfg, ImgCaliCfg* pFirmwareCaliCfg, SOFTWARE_CALIBRATE_ENABLE* pSoftwareCaliCfg);

/*********************************************************
* 编    号: No038
* 函 数 名: HBI_QuckInitDllCfg
* 功能描述: 快速初始化参数（工作站）旧版本
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			int _triggerMode,1-软触发，3-高压触发，4-FreeAED。
			ImgCaliCfg* pFirmwareCaliCfg,见HbDllType.h。
			SOFTWARE_CALIBRATE_ENABLE* pSoftwareCaliCfg,见HbDllType.h。
		Out:无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:替换HBI_WorkStationInitDllCfg接口
*********************************************************/
HB_PDF_DLL int __stdcall HBI_QuckInitDllCfg(void *handle, int _triggerMode, ImgCaliCfg* pFirmwareCaliCfg, SOFTWARE_CALIBRATE_ENABLE* pSoftwareCaliCfg);

/*********************************************************
* 编    号: No039
* 函 数 名: HBI_GetDevCfgInfo
* 功能描述: 获取固件ROM参数
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	Out:RegCfgInfo* pRegCfg,见HbDllType.h。
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetDevCfgInfo(void *handle, RegCfgInfo* pRegCfg);

/*********************************************************
* 编    号: No040
* 函 数 名: HBI_SetEnableCalibrate
* 功能描述: 设置软件校正使能
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			SOFTWARE_CALIBRATE_ENABLE inEnable - 校正使能状态,见HbDllType.h中SOFTWARE_CALIBRATE_ENABLE struct
		Out: 无
* 返 回 值：int
		0   成功
		非0 失败，见HbDllError.h错误码
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetEnableCalibrate(void *handle, SOFTWARE_CALIBRATE_ENABLE inEnable);

/*********************************************************
* 编    号: No041
* 函 数 名: HBI_GetEnableCalibrate
* 功能描述: 获取软件校正使能参数
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			SOFTWARE_CALIBRATE_ENABLE inEnable - 校正使能状态,见HbDllType.h中
			SOFTWARE_CALIBRATE_ENABLE struct
		Out: 无
* 返 回 值：int
		0   成功
		非0 失败，见HbDllError.h错误码
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetEnableCalibrate(void *handle, SOFTWARE_CALIBRATE_ENABLE *inEnable);

/*********************************************************
* 编    号: No040
* 函 数 名: HBI_SetSoftwareCorrect
* 功能描述: 设置校正使能，替换HBI_SetEnableCalibrate
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			SOFTWARE_CALIBRATE_ENABLE inEnable - 校正使能状态,见HbDllType.h中SOFTWARE_CALIBRATE_ENABLE struct
		Out: 无
* 返 回 值：int
		0   成功
		非0 失败，见HbDllError.h错误码
* 备    注:新接口
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSoftwareCorrect(void *handle, SOFTWARE_CALIBRATE_ENABLE inEnable);

/*********************************************************
* 编    号: No042
* 函 数 名: HBI_GetSoftwareCorrect
* 功能描述: 获取校正使能参数，替换HBI_GetEnableCalibrate
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			SOFTWARE_CALIBRATE_ENABLE inEnable - 校正使能状态,见HbDllType.h中
			SOFTWARE_CALIBRATE_ENABLE struct
		Out: 无
* 返 回 值：int
		0   成功
		非0 失败，见HbDllError.h错误码
* 备    注:新接口
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSoftwareCorrect(void *handle, SOFTWARE_CALIBRATE_ENABLE *inEnable);

/*********************************************************
* 编    号: No042
* 函 数 名: HBI_SetSysParamCfg
* 功能描述: 配置系统RAM/ROM/FACTORY参数
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	int cmd -
	int type -
	RegCfgInfo* pRegCfg -
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSysParamCfg(void *handle, int cmd, int type, RegCfgInfo* pRegCfg);

/*********************************************************
* 编    号: No043
* 函 数 名: HBI_GetSysParamCfg
* 功能描述: 回读RAM/ROM/FACTORY参数,异步事件，在参数在回调函数中反馈
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	int cmd  - rom or ram
	int type - user or admin
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSysParamCfg(void *handle, int cmd, int type);

/*********************************************************
* 编    号: No044
* 函 数 名: HBI_SetSystemManufactureInfo
* 功能描述: Set System Manufacture Information
* 参数说明:
	In: void *handle - 句柄(无符号指针)
		SysManuInfo* pSysManuInfo,见HbDllType.h。
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSystemManufactureInfo(void *handle, SysManuInfo* pSysManuInfo);

/*********************************************************
* 编    号: No045
* 函 数 名: HBI_SetSystemStatus
* 功能描述: Set System Status Information
* 参数说明:
	In: void *handle - 句柄(无符号指针)
		SysStatusInfo* pSysStatus,见HbDllType.h。
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSystemStatus(void *handle, SysStatusInfo* pSysStatus);

/*********************************************************
* 编    号: No046
* 函 数 名: HBI_GetSystemManufactureInfo
* 功能描述: Get System Manufacture Information
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	Out:SysManuInfo* pSysManuInfo,见HbDllType.h。
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSystemManufactureInfo(void *handle, SysManuInfo* pSysManuInfo);

/*********************************************************
* 编    号: No047
* 函 数 名: HBI_GetSystemStatus
* 功能描述: Get System Status Information
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	Out:SysStatusInfo* pSysStatus,见HbDllType.h。
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSystemStatus(void *handle, SysStatusInfo* pSysStatus);

/*********************************************************
* 编    号: No048
* 函 数 名: HBI_SetGigabitEther
* 功能描述: 设置网络信息参数
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	    EtherInfo* pEther,见HbDllType.h。
	Out:无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetGigabitEther(void *handle, EtherInfo* pEther);

/*********************************************************
* 编    号: No049
* 函 数 名: HBI_SetHvgTriggerMode
* 功能描述: Sety High Voltage Generator Interface Trigger Mode Information
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	    HiVolTriggerModeInfo* pHvgTrigMode,见HbDllType.h。
	Out:无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetHvgTriggerMode(void *handle, HiVolTriggerModeInfo* pHvgTrigMode);

/*********************************************************
* 编    号: No050
* 函 数 名: HBI_SetSystemConfig
* 功能描述: Set System Configuration Information
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	    SysCfgInfo* pSysCfg,见HbDllType.h。
	Out:无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSystemConfig(void *handle, SysCfgInfo* pSysCfg);

/*********************************************************
* 编    号: No051
* 函 数 名: HBI_SetImageCalibration
* 功能描述: 设置ROM Image firmware calibrate使能
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	    ImgCaliCfg* pImgCaliCfg,见HbDllType.h。
	Out:无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetImageCalibration(void *handle, ImgCaliCfg* pImgCaliCfg);

/*********************************************************
* 编    号: No052
* 函 数 名: HBI_SetVoltageAdjustConfig
* 功能描述: Set Voltage Adjust Configuration.
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	    VolAdjustCfg* pVolAdjustCfg,见HbDllType.h。
	Out:无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetVoltageAdjustConfig(void *handle, VolAdjustCfg* pVolAdjustCfg);

/*********************************************************
* 编    号: No053
* 函 数 名: HBI_SetTICOFConfig
* 功能描述: Set TI COF Parameter Configuration.
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	    TICOFCfg* pTICOFCfg,见HbDllType.h。
	Out:无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetTICOFConfig(void *handle, TICOFCfg* pTICOFCfg);

/*********************************************************
* 编    号: No054
* 函 数 名: HBI_SetCMOSConfig
* 功能描述: Set CMOS Parameter Configuration.
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	    CMOSCfg* pCMOSCfg,见HbDllType.h。
	Out:无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetCMOSConfig(void *handle, CMOSCfg* pCMOSCfg);

/*********************************************************
* 编    号: No055
* 函 数 名: HBI_SetADICOFConfig
* 功能描述: Set ADI COF Parameter Configuration.
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	    ADICOFCfg* pADICOFCfg,见HbDllType.h。
	Out:无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetADICOFConfig(void *handle, ADICOFCfg* pADICOFCfg);

/*********************************************************
* 编    号: No056
* 函 数 名: HBI_InitOffsetMode
* 功能描述: 初始化暗场模板
* 参数说明:
	In: void *handle - 句柄(无符号指针)
		CALIBRATE_INPUT_PARAM calibrate_param,见HbDllType.h。
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InitOffsetMode(void *handle, CALIBRATE_INPUT_PARAM calibrate_param);

/*********************************************************
* 编    号: No057
* 函 数 名: HBI_InsertOffsetMode
* 功能描述: 向offset矫正模型中插入数据
* 参数说明:
	In: void *handle - 句柄(无符号指针)
		int group_id - 组ID
		char *filepath - 文件路径
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InsertOffsetMode(void *handle, int group_id, char *filepath);

/*********************************************************
* 编    号: No058
* 函 数 名: HBI_ClearOffsetMode
* 功能描述: 清空offset矫正模型
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ClearOffsetMode(void *handle);

/*********************************************************
* 编    号: No059
* 函 数 名: HBI_GenerateOffsetTemp
* 功能描述: 生成offset模板
* 参数说明:
	In: void *handle - 句柄(无符号指针)
		int raw_num - 暗场图个数
		int bprevew - 是否生成preview模板，1-生成，0-不生成
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GenerateOffsetTemp(void *handle, int raw_num, int bprevew = 0);

/*********************************************************
* 编    号: No060
* 函 数 名: HBI_InitGainMode
* 功能描述: 初始化gain矫正模型
* 参数说明:
	In: void *handle - 句柄(无符号指针)
		CALIBRATE_INPUT_PARAM calibrate_param - 矫正参数
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InitGainMode(void *handle, CALIBRATE_INPUT_PARAM calibrate_param);

/*********************************************************
* 编    号: No061
* 函 数 名: HBI_InsertGainMode
* 功能描述: 向gain矫正模型中插入数据
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	int group_id - 组ID
	char *filepath - 文件路径
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InsertGainMode(void *handle, int group_id, char *filepath);

/*********************************************************
* 编    号: No062
* 函 数 名: HBI_ClearGainMode
* 功能描述: 清空gain矫正模型
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ClearGainMode(void *handle);

/*********************************************************
* 编    号: No063
* 函 数 名: HBI_GenerateGainTemp
* 功能描述: 生成gain模板
* 参数说明:
	In: void *handle - 句柄(无符号指针)
		int group_sum - 组数
		int per_group_num - 每组包含图片个数
		int bprevew       - 是否生成preview模板，1-生成，0-不生成
		Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GenerateGainTemp(void *handle, int group_sum, int per_group_num, int bprevew = 0);

/*********************************************************
* 编    号: No064
* 函 数 名: HBI_InitDefectMode
* 功能描述: 初始化defect矫正模型
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	    CALIBRATE_INPUT_PARAM calibrate_param - 矫正参数
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InitDefectMode(void *handle, CALIBRATE_INPUT_PARAM calibrate_param);

/*********************************************************
* 编    号: No065
* 函 数 名: HBI_InsertDefectMode
* 功能描述: 向defect矫正模型中插入数据
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	    int group_id - 组ID
		char *filepath - 文件路径
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InsertDefectMode(void *handle, int group_id, char *filepath);

/*********************************************************
* 编    号: No066
* 函 数 名: HBI_ClearDefectMode
* 功能描述: 清空defect矫正模型
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ClearDefectMode(void *handle);

/*********************************************************
* 编    号: No067
* 函 数 名: HBI_GenerateDefectTemp
* 功能描述: 生成defect模板
* 参数说明:
	In: void *handle  - 句柄(无符号指针)
	int group_sum     - 组数
	int per_group_num - 每组包含图片个数
	int bprevew       - 是否生成preview模板，1-生成，0-不生成
	Out: 无
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GenerateDefectTemp(void *handle, int group_sum, int per_group_num, int bprevew = 0);

/*********************************************************
* 编    号: No068
* 函 数 名: HBI_UpgradeFirmware
* 功能描述: 升级固件信息
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			char *binfile - bin文件路径
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_UpgradeFirmware(void *handle, char *binfile);

/*********************************************************
* 编    号: No069
* 函 数 名: HBI_StopUpdateFirmware
* 功能描述: 停止更新固件
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		out:无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_StopUpdateFirmware(void *handle);

/*********************************************************
* 编    号: No070
* 函 数 名: HBI_UploadTemplate
* 功能描述: 上传矫正模板
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			UPLOAD_MODE *uploadmode - 上传模型对象指针
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_UploadTemplate(void *handle, UPLOAD_MODE *uploadmode);

/*********************************************************
* 编    号: No071
* 函 数 名: HBI_StopUploadTemplate
* 功能描述: 停止上传矫正模板
* 参数说明:
		In:  void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_StopUploadTemplate(void *handle);

/*********************************************************
* 编    号: No072
* 函 数 名: HBI_GetWiffyProperty
* 功能描述: 获取无线属性
* 参数说明:
	In: void *handle - 句柄(无符号指针)
	Out: void *pWiffy - 暂不支持
* 返 回 值：int
	0   - 成功
	非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetWiffyProperty(void *handle, void *pWiffy);

/*********************************************************
* 编    号: No073
* 函 数 名: HBI_SetAedThreshold
* 功能描述: 设置AED阈值
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		int out_ivalue - 阈值 [25,000~1,000,000]
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetAedThreshold(void *handle, int in_ivalue);

/*********************************************************
* 编    号: No074
* 函 数 名: HBI_GetAedThreshold
* 功能描述: 获取AED阈值
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		out:int *out_ivalue - 阈值 [10,000~1,000,000]
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetAedThreshold(void *handle, int *out_ivalue);

/*********************************************************
* 编    号: No075
* 函 数 名: HBI_SetSaturationValue
* 功能描述: 设置饱和值
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		out:int *out_ivalue - 饱和值 [0~65,535]
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSaturationValue(void *handle, int in_ivalue);

/*********************************************************
* 编    号: No076
* 函 数 名: HBI_GetSaturationValue
* 功能描述: 获取饱和值
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		    int *out_ivalue - 饱和值 [0~65,535]
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSaturationValue(void *handle, int *out_ivalue);

/*********************************************************
* 编    号: No077
* 函 数 名: HBI_SetClippingValue
* 功能描述: 设置图像剪裁值
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		out:int *out_ivalue - 剪裁值 [0~65,535]
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetClippingValue(void *handle, int in_ivalue);

/*********************************************************
* 编    号: No078
* 函 数 名: HBI_GetClippingValue
* 功能描述: 获取图像剪裁值
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		out:int *out_ivalue - 剪裁值 [0~65,535]
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetClippingValue(void *handle, int *out_ivalue);

/*********************************************************
* 编    号: No079
* 函 数 名: HBI_SetPreAcqTm
* 功能描述: 设置软触发单帧采集清空和采集之间的时间间隔
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		    int *in_itime - 时间 [0~10000] 单位:mm
				0-表示软触发单帧采集先HBI_Prepare后HBI_SingleAcquisition完成单帧采集
				非0-表示软触发单帧采集，只需HBI_Prepare即可按照预定时间完成单帧采集
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetPreAcqTm(void *handle, int in_itime);

/*********************************************************
* 编    号: No080
* 函 数 名: HBI_GetPreAcqTm
* 功能描述: 获取软触发单帧采集清空和采集之间的时间间隔
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		out:int *out_itime - 时间 [0~10000] 单位:mm
		        0-表示软触发单帧采集先HBI_Prepare后HBI_SingleAcquisition完成单帧采集
				非0-表示软触发单帧采集，只需HBI_Prepare即可按照预定时间完成单帧采集
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetPreAcqTm(void *handle, int *out_itime);

/*********************************************************
* 编    号: No081
* 函 数 名: HBI_SetSDKWorkMode
* 功能描述: 设置SDK工作模式
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		    mode - 1：debug mode，0：product mode
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSDKWorkMode(void *handle, int mode);

/*********************************************************
* 编    号: No082
* 函 数 名: HBI_GetSDKWorkMode
* 功能描述: 获取SDK工作模式
* 参数说明:
		In: void *handle - 句柄(无符号指针)
* 返 回 值：int
		0    - product mode
		1    - debug mode
		8007 - No Device
		8021 - Error work mode
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSDKWorkMode(void *handle);

/*********************************************************
* 编    号: No083
* 函 数 名: HBI_SendData
* 功能描述: 发送函数
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		    unsigned char *data - 数据缓冲区
			int datalen - 数据缓冲区长度
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SendData(void *handle, unsigned char *data, int datalen);

/*********************************************************
* 编    号: No084
* 函 数 名: HBI_RecvData
* 功能描述: 接收函数
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out unsigned char *data - 数据缓冲区
		int datalen - 数据缓冲区长度
		int time - 超时时间，单位ms
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_RecvData(void *handle, unsigned char *data, int datalen, int time=1000);

/*********************************************************
* 编    号: No085
* 函 数 名: HBI_SetLPFLevel
* 功能描述: 设置低通滤波器档位参数
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			int nLPFLevel - 档位
			[n]-Invalid
			[0]-210khz,默认210khz
			[1]-105khz
			[2]-52khz
			[3]-26khz
			[4]-13khz
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetLPFLevel(void *handle, int nLPFLevel);

/*********************************************************
* 编    号: No086
* 函 数 名: HBI_GetLPFLevel
* 功能描述: 获取低通滤波器档位参数
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		[0]-210khz,默认210khz
		[1]-105khz
		[2]-52khz
		[3]-26khz
		[4]-13khz
        [n]-Invalid
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetLPFLevel(void *handle);

/*********************************************************
* 编    号: No087
* 函 数 名: HBI_ImmediateGenerateTemplate
* 功能描述: 一键生成模板
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		    EnumIMAGE_ACQ_MODE _mode - 生成模板类型
				OFFSET_TEMPLATE_MODE    连续采集一组暗场图 - offset
				GAIN_TEMPLATE_MODE      连续采集一组亮场图 - gain
				DEFECT_ACQ_GROUP1,      连续采集一组亮场图 - defect group1
	            DEFECT_ACQ_GROUP2,      连续采集一组亮场图 - defect group2
	            DEFECT_ACQ_AND_TEMPLATE 连续采集一组亮场图 - defect group3 and generate template
			int bprevew - 是否生成preview模板，1-生成，0-不生成
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ImmediateGenerateTemplate(void *handle, EnumIMAGE_ACQ_MODE _mode, int bprevew = 0);

/*********************************************************
* 编    号: No087
* 函 数 名: HBI_16UCConvertTo8UC
* 功能描述: 16位数据转换为8位
* 参数说明:
		In:     void *handle - 句柄(无符号指针)
		In/Out: unsigned char *imgbuff
		In/Out: int *nbufflen
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_16UCConvertTo8UC(void *handle, unsigned char *imgbuff, int *nbufflen);

/*********************************************************
* 编    号: No088
* 函 数 名: HBI_SetPreviewMode
* 功能描述: 设置Preview预览或取消预览
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			int inTriggMode
				1-软触发
				3-高压触发
				4-FreeAED
				8-SAEC
				其他不支持。
			int inPreviewMode
				0-normal image
				1-preview image
		Out:无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetPreviewMode(void *handle, int inTriggMode, int inPreviewMode);

/*********************************************************
* 编    号: No089
* 函 数 名: HBI_GetPreviewMode
* 功能描述: 设置Preview mode当前值
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		out:int *outMode - preview mode [0~2]
		    0-normal image,1-preview image
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetPreviewMode(void *handle, int *outMode);

/*********************************************************
* 编    号: No090
* 函 数 名: HBI_SAecModeApply
* 功能描述: 设置或取消SAEC模式
* 参数说明:
		In: void *handle - 句柄(无符号指针)
			int inPreviewMode
				0-normal image
				1-preview image
			int inPerTime  - fpd pre ready delay time
			int inPostTime - fpd post ready delay time
		Out:无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:分为两种情况：
           1》启用：inPreviewMode=1(preview image)，inPerTime和inPostTime(客户指定)
		   2》取消：inPreviewMode=0(normal image)，inPerTime和inPostTime默认即可
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SAecModeApply(void *handle, int inPreviewMode, int inPerTime=0, int inPostTime=0);

/*********************************************************
* 编    号: No091
* 函 数 名: HBI_SAecAcq
* 功能描述: SAEC采集上图指令
* 参数说明:
		In: void *handle - 句柄(无符号指针)
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SAecAcq(void *handle);

/*********************************************************
* 编    号: No092
* 函 数 名: HBI_GetFPDSerialNumber
* 功能描述: 获取产品序列号
* 参数说明:
	    In: void *handle - 句柄(无符号指针)
	    In/Out: char *szSn,长度14位
* 返 回 值：int
	    0   - 成功
	    非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetFPDSerialNumber(void *handle, char *szSn);
#ifdef _cplusplus
}
#endif
#endif // __HB_FPD_DLL_H_