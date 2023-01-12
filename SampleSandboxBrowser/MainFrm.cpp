// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "SampleSandboxBrowser.h"
#include <string.h>
#include "MainFrm.h"
#include "CommonHostObject.h"
#include "CLogsWindow.h"
#include <WebView2EnvironmentOptions.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define CHECK_FAILURE_STRINGIFY(arg)         #arg
#define CHECK_FAILURE_FILE_LINE(file, line)  ([](HRESULT hr){ CheckFailure(hr, L"Failure at " CHECK_FAILURE_STRINGIFY(file) L"(" CHECK_FAILURE_STRINGIFY(line) L")"); })
#define CHECK_FAILURE                        CHECK_FAILURE_FILE_LINE(__FILE__, __LINE__)
#define CHECK_FAILURE_BOOL(value)            CHECK_FAILURE((value) ? S_OK : E_UNEXPECTED)

static constexpr UINT MSG_NAVIGATE = WM_APP + 123;
static constexpr UINT MSG_RUN_ASYNC_CALLBACK = WM_APP + 124;

struct CMainFrameImpl
{
	wil::com_ptr<ICoreWebView2Environment> m_webViewEnvironment;
	wil::com_ptr<ICoreWebView2>            m_webView;
	wil::com_ptr<ICoreWebView2Controller>  m_webController;
	wil::com_ptr<ICoreWebView2Settings>    m_webSettings;
};

void ShowFailure(HRESULT hr, CString const& message)
{
	CString text;
	text.Format(L"%s (0x%08X)", (LPCTSTR)message, hr);
	AfxMessageBox(static_cast<LPCTSTR>(text), MB_ICONERROR | MB_OK);
}

void CheckFailure(HRESULT hr, CString const& message)
{
	if (FAILED(hr))
	{
		CString text;
		text.Format(L"%s : 0x%08X", (LPCTSTR)message, hr);
		AfxMessageBox(text, MB_ICONERROR | MB_OK);
		std::exit(hr);
	}
}


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_CLOSE, &CMainFrame::OnFileClose)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_GO, &CMainFrame::OnBnClickedButtonGo)
	ON_COMMAND(ID_VIEW_LOGSWINDOW, &CMainFrame::OnViewLogswindow)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept  :m_pImpl(new CMainFrameImpl())
{
	m_callbacks[CallbackType::CreationCompleted] = nullptr;
	m_callbacks[CallbackType::NavigationCompleted] = nullptr;
}

CMainFrame::~CMainFrame()
{
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, 0);
	CloseWebView();
	delete m_pImpl;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_myDlgBar.Create(this, IDD_MAINFRAME,
		CBRS_ALIGN_TOP | CBRS_GRIPPER,
		AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialog bar m_wndDlgBar\n");
		return -1;		// fail to create
	}
	else {
		m_myDlgBar.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
		EnableDocking(CBRS_ALIGN_ANY);
		//DockControlBar(&m_myDlgBar);
	}

	if (!m_wndReBar.Create(this) || !m_wndReBar.AddBar(&m_wndToolBar) || !m_wndReBar.AddBar(&m_myDlgBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	/*RegisterCallback(CMainFrame::CallbackType::CreationCompleted, [this]() {
		NavigateTo(L"https://www.microsoft.com");
	});*/

	m_logsWindow = new CLogsWindow(this);
	if (m_logsWindow == NULL)
	{
		delete m_logsWindow;
	}
	else {
		if (m_logsWindow->Create(IDD_DIALOG_LOGS, this))
		{
			int showLogs = NO;
			showLogs = AfxGetApp()->GetProfileIntW(REG_SECTION_PREFERENCES, REG_PREFERENCE_SHOWLOGS, NO);
			//showLogs== YES ? m_logsWindow->ShowWindow(SW_SHOW): m_logsWindow->ShowWindow(SW_HIDE);
		}
	}

	InitializeWebView();
	ResizeToClientArea();

	return 0;
}

void CMainFrame::DoDataExchange(CDataExchange* pDX)
{
	CFrameWnd::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_ADDRESS_EDIT, m_webUrl);
}

void CMainFrame::OnInitialUpdate()
{
	
}

