// CLogsWindow.cpp : implementation file
//

#include "pch.h"
#include "SampleSandboxBrowser.h"
#include "CLogsWindow.h"


// CLogsWindow

IMPLEMENT_DYNAMIC(CLogsWindow, CDialog)

CLogsWindow::CLogsWindow() : CDialog()
{
}

CLogsWindow::CLogsWindow(CWnd* parent) : CDialog(IDD_DIALOG_LOGS, parent)
{

}

CLogsWindow::~CLogsWindow()
{
}

void CLogsWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_LOGS, m_logsEdit);
}

BEGIN_MESSAGE_MAP(CLogsWindow, CDialog)
	ON_BN_CLICKED(IDOK, &CLogsWindow::OnBnClickedOk)
	ON_COMMAND(IDCLOSE, &CLogsWindow::OnClose)
	ON_WM_MOVE()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
int CLogsWindow::AppendToLog(CString str, COLORREF color)
{
	int nOldLines = 0, nNewLines = 0, nScroll = 0;
	long nInsertionPoint = 0;
	//CHARRANGE cr;
	CHARFORMAT cf;

	// Save number of lines before insertion of new text
	nOldLines = m_logsEdit.GetLineCount();

	// Initialize character format structure
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0;	// To disable CFE_AUTOCOLOR
	cf.crTextColor = color;

	// Set insertion point to end of text
	nInsertionPoint = m_logsEdit.GetWindowTextLength();
	/*if (nInsertionPoint > 800)
	{
		// Delete first half of text to avoid running into the 64k limit
		m_logsEdit.SetSel(0, nInsertionPoint / 2);
		m_logsEdit.ReplaceSel("");
		UpdateData(FALSE);
	}*/
	nInsertionPoint = -1;
	m_logsEdit.SetSel(nInsertionPoint, -1);

	//  Set the character format
	m_logsEdit.SetSelectionCharFormat(cf);

	// Replace selection. Because we have nothing selected, this will simply insert
	// the string at the current caret position.
	m_logsEdit.ReplaceSel(str);

	// Get new line count
	nNewLines = m_logsEdit.GetLineCount();

	// Scroll by the number of lines just inserted
	nScroll = nNewLines - nOldLines;
	m_logsEdit.LineScroll(nScroll);

	return 0;
}

int CLogsWindow::AppendToLogAndScroll(CString str, COLORREF color)
{
	long nVisible = 0;
	long nInsertionPoint = 0;
	CHARFORMAT cf;

	// Initialize character format structure
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0; // To disable CFE_AUTOCOLOR

	cf.crTextColor = color;

	// Set insertion point to end of text
	nInsertionPoint = m_logsEdit.GetWindowTextLength();
	m_logsEdit.SetSel(nInsertionPoint, -1);

	// Set the character format
	m_logsEdit.SetSelectionCharFormat(cf);

	// Replace selection. Because we have nothing 
	// selected, this will simply insert
	// the string at the current caret position.
	m_logsEdit.ReplaceSel(str);

	// Get number of currently visible lines or maximum number of visible lines
	// (We must call GetNumVisibleLines() before the first call to LineScroll()!)
	nVisible = GetNumVisibleLines(&m_logsEdit);

	// Now this is the fix of CRichEditCtrl's abnormal behaviour when used
	// in an application not based on dialogs. Checking the focus prevents
	// us from scrolling when the CRichEditCtrl does so automatically,
	// even though ES_AUTOxSCROLL style is NOT set.
	if (&m_logsEdit != m_logsEdit.GetFocus())
	{
		m_logsEdit.LineScroll(INT_MAX);
		m_logsEdit.LineScroll(1 - nVisible);
	}

	return 0;
}

int CLogsWindow::GetNumVisibleLines(CRichEditCtrl* pCtrl)
{
	CRect rect;
	long nFirstChar, nLastChar;
	long nFirstLine, nLastLine;

	// Get client rect of rich edit control
	pCtrl->GetClientRect(rect);

	// Get character index close to upper left corner
	nFirstChar = pCtrl->CharFromPos(CPoint(0, 0));

	// Get character index close to lower right corner
	nLastChar = pCtrl->CharFromPos(CPoint(rect.right, rect.bottom));
	if (nLastChar < 0)
	{
		nLastChar = pCtrl->GetTextLength();
	}

	// Convert to lines
	nFirstLine = pCtrl->LineFromChar(nFirstChar);
	nLastLine = pCtrl->LineFromChar(nLastChar);

	return (nLastLine - nFirstLine);
}


// CLogsWindow message handlers




void CLogsWindow::OnBnClickedOk()
{
	CDialog::ShowWindow(SW_HIDE);
}


void CLogsWindow::OnClose()
{
	// TODO: Add your command handler code here
	CDialog::ShowWindow(SW_HIDE);
}


void CLogsWindow::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	// TODO: Add your message handler code here
	//AfxGetApp()->WriteProfileInt(REG_SECTION_PREFERENCES, L"WindowX", x);
}
