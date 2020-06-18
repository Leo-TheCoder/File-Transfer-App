#pragma once


// AboutBoxDlg dialog

class AboutBoxDlg : public CDialog
{
	DECLARE_DYNAMIC(AboutBoxDlg)

public:
	AboutBoxDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~AboutBoxDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