BOOL CMainFrame::CreateHostWindow(
	LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd,
	UINT nID)
{
	if (lpszClassName == nullptr)
		lpszClassName = GetWindowClass();

	//if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID))
	//	return FALSE;

	::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	return TRUE;
}

BOOL CMainFrame::Create(
	LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd,
	UINT nID,
	CCreateContext*)
{
	if (!CreateHostWindow(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	InitializeWebView();

	return TRUE;
}

BOOL CMainFrame::CreateAsync(
	DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd,
	UINT nID,
	CallbackFunc onCreated)
{
	if (!CreateHostWindow(nullptr, nullptr, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	m_callbacks[CallbackType::CreationCompleted] = onCreated;

	InitializeWebView();

	return TRUE;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// base class does the real work

	if (!CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	CWinApp* pApp = AfxGetApp();
	if (pApp->m_pMainWnd == nullptr)
		pApp->m_pMainWnd = this;

	// replace Exit option on File menu with Close for secondary windows
	if (AfxGetApp()->m_pMainWnd != this)
	{
		CMenu *pMenu = GetMenu();
		ASSERT(pMenu);
		BOOL bNameValid;
		CString strTemp;
		bNameValid = strTemp.LoadString(IDS_CLOSE);
		ASSERT(bNameValid);
		pMenu->ModifyMenu(ID_APP_EXIT, MF_BYCOMMAND | MF_STRING, ID_FILE_CLOSE, strTemp);
	}

	return TRUE;
}

void CMainFrame::OnFileClose()
{
	DestroyWindow();
}

void CMainFrame::OnClose()
{
	CloseWebView();
	CSandboxBrowserApp *pApp = reinterpret_cast<CSandboxBrowserApp*>(AfxGetApp());
	// main window is being closed so cleanup
	if (pApp->m_pMainWnd == this)
	{
		for (int iCntr=0; iCntr < pApp->m_aryFrames.GetSize(); iCntr++)
		{
			HWND hFrame = pApp->m_aryFrames.GetAt(iCntr);
			if (::IsWindow(hFrame))
				::SendMessage(hFrame, WM_CLOSE, 0, 0);
		}
	}
	CFrameWnd::OnClose();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	ResizeToClientArea();
}

void CMainFrame::RegisterCallback(CallbackType const type, CallbackFunc callback)
{
	m_callbacks[type] = callback;
}

void CMainFrame::CloseWebView()
{
	if (m_pImpl->m_webView)
	{
		m_pImpl->m_webView->remove_NavigationCompleted(m_navigationCompletedToken);
		m_pImpl->m_webView->remove_NavigationStarting(m_navigationStartingToken);
		m_pImpl->m_webView->remove_DocumentTitleChanged(m_documentTitleChangedToken);
		m_pImpl->m_webView->remove_WebMessageReceived(m_webMessageReceivedToken);

		m_pImpl->m_webController->Close();

		m_pImpl->m_webController = nullptr;
		m_pImpl->m_webView = nullptr;
		m_pImpl->m_webSettings = nullptr;
	}

	m_pImpl->m_webViewEnvironment = nullptr;
}

void CMainFrame::InitializeWebView()
{
	CloseWebView();

	CString subFolder = GetInstallPath(); 
	CString appData = GetUserDataFolder();
	CStringW additionalBrowserSwitches = L" --disable-web-security --disable-features=msWebOOUI  --remote-debugging-port=9222 --enable-logging --v=1 --log-file=c:\\temp\\SampleSandboxBrowser.log";
	auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
	options->put_AdditionalBrowserArguments(additionalBrowserSwitches);

	// LPWSTR versionInfo;
	// GetAvailableCoreWebView2BrowserVersionString(CT2W(CString(".\\WebView2Runtime_108")), &versionInfo);

	HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
		// CT2W(CString(".\\WebView2Runtime_98")),
		CT2W(CString(".\\WebView2Runtime_108")),
		CT2W(CString(".\\WebView2Cache")), 
		options.Get(),
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			this,
			&CMainFrame::OnCreateEnvironmentCompleted).Get());

	if (!SUCCEEDED(hr))
	{
		CString text;
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			text = L"Cannot find the Edge browser.";
		}
		else
		{
			text = L"Cannot create the webview environment.";
		}

		ShowFailure(hr, text);
	}
}

HRESULT CMainFrame::OnCreateEnvironmentCompleted(
	HRESULT result,
	ICoreWebView2Environment* environment)
{
	CHECK_FAILURE(result);
	CHECK_FAILURE(environment->QueryInterface(IID_PPV_ARGS(&m_pImpl->m_webViewEnvironment)));
	CHECK_FAILURE(m_pImpl->m_webViewEnvironment->CreateCoreWebView2Controller(
		m_hWnd,
		Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
			this,
			&CMainFrame::OnCreateWebViewControllerCompleted).Get()));

	return S_OK;
}

HRESULT CMainFrame::OnCreateWebViewControllerCompleted(
	HRESULT result,
	ICoreWebView2Controller* controller)
{
	if (result == S_OK)
	{
		if (controller != nullptr)
		{
			m_pImpl->m_webController = controller;
			CHECK_FAILURE(controller->get_CoreWebView2(&m_pImpl->m_webView));
		}

		CHECK_FAILURE(m_pImpl->m_webView->get_Settings(&m_pImpl->m_webSettings));

		// Add a few settings for the webview
		m_pImpl->m_webSettings->put_IsScriptEnabled(TRUE);
		m_pImpl->m_webSettings->put_AreDefaultScriptDialogsEnabled(TRUE);
		m_pImpl->m_webSettings->put_IsWebMessageEnabled(TRUE);
		m_pImpl->m_webSettings->put_IsZoomControlEnabled(FALSE);
#ifndef DEBUG
		m_pImpl->m_webSettings->put_AreDefaultContextMenusEnabled(FALSE);
#endif // DEBUG
		m_pImpl->m_webSettings->put_AreHostObjectsAllowed(TRUE);
		m_pImpl->m_webSettings->put_AreDevToolsEnabled(TRUE);

		ResizeToClientArea();
		RegisterEventHandlers();

		auto callback = m_callbacks[CallbackType::CreationCompleted];
		if (callback != nullptr)
		{
			RunAsync(callback);
		}
		else
		{	
			std::wstring path = GetLocalUri(L"samplepage.html");
			NavigateTo(path.c_str());
		}
	}
	else
	{
		ShowFailure(result, L"Cannot create webview environment.");
	}

	return S_OK;
}

void CMainFrame::RegisterEventHandlers()
{

	//! [NavigationStarting]
	CHECK_FAILURE(m_pImpl->m_webView->add_NavigationStarting(
		Callback<ICoreWebView2NavigationStartingEventHandler>(
			[this](
				ICoreWebView2*,
				ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
	{
		wil::unique_cotaskmem_string uri;
		CHECK_FAILURE(args->get_Uri(&uri));

		m_isNavigating = true;
		
		this->m_myDlgBar.set_Url(uri.get());

		AttachHostObjects();

		auto callback = m_callbacks[CallbackType::NavigationStarting];
		if (callback != nullptr)
			RunAsync(callback);

		return S_OK;
	}).Get(), &m_navigationStartingToken));

	//! [NavigationCompleted]
	CHECK_FAILURE(m_pImpl->m_webView->add_NavigationCompleted(
		Callback<ICoreWebView2NavigationCompletedEventHandler>(
			[this](
				ICoreWebView2*,
				ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT
	{
		m_isNavigating = false;

		BOOL success;
		CHECK_FAILURE(args->get_IsSuccess(&success));

		if (!success)
		{
			COREWEBVIEW2_WEB_ERROR_STATUS webErrorStatus{};
			CHECK_FAILURE(args->get_WebErrorStatus(&webErrorStatus));
			if (webErrorStatus == COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED)
			{
				ASSERT(webErrorStatus == 11);
				// Do something here if you want to handle a specific error case.
				// In most cases this isn't necessary, because the WebView will
				// display its own error page automatically.
			}

			if (webErrorStatus == COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_IS_INVALID)
			{
				//ADD check for certificate validation.
			}
		}

		wil::unique_cotaskmem_string uri;
		m_pImpl->m_webView->get_Source(&uri);

		if (wcscmp(uri.get(), L"about:blank") == 0)
		{
			uri = wil::make_cotaskmem_string(L"");
		}

		this->m_myDlgBar.set_Url(uri.get());

		//CHECK_FAILURE(m_pImpl->m_webView->AddScriptToExecuteOnDocumentCreated(L"Object.freeze(Object);", nullptr));

		// Schedule an async task to get the document URL
		CHECK_FAILURE(m_pImpl->m_webView->ExecuteScript(L"window.document.URL", Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
			[this](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
			LPCWSTR URL = resultObjectAsJson;
			UNREFERENCED_PARAMETER(URL);
			UNREFERENCED_PARAMETER(errorCode);
			//doSomethingWithURL(URL);
			//::MessageBox(NULL,URL);
			return S_OK;
		}).Get()));

		auto callback = m_callbacks[CallbackType::NavigationCompleted];
		if (callback != nullptr)
			RunAsync(callback);

		return S_OK;
	}).Get(), &m_navigationCompletedToken));


	//! [AcceleratorKeyPressed]
	// Register a handler for the AcceleratorKeyPressed event.
	CHECK_FAILURE(m_pImpl->m_webController->add_AcceleratorKeyPressed(
		Callback<ICoreWebView2AcceleratorKeyPressedEventHandler>(
			[this](
				ICoreWebView2Controller* sender,
				ICoreWebView2AcceleratorKeyPressedEventArgs* args) -> HRESULT {
		UNREFERENCED_PARAMETER(sender);
		UNREFERENCED_PARAMETER(args);
		COREWEBVIEW2_KEY_EVENT_KIND kind;
		CHECK_FAILURE(args->get_KeyEventKind(&kind));
		// We only care about key down events.
		if (kind == COREWEBVIEW2_KEY_EVENT_KIND_KEY_DOWN ||
			kind == COREWEBVIEW2_KEY_EVENT_KIND_SYSTEM_KEY_DOWN)
		{
			UINT key;
			CHECK_FAILURE(args->get_VirtualKey(&key));
			// Check if the key is one we want to handle.
			if (std::function<void()> action =
				this->GetAcceleratorKeyFunction(key))
			{
				// Keep the browser from handling this key, whether it's autorepeated or
				// not.
				CHECK_FAILURE(args->put_Handled(TRUE));

				// Filter out autorepeated keys.
				COREWEBVIEW2_PHYSICAL_KEY_STATUS status;
				CHECK_FAILURE(args->get_PhysicalKeyStatus(&status));
				if (!status.WasKeyDown)
				{
					// Perform the action asynchronously to avoid blocking the
					// browser process's event queue.
					this->RunAsync(action);
				}
			}
		}
		return S_OK;
	}).Get(), &m_acceleratorKeyPressedToken));

	//! [DocumentTitleChanged] 
	CHECK_FAILURE(m_pImpl->m_webView->add_DocumentTitleChanged(
		Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
			[this](ICoreWebView2* sender, IUnknown* args) -> HRESULT {
		UNREFERENCED_PARAMETER(sender);
		UNREFERENCED_PARAMETER(args);

		wil::unique_cotaskmem_string title;
		CHECK_FAILURE(sender->get_DocumentTitle(&title));

		m_strTitle = title.get();

		auto callback = m_callbacks[CallbackType::TitleChanged];
		if (callback != nullptr)
			RunAsync(callback);

		return S_OK;
	}).Get(), &m_documentTitleChangedToken));

	// Set an event handler for the host to return received message back to the web content
	//! [WebMessageReceived]
	CHECK_FAILURE(m_pImpl->m_webView->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
		[this](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
		PWSTR message;
		args->TryGetWebMessageAsString(&message);
		// processMessage(&message);

		auto callback = m_callbacks[CallbackType::WebMessageReceived];
		if (callback != nullptr)
			RunAsync(callback);

		webview->PostWebMessageAsString(message);
		CoTaskMemFree(message);
		return S_OK;
	}).Get(), &m_webMessageReceivedToken));

}

