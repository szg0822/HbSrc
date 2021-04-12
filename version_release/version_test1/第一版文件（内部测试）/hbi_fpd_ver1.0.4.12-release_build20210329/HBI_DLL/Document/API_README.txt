//////////////////////////////////////////////
// �����HBI_FPD_Tool �� SDK
// �汾��Ver 1.0.3.7
// ʱ�䣺2020/06/24
// �Ϻ�껲�Ӱ��Ƽ����޹�˾
//////////////////////////////////////////////
Include��SDK����ͷ�ļ���HbFpdDll.h��HbDllType.h��HbDllError.h
Bin���ӿڿ�-HBI_FPD.dll��������opencv_core249.dll��opencv_highgui249.dll��opencv_imgproc249.dll
Lib��HBI_FPD.lib
Demo��Demo Tool��껲����������Demo.rar��DemoDll.rar���ṩ���û��Ŀ���������
            ����Demo�Ǿ�̬���ã�DemoDll��̬���á�
Doc��SDK API�ӿ��ĵ���
ע��������İ汾��32λ��64λ��

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
############################################################################
# ԭ�ӿ�˵��
# �µĽ�������������ο�
############################################################################
#ԭ�ӿ�˵����
1��Document
��HBI_FPD API Programming Reference Ver1.0.3.7.pdf����SDK�ӿ�˵���ĵ���
��Software_Demoʹ�ò����ֲ�v1.2_20200212.pdf����Tool����˵���ĵ���

2��HBI_FPD_SDK����̬��ӿڡ�ͷ�ļ��Լ�����Demo����
  2.1��HBI_DLL\:SDK�ӿ��ļ�������ͷ�ļ��Ͳ�ͬƽ̨��dll��lib�ļ���

   2.1.1�� HBI_DLL\INCLUDES������HbFpdDll.h��HbDllType.h��HbDllError.hһ������ͷ�ļ���
     a.��HbFpdDll.h�������������Լ�˵��������ɲο���HBI_FPD API Programming Reference Ver1.0.2.8.pdf����
     b.��HbDllType.h��������ص��������巵���¼������Լ��̼������ṹ�壻
     c.��HbDllError.h���������Լ���������Ϣ��
  
   2.1.2��HBI_FPD.dll��HBI_FPD.lib������x64-Ӧ��64λ������̬��,x86-Ӧ��32λ������̬�⣻

  2.2��HBI_FPD_SDK\SDK_Demo_Example���ӿڿ���Demo�����������C#��VC��������Demo������DemoLib.rarΪ��̬���ã�DemoDll.rarΪ��̬���÷����������ο�����C#�汾Demo��δά���������ο���

3��HBI_FPD_Tool��Ϊ�ͻ��ṩ���Թ�����������ļ��У�HBI_FPD_Tool.exe��HBI_FPD.dll��HBI_FPD.lib�Լ������⣨opencv�⣩���ð汾SDK��ȥ��opencv�����⡣

4����������
   4.1 ��"HBI_DLL\INCLUDES"ͷ�ļ��������û�����ָ��Ŀ¼�£�
   4.2 ����̬���ļ�"HBI_DLL\BIN"�������û�����Ŀ¼����ϵͳĿ¼�£�ע��32bits��64bits��
   4.3 ����
 1���û������а���ͷ�ļ�
#define _DLL_EX_IM 0
#include "HbFpdDll.h" // ��̬��ͷ�ļ�
#pragma comment(lib,"HBI_FPD.lib") 

   2���û������豸
// �豸���������
void* m_pFpd;                            // fpd sdk������
RegCfgInfo* m_pLastRegCfg;              // �̼�����
FPD_AQC_MODE aqc_mode;// �ɼ�ģʽ�Ͳ�������֡�ɼ�����֡�ɼ�������ͼ�Լ�����ͼ�ɼ�

// ͼ��ֱ���
int m_imgW, m_imgH;
char m_path[MAX_PATH];
fpd_base_cfg_st *fpd_base;
IMAGE_PROPERTY img_pro;
IMAGE_CORRECT_ENABLE* pCorrect;
	
// һЩ������������
m_pLastRegCfg = new RegCfgInfo;
pCorrect = new IMAGE_CORRECT_ENABLE;
fpd_base = new fpd_base_cfg_st;
	    
      �����豸�б�ṹ�壬���ڡ�OEMFPDSDK_TYPE.H���ж���.    

3���ӿڵ�������  
// ��ʼ��DLL��Դ
void *m_pFpd = HBI_Init();
if (m_pFpd == NULL) {
  printf("HBI_Init init failed!\n");
  return false;
}	
				
/*
* ע��ص�����
*/
// ����ע��ص�����
// myCallbackFun �ص�����
int ret = HBI_RegEventCallBackFun(m_pFpd, myCallbackFun); 
if (ret != 0) {
	printf("HBI_RegEventCallBackFun failed!\n");
	HBI_Destroy(m_pFpd);
  m_pFpd = NULL;	
	return false;
}

/*
* ����
*/
ret = HBI_ConnectDetector(m_pFpd, "192.168.10.40", 32897, "192.168.10.20", 32896,20);
if (ret != 0) {
	printf("����ʧ��!\n");
	HBI_Destroy(m_pFpd);
  m_pFpd = NULL;	
	return false;
}
			
