#pragma once


// CLoginDialog dialog

class CLoginDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDialog)

public:
	CLoginDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLoginDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_userName;
	CString m_userPassword;
	afx_msg void OnClickedLogin();
	CStatic m_StatusLabel;
	virtual BOOL OnInitDialog();
//	CEdit m_pUserNameEdit;
};
