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
	DDX_Control(pDX, IDC_usersLog, userList);
	DDX_Control(pDX, IDC_fileList, fileList);
	DDX_Control(pDX, IDC_txtTest, txtTest);
}


BEGIN_MESSAGE_MAP(ClientScreenDlg, CDialog)
	
	ON_BN_CLICKED(IDC_btnLogIn, &ClientScreenDlg::OnBnClickedbtnlogin)
	ON_BN_CLICKED(IDD_ABOUTBOX, &ClientScreenDlg::OnBnClickedAboutbox)
	ON_BN_CLICKED(IDC_btnRefreshFileList, &ClientScreenDlg::OnBnClickedbtnrefreshfilelist)
	ON_BN_CLICKED(IDC_btnRefreshLog, &ClientScreenDlg::OnBnClickedbtnrefreshlog)
	ON_BN_CLICKED(IDC_btnRegister, &ClientScreenDlg::OnBnClickedbtnregister)
	ON_BN_CLICKED(IDC_btnUpload, &ClientScreenDlg::OnBnClickedbtnupload)
	ON_BN_CLICKED(IDC_btnDownload, &ClientScreenDlg::OnBnClickedbtndownload)
END_MESSAGE_MAP()


// ClientScreenDlg message handlers


void ClientScreenDlg::OnBnClickedbtnlogin()
{
	string choice = "1";
	int iResult = send(server, choice.c_str(), choice.length() + 1, 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {
		staticLoginResult.SetWindowText("Send failed!\n");
		return;
	}

	txtTest.SetWindowText(" ");
	CString textInput;
	GetDlgItemText(IDC_username, textInput);
	string username = textInput;
	GetDlgItemText(IDC_password, textInput);
	string password = textInput;
	
	txtTest.SetWindowText(" ");
	int checkLogIn = LogIn(server, username, password);
	switch(checkLogIn) {
	case -1: {
		staticLoginResult.SetWindowText("Send failed!\n");
		GetDlgItem(IDC_btnLogIn)->EnableWindow(FALSE);
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_password)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnRegister)->EnableWindow(FALSE);
		return;
	}
	case 0: {
		staticLoginResult.SetWindowText("Wrong username or password!\n");
		GetDlgItem(IDC_btnLogIn)->EnableWindow(FALSE);
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_password)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnRegister)->EnableWindow(FALSE);
		return;
	}
	case 1: {
		staticLoginResult.SetWindowText("Logged in!\n");
		GetDlgItem(IDC_btnLogIn)->EnableWindow(FALSE);
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_password)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnRegister)->EnableWindow(FALSE);

		GetDlgItem(IDC_btnDownload)->EnableWindow(TRUE);
		GetDlgItem(IDC_btnRefreshFileList)->EnableWindow(TRUE);
		GetDlgItem(IDC_btnRefreshLog)->EnableWindow(TRUE);
		GetDlgItem(IDC_btnUpload)->EnableWindow(TRUE);
		return;
	}
	case 2: {
		staticLoginResult.SetWindowText("This account has already been used! Try again later!\n");
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
	fileList.SetRedraw(FALSE);
	fileList.ResetContent();
	fileList.SetRedraw(TRUE);

	string flag = "refreshfilelist";
	int iResult = send(server, flag.c_str(), flag.length() + 1, 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {
		return;
	}

	char buf[1024];
	fileList.SetRedraw(FALSE);
	while (1) {
		iResult = recv(server, buf, sizeof(buf), 0);

		if (iResult == 0 || iResult == SOCKET_ERROR) {
			return;
		}

		char tmp[1024];
		strcpy(tmp, "");
		strcpy(tmp, buf);
		if (strcmp(tmp, "endlist") == 0)	//Nếu server gửi endlog thì dừng
		{
			break;
		}

		int i = 0;
		LPCTSTR logString = (LPCTSTR)buf;
		fileList.AddString(logString);
		//userLog.InsertItem(0, logString);
		//staticLoginResult.SetWindowText(L"hello");
	}

	fileList.SetRedraw(TRUE);
	return;
}

//refresh log
void ClientScreenDlg::OnBnClickedbtnrefreshlog()
{
	userList.SetRedraw(FALSE);
	userList.ResetContent();
	userList.SetRedraw(TRUE);

	string flag = "refreshlog";
	int iResult = send(server, flag.c_str(), flag.length() + 1, 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {
		return;
	}

	char buf[1024];
	userList.SetRedraw(FALSE);
	while (1) {
		iResult = recv(server, buf, sizeof(buf), 0);

		if (iResult == 0 || iResult == SOCKET_ERROR) {
			return;
		}

		char tmp[1024];
		strcpy(tmp, "");
		strcpy(tmp, buf);
		if (strcmp(tmp, "endlog") == 0)	//Nếu server gửi endlog thì dừng
		{
			break;
		}

		int i = 0;
		LPCTSTR logString = (LPCTSTR)buf;
		userList.AddString(logString);
		//userLog.InsertItem(0, logString);
		//staticLoginResult.SetWindowText(L"hello");
	}
	
	userList.SetRedraw(TRUE);
	return;
}

//khi nhấn nút đăng ký
void ClientScreenDlg::OnBnClickedbtnregister()
{
	txtTest.SetWindowText(" ");

	string choice = "0";
	int iResult = send(server, choice.c_str(), choice.length() + 1, 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {
		staticLoginResult.SetWindowText("Send failed!");
		return;
	}

	CString textInput;
	GetDlgItemText(IDC_username, textInput);
	string username = textInput;
	GetDlgItemText(IDC_password, textInput);
	string password = textInput;

	txtTest.SetWindowText(" ");
	int checkRegister = Register(server, username, password);
	switch (checkRegister) {
	case -1: {
		staticLoginResult.SetWindowText("Send failed!");
		GetDlgItem(IDC_btnLogIn)->EnableWindow(FALSE);
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_password)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnRegister)->EnableWindow(FALSE);
		return;
	}
	case 0: {
		staticLoginResult.SetWindowText("Username not available!");
		GetDlgItem(IDC_btnLogIn)->EnableWindow(FALSE);
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_password)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnRegister)->EnableWindow(FALSE);
		return;
	}
	case 1: {
		staticLoginResult.SetWindowText("Logged in!");
		GetDlgItem(IDC_btnLogIn)->EnableWindow(FALSE);
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_password)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnRegister)->EnableWindow(FALSE);

		GetDlgItem(IDC_btnDownload)->EnableWindow(TRUE);
		GetDlgItem(IDC_btnRefreshFileList)->EnableWindow(TRUE);
		GetDlgItem(IDC_btnRefreshLog)->EnableWindow(TRUE);
		GetDlgItem(IDC_btnUpload)->EnableWindow(TRUE);
		return;
	}
	}
}


void ClientScreenDlg::OnBnClickedbtnupload()
{
	txtTest.SetWindowText(" ");
	string choice = "upload";
	int iResult = send(server, choice.c_str(), choice.length() + 1, 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {
		MessageBox(_T("Send failed!"), _T("Send result"), MB_OK | NULL);
		return;
	}

	CString textInput;
	GetDlgItemText(IDC_txtFileName, textInput);
	string filename = (string)textInput;

	txtTest.SetWindowText(" ");
	iResult = FileSend(server, filename);
	switch (iResult) {
	case -1: {
		MessageBox(_T("Upload failed!"), _T("Upload result"), MB_OK | NULL);
		return;
	}
	case 1: {
		MessageBox(_T("Uploaded!"), _T("Upload result"), MB_OK | NULL);
		return;
	}
	case 2: {
		MessageBox(_T("File too big!"), _T("Upload result"), MB_OK | NULL);
		return;
	}
	}
}


void ClientScreenDlg::OnBnClickedbtndownload()
{
	txtTest.SetWindowText(" ");
	string choice = "download";
	int iResult = send(server, choice.c_str(), choice.length() + 1, 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {
		MessageBox(_T("Send failed!"), _T("Send result"), MB_OK | NULL);
		return;
	}

	CString textInput;
	GetDlgItemText(IDC_txtFileName, textInput);
	string filename = (string)textInput;

	txtTest.SetWindowText(" ");
	//send filename
	iResult = send(server, filename.c_str(), filename.length() + 1, 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {
		MessageBox(_T("Send failed!"), _T("Send result"), MB_OK | NULL);
		return;
	}

	iResult = FileRecv(server);
	switch (iResult) {
	case -1: {
		MessageBox(_T("Download failed!"), _T("Download result"), MB_OK | NULL);
		return;
	}
	case 1: {
		MessageBox(_T("Downloaded!"), _T("Download result"), MB_OK | NULL);
		return;
	}
	}
}
