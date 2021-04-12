
// HB_SDK_DEMO2008.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HB_SDK_DEMO2008.h"
#include "HB_SDK_DEMO2008Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHB_SDK_DEMO2008App

BEGIN_MESSAGE_MAP(CHB_SDK_DEMO2008App, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CHB_SDK_DEMO2008App construction

CHB_SDK_DEMO2008App::CHB_SDK_DEMO2008App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CHB_SDK_DEMO2008App object

CHB_SDK_DEMO2008App theApp;


// CHB_SDK_DEMO2008App initialization

BOOL CHB_SDK_DEMO2008App::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	/*
	* 控制台输出
	*/ 
	AllocConsole();                             // 开辟控制台
	SetConsoleTitle(_T("HBI_SDK_DEMO2008"));    // 设置控制台窗口标题
	freopen("CONOUT$", "w", stdout);            // 重定向输出

	CHB_SDK_DEMO2008Dlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CHB_SDK_DEMO2008App::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	/*
	* 关闭控制台
	*/
	HWND hwndFound = FindWindow(NULL, _T("HBI_SDK_DEMO2008"));
	if (hwndFound != NULL) {
		FreeConsole();
		hwndFound = NULL;
	}

	return CWinAppEx::ExitInstance();
}