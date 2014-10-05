//Spanform.h
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#if !defined(AFX_SPANFORM_H__B9125B04_DA5C_11D2_A630_D5FD7E869135__INCLUDED_)
#define AFX_SPANFORM_H__B9125B04_DA5C_11D2_A630_D5FD7E869135__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CSpanformApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//my dummy function
	void MyDummyNew();
	CSpanformApp();
	//pitch arrays
	int pitch[9];
	CString spitch[9];
	//true if audio file is playing at the moment
	BOOL weplaying;
	//true if ANY file audio is open, since we only open one at a time
	BOOL weopen;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpanformApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSpanformApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileCreate();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPANFORM_H__B9125B04_DA5C_11D2_A630_D5FD7E869135__INCLUDED_)
