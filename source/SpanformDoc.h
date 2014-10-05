//SpanformDoc.h
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#if !defined(AFX_SPANFORMDOC_H__B9125B0C_DA5C_11D2_A630_D5FD7E869135__INCLUDED_)
#define AFX_SPANFORMDOC_H__B9125B0C_DA5C_11D2_A630_D5FD7E869135__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dobj.h"

class CSpanformDoc : public CDocument
{
protected: // create from serialization only
	CSpanformDoc();
	DECLARE_DYNCREATE(CSpanformDoc)
//the dobj object associated with each document
public:
	cdobj docdobj;

// Operations - used to retrieve info about the dobj object
public:
	short getversion();
	CString getpath();
	CString getname();
	short getchan();
	int getbits();
	long getsamprate();
	float getlengthy();
	long getnumsamps();
	int getpower2();
	int getslider();
	int getspanwindow();
	long getnumframes();
	int getframesize();
	int gethifreq();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpanformDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpanformDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CSpanformDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPANFORMDOC_H__B9125B0C_DA5C_11D2_A630_D5FD7E869135__INCLUDED_)