void CMainFrame::AttachHostObjects()
{
	m_hostObject = Microsoft::WRL::Make<CommonHostObject>(
		[appWindow = this](std::function<void(void)> callback)
	{
	});

	m_hostObject->SetLogsWindow(m_logsWindow);

	VARIANT remoteObjectAsVariant = {};
	m_hostObject.query_to<IDispatch>(&remoteObjectAsVariant.pdispVal);
	remoteObjectAsVariant.vt = VT_DISPATCH;

	CHECK_FAILURE(
		m_pImpl->m_webView->AddHostObjectToScript(L"common", &remoteObjectAsVariant));
	remoteObjectAsVariant.pdispVal->Release();

	ExecuteScript(L"chrome.webview.hostObjects.options.log = console.log.bind(console);");
}

void CMainFrame::ExecuteScript(LPCWSTR javascript, std::function<void(HRESULT, LPCWSTR)> callback) {
	CHECK_FAILURE(m_pImpl->m_webView->ExecuteScript(javascript, Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
		[&](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
		callback(errorCode, resultObjectAsJson);
		return S_OK;
	}).Get()));
}

void CMainFrame::ExecuteScript(LPCWSTR javascript) {
	CHECK_FAILURE(m_pImpl->m_webView->ExecuteScript(javascript, Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
		[&](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
		UNREFERENCED_PARAMETER(errorCode);
		UNREFERENCED_PARAMETER(resultObjectAsJson);
		return S_OK;
	}).Get()));
}

