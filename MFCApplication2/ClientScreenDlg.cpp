// ClientScreenDlg.cpp : implementation file
//
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "FileClient.h"
#include "ClientScreenDlg.h"
#include "afxdialogex.h"

#include "Client.h"

// ClientScreenDlg dialog

IMPLEMENT_DYNAMIC(ClientScreenDlg, CDialog)

ClientScreenDlg::ClientScreenDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ClientScreen, pParent)
{

}

ClientScreenDlg::~ClientScreenDlg()
{
}

void ClientScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_loginResult, staticLoginResult);
	DDX_Control(pDX, IDC_usersLog, userLog);
}


BEGIN_MESSAGE_MAP(ClientScreenDlg, CDialog)
	
	ON_BN_CLICKED(IDC_btnLogIn, &ClientScreenDlg::OnBnClickedbtnlogin)
	ON_BN_CLICKED(IDD_ABOUTBOX, &ClientScreenDlg::OnBnClickedAboutbox)
	ON_BN_CLICKED(IDC_btnRefreshFileList, &ClientScreenDlg::OnBnClickedbtnrefreshfilelist)
	ON_BN_CLICKED(IDC_btnRefreshLog, &ClientScreenDlg::OnBnClickedbtnrefreshlog)
END_MESSAGE_MAP()


// ClientScreenDlg message handlers


void ClientScreenDlg::OnBnClickedbtnlogin()
{
	CString textInput;
	GetDlgItemText(IDC_username, textInput);
	string username(CW2A(textInput.GetString()));
	GetDlgItemText(IDC_password, textInput);
	string password(CW2A(textInput.GetString()));
	
	int checkLogIn = LogIn(server, username, password);
	switch(checkLogIn) {
	case -1: {
		staticLoginResult.SetWindowText(L"Send failed!\n");
		GetDlgItem(IDC_btnLogIn)->EnableWindow(FALSE);
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_password)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnRegister)->EnableWindow(FALSE);
		return;
	}
	case 0: {
		staticLoginResult.SetWindowText(L"Wrong username or password!\n");
		GetDlgItem(IDC_btnLogIn)->EnableWindow(FALSE);
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_password)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnRegister)->EnableWindow(FALSE);
		return;
	}
	case 1: {
		staticLoginResult.SetWindowText(L"Logged in!\n");
		GetDlgItem(IDC_btnLogIn)->EnableWindow(FALSE);
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_password)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnRegister)->EnableWindow(FALSE);
		return;
	}
	case 2: {
		staticLoginResult.SetWindowText(L"This account has already been used! Try again later!\n");
		GetDlgItem(IDC_btnLogIn)->EnableWindow(FALSE);
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_password)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnRegister)->EnableWindow(FALSE);
		return;
	}
	}

}


//chuyện sẽ xảy ra khi bấm nút About
void ClientScreenDlg::OnBnClickedAboutbox()
{
	AboutBoxDlg dlg;
	dlg.DoModal();
}



//refresh file list
void ClientScreenDlg::OnBnClickedbtnrefreshfilelist()
{
	
}

//refresh log
void ClientScreenDlg::OnBnClickedbtnrefreshlog()
{
	char* flag = "refreshlog";
	int iResult = send(server, flag, sizeof(flag), 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {
		return;
	}
	char buf[1024];
	while (1) {
		iResult = recv(server, buf, sizeof(buf), 0);
		if (iResult == 0 || iResult == SOCKET_ERROR) {
			return;
		}
		LPCTSTR logString = (LPCTSTR)buf;
		userLog.InsertItem(0, logString);
	}
	return;
}
