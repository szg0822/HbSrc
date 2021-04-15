
// HB_SDK_DEMO2008Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "HB_SDK_DEMO2008.h"
#include "HB_SDK_DEMO2008Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CHB_SDK_DEMO2008Dlg* theDemo = NULL;

enum EFpdStatusType {
	FPD_STATUS_CONN = 0x00,
	FPD_STATUS_DISCONN,
	FPD_STATUS_READY,
	FPD_STATUS_BUSY,
	FPD_STATUS_PREPARE
};

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CHB_SDK_DEMO2008Dlg dialog




CHB_SDK_DEMO2008Dlg::CHB_SDK_DEMO2008Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHB_SDK_DEMO2008Dlg::IDD, pParent)
	, m_uFrameNum(0)
	, m_uDiscardNum(0)
	, m_uAcqInterval(0)
	, m_uPrepareTm(0)
	{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//
	theDemo = this;
	m_pFpd  = NULL;
	m_pLastRegCfg = NULL;
	m_imgW = m_imgH = 3072;
	fpd_base = NULL;
	pIplimage = NULL;
	desIplimage = NULL;
	// ��ʼ��DLL��Դ
	m_pFpd = HBI_Init();

	m_IsExitsSubWnd = 0;
	pic_factor = 1.0;
	bShowPic = true;
	pPicWnd = NULL;
	//
	m_hEvent = NULL;
	m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hEvent == NULL) {
		printf("err:%s", "CreateEvent failed!");
	}
	//
	RunFlag = 1;
	m_Threadhdl = (HANDLE)_beginthreadex(NULL, 0, &ShowThreadProcFun, (void *)this, CREATE_SUSPENDED, &m_uThreadFunID);
	//
	IsOpen = false;
}

CHB_SDK_DEMO2008Dlg::~CHB_SDK_DEMO2008Dlg()
{
	CloseShowThread();
	// �ͷ�DLL��Դ
	if (m_pFpd != NULL) {
		HBI_Destroy(m_pFpd);
		m_pFpd = NULL;
	}
	//
	if (fpd_base != NULL) {
		delete fpd_base;
		fpd_base = NULL;
	}
	//
	if (m_pLastRegCfg != NULL) {
		delete m_pLastRegCfg;
		m_pLastRegCfg = NULL;
	}
	if (pIplimage != NULL) {
		cvReleaseImage(&pIplimage);
		pIplimage = NULL;
	}
	if (desIplimage != NULL) {
		cvReleaseImage(&desIplimage);
		desIplimage = NULL;
	}
	// �ر�Opencv����
	if (m_IsExitsSubWnd == 1) {
		cvDestroyWindow(MY_WINDOW_NAME);
	}
	//
	IsOpen = false;
}

void CHB_SDK_DEMO2008Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_MAX_FRAME_SUM, m_ctlSpinAqcSum);
	DDX_Control(pDX, IDC_SPIN_DISCARD_FRAME_NUM, m_ctlSpinDiscardNum);
	DDX_Text(pDX, IDC_EDIT_MAX_FRAME_SUM, m_uFrameNum);
	DDV_MinMaxUInt(pDX, m_uFrameNum, 0, 3000);
	DDX_Text(pDX, IDC_EDIT_DISCARD_FRAME_NUM, m_uDiscardNum);
	DDV_MinMaxUInt(pDX, m_uDiscardNum, 0, 10);
	DDX_Control(pDX, IDI_ICON_DISCONNECT, m_ctlConnStatus);
	DDX_Text(pDX, IDC_EDIT_LIVE_TIME, m_uAcqInterval);
	DDX_Text(pDX, IDC_EDIT_PREPARE_DELAY_TIME, m_uPrepareTm);
}

BEGIN_MESSAGE_MAP(CHB_SDK_DEMO2008Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_APP_ABOUT, &CHB_SDK_DEMO2008Dlg::OnAppAbout)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CONN, &CHB_SDK_DEMO2008Dlg::OnBnClickedBtnConn)
	ON_BN_CLICKED(IDC_BTN_DISCONN, &CHB_SDK_DEMO2008Dlg::OnBnClickedBtnDisconn)
	ON_BN_CLICKED(IDC_BTN_LIVE_ACQ, &CHB_SDK_DEMO2008Dlg::OnBnClickedBtnLiveAcq)
	ON_BN_CLICKED(IDC_BTN_STOP_LIVE_ACQ, &CHB_SDK_DEMO2008Dlg::OnBnClickedBtnStopLiveAcq)
	ON_BN_CLICKED(IDC_RADIO_SHOW_PIC, &CHB_SDK_DEMO2008Dlg::OnBnClickedRadioShowPic)
	ON_BN_CLICKED(IDC_RADIO_SAVE_PIC, &CHB_SDK_DEMO2008Dlg::OnBnClickedRadioSavePic)
	ON_COMMAND(ID_FILE_GENERATETEMPLATE, &CHB_SDK_DEMO2008Dlg::OnFileTemplate)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_SHOT, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonSingleShot)
	ON_BN_CLICKED(IDC_BUTTON_FIRMWARE_VER, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonFirmwareVer)
	ON_BN_CLICKED(IDC_BUTTON_SOFTWARE_VER, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonSoftwareVer)
	ON_BN_CLICKED(IDC_BTN_GET_IMAGE_PROPERTY, &CHB_SDK_DEMO2008Dlg::OnBnClickedBtnGetImageProperty)
	ON_BN_CLICKED(IDC_BUTTON_GET_CONFIG, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonGetConfig)
	ON_BN_CLICKED(IDC_BTN_SET_TRIGGER_CORRECTION, &CHB_SDK_DEMO2008Dlg::OnBnClickedBtnSetTriggerCorrection)
	ON_BN_CLICKED(IDC_BTN_SET_TRIGGER_MODE, &CHB_SDK_DEMO2008Dlg::OnBnClickedBtnSetTriggerMode)
	ON_BN_CLICKED(IDC_BTN_FIRMWARE_CORRECT_ENABLE, &CHB_SDK_DEMO2008Dlg::OnBnClickedBtnCorrectEnable)
	ON_BN_CLICKED(IDC_BUTTON_SET_GAIN_MODE, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonSetGainMode)
	ON_BN_CLICKED(IDC_BUTTON_GET_GAIN_MODE, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonGetGainMode)
	ON_BN_CLICKED(IDC_BUTTON_SET_PGA_LEVEL, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonGetLiveAcqTime)
	ON_BN_CLICKED(IDC_BUTTON_SET_PREPARE_TIME, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonSetPrepareTime)
	ON_BN_CLICKED(IDC_BUTTON_SET_LIVE_ACQUISITION_TM, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonSetLiveAcquisitionTm)
	ON_BN_CLICKED(IDC_BUTTON_GET_SERIAL_NUMBER, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonLiveAcquisitionTm)
	ON_MESSAGE(WM_USER_CONNECT_STATUS_ICON, &CHB_SDK_DEMO2008Dlg::OnUpdateConnectStatus)
	ON_MESSAGE(WM_USER_CURR_CONTROL_DATA, &CHB_SDK_DEMO2008Dlg::OnUpdateCurControlData)
	ON_BN_CLICKED(IDC_BUTTON_PREPARE, &CHB_SDK_DEMO2008Dlg::OnBnClickedButtonPrepare)
END_MESSAGE_MAP()


// CHB_SDK_DEMO2008Dlg message handlers

