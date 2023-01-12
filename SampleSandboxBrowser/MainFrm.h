
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#define YES 1
#define NO 0
#define REG_SECTION_PREFERENCES L"Preferences"
#define REG_PREFERENCE_SHOWLOGS L"ShowLogs"

#include <EventToken.h>
#include <functional>
#include <map>
#include "CBrowserDlgBar.h"
#include <string>
using namespace Microsoft::WRL;

struct ICoreWebView2Environment;
struct ICoreWebView2Controller;

struct CMainFrameImpl;
class CommonHostObject;
class CLogsWindow;

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame() noexcept;

protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:
	enum class CallbackType
	{
		CreationCompleted,
		NavigationStarting,
		NavigationCompleted,
		TitleChanged,
		WebMessageReceived
	};

	using CallbackFunc = std::function<void()>;
// Operations
public:
	void RegisterCallback(CallbackType const type, CallbackFunc callback);
	HRESULT AddHostObject(LPCWSTR name, VARIANT *object);
	RECT GetBounds();
	void SetBounds(LONG const width, LONG const height) { Resize(width, height); }
	void Resize(LONG const width, LONG const height);

	CString GetLocationURL();

	void Navigate(CString const & url, CallbackFunc onComplete);
	void GoBack();
	void GoForward();
	void Reload();
	void Stop();
	bool IsNavigating() const { return m_isNavigating; }
	void DisablePopups();
	void ExecuteScript(LPCWSTR javascript);
	void ExecuteScript(LPCWSTR javascript, std::function<void(HRESULT, LPCWSTR)>);
	void AttachHostObjects();

	void PrintDocument();
	CString GetTitle() const { return m_strTitle; }

	bool IsWebViewCreated() const;

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

// Implementation
public:
	virtual ~CMainFrame();

	virtual BOOL Create(
		LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName,
		DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd,
		UINT nID,
		CCreateContext* = NULL) override;

	BOOL CreateAsync(
		DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd,
		UINT nID,
		CallbackFunc onCreated);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CReBar            m_wndReBar;
	CStatusBar        m_wndStatusBar;
	CBrowserDlgBar m_myDlgBar;
	CLogsWindow* m_logsWindow;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileClose();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnInitialUpdate();
	DECLARE_MESSAGE_MAP()

private:

	CMainFrameImpl* m_pImpl;
	std::map<CallbackType, CallbackFunc> m_callbacks;
	EventRegistrationToken m_navigationCompletedToken = {};
	EventRegistrationToken m_navigationStartingToken = {};
	EventRegistrationToken m_documentTitleChangedToken = {};
	EventRegistrationToken m_acceleratorKeyPressedToken = {};
	EventRegistrationToken m_webMessageReceivedToken = {};
	bool m_isNavigating = false;

private:
	void RunAsync(CallbackFunc callback);

	void InitializeWebView();
	void CloseWebView();
	void RegisterEventHandlers();
	void ResizeToClientArea();
	void NavigateTo(CString url);

	static CString GetInstallPath();
	static CString GetInstallPathFromRegistry();
	static CString GetInstallPathFromDisk();
	static CString GetUserDataFolder();

	std::wstring GetLocalPath(std::wstring relativePath);
	std::wstring GetLocalUri(std::wstring relativePath);

	std::function<void()> GetAcceleratorKeyFunction(UINT key);

	wil::com_ptr<CommonHostObject> m_hostObject;
	
	HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment);
	HRESULT OnCreateWebViewControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);

	static PCTSTR GetWindowClass();
	static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool HandleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);

	BOOL CreateHostWindow(
		LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName,
		DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd,
		UINT nID);

	CString m_webUrl=L"";
public:
	afx_msg void OnBnClickedButtonGo();
	afx_msg void OnViewLogswindow();
};


