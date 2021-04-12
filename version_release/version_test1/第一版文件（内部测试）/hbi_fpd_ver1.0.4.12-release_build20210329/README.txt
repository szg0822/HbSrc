用户须知：
1、主目录：hbi_fpd_ver*.*.*.*-*_build* 
     例如，hbi_fpd_ver1.0.4.6-beta_build20201104表示版本号为ver1.0.4.6，软件发布状态beta或release，创建日期2020年9月11日
     
包括HBI_DLL、SDK_Demo_Example和HBI_FPD_Tool。
2、HBI_DLL：昊博探测器SDK开发接口库文件以及集成开发说明书
     INCLUDES：SDK API包含头文件               
     bin：库文件
     Document：API接口开发文档以及注意事项

3、SDK_Demo_Example：SDK开发Demo源码(VC++)，以DemoLib为准

4、HBI_FPD_Tool：昊博探测器工具软件
     Win32：表示32位程序
      x64：表示64位程序
      Document：factory.csv 出厂配置文件，用于出厂参数恢复，XDiscovery软件使用操作手册V1.*_* 工具软件说明书。
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//20201015校正接口集成
1、替换头文件HbDllType.h和HbFpdDll.h
2、新增接口函数，包含头文件即可
/*********************************************************
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
		Out: 无
* 返 回 值：int
		0   - 成功
		非0 - 失败
* 备    注:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ImmediateGenerateTemplate(void *handle, EnumIMAGE_ACQ_MODE _mode)；
3、回调函数
case ECALLBACK_TYPE_GENERATE_TEMPLATE:
if (len == ECALLBACK_TEMPLATE_RESULT) {
	//if (theDoc->m_templateHwnd != NULL)
	 / /  PostMessage(theDoc->m_templateHwnd, WM_USER_NOTICE_TEMPLATE_TOOL, (WPARAM)nid, (LPARAM)0);
                if (nid == -1)
                     printf("offset template generate failed!");
                else if (nid == 1)
                      printf("offset template generate success!");
                else if (nid == -2)
                      printf("gain template generate failed!");
                else if (nid == 2)
                      printf("gain template generate success!");
                else if (nid == -3)
                      printf("defect template generate failed!");
                else if (nid == 3)
                      printf("defect template generate success!");
	else if (nid == 4)
	      printf("defect acq light image success!");
	else if (nid == -4)
	      printf("defect acq light image failed!");
                else
                     printf("err:other error!");
	}
}
break;
4、调用
                EnumIMAGE_ACQ_MODE enumTemplateType = EnumIMAGE_ACQ_MODE::OFFSET_TEMPLATE_MODE;
	int ret = HBI_ImmediateGenerateTemplate(theDoc->m_pFpd, enumTemplateType);
	if (ret != HBI_SUCCSS) {
		::AfxMessageBox("err:call HBI_ImmediateGenerateTemplate failed!");
	}
5、HBI_UpdateCorrectEnable和HBI_TriggerAndCorrectApplay
HBI_UpdateCorrectEnable：设置矫正使能
HBI_TriggerAndCorrectApplay：设置触发模式和矫正是能
用户根据实际情况选用接口