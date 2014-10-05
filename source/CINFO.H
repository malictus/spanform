//cinfo.h
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#if !defined(AFX_CINFO_H__93938620_F757_11D2_A630_F1BAA2A77E38__INCLUDED_)
#define AFX_CINFO_H__93938620_F757_11D2_A630_F1BAA2A77E38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// cinfo dialog

class cinfo : public CDialog
{
// Construction
public:
	cinfo(CSpanformDoc* pDoc, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(cinfo)
	enum { IDD = IDD_INFO };
	CString	m_infoname;
	CString	m_infosamprate;
	CString	m_infobits;
	CString	m_infochannels;
	CString	m_infolength;
	CString	m_infoanalframes;
	CString	m_infocutoff;
	CString	m_infoframesize;
	CString	m_infopower2;
	CString	m_infosampunits;
	CString	m_infoslide;
	CString	m_infowindow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cinfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cinfo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CINFO_H__93938620_F757_11D2_A630_F1BAA2A77E38__INCLUDED_)