BOOL CHB_SDK_DEMO2008Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	InitCtrlData(); // ��ʼ���ؼ�
	
	// ��ȡ��ǰģ���·��
	memset(m_path, 0x00, MAX_PATH);
	GetModuleFileName(NULL, m_path, MAX_PATH);

	// 
	((CButton *)GetDlgItem(IDC_RADIO_SHOW_PIC))->SetCheck(TRUE);

	// ��ǰִ���ļ�Ŀ¼
	PathRemoveFileSpec(m_path);

	m_pLastRegCfg = new RegCfgInfo;
	//m_pRegRomCfg  = new RegCfgInfo;
	fpd_base = new fpd_base_cfg_st;
	if (fpd_base != NULL) {
		// ��ʼ��
		char buff[32] = { 0 };
		memset(buff, 0x00, 32);
		sprintf(buff, "%s", fpd_base->DstIP);
		SetDlgItemText(IDC_IPADDRESS_REMOTE, buff);
		memset(buff, 0x00, 32);
		sprintf(buff, "%u", fpd_base->DstPort);
		SetDlgItemText(IDC_EDIT_REMOTE_PORT, buff);
		//
		memset(buff, 0x00, 32);
		sprintf(buff, "%s", fpd_base->SrcIP);
		SetDlgItemText(IDC_IPADDRESS_LOCAL, buff);
		memset(buff, 0x00, 32);
		sprintf(buff, "%u", fpd_base->SrcPort);
		SetDlgItemText(IDC_EDIT_LOCAL_PORT, buff);

		((CComboBox *)GetDlgItem(IDC_COMBO_TRIGGER_MODE))->SetCurSel(fpd_base->trigger_mode);
		((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_OFFSET))->SetCurSel(fpd_base->offset_enable);
		((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_GAIN))->SetCurSel(fpd_base->gain_enable);
		((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_DEFECT))->SetCurSel(fpd_base->defect_enable);
		((CEdit *)GetDlgItem(IDC_EDIT_MAX_FRAME_SUM))->SetWindowTextA(_T("0"));
		((CEdit *)GetDlgItem(IDC_EDIT_DISCARD_FRAME_NUM))->SetWindowTextA(_T("0"));
		((CComboBox *)GetDlgItem(IDC_COMBO_PGA_LEVEL))->SetCurSel(6);
		fpd_base->PRINT_FPD_INFO();
	}

	this->UpdateData(FALSE);

	//��ȡ��ʾ����
	pPicWnd = GetDlgItem(IDC_STATIC_PICTURE_ZONE);
	if (pPicWnd != NULL) {
		pPicWnd->GetClientRect(&m_PicRect);
		pic_width  = m_PicRect.Width();
		pic_height = m_PicRect.Height();

//		/* ����Opencv���� */
//#if OPENCV210
//		m_IsExitsSubWnd = cvNamedWindow(MY_WINDOW_NAME, WINDOW_AUTOSIZE);
//#else
//		m_IsExitsSubWnd = cvNamedWindow(MY_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
//#endif
//		if (m_IsExitsSubWnd) {
//			HWND hWnd = (HWND)cvGetWindowHandle(MY_WINDOW_NAME);
//			HWND hParent = ::GetParent(hWnd);
//			::SetParent(hWnd, pWnd->m_hWnd); // ��ͼƬ�ؼ�λ������
		int WIDTH  = m_imgW;
		int HEIGHT = m_imgH;

		// ���ű���
		if ((pic_width > WIDTH) && (pic_height > HEIGHT))
		{		
			pic_factor = 1.0;
		}
		else
		{
			int iTemp = (int)((double)((double)pic_width / (double)pic_height) * 100);   //ˢͼ����ĳ����
			int iTempBK = (int)((double)((double)WIDTH / (double)HEIGHT) * 100); //ͼƬ�ĳ����
			if (iTemp < iTempBK) //����ͼ�Ŀ�ȒO��ˢͼȥ�Ŀ�� ���ݳ���ȼ����µĸ߶�  �ó��µ�ˢͼ����
			{
				int iNewPicHight = (pic_width * HEIGHT) / WIDTH;
				// ���ű���
				pic_factor = (double)pic_width / (double)WIDTH;
			}
			else //�����¸߶���Ϣˢͼ
			{
				int iNewWidth = (pic_height * WIDTH) / HEIGHT;
				// ���ű���
				pic_factor = (double)pic_height / (double)HEIGHT;
			}
		}
//			cvResizeWindow(MY_WINDOW_NAME, pic_width, pic_height);
//			::ShowWindow(hParent, SW_HIDE);
		// ������������
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 2.0, 2.0, 1, 2, 8);
	}
    // �����߳�
	if (m_Threadhdl != NULL)  {
		ResumeThread(m_Threadhdl);
	}
	else {
		printf("%s", "start ShowThreadProcFun failed!");
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHB_SDK_DEMO2008Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHB_SDK_DEMO2008Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//if (pIplimage != NULL) {	
		//	//Mat imageROI;
		//	//
		//	CDC *pDC = pPicWnd->GetDC();//IDC_STATIC_PICΪͼƬ�ؼ���ID 
		//	if (pDC != NULL) {
		//		HDC hDC = pDC->GetSafeHdc();
		//		CvvImage cimg;
		//		cimg.CopyOf(pIplimage);
		//		cimg.DrawToHDC(hDC, &m_PicRect);
		//		ReleaseDC(pDC);
		//	}
		//	//ROIAdjust(m_PicRect);
		//	//imageROI = m_MapImg(Rect(m_ROIX, m_ROIY, m_PicRect.Width()/m_Scale, m_PicRect.Height()/m_Scale));
		//	//IplImage *pImg = &IplImage(imageROI);
		//	//
		//	//CvvImage cimg;
		//	//cimg.CopyOf(pIplimage);
		//	//cimg.DrawToHDC(hDC, &m_PicRect);
		//	//ReleaseDC(pDC);
		//	//CvvImage &img=pDoc->m_img;
		//	//HDC hDC=pDC->GetSafeHdc();
		//	//CRect rect;
		//	//GetClientRect(&rect);
		//	//CRect rc(0,0,img.Width(),img.Height());
		//	//img.DrawToHDC(hDC,&rc);
		//}

		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHB_SDK_DEMO2008Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CHB_SDK_DEMO2008Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	////if (nCtlColor == CTLCOLOR_STATIC) {
	////	if (pWnd->GetDlgCtrlID() == IDC_STATIC_LIVE_ACQ_TIME) {
	////		pDC->SetBkMode(TRANSPARENT);
	////		pDC->SetTextColor(RGB(255, 0, 0));
	////	}
	////}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void CHB_SDK_DEMO2008Dlg::OnAppAbout()
{
	// TODO: Add your command handler code here
	CAboutDlg AboutDlg;
	AboutDlg.DoModal();
}



void CHB_SDK_DEMO2008Dlg::OnFileTemplate()
{
	// TODO: �ڴ���������������
	if (m_pFpd == NULL) {
		::AfxMessageBox(_T("HBI_Init failed!"));
		return;
	}
	//
	if (!IsOpen) {
		::AfxMessageBox(_T("warning:fpd is disconnect!"));
		return;
	}
	//
	CTemplateTool dlg;
	dlg.DoModal();
}


void CHB_SDK_DEMO2008Dlg::InitCtrlData()
{
	//������ת��ť��λ�÷�Χ
	m_ctlSpinAqcSum.SetRange(0, 3000);
	//������ת��ť�ĵ�ǰλ��
	m_ctlSpinAqcSum.SetPos(0);
	//������ת��ť�ĵ�ǰ����
	m_ctlSpinAqcSum.SetBase(1);
	//������ת��ť�Ļ�鴰��
	m_ctlSpinAqcSum.SetBuddy(GetDlgItem(IDC_EDIT_MAX_FRAME_SUM));

	//������ת��ť��λ�÷�Χ
	m_ctlSpinDiscardNum.SetRange(0, 10);
	//������ת��ť�ĵ�ǰλ��
	m_ctlSpinDiscardNum.SetPos(0);
	//������ת��ť�ĵ�ǰ����
	m_ctlSpinDiscardNum.SetBase(1);
	//������ת��ť�Ļ�鴰��
	m_ctlSpinDiscardNum.SetBuddy(GetDlgItem(IDC_EDIT_DISCARD_FRAME_NUM));

	m_hConnIco    = ::AfxGetApp()->LoadIconA(IDI_ICON_CONNECT);
	m_hDisConnIco = ::AfxGetApp()->LoadIconA(IDI_ICON_DISCONNECT);
	m_hReadyIco   = ::AfxGetApp()->LoadIconA(IDI_ICON_READY);
	m_hbusyIco    = ::AfxGetApp()->LoadIconA(IDI_ICON_BUSY);
	m_hprepareIco = ::AfxGetApp()->LoadIconA(IDI_ICON_PREPARE);
}


int CHB_SDK_DEMO2008Dlg::GetPGA(unsigned short usValue)
{
	unsigned short gainMode = ((usValue & 0xff) << 8) | ((usValue >> 8) & 0xff);
	int nPGA = (gainMode >> 10) & 0x3f;
	if (nPGA == 0x02) return 1;
	else if (nPGA == 0x04) return 2;
	else if (nPGA == 0x08) return 3;
	else if (nPGA == 0x0c) return 4;
	else if (nPGA == 0x10) return 5;
	else if (nPGA == 0x18) return 6;
	else if (nPGA == 0x3e) return 7;
	else return 0;
}


void CHB_SDK_DEMO2008Dlg::AutoResize()
{
	int WIDTH  = m_imgW;
	int HEIGHT = m_imgH;

	// ���ű���
	if ((pic_width > WIDTH) && (pic_height > HEIGHT))
	{
		pic_factor = 1.0;
	}
	else
	{
		int iTemp = (int)((double)((double)pic_width / (double)pic_height) * 100);   //ˢͼ����ĳ����
		int iTempBK = (int)((double)((double)WIDTH / (double)HEIGHT) * 100); //ͼƬ�ĳ����
		if (iTemp < iTempBK) //����ͼ�Ŀ�ȒO��ˢͼȥ�Ŀ�� ���ݳ���ȼ����µĸ߶�  �ó��µ�ˢͼ����
		{
			int iNewPicHight = (pic_width * HEIGHT) / WIDTH;
			// ���ű���
			pic_factor = (double)pic_width / (double)WIDTH;
		}
		else //�����¸߶���Ϣˢͼ
		{
			int iNewWidth = (pic_height * WIDTH) / HEIGHT;
			// ���ű���
			pic_factor = (double)pic_height / (double)HEIGHT;
		}
	}
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedBtnConn()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_pFpd == NULL) {
		::AfxMessageBox(_T("HBI_Init failed!"));
		return;
	}

	if (fpd_base == NULL) {
		::AfxMessageBox("fpd_base is NULL!");
		return;
	}

	// Զ��ip
	CString strstr = _T("");
	GetDlgItemText(IDC_IPADDRESS_REMOTE, strstr);
	memset(fpd_base->DstIP, 0x00, 16);
	memcpy(fpd_base->DstIP, strstr.GetBuffer(15), 15);
	strstr.ReleaseBuffer();

	// Զ��port
	GetDlgItemText(IDC_EDIT_REMOTE_PORT, strstr);
	fpd_base->DstPort = (unsigned short)atol(strstr.GetBuffer());
	strstr.ReleaseBuffer();

	// ����ip	
	GetDlgItemText(IDC_IPADDRESS_LOCAL, strstr);
	memset(fpd_base->SrcIP, 0x00, 16);
	memcpy(fpd_base->SrcIP, strstr.GetBuffer(), 15);
	strstr.ReleaseBuffer();

	// ���ض˿�
	GetDlgItemText(IDC_EDIT_LOCAL_PORT, strstr);
	fpd_base->SrcPort = (unsigned short)atol(strstr.GetBuffer());
	strstr.ReleaseBuffer();

	/* 
	* ע��ص�����
	*/
#if USER_WEI_MING
	int ret = HBI_RegEventCallBackFun(m_pFpd, handleCommandEvent, (void *)this);
#else
	int ret = HBI_RegEventCallBackFun(m_pFpd, handleCommandEvent);
#endif

	/*
	* ����ƽ��̽����
	*/
	//////char szFpdIp[16]    = "192.168.10.40";
	//////char szlocalIp[16]  = "192.168.10.20";
	//////unsigned short fpdPort   = 32897;
	//////unsigned short localPort = 32896;
	int timeout = 20; // ms
	/////ret = HBI_ConnectDetector(m_pFpd, szFpdIp, fpdPort, szlocalIp, localPort, timeout);
	ret = HBI_ConnectDetector(m_pFpd, fpd_base->DstIP, fpd_base->DstPort, fpd_base->SrcIP, fpd_base->SrcPort, timeout);
	if (ret != 0) {
		IsOpen = false;
		::AfxMessageBox(_T("����ʧ��!"));
	}
	else {
		printf("HBI_ConnectDetector success!\n");
		conn_button_status();
		IsOpen = true;
	}
}

void CHB_SDK_DEMO2008Dlg::OnBnClickedBtnDisconn()
{
	// TODO: Add your control notification handler code here
	if (m_pFpd != NULL) {
		HBI_DisConnectDetector(m_pFpd);
		//
		printf("HBI_DisConnectDetector success!\n");
		//disconn_button_status();
	}
	else {
		printf("disconnect failed!\n");
	}
	PostMessage(WM_USER_CONNECT_STATUS_ICON, (WPARAM)FPD_STATUS_DISCONN, (LPARAM)0);
}

void CHB_SDK_DEMO2008Dlg::OnBnClickedBtnLiveAcq()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	printf("OnBnClickedBtnLiveAcq!\n");
	if (m_pFpd == NULL) {
		::AfxMessageBox(_T("err:m_pFpd is NULL!"));
		return;
	}
	//
	aqc_mode.aqc_mode  = DYNAMIC_ACQ_DEFAULT_MODE;
	aqc_mode.nframesum = m_uFrameNum + m_uDiscardNum ;
	aqc_mode.nframeid  = 0;
	aqc_mode.ngroupno  = 0;
	aqc_mode.ndiscard  = m_uDiscardNum;

	printf("DoLiveAcq:[_curmode]=%d,[_acqMaxNum]=%d,[_discardNum]=%d,[_groupNum]=%d\n", aqc_mode.aqc_mode, aqc_mode.nframesum, aqc_mode.ndiscard, aqc_mode.ngroupno);
	int ret = HBI_LiveAcquisition(m_pFpd, aqc_mode);
	if (ret == 0)
		printf("mode=%d,sum=%d,num=%d,group=%d,Do Live Acquisition succss!\n", aqc_mode.aqc_mode, aqc_mode.nframesum, aqc_mode.ndiscard, aqc_mode.ngroupno);
	else {
		printf("mode=%d,sum=%d,num=%d,group=%d,Do Live Acquisition failed!\n", aqc_mode.aqc_mode, aqc_mode.nframesum, aqc_mode.ndiscard, aqc_mode.ngroupno);
	}
}

void CHB_SDK_DEMO2008Dlg::OnBnClickedBtnStopLiveAcq()
{
	// TODO: Add your control notification handler code here
	printf("OnBnClickedBtnStopLiveAcq!\n");
	if (m_pFpd == NULL) {
		::AfxMessageBox(_T("err:m_pFpd is NULL!"));
		return;
	}
	//
	int ret = HBI_StopAcquisition(m_pFpd);
	if (ret == 0)
		printf("Stop Live Acquisition succss!\n");
	else {
		printf("Stop Live Acquisition failed!\n");
	}
}


void CHB_SDK_DEMO2008Dlg::conn_button_status()
{
	((CButton *)GetDlgItem(IDC_BTN_CONN))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BTN_DISCONN))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BTN_LIVE_ACQ))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BTN_STOP_LIVE_ACQ))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BUTTON_FIRMWARE_VER))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BTN_GET_IMAGE_PROPERTY))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BUTTON_SINGLE_SHOT))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BTN_SET_TRIGGER_MODE))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BUTTON_SET_GAIN_MODE))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BUTTON_SET_PGA_LEVEL))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BUTTON_SET_LIVE_ACQUISITION_TM))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BUTTON_GET_CONFIG))->EnableWindow(TRUE);
    ((CButton *)GetDlgItem(IDC_BTN_SET_TRIGGER_CORRECTION))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BTN_FIRMWARE_CORRECT_ENABLE))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BUTTON_GET_GAIN_MODE))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BUTTON_SET_PREPARE_TIME))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BUTTON_GET_SERIAL_NUMBER))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BUTTON_PREPARE))->EnableWindow(TRUE);
	if (!IsOpen)	IsOpen = true;
}


void CHB_SDK_DEMO2008Dlg::disconn_button_status()
{
	((CButton *)GetDlgItem(IDC_BTN_CONN))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_BTN_DISCONN))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BTN_LIVE_ACQ))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BTN_STOP_LIVE_ACQ))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BUTTON_FIRMWARE_VER))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BTN_GET_IMAGE_PROPERTY))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BUTTON_SINGLE_SHOT))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BTN_SET_TRIGGER_MODE))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BUTTON_SET_GAIN_MODE))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BUTTON_SET_PGA_LEVEL))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BUTTON_SET_LIVE_ACQUISITION_TM))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BUTTON_GET_CONFIG))->EnableWindow(FALSE);
    ((CButton *)GetDlgItem(IDC_BTN_SET_TRIGGER_CORRECTION))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BTN_FIRMWARE_CORRECT_ENABLE))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BUTTON_GET_GAIN_MODE))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BUTTON_SET_PREPARE_TIME))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BUTTON_GET_SERIAL_NUMBER))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_BUTTON_PREPARE))->EnableWindow(FALSE);
	if (IsOpen)	IsOpen = false;
}


