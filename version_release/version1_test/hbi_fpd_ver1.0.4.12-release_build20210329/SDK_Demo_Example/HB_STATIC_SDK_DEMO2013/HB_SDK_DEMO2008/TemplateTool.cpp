// TemplateTool.cpp : implementation file
//

#include "stdafx.h"
#include "HB_SDK_DEMO2008.h"
#include "TemplateTool.h"
#include "HB_SDK_DEMO2008Dlg.h"

#define WAITTING_TIME 5

extern CHB_SDK_DEMO2008Dlg* theDemo;

#ifndef _XSLEEP_H_
#define _XSLEEP_H_

// This structure is used internally by the XSleep function 
struct XSleep_Structure {
	XSleep_Structure() {
		duration = 1000;
		eventHandle = NULL;
	}
	int duration;
	HANDLE eventHandle;
};

void XSleep(int nWaitInMSecs);

#endif // _XSLEEP_H_

DWORD WINAPI XSleepThread(LPVOID pWaitTime)
{
	XSleep_Structure *sleep = (XSleep_Structure *)pWaitTime;

	Sleep(sleep->duration);
	SetEvent(sleep->eventHandle);

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Function  : XSleep()
// Purpose   : To make the application sleep for the specified time
//			   duration.
//			   Duration the entire time duration XSleep sleeps, it
//			   keeps processing the message pump, to ensure that all
//			   messages are posted and that the calling thread does
//			   not appear to block all threads!
// Returns   : none
// Parameters:       
//  1. nWaitInMSecs - Duration to sleep specified in miliseconds.
//////////////////////////////////////////////////////////////////////
void XSleep(int nWaitInMSecs)
{
	XSleep_Structure sleep;
	sleep.duration = nWaitInMSecs;
	sleep.eventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);

	HANDLE getHandle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void *))XSleepThread, &sleep, 0, NULL);
	if (getHandle == NULL)
		return;

	MSG msg;
	while (::WaitForSingleObject(sleep.eventHandle, 0) == WAIT_TIMEOUT)
	{
		//get and dispatch messages
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	CloseHandle(sleep.eventHandle);
	CloseHandle(getHandle);
}

// CTemplateTool dialog

IMPLEMENT_DYNAMIC(CTemplateTool, CDialog)

CTemplateTool::CTemplateTool(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateTool::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_TEMPLATE);//修改对话框的图标
	enumTemplateType = OFFSET_TEMPLATE_MODE;
	m_ungroupid = 0;
}

CTemplateTool::~CTemplateTool()
{
	if (theDemo != NULL)
		theDemo->m_templateHwnd = NULL;
}

void CTemplateTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTemplateTool, CDialog)
	ON_BN_CLICKED(IDC_RADIO_OFFSET, &CTemplateTool::OnBnClickedRadioOffset)
	ON_BN_CLICKED(IDC_RADIO_GAIN, &CTemplateTool::OnBnClickedRadioGain)
	ON_BN_CLICKED(IDC_RADIO_DEFECT, &CTemplateTool::OnBnClickedRadioDefect)
	ON_BN_CLICKED(IDC_BTN_TEMPLATE_GENERATE, &CTemplateTool::OnBnClickedBtnTemplateGenerate)
	ON_MESSAGE(WM_USER_NOTICE_TEMPLATE_TOOL, &CTemplateTool::OnGenerateTemplateResult)
END_MESSAGE_MAP()


// CTemplateTool message handlers
BOOL CTemplateTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化
	if (theDemo != NULL)
		theDemo->m_templateHwnd = GetSafeHwnd();

	// 初始化参数
	CheckRadioButton(IDC_RADIO_GAIN, IDC_RADIO_DEFECT, IDC_RADIO_OFFSET);
	if (enumTemplateType != OFFSET_TEMPLATE_MODE)
		enumTemplateType = OFFSET_TEMPLATE_MODE;
	if (m_ungroupid != 0) m_ungroupid = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



void CTemplateTool::OnBnClickedRadioOffset()
{
	// TODO: 在此添加控件通知处理程序代码
	if (enumTemplateType != OFFSET_TEMPLATE_MODE) {
		enumTemplateType = OFFSET_TEMPLATE_MODE;
		((CButton *)GetDlgItem(IDC_BTN_TEMPLATE_GENERATE))->SetWindowTextA("Generate Offset Template");
	}
}