HRESULT CMainFrame::AddHostObject(LPCWSTR name, VARIANT* remoteObjectAsVariant) {

	// We can call AddHostObjectToScript multiple times in a row without
	// calling RemoveHostObject first. This will replace the previous object
	// with the new object. In our case this is the same object and everything
	// is fine.
	CHECK_FAILURE(
		m_pImpl->m_webView->AddHostObjectToScript(name, remoteObjectAsVariant));
	//! [AddHostObjectToScript]
	return S_OK;
}

void CMainFrame::ResizeToClientArea()
{
	if (m_pImpl->m_webController)
	{
		RECT rect;
		this->GetClientRect(&rect);
		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(ncm);
		RECT toolbarRect = { 0,0,0,0 }, statusBarRect = { 0,0,0,0 };
		m_wndToolBar.GetWindowRect(&toolbarRect);
		/*if (m_wndDlgBar.IsVisible())
		{
			m_wndDlgBar.GetWindowRect(&toolbarRect);
		}*/
		
		if (m_myDlgBar.IsVisible())
		{
			m_myDlgBar.GetWindowRect(&toolbarRect);
		}

		if (m_wndStatusBar.IsVisible())
		{
			m_wndStatusBar.GetWindowRect(&statusBarRect);
		}

		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
		rect.top += ncm.iMenuHeight + (toolbarRect.bottom - toolbarRect.top)+5;
		rect.bottom -=(statusBarRect.bottom-statusBarRect.top);

		m_pImpl->m_webController->put_Bounds(rect);
	}
}

