#include <string.h>
#include <iostream>
#include "my.h"
using namespace std;
extern CMyWinApp theApp;

CRuntimeClass* CRuntimeClass::pFirstClass = NULL;
//AFX_ASSINIT�������һ�������һ����̬���󣬲�������������ȴ����λ�����У�����λ���ļ��ģ�ͨ�������̬�������������乹�캯����������������ӹ���
AFX_CLASSINIT::AFX_CLASSINIT(CRuntimeClass* pNewClass)
{
	pNewClass->m_pNextClass = CRuntimeClass::pFirstClass;
	CRuntimeClass::pFirstClass = pNewClass;
}

//CObject��ͷ����Ҫ���⴦����
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
IMPLEMENT_DYNCREATE(CWnd, CCmdTarget) // ��ʵ��MFC ����IMPLEMENT_DYNCREATE()�����ڡ�
IMPLEMENT_DYNCREATE(CFrameWnd, CWnd) // ��ʵ��MFC ����IMPLEMENT_DYNCREATE()�����ڡ�
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
//���ļ���load
CRuntimeClass* PASCAL CRuntimeClass::Load(CArchive& ar, UINT* pwSchemaNum)
{
	WORD nLen = 0;
	char szClassName[64];
	CRuntimeClass* pClass;
	char c = '|';
	int SchemaNum = 0;
	ar >> (WORD&)SchemaNum >>  c >> nLen >> c;//���ʶ,��������

	*pwSchemaNum = SchemaNum;
	if (nLen >= sizeof(szClassName) || ar.Read(szClassName, nLen) != nLen)//������
		return NULL;
	szClassName[nLen] = '\0';
	for (pClass = pFirstClass; pClass != NULL; pClass = pClass->m_pNextClass)
	{
		if (strcmp(szClassName, pClass->m_lpszClassName) == 0)//��ȡ�����ƶ�Ӧ�� CRuntimeClass
		{
			ar >> c;
			return pClass;
		}
	}
	ar >> c;
	return NULL; // not found
}
//����������
void CRuntimeClass::Store(CArchive& ar) const
// stores a runtime class description
{
	WORD nLen = (WORD)strlen(m_lpszClassName);
	char c= '|';
	ar << (WORD)m_wSchema <<c<< nLen<<c;
	ar.Write(m_lpszClassName, nLen*sizeof(char));
	ar << c;
}

CWinApp* AfxGetApp()
{
	return theApp.m_pCurrentWinApp;
}