void CTemplateTool::OnBnClickedRadioGain()
{
	// TODO: 在此添加控件通知处理程序代码
	if (enumTemplateType != GAIN_TEMPLATE_MODE) {
		enumTemplateType = GAIN_TEMPLATE_MODE;
		((CButton *)GetDlgItem(IDC_BTN_TEMPLATE_GENERATE))->SetWindowTextA("Generate Gain Template");
	}
}


void CTemplateTool::OnBnClickedRadioDefect()
{
	// TODO: 在此添加控件通知处理程序代码
	if (enumTemplateType != DEFECT_ACQ_GROUP1) {
		enumTemplateType = DEFECT_ACQ_GROUP1;
		if (m_ungroupid != 0) m_ungroupid = 0;
		((CButton *)GetDlgItem(IDC_BTN_TEMPLATE_GENERATE))->SetWindowTextA("Defect Template-Collection Group1");
	}
}


LRESULT CTemplateTool::OnGenerateTemplateResult(WPARAM wparam, LPARAM lparam)
{
	int result = (int)wparam;
	if (1 == result)
		::AfxMessageBox("generate offset template success!");
	else if (-1 == result)
		::AfxMessageBox("generate offset template failed!");
	else if (2 == result)
		::AfxMessageBox("generate gain template success!");
	else if (-2 == result)
		::AfxMessageBox("generate gain template failed!");
	else if (3 == result)
		::AfxMessageBox("generate defect template success!");
	else if (-3 == result)
		::AfxMessageBox("generate defect template failed!");
	else if (4 == result)
		::AfxMessageBox("defect acq light image success!");
	else if (-4 == result)
		::AfxMessageBox("defect acq light image failed!");
	else
		::AfxMessageBox("generate template other error!");
	return S_OK;
}

