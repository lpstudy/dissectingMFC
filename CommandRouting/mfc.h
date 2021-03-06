#ifndef __MFC__H__
#define __MFC__H__
#include "log.h"
#include "typedef.h"

//每一个类有一个CRuntimeClass静态对象，于是又多少个类，就有多少个CRuntimeClass对象
//所有类的CRuntimeClass对象组成一个链表，链表的头部又是一个静态指针对象。
//于是整个mfc就只有一个链表头部静态指针对象,pFirstClass
struct CRuntimeClass;
class CObject;
class CArchive;
class CCmdTarget;
class CView;
class CWinApp;
class CWnd;

struct AFX_CLASSINIT
{ AFX_CLASSINIT(CRuntimeClass* pNewClass); };

struct CRuntimeClass
{
	// Attributes
	LPCSTR m_lpszClassName;
	int m_nObjectSize;
	UINT m_wSchema; // schema number of the loaded class
	CObject* (PASCAL* m_pfnCreateObject)(); // NULL => abstract class
	CRuntimeClass* m_pBaseClass;
	CObject* CreateObject();
	void Store(CArchive& ar) const;
	static CRuntimeClass* PASCAL Load(CArchive& ar, UINT* pwSchemaNum);
	//static CRuntimeClass* PASCAL Load();
	// CRuntimeClass objects linked together in simple list
	static CRuntimeClass* pFirstClass; // start of class list
	CRuntimeClass* m_pNextClass; // linked list of registered classes
};

//序列化，重载对象的输入到文件和输出到文件
#define DECLARE_SERIAL(class_name) \
	DECLARE_DYNCREATE(class_name) \
	//friend CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb);

#define DECLARE_DYNAMIC(class_name) \
public: \
	static CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const;
#define DECLARE_DYNCREATE(class_name) \
	DECLARE_DYNAMIC(class_name) \
	static CObject* PASCAL CreateObject();
#define RUNTIME_CLASS(class_name) \
	(&class_name::class##class_name)


#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
	_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, NULL)

#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name,wSchema,pfnNew) \
	static char _lpsz##class_name[] = #class_name; \
	CRuntimeClass class_name::class##class_name = { \
	_lpsz##class_name, sizeof(class_name), wSchema, pfnNew, \
	RUNTIME_CLASS(base_class_name), NULL }; \
	static AFX_CLASSINIT _init_##class_name(&class_name::class##class_name); \
	CRuntimeClass* class_name::GetRuntimeClass() const \
	{ return &class_name::class##class_name; } \

//实现序列号，重载>>运算符
#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema) \
	CObject* PASCAL class_name::CreateObject() \
	{ return new class_name; } \
	_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, \
	class_name::CreateObject) \
	//CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb) \
	//{ pOb = (class_name*) ar.ReadObject(RUNTIME_CLASS(class_name)); \
	//return ar; } 

#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
	CObject* PASCAL class_name::CreateObject() \
	{ return new class_name; } \
	_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, class_name::CreateObject)

//每一个类有一个此静态数据成员，它包含基类的map和自身的lpEntries数据。
struct AFX_MSGMAP_ENTRY;
struct AFX_MSGMAP
{
	AFX_MSGMAP* pBaseMessageMap;
	AFX_MSGMAP_ENTRY* lpEntries;
};

typedef void (CCmdTarget::*AFX_PMSG)(void);

#define DECLARE_MESSAGE_MAP() \
	static AFX_MSGMAP_ENTRY _messageEntries[]; \
	static AFX_MSGMAP messageMap; \
	virtual AFX_MSGMAP* GetMessageMap() const;

struct AFX_MSGMAP_ENTRY // MFC 4.0 format
{
	UINT nMessage; // windows message
	UINT nCode; // control code or WM_NOTIFY code
	UINT nID; // control ID (or 0 for windows messages)
	UINT nLastID; // used for entries specifying a range of control id's
	UINT nSig; // signature type (action) or pointer to message #
	AFX_PMSG pfn; // routine to call (or special value)
};

enum AfxSig
{
	AfxSig_end = 0, // [marks end of message map]
	AfxSig_vv,
};

#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
	AFX_MSGMAP* theClass::GetMessageMap() const \
{ return &theClass::messageMap; } \
	AFX_MSGMAP theClass::messageMap = \
{ &(baseClass::messageMap), \
	(AFX_MSGMAP_ENTRY*) &(theClass::_messageEntries) }; \
	AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
{
#define ON_COMMAND(id, memberFxn) \
{ WM_COMMAND, 0, (WORD)id, (WORD)id, AfxSig_vv, (AFX_PMSG)memberFxn },
#define ON_NON_COMMAND(message, id, memberFxn) \
{ message, 0, (WORD)id, (WORD)id, AfxSig_vv, (AFX_PMSG)memberFxn },
#define END_MESSAGE_MAP() \
{ 0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 } \
};\


#define WM_COMMAND 0x0111
#define WM_CREATE 0x0001
#define WM_PAINT 0x000F
#define WM_NOTIFY 0x004E

