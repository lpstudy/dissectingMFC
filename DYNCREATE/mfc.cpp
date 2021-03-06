#include "my.h"
#include <iostream>
using namespace std;
extern CMyWinApp theApp;

CRuntimeClass* CRuntimeClass::pFirstClass = NULL;
//AFX_ASSINIT这个对象一个类会有一个静态对象，不过它的作用域却不是位于类中，而是位于文件的，通过这个静态对象自身调用其构造函数来完成链表的连接过程
AFX_CLASSINIT::AFX_CLASSINIT(CRuntimeClass* pNewClass)
{
	pNewClass->m_pNextClass = CRuntimeClass::pFirstClass;
	CRuntimeClass::pFirstClass = pNewClass;
}

//CObject的头部需要特殊处理，
static char szCObject[] = "CObject";
struct CRuntimeClass CObject::classCObject =
{ szCObject, sizeof(CObject), 0xffff, NULL, NULL };
static AFX_CLASSINIT _init_CObject(&CObject::classCObject);
CRuntimeClass* CObject::GetRuntimeClass() const
{
	return &CObject::classCObject;
}

IMPLEMENT_DYNAMIC(CCmdTarget, CObject)
IMPLEMENT_DYNAMIC(CWinThread, CCmdTarget)
IMPLEMENT_DYNAMIC(CWinApp, CWinThread)
IMPLEMENT_DYNCREATE(CWnd, CCmdTarget) // 其实在MFC 它是IMPLEMENT_DYNCREATE()，见节。
IMPLEMENT_DYNCREATE(CFrameWnd, CWnd) // 其实在MFC 它是IMPLEMENT_DYNCREATE()，见节。
IMPLEMENT_DYNAMIC(CDocument, CCmdTarget)
IMPLEMENT_DYNAMIC(CView, CWnd)

// in implementation file
BOOL CObject::IsKindOf(const CRuntimeClass* pClass) const
{
	CRuntimeClass* pClassThis = GetRuntimeClass();
	while (pClassThis != NULL)
	{
		if (pClassThis == pClass)
			return TRUE;
		pClassThis = pClassThis->m_pBaseClass;
	}
	return FALSE; // walked to the top, no match
}
void PrintAllClasses()
{
	CRuntimeClass* pClass;
	// just walk through the simple list of registered classes
	for (pClass = CRuntimeClass::pFirstClass; pClass != NULL;
		pClass = pClass->m_pNextClass)
	{
		TRACE_FUCTION_AND_LINE("%s", pClass->m_lpszClassName);
		TRACE_FUCTION_AND_LINE("%d", pClass->m_nObjectSize);
		TRACE_FUCTION_AND_LINE("%d", pClass->m_wSchema);
	}
}
// in implementation file
CObject* CRuntimeClass::CreateObject()
{
	if (m_pfnCreateObject == NULL)
	{
		TRACE_FUCTION_AND_LINE("Error: Trying to create object which is not "
			"DECLARE_DYNCREATE \nor DECLARE_SERIAL: %s.\n",
			m_lpszClassName);
		return NULL;
	}
	CObject* pObject = NULL;
	pObject = (*m_pfnCreateObject)();
	return pObject;
}
CRuntimeClass* PASCAL CRuntimeClass::Load()
{
	char szClassName[64];
	CRuntimeClass* pClass;
	// JJHOU : instead of Load from file, we Load from cin.
	cout << "enter a class name... ";
	cin >> szClassName;
	for (pClass = pFirstClass; pClass != NULL; pClass = pClass->m_pNextClass)
	{
		if (strcmp(szClassName, pClass->m_lpszClassName) == 0)
			return pClass;
	}
	TRACE_FUCTION_AND_LINE("Error: Class not found: %s \n", szClassName);
	return NULL; // not found
}

CWinApp* AfxGetApp()
{
	return theApp.m_pCurrentWinApp;
}