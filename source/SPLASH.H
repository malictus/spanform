//Splash.h
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

class CSplashWnd : public CWnd
{
// Construction
protected:
	CSplashWnd();
// Attributes:
public:
	CBitmap m_bitmap;
// Operations
public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL
// Implementation
public:
	~CSplashWnd();
	virtual void PostNcDestroy();
protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;
// Generated message map functions
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
