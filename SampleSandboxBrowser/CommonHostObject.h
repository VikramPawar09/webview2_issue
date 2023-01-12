
#pragma once

//#include "Stdafx.h"
#include "atlstr.h"	
#include <functional>
#include <map>
#include <string>
#include <wrl.h>
#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result.h>
#include <wrl\client.h>
#include "CommonHostObject_h.h"
#include "CLogsWindow.h"

class CommonHostObject : public Microsoft::WRL::RuntimeClass<
	Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
	ICommonHostObject, IDispatch>
{
public:
	enum class ActionType
	{
		Close,
		Refresh,
		OpenVisit
	};

	typedef std::function<void(BSTR actionName, BSTR payload)> ActionCallback;
	typedef std::function<void(void)> Callback;
	typedef std::function<void(Callback)> RunCallbackAsync;

	CommonHostObject(RunCallbackAsync runCallbackAsync);

	STDMETHODIMP LogInfo(BSTR text)override;
	STDMETHODIMP LogWarning(BSTR text)override;
	STDMETHODIMP LogDebug(BSTR text)override;
	STDMETHODIMP LogError(BSTR text)override;
	STDMETHODIMP DialogMethod(BSTR text)override;

	STDMETHODIMP ExecuteAction(BSTR actionName, BSTR payload) override;

	// Demonstrate native calling back into JavaScript.
	STDMETHODIMP CallCallbackAsynchronously(IDispatch* callbackParameter) override;

	// IDispatch implementation
	STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;

	void RegisterActionCallback(CommonHostObject::ActionType actionType, CommonHostObject::ActionCallback callback);

	//// Demonstrate getting and setting a property.
	STDMETHODIMP get_UserName(BSTR* userName);
	STDMETHODIMP get_UserPassword(BSTR* userPassword);
	STDMETHODIMP get_UserCredentials(BSTR* userCredentials);

	void SetLogsWindow(CLogsWindow* logsWindow) { m_logsWindow = logsWindow; }

private:
	wil::com_ptr<IDispatch> m_callback;
	RunCallbackAsync m_runCallbackAsync;
	wil::com_ptr<ITypeLib> m_typeLib;
	std::map<ActionType, ActionCallback> m_actioncallbacks;
	CLogsWindow* m_logsWindow;
};

