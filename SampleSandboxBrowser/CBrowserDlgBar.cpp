// MyDlgBar.cpp : implementation file
//

#include "pch.h"
#include "SampleSandboxBrowser.h"
#include "CBrowserDlgBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyDlgBar dialog


CBrowserDlgBar::CBrowserDlgBar()
{
	//{{AFX_DATA_INIT(CMyDlgBar)
	//}}AFX_DATA_INIT
}


void CBrowserDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADDRESS_EDIT, m_editUrl);
	DDX_Text(pDX, IDC_ADDRESS_EDIT, m_url);
	//{{AFX_DATA_MAP(CMyDlgBar)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrowserDlgBar, CDialogBar)
	//{{AFX_MSG_MAP(CMyDlgBar)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	//ON_BN_CLICKED(IDC_BUTTON_GO, &CBrowserDlgBar::OnBnClickedButtonGo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlgBar message handlers

LRESULT CBrowserDlgBar::OnInitDialog(WPARAM wParam, LPARAM lParam)
{

	auto bRet = HandleInitDialog(wParam, lParam);

	if (!UpdateData(FALSE))
	{
		TRACE0("Warning: UpdateData failed during dialog init.\n");
	}

	return bRet;

}


BOOL CBrowserDlgBar::Create(CWnd* pParent, UINT nIDTemplate, UINT nStyle, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL bReturn = CDialogBar::Create(pParent, nIDTemplate, nStyle, nID);

	/*if (bReturn)
	{
		m_bmButton.AutoLoad(IDC_BUTTON_BITMAP, this);
		m_subButton.SubclassDlgItem(IDC_DBSUB_BUTTON, this);
	}*/

	return bReturn;
}

//void CBrowserDlgBar::OnBnClickedButtonGo()
//{
//	// TODO: Add your control notification handler code here
//	UpdateData(FALSE);
//	AfxMessageBox(L"Going", MB_OK);
//}

HRESULT CBrowserDlgBar::get_Url(CString* url)
{
	UpdateData();
	url->Empty();
	url->Append(m_url);
	return S_OK;
}

HRESULT CBrowserDlgBar::set_Url(CString url)
{
	m_editUrl.SetWindowTextW(url);
	return S_OK;
}
