//////////////////////////////////////////////
// 软件：HBI_FPD_Tool 和 SDK
// 版本：Ver 1.0.3.7
// 时间：2020/06/24
// 上海昊博影像科技有限公司
//////////////////////////////////////////////
Include：SDK所需头文件，HbFpdDll.h、HbDllType.h、HbDllError.h
Bin：接口库-HBI_FPD.dll、依赖库opencv_core249.dll、opencv_highgui249.dll、opencv_imgproc249.dll
Lib：HBI_FPD.lib
Demo：Demo Tool是昊博测试软件，Demo.rar和DemoDll.rar是提供给用户的开发用例，
            区别Demo是静态调用，DemoDll动态调用。
Doc：SDK API接口文档。
注意依赖库的版本，32位和64位。

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
############################################################################
# 原接口说明
# 新的解决方案，仅供参考
############################################################################
#原接口说明：
1、Document
《HBI_FPD API Programming Reference Ver1.0.3.7.pdf》：SDK接口说明文档；
《Software_Demo使用操作手册v1.2_20200212.pdf》：Tool操作说明文档；

2、HBI_FPD_SDK：动态库接口、头文件以及开发Demo程序。
  2.1》HBI_DLL\:SDK接口文件，包含头文件和不同平台的dll和lib文件；

   2.1.1》 HBI_DLL\INCLUDES：包含HbFpdDll.h、HbDllType.h和HbDllError.h一共三个头文件。
     a.《HbFpdDll.h》：导出函数以及说明，具体可参考《HBI_FPD API Programming Reference Ver1.0.2.8.pdf》；
     b.《HbDllType.h》：命令、回调函数定义返回事件类型以及固件参数结构体；
     c.《HbDllError.h》：错误以及返回码信息表；
  
   2.1.2》HBI_FPD.dll和HBI_FPD.lib，其中x64-应用64位开发动态库,x86-应用32位开发动态库；

  2.2》HBI_FPD_SDK\SDK_Demo_Example：接口开发Demo程序，里面包含C#和VC开发程序Demo，其中DemoLib.rar为静态调用，DemoDll.rar为动态调用方法（仅供参考），C#版本Demo暂未维护，仅供参考。

3、HBI_FPD_Tool：为客户提供调试工具软件包含文件有：HBI_FPD_Tool.exe、HBI_FPD.dll、HBI_FPD.lib以及依赖库（opencv库），该版本SDK已去除opencv依赖库。

4、开发流程
   4.1 将"HBI_DLL\INCLUDES"头文件拷贝的用户开发指定目录下；
   4.2 将动态库文件"HBI_DLL\BIN"拷贝到用户工程目录或者系统目录下，注意32bits和64bits；
   4.3 步骤
 1》用户工程中包含头文件
#define _DLL_EX_IM 0
#include "HbFpdDll.h" // 动态库头文件
#pragma comment(lib,"HBI_FPD.lib") 

   2》用户定义设备
// 设备句柄和配置
void* m_pFpd;                            // fpd sdk对象句柄
RegCfgInfo* m_pLastRegCfg;              // 固件配置
FPD_AQC_MODE aqc_mode;// 采集模式和参数，单帧采集、多帧采集、暗场图以及亮场图采集

// 图像分辨率
int m_imgW, m_imgH;
char m_path[MAX_PATH];
fpd_base_cfg_st *fpd_base;
IMAGE_PROPERTY img_pro;
IMAGE_CORRECT_ENABLE* pCorrect;
	
// 一些基本数据类型
m_pLastRegCfg = new RegCfgInfo;
pCorrect = new IMAGE_CORRECT_ENABLE;
fpd_base = new fpd_base_cfg_st;
	    
      其中设备列表结构体，已在《OEMFPDSDK_TYPE.H》中定义.    

3》接口调用流程  
// 初始化DLL资源
void *m_pFpd = HBI_Init();
if (m_pFpd == NULL) {
  printf("HBI_Init init failed!\n");
  return false;
}	
				
/*
* 注册回调函数
*/
// 设置注册回调函数
// myCallbackFun 回调函数
int ret = HBI_RegEventCallBackFun(m_pFpd, myCallbackFun); 
if (ret != 0) {
	printf("HBI_RegEventCallBackFun failed!\n");
	HBI_Destroy(m_pFpd);
  m_pFpd = NULL;	
	return false;
}

/*
* 连接
*/
ret = HBI_ConnectDetector(m_pFpd, "192.168.10.40", 32897, "192.168.10.20", 32896,20);
if (ret != 0) {
	printf("连接失败!\n");
	HBI_Destroy(m_pFpd);
  m_pFpd = NULL;	
	return false;
}
			
