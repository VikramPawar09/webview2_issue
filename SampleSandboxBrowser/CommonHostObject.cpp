#include "pch.h"
#include "CommonHostObject.h"
#include "SampleSandboxBrowser.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "crypt32.lib") 
#pragma comment(lib, "Cabinet.lib")

#include <string>
#include <wincrypt.h>
#include "CLoginDialog.h"

#define LOG_INFO RGB(0, 0x99, 0)
#define LOG_ERROR RGB(255, 0, 0)
#define LOG_WARNING RGB(255,69,0)
#define LOG_DEBUG RGB(0,0,80)


CommonHostObject::CommonHostObject(CommonHostObject::RunCallbackAsync runCallbackAsync)
	: m_runCallbackAsync(runCallbackAsync)
{

}

void CommonHostObject::RegisterActionCallback(CommonHostObject::ActionType actionType, CommonHostObject::ActionCallback callback)
{
	m_actioncallbacks[actionType] = callback;
}

static constexpr UINT MSG_NAVIGATE = WM_APP + 123;
static constexpr UINT MSG_RUN_ASYNC_CALLBACK = WM_APP + 124;

STDMETHODIMP_(HRESULT __stdcall) CommonHostObject::LogInfo(BSTR text)
{
	auto timeStamp = COleDateTime::GetCurrentTime().Format(L"%m%d%Y-%H:%M:%S");
	this->m_logsWindow->AppendToLogAndScroll(timeStamp + L" :: " + text + L"\r\n", LOG_INFO);
	return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) CommonHostObject::DialogMethod(BSTR text)
{
	CLoginDialog* pLoginDlg = new CLoginDialog();
	if (pLoginDlg->DoModal() != IDOK)
	{
		return FALSE;
	}
	return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) CommonHostObject::LogWarning(BSTR text)
{
	auto timeStamp = COleDateTime::GetCurrentTime().Format(L"%m%d%Y-%H:%M:%S");
	this->m_logsWindow->AppendToLogAndScroll(timeStamp + L" :: " + text + L"\r\n", LOG_WARNING);
	return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) CommonHostObject::LogDebug(BSTR text)
{
	auto timeStamp = COleDateTime::GetCurrentTime().Format(L"%m%d%Y-%H:%M:%S");
	this->m_logsWindow->AppendToLogAndScroll(timeStamp + L" :: " + text + L"\r\n", LOG_DEBUG);
	return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) CommonHostObject::LogError(BSTR text)
{
	auto timeStamp = COleDateTime::GetCurrentTime().Format(L"%m%d%Y-%H:%M:%S");
	this->m_logsWindow->AppendToLogAndScroll(timeStamp + L" :: " + text + L"\r\n", LOG_ERROR);
	return S_OK;
}

STDMETHODIMP CommonHostObject::get_UserName(BSTR* userName)
{
	CString loginName=L"";
	CSandboxBrowserApp *pApp = reinterpret_cast<CSandboxBrowserApp*>(AfxGetApp());
	loginName = pApp->m_UserName;
	*userName = loginName.AllocSysString();
	return S_OK;
}

STDMETHODIMP CommonHostObject::get_UserPassword(BSTR* userPassword)
{
	CString loginPassword=L"";
	CSandboxBrowserApp *pApp = reinterpret_cast<CSandboxBrowserApp*>(AfxGetApp());
	loginPassword = pApp->m_Password;

	*userPassword = loginPassword.AllocSysString();
	return S_OK;
}

STDMETHODIMP CommonHostObject::get_UserCredentials(BSTR* userCredentials)
{
	CString loginUserName=L"", loginPassword=L"";
	
	CSandboxBrowserApp *pApp = reinterpret_cast<CSandboxBrowserApp*>(AfxGetApp());
	loginUserName = pApp->m_UserName;
	loginPassword = pApp->m_Password;

	CString credentials = loginUserName + ":" + loginPassword;

	USES_CONVERSION;
	auto creds = T2A(credentials);

	const DWORD newsizea = (credentials.GetLength() + 1);
	char *nstringa = new char[newsizea];
	strcpy_s(nstringa, newsizea, creds);

	DWORD dwSize = 0;
	if (!CryptBinaryToString((const BYTE*)nstringa, newsizea, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &dwSize)) {
		return FALSE;
	}

	dwSize++;
	LPWSTR outputBuffer = new TCHAR[dwSize];
	if (!CryptBinaryToString((const BYTE*)nstringa, newsizea, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, outputBuffer, &dwSize)) {
		return FALSE;
	}

	*userCredentials = CString(outputBuffer).AllocSysString();
	return S_OK;
}

STDMETHODIMP CommonHostObject::ExecuteAction(BSTR actionName, BSTR payload) {
	auto timeStamp = COleDateTime::GetCurrentTime().Format(L"%m%d%Y-%H:%M:%S");
	this->m_logsWindow->AppendToLogAndScroll(timeStamp + L" :: " + actionName + L"\t" + payload + L"\r\n", LOG_ERROR);
	return S_OK;
}

STDMETHODIMP CommonHostObject::CallCallbackAsynchronously(IDispatch* callbackParameter)
{
	wil::com_ptr<IDispatch> callbackParameterForCapture = callbackParameter;
	m_runCallbackAsync([callbackParameterForCapture]() -> void {
		callbackParameterForCapture->Invoke(
			DISPID_UNKNOWN, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, nullptr, nullptr,
			nullptr, nullptr);
	});

	return S_OK;
}

STDMETHODIMP CommonHostObject::GetTypeInfoCount(UINT* pctinfo)
{
	*pctinfo = 1;
	return S_OK;
}

STDMETHODIMP CommonHostObject::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
	UNREFERENCED_PARAMETER(lcid);
	if (0 != iTInfo)
	{
		return TYPE_E_ELEMENTNOTFOUND;
	}
	if (!m_typeLib)
	{
		RETURN_IF_FAILED(LoadTypeLib(L"SampleSandboxBrowser.tlb", &m_typeLib));
	}
	return m_typeLib->GetTypeInfoOfGuid(__uuidof(ICommonHostObject), ppTInfo);
}

STDMETHODIMP CommonHostObject::GetIDsOfNames(
	REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	UNREFERENCED_PARAMETER(riid);
	wil::com_ptr<ITypeInfo> typeInfo;
	RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
	return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP CommonHostObject::Invoke(
	DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
	VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	UNREFERENCED_PARAMETER(riid);
	wil::com_ptr<ITypeInfo> typeInfo;
	RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
	return typeInfo->Invoke(
		this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}
