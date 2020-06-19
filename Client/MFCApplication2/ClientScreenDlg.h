#pragma once

#include "FileClientDlg.h"
#include "AboutBoxDlg.h"
// ClientScreenDlg dialog

class ClientScreenDlg : public CDialog
{
	DECLARE_DYNAMIC(ClientScreenDlg)

public:
	ClientScreenDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClientScreenDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ClientScreen };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	string ip;
	SOCKET server;
	afx_msg void OnBnClickedbtnlogin();
	CStatic staticLoginResult;
	afx_msg void OnBnClickedAboutbox();
	afx_msg void OnBnClickedbtnrefreshfilelist();
	afx_msg void OnBnClickedbtnrefreshlog();
	afx_msg void OnBnClickedbtnregister();
	CListBox userList;
	CListBox fileList;
	afx_msg void OnBnClickedbtnupload();
	CStatic txtTest;
	afx_msg void OnBnClickedbtndownload();
};