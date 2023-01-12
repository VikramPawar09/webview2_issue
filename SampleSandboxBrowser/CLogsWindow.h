#pragma once

#include "resource.h"

// CLogsWindow
class CLogsWindow : public CDialog
{
	DECLARE_DYNAMIC(CLogsWindow)

public:
	enum {IDD = IDD_DIALOG_LOGS};
	CLogsWindow();
	CLogsWindow(CWnd * parent);
	virtual ~CLogsWindow();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	int AppendToLog(CString str, COLORREF color);
	int AppendToLogAndScroll(CString str, COLORREF color);
	int GetNumVisibleLines(CRichEditCtrl * pCtrl);
	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_logsEdit;
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
};