// ���ô���ģʽ��ͼ�����ʹ�� 
int _triggerMode = 3; // 1-������3-��ѹ������4-freeAED
IMAGE_CORRECT_ENABLE* pCorrect = new IMAGE_CORRECT_ENABLE;
if (pCorrect != NULL) {
	pcorrect->bFeedbackCfg = true;       //true-ECALLBACK_TYPE_ROM_UPLOAD Event,false-ECALLBACK_TYPE_SET_CFG_OK Event
	pcorrect->ucOffsetCorrection = 0x02; //00-"Do nothing";01-"prepare Offset Correction";  02-"post Offset Correction";
	pcorrect->ucGainCorrection   = 0x01; //00-"Do nothing";01-"Software Gain Correction";   02-"Hardware Gain Correction"
	pcorrect->ucDefectCorrection = 0x01; //00-"Do nothing";01-"Software Defect Correction"; 02-"Software Defect Correction"
	pcorrect->ucDummyCorrection  = 0x01; //00-"Do nothing";01-"Software Dummy Correction";  02-"Software Dummy Correction"
	ret=HBI_TriggerAndCorrectApplay(m_pFpd,_triggerMode, pCorrect);	
	if (ret != 0) {
		printf("����ʧ��!\n");
	}
}
  
// �Ͽ�����
HBI_DisConnectDetector(m_pFpd));

// ������Դ(�����Ͽ����Ӻ���Դ�ͷ�)
HBI_Destroy(m_pFpd);
m_pFpd = NULL;	
	
ע�⣺  
1�����ӳɹ���ƽ����Զ�����ROM������
2��HBI_Init��HBI_Destroy�����ӺͶϿ�ƽ���Ӧ��
3��HBI_ConnectDetector��HBI_DisConnectDetector����ʼ�����ͷ��豸��Ӧ��
4��HBI_SetSysParamCfg��HBI_GetSDKVerion�����Ƕ�ȡƽ��̼�������HBI_SetSysParamCfg����̼��������ȡ�������첽������HBI_GetSDKVerion�����ӳɹ������óɹ����ȡ������ͬ��������
5��HBI_ConnectDetector  ����ƽ�壬�ص��¼�ECALLBACK_TYPE_ROM_UPLOAD������ǰ�̼��Ĳ��������������Ϣ�ѹ̻��ã��û�����ֱ��ʹ��
6��HBI_TriggerAndCorrectApplay�����ݲ�������ECALLBACK_TYPE_ROM_UPLOAD������ǰ�̼��Ĳ�����ECALLBACK_TYPE_SET_CFG_OK�¼�ȷ�ϳɹ����û�����ʵ��������ò�����
7��HBI_Destroy  �ͷ���Դ�����ΪNULL�����ֱ�ӹرգ�����HBI_Destroy���ɣ�HBI_Destroy���Ѱ���HBI_DisConnectDetector�ĵ��á�
  		
4������ص�����
############################################
#�ص��������¼�˵����
#command���¼�ID
#buff:��������ַ
#len:����������
#nid��ͼ������Ϊ֡��
############################################
int CDemoDlg::myCallbackFun(unsigned char command, void *buff, int len, int nId)
{
	int ret = 0;
	if ((command == ECALLBACK_TYPE_SINGLE_IMAGE) || 
		(command == ECALLBACK_TYPE_MULTIPLE_IMAGE) ||
		(command == ECALLBACK_TYPE_ROM_UPLOAD)) {
		if (buff == NULL || len == 0) {
			printf("ע��ص����������쳣!\n");
			return ret;
		}
	}

	int status = -1;
	int j = 0;
	ret = 1;
	switch (command)
	{
	case ECALLBACK_TYPE_NET_ERR_MSG: // ƽ��״̬������/�Ͽ�/ready/busy
		printf("ECALLBACK_TYPE_NET_ERR_MSG,recode=%d\n", len);
		}
		break;
	case ECALLBACK_TYPE_ROM_UPLOAD:/* �������� */
		printf("OEMFPD_CALLBACK_EVENT_ROM_CONFIG:\n");

		break;
	case ECALLBACK_TYPE_SET_CFG_OK:
		printf("ECALLBACK_TYPE_SET_CFG_OK:\n");
		break;
	case ECALLBACK_TYPE_SINGLE_IMAGE:   // ��֡�ɼ���ͼ
	case ECALLBACK_TYPE_MULTIPLE_IMAGE:  // �����ɼ���ͼ
		break;
	case ECALLBACK_TYPE_THREAD_EVENT:
	default:
		printf("ECALLBACK_TYPE_THREAD_EVENT,command=%02x\n", command);
		break;
	}
	return ret;
}
	ͨ���ص��������أ�ͼ������ԣ������Լ�λ���������Լ���С����Ϣ������ģʽ���̼�У��ʹ�ܣ�Ŀǰֻ֧��offsetУ������������������У�����ȡ�
	ͨ���ص���������ͼ�����ݵ�ַ�Լ����ȣ������Ҫ��������˵����
