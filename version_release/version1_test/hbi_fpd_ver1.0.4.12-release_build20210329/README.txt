�û���֪��
1����Ŀ¼��hbi_fpd_ver*.*.*.*-*_build* 
     ���磬hbi_fpd_ver1.0.4.6-beta_build20201104��ʾ�汾��Ϊver1.0.4.6���������״̬beta��release����������2020��9��11��
     
����HBI_DLL��SDK_Demo_Example��HBI_FPD_Tool��
2��HBI_DLL��껲�̽����SDK�����ӿڿ��ļ��Լ����ɿ���˵����
     INCLUDES��SDK API����ͷ�ļ�               
     bin�����ļ�
     Document��API�ӿڿ����ĵ��Լ�ע������

3��SDK_Demo_Example��SDK����DemoԴ��(VC++)����DemoLibΪ׼

4��HBI_FPD_Tool��껲�̽�����������
     Win32����ʾ32λ����
      x64����ʾ64λ����
      Document��factory.csv ���������ļ������ڳ��������ָ���XDiscovery���ʹ�ò����ֲ�V1.*_* �������˵���顣
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//20201015У���ӿڼ���
1���滻ͷ�ļ�HbDllType.h��HbFpdDll.h
2�������ӿں���������ͷ�ļ�����
/*********************************************************
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
		Out: ��
* �� �� ֵ��int
		0   - �ɹ�
		��0 - ʧ��
* ��    ע:
*********************************************************/
HB_PDF_DLL int __stdcall HBI_ImmediateGenerateTemplate(void *handle, EnumIMAGE_ACQ_MODE _mode)��
3���ص�����
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
4������
                EnumIMAGE_ACQ_MODE enumTemplateType = EnumIMAGE_ACQ_MODE::OFFSET_TEMPLATE_MODE;
	int ret = HBI_ImmediateGenerateTemplate(theDoc->m_pFpd, enumTemplateType);
	if (ret != HBI_SUCCSS) {
		::AfxMessageBox("err:call HBI_ImmediateGenerateTemplate failed!");
	}
5��HBI_UpdateCorrectEnable��HBI_TriggerAndCorrectApplay
HBI_UpdateCorrectEnable�����ý���ʹ��
HBI_TriggerAndCorrectApplay�����ô���ģʽ�ͽ�������
�û�����ʵ�����ѡ�ýӿ