//SpanformView.h
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#if !defined(AFX_SPANFORMVIEW_H__B9125B0E_DA5C_11D2_A630_D5FD7E869135__INCLUDED_)
#define AFX_SPANFORMVIEW_H__B9125B0E_DA5C_11D2_A630_D5FD7E869135__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpanformView : public CScrollView
{
protected: // create from serialization only
	CSpanformView();
	DECLARE_DYNCREATE(CSpanformView)

// Attributes
public:
	CSpanformDoc* GetDocument();

// Operations
public:
	//redraw bitmap from scratch
	void redobit();
	//redraw stretched bitmap
	void restretch();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpanformView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpanformView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//MY GLOBAL VIEW VARIABLES
	//cleanup audio when file is closed
	void Audiocleanup();
	//open the audio file associated with this file
	void Audioopen();
	//timer
	//void OnTimey();
	//rect to invalidate for timer
	CRect ret;
	//true if audio file associated with this file is open
	BOOL audioopen;
	//TIMER ID
	UINT timeyid;
	//audio points
	int startme;
	int starme;
	//version number check
	short versionOK;
	//graph bitmap
	CBitmap mybit;
	//graph bitmap - the actual one being drawn which is stretched
	CBitmap actbit;
	//max amount of space in logical units for the time and freq to be printed on the page
	CSize maxfreq;
	CSize maxtime;
	//point of upper right corner of start of bitmap area to be printed
	CPoint pbitstart;
	//total number of 1/100 of an inch of the paper (used for printing)
	int horzi;
	int verzi;
	//total number of pages to print, x dimension and y dimension...
	int pagex;
	int pagey;
	//portion of bitmap to be printed on each page
	int xer;
	int yer;
	
	MCI_WAVE_OPEN_PARMS openParams;
	
// Generated message map functions
protected:
	//{{AFX_MSG(CSpanformView)
	afx_msg void OnSpanformShowfileinfo();
	afx_msg void OnSpanformDisplay();
	afx_msg void OnAudioPlay();
	afx_msg void OnAudioStop();
	afx_msg void OnAudioAssociate();
	afx_msg void OnAudioRestart();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT vall);
	afx_msg LONG OnMciNotify(UINT wParam, LONG lDevId);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SpanformView.cpp
inline CSpanformDoc* CSpanformView::GetDocument()
   { return (CSpanformDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPANFORMVIEW_H__B9125B0E_DA5C_11D2_A630_D5FD7E869135__INCLUDED_)
