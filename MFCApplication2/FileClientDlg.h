#define _WINSOCK_DEPRECATED_NO_WARNINGS

// MFCApplication2Dlg.h : header file
//

#pragma once
#include "Client.h"


// CFileClientDlg dialog
class CFileClientDlg : public CDialogEx
{
// Construction
public:
	CFileClientDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION2_DIALOG };
#endif

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
	CString txtIP;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedOk();
	CStatic staResult;
};
