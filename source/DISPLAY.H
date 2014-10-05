//Display.h
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#if !defined(AFX_DISPLAY_H__2ACD43C0_FA62_11D2_A630_EA8784DA323E__INCLUDED_)
#define AFX_DISPLAY_H__2ACD43C0_FA62_11D2_A630_EA8784DA323E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SpanformDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDisplay dialog

class CDisplay : public CDialog
{
// Construction
public:
	CDisplay(CWnd* pParent = NULL);   // standard constructor
	//get data and show dialog
	bool gogo(CSpanformDoc* pDoc);
	
	//local vars which mimic document vars
	BOOL clr;
	BOOL lett;
	BOOL expo;
	int sensitive;
	int noize;
	int horr;
	int vertt;

// Dialog Data
	//{{AFX_DATA(CDisplay)
	enum { IDD = IDD_DISPLAY };
	CSliderCtrl	m_vert;
	CSliderCtrl	m_hor;
	CSliderCtrl	m_noize;
	CSliderCtrl	m_sensitive;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplay)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDisplay)
	virtual BOOL OnInitDialog();
	afx_msg void OnColor();
	afx_msg void OnBw();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHz();
	afx_msg void OnLetters();
	afx_msg void OnLinear();
	afx_msg void OnExpo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAY_H__2ACD43C0_FA62_11D2_A630_EA8784DA323E__INCLUDED_)