RECT CMainFrame::GetBounds()
{
	RECT rc{ 0,0,0,0 };
	if (m_pImpl->m_webController)
	{
		m_pImpl->m_webController->get_Bounds(&rc);
	}

	return rc;
}

std::function<void()> CMainFrame::GetAcceleratorKeyFunction(UINT key)
{
	if (key == VK_F4)
	{
		TRACE(L"F4 key pressed");
		if (this != nullptr)
		{
			return [this] {
				this->PostMessage(WM_KEYDOWN, VK_F4);
			};
		}
	}

	if (GetKeyState(VK_CONTROL) < 0)
	{
		switch (key)
		{
		case 'N':
			return [this] {};
		case 'Q':
			return [this] {};
		case 'S':
			return [this] {};
		case 'T':
			return [this] { /*CreateNewThread(m_creationModeId);*/ };
		case 'W':
			return [this] { CloseWebView(); };
		}
	}
	return nullptr;
}

void CMainFrame::Resize(LONG const width, LONG const height)
{
	SetWindowPos(nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOREPOSITION);
	ResizeToClientArea();
}

CString CMainFrame::GetLocationURL()
{
	CString url;
	if (m_pImpl->m_webView)
	{
		wil::unique_cotaskmem_string uri;
		m_pImpl->m_webView->get_Source(&uri);

		if (wcscmp(uri.get(), L"about:blank") == 0)
		{
			uri = wil::make_cotaskmem_string(L"");
		}

		url = uri.get();
	}

	return url;
}

