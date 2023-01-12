
// SampleSandboxBrowser.h : main header file for the SampleSandboxBrowser application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CSandboxBrowserApp:
// See SampleSandboxBrowser.cpp for the implementation of this class
//

class CSandboxBrowserApp : public CWinApp
{
public:
	CSandboxBrowserApp() noexcept;

	CArray<HWND, HWND> m_aryFrames;

	CString m_UserName=L"";
	CString m_Password = L"";
public:

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
protected:
	HMENU  m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	afx_msg void OnAppAbout();
	afx_msg void OnFileNewFrame();
	DECLARE_MESSAGE_MAP()
};

extern CSandboxBrowserApp theApp;