LRESULT CHB_SDK_DEMO2008Dlg::OnUpdateConnectStatus(WPARAM wparam, LPARAM lparam)
{
	int status = (int)wparam;
	if (status == FPD_STATUS_CONN)
		m_ctlConnStatus.SetIcon(m_hConnIco);
	else if (status == FPD_STATUS_READY)
		m_ctlConnStatus.SetIcon(m_hReadyIco);
	else if (status == FPD_STATUS_BUSY)
		m_ctlConnStatus.SetIcon(m_hbusyIco);
	else if (status == FPD_STATUS_PREPARE)
		m_ctlConnStatus.SetIcon(m_hprepareIco);
	else {/* if (status == FPD_STATUS_DISCONN)*/
		m_ctlConnStatus.SetIcon(m_hDisConnIco);
		disconn_button_status();
	}
	return S_OK;
}


LRESULT CHB_SDK_DEMO2008Dlg::OnUpdateCurControlData(WPARAM wparam, LPARAM lparam)
{
	if (fpd_base != NULL) {
		////////////////////////////////////
		if (fpd_base->trigger_mode >= 1 && fpd_base->trigger_mode <= 7)
			((CComboBox *)GetDlgItem(IDC_COMBO_TRIGGER_MODE))->SetCurSel(fpd_base->trigger_mode);
		else
			((CComboBox *)GetDlgItem(IDC_COMBO_TRIGGER_MODE))->SetCurSel(0);
		////////////////////////////////////
	    if (fpd_base->offset_enable >= 0 && fpd_base->offset_enable <=3)
			((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_OFFSET))->SetCurSel(fpd_base->offset_enable);
		else
			((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_OFFSET))->SetCurSel(0);
		//
		if (fpd_base->gain_enable >= 0 && fpd_base->gain_enable <=2)
			((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_GAIN))->SetCurSel(fpd_base->gain_enable);
		else
			((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_GAIN))->SetCurSel(0);
		//
		if (fpd_base->gain_enable >= 0 && fpd_base->gain_enable <=2)
			((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_DEFECT))->SetCurSel(fpd_base->defect_enable);
		else
			((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_DEFECT))->SetCurSel(0);
		////////////////////////////////////
		CString strstr = _T("");
		////strstr.Format(_T("width=%d,hight=%d,datatype=%d,databit=%d,endian=%d,trigger mode=%d,offset=%d,gain=%d,defect=%d,dummy=%d"),
		////	fpd_base->xxxx,
		////	fpd_base->yyyy,
		////	fpd_base->datatype,
		////	fpd_base->databit,
		////	fpd_base->endian,
		////	fpd_base->trigger_mode,
		////	fpd_base->offset_enable,
		////	fpd_base->gain_enable,
		////	fpd_base->defect_enable,
		////	fpd_base->dummy_enable);
		////((CStatic *)GetDlgItem(IDC_STATIC_FPD_MSG))->SetWindowTextA(strstr);
		////strstr.ReleaseBuffer();
		strstr.Format(_T("%u"), theDemo->m_uAcqInterval);
		((CEdit *)GetDlgItem(IDC_EDIT_LIVE_TIME))->SetWindowTextA(strstr);
		strstr.ReleaseBuffer();
		//
		strstr.Format(_T("%u"), theDemo->m_uPrepareTm);
		((CEdit *)GetDlgItem(IDC_EDIT_PREPARE_DELAY_TIME))->SetWindowTextA(strstr);
		strstr.ReleaseBuffer();
		//
		if (fpd_base->nPGA >= 1 && fpd_base->nPGA <= 7)
			((CComboBox *)GetDlgItem(IDC_COMBO_PGA_LEVEL))->SetCurSel(fpd_base->nPGA);
		else
			((CComboBox *)GetDlgItem(IDC_COMBO_PGA_LEVEL))->SetCurSel(0);
	}
	return S_OK;
}

int CHB_SDK_DEMO2008Dlg::SaveImage(unsigned char *imgbuff, int nbufflen)
{
	/* ̽�����ֱ��ʴ�С */
	int WIDTH  = m_imgW;
	int HEIGHT = m_imgH;
	if ((imgbuff == NULL) || (nbufflen != (WIDTH * HEIGHT * 2))) {
		printf("�����쳣!\n");
		return 0;
	}

	// �������ʾ
	char filename[MAX_PATH];
	memset(filename, 0x00, MAX_PATH);
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	sprintf(filename, "%s\\raw_dir\\%4d%02d%02d%02d%02d%02d%03d.raw",
		m_path,
		sys.wYear,
		sys.wMonth,
		sys.wDay,
		sys.wHour,
		sys.wMinute,
		sys.wSecond,
		sys.wMilliseconds);
	FILE *fp = fopen(filename, "wb");
	if (fp != NULL) {
		fwrite(imgbuff, 1, nbufflen, fp);
		fclose(fp);
		printf("SaveImage:%s success!\n", filename);
		return 1;
	}
	else {
		printf("SaveImage:%s failed!\n", filename);
		return 0;
	}
}