void CMainFrame::NavigateTo(CString url)
{
	if (url.Find(L"://") < 0)
	{
		if (url.GetLength() > 1 && url[1] == ':')
			url = L"file://" + url;
		else
			url = L"http://" + url;
	}

	m_pImpl->m_webView->Navigate(CT2W(url));
}

void CMainFrame::Navigate(CString const& url, CallbackFunc onComplete)
{
	if (m_pImpl->m_webView)
	{
		m_callbacks[CallbackType::NavigationCompleted] = onComplete;
		NavigateTo(url);
	}
}

void CMainFrame::PrintDocument()
{
	if (m_pImpl->m_webView)
	{
		m_pImpl->m_webView->ExecuteScript(L"window.print();", nullptr);
	}
}

void CMainFrame::Stop()
{
	if (m_pImpl->m_webView)
	{
		m_pImpl->m_webView->Stop();
	}
}

void CMainFrame::Reload()
{
	if (m_pImpl->m_webView)
	{
		m_pImpl->m_webView->Reload();
	}
}

void CMainFrame::GoBack()
{
	if (m_pImpl->m_webView)
	{
		BOOL possible = FALSE;
		m_pImpl->m_webView->get_CanGoBack(&possible);
		if (possible)
			m_pImpl->m_webView->GoBack();
	}
}

void CMainFrame::GoForward()
{
	if (m_pImpl->m_webView)
	{
		BOOL possible = FALSE;
		m_pImpl->m_webView->get_CanGoForward(&possible);
		if (possible)
			m_pImpl->m_webView->GoForward();
	}
}

void CMainFrame::DisablePopups()
{
	if (m_pImpl->m_webSettings)
	{
		m_pImpl->m_webSettings->put_AreDefaultScriptDialogsEnabled(FALSE);
	}
}

PCTSTR CMainFrame::GetWindowClass()
{
	static PCTSTR windowClass = []
	{
		static TCHAR const* className = _T("EdgeBrowserHost");

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProcStatic;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = AfxGetInstanceHandle();
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = className;
		wcex.hIconSm = nullptr;

		ATOM result = RegisterClassEx(&wcex);
		if (result == 0)
		{
			/*[[maybe_unused]] DWORD lastError = ::GetLastError();*/
		}

		return className;
	}();

	return windowClass;
}

LRESULT CALLBACK CMainFrame::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (auto app = (CMainFrame*)::GetWindowLongPtr(hWnd, GWLP_USERDATA))
	{
		LRESULT result = 0;
		if (app->HandleWindowMessage(hWnd, message, wParam, lParam, &result))
		{
			return result;
		}
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool CMainFrame::HandleWindowMessage(
	HWND, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	*result = 0;

	switch (message)
	{
	case WM_CLOSE:
	case SC_CLOSE:
		CloseWebView();
		break;

	case SC_SIZE:
	case WM_SIZE:
	{
		if (lParam != 0)
		{
			//ResizeToClientArea();
			return true;
		}
	}
	break;
	case MSG_RUN_ASYNC_CALLBACK:
	{
		auto* task = reinterpret_cast<CallbackFunc*>(wParam);
		(*task)();
		delete task;
		return true;
	}
	break;
	}

	return false;
}

void CMainFrame::RunAsync(CallbackFunc callback)
{
	auto* task = new CallbackFunc(callback);
	PostMessage(MSG_RUN_ASYNC_CALLBACK, reinterpret_cast<WPARAM>(task), 0);
}

bool CMainFrame::IsWebViewCreated() const
{
	return m_pImpl->m_webView != nullptr;
}

CString CMainFrame::GetInstallPath()
{
	static CString path = []
	{
		// To work with specific version of Edge Beta just return a string like this
		// return CString{ R"(C:\Program Files (x86)\Microsoft\Edge Beta\Application\83.0.478.13\)" };

		 // return CString{ "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\108.0.1462.54\\" };
		
		 return CString{ "D:\\Codebase\\cpo\\CPO\\Bin\\Debug\\WebView2Runtime\\" };

		
		auto installPath = GetInstallPathFromRegistry();
		if (installPath.IsEmpty())
			installPath = GetInstallPathFromDisk();

		return installPath;
	}();

	return path;
}

CString CMainFrame::GetInstallPathFromRegistry()
{
	CString path;

	HKEY handle = nullptr;
	auto result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Microsoft Edge)",
		0,
		KEY_READ,
		&handle);

	if (result != ERROR_SUCCESS)
		result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			LR"(SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Microsoft Edge)",
			0,
			KEY_READ,
			&handle);

	if (result == ERROR_SUCCESS)
	{
		TCHAR buffer[MAX_PATH + 1]{ 0 };
		DWORD type = REG_SZ;
		DWORD size = MAX_PATH;
		result = RegQueryValueEx(handle, L"InstallLocation", 0, &type, reinterpret_cast<LPBYTE>(buffer), &size);
		if (result == ERROR_SUCCESS)
			path += CString{ buffer };

		TCHAR version[100]{ 0 };
		size = 100;
		result = RegQueryValueEx(handle, L"Version", 0, &type, reinterpret_cast<LPBYTE>(version), &size);
		if (result == ERROR_SUCCESS)
		{
			if (path.GetAt(path.GetLength() - 1) != L'\\')
				path += L"\\";
			path += CString{ version };
		}
		else
			path.Empty();

		RegCloseKey(handle);
	}

	return path;
}

