//progdlg.h
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#if !defined(AFX_PROGDLG_H__7BD273A0_F362_11D2_A630_997F76BF2239__INCLUDED_)
#define AFX_PROGDLG_H__7BD273A0_F362_11D2_A630_997F76BF2239__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Cprogdlg dialog

class Cprogdlg : public CDialog
{
// Construction
public:
	Cprogdlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Cprogdlg)
	enum { IDD = IDD_PROG };
	CProgressCtrl	progger;
	CString	m_unitleft;
	CString	m_unitanalyzed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cprogdlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cprogdlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeep();
	afx_msg void OnAnalysisCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int progstatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGDLG_H__7BD273A0_F362_11D2_A630_997F76BF2239__INCLUDED_)
