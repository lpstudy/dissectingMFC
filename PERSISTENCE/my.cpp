#include "my.h"

CMyWinApp theApp;

IMPLEMENT_DYNCREATE(CMyFrameWnd, CFrameWnd)
IMPLEMENT_DYNCREATE(CMyDoc, CDocument)
IMPLEMENT_DYNCREATE(CMyView, CView)
IMPLEMENT_SERIAL(CStroke, CObject, 1000)
IMPLEMENT_SERIAL(CRectangle, CObject, 10000)
IMPLEMENT_SERIAL(CCircle, CObject, 10000)