CString CMainFrame::GetInstallPathFromDisk()
{
	CString path = LR"(c:\Program Files (x86)\Microsoft\Edge\Application\)";
	CString pattern = path + L"*";

	WIN32_FIND_DATA ffd{ 0 };
	HANDLE hFind = FindFirstFile(pattern, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		/*[[maybe_unused]] DWORD error = ::GetLastError();*/
		return {};
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CString name{ ffd.cFileName };
			int a, b, c, d;
			if (4 == swscanf_s(CT2W(ffd.cFileName), L"%d.%d.%d.%d", &a, &b, &c, &d))
			{
				FindClose(hFind);
				return path + name;
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

	return {};
}

CString CMainFrame::GetUserDataFolder()
{
	TCHAR szPath[MAX_PATH]{ 0 };
	::SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, szPath);
	::PathAppend(szPath,LR"(\SBB\)");
	::PathAppend(szPath, L"Cache");
	return CString{ szPath };
}

std::wstring CMainFrame::GetLocalPath(std::wstring relativePath)
{
	WCHAR rawPath[MAX_PATH];
	GetModuleFileNameW(NULL, rawPath, MAX_PATH);
	std::wstring path(rawPath);

	std::size_t index = path.find_last_of(L"\\") + 1;
	path.replace(index, path.length(), relativePath);
	return path;
}

std::wstring CMainFrame::GetLocalUri(std::wstring relativePath)
{
	std::wstring path = GetLocalPath(relativePath);

	wil::com_ptr<IUri> uri;
	CHECK_FAILURE(CreateUri(path.c_str(), Uri_CREATE_ALLOW_IMPLICIT_FILE_SCHEME, 0, &uri));

	wil::unique_bstr uriBstr;
	CHECK_FAILURE(uri->GetAbsoluteUri(&uriBstr));
	return std::wstring(uriBstr.get());
}

void CMainFrame::OnBnClickedButtonGo()
{
	// UpdateData(FALSE);
	CString strUrl;
	m_myDlgBar.get_Url(&strUrl);
	NavigateTo(strUrl);
}


void CMainFrame::OnViewLogswindow()
{
	// TODO: Add your command handler code here
	if (m_logsWindow != NULL)
	{
		if (!m_logsWindow->IsWindowVisible())
		{
			m_logsWindow->ShowWindow(SW_SHOW);
			AfxGetApp()->WriteProfileInt(REG_SECTION_PREFERENCES, REG_PREFERENCE_SHOWLOGS, 1);
		}
		else {
			m_logsWindow->ShowWindow(SW_HIDE);
			AfxGetApp()->WriteProfileInt(REG_SECTION_PREFERENCES, REG_PREFERENCE_SHOWLOGS, 0);
		}
	}
}