/* 
* ע��ص�����
* һ�������֣�ȫ�ֺ�����̬��Ա������
* ����ʹ���˾�̬��Ա������ȫ�ֺ���Ҳ���ԣ��û�����ʵ�����ʹ�á�
* handleCommandEvent
*/
#if USER_WEI_MING
int CHB_SDK_DEMO2008Dlg::handleCommandEvent(void *pObj, unsigned char command, void *buff, int len, int nId)
	{
	int ret = 0;
	if ((command == ECALLBACK_TYPE_SINGLE_IMAGE) || (command == ECALLBACK_TYPE_MULTIPLE_IMAGE) ||
		(command == ECALLBACK_TYPE_ROM_UPLOAD) || (command == ECALLBACK_TYPE_RAM_UPLOAD)) {
			if (buff == NULL || len == 0) {
				printf("ע��ص����������쳣!\n");
				return ret;
				}
		}

	int status = -1;
	int j = 0;
	ret = 1;
	bool b = false;
	CHB_SDK_DEMO2008Dlg *pDlg = (CHB_SDK_DEMO2008Dlg *)pObj;

	switch (command)
		{
		case ECALLBACK_TYPE_NET_ERR_MSG: // ƽ��״̬������/�Ͽ�/ready/busy
			printf("ECALLBACK_TYPE_NET_ERR_MSG,recode=%d\n", len);
			if (theDemo != NULL) {
				CString strMsg = _T("");
				if (len <= 0 && len >= -7) {
					if (len == 0)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:����δ����!\n");
					else if (len == -1)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:�����쳣!\n");
					else if (len == -2)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:׼��������������������ʧ��!\n");
					else if (len == -3)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:���ճ�ʱ!\n");
					else if (len == -4)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:����ʧ��!\n");
					else if (len == -5)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:�˿ڲ��ɶ�!\n");
					else if (len == -6)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,network card unusual!\n");
					else if (len == -7)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,network card ok!\n");
					status = (int)FPD_STATUS_DISCONN;
				}
				else if (len == 1) { // connect
					printf("ECALLBACK_TYPE_NET_ERR_MSG,��ʼ����!\n");
					status = (int)FPD_STATUS_CONN;
				}
				else if (len == 2) { // ready
					printf("ECALLBACK_TYPE_NET_ERR_MSG,ready!\n");
					status = (int)FPD_STATUS_READY;
				}
				else if (len == 3) { // busy
					printf("ECALLBACK_TYPE_NET_ERR_MSG,busy!\n");
					status = (int)FPD_STATUS_BUSY;
				}
				else if (len == 4) { // prepare
					printf("ECALLBACK_TYPE_NET_ERR_MSG,prepare!\n");
					status = (int)FPD_STATUS_PREPARE;
				}
				else
					printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:other error=%d\n", len);

				// ADD BY MH.YANG 2019/11/12
				if (status != -1) {
					// ����ͼ��
					theDemo->PostMessage(WM_USER_CONNECT_STATUS_ICON, (WPARAM)status, (LPARAM)0);

					// ����״̬��Ϣ
					if (len <= 0 && len >= -6) {
						/*
						* �����Ͽ���Ϣ
						*/ 
						if (theDemo->m_pFpd != NULL) 
							HBI_DisConnectDetector(theDemo->m_pFpd);
					}
				}
			}
			break;
		case ECALLBACK_TYPE_ROM_UPLOAD:/* �������� */ 
			printf("ECALLBACK_TYPE_ROM_UPLOAD:\n");
			if (theDemo->m_pLastRegCfg != NULL) {
				memset(theDemo->m_pLastRegCfg, 0x00, sizeof(RegCfgInfo));
				memcpy(theDemo->m_pLastRegCfg, (unsigned char *)buff, sizeof(RegCfgInfo));

				printf("\tSerial Number:%s\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_cSnNumber);

				if (theDemo->fpd_base != NULL) {
					/*
					* ip and port
					*/
					// �ߵ�λ��Ҫת��
					unsigned short usValue = ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort & 0xff) << 8) | ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort >> 8) & 0xff);
					printf("\tSourceIP:%d.%d.%d.%d:%u\n",
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[0],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[1],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[2],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[3],
						usValue);

					usValue = ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort & 0xff) << 8) | ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort >> 8) & 0xff);
					printf("\tDestIP:%d.%d.%d.%d:%u\n",
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[0],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[1],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[2],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[3],
						usValue);

					j = sprintf(theDemo->fpd_base->DstIP, "%d.%d.%d.%d",
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[0],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[1],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[2],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[3]);
					theDemo->fpd_base->DstIP[j] = '\0';

					j = sprintf(theDemo->fpd_base->SrcIP, "%d.%d.%d.%d",
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[0],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[1],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[2],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[3]);
					theDemo->fpd_base->SrcIP[j] = '\0';

					// �ߵ�λ��Ҫת��
					usValue = ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort & 0xff) << 8) | ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort >> 8) & 0xff);
					theDemo->fpd_base->DstPort = usValue;

					usValue = ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort & 0xff) << 8) | ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort >> 8) & 0xff);
					theDemo->fpd_base->SrcPort = usValue;

					//
					if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x01)
						printf("\tPanelSize:0x%02x,fpd type:4343-140um\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x02)
						printf("\tPanelSize:0x%02x,fpd type:3543\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x03)
						printf("\tPanelSize:0x%02x,fpd type:1613\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x04)
						printf("\tPanelSize:0x%02x,fpd type:3030\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x05)
						printf("\tPanelSize:0x%02x,fpd type:2530\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x06)
						printf("\tPanelSize:0x%02x,fpd type:3025\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x07)
						printf("\tPanelSize:0x%02x,fpd type:4343-100um\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else
						printf("\tPanelSize:0x%02x,fpd type:3543\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);

					if (theDemo->m_imgW != theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth)  theDemo->m_imgW = theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth;
					if (theDemo->m_imgH != theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight) theDemo->m_imgH = theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight;
					printf("\twidth=%d,hight=%d\n", theDemo->m_imgW, theDemo->m_imgH);
					printf("\tdatatype is unsigned char.\n");
					printf("\tdatabit is 16bits.\n");
					printf("\tdata is little endian.\n");

					theDemo->AutoResize();

					if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize >= 0x01 && theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize <= 0x07)
					{
						/*
						* workmode
						*/
						if (theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x01)
							printf("\tstatic software trigger.[0x%02x]\n", theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
						else if (theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x03)
							printf("\tstatic hvg trigger.[0x%02x]\n", theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
						else if (theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x04)
							printf("\tFreeAED trigger mode.[0x%02x]\n", theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
						else
							printf("\tother trigger mode.[0x%02x]\n", theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);

						theDemo->fpd_base->trigger_mode = theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode;

						printf("\tPre Acquisition Delay Time.%u\n", theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime);
						/*
						* correction enable
						*/
						if (theDemo->fpd_base->offset_enable != theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection)
							theDemo->fpd_base->offset_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection;

						if (theDemo->fpd_base->gain_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection)
							theDemo->fpd_base->gain_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection;

						if (theDemo->fpd_base->defect_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection)
							theDemo->fpd_base->defect_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection;

						if (theDemo->fpd_base->dummy_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection)
							theDemo->fpd_base->dummy_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection;

						if (0x00 == theDemo->fpd_base->offset_enable)
							printf("\tNo Offset Correction\n");
						else if (0x01 == theDemo->fpd_base->offset_enable)
							printf("\tSoftware Offset Correction\n");
						else if (0x02 == theDemo->fpd_base->offset_enable)
							printf("\tFirmware PostOffset Correction\n");
						else if (0x03 == theDemo->fpd_base->offset_enable)
							printf("\tFirmware PreOffset Correction\n");
						else
							printf("\tInvalid Offset Correction\n");

						if (0x00 == theDemo->fpd_base->gain_enable)
							printf("\tNo Gain Correction\n");
						else if (0x01 == theDemo->fpd_base->gain_enable)
							printf("\tSoftware Gain Correction\n");
						else if (0x02 == theDemo->fpd_base->gain_enable)
							printf("\tFirmware Gain Correction\n");
						else
							printf("\tInvalid Gain Correction\n");

						if (0x00 == theDemo->fpd_base->defect_enable)
							printf("\tNo Defect Correction\n");
						else if (0x01 == theDemo->fpd_base->defect_enable)
							printf("\tSoftware Defect Correction\n");
						else if (0x02 == theDemo->fpd_base->defect_enable)
							printf("\tFirmware Defect Correction\n");
						else
							printf("\tInvalid Defect Correction\n");

						if (0x00 == theDemo->fpd_base->dummy_enable)
							printf("\tNo Dummy Correction\n");
						else if (0x01 == theDemo->fpd_base->dummy_enable)
							printf("\tSoftware Dummy Correction\n");
						else if (0x02 == theDemo->fpd_base->dummy_enable)
							printf("\tFirmware Dummy Correction\n");
						else
							printf("\tInvalid Dummy Correction\n");

						// �����ɼ�ʱ����
						BYTE byte1=0, byte2=0, byte3=0, byte4=0;
						byte1 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unContinuousAcquisitionSpanTime, 0);
						byte2 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unContinuousAcquisitionSpanTime, 1);
						byte3 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unContinuousAcquisitionSpanTime, 2);
						byte4 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unContinuousAcquisitionSpanTime, 3);
						unsigned int unValue = BUILD_UINT32(byte4, byte3, byte2, byte1);
						if (theDemo->m_uAcqInterval != unValue) theDemo->m_uAcqInterval = unValue;

						// Prepare ʱ����
						byte1 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime, 0);
						byte2 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime, 1);
						byte3 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime, 2);
						byte4 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime, 3);
						unValue = BUILD_UINT32(byte4, byte3, byte2, byte1);
						if (theDemo->m_uPrepareTm != unValue) theDemo->m_uPrepareTm = unValue;

						// PGA��λ
						theDemo->fpd_base->nPGA = theDemo->GetPGA(theDemo->m_pLastRegCfg->m_TICOFCfg.m_sTICOFRegister[26]);

						if (theDemo->m_imgW != theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth)  theDemo->m_imgW = theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth;
						if (theDemo->m_imgH != theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight) theDemo->m_imgH = theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight;

						//
						int j = (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize - 1);// ƥ�侲̬����
						if (theDemo->img_pro.nSize != HB_FPD_SIZE[j].fpd_size)   theDemo->img_pro.nSize = HB_FPD_SIZE[j].fpd_size;
						theDemo->img_pro.nwidth  = HB_FPD_SIZE[j].fpd_width;
						theDemo->img_pro.nheight = HB_FPD_SIZE[j].fpd_height;
	
						theDemo->img_pro.packet_sum = HB_FPD_SIZE[j].fpd_packet_sum;
						int frame_size = theDemo->img_pro.nwidth * theDemo->img_pro.nheight * 2;
						if (theDemo->img_pro.frame_size != frame_size) theDemo->img_pro.frame_size = frame_size; // ĿǰĬ��Ϊ16bits����
						if (theDemo->img_pro.datatype != 0) theDemo->img_pro.datatype = 0;
						if (theDemo->img_pro.ndatabit != 0) theDemo->img_pro.ndatabit = 0;
						if (theDemo->img_pro.nendian != 0)  theDemo->img_pro.nendian = 0;

						// ���к� 14λ�ַ���
						memcpy(theDemo->szSeiralNum, theDemo->m_pLastRegCfg->m_SysBaseInfo.m_cSnNumber, 14);				
						theDemo->szSeiralNum[14]='\0';

						printf("\twidth=%d,hight=%d\n", theDemo->m_imgW, theDemo->m_imgH);
						printf("\tdatatype is unsigned char.\n");
						printf("\tdatabit is 16bits.\n");
						printf("\tdata is little endian.\n");
						printf("\tm_uPrepareTm=%u\n",      theDemo->m_uPrepareTm);
						printf("\tm_uAcqInterval=%u\n", theDemo->m_uAcqInterval);
						printf("\tszSeiralNum=%s\n", theDemo->szSeiralNum);

						// ����fpd_base
						theDemo->PostMessage(WM_USER_CURR_CONTROL_DATA, (WPARAM)0, (LPARAM)0);
				}			
			}
		}
		break;
	case ECALLBACK_TYPE_SET_CFG_OK: // ADD BY MH.YANG 2020-02-11
		printf("ECALLBACK_TYPE_SET_CFG_OK:\n");
		printf("\tReedback set rom param succuss!\n");
		b = theDemo->GetFirmwareConfig();
		if (b)
			printf("GetFirmwareConfig success\n");
		else
			printf("err:GetFirmwareConfig failed\n");
		break;
	case ECALLBACK_TYPE_SINGLE_IMAGE:   // ��֡�ɼ���ͼ
	case ECALLBACK_TYPE_MULTIPLE_IMAGE: // �����ɼ���ͼ
		if (theDemo->bShowPic) {
			ret = theDemo->ShowImage((unsigned short *)buff, (len / sizeof(unsigned short)), nId);
			if (ret)
				printf("theDemo->ShowImage succss!Frame ID:%05d\n", nId);
			else
				printf("theDemo->ShowImage failed!Frame ID:%05d\n", nId);

			// ����Ϊ�ź�״̬
			if (pDlg != NULL) {
				if (pDlg->m_hEvent != NULL) {
					::SetEvent(pDlg->m_hEvent);
				}
			}
		}
		else {
			ret = theDemo->SaveImage((unsigned char *)buff, len);
			if (ret)
				printf("theDemo->SaveImage succss!Frame ID:%05d\n", nId);
			else
				printf("theDemo->SaveImage failed!Frame ID:%05d\n", nId);
		}
		break;
	case ECALLBACK_TYPE_THREAD_EVENT:
		if (len == 112)// offsetʹ�ܣ�У��������Ϣ
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],offset calibrate:success!\n",len);
		else if (len == 113)
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],offset calibrate:failed!\n", len);
		else if (len == 114)// gainʹ�ܣ�У��������Ϣ
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],gain calibrate:success!\n", len);
		else if (len == 115)
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],gain calibrate:failed!\n", len);
		else if (len == 116)// defectʹ�ܣ�У��������Ϣ
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],defect calibrate:success!\n", len);
		else if (len == 117)
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],defect calibrate:failed!\n", len);
		else
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],other feedback message!\n", len);
		break;
	case ECALLBACK_TYPE_ACQ_SUCCESS:  /* �����ɼ�,��ʱ����ΪRAW�ļ� */
		if (theDemo != NULL) {
			CString strMsg = _T("");
			ECALLBACK_RAW_INFO *ptr = (ECALLBACK_RAW_INFO *)buff;
			if (ptr != NULL) {
				if (len == 1) {
					printf("%s success!,dMean=%0.2f\n", ptr->szRawName, ptr->dMean);
				}
				else {
					printf("%s failed!\n", ptr->szRawName);
				}
			} // if (ptr != NULL)
			else {
				printf("ECALLBACK_RAW_INFO ptr is NULL!\n");
			}
		} // if (theDoc != NULL)
		break;
	case ECALLBACK_TYPE_GENERATE_TEMPLATE:
		if (theDemo != NULL) {
			if (len == ECALLBACK_TEMPLATE_BEGIN) {
				printf("ECALLBACK_TEMPLATE_BEGIN\n");
			}
			else if (len == ECALLBACK_TEMPLATE_INVALVE_PARAM) {
				printf("ECALLBACK_TEMPLATE_INVALVE_PARAM:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_MALLOC_FAILED) {
				printf("ECALLBACK_TEMPLATE_MALLOC_FAILED:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_SEND_FAILED) {
				printf("ECALLBACK_TEMPLATE_SEND_FAILED:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_STATUS_ABORMAL) {
				printf("ECALLBACK_TEMPLATE_STATUS_ABORMAL:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_FRAME_NUM) {
				printf("ECALLBACK_TEMPLATE_FRAME_NUM:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_TIMEOUT) {
				printf("ECALLBACK_TEMPLATE_TIMEOUT:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_MEAN) {
				ECALLBACK_RAW_INFO *ptr = (ECALLBACK_RAW_INFO *)buff;
				if (ptr != NULL) {
					printf("ECALLBACK_TEMPLATE_MEAN:%s,dMean=%0.2f\n", ptr->szRawName, ptr->dMean);
				}
			}
			else if (len == ECALLBACK_TEMPLATE_GENERATE) {
				printf("ECALLBACK_TEMPLATE_GENERATE:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_RESULT) {
				////if (nId == -1)
				////	printf("offset template generate failed!");
				////else if (nId == 1)
				////	printf("offset template generate success!");
				////else if (nId == -2)
				////	printf("gain template generate failed!");
				////else if (nId == 2)
				////	printf("gain template generate success!");
				////else if (nId == -3)
				////	printf("defect template generate failed!");
				////else if (nId == 3)
				////	printf("defect template generate success!");
				////else
				////	printf("err:other error!");
				if (theDemo->m_templateHwnd != NULL)
					::PostMessage(theDemo->m_templateHwnd, WM_USER_NOTICE_TEMPLATE_TOOL, (WPARAM)nId, (LPARAM)0);
			}
			else {// other
				printf("other:nid=%d\n", nId);
			}
		}
		break;
	case ECALLBACK_TYPE_FILE_NOTEXIST:
		if (theDemo != NULL) {
			if (buff != NULL) {
				printf("err:%s not exist!\n", (char *)buff);
			}
		}
		break;
	default:
		printf("ECALLBACK_TYPE_INVALVE,command=%02x\n", command);
		break;
	}
	return ret;
}
#else
int CHB_SDK_DEMO2008Dlg::handleCommandEvent(unsigned char command, void *buff, int len, int nId)
	{
	int ret = 0;
	if ((command == ECALLBACK_TYPE_SINGLE_IMAGE) || (command == ECALLBACK_TYPE_MULTIPLE_IMAGE) ||
		(command == ECALLBACK_TYPE_ROM_UPLOAD) || (command == ECALLBACK_TYPE_RAM_UPLOAD)) {
			if (buff == NULL || len == 0) {
				printf("ע��ص����������쳣!\n");
				return ret;
				}
		}

	int status = -1;
	int j = 0;
	ret = 1;
	bool b = false;
	switch (command)
		{
		case ECALLBACK_TYPE_NET_ERR_MSG: // ƽ��״̬������/�Ͽ�/ready/busy
			printf("ECALLBACK_TYPE_NET_ERR_MSG,recode=%d\n", len);
			if (theDemo != NULL) {
				CString strMsg = _T("");
				if (len <= 0 && len >= -7) {
					if (len == 0)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:����δ����!\n");
					else if (len == -1)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:�����쳣!\n");
					else if (len == -2)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:׼��������������������ʧ��!\n");
					else if (len == -3)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:���ճ�ʱ!\n");
					else if (len == -4)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:����ʧ��!\n");
					else if (len == -5)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:�˿ڲ��ɶ�!\n");
					else if (len == -6)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,network card unusual!\n");
					else if (len == -7)
						printf("ECALLBACK_TYPE_NET_ERR_MSG,network card ok!\n");
					status = (int)FPD_STATUS_DISCONN;
				}
				else if (len == 1) { // connect
					printf("ECALLBACK_TYPE_NET_ERR_MSG,��ʼ����!\n");
					status = (int)FPD_STATUS_CONN;
				}
				else if (len == 2) { // ready
					printf("ECALLBACK_TYPE_NET_ERR_MSG,ready!\n");
					status = (int)FPD_STATUS_READY;
				}
				else if (len == 3) { // busy
					printf("ECALLBACK_TYPE_NET_ERR_MSG,busy!\n");
					status = (int)FPD_STATUS_BUSY;
				}
				else if (len == 4) { // prepare
					printf("ECALLBACK_TYPE_NET_ERR_MSG,prepare!\n");
					status = (int)FPD_STATUS_PREPARE;
				}
				else
					printf("ECALLBACK_TYPE_NET_ERR_MSG,Err:other error=%d\n", len);

				// ADD BY MH.YANG 2019/11/12
				if (status != -1) {
					// ����ͼ��
					theDemo->PostMessage(WM_USER_CONNECT_STATUS_ICON, (WPARAM)status, (LPARAM)0);

					// ����״̬��Ϣ
					if (len <= 0 && len >= -6) {
						/*
						* �����Ͽ���Ϣ
						*/ 
						if (theDemo->m_pFpd != NULL) 
							HBI_DisConnectDetector(theDemo->m_pFpd);
					}
				}
			}
			break;
		case ECALLBACK_TYPE_ROM_UPLOAD:/* �������� */ 
			printf("ECALLBACK_TYPE_ROM_UPLOAD:\n");
			if (theDemo->m_pLastRegCfg != NULL) {
				memset(theDemo->m_pLastRegCfg, 0x00, sizeof(RegCfgInfo));
				memcpy(theDemo->m_pLastRegCfg, (unsigned char *)buff, sizeof(RegCfgInfo));

				printf("\tSerial Number:%s\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_cSnNumber);

				if (theDemo->fpd_base != NULL) {
					/*
					* ip and port
					*/
					// �ߵ�λ��Ҫת��
					unsigned short usValue = ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort & 0xff) << 8) | ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort >> 8) & 0xff);
					printf("\tSourceIP:%d.%d.%d.%d:%u\n",
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[0],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[1],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[2],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[3],
						usValue);

					usValue = ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort & 0xff) << 8) | ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort >> 8) & 0xff);
					printf("\tDestIP:%d.%d.%d.%d:%u\n",
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[0],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[1],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[2],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[3],
						usValue);

					j = sprintf(theDemo->fpd_base->DstIP, "%d.%d.%d.%d",
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[0],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[1],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[2],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_byDestIP[3]);
					theDemo->fpd_base->DstIP[j] = '\0';

					j = sprintf(theDemo->fpd_base->SrcIP, "%d.%d.%d.%d",
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[0],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[1],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[2],
						theDemo->m_pLastRegCfg->m_EtherInfo.m_bySourceIP[3]);
					theDemo->fpd_base->SrcIP[j] = '\0';

					// �ߵ�λ��Ҫת��
					usValue = ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort & 0xff) << 8) | ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort >> 8) & 0xff);
					theDemo->fpd_base->DstPort = usValue;

					usValue = ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort & 0xff) << 8) | ((theDemo->m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort >> 8) & 0xff);
					theDemo->fpd_base->SrcPort = usValue;

					//
					if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x01)
						printf("\tPanelSize:0x%02x,fpd type:4343-140um\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x02)
						printf("\tPanelSize:0x%02x,fpd type:3543\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x03)
						printf("\tPanelSize:0x%02x,fpd type:1613\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x04)
						printf("\tPanelSize:0x%02x,fpd type:3030\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x05)
						printf("\tPanelSize:0x%02x,fpd type:2530\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x06)
						printf("\tPanelSize:0x%02x,fpd type:3025\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x07)
						printf("\tPanelSize:0x%02x,fpd type:4343-100um\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
					else
						printf("\tPanelSize:0x%02x,fpd type:3543\n", theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);

					if (theDemo->m_imgW != theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth)  theDemo->m_imgW = theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth;
					if (theDemo->m_imgH != theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight) theDemo->m_imgH = theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight;
					printf("\twidth=%d,hight=%d\n", theDemo->m_imgW, theDemo->m_imgH);
					printf("\tdatatype is unsigned char.\n");
					printf("\tdatabit is 16bits.\n");
					printf("\tdata is little endian.\n");

					theDemo->AutoResize();

					if (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize >= 0x01 && theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize <= 0x07)
					{
						/*
						* workmode
						*/
						if (theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x01)
							printf("\tstatic software trigger.[0x%02x]\n", theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
						else if (theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x03)
							printf("\tstatic hvg trigger.[0x%02x]\n", theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
						else if (theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x04)
							printf("\tFreeAED trigger mode.[0x%02x]\n", theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
						else
							printf("\tother trigger mode.[0x%02x]\n", theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);

						theDemo->fpd_base->trigger_mode = theDemo->m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode;

						printf("\tPre Acquisition Delay Time.%u\n", theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime);
						/*
						* correction enable
						*/
						if (theDemo->fpd_base->offset_enable != theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection)
							theDemo->fpd_base->offset_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection;

						if (theDemo->fpd_base->gain_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection)
							theDemo->fpd_base->gain_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection;

						if (theDemo->fpd_base->defect_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection)
							theDemo->fpd_base->defect_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection;

						if (theDemo->fpd_base->dummy_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection)
							theDemo->fpd_base->dummy_enable = theDemo->m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection;

						if (0x00 == theDemo->fpd_base->offset_enable)
							printf("\tNo Offset Correction\n");
						else if (0x01 == theDemo->fpd_base->offset_enable)
							printf("\tSoftware Offset Correction\n");
						else if (0x02 == theDemo->fpd_base->offset_enable)
							printf("\tFirmware PostOffset Correction\n");
						else if (0x03 == theDemo->fpd_base->offset_enable)
							printf("\tFirmware PreOffset Correction\n");
						else
							printf("\tInvalid Offset Correction\n");

						if (0x00 == theDemo->fpd_base->gain_enable)
							printf("\tNo Gain Correction\n");
						else if (0x01 == theDemo->fpd_base->gain_enable)
							printf("\tSoftware Gain Correction\n");
						else if (0x02 == theDemo->fpd_base->gain_enable)
							printf("\tFirmware Gain Correction\n");
						else
							printf("\tInvalid Gain Correction\n");

						if (0x00 == theDemo->fpd_base->defect_enable)
							printf("\tNo Defect Correction\n");
						else if (0x01 == theDemo->fpd_base->defect_enable)
							printf("\tSoftware Defect Correction\n");
						else if (0x02 == theDemo->fpd_base->defect_enable)
							printf("\tFirmware Defect Correction\n");
						else
							printf("\tInvalid Defect Correction\n");

						if (0x00 == theDemo->fpd_base->dummy_enable)
							printf("\tNo Dummy Correction\n");
						else if (0x01 == theDemo->fpd_base->dummy_enable)
							printf("\tSoftware Dummy Correction\n");
						else if (0x02 == theDemo->fpd_base->dummy_enable)
							printf("\tFirmware Dummy Correction\n");
						else
							printf("\tInvalid Dummy Correction\n");

						// �����ɼ�ʱ����
						BYTE byte1=0, byte2=0, byte3=0, byte4=0;
						byte1 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unContinuousAcquisitionSpanTime, 0);
						byte2 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unContinuousAcquisitionSpanTime, 1);
						byte3 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unContinuousAcquisitionSpanTime, 2);
						byte4 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unContinuousAcquisitionSpanTime, 3);
						unsigned int unValue = BUILD_UINT32(byte4, byte3, byte2, byte1);
						if (theDemo->m_uAcqInterval != unValue) theDemo->m_uAcqInterval = unValue;

						// Prepare ʱ����
						byte1 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime, 0);
						byte2 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime, 1);
						byte3 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime, 2);
						byte4 = BREAK_UINT32(theDemo->m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime, 3);
						unValue = BUILD_UINT32(byte4, byte3, byte2, byte1);
						if (theDemo->m_uPrepareTm != unValue) theDemo->m_uPrepareTm = unValue;

						// PGA��λ
						theDemo->fpd_base->nPGA = theDemo->GetPGA(theDemo->m_pLastRegCfg->m_TICOFCfg.m_sTICOFRegister[26]);

						if (theDemo->m_imgW != theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth)  theDemo->m_imgW = theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth;
						if (theDemo->m_imgH != theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight) theDemo->m_imgH = theDemo->m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight;

						//
						int j = (theDemo->m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize - 1);// ƥ�侲̬����
						if (theDemo->img_pro.nSize != HB_FPD_SIZE[j].fpd_size)   theDemo->img_pro.nSize = HB_FPD_SIZE[j].fpd_size;
						theDemo->img_pro.nwidth  = HB_FPD_SIZE[j].fpd_width;
						theDemo->img_pro.nheight = HB_FPD_SIZE[j].fpd_height;
	
						theDemo->img_pro.packet_sum = HB_FPD_SIZE[j].fpd_packet_sum;
						int frame_size = theDemo->img_pro.nwidth * theDemo->img_pro.nheight * 2;
						if (theDemo->img_pro.frame_size != frame_size) theDemo->img_pro.frame_size = frame_size; // ĿǰĬ��Ϊ16bits����
						if (theDemo->img_pro.datatype != 0) theDemo->img_pro.datatype = 0;
						if (theDemo->img_pro.ndatabit != 0) theDemo->img_pro.ndatabit = 0;
						if (theDemo->img_pro.nendian != 0)  theDemo->img_pro.nendian = 0;

						// ���к� 14λ�ַ���
						memcpy(theDemo->szSeiralNum, theDemo->m_pLastRegCfg->m_SysBaseInfo.m_cSnNumber, 14);				
						theDemo->szSeiralNum[14]='\0';

						printf("\twidth=%d,hight=%d\n", theDemo->m_imgW, theDemo->m_imgH);
						printf("\tdatatype is unsigned char.\n");
						printf("\tdatabit is 16bits.\n");
						printf("\tdata is little endian.\n");
						printf("\tm_uPrepareTm=%u\n",      theDemo->m_uPrepareTm);
						printf("\tm_uAcqInterval=%u\n", theDemo->m_uAcqInterval);
						printf("\tszSeiralNum=%s\n", theDemo->szSeiralNum);

						// ����fpd_base
						theDemo->PostMessage(WM_USER_CURR_CONTROL_DATA, (WPARAM)0, (LPARAM)0);
				}			
			}
		}
		break;
	case ECALLBACK_TYPE_SET_CFG_OK: // ADD BY MH.YANG 2020-02-11
		printf("ECALLBACK_TYPE_SET_CFG_OK:\n");
		printf("\tReedback set rom param succuss!\n");
		b = theDemo->GetFirmwareConfig();
		if (b)
			printf("GetFirmwareConfig success\n");
		else
			printf("err:GetFirmwareConfig failed\n");
		break;
	case ECALLBACK_TYPE_SINGLE_IMAGE:   // ��֡�ɼ���ͼ
	case ECALLBACK_TYPE_MULTIPLE_IMAGE: // �����ɼ���ͼ
		if (theDemo->bShowPic) {
			ret = theDemo->ShowImage((unsigned short *)buff, (len / sizeof(unsigned short)), nId);
			if (ret)
				printf("theDemo->ShowImage succss!Frame ID:%05d\n", nId);
			else
				printf("theDemo->ShowImage failed!Frame ID:%05d\n", nId);

			// ����Ϊ�ź�״̬
			if (theDemo->m_hEvent != NULL) {
				::SetEvent(theDemo->m_hEvent);
			}
		}
		else {
			ret = theDemo->SaveImage((unsigned char *)buff, len);
			if (ret)
				printf("theDemo->SaveImage succss!Frame ID:%05d\n", nId);
			else
				printf("theDemo->SaveImage failed!Frame ID:%05d\n", nId);
		}
		break;
	case ECALLBACK_TYPE_THREAD_EVENT:
		if (len == 112)// offsetʹ�ܣ�У��������Ϣ
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],offset calibrate:success!\n",len);
		else if (len == 113)
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],offset calibrate:failed!\n", len);
		else if (len == 114)// gainʹ�ܣ�У��������Ϣ
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],gain calibrate:success!\n", len);
		else if (len == 115)
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],gain calibrate:failed!\n", len);
		else if (len == 116)// defectʹ�ܣ�У��������Ϣ
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],defect calibrate:success!\n", len);
		else if (len == 117)
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],defect calibrate:failed!\n", len);
		else
			printf("ECALLBACK_TYPE_THREAD_EVENT[%d],other feedback message!\n", len);
		break;
	case ECALLBACK_TYPE_ACQ_SUCCESS:  /* �����ɼ�,��ʱ����ΪRAW�ļ� */
		if (theDemo != NULL) {
			CString strMsg = _T("");
			ECALLBACK_RAW_INFO *ptr = (ECALLBACK_RAW_INFO *)buff;
			if (ptr != NULL) {
				if (len == 1) {
					printf("%s success!,dMean=%0.2f\n", ptr->szRawName, ptr->dMean);
				}
				else {
					printf("%s failed!\n", ptr->szRawName);
				}
			} // if (ptr != NULL)
			else {
				printf("ECALLBACK_RAW_INFO ptr is NULL!\n");
			}
		} // if (theDoc != NULL)
		break;
	case ECALLBACK_TYPE_GENERATE_TEMPLATE:
		if (theDemo != NULL) {
			if (len == ECALLBACK_TEMPLATE_BEGIN) {
				printf("ECALLBACK_TEMPLATE_BEGIN\n");
			}
			else if (len == ECALLBACK_TEMPLATE_INVALVE_PARAM) {
				printf("ECALLBACK_TEMPLATE_INVALVE_PARAM:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_MALLOC_FAILED) {
				printf("ECALLBACK_TEMPLATE_MALLOC_FAILED:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_SEND_FAILED) {
				printf("ECALLBACK_TEMPLATE_SEND_FAILED:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_STATUS_ABORMAL) {
				printf("ECALLBACK_TEMPLATE_STATUS_ABORMAL:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_FRAME_NUM) {
				printf("ECALLBACK_TEMPLATE_FRAME_NUM:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_TIMEOUT) {
				printf("ECALLBACK_TEMPLATE_TIMEOUT:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_MEAN) {
				ECALLBACK_RAW_INFO *ptr = (ECALLBACK_RAW_INFO *)buff;
				if (ptr != NULL) {
					printf("ECALLBACK_TEMPLATE_MEAN:%s,dMean=%0.2f\n", ptr->szRawName, ptr->dMean);
				}
			}
			else if (len == ECALLBACK_TEMPLATE_GENERATE) {
				printf("ECALLBACK_TEMPLATE_GENERATE:%d\n", nId);
			}
			else if (len == ECALLBACK_TEMPLATE_RESULT) {
				////if (nId == -1)
				////	printf("offset template generate failed!");
				////else if (nId == 1)
				////	printf("offset template generate success!");
				////else if (nId == -2)
				////	printf("gain template generate failed!");
				////else if (nId == 2)
				////	printf("gain template generate success!");
				////else if (nId == -3)
				////	printf("defect template generate failed!");
				////else if (nId == 3)
				////	printf("defect template generate success!");
				////else
				////	printf("err:other error!");
				if (theDemo->m_templateHwnd != NULL)
					::PostMessage(theDemo->m_templateHwnd, WM_USER_NOTICE_TEMPLATE_TOOL, (WPARAM)nId, (LPARAM)0);
			}
			else {// other
				printf("other:nid=%d\n", nId);
			}
		}
		break;
	case ECALLBACK_TYPE_FILE_NOTEXIST:
		if (theDemo != NULL) {
			if (buff != NULL) {
				printf("err:%s not exist!\n", (char *)buff);
			}
		}
		break;
	default:
		printf("ECALLBACK_TYPE_INVALVE,command=%02x\n", command);
		break;
	}
	return ret;
}
#endif

void CHB_SDK_DEMO2008Dlg::OnBnClickedRadioShowPic()
{
	// TODO: Add your control notification handler code here
	if (!bShowPic) bShowPic = true;
}

void CHB_SDK_DEMO2008Dlg::OnBnClickedRadioSavePic()
{
	// TODO: Add your control notification handler code here	
	if (bShowPic) bShowPic = false;
}


int CHB_SDK_DEMO2008Dlg::ShowImage(unsigned short *imgbuff, int nbufflen, int nframeid)
{
	//if (m_IsExitsSubWnd != 1) {
	//	printf("err:ShowImage failed!m_IsExitsSubWnd is not 1!\n");
	//	return 0;
	//}
	if (m_hEvent == NULL) {
		printf("err:m_hEvent is NULL!\n");
		return 0;
	}
	if (m_Threadhdl == NULL)
	{
		printf("err:m_Threadhdl is NULL!\n");
		return 0;
	}

	/* ̽�����ֱ��ʴ�С */
	int WIDTH  = m_imgW;
	int HEIGHT = m_imgH;
	if ((imgbuff == NULL) || (nbufflen != (WIDTH * HEIGHT))) {
		printf("�����쳣!\n");
		return 0;
	}
	// 
	CvSize sz;
	sz.width = (int)(WIDTH  *  pic_factor);
	sz.height = (int)(HEIGHT *  pic_factor);
	//
	if (pIplimage != NULL) {
		if (pIplimage->width != WIDTH || pIplimage->height != HEIGHT)
		{
			cvReleaseImage(&pIplimage);
			pIplimage = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_16U, 1);
		}
	}
	else {
		pIplimage = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_16U, 1);
	}
	if (pIplimage == NULL) {
		printf("err:pIplimage or desIplimage is NULL!");
		return -1;
	}
	//
	if (desIplimage != NULL)
	{
		if (desIplimage->width != sz.width || desIplimage->height != sz.height)
		{
			cvReleaseImage(&desIplimage);
			desIplimage = cvCreateImage(sz, pIplimage->depth, pIplimage->nChannels);
		}
	}
	else {
		desIplimage = cvCreateImage(sz, pIplimage->depth, pIplimage->nChannels);
	}
	if (desIplimage == NULL) {
		printf("err:desIplimage is NULL!");
		return -1;
	}

	// ���ݿ���
	memcpy(pIplimage->imageData, imgbuff, nbufflen * sizeof(unsigned short));
	if (m_frameid != nframeid) m_frameid = nframeid;

	return 1;
}

void CHB_SDK_DEMO2008Dlg::CloseShowThread()
{
    RunFlag = 0;
	if (m_Threadhdl != NULL)		{
		if (m_hEvent != NULL) {
			SetEvent(m_hEvent);
			printf("%s\n", "SetEvent(m_hEvent)!");
		}
		//
		DWORD dw = WaitForSingleObject(m_Threadhdl, 5000);
		if (dw != WAIT_OBJECT_0)	{
			printf("err:%s\n", "TerminateThread(m_Threadhdl�� 0��!");
		}
		else	{
			printf("%s\n", "CloseHandle(m_Threadhdl�� 0��!");
		}
		//
		CloseHandle(m_Threadhdl);
		m_Threadhdl = NULL;
		m_uThreadFunID = 0;
		//
		if (m_hEvent != NULL) {
			::CloseHandle(m_hEvent);
			m_hEvent = NULL;
			printf("%s\n", "CloseHandle(m_hEvent)!");
		}
	}
#ifdef _DEBUG
	printf("CloseShowThread\n");
#endif
}

void CHB_SDK_DEMO2008Dlg::UpdateImage()
{
	CDC *pDC = pPicWnd->GetDC();        // �����ʾ�ؼ��� DC wishchin
	if (pDC == NULL) return;

	// �����ʷ
	pDC->FillSolidRect(&m_PicRect, RGB(0, 0, 0));

	HDC hDC = pDC->GetSafeHdc();        // ��ȡ HDC(�豸���) �����л�ͼ����
	CvvImage cimg;
	cimg.CopyOf(pIplimage );                        // ����ͼƬ
	cimg.DrawToHDC(hDC, &m_PicRect);                // ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������  �������ԣ�
	SetBkMode( hDC,TRANSPARENT);
	SetTextColor( hDC,RGB(255, 0, 0));
	// ��ʾͼ����Ϣ
	char buff[32];
	memset(buff, 0x00, 32);
	// ��ʾʱ��
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	int j = sprintf(buff, "Date:%4d-%02d-%02d %02d:%02d:%02d.%03d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
	buff[j] = '\0';
	::TextOut(hDC, 10, 10, buff, j);
	// ��ʾ֡��
	j = sprintf(buff, "Frame ID:%05d", m_frameid);
	buff[j] = '\0';
	::TextOut(hDC, 10, 30, buff, j);
	// �ֱ���
	j = sprintf(buff, "Pixel:%d * %d", m_imgW, m_imgH);
	buff[j] = '\0';
	::TextOut(hDC, 10, 50, buff, j);
	ReleaseDC( pDC );
}

unsigned int __stdcall CHB_SDK_DEMO2008Dlg::ShowThreadProcFun(LPVOID pParam)
{
	printf("%s\n", "ShowThreadProcFun start!");
	int ret = 0;
	int j = 0;
	DWORD dwResult = 0;
	
	///////////////////////////////////////////////////////////////////////////////////////
	CHB_SDK_DEMO2008Dlg* pPlugIn = (CHB_SDK_DEMO2008Dlg *)pParam;
	if (pPlugIn == NULL) {
		printf("%s\n", "�̲߳���Ϊ�գ����� ShowThreadProcFun ʧ��!");
		goto EXIT_SHOW_IMAGE_THREAD;
	}


	// �¼����
	if (NULL == pPlugIn->m_hEvent) {
		printf("err:ShowThreadProcFun,%s\n", "pPlugIn->m_hEvent is NULL!");
		goto EXIT_SHOW_IMAGE_THREAD;
	}
	// ����Ϊ���ź�״̬
	::ResetEvent(pPlugIn->m_hEvent);

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	while (pPlugIn->RunFlag) {
		dwResult = ::WaitForSingleObject(pPlugIn->m_hEvent, INFINITE);
		if (WAIT_OBJECT_0 == dwResult) {
			if (!pPlugIn->RunFlag)
			{
				printf("pPlugIn->RunFlag=%dexit!\n", pPlugIn->RunFlag);
				break;
			}
			//
			if (pPlugIn->pPicWnd != NULL) {
				//printf("ShowThreadProcFun:frameid=%d\n", pPlugIn->m_frameid);
				pPlugIn->UpdateImage();
			}
		}
		else { /* timeout or other error */	
			if (WAIT_TIMEOUT == dwResult) {
			}
		}
		// ����Ϊ���ź�״̬
		::ResetEvent(pPlugIn->m_hEvent);
	}
EXIT_SHOW_IMAGE_THREAD:
	// �˳��߳�
	CloseHandle(pPlugIn->m_Threadhdl);
	pPlugIn->m_Threadhdl = NULL;
	pPlugIn->m_uThreadFunID = 0;
	//
	printf("%s\n", "ShowThreadProcFun end!");
	return 0;
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonFirmwareVer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	printf("OnBnClickedButtonFirmwareVer\n");
	if (m_pFpd == NULL) {
		::AfxMessageBox("m_pFpd is NULL!");
		return;
	}
	int ret = HBI_GetFirmareVerion(m_pFpd, szFirmVer);
	if (0 != ret) {
		CString strStr = _T("");
		strStr.Format(_T("HBI_GetFirmareVerion Err,ret=%d"), ret);
		::AfxMessageBox(strStr);
		return;
	}
	printf("szFirmVer=%s\n", szFirmVer);
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonSoftwareVer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	printf("OnBnClickedButtonSoftwareVer\n");
	if (m_pFpd == NULL) {
		::AfxMessageBox("m_pFpd is NULL!");
		return;
	}

	int ret = HBI_GetSDKVerion(m_pFpd, szSdkVer);
	if (0 != ret) {
		CString strStr = _T("");
		strStr.Format(_T("HBI_GetSDKVerion Err,ret=%d"), ret);
		::AfxMessageBox(strStr);
		return;
	}
	printf("szSdkVer=%s\n", szSdkVer);
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedBtnGetImageProperty()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	printf("get Image property begin!\n");

	size_t size = sizeof(HB_FPD_SIZE) / sizeof(struct FpdPixelMatrixTable);
	for (size_t i = 0; i < size; i++) {
		img_pro.nSize      = HB_FPD_SIZE[i].fpd_size;
		img_pro.nwidth     = HB_FPD_SIZE[i].fpd_width;
		img_pro.nheight    = HB_FPD_SIZE[i].fpd_height;
		img_pro.packet_sum = HB_FPD_SIZE[i].fpd_packet_sum;

		printf("\timg_pro.nSize=%u\n", img_pro.nSize);
//		printf("\timg_pro.datatype=%u\n", img_pro.datatype);
		printf("\timg_pro.nwidth=%u\n", img_pro.nwidth);
		printf("\timg_pro.nheight=%u\n", img_pro.nheight);
//		printf("\timg_pro.ndatabit=%u\n", img_pro.ndatabit);
//		printf("\timg_pro.nendian=%u\n", img_pro.nendian);
		printf("\timg_pro.packet_sum=%u\n\n", img_pro.packet_sum);
//		printf("\timg_pro.frame_size=%d\n\n", img_pro.frame_size);
	}

	if (m_pFpd == NULL) {
		printf("δ����״̬!\n");
		return;
	}

	int ret = HBI_GetImageProperty(m_pFpd, &img_pro);
	if (ret == 0) {
		printf("HBI_GetImageProperty:\n\tnSize=%u\n", img_pro.nSize);
		if (m_imgW != img_pro.nwidth) m_imgW = img_pro.nwidth;
		if (m_imgH != img_pro.nheight) m_imgH = img_pro.nheight;
		printf("\twidth=%d,hight=%d\n", m_imgW, m_imgH);

		//
		if (img_pro.datatype == 0) printf("\tdatatype is unsigned char.\n");
		else if (img_pro.datatype == 1) printf("\tdatatype is char.\n");
		else if (img_pro.datatype == 2) printf("\tdatatype is unsigned short.\n");
		else if (img_pro.datatype == 3) printf("\tdatatype is float.\n");
		else if (img_pro.datatype == 4) printf("\tdatatype is double.\n");
		else printf("\tdatatype is not support.\n");
		//
		if (img_pro.ndatabit == 0) printf("\tdatabit is 16bits.\n");
		else if (img_pro.ndatabit == 1) printf("\tdatabit is 14bits.\n");
		else if (img_pro.ndatabit == 2) printf("\tdatabit is 12bits.\n");
		else if (img_pro.ndatabit == 3) printf("\tdatabit is 8bits.\n");
		else printf("\tdatatype is unsigned char.\n");
		//
		if (img_pro.nendian == 0) printf("\tdata is little endian.\n");
		else printf("\tdata is bigger endian.\n");

		printf("\tpacket_sum=%u\n", img_pro.packet_sum);
		printf("\tframe_size=%d\n\n", img_pro.frame_size);
	}
	else {
		::AfxMessageBox("HBI_GetImageProperty failed!");
	}
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonGetConfig()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bool b = GetFirmwareConfig();
	if (b)
		printf("GetFirmwareConfig success\n");
	else
		printf("err:GetFirmwareConfig failed\n");
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonSingleShot()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	printf("OnBnClickedButtonSingleShot!\n");
	aqc_mode.aqc_mode = STATIC_ACQ_DEFAULT_MODE;
	aqc_mode.nframesum = 0;
	aqc_mode.nframeid = 0;
	aqc_mode.ngroupno = 0;
	aqc_mode.ndiscard = 0;

	printf("DoSingleShot:[_curmode]=%d,[_acqMaxNum]=%d,[_frameId]=%d,[_groupNum]=%d\n", aqc_mode.aqc_mode, aqc_mode.nframesum, aqc_mode.nframeid, aqc_mode.ngroupno);

	unsigned char cmd = EDL_COMMON_TYPE_SINGLE_SHORT;
	int ret = HBI_SingleAcquisition(m_pFpd, aqc_mode);
	if (ret == 0)
		printf("mode=%d,sum=%d,num=%d,group=%d,Do Live Acquisition succss!\n", aqc_mode.aqc_mode, aqc_mode.nframesum, aqc_mode.ndiscard, aqc_mode.ngroupno);
	else {
		printf("mode=%d,sum=%d,num=%d,group=%d,Do Live Acquisition failed!\n", aqc_mode.aqc_mode, aqc_mode.nframesum, aqc_mode.ndiscard, aqc_mode.ngroupno);
	}
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedBtnSetTriggerCorrection()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	printf("OnBnClickedBtnSetTriggerAndCorrect begin!\n");
	if (m_pFpd == NULL) {
		printf("δ����״̬!\n");
		return;
	}

	// ����ģʽ
	int _triggerMode = STATIC_SOFTWARE_TRIGGER_MODE; //1 - ������3 - ��ѹ������4 - FreeAED��	
	int select = ((CComboBox *)GetDlgItem(IDC_COMBO_TRIGGER_MODE))->GetCurSel();
	if (select != 1 && select != 3 && select != 4)  {
		::AfxMessageBox("Not surport yet!");
		return;
	}
	else _triggerMode = select;

	// ����վ����SDK������������ģʽ��У��
	IMAGE_CORRECT_ENABLE *pcorrect = new IMAGE_CORRECT_ENABLE;
	if (pcorrect != NULL) {		
#if 0
		pcorrect->bFeedbackCfg = false; // false-ECALLBACK_TYPE_SET_CFG_OK Event
#else
		pcorrect->bFeedbackCfg = true;  // //true-ECALLBACK_TYPE_ROM_UPLOAD Event,
#endif
		pcorrect->ucOffsetCorrection = ((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_OFFSET))->GetCurSel();
		pcorrect->ucGainCorrection   = ((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_GAIN))->GetCurSel();
		pcorrect->ucDefectCorrection = ((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_DEFECT))->GetCurSel();
		pcorrect->ucDummyCorrection  = 0;
		int ret = HBI_TriggerAndCorrectApplay(m_pFpd, _triggerMode, pcorrect);
		if (ret == 0)
			printf("OnBnClickedBtnSetTriggerAndCorrect:\n\tHBI_TriggerAndCorrectApplay success!\n");
		else
			::AfxMessageBox("HBI_TriggerAndCorrectApplay failed!");
		delete pcorrect;
		pcorrect = NULL;
	}
	else {
		::AfxMessageBox("malloc IMAGE_CORRECT_ENABLE failed!");
	}
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedBtnSetTriggerMode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	printf("set Firmware trigger mode begin!\n");
	if (m_pFpd == NULL) {
		printf("δ����״̬!\n");
		return;
	}
	/*
	* ������ģʽ�����޸��⣬����ֵ����
	typedef enum
	{
		INVALID_TRIGGER_MODE         = 0x00,
		STATIC_SOFTWARE_TRIGGER_MODE = 0x01,         // 
		STATIC_PREP_TRIGGER_MODE     = 0x02,         //
		STATIC_HVG_TRIGGER_MODE      = 0x03,         //
		STATIC_AED_TRIGGER_MODE      = 0x04,
		DYNAMIC_HVG_TRIGGER_MODE     = 0x05,
		DYNAMIC_FPD_TRIGGER_MODE     = 0x06,
		DYNAMIC_FPD_CONTINUE_MODE    = 0x07
	}EnumTRIGGER_MODE;
	*/
	// ����ģʽ
	int _triggerMode = STATIC_SOFTWARE_TRIGGER_MODE; //1 - ������3 - ��ѹ������4 - FreeAED��	
	int select = ((CComboBox *)GetDlgItem(IDC_COMBO_TRIGGER_MODE))->GetCurSel();
	if (select != 1 && select != 3 && select != 4)  {
		::AfxMessageBox("Not surport yet!");
		return;
	}
	else 
		_triggerMode = select;
	//
	int ret = HBI_UpdateTriggerMode(m_pFpd, _triggerMode);
	if (ret == 0) {
		printf("OnBnClickedBtnSetFirmwareWorkmode:\n\tHBI_UpdateTriggerMode success!\n");
	}
	else {
		::AfxMessageBox("HBI_UpdateTriggerMode failed!");
	}
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedBtnCorrectEnable()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	printf("set correct enable begin!\n");

	if (m_pFpd == NULL) {
		printf("δ����״̬!\n");
		return;
	}

	// ����SDK��������У��ʹ��
	IMAGE_CORRECT_ENABLE *pcorrect = new IMAGE_CORRECT_ENABLE;
	if (pcorrect != NULL) {
#if 0
		pcorrect->bFeedbackCfg = false; // false-ECALLBACK_TYPE_SET_CFG_OK Event
#else
		pcorrect->bFeedbackCfg = true;  // //true-ECALLBACK_TYPE_ROM_UPLOAD Event,
#endif
		pcorrect->ucOffsetCorrection = ((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_OFFSET))->GetCurSel();
		pcorrect->ucGainCorrection = ((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_GAIN))->GetCurSel();
		pcorrect->ucDefectCorrection = ((CComboBox *)GetDlgItem(IDC_COMBO_ENABLE_DEFECT))->GetCurSel();
		pcorrect->ucDummyCorrection = 0;
		int ret = HBI_UpdateCorrectEnable(m_pFpd, pcorrect);
		if (ret == 0)
			printf("OnBnClickedBtnCorrectEnable:\n\tHBI_UpdateCorrectEnable success!\n");
		else
			::AfxMessageBox("HBI_QuckInitDllCfg failed!");
		delete pcorrect;
		pcorrect = NULL;
	}
	else {
		::AfxMessageBox("malloc IMAGE_CORRECT_ENABLE failed!");
	}
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonSetGainMode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	printf("set Firmware gain mode begin!\n");
	if (m_pFpd == NULL) {
		printf("δ����״̬!\n");
		return;
	}
	/*
	[n]-Invalid
	[1]-0.6pC
	[2]-1.2pC
	[3]-2.4pC
	[4]-3.6pC
	[5]-4.8pC
	[6]-7.2pC
	[7]-9.6pC
	*/
	int nGainMode = ((CComboBox *)GetDlgItem(IDC_COMBO_PGA_LEVEL))->GetCurSel();
	int ret = HBI_SetGainMode(m_pFpd, nGainMode);
	if (ret == 0) {
		printf("OEMFPD_SET_GAIN_MODE:\n\tOEMFPD_SET_GAIN_MODE success!\n");
	}
	else {
		::AfxMessageBox("OEMFPD_SET_GAIN_MODE failed!");
	}
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonGetGainMode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	printf("get Firmware gain mode begin!\n");
	if (m_pFpd == NULL) {
		::AfxMessageBox("m_pFpd is NULL!");
		return;
	}

	/*
	* ������ģʽ�����޸��⣬����ֵ����
	[n]-Invalid
	[1]-0.6pC
	[2]-1.2pC
	[3]-2.4pC
	[4]-3.6pC
	[5]-4.8pC
	[6]-7.2pC
	[7]-9.6pC
	*/
	int gainmode = HBI_GetGainMode(m_pFpd);
	if (gainmode == 1)
		printf("HBI_GetGainMode [1]-0.6pC\n");
	else if (gainmode == 2)
		printf("HBI_GetGainMode [2]-1.2pC\n");
	else if (gainmode == 3)
		printf("HBI_GetGainMode [3]-2.4pC\n");
	else if (gainmode == 4)
		printf("HBI_GetGainMode [4]-3.6pC\n");
	else if (gainmode == 5)
		printf("HBI_GetGainMode [5]-4.8pC\n");
	else if (gainmode == 6)
		printf("HBI_GetGainMode [6]-7.2pC\n");
	else if (gainmode == 7)
		printf("HBI_GetGainMode [7]-9.6pC\n");
	else {
		printf("HBI_GetGainMode Invalid.\n\tgainmode=%d\n", gainmode);
	}
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonGetLiveAcqTime()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_pFpd == NULL) {
		::AfxMessageBox("m_pFpd is NULL!");
		return;
	}

	int time = 0;
	int ret = HBI_GetAcqSpanTm(m_pFpd, &time);
	if (ret)
		::AfxMessageBox("HBI_SetAcqSpanTm failed!");
	printf("HBI_GeAcqSpanTm:time=%d\n", time);
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonSetPrepareTime()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->UpdateData(TRUE);

	if (m_pFpd == NULL) {
		::AfxMessageBox("m_pFpd is NULL!");
		return;
	}
	if (!IsOpen) {
		::AfxMessageBox(_T("warning:fpd is disconnect!"));
		return;
	}

	int ret = HBI_SetPreAcqTm(m_pFpd, m_uPrepareTm);
	if (ret != ::HBI_SUCCSS)
		::AfxMessageBox("HBI_SetAcqSpanTm failed!");
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonPrepare()
{
	// TODO: Add your control notification handler code here
	if (m_pFpd == NULL) {
		::AfxMessageBox("m_pFpd is NULL!");
		return;
	}
	if (!IsOpen) {
		::AfxMessageBox(_T("warning:fpd is disconnect!"));
		return;
	}
	int ret = HBI_Prepare(m_pFpd);
	if (ret != ::HBI_SUCCSS)
		::AfxMessageBox("HBI_SetAcqSpanTm failed!");
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonSetLiveAcquisitionTm()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
    this->UpdateData(TRUE);

	if (m_pFpd == NULL) {
		::AfxMessageBox("m_pFpd is NULL!");
		return;
	}
	int time = m_uAcqInterval;
	printf("HBI_SetAcqSpanTm:time=%d\n", time);
	int ret = HBI_SetAcqSpanTm(m_pFpd, time);
	if (ret)
		::AfxMessageBox("HBI_SetAcqSpanTm failed!");
}


void CHB_SDK_DEMO2008Dlg::OnBnClickedButtonLiveAcquisitionTm()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_pFpd == NULL) {
		::AfxMessageBox("m_pFpd is NULL!");
		return;
	}
	memset(szSeiralNum, 0x00, 16);
	int ret = HBI_GetFPDSerialNumber(m_pFpd, szSeiralNum);
	if (ret)
		::AfxMessageBox("HBI_GetFPDSerialNumber failed!");
	printf("HBI_GetFPDSerialNumber:szSeiralNum=%s\n", szSeiralNum);
}

bool CHB_SDK_DEMO2008Dlg::GetFirmwareConfig()
{
	printf("get all firmware config info!\n");
	if (m_pFpd == NULL) {
		printf("δ����״̬!\n");
		return false;
	}
	if (m_pLastRegCfg == NULL)
		m_pLastRegCfg = new RegCfgInfo;
	if (m_pLastRegCfg == NULL) {
		printf("m_pLastRegCfg is NULL!\n");
		return false;
	}
	// ���ԣ����㣬�����
	memset(m_pLastRegCfg, 0x00, sizeof(RegCfgInfo));
	/*
	* ��ȡ�̼����������Ӻ󼴿ɻ�ȡ����
	*/
	int ret = HBI_GetDevCfgInfo(m_pFpd, m_pLastRegCfg);
	if (!ret) {
		printf("HBI_GetDevCfgInfo:width=%d,height=%d\n", img_pro.nwidth, img_pro.nheight);
		/*
		* ip and port
		*/
		// �ߵ�λ��Ҫת��
		unsigned short usValue = ((m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort & 0xff) << 8) | ((m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort >> 8) & 0xff);
		printf("\tSourceIP:%d.%d.%d.%d:%u\n",
			m_pLastRegCfg->m_EtherInfo.m_byDestIP[0],
			m_pLastRegCfg->m_EtherInfo.m_byDestIP[1],
			m_pLastRegCfg->m_EtherInfo.m_byDestIP[2],
			m_pLastRegCfg->m_EtherInfo.m_byDestIP[3],
			usValue);
		usValue = ((m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort & 0xff) << 8) | ((m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort >> 8) & 0xff);
		printf("\tDestIP:%d.%d.%d.%d:%u\n",
			m_pLastRegCfg->m_EtherInfo.m_bySourceIP[0],
			m_pLastRegCfg->m_EtherInfo.m_bySourceIP[1],
			m_pLastRegCfg->m_EtherInfo.m_bySourceIP[2],
			m_pLastRegCfg->m_EtherInfo.m_bySourceIP[3],
			usValue);
		/*
		* width and hight
		* modified by mhyang 20191224
		*/
		// 0x01-4343,0x02-3543
		if (m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize >= 0x01 && m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize <= 0x06) {
			if (m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x01)
				printf("\tPanelSize:0x%02x,fpd type:4343\n", m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
			else
				printf("\tPanelSize:0x%02x,fpd type:3543\n", m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
		}
		else {
			printf("\tErr:fpd property:Do not know!fpd type:0x%02x\n", m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
		}
		if (m_imgW != m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth) m_imgW = m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth;
		if (m_imgH != m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight) m_imgH = m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight;
		printf("\twidth=%d,hight=%d\n", m_imgW, m_imgH);
		printf("\tdatatype is unsigned char.\n");
		printf("\tdatabit is 16bits.\n");
		printf("\tdata is little endian.\n");
		/*
		* workmode
		*/
		if (m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x01)
			printf("\tstatic software trigger.[0x%02x]\n", m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
		else if (m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x03)
			printf("\tstatic hvg trigger.[0x%02x]\n", m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
		else
			printf("\tother trigger mode.[0x%02x]\n", m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
		printf("\tPre Acquisition Delay Time.%u\n", m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime);
		/*
		* correction enable
		*/
		if (m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection == 0x01)
			printf("\tFirmware offset correction disenable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection);
		else if (m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection == 0x02)
			printf("\tFirmware offset correction enable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection);
		else
			printf("\tFirmware other offset correction enable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection);
		if (m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection == 0x01)
			printf("\tFirmware gain correction disenable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection);
		else if (m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection == 0x02)
			printf("\tFirmware gain correction enable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection);
		else
			printf("\tFirmware gain offset correction enable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection);
		if (m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection == 0x01)
			printf("\tFirmware defect correction disenable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection);
		else if (m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection == 0x02)
			printf("\tFirmware defect correction enable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection);
		else
			printf("\tFirmware defect offset correction enable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection);
		if (m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection == 0x01)
			printf("\tFirmware Dummy correction disenable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection);
		else if (m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection == 0x02)
			printf("\tFirmware Dummy correction enable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection);
		else
			printf("\tFirmware Dummy offset correction enable.[0x%02x]\n", m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection);

		if (fpd_base != NULL) {
			/*
			* ip and port
			*/
			// �ߵ�λ��Ҫת��
			unsigned short usValue = ((m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort & 0xff) << 8) | ((m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort >> 8) & 0xff);
			printf("\tSourceIP:%d.%d.%d.%d:%u\n",
				m_pLastRegCfg->m_EtherInfo.m_byDestIP[0],
				m_pLastRegCfg->m_EtherInfo.m_byDestIP[1],
				m_pLastRegCfg->m_EtherInfo.m_byDestIP[2],
				m_pLastRegCfg->m_EtherInfo.m_byDestIP[3],
				usValue/*m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort*/);

			usValue = ((m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort & 0xff) << 8) | ((m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort >> 8) & 0xff);
			printf("\tDestIP:%d.%d.%d.%d:%u\n",
				m_pLastRegCfg->m_EtherInfo.m_bySourceIP[0],
				m_pLastRegCfg->m_EtherInfo.m_bySourceIP[1],
				m_pLastRegCfg->m_EtherInfo.m_bySourceIP[2],
				m_pLastRegCfg->m_EtherInfo.m_bySourceIP[3],
				usValue/*m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort*/);

			int j = sprintf(fpd_base->DstIP, "%d.%d.%d.%d",
				m_pLastRegCfg->m_EtherInfo.m_byDestIP[0],
				m_pLastRegCfg->m_EtherInfo.m_byDestIP[1],
				m_pLastRegCfg->m_EtherInfo.m_byDestIP[2],
				m_pLastRegCfg->m_EtherInfo.m_byDestIP[3]);
			fpd_base->DstIP[j] = '\0';

			j = sprintf(fpd_base->SrcIP, "%d.%d.%d.%d",
				m_pLastRegCfg->m_EtherInfo.m_bySourceIP[0],
				m_pLastRegCfg->m_EtherInfo.m_bySourceIP[1],
				m_pLastRegCfg->m_EtherInfo.m_bySourceIP[2],
				m_pLastRegCfg->m_EtherInfo.m_bySourceIP[3]);
			fpd_base->SrcIP[j] = '\0';

			// �ߵ�λ��Ҫת��
			usValue = ((m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort & 0xff) << 8) | ((m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort >> 8) & 0xff);
			fpd_base->DstPort = usValue/*m_pLastRegCfg->m_EtherInfo.m_sDestUDPPort*/;

			usValue = ((m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort & 0xff) << 8) | ((m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort >> 8) & 0xff);
			fpd_base->SrcPort = usValue/*m_pLastRegCfg->m_EtherInfo.m_sSourceUDPPort*/;

			/*
			* width and hight
			*/
			if (m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize >= 0x01 && m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize <= 0x02) {
				if (m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize == 0x01)
					printf("\tPanelSize:0x%02x,fpd type:4343\n", m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
				else
					printf("\tPanelSize:0x%02x,fpd type:3543\n", m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
			}
			else {
				printf("\tErr:fpd property:Do not know!fpd type:0x%02x\n", m_pLastRegCfg->m_SysBaseInfo.m_byPanelSize);
			}

			if (m_imgW != m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth) m_imgW = m_pLastRegCfg->m_SysBaseInfo.m_sImageWidth;
			if (m_imgH != m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight) m_imgH = m_pLastRegCfg->m_SysBaseInfo.m_sImageHeight;
			printf("\twidth=%d,hight=%d\n", m_imgW, m_imgH);
			printf("\tdatatype is unsigned char.\n");
			printf("\tdatabit is 16bits.\n");
			printf("\tdata is little endian.\n");

			/*
			* workmode
			*/
			if (m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x01)
				printf("\tstatic software trigger.[0x%02x]\n", m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
			else if (m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x03)
				printf("\tstatic hvg trigger.[0x%02x]\n", m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
			else if (m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode == 0x04)
				printf("\tFreeAED trigger mode.[0x%02x]\n", m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);
			else
				printf("\tother trigger mode.[0x%02x]\n", m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode);

			fpd_base->trigger_mode = m_pLastRegCfg->m_SysCfgInfo.m_byWorkMode;

			printf("\tPre Acquisition Delay Time.%u\n", m_pLastRegCfg->m_SysCfgInfo.m_unPreAcquisitionDelayTime);
			/*
			* correction enable
			*/
			if (fpd_base->offset_enable != m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection)
				fpd_base->offset_enable = m_pLastRegCfg->m_ImgCaliCfg.m_byOffsetCorrection;

			if (fpd_base->gain_enable = m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection)
				fpd_base->gain_enable = m_pLastRegCfg->m_ImgCaliCfg.m_byGainCorrection;

			if (fpd_base->defect_enable = m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection)
				fpd_base->defect_enable = m_pLastRegCfg->m_ImgCaliCfg.m_byDefectCorrection;

			if (fpd_base->dummy_enable = m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection)
				fpd_base->dummy_enable = m_pLastRegCfg->m_ImgCaliCfg.m_byDummyCorrection;

			if (0x00 == fpd_base->offset_enable)
				printf("\tNo Offset Correction\n");
			else if (0x01 == fpd_base->offset_enable)
				printf("\tSoftware Offset Correction\n");
			else if (0x02 == fpd_base->offset_enable)
				printf("\tFirmware PostOffset Correction\n");
			else if (0x03 == fpd_base->offset_enable)
				printf("\tFirmware PreOffset Correction\n");
			else
				printf("\tInvalid Offset Correction\n");

			if (0x00 == fpd_base->gain_enable)
				printf("\tNo Gain Correction\n");
			else if (0x01 == fpd_base->gain_enable)
				printf("\tSoftware Gain Correction\n");
			else if (0x02 == fpd_base->gain_enable)
				printf("\tFirmware Gain Correction\n");
			else
				printf("\tInvalid Gain Correction\n");

			if (0x00 == fpd_base->defect_enable)
				printf("\tNo Defect Correction\n");
			else if (0x01 == fpd_base->defect_enable)
				printf("\tSoftware Defect Correction\n");
			else if (0x02 == fpd_base->defect_enable)
				printf("\tFirmware Defect Correction\n");
			else
				printf("\tInvalid Defect Correction\n");

			if (0x00 == fpd_base->dummy_enable)
				printf("\tNo Dummy Correction\n");
			else if (0x01 == fpd_base->dummy_enable)
				printf("\tSoftware Dummy Correction\n");
			else if (0x02 == fpd_base->dummy_enable)
				printf("\tFirmware Dummy Correction\n");
			else
				printf("\tInvalid Dummy Correction\n");

			//
			printf("\tnPGA=%d\n",           fpd_base->nPGA);
			printf("\tnBinning=%d\n",       fpd_base->nBinning);
			printf("\tm_uPrepareTm=%u\n",      m_uPrepareTm);
			printf("\tm_uAcqInterval=%u\n", m_uAcqInterval);
		}
		//// ������ʾ��Ϣ
		//PostMessage(WM_USER_CURR_FPD_INFO, (WPARAM)0, (LPARAM)0);
	}
	else {
		printf("��ȡ�̼�����ʧ��!\n");
		return false;
	}

	return true;
}