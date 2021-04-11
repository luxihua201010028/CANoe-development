
// FDX_DemoDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CFDX_DemoDlg dialog
class CFDX_DemoDlg : public CDialogEx
{
// Construction
public:
	CFDX_DemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FDX_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CSliderCtrl m_Eng_Speed;
	CSliderCtrl m_Veh_Speed;
	afx_msg void OnCustomdrawSliderEngSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawSliderVehSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedExchange();
	CButton m_btnExchange;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnRadioAccesswrite();
	afx_msg void OnRadioAccessread();
	afx_msg void OnRadioDataread();
	afx_msg void OnRadioDatawrite();
	CString m_status;
	afx_msg void OnBnClickedStart();
	CButton m_btnStart;
	afx_msg void OnBnClickedCheckHazards();
	CButton m_chkHazards;
	int m_vehspeed;
	int m_engspeed;
};