void CTemplateTool::OnBnClickedBtnTemplateGenerate()
{
	// TODO: 在此添加控件通知处理程序代码
	if (theDemo == NULL) {
		::AfxMessageBox("err:theDoc is NULL!");
		return;
	}
	if (theDemo->m_pFpd == NULL) {
		::AfxMessageBox("err:theDoc->m_pFpd is NULL!");
		return;
	}
	if (!theDemo->IsOpen) {
		::AfxMessageBox("err:Fpd is not connected!");
		return;
	}
	// 获取一组单选框选中状态
	int nID = GetCheckedRadioButton(IDC_RADIO_GAIN, IDC_RADIO_DEFECT);
	if (nID == IDC_RADIO_OFFSET) {
		if (enumTemplateType != OFFSET_TEMPLATE_MODE) enumTemplateType = OFFSET_TEMPLATE_MODE;
		printf("OnBnClickedTemplateGenerate:OFFSET_TEMPLATE_MODE\n");
	}
	else if (nID == IDC_RADIO_GAIN) {
		if (enumTemplateType != GAIN_TEMPLATE_MODE) enumTemplateType = GAIN_TEMPLATE_MODE;
		printf("OnBnClickedTemplateGenerate:GAIN_TEMPLATE_MODE\n");
	}
	else if (nID == IDC_RADIO_DEFECT) {
		if (0 == m_ungroupid) {
			if (enumTemplateType != DEFECT_ACQ_GROUP1) enumTemplateType = DEFECT_ACQ_GROUP1;
			printf("OnBnClickedTemplateGenerate:DEFECT_ACQ_GROUP1\n");
		}
		else if (1 == m_ungroupid) {
			if (enumTemplateType != DEFECT_ACQ_GROUP2) enumTemplateType = DEFECT_ACQ_GROUP2;
			printf("OnBnClickedTemplateGenerate:DEFECT_ACQ_GROUP2\n");
		}
		else if (2 == m_ungroupid) {
			if (enumTemplateType != DEFECT_ACQ_AND_TEMPLATE) enumTemplateType = DEFECT_ACQ_AND_TEMPLATE;
			printf("OnBnClickedTemplateGenerate:DEFECT_ACQ_AND_TEMPLATE\n");
		}
		else {
			::AfxMessageBox("err:m_ungroupid is error!");
			return;
		}
	}
	else {
		::AfxMessageBox("err:Control nID is not Exits!");
		return;
	}
	///////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	// 开始采图并生成模板
	int ret = -1;
	if (enumTemplateType == OFFSET_TEMPLATE_MODE) {
		ret = DoOffsetTemp();
	}
	else if (enumTemplateType == GAIN_TEMPLATE_MODE) {
		// 调节电压
		if (MessageBoxEx(NULL, "请确定剂量:正常电压+正常毫安秒?", "亮场图", MB_ICONINFORMATION | MB_YESNO, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) == IDNO)
		{
			return; //注意无返回值
		}
		ret = DoGainTemp();
	}
	else if (enumTemplateType == DEFECT_ACQ_GROUP1 ||
		enumTemplateType == DEFECT_ACQ_GROUP2 ||
		enumTemplateType == DEFECT_ACQ_AND_TEMPLATE)
	{
		if (enumTemplateType == DEFECT_ACQ_GROUP1) {
			if (MessageBoxEx(NULL, "请确定剂量:正常电压+正常毫安秒的10%?", "第一组亮场图", MB_ICONINFORMATION | MB_YESNO, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) == IDNO)
			{
				return; //注意无返回值
			}
			ret =DoDefectGroup1();
		}
		else if (enumTemplateType == DEFECT_ACQ_GROUP2) {
			if (MessageBoxEx(NULL, "请确定剂量:正常电压+正常毫安秒的50%?", "第二组亮场图", MB_ICONINFORMATION | MB_YESNO, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) == IDNO)
			{
				return; //注意无返回值
			}
			ret =DoDefectGroup2();
		}
		else /*if (enumTemplateType == DEFECT_ACQ_AND_TEMPLATE) */{
			if (MessageBoxEx(NULL, "请确定剂量:正常电压+正常毫安秒?", "第三组亮场图&生成模板", MB_ICONINFORMATION | MB_YESNO, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) == IDNO)
			{
				return; //注意无返回值
			}	
			ret =DoDefectGroup3();
		}
	}
	//
	if (ret != HBI_SUCCSS) {
		printf("err:Generate Template failed!\n");
	}
	else {// 成功后,下一步
		if (enumTemplateType == OFFSET_TEMPLATE_MODE) {
			printf("DO OFFSET_TEMPLATE_MODE SUCCESS!\n");
		}
		else if (enumTemplateType == GAIN_TEMPLATE_MODE) {
			printf("DO GAIN_TEMPLATE_MODE SUCCESS!\n");
		}
	else if (enumTemplateType == DEFECT_ACQ_GROUP1 ||
		enumTemplateType == DEFECT_ACQ_GROUP2 ||
		enumTemplateType == DEFECT_ACQ_AND_TEMPLATE)
		{
			if (enumTemplateType == DEFECT_ACQ_GROUP1) {
				printf("DO DEFECT_ACQ_GROUP1 SUCCESS!\n");
				((CButton *)GetDlgItem(IDC_BTN_TEMPLATE_GENERATE))->SetWindowTextA(_T("Defect Template-Collection Group2"));
			}
			else if (enumTemplateType == DEFECT_ACQ_GROUP2) {
				printf("DO DEFECT_ACQ_GROUP2 SUCCESS!\n");
				((CButton *)GetDlgItem(IDC_BTN_TEMPLATE_GENERATE))->SetWindowTextA(_T("Defect Template-Collection Group3 And Generate"));
			}
			else /*if (enumTemplateType == DEFECT_ACQ_AND_TEMPLATE)*/ {
				printf("DO DEFECT_ACQ_AND_TEMPLATE SUCCESS!\n");
				m_ungroupid = 0; // 重置
				((CButton *)GetDlgItem(IDC_BTN_TEMPLATE_GENERATE))->SetWindowTextA(_T("Defect Template-Collection Group1"));
			}
			// ADD +1
			m_ungroupid++;
		}
	}
}

int CTemplateTool::DoOffsetTemp()
{
	if (theDemo == NULL) {
		::AfxMessageBox("err:theDemo is NULL!");
		return -1;
	}
	if (!theDemo->m_pFpd) {
		::AfxMessageBox("warning:请初始化动态库!");
		return -1;
	}
	if (!theDemo->IsOpen) {
		::AfxMessageBox("warning:请连接平板!");
		return -1;
	}
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(false);
	EnumIMAGE_ACQ_MODE enumTemplateType = OFFSET_TEMPLATE_MODE;
	int ret = HBI_ImmediateGenerateTemplate(theDemo->m_pFpd, enumTemplateType);
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(true);
	return ret;
}

/*
    分3步：
	1、采集亮场图；
	2、生成模板；
	3、下载到固件；
*/
int CTemplateTool::DoGainTemp()
{
	if (theDemo == NULL) {
		::AfxMessageBox("err:theDemo is NULL!");
		return -1;
	}
	if (!theDemo->m_pFpd) {
		::AfxMessageBox("warning:请初始化动态库!");
		return -1;
	}
	if (!theDemo->IsOpen) {
		::AfxMessageBox("warning:请连接平板!");
		return -1;
	}
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(false);
	EnumIMAGE_ACQ_MODE enumTemplateType = GAIN_TEMPLATE_MODE;
	int ret = HBI_ImmediateGenerateTemplate(theDemo->m_pFpd, enumTemplateType);
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(true);
	return ret;
}

