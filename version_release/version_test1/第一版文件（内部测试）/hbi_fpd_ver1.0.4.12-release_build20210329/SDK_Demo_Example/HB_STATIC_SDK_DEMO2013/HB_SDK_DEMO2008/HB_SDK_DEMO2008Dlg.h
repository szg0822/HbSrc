
// HB_SDK_DEMO2008Dlg.h : header file
//

#pragma once

#include "TemplateTool.h"
#include "Cvvimage\\CvvImage.h"

#define WM_USER_CONNECT_STATUS_ICON   (WM_USER + 100)  // 连接状态图标
#define WM_USER_CURR_CONTROL_DATA     (WM_USER + 101)  // 更新控件信息
#define WM_OFFSET_DOWNLOAD_CB_MESSAGE (WM_USER + 102)  // 下载offset
#define WM_GAIN_DOWNLOAD_CB_MESSAGE   (WM_USER + 103)  // 下载gain
#define WM_DEFECT_DOWNLOAD_CB_MESSAGE (WM_USER + 104)  // 下载defect

#define MY_WINDOW_NAME "pic_window"
#define USER_WEI_MING 0

#define BREAK_UINT32(var, ByteNum) \
		(unsigned char)((unsigned int)(((var) >> ((ByteNum)* 8)) & 0x00FF))
#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
	((unsigned int)((unsigned int)((Byte0)& 0x00FF) \
	+ ((unsigned int)((Byte1)& 0x00FF) << 8) \
	+ ((unsigned int)((Byte2)& 0x00FF) << 16) \
	+ ((unsigned int)((Byte3)& 0x00FF) << 24)))

// CHB_SDK_DEMO2008Dlg dialog
class fpd_base_cfg_st;
class CHB_SDK_DEMO2008Dlg : public CDialog
{
// Construction
public:
	CHB_SDK_DEMO2008Dlg(CWnd* pParent = NULL);	// standard constructor
	~CHB_SDK_DEMO2008Dlg();
// Dialog Data
	enum { IDD = IDD_HB_SDK_DEMO2008_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HICON m_hConnIco;
	HICON m_hDisConnIco;
	HICON m_hReadyIco;
	HICON m_hbusyIco;
	HICON m_hprepareIco;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnAppAbout();
	afx_msg void OnBnClickedBtnConn();
	afx_msg void OnBnClickedBtnDisconn();
	afx_msg void OnBnClickedBtnLiveAcq();
	afx_msg void OnBnClickedBtnStopLiveAcq();
	afx_msg void OnBnClickedRadioShowPic();
	afx_msg void OnBnClickedRadioSavePic();
	afx_msg void OnFileTemplate();
	afx_msg void OnBnClickedButtonSingleShot();
	afx_msg void OnBnClickedButtonFirmwareVer();
	afx_msg void OnBnClickedButtonSoftwareVer();
	afx_msg void OnBnClickedBtnGetImageProperty();
	afx_msg void OnBnClickedButtonGetConfig();
	afx_msg void OnBnClickedBtnSetTriggerCorrection();
	afx_msg void OnBnClickedBtnSetTriggerMode();
	afx_msg void OnBnClickedBtnCorrectEnable();
	afx_msg void OnBnClickedButtonSetGainMode();
	afx_msg void OnBnClickedButtonGetGainMode();
	afx_msg void OnBnClickedButtonGetLiveAcqTime();
	afx_msg void OnBnClickedButtonSetPrepareTime();
	afx_msg void OnBnClickedButtonSetLiveAcquisitionTm();
	afx_msg void OnBnClickedButtonLiveAcquisitionTm();
	afx_msg void OnBnClickedButtonPrepare();
	// 自定义消息
	LRESULT OnUpdateConnectStatus(WPARAM wparam, LPARAM lparam);
	LRESULT OnUpdateCurControlData(WPARAM wparam, LPARAM lparam);

public:
	CSpinButtonCtrl m_ctlSpinAqcSum;
	CSpinButtonCtrl m_ctlSpinDiscardNum;
	CStatic m_ctlConnStatus;
	UINT m_uFrameNum;
	UINT m_uDiscardNum;
	UINT m_uPrepareTm;
	UINT m_uAcqInterval;

public:
#if USER_WEI_MING
	static int handleCommandEvent(void *pObj, unsigned char command, void *buff, int len, int nId);
#else
	static int handleCommandEvent(unsigned char command, void *buff, int len, int nId);
#endif
	static unsigned int __stdcall ShowThreadProcFun(LPVOID pParam);
	void conn_button_status();
	void disconn_button_status();
	bool GetFirmwareConfig();
	int SaveImage(unsigned char *imgbuff, int nbufflen);
	void InitCtrlData();
	int ShowImage(unsigned short *imgbuff, int nbufflen, int nframeid);
	void CloseShowThread();
	void UpdateImage();
	int GetPGA(unsigned short usValue);
	void AutoResize();

