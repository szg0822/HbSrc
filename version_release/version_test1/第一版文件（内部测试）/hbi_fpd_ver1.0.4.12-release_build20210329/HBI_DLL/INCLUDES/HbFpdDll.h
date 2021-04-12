/*---------------------------------------------------------------------------
* Copyright (c) 2019 �Ϻ�껲�Ӱ��Ƽ����޹�˾
* All rights reserved.
*
* �ļ�����: HbFpdDll.h
* �ļ���ʶ:
* ժ    Ҫ: ƽ��̽������������ͷ�ļ�
*
* ��ǰ�汾: 1.0
* ��    ��: mhyang
* ��������: 2019/04/18
* �޸�����: 2020/06/29
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
* ��    ��: No001
* �� �� ��: HBI_Init
* ��������: ��ʼ����̬��
* ����˵��:
* �� �� ֵ��void *
		    ʧ�ܣ�NULL,�ɹ����ǿ�
* ��    ע:
*********************************************************/
HB_PDF_DLL void* __stdcall HBI_Init();

/*********************************************************
* ��    ��: No002
* �� �� ��: HBI_Destroy
* ��������: �ͷŶ�̬����Դ
* ����˵��:
        In: void *handle - ���
		Out: ��
* �� �� ֵ��void
* ��    ע:
*********************************************************/
HB_PDF_DLL void __stdcall HBI_Destroy(void *handle);