// 设置触发模式和图像矫正使能 
int _triggerMode = 3; // 1-软触发，3-高压触发，4-freeAED
IMAGE_CORRECT_ENABLE* pCorrect = new IMAGE_CORRECT_ENABLE;
if (pCorrect != NULL) {
	pcorrect->bFeedbackCfg = true;       //true-ECALLBACK_TYPE_ROM_UPLOAD Event,false-ECALLBACK_TYPE_SET_CFG_OK Event
	pcorrect->ucOffsetCorrection = 0x02; //00-"Do nothing";01-"prepare Offset Correction";  02-"post Offset Correction";
	pcorrect->ucGainCorrection   = 0x01; //00-"Do nothing";01-"Software Gain Correction";   02-"Hardware Gain Correction"
	pcorrect->ucDefectCorrection = 0x01; //00-"Do nothing";01-"Software Defect Correction"; 02-"Software Defect Correction"
	pcorrect->ucDummyCorrection  = 0x01; //00-"Do nothing";01-"Software Dummy Correction";  02-"Software Dummy Correction"
	ret=HBI_TriggerAndCorrectApplay(m_pFpd,_triggerMode, pCorrect);	
	if (ret != 0) {
		printf("连接失败!\n");
	}
}
  
// 断开连接
HBI_DisConnectDetector(m_pFpd));

// 回收资源(包括断开连接和资源释放)
HBI_Destroy(m_pFpd);
m_pFpd = NULL;	
	
注意：  
1》连接成功后，平板会自动反馈ROM参数。
2》HBI_Init和HBI_Destroy：连接和断开平板对应；
3》HBI_ConnectDetector和HBI_DisConnectDetector：初始化和释放设备对应；
4》HBI_SetSysParamCfg和HBI_GetSDKVerion：都是读取平板固件参数，HBI_SetSysParamCfg是向固件发请求获取参数，异步函数；HBI_GetSDKVerion是连接成功或设置成功后获取参数，同步函数。
5》HBI_ConnectDetector  连接平板，回调事件ECALLBACK_TYPE_ROM_UPLOAD反馈当前固件的参数，这里基本信息已固化好，用户可以直接使用
6》HBI_TriggerAndCorrectApplay，根据参数反馈ECALLBACK_TYPE_ROM_UPLOAD反馈当前固件的参数和ECALLBACK_TYPE_SET_CFG_OK事件确认成功，用户根据实际情况设置参数。
7》HBI_Destroy  释放资源，句柄为NULL，如果直接关闭，调用HBI_Destroy即可，HBI_Destroy中已包含HBI_DisConnectDetector的调用。
  		
4》定义回调函数
############################################
#回调函数及事件说明：
#command：事件ID
#buff:缓冲区地址
#len:缓冲区长度
#nid：图像数据为帧号
############################################
int CDemoDlg::myCallbackFun(unsigned char command, void *buff, int len, int nId)
{
	int ret = 0;
	if ((command == ECALLBACK_TYPE_SINGLE_IMAGE) || 
		(command == ECALLBACK_TYPE_MULTIPLE_IMAGE) ||
		(command == ECALLBACK_TYPE_ROM_UPLOAD)) {
		if (buff == NULL || len == 0) {
			printf("注册回调函数参数异常!\n");
			return ret;
		}
	}

	int status = -1;
	int j = 0;
	ret = 1;
	switch (command)
	{
	case ECALLBACK_TYPE_NET_ERR_MSG: // 平板状态：连接/断开/ready/busy
		printf("ECALLBACK_TYPE_NET_ERR_MSG,recode=%d\n", len);
		}
		break;
	case ECALLBACK_TYPE_ROM_UPLOAD:/* 更新配置 */
		printf("OEMFPD_CALLBACK_EVENT_ROM_CONFIG:\n");

		break;
	case ECALLBACK_TYPE_SET_CFG_OK:
		printf("ECALLBACK_TYPE_SET_CFG_OK:\n");
		break;
	case ECALLBACK_TYPE_SINGLE_IMAGE:   // 单帧采集上图
	case ECALLBACK_TYPE_MULTIPLE_IMAGE:  // 连续采集上图
		break;
	case ECALLBACK_TYPE_THREAD_EVENT:
	default:
		printf("ECALLBACK_TYPE_THREAD_EVENT,command=%02x\n", command);
		break;
	}
	return ret;
}
	通过回调函数返回，图像的属性，长宽以及位数、类型以及大小端信息、触发模式、固件校正使能（目前只支持offset校正，后续会增加其他校正）等。
	通过回调函数返回图像数据地址以及长度，如果需要器参数请说明。
