
// MFCApplication2.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CFileClientApp:
// See MFCApplication2.cpp for the implementation of this class
//

class CFileClientApp : public CWinApp
{
public:
	CFileClientApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFileClientApp theApp;
