//canaldialog.h
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#if !defined(AFX_CANALDIALOG_H__D7045160_E9E6_11D2_A630_DB139930963F__INCLUDED_)
#define AFX_CANALDIALOG_H__D7045160_E9E6_11D2_A630_DB139930963F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// canaldialog dialog

class canaldialog : public CDialog
{
// Construction
public:
	canaldialog(CWnd* pParent = NULL);   // standard constructor

	//variables for dialog
	int slider;
	int power2;
	int windowtype;
	int hifreq;

// Dialog Data
	//{{AFX_DATA(canaldialog)
	enum { IDD = IDD_ANAL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(canaldialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(canaldialog)
	virtual BOOL OnInitDialog();
	afx_msg void Onsize1024();
	afx_msg void Onsize128();
	afx_msg void Onsize2048();
	afx_msg void Onsize256();
	afx_msg void Onsize512();
	afx_msg void Onslide100();
	afx_msg void Onslide150();
	afx_msg void Onslide200();
	afx_msg void Onslide300();
	afx_msg void Onslide500();
	afx_msg void Onslide50();
	afx_msg void OnRectangular();
	afx_msg void OnHamming();
	afx_msg void Onslide1600();
	afx_msg void Onslide1000();
	afx_msg void Onslide700();
	afx_msg void On12000Cut();
	afx_msg void On6000cut();
	afx_msg void On9000cut();
	afx_msg void OnHighthresh();
	afx_msg void OnLowThresh();
	afx_msg void OnMediumThresh();
	afx_msg void Onnocut();
	afx_msg void OnNoThresh();
	afx_msg void Onsize4096();
	afx_msg void OnSize128();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANALDIALOG_H__D7045160_E9E6_11D2_A630_DB139930963F__INCLUDED_)
