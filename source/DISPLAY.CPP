//Display.cpp
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#include "stdafx.h"
#include "spanform.h"
#include "Display.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplay dialog

CDisplay::CDisplay(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplay::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDisplay)
	//}}AFX_DATA_INIT
}


void CDisplay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplay)
	DDX_Control(pDX, IDC_vert, m_vert);
	DDX_Control(pDX, IDC_hor, m_hor);
	DDX_Control(pDX, IDC_noize, m_noize);
	DDX_Control(pDX, IDC_Sensitive, m_sensitive);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplay, CDialog)
	//{{AFX_MSG_MAP(CDisplay)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_BN_CLICKED(IDC_BW, OnBw)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_Hz, OnHz)
	ON_BN_CLICKED(IDC_Letters, OnLetters)
	ON_BN_CLICKED(IDC_Linear, OnLinear)
	ON_BN_CLICKED(IDC_Expo, OnExpo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//gogo - show dialog, get info
bool CDisplay::gogo(CSpanformDoc* pDoc)
{
	//used for cancel or not
	int cooper;
	//setup dialog vars based on existing doc info
	clr = pDoc->docdobj.clr;
	noize = pDoc->docdobj.noisy;
	sensitive = pDoc->docdobj.sensitive;
	horr = pDoc->docdobj.horr;
	vertt = pDoc->docdobj.vertt;
	lett = pDoc->docdobj.lett;
	expo = pDoc->docdobj.expo;
	//show dialog
	cooper = DoModal();
	if (cooper == IDCANCEL)
	{
		//don't change anything, user canceled
		return false;
	}
	//set document variables to reflect our new orientation
	pDoc->docdobj.clr = clr;
	pDoc->docdobj.sensitive = sensitive;
	pDoc->docdobj.noisy = noize;
	pDoc->docdobj.horr = horr;
	pDoc->docdobj.vertt = vertt;
	pDoc->docdobj.lett = lett;
	pDoc->docdobj.expo = expo;
	return true;
}
/////////////////////////////////////////////////////////////////////////////
// CDisplay message handlers
//setup dialog buttons with defaults, based on local variables
BOOL CDisplay::OnInitDialog()
{
	CDialog::OnInitDialog();
	//setup dialog buttons with defaults, based on local variables
	if (lett == TRUE)
		CheckDlgButton(IDC_Letters, 1);
	else
		CheckDlgButton(IDC_Hz, 1);
	if (clr == TRUE)
		CheckDlgButton(IDC_COLOR, 1);
	else
		CheckDlgButton(IDC_BW, 1);
	if (expo == TRUE)
		CheckDlgButton(IDC_Expo, 1);
	else
		CheckDlgButton(IDC_Linear, 1);
	m_sensitive.SetRange(0, 9);
	m_sensitive.SetPos(sensitive);
	m_noize.SetRange(0, 9);
	m_noize.SetPos(noize);
	m_hor.SetRange(1, 20);
	m_vert.SetRange(1, 20);
	m_hor.SetPos(horr);
	m_vert.SetPos(vertt);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDisplay::OnColor() 
{
	clr = TRUE;
}

void CDisplay::OnBw() 
{
	clr = FALSE;
}

void CDisplay::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ((CSliderCtrl*)pScrollBar == &m_sensitive)
		sensitive = m_sensitive.GetPos();
	if ((CSliderCtrl*)pScrollBar == &m_noize)
		noize = m_noize.GetPos();
	if ((CSliderCtrl*)pScrollBar == &m_hor)
		horr = m_hor.GetPos();
	if ((CSliderCtrl*)pScrollBar == &m_vert)
		vertt = m_vert.GetPos();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDisplay::OnHz() 
{
	lett = FALSE;
}

void CDisplay::OnLetters() 
{
	lett = TRUE;
}

void CDisplay::OnLinear() 
{
	expo = FALSE;	
}

void CDisplay::OnExpo() 
{
	expo = TRUE;	
}
