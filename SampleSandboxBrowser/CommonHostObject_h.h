

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 08:44:07 2038
 */
/* Compiler settings for CommonHostObject.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __CommonHostObject_h_h__
#define __CommonHostObject_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICommonHostObject_FWD_DEFINED__
#define __ICommonHostObject_FWD_DEFINED__
typedef interface ICommonHostObject ICommonHostObject;

#endif 	/* __ICommonHostObject_FWD_DEFINED__ */


#ifndef __CommonHostObject_FWD_DEFINED__
#define __CommonHostObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class CommonHostObject CommonHostObject;
#else
typedef struct CommonHostObject CommonHostObject;
#endif /* __cplusplus */

#endif 	/* __CommonHostObject_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __CommonHostObjects_LIBRARY_DEFINED__
#define __CommonHostObjects_LIBRARY_DEFINED__

/* library CommonHostObjects */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_CommonHostObjects;

#ifndef __ICommonHostObject_INTERFACE_DEFINED__
#define __ICommonHostObject_INTERFACE_DEFINED__

/* interface ICommonHostObject */
/* [local][object][uuid] */ 


EXTERN_C const IID IID_ICommonHostObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("62C6C473-075B-4A42-A506-676A22F7F0D9")
    ICommonHostObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LogInfo( 
            /* [in] */ BSTR text) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LogWarning( 
            /* [in] */ BSTR text) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LogDebug( 
            /* [in] */ BSTR text) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LogError( 
            /* [in] */ BSTR text) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DialogMethod( 
            /* [in] */ BSTR text) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExecuteAction( 
            /* [in] */ BSTR actionName,
            /* [in] */ BSTR payload) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_UserName( 
            /* [retval][out] */ BSTR *userName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_UserPassword( 
            /* [retval][out] */ BSTR *userPassword) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_UserCredentials( 
            /* [retval][out] */ BSTR *userCredentials) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CallCallbackAsynchronously( 
            /* [in] */ IDispatch *callbackParameter) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICommonHostObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICommonHostObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICommonHostObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICommonHostObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *LogInfo )( 
            ICommonHostObject * This,
            /* [in] */ BSTR text);
        
        HRESULT ( STDMETHODCALLTYPE *LogWarning )( 
            ICommonHostObject * This,
            /* [in] */ BSTR text);
        
        HRESULT ( STDMETHODCALLTYPE *LogDebug )( 
            ICommonHostObject * This,
            /* [in] */ BSTR text);
        
        HRESULT ( STDMETHODCALLTYPE *LogError )( 
            ICommonHostObject * This,
            /* [in] */ BSTR text);
        
        HRESULT ( STDMETHODCALLTYPE *DialogMethod )( 
            ICommonHostObject * This,
            /* [in] */ BSTR text);
        
        HRESULT ( STDMETHODCALLTYPE *ExecuteAction )( 
            ICommonHostObject * This,
            /* [in] */ BSTR actionName,
            /* [in] */ BSTR payload);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserName )( 
            ICommonHostObject * This,
            /* [retval][out] */ BSTR *userName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserPassword )( 
            ICommonHostObject * This,
            /* [retval][out] */ BSTR *userPassword);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserCredentials )( 
            ICommonHostObject * This,
            /* [retval][out] */ BSTR *userCredentials);
        
        HRESULT ( STDMETHODCALLTYPE *CallCallbackAsynchronously )( 
            ICommonHostObject * This,
            /* [in] */ IDispatch *callbackParameter);
        
        END_INTERFACE
    } ICommonHostObjectVtbl;

    interface ICommonHostObject
    {
        CONST_VTBL struct ICommonHostObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICommonHostObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICommonHostObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICommonHostObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICommonHostObject_LogInfo(This,text)	\
    ( (This)->lpVtbl -> LogInfo(This,text) ) 

#define ICommonHostObject_LogWarning(This,text)	\
    ( (This)->lpVtbl -> LogWarning(This,text) ) 

#define ICommonHostObject_LogDebug(This,text)	\
    ( (This)->lpVtbl -> LogDebug(This,text) ) 

#define ICommonHostObject_LogError(This,text)	\
    ( (This)->lpVtbl -> LogError(This,text) ) 

#define ICommonHostObject_DialogMethod(This,text)	\
    ( (This)->lpVtbl -> DialogMethod(This,text) ) 

#define ICommonHostObject_ExecuteAction(This,actionName,payload)	\
    ( (This)->lpVtbl -> ExecuteAction(This,actionName,payload) ) 

#define ICommonHostObject_get_UserName(This,userName)	\
    ( (This)->lpVtbl -> get_UserName(This,userName) ) 

#define ICommonHostObject_get_UserPassword(This,userPassword)	\
    ( (This)->lpVtbl -> get_UserPassword(This,userPassword) ) 

#define ICommonHostObject_get_UserCredentials(This,userCredentials)	\
    ( (This)->lpVtbl -> get_UserCredentials(This,userCredentials) ) 

#define ICommonHostObject_CallCallbackAsynchronously(This,callbackParameter)	\
    ( (This)->lpVtbl -> CallCallbackAsynchronously(This,callbackParameter) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICommonHostObject_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CommonHostObject;

#ifdef __cplusplus

class DECLSPEC_UUID("64DBD606-9088-42DB-B39E-B3EAFBE2EDDD")
CommonHostObject;
#endif
#endif /* __CommonHostObjects_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


