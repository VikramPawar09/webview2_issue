// CLoginDialog.cpp : implementation file
//

#include "pch.h"
#include "SampleSandboxBrowser.h"
#include "CLoginDialog.h"
#include "afxdialogex.h"


// CLoginDialog dialog

IMPLEMENT_DYNAMIC(CLoginDialog, CDialogEx)

CLoginDialog::CLoginDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_DIALOG, pParent)
	, m_userName(_T(""))
	, m_userPassword(_T(""))
{

}

CLoginDialog::~CLoginDialog()
{
}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_userName);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_userPassword);
	DDX_Control(pDX, IDC_STATUSMESSAGE, m_StatusLabel);
}


BEGIN_MESSAGE_MAP(CLoginDialog, CDialogEx)
	ON_BN_CLICKED(IDLOGIN, &CLoginDialog::OnClickedLogin)
END_MESSAGE_MAP()


// CLoginDialog message handlers


void CLoginDialog::OnClickedLogin()
{
	UpdateData();
	if (!m_userName.IsEmpty() && !m_userPassword.IsEmpty())
	{
		CSandboxBrowserApp *pApp = reinterpret_cast<CSandboxBrowserApp*>(AfxGetApp());
		pApp->m_UserName = m_userName;
		pApp->m_Password = m_userPassword;
		CDialogEx::OnOK();
	}
	else {
		m_StatusLabel.SetWindowTextW(L"Username and Password are mandatory.");
	}
}


BOOL CLoginDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_StatusLabel.SetWindowTextW(L"Please enter Username and Password to login.");
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