	// Add show window by mhyang 20201211
	bool bShowPic;
	int m_IsExitsSubWnd;
	CRect m_PicRect;
	IplImage* pIplimage; IplImage *desIplimage;
	int pic_width;
	int pic_height;
	double pic_factor;
	CvFont font;

//private:
public:
	void* m_pFpd;  
	RegCfgInfo* m_pLastRegCfg; // 固件配置
	FPD_AQC_MODE aqc_mode;     // 采集模式和参数，单帧采集、多帧采集、暗场图以及亮场图采集
	bool IsOpen;
	HWND m_templateHwnd;                             //快速生成模板窗口句柄
	int m_imgW, m_imgH;        // 图像分辨率
	char m_path[MAX_PATH];
	fpd_base_cfg_st *fpd_base;
	IMAGE_PROPERTY img_pro;
	char szSdkVer[128];
	char szFirmVer[128];
	char szSeiralNum[16];

public:
	CvvImage m_img;
	CWnd *pPicWnd;
	int m_frameid;
	HANDLE m_hEvent;
	volatile int RunFlag;
	HANDLE  m_Threadhdl;
	unsigned int m_uThreadFunID;
};

/* 平板基本信息 */
class fpd_base_cfg_st
{
	// 构造
public:
	fpd_base_cfg_st() 
	{
		memset(DstIP, 0x00, 16);
		strcpy(DstIP, "192.168.10.40");
		DstPort = 32897;
		//
		memset(SrcIP, 0x00, 16);
		strcpy(SrcIP, "192.168.10.20");
		SrcPort = 32896;
		//
		m_rawpixel_x = 3072;
		m_rawpixel_y = 3072;
		databit  = 0;
		datatype = 0;
		endian   = 0;
		//
		trigger_mode  = 1;
		offset_enable = 0;
		gain_enable   = 0;
		defect_enable = 0;
		dummy_enable  = 0;
		//
		nBinning = 1;
	    nPGA = 6;
	}
	~fpd_base_cfg_st(){};

	// 对话框数据
public:
	void PRINT_FPD_INFO()
	{
		printf("@=============================================\n");
		printf("Local  IP Addr:%s:%u\n", SrcIP, SrcPort);
		printf("Remote IP Addr:%s:%u\n", DstIP, DstPort);
		//
		printf("Image property:\n\twidth=%d,hight=%d\n", m_rawpixel_x, m_rawpixel_y);
		//
		if (datatype == 0) printf("\tdatatype is unsigned char.\n");
		else if (datatype == 1) printf("\tdatatype is char.\n");
		else if (datatype == 2) printf("\tdatatype is unsigned short.\n");
		else if (datatype == 3) printf("\tdatatype is float.\n");
		else if (datatype == 4) printf("\tdatatype is double.\n");
		else printf("\tdatatype is not support.\n");
		//
		if (databit == 0) printf("\tdatabit is 16bits.\n");
		else if (databit == 1) printf("\tdatabit is 14bits.\n");
		else if (databit == 2) printf("\tdatabit is 12bits.\n");
		else if (databit == 3) printf("\tdatabit is 8bits.\n");
		else printf("\tdatatype is unsigned char.\n");
		//
		if (endian == 0) printf("\tdata is little endian.\n");
		else printf("\tdata is bigger endian.\n");
		//
		if (trigger_mode == 0x01)
			printf("trigger_mode[0x%02x]:\n\tstatic software trigger.\n", trigger_mode);
		else if (trigger_mode == 0x03)
			printf("trigger_mode[0x%02x]:\n\tstatic hvg trigger.\n", trigger_mode);
		else
			printf("trigger_mode[0x%02x]:\n\tother trigger mode.\n", trigger_mode);
		//
		printf("Image Calibration:\n");
		if (0x00 == offset_enable)
			printf("\tNo Offset Correction\n");
		else if (0x01 == offset_enable)
			printf("\tSoftware Offset Correction\n");
		else if (0x02 == offset_enable)
			printf("\tFirmware PostOffset Correction\n");
		else if (0x03 == offset_enable)
			printf("\tFirmware PreOffset Correction\n");		
		else 
			printf("\tInvalid Offset Correction\n");

		if (0x00 == gain_enable)
			printf("\tNo Gain Correction\n");
		else if (0x01 == gain_enable)
			printf("\tSoftware Gain Correction\n");
		else if (0x02 == gain_enable)
			printf("\tFirmware Gain Correction\n");
		else
			printf("\tInvalid Gain Correction\n");

		if (0x00 == defect_enable)
			printf("\tNo Defect Correction\n");
		else if (0x01 == defect_enable)
			printf("\tSoftware Defect Correction\n");
		else if (0x02 == defect_enable)
			printf("\tFirmware Defect Correction\n");
		else
			printf("\tInvalid Defect Correction\n");

		if (0x00 == dummy_enable)
			printf("\tNo Dummy Correction\n");
		else if (0x01 == dummy_enable)
			printf("\tSoftware Dummy Correction\n");
		else if (0x02 == dummy_enable)
			printf("\tFirmware Dummy Correction\n");
		else
			printf("\tInvalid Dummy Correction\n");
		//
		if (0x00 == nPGA)
			printf("\tNo Dummy Correction\n");
		else if (0x01 == nPGA)
			printf("\tSoftware Dummy Correction\n");
		else if (0x02 == nPGA)
			printf("\tFirmware Dummy Correction\n");
		else
			printf("\tInvalid Dummy Correction\n");

		if (0x01 == nBinning)
			printf("\t0x01:1*1\n");
		else if (0x02 == nBinning)
			printf("\t0x02:2*2\n");
		else if (0x03 == nBinning)
			printf("\t0x03:3*3\n");
		else if (0x04 == nBinning)
			printf("\t0x04:4*4\n");
		else
			printf("\tInvalid Binning\n");
		printf("----------------------------------------------\n");
	}

public:
	char SrcIP[16];            // 本地IP
	char  DstIP[16];           // 远程IP
	unsigned short SrcPort;    // 本地PORT
	unsigned short DstPort;    // 远程PORT
	int m_rawpixel_x;                  // 平板width
	int m_rawpixel_y;                  // 平板hight
	int databit;
	int datatype;
	int endian;
	int trigger_mode;
	int offset_enable;
	int gain_enable;
	int defect_enable;
	int dummy_enable;
	//
	int nBinning;
	int nPGA;
//public:
//	int m_rawtype;
//	int m_rawpixel_x;
//	int m_rawpixel_y;
//	int m_rawoffset;
};