#define CObjectid	  0xffff
#define CCmdTargetid  1
#define CWinThreadid  11
#define CWinAppid     111
#define CMyWinAppid   1111
#define CWndid        12
#define CFrameWndid   121
#define CMyFrameWndid 1211
#define CViewid       122
#define CMyViewid     1221
#define CDocumentid   13
#define CMyDocid      131

void PrintAllClasses();
CWinApp* AfxGetApp();
LRESULT AfxWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam,
				   CWnd *pWnd);
LRESULT AfxCallWndProc(CWnd* pWnd, HWND hWnd, UINT nMsg,
					   WPARAM wParam, LPARAM lParam);
extern void printlpEntries(AFX_MSGMAP_ENTRY* lpEntry);
extern BOOL calllpEntries(AFX_MSGMAP_ENTRY* lpEntry, UINT nMessage, UINT nId, CCmdTarget* handle);
extern void MsgMapPrinting(AFX_MSGMAP* pMessageMap);

//CObject
class CObject
{
public:
	CObject()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
	~CObject()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
public:
	virtual CRuntimeClass* GetRuntimeClass() const;
	BOOL IsKindOf(const CRuntimeClass* pClass) const;
public:
	static CRuntimeClass classCObject;
public:
	virtual void SayHello()
	{
		TRACE_FUCTION_AND_LINE("Hello!");
	}
	virtual void Serialize(CArchive& ar)
	{
		TRACE_FUCTION_AND_LINE("");
	}
};

//CCmdTarget
class CCmdTarget : public CObject
{
	DECLARE_DYNAMIC(CCmdTarget)
	DECLARE_MESSAGE_MAP()
public:
	CCmdTarget()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
	~CCmdTarget()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
public:
	virtual void SayHello()
	{
		TRACE_FUCTION_AND_LINE("Hello!");
	}
	virtual BOOL OnCmdMsg(UINT nID, int nCode)
	{
		TRACE_FUCTION_AND_LINE("");
		// Now look through message map to see if it applies to us
		AFX_MSGMAP* pMessageMap;
		AFX_MSGMAP_ENTRY* lpEntry;
		for (pMessageMap = GetMessageMap(); pMessageMap != NULL;
			pMessageMap = pMessageMap->pBaseMessageMap)
		{
			lpEntry = pMessageMap->lpEntries;
			printlpEntries(lpEntry);
		}

		return FALSE; // not handled
	}
};

//CWinThread
class CWinThread : public CCmdTarget
{
	DECLARE_DYNAMIC(CWinThread)
public:
	CWinThread()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
	~CWinThread()
	{
		//TRACE_FUCTION_AND_LINE("");
	}

public:
	virtual BOOL InitInstance()
	{
		TRACE_FUCTION_AND_LINE("");
		return true;
	}
	virtual int Run()
	{
		TRACE_FUCTION_AND_LINE("");
		return 1;
	}
public:
	virtual void SayHello()
	{
		TRACE_FUCTION_AND_LINE("Hello!");
	}
};

class CWnd;

//CWinApp
class CWinApp : public CWinThread
{
	DECLARE_DYNAMIC(CWinApp)
	DECLARE_MESSAGE_MAP()
public:
	CWinApp* m_pCurrentWinApp;
	CWnd*    m_pMainWnd;
public:
	CWinApp()
	{
		m_pCurrentWinApp = this;
		//TRACE_FUCTION_AND_LINE("");
	}
	~CWinApp()
	{
		//TRACE_FUCTION_AND_LINE("");
	}

public:
	virtual BOOL InitInstance()
	{
		TRACE_FUCTION_AND_LINE("");
		return true;
	}
	virtual BOOL InitApplication()
	{
		TRACE_FUCTION_AND_LINE("");
		return true;
	}
	virtual int Run()
	{
		//TRACE_FUCTION_AND_LINE("");
		CWinThread::Run();
		return 1;
	}
public:
	virtual void SayHello()
	{
		TRACE_FUCTION_AND_LINE("Hello!");
	}
	virtual BOOL OnCmdMsg(UINT nID, int nCode)
	{
		TRACE_FUCTION_AND_LINE("");
		if (CCmdTarget::OnCmdMsg(nID, nCode))
			return TRUE;

		return FALSE;
	}
};
//CObject->CCmdTarget->CWinThread->CWinApp

//CDocument
class CDocument : public CCmdTarget
{
	DECLARE_DYNAMIC(CDocument)
	DECLARE_MESSAGE_MAP()
public:
	CDocument()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
	~CDocument()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
public:
	virtual void SayHello()
	{
		TRACE_FUCTION_AND_LINE("Hello!");
	}
	virtual BOOL OnCmdMsg(UINT nID, int nCode)
	{
		TRACE_FUCTION_AND_LINE("");
		if (CCmdTarget::OnCmdMsg(nID, nCode))
			return TRUE;

		return FALSE;
	}

};
//CObject->CCmdTarget->CDocument