/*********************************************************
* ��    ��: No003
* �� �� ��: HBI_ConnectDetector
* ��������: ��������
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			char *szRemoteIp - ƽ��IP��ַ,��192.168.10.40
			unsigned short remotePort - ƽ��˿�,��32897(0x8081)
			char *szlocalIp - ��λ����ַ,��192.168.10.20
			unsigned short localPort -��λ���˿�,��192.168.10.40
			int timeout - ��λ�����ճ�ʱ,��20ms
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ConnectDetector(void *handle, char *szRemoteIp, unsigned short remotePort, char *szlocalIp, unsigned short localPort, int timeout=20);

/*********************************************************
* ��    ��: No004
* �� �� ��: HBI_DisConnectDetector
* ��������: �Ͽ�����
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
			0   - �ɹ�
			��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_DisConnectDetector(void *handle);

/*********************************************************
* ��    ��: No005
* �� �� ��: HBI_RegEventCallBackFun
* ��������: ע��ص�����
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		USER_CALLBACK_HANDLE_ENVENT handleEventfun - ע��ص�����
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_RegEventCallBackFun(void *handle, USER_CALLBACK_HANDLE_ENVENT handleEventfun);

/*********************************************************
* ��    ��: No006
* �� �� ��: HBI_RegProgressCallBack
* ��������: ע��ص�����
* ����˵��: ����̼�����������Ϣ
		In: void *handle - ���(�޷���ָ��)
			USER_CALLBACK_HANDLE_ENVENT handleStatusfun - ע��ص�����
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_RegProgressCallBack(void *handle, USER_CALLBACK_HANDLE_PROCESS handleStatusfun, void* _Object);

/*********************************************************
* ��    ��: No007
* �� �� ��: HBI_IsConnect
* ��������: �ж�ƽ����Ϊ����״̬
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
* �� �� ֵ��int
		1 - ����
		0 - �Ͽ�
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_IsConnect(void *handle);

/*********************************************************
* ��    ��: No008
* �� �� ��: HBI_GetSDKVerion
* ��������: ��ȡSDK�汾��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: char *szVer
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSDKVerion(void *handle, char *szVer);

/*********************************************************
* ��    ��: No009
* �� �� ��: HBI_GetFirmareVerion
* ��������: ��ȡ�̼��汾��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: char *szVer
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetFirmareVerion(void *handle, char *szFirmwareVer);

/*********************************************************
* ��    ��: No010
* �� �� ��: HBI_Prepare
* ��������: ���������׼��ָ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_Prepare(void *handle);

/*********************************************************
* ��    ��: No011
* �� �� ��: HBI_SingleAcquisition
* ��������: ��֡�ɼ�
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		FPD_AQC_MODE _mode - �ɼ�ģʽ�Լ�����
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SingleAcquisition(void *handle, FPD_AQC_MODE _mode);

/*********************************************************
* ��    ��: No012
* �� �� ��: HBI_LiveAcquisition
* ��������: �����ɼ�
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		FPD_AQC_MODE _mode - �ɼ�ģʽ�Լ�����
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_LiveAcquisition(void *handle, FPD_AQC_MODE _mode);

/*********************************************************
* ��    ��: No013
* �� �� ��: HBI_StopAcquisition
* ��������: ֹͣ�����ɼ�
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��     ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_StopAcquisition(void *handle);

/*********************************************************
* ��    ��: No014
* �� �� ��: HBI_ForceStopAcquisition
* ��������: ǿ��ֹͣ�ɼ�
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ForceStopAcquisition(void *handle);

/*********************************************************
* ��    ��: No015
* �� �� ��: HBI_ResetDetector
* ��������: ����̽����
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ResetDetector(void *handle);

/*********************************************************
* ��    ��: No016
* �� �� ��: HBI_ResetFirmware
* ��������: ���ù̼���������
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ResetFirmware(void *handle);

/*********************************************************
* ��    ��: No017
* �� �� ��: HBI_Dumping
* ��������: ���ָ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_Dumping(void *handle);

/*********************************************************
* ��    ��: No018
* �� �� ��: HBI_RetransMissPacket
* ��������: ���������ش�
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_RetransMissPacket(void *handle);

/*********************************************************
* ��    ��: No019
* �� �� ��: HBI_RetransMissFrame
* ��������: ��֡�ش�
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_RetransMissFrame(void *handle);

/*********************************************************
* ��    ��: No020
* �� �� ��: HBI_GetLastestConfig
* ��������: �ض�ƽ��ROM����,�첽�¼����ڲ����ڻص������з���
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetLastestConfig(void *handle);

/*********************************************************
* ��    ��: No021
* �� �� ��: HBI_GetFirmwareConfig
* ��������: ��ȡ��ǰSDK�б��������ROM������ͬ���¼�
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out:RegCfgInfo* pRegCfg,��OEMFPDSDK_TYPE.H��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:Latest configuration
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetFirmwareConfig(void *handle, RegCfgInfo* pRegCfg);

/*********************************************************
* ��    ��: No022
* �� �� ��: HBI_SwitchSDKMode
* ��������: �л�SDK����ģʽ���ݲ�֧��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		int mode - �л���ͬ������ģʽ���������ģʽ������ģʽ�Լ�����ģʽ������ģʽ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SwitchSDKMode(void *handle, int mode);

/*********************************************************
* ��    ��: No023
* �� �� ��: HBI_SetRawStyle
* ��������: �����Ƿ񱣴�ͼ���Լ�ͼ���ļ���ʽ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			bool bsave - ���������ʾ,false:��ʾ�����棬true:���治��ʾ
			bool bsingleraw - �����ڵ����ļ������ļ�,false:1֡���ݱ���1���ļ���true:��֡���ݿɱ�����һ���ļ���
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetRawStyle(void *handle, bool bsave, bool bsingleraw = false);

/*********************************************************
* ��    ��: No024
* �� �� ��: HBI_SetAqcProperty
* ��������: ���òɼ�����
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		    FPD_AQC_MODE _mode - �ɼ�ģʽ�Լ�����
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetAqcProperty(void *handle, FPD_AQC_MODE _mode);

/*********************************************************
* ��    ��: No025
* �� �� ��: HBI_GetError
* ��������: ��ȡ������Ϣ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			CodeStringTable* inTable - ������Ϣ��Ϣ�б�
			int count - ��Ϣ�б����
			int recode - ������
		Out:��
* �� �� ֵ��const char *
		��NULL - �ɹ���������Ϣ
		NULL   - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL const char * __stdcall HBI_GetError(void *handle, CodeStringTable* inTable, int count, int recode);

/*********************************************************
* ��    ��: No026
* �� �� ��: HBI_SetAcqSpanTm
* ��������: ���òɼ�ʱ����
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			int time - ���ʱ��,��λ�Ǻ���ms��>= 1000ms
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetAcqSpanTm(void *handle, int time);

/*********************************************************
* ��    ��: No027
* �� �� ��: HBI_GetAcqSpanTm
* ��������: ��ȡ�ɼ�ʱ����
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		out:int *out_time - ʱ��,��λ�Ǻ���ms
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetAcqSpanTm(void *handle, int *out_time);

/*********************************************************
* ��    ��: No028
* �� �� ��: HBI_SetAcqSpanTm
* ��������: ����AED����ʱ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			int time - ���ʱ��,��λ�Ǻ���ms��>= 500ms
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetAedIntegrateTm(void *handle, int time);

/*********************************************************
* ��    ��: No029
* �� �� ��: HBI_GetAedIntegrateTm
* ��������: ��ȡAED����ʱ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		out:int *out_time - ʱ��,��λ�Ǻ���ms
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetAedIntegrateTm(void *handle, int *out_time);

/*********************************************************
* ��    ��: No030
* �� �� ��: HBI_SetGainMode
* ��������: ��������ģʽ,�����ÿɱ�̻��ֵ��ݵ�λ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			int mode - ģʽ
			[n]-Invalid
			[1]-0.6pC
			[2]-1.2pC
			[3]-2.4pC
			[4]-3.6pC
			[5]-4.8pC
			[6]-7.2pC,Ĭ��7.2pC
			[7]-9.6pC	
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetGainMode(void *handle, int nGainMode);

/*********************************************************
* ��    ��: No031
* �� �� ��: HBI_GetGainMode
* ��������: ��ȡ����ģʽ,����ȡ�ɱ�̻��ֵ��ݵ�λ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
        [n]-Invalid
		[1]-0.6pC
		[2]-1.2pC
		[3]-2.4pC
		[4]-3.6pC
		[5]-4.8pC
		[6]-7.2pC
		[7]-9.6pC
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetGainMode(void *handle);

/*********************************************************
* ��    ��: No032
* �� �� ��: HBI_TriggerAndCorrectApplay
* ��������: ���ô���ģʽ��ͼ��У��ʹ�ܣ�����վ���°汾
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			int _triggerMode,1-������3-��ѹ������4-FreeAED��
			IMAGE_CORRECT_ENABLE* pCorrect,��HbDllType.h��
		Out:��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:�滻HBI_WorkStationInitDllCfg��HBI_QuckInitDllCfg�ӿ�
*********************************************************/
HB_PDF_DLL int __stdcall HBI_TriggerAndCorrectApplay(void *handle, int _triggerMode, IMAGE_CORRECT_ENABLE* pCorrect);