int CTemplateTool::DoDefectTemp()
{
	if (theDemo == NULL) {
		::AfxMessageBox("err:theDemo is NULL!");
		return -1;
	}
	if (!theDemo->m_pFpd) {
		::AfxMessageBox("warning:请初始化动态库!");
		return -1;
	}
	if (!theDemo->IsOpen) {
		::AfxMessageBox("warning:请连接平板!");
		return -1;
	}
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(false);
	int result = -1;
	EnumIMAGE_ACQ_MODE enumTemplateType = DEFECT_ACQ_GROUP1;
	for (int i = 0; i < 3; i++) {
		if (i == 0) {
			enumTemplateType = DEFECT_ACQ_GROUP1;
			if (MessageBoxEx(NULL, "请确定剂量:正常电压+正常毫安秒的10%?", "第一组亮场图", 
				MB_ICONINFORMATION | MB_YESNO, 
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) == IDNO)
			{
				printf("取消采集第一组亮场!\n");
				break;
			}
		}
		else if (i == 1) {
			enumTemplateType = DEFECT_ACQ_GROUP2;
			if (MessageBoxEx(NULL, "请确定剂量:正常电压+正常毫安秒的50%?", "第二组亮场图", 
				MB_ICONINFORMATION | MB_YESNO, 
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) == IDNO)
			{
				printf("取消采集第二组亮场!\n");
				break;
			}
		}
		else {
			enumTemplateType = DEFECT_ACQ_AND_TEMPLATE;
			if (MessageBoxEx(NULL, "请确定剂量:正常电压+正常毫安秒?", "第三组亮场图&生成模板", 
				MB_ICONINFORMATION | MB_YESNO,
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) == IDNO)
			{
				printf("取消采集第三组亮场!\n");
				break;
			}
		}
		// 调用接口
		result = HBI_ImmediateGenerateTemplate(theDemo->m_pFpd, enumTemplateType);
		if (result != HBI_SUCCSS) {
			if (i == 0)
				printf("采集第一组亮场失败!\n");
			else if (i ==1)
				printf("采集第二组亮场失败!\n");
			else
				printf("采集第三组亮场并生成模板失败!\n");
			break;
		}
	}
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(true);
	return result;
}

int CTemplateTool::DoDefectGroup1()
{
	if (theDemo == NULL) {
		::AfxMessageBox("err:theDemo is NULL!");
		return -1;
	}
	if (!theDemo->m_pFpd) {
		::AfxMessageBox("warning:请初始化动态库!");
		return -1;
	}
	if (!theDemo->IsOpen) {
		::AfxMessageBox("warning:请连接平板!");
		return -1;
	}
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(false);
	EnumIMAGE_ACQ_MODE enumTemplateType = DEFECT_ACQ_GROUP1;
	int ret = HBI_ImmediateGenerateTemplate(theDemo->m_pFpd, enumTemplateType);
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(true);
	return ret;
}

int CTemplateTool::DoDefectGroup2()
{
	if (theDemo == NULL) {
		::AfxMessageBox("err:theDemo is NULL!");
		return -1;
	}
	if (!theDemo->m_pFpd) {
		::AfxMessageBox("warning:请初始化动态库!");
		return -1;
	}
	if (!theDemo->IsOpen) {
		::AfxMessageBox("warning:请连接平板!");
		return -1;
	}
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(false);
	EnumIMAGE_ACQ_MODE enumTemplateType = DEFECT_ACQ_GROUP2;
	int ret = HBI_ImmediateGenerateTemplate(theDemo->m_pFpd, enumTemplateType);
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(true);
	return ret;
}

int CTemplateTool::DoDefectGroup3()
{
	if (theDemo == NULL) {
		::AfxMessageBox("err:theDemo is NULL!");
		return -1;
	}
	if (!theDemo->m_pFpd) {
		::AfxMessageBox("warning:请初始化动态库!");
		return -1;
	}
	if (!theDemo->IsOpen) {
		::AfxMessageBox("warning:请连接平板!");
		return -1;
	}
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(false);
	EnumIMAGE_ACQ_MODE enumTemplateType = DEFECT_ACQ_AND_TEMPLATE;
	int ret = HBI_ImmediateGenerateTemplate(theDemo->m_pFpd, enumTemplateType);
	GetDlgItem(IDC_BTN_TEMPLATE_GENERATE)->EnableWindow(true);
	return ret;
}