//CWnd
class CWnd : public CCmdTarget
{
	DECLARE_DYNCREATE(CWnd)
	DECLARE_MESSAGE_MAP()
public:
	CWnd()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
	~CWnd()
	{
		//TRACE_FUCTION_AND_LINE("");
	}

public:
	virtual BOOL Create()
	{
		//TRACE_FUCTION_AND_LINE("");
		return 1;
	}
	BOOL CreateEx()
	{
		//TRACE_FUCTION_AND_LINE("");
		PreCreateWindow();
		return true;
	}
	virtual BOOL PreCreateWindow()
	{
		//TRACE_FUCTION_AND_LINE("");
		return true;
	}
public:
	LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
	{
		TRACE_FUCTION_AND_LINE("");
		AFX_MSGMAP* pMessageMap;
		AFX_MSGMAP_ENTRY* lpEntry;

		if (nMsg == WM_COMMAND) // special case for commands
		{
			TRACE_FUCTION_AND_LINE("WM_COMMAND message");
			if (OnCommand(wParam, lParam))
				return 1L; // command handled
			else
				return (LRESULT)DefWindowProc(nMsg, wParam, lParam);
		}

		TRACE_FUCTION_AND_LINE("WM_XXXXXX message");
		pMessageMap = GetMessageMap();

		UINT nMessage= nMsg;
		UINT nId  = wParam;

		for (; pMessageMap != NULL;
			pMessageMap = pMessageMap->pBaseMessageMap)
		{
			lpEntry = pMessageMap->lpEntries;
			if(calllpEntries(lpEntry,nMessage, nId, this))
				return 1L;

		}
		return 0; // add by JJHou. if find, should call lpEntry->pfn,
		// otherwise should call DefWindowProc.
	}

	LRESULT CWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		TRACE_FUCTION_AND_LINE("");
		return TRUE;
	}

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam)//begin virtual
	{
		// ...
		TRACE_FUCTION_AND_LINE("");
		return OnCmdMsg(0, 0);
	}
	virtual BOOL OnCmdMsg(UINT nID, int nCode)
	{
		TRACE_FUCTION_AND_LINE("");
		if (CCmdTarget::OnCmdMsg(nID, nCode))
			return TRUE;

		return FALSE;
	}
public:
	virtual void SayHello()
	{
		TRACE_FUCTION_AND_LINE("Hello!");
	}
};

//CView
class CView : public CWnd
{
	DECLARE_DYNAMIC(CView)
	DECLARE_MESSAGE_MAP()
public:
	CView()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
	~CView()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
public:
	CDocument* m_pDocument;
	virtual void SayHello()
	{
		TRACE_FUCTION_AND_LINE("Hello!");
	}
	BOOL OnCmdMsg(UINT nID, int nCode)
	{
		TRACE_FUCTION_AND_LINE("");
		if (CWnd::OnCmdMsg(nID, nCode))
			return TRUE;

		BOOL bHandled = FALSE;
		bHandled = m_pDocument->OnCmdMsg(nID, nCode);
		return bHandled;
	}
	void OnPaint()
	{
		TRACE_FUCTION_AND_LINE("");
	}
	//friend CFrameWnd;
};

//CFrameWnd
class CFrameWnd : public CWnd
{
	DECLARE_DYNCREATE(CFrameWnd)
	DECLARE_MESSAGE_MAP()
public:
	CFrameWnd()
	{
		//TRACE_FUCTION_AND_LINE("");
	}
	~CFrameWnd()
	{
		//TRACE_FUCTION_AND_LINE("");
	}

public:
	BOOL Create()
	{
		//TRACE_FUCTION_AND_LINE("");
		CreateEx();
		return TRUE;
	}
	virtual BOOL PreCreateWindow()
	{
		//TRACE_FUCTION_AND_LINE("");
		return true;
	}
public:
	CView* m_pViewActive;       // current active view
	virtual void SayHello()
	{
		TRACE_FUCTION_AND_LINE("Hello!");
	}
	CView* GetActiveView() const
	{
		return m_pViewActive;
	}
	BOOL OnCmdMsg(UINT nID, int nCode)
	{
		TRACE_FUCTION_AND_LINE("");
		// pump through current view FIRST
		CView* pView = GetActiveView();
		if (pView->OnCmdMsg(nID, nCode))
			return TRUE;

		// then pump through frame
		if (CWnd::OnCmdMsg(nID, nCode))
			return TRUE;

		// last but not least, pump through app
		CWinApp* pApp = AfxGetApp();
		if (pApp->OnCmdMsg(nID, nCode))
			return TRUE;

		return FALSE;
	}
	BOOL OnCommand(WPARAM wParam, LPARAM lParam)
	{
		// ...这个有点冗余，通过父类的WindowProc方法，调用OnCommand方法，会进入到当前类，当前类再调用父类的OnCommand，父类的OnCommand再调用OnCmdMsg，再次回到当前类。
		//这样子的话，实际上还有一个流程，OnCommand的流程
		// route as normal command
		return CWnd::OnCommand(wParam, lParam);
	}

	//friend CView;
};


//CObject->CCmdTarget->CWnd->(CFrameWnd,CView)


#endif//__MFC__H__
