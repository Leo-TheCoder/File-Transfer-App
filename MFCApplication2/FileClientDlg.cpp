#define _WINSOCK_DEPRECATED_NO_WARNINGS
// MFCApplication2Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FileClient.h"
#include "FileClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "ClientScreenDlg.h"


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileClientDlg dialog



CFileClientDlg::CFileClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2_DIALOG, pParent)
	, txtIP(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, txtIP);
	DDX_Control(pDX, IDC_result, staResult);
}

BEGIN_MESSAGE_MAP(CFileClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CFileClientDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK, &CFileClientDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFileClientDlg message handlers

BOOL CFileClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileClientDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFileClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFileClientDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CFileClientDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//--------------------------------Init socket

	WORD wVersionRequested;
	WSADATA wsaData;
	int retcode;

	SOCKET socket_descriptor;		//Socket của client
	struct sockaddr_in sin;			//Địa chỉ của client

	char Buffer[4096];
	int length;


	/*
	[1] WSAStartup() must be called before any other socket
	routines.
	*/
	wVersionRequested = MAKEWORD(2, 2);	// Use MAKEWORD(1,1) if you're at WinSock 1.1
	retcode = WSAStartup(wVersionRequested, &wsaData);
	if (retcode != 0)
	{
		//printf("Startup failed: %d\n", retcode);
		//return 1;
	}


	/*
	[2] Once WSAStartup has been called, the socket can be
	created using the socket() call.  The following creates an
	Internet protocol family (PF_INET) socket providing stream
	service (SOCK_STREAM).
	*/

	printf("socket()\n");
	socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_descriptor == INVALID_SOCKET)
	{
		//printf("Can't create the client's socket!\n");
		//return 1;
	}


	/*
	[3] Before making a connection, an Internet address
	family structure must be initialized.
	*/

	memset(&sin, 0, sizeof(sin));	//Initialize sockadrr
	sin.sin_family = AF_INET;		//Client IP's type = IPv4
	sin.sin_port = htons(5800);		//The port number is 5800

	//------------------------------------------

	CString textInput;
	GetDlgItemText(IDC_EDIT1, textInput);
	string ip(CW2A(textInput.GetString()));

	sin.sin_addr.s_addr = inet_addr(ip.c_str());	//Truyền IP đó vào sin
	//------------WARNING------------------
	//--- Thêm xử lí việc nhập tào lao-----
	//--- Dẫn đến client bị đứng ----------
	//-------------------------------------


	/*
	[4] connect() is used to establish a connection to a remote
	endpoint.
	*/

	//printf("connect()\n");

	retcode = connect(socket_descriptor, (struct sockaddr*)&sin, sizeof(sin));
	if (retcode == SOCKET_ERROR)
	{
		//txt.LoadString(IDC_STATIC2);
		staResult.SetWindowText(L"Cannot connect to this server!");
		//printf("Can't connect to the server!\n");
		return;
	}
	//staResult.SetWindowText(L"Connected!");

	ClientScreenDlg clientScreen;
	clientScreen.ip = ip;
	clientScreen.server = socket_descriptor;
	clientScreen.DoModal();

	closesocket(socket_descriptor);

}
