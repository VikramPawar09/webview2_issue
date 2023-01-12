#if !defined(AFX_MYDLGBAR_H__3C6B41A8_97B4_441D_9148_B624A74DF364__INCLUDED_)
#define AFX_MYDLGBAR_H__3C6B41A8_97B4_441D_9148_B624A74DF364__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyDlgBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyDlgBar dialog

class CBrowserDlgBar : public CDialogBar
{
	// Construction
public:
	CBrowserDlgBar();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMyDlgBar)
	enum { IDD = IDD_MAINFRAME };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDlgBar)
public:
	virtual BOOL Create(CWnd* pParent, UINT nIDTemplate, UINT nStyle, UINT nID);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_url;
	// CButton	m_subButton;
	CEdit m_editUrl;
	// Generated message map functions
	//{{AFX_MSG(CMyDlgBar)
	afx_msg LRESULT OnInitDialog(WPARAM, LPARAM);
	//afx_msg void OnBnClickedButtonGo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	HRESULT get_Url(CString* url);
	HRESULT set_Url(CString url);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDLGBAR_H__3C6B41A8_97B4_441D_9148_B624A74DF364__INCLUDED_)
