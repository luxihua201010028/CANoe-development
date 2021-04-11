
// FDX_Demo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CFDX_DemoApp:
// See FDX_Demo.cpp for the implementation of this class
//

class CFDX_DemoApp : public CWinApp
{
public:
	CFDX_DemoApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFDX_DemoApp theApp;