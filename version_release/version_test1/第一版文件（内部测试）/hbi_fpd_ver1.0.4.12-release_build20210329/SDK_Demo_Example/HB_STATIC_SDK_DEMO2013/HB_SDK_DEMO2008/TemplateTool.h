#pragma once

//typedef enum {
//	OFFSET_TEMPLATE=0X00,
//    GAIN_TEMPLATE,
//	DEFECT_ACQ_GROUP1,
//	DEFECT_ACQ_GROUP2,
//	DEFECT_ACQ_AND_TEMPLATE,
//}EnumGENERATE_TEMPLATE;

// CTemplateTool dialog

class CTemplateTool : public CDialog
{
	DECLARE_DYNAMIC(CTemplateTool)

public:
	CTemplateTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTemplateTool();

// Dialog Data
	enum { IDD = IDD_QGENERATE_TEMPLATE_TOOL };
	HICON m_hIcon;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioOffset();
	afx_msg void OnBnClickedRadioGain();
	afx_msg void OnBnClickedRadioDefect();
	afx_msg void OnBnClickedBtnTemplateGenerate();
	//
	afx_msg LRESULT OnGenerateTemplateResult(WPARAM wparam, LPARAM lparam = 0);

public:
	int DoOffsetTemp();
	int DoGainTemp();
	int DoDefectTemp();
	int DoDefectGroup1();
	int DoDefectGroup2();
	int DoDefectGroup3();

public:
	EnumIMAGE_ACQ_MODE enumTemplateType;
	/*  
	* defect ÁÁ³¡Í¼×éºÅ
	* 0-first group acq
	* 1-second group acq
	* 2-third group acq and generate template
	*/ 
	unsigned int m_ungroupid;
};