/*********************************************************
* ��    ��: No033
* �� �� ��: HBI_UpdateTriggerMode
* ��������: ���ô���ģʽ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			int _triggerMode,1-������3-��ѹ������4-FreeAED��
		Out:��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_UpdateTriggerMode(void *handle, int _triggerMode);

/*********************************************************
* ��    ��: No034
* �� �� ��: HBI_UpdateCorrectEnable
* ��������: ����ͼ��̼�У��ʹ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			IMAGE_CORRECT_ENABLE* pCorrect,��HbDllType.h��
		Out:��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_UpdateCorrectEnable(void *handle, IMAGE_CORRECT_ENABLE* pCorrect);

/*********************************************************
* ��    ��: No035
* �� �� ��: HBI_GetCorrectEnable
* ��������: ��ȡͼ��̼�У��ʹ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			IMAGE_CORRECT_ENABLE* pCorrect,��HbDllType.h��
		Out:��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetCorrectEnable(void *handle, IMAGE_CORRECT_ENABLE *pCorrect);

/*********************************************************
* ��    ��: No036
* �� �� ��: HBI_GetImageProperty
* ��������: ��ȡͼ������
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: IMAGE_PROPERTY *img_pro,��HbDllType.h��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetImageProperty(void *handle, IMAGE_PROPERTY *img_pro);

/*********************************************************
* ��    ��: No037
* �� �� ��: HBI_WorkStationInitDllCfg
* ��������: ��ʼ������������վ���ɰ汾
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	     SysCfgInfo* pSysCfg,��HbDllType.h��
	     ImgCaliCfg* pFirmwareCaliCfg,��HbDllType.h��
		 SOFTWARE_CALIBRATE_ENABLE* pSoftwareCaliCfg,��HbDllType.h��
	Out:��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:�򲿷����û�����ʹ��,���ұ���,�����û�ʹ��HBI_QuckInitDllCfg�滻��
*********************************************************/
HB_PDF_DLL int __stdcall HBI_WorkStationInitDllCfg(void *handle, SysCfgInfo* pSysCfg, ImgCaliCfg* pFirmwareCaliCfg, SOFTWARE_CALIBRATE_ENABLE* pSoftwareCaliCfg);

