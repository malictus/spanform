//SpanformDoc.cpp
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#include "stdafx.h"
#include "Spanform.h"
#include "dobj.h"
#include "SpanformDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpanformDoc

IMPLEMENT_DYNCREATE(CSpanformDoc, CDocument)
BEGIN_MESSAGE_MAP(CSpanformDoc, CDocument)
	//{{AFX_MSG_MAP(CSpanformDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpanformDoc construction/destruction

CSpanformDoc::CSpanformDoc(){}

CSpanformDoc::~CSpanformDoc(){}

BOOL CSpanformDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSpanformDoc serialization
void CSpanformDoc::Serialize(CArchive& ar)
{
	//simply serialize the dobj object
	docdobj.Serialize(ar);
	if (ar.IsLoading())
	{
		//ensure that a foreign file does not get loaded!!!
		if (getversion() != 1)
			AfxThrowArchiveException(CArchiveException::badIndex);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSpanformDoc diagnostics
#ifdef _DEBUG
void CSpanformDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSpanformDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSpanformDoc commands
// simple retrieval functions

short CSpanformDoc::getversion()
{
	return docdobj.getversion();
}

CString CSpanformDoc::getname()
{
	return docdobj.getname();
}

CString CSpanformDoc::getpath()
{
	return docdobj.getpath();
}

short CSpanformDoc::getchan()
{
	return docdobj.getchan();
}

int CSpanformDoc::getbits()
{
	return docdobj.getbits();
}

long CSpanformDoc::getsamprate()
{
	return docdobj.getsamprate();
}

float CSpanformDoc::getlengthy()
{
	return docdobj.getlengthy();
}

long CSpanformDoc::getnumsamps()
{
	return docdobj.getnumsamps();
}

int CSpanformDoc::getpower2()
{
	return docdobj.getpower2();
}

int CSpanformDoc::getslider()
{
	return docdobj.getslider();
}

int CSpanformDoc::getspanwindow()
{
	return docdobj.getspanwindow();
}

long CSpanformDoc::getnumframes()
{
	return docdobj.getnumframes();
}

int CSpanformDoc::getframesize()
{
	return docdobj.getframesize();
}

int CSpanformDoc::gethifreq()
{
	return docdobj.gethifreq();
}
