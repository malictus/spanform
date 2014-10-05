//progdlg.cpp
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#include "stdafx.h"
#include "spanform.h"
#include "progdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cprogdlg dialog


Cprogdlg::Cprogdlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cprogdlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cprogdlg)
	m_unitleft = _T("");
	m_unitanalyzed = _T("");
	//}}AFX_DATA_INIT
	m_unitleft = "ere";
}


void Cprogdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cprogdlg)
	DDX_Control(pDX, IDC_PROGRESS1, progger);
	DDX_Text(pDX, IDC_unitleft, m_unitleft);
	DDX_Text(pDX, IDC_unitsanalyzed, m_unitanalyzed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Cprogdlg, CDialog)
	//{{AFX_MSG_MAP(Cprogdlg)
	ON_BN_CLICKED(IDC_Keep, OnKeep)
	ON_BN_CLICKED(IDC_AnalysisCancel, OnAnalysisCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cprogdlg message handlers

BOOL Cprogdlg::OnInitDialog() 
{
	//0=keepgoing; 1=Keep. 2=Cancel
	progstatus = 0;
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Cprogdlg::OnKeep() 
{
	progstatus = 1;
}

void Cprogdlg::OnAnalysisCancel() 
{
	progstatus = 2;
}