/*********************************************************
* ��    ��: No038
* �� �� ��: HBI_QuckInitDllCfg
* ��������: ���ٳ�ʼ������������վ���ɰ汾
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			int _triggerMode,1-������3-��ѹ������4-FreeAED��
			ImgCaliCfg* pFirmwareCaliCfg,��HbDllType.h��
			SOFTWARE_CALIBRATE_ENABLE* pSoftwareCaliCfg,��HbDllType.h��
		Out:��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:�滻HBI_WorkStationInitDllCfg�ӿ�
*********************************************************/
HB_PDF_DLL int __stdcall HBI_QuckInitDllCfg(void *handle, int _triggerMode, ImgCaliCfg* pFirmwareCaliCfg, SOFTWARE_CALIBRATE_ENABLE* pSoftwareCaliCfg);

/*********************************************************
* ��    ��: No039
* �� �� ��: HBI_GetDevCfgInfo
* ��������: ��ȡ�̼�ROM����
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	Out:RegCfgInfo* pRegCfg,��HbDllType.h��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetDevCfgInfo(void *handle, RegCfgInfo* pRegCfg);

/*********************************************************
* ��    ��: No040
* �� �� ��: HBI_SetEnableCalibrate
* ��������: �������У��ʹ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			SOFTWARE_CALIBRATE_ENABLE inEnable - У��ʹ��״̬,��HbDllType.h��SOFTWARE_CALIBRATE_ENABLE struct
		Out: ��
* �� �� ֵ��int
		0   �ɹ�
		��0 ʧ�ܣ���HbDllError.h������
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetEnableCalibrate(void *handle, SOFTWARE_CALIBRATE_ENABLE inEnable);

/*********************************************************
* ��    ��: No041
* �� �� ��: HBI_GetEnableCalibrate
* ��������: ��ȡ���У��ʹ�ܲ���
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			SOFTWARE_CALIBRATE_ENABLE inEnable - У��ʹ��״̬,��HbDllType.h��
			SOFTWARE_CALIBRATE_ENABLE struct
		Out: ��
* �� �� ֵ��int
		0   �ɹ�
		��0 ʧ�ܣ���HbDllError.h������
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetEnableCalibrate(void *handle, SOFTWARE_CALIBRATE_ENABLE *inEnable);

/*********************************************************
* ��    ��: No040
* �� �� ��: HBI_SetSoftwareCorrect
* ��������: ����У��ʹ�ܣ��滻HBI_SetEnableCalibrate
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			SOFTWARE_CALIBRATE_ENABLE inEnable - У��ʹ��״̬,��HbDllType.h��SOFTWARE_CALIBRATE_ENABLE struct
		Out: ��
* �� �� ֵ��int
		0   �ɹ�
		��0 ʧ�ܣ���HbDllError.h������
* ��    ע:�½ӿ�
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSoftwareCorrect(void *handle, SOFTWARE_CALIBRATE_ENABLE inEnable);

/*********************************************************
* ��    ��: No042
* �� �� ��: HBI_GetSoftwareCorrect
* ��������: ��ȡУ��ʹ�ܲ������滻HBI_GetEnableCalibrate
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			SOFTWARE_CALIBRATE_ENABLE inEnable - У��ʹ��״̬,��HbDllType.h��
			SOFTWARE_CALIBRATE_ENABLE struct
		Out: ��
* �� �� ֵ��int
		0   �ɹ�
		��0 ʧ�ܣ���HbDllError.h������
* ��    ע:�½ӿ�
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSoftwareCorrect(void *handle, SOFTWARE_CALIBRATE_ENABLE *inEnable);

/*********************************************************
* ��    ��: No042
* �� �� ��: HBI_SetSysParamCfg
* ��������: ����ϵͳRAM/ROM/FACTORY����
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	int cmd -
	int type -
	RegCfgInfo* pRegCfg -
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSysParamCfg(void *handle, int cmd, int type, RegCfgInfo* pRegCfg);

/*********************************************************
* ��    ��: No043
* �� �� ��: HBI_GetSysParamCfg
* ��������: �ض�RAM/ROM/FACTORY����,�첽�¼����ڲ����ڻص������з���
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	int cmd  - rom or ram
	int type - user or admin
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSysParamCfg(void *handle, int cmd, int type);

/*********************************************************
* ��    ��: No044
* �� �� ��: HBI_SetSystemManufactureInfo
* ��������: Set System Manufacture Information
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
		SysManuInfo* pSysManuInfo,��HbDllType.h��
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSystemManufactureInfo(void *handle, SysManuInfo* pSysManuInfo);

/*********************************************************
* ��    ��: No045
* �� �� ��: HBI_SetSystemStatus
* ��������: Set System Status Information
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
		SysStatusInfo* pSysStatus,��HbDllType.h��
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSystemStatus(void *handle, SysStatusInfo* pSysStatus);

/*********************************************************
* ��    ��: No046
* �� �� ��: HBI_GetSystemManufactureInfo
* ��������: Get System Manufacture Information
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	Out:SysManuInfo* pSysManuInfo,��HbDllType.h��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSystemManufactureInfo(void *handle, SysManuInfo* pSysManuInfo);

/*********************************************************
* ��    ��: No047
* �� �� ��: HBI_GetSystemStatus
* ��������: Get System Status Information
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	Out:SysStatusInfo* pSysStatus,��HbDllType.h��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSystemStatus(void *handle, SysStatusInfo* pSysStatus);

/*********************************************************
* ��    ��: No048
* �� �� ��: HBI_SetGigabitEther
* ��������: ����������Ϣ����
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	    EtherInfo* pEther,��HbDllType.h��
	Out:��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetGigabitEther(void *handle, EtherInfo* pEther);

/*********************************************************
* ��    ��: No049
* �� �� ��: HBI_SetHvgTriggerMode
* ��������: Sety High Voltage Generator Interface Trigger Mode Information
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	    HiVolTriggerModeInfo* pHvgTrigMode,��HbDllType.h��
	Out:��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetHvgTriggerMode(void *handle, HiVolTriggerModeInfo* pHvgTrigMode);

/*********************************************************
* ��    ��: No050
* �� �� ��: HBI_SetSystemConfig
* ��������: Set System Configuration Information
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	    SysCfgInfo* pSysCfg,��HbDllType.h��
	Out:��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSystemConfig(void *handle, SysCfgInfo* pSysCfg);

/*********************************************************
* ��    ��: No051
* �� �� ��: HBI_SetImageCalibration
* ��������: ����ROM Image firmware calibrateʹ��
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	    ImgCaliCfg* pImgCaliCfg,��HbDllType.h��
	Out:��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetImageCalibration(void *handle, ImgCaliCfg* pImgCaliCfg);

/*********************************************************
* ��    ��: No052
* �� �� ��: HBI_SetVoltageAdjustConfig
* ��������: Set Voltage Adjust Configuration.
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	    VolAdjustCfg* pVolAdjustCfg,��HbDllType.h��
	Out:��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetVoltageAdjustConfig(void *handle, VolAdjustCfg* pVolAdjustCfg);

/*********************************************************
* ��    ��: No053
* �� �� ��: HBI_SetTICOFConfig
* ��������: Set TI COF Parameter Configuration.
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	    TICOFCfg* pTICOFCfg,��HbDllType.h��
	Out:��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetTICOFConfig(void *handle, TICOFCfg* pTICOFCfg);

/*********************************************************
* ��    ��: No054
* �� �� ��: HBI_SetCMOSConfig
* ��������: Set CMOS Parameter Configuration.
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	    CMOSCfg* pCMOSCfg,��HbDllType.h��
	Out:��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetCMOSConfig(void *handle, CMOSCfg* pCMOSCfg);

/*********************************************************
* ��    ��: No055
* �� �� ��: HBI_SetADICOFConfig
* ��������: Set ADI COF Parameter Configuration.
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	    ADICOFCfg* pADICOFCfg,��HbDllType.h��
	Out:��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetADICOFConfig(void *handle, ADICOFCfg* pADICOFCfg);

/*********************************************************
* ��    ��: No056
* �� �� ��: HBI_InitOffsetMode
* ��������: ��ʼ������ģ��
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
		CALIBRATE_INPUT_PARAM calibrate_param,��HbDllType.h��
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InitOffsetMode(void *handle, CALIBRATE_INPUT_PARAM calibrate_param);

/*********************************************************
* ��    ��: No057
* �� �� ��: HBI_InsertOffsetMode
* ��������: ��offset����ģ���в�������
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
		int group_id - ��ID
		char *filepath - �ļ�·��
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InsertOffsetMode(void *handle, int group_id, char *filepath);

/*********************************************************
* ��    ��: No058
* �� �� ��: HBI_ClearOffsetMode
* ��������: ���offset����ģ��
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ClearOffsetMode(void *handle);

/*********************************************************
* ��    ��: No059
* �� �� ��: HBI_GenerateOffsetTemp
* ��������: ����offsetģ��
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
		int raw_num - ����ͼ����
		int bprevew - �Ƿ�����previewģ�壬1-���ɣ�0-������
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GenerateOffsetTemp(void *handle, int raw_num, int bprevew = 0);

/*********************************************************
* ��    ��: No060
* �� �� ��: HBI_InitGainMode
* ��������: ��ʼ��gain����ģ��
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
		CALIBRATE_INPUT_PARAM calibrate_param - ��������
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InitGainMode(void *handle, CALIBRATE_INPUT_PARAM calibrate_param);

/*********************************************************
* ��    ��: No061
* �� �� ��: HBI_InsertGainMode
* ��������: ��gain����ģ���в�������
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	int group_id - ��ID
	char *filepath - �ļ�·��
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InsertGainMode(void *handle, int group_id, char *filepath);

/*********************************************************
* ��    ��: No062
* �� �� ��: HBI_ClearGainMode
* ��������: ���gain����ģ��
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ClearGainMode(void *handle);

/*********************************************************
* ��    ��: No063
* �� �� ��: HBI_GenerateGainTemp
* ��������: ����gainģ��
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
		int group_sum - ����
		int per_group_num - ÿ�����ͼƬ����
		int bprevew       - �Ƿ�����previewģ�壬1-���ɣ�0-������
		Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GenerateGainTemp(void *handle, int group_sum, int per_group_num, int bprevew = 0);

/*********************************************************
* ��    ��: No064
* �� �� ��: HBI_InitDefectMode
* ��������: ��ʼ��defect����ģ��
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	    CALIBRATE_INPUT_PARAM calibrate_param - ��������
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InitDefectMode(void *handle, CALIBRATE_INPUT_PARAM calibrate_param);

/*********************************************************
* ��    ��: No065
* �� �� ��: HBI_InsertDefectMode
* ��������: ��defect����ģ���в�������
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	    int group_id - ��ID
		char *filepath - �ļ�·��
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_InsertDefectMode(void *handle, int group_id, char *filepath);

/*********************************************************
* ��    ��: No066
* �� �� ��: HBI_ClearDefectMode
* ��������: ���defect����ģ��
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ClearDefectMode(void *handle);

/*********************************************************
* ��    ��: No067
* �� �� ��: HBI_GenerateDefectTemp
* ��������: ����defectģ��
* ����˵��:
	In: void *handle  - ���(�޷���ָ��)
	int group_sum     - ����
	int per_group_num - ÿ�����ͼƬ����
	int bprevew       - �Ƿ�����previewģ�壬1-���ɣ�0-������
	Out: ��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GenerateDefectTemp(void *handle, int group_sum, int per_group_num, int bprevew = 0);

/*********************************************************
* ��    ��: No068
* �� �� ��: HBI_UpgradeFirmware
* ��������: �����̼���Ϣ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			char *binfile - bin�ļ�·��
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_UpgradeFirmware(void *handle, char *binfile);

/*********************************************************
* ��    ��: No069
* �� �� ��: HBI_StopUpdateFirmware
* ��������: ֹͣ���¹̼�
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		out:��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_StopUpdateFirmware(void *handle);

/*********************************************************
* ��    ��: No070
* �� �� ��: HBI_UploadTemplate
* ��������: �ϴ�����ģ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			UPLOAD_MODE *uploadmode - �ϴ�ģ�Ͷ���ָ��
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_UploadTemplate(void *handle, UPLOAD_MODE *uploadmode);

/*********************************************************
* ��    ��: No071
* �� �� ��: HBI_StopUploadTemplate
* ��������: ֹͣ�ϴ�����ģ��
* ����˵��:
		In:  void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_StopUploadTemplate(void *handle);

/*********************************************************
* ��    ��: No072
* �� �� ��: HBI_GetWiffyProperty
* ��������: ��ȡ��������
* ����˵��:
	In: void *handle - ���(�޷���ָ��)
	Out: void *pWiffy - �ݲ�֧��
* �� �� ֵ��int
	0   - �ɹ�
	��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetWiffyProperty(void *handle, void *pWiffy);

/*********************************************************
* ��    ��: No073
* �� �� ��: HBI_SetAedThreshold
* ��������: ����AED��ֵ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		int out_ivalue - ��ֵ [25,000~1,000,000]
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetAedThreshold(void *handle, int in_ivalue);

/*********************************************************
* ��    ��: No074
* �� �� ��: HBI_GetAedThreshold
* ��������: ��ȡAED��ֵ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		out:int *out_ivalue - ��ֵ [10,000~1,000,000]
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetAedThreshold(void *handle, int *out_ivalue);

/*********************************************************
* ��    ��: No075
* �� �� ��: HBI_SetSaturationValue
* ��������: ���ñ���ֵ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		out:int *out_ivalue - ����ֵ [0~65,535]
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSaturationValue(void *handle, int in_ivalue);

/*********************************************************
* ��    ��: No076
* �� �� ��: HBI_GetSaturationValue
* ��������: ��ȡ����ֵ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		    int *out_ivalue - ����ֵ [0~65,535]
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSaturationValue(void *handle, int *out_ivalue);

/*********************************************************
* ��    ��: No077
* �� �� ��: HBI_SetClippingValue
* ��������: ����ͼ�����ֵ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		out:int *out_ivalue - ����ֵ [0~65,535]
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetClippingValue(void *handle, int in_ivalue);

/*********************************************************
* ��    ��: No078
* �� �� ��: HBI_GetClippingValue
* ��������: ��ȡͼ�����ֵ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		out:int *out_ivalue - ����ֵ [0~65,535]
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetClippingValue(void *handle, int *out_ivalue);

/*********************************************************
* ��    ��: No079
* �� �� ��: HBI_SetPreAcqTm
* ��������: ����������֡�ɼ���պͲɼ�֮���ʱ����
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		    int *in_itime - ʱ�� [0~10000] ��λ:mm
				0-��ʾ������֡�ɼ���HBI_Prepare��HBI_SingleAcquisition��ɵ�֡�ɼ�
				��0-��ʾ������֡�ɼ���ֻ��HBI_Prepare���ɰ���Ԥ��ʱ����ɵ�֡�ɼ�
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetPreAcqTm(void *handle, int in_itime);

/*********************************************************
* ��    ��: No080
* �� �� ��: HBI_GetPreAcqTm
* ��������: ��ȡ������֡�ɼ���պͲɼ�֮���ʱ����
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		out:int *out_itime - ʱ�� [0~10000] ��λ:mm
		        0-��ʾ������֡�ɼ���HBI_Prepare��HBI_SingleAcquisition��ɵ�֡�ɼ�
				��0-��ʾ������֡�ɼ���ֻ��HBI_Prepare���ɰ���Ԥ��ʱ����ɵ�֡�ɼ�
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetPreAcqTm(void *handle, int *out_itime);

/*********************************************************
* ��    ��: No081
* �� �� ��: HBI_SetSDKWorkMode
* ��������: ����SDK����ģʽ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		    mode - 1��debug mode��0��product mode
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetSDKWorkMode(void *handle, int mode);

/*********************************************************
* ��    ��: No082
* �� �� ��: HBI_GetSDKWorkMode
* ��������: ��ȡSDK����ģʽ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
* �� �� ֵ��int
		0    - product mode
		1    - debug mode
		8007 - No Device
		8021 - Error work mode
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetSDKWorkMode(void *handle);

/*********************************************************
* ��    ��: No083
* �� �� ��: HBI_SendData
* ��������: ���ͺ���
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		    unsigned char *data - ���ݻ�����
			int datalen - ���ݻ���������
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SendData(void *handle, unsigned char *data, int datalen);

/*********************************************************
* ��    ��: No084
* �� �� ��: HBI_RecvData
* ��������: ���պ���
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out unsigned char *data - ���ݻ�����
		int datalen - ���ݻ���������
		int time - ��ʱʱ�䣬��λms
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_RecvData(void *handle, unsigned char *data, int datalen, int time=1000);

/*********************************************************
* ��    ��: No085
* �� �� ��: HBI_SetLPFLevel
* ��������: ���õ�ͨ�˲�����λ����
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			int nLPFLevel - ��λ
			[n]-Invalid
			[0]-210khz,Ĭ��210khz
			[1]-105khz
			[2]-52khz
			[3]-26khz
			[4]-13khz
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetLPFLevel(void *handle, int nLPFLevel);

/*********************************************************
* ��    ��: No086
* �� �� ��: HBI_GetLPFLevel
* ��������: ��ȡ��ͨ�˲�����λ����
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		[0]-210khz,Ĭ��210khz
		[1]-105khz
		[2]-52khz
		[3]-26khz
		[4]-13khz
        [n]-Invalid
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetLPFLevel(void *handle);

/*********************************************************
* ��    ��: No087
* �� �� ��: HBI_ImmediateGenerateTemplate
* ��������: һ������ģ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		    EnumIMAGE_ACQ_MODE _mode - ����ģ������
				OFFSET_TEMPLATE_MODE    �����ɼ�һ�鰵��ͼ - offset
				GAIN_TEMPLATE_MODE      �����ɼ�һ������ͼ - gain
				DEFECT_ACQ_GROUP1,      �����ɼ�һ������ͼ - defect group1
	            DEFECT_ACQ_GROUP2,      �����ɼ�һ������ͼ - defect group2
	            DEFECT_ACQ_AND_TEMPLATE �����ɼ�һ������ͼ - defect group3 and generate template
			int bprevew - �Ƿ�����previewģ�壬1-���ɣ�0-������
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ImmediateGenerateTemplate(void *handle, EnumIMAGE_ACQ_MODE _mode, int bprevew = 0);

/*********************************************************
* ��    ��: No087
* �� �� ��: HBI_16UCConvertTo8UC
* ��������: 16λ����ת��Ϊ8λ
* ����˵��:
		In:     void *handle - ���(�޷���ָ��)
		In/Out: unsigned char *imgbuff
		In/Out: int *nbufflen
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_16UCConvertTo8UC(void *handle, unsigned char *imgbuff, int *nbufflen);

/*********************************************************
* ��    ��: No088
* �� �� ��: HBI_SetPreviewMode
* ��������: ����PreviewԤ����ȡ��Ԥ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			int inTriggMode
				1-����
				3-��ѹ����
				4-FreeAED
				8-SAEC
				������֧�֡�
			int inPreviewMode
				0-normal image
				1-preview image
		Out:��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SetPreviewMode(void *handle, int inTriggMode, int inPreviewMode);

/*********************************************************
* ��    ��: No089
* �� �� ��: HBI_GetPreviewMode
* ��������: ����Preview mode��ǰֵ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		out:int *outMode - preview mode [0~2]
		    0-normal image,1-preview image
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetPreviewMode(void *handle, int *outMode);

/*********************************************************
* ��    ��: No090
* �� �� ��: HBI_SAecModeApply
* ��������: ���û�ȡ��SAECģʽ
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
			int inPreviewMode
				0-normal image
				1-preview image
			int inPerTime  - fpd pre ready delay time
			int inPostTime - fpd post ready delay time
		Out:��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:��Ϊ���������
           1�����ã�inPreviewMode=1(preview image)��inPerTime��inPostTime(�ͻ�ָ��)
		   2��ȡ����inPreviewMode=0(normal image)��inPerTime��inPostTimeĬ�ϼ���
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SAecModeApply(void *handle, int inPreviewMode, int inPerTime=0, int inPostTime=0);

/*********************************************************
* ��    ��: No091
* �� �� ��: HBI_SAecAcq
* ��������: SAEC�ɼ���ͼָ��
* ����˵��:
		In: void *handle - ���(�޷���ָ��)
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_SAecAcq(void *handle);

/*********************************************************
* ��    ��: No092
* �� �� ��: HBI_GetFPDSerialNumber
* ��������: ��ȡ��Ʒ���к�
* ����˵��:
	    In: void *handle - ���(�޷���ָ��)
	    In/Out: char *szSn,����14λ
* �� �� ֵ��int
	    0   - �ɹ�
	    ��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_GetFPDSerialNumber(void *handle, char *szSn);
#ifdef _cplusplus
}
#endif
#endif // __HB_FPD_DLL_H_