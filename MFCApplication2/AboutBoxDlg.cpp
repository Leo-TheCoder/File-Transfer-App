// AboutBoxDlg.cpp : implementation file
//

#include "pch.h"
#include "FileClient.h"
#include "AboutBoxDlg.h"
#include "afxdialogex.h"


// AboutBoxDlg dialog

IMPLEMENT_DYNAMIC(AboutBoxDlg, CDialog)

AboutBoxDlg::AboutBoxDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ABOUTBOX, pParent)
{

}

AboutBoxDlg::~AboutBoxDlg()
{
}

void AboutBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AboutBoxDlg, CDialog)
END_MESSAGE_MAP()


// AboutBoxDlg message handlers
