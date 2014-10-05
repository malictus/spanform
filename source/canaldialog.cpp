//canaldialog.cpp
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#include "stdafx.h"
#include "Spanform.h"
#include "canaldialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// canaldialog dialog

canaldialog::canaldialog(CWnd* pParent /*=NULL*/)
	: CDialog(canaldialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(canaldialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void canaldialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(canaldialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

//some have been changed so slider works better
BEGIN_MESSAGE_MAP(canaldialog, CDialog)
	//{{AFX_MSG_MAP(canaldialog)
	ON_BN_CLICKED(IDC_size1024, Onsize1024)
	ON_BN_CLICKED(IDC_size128, Onsize128)
	ON_BN_CLICKED(IDC_size2048, Onsize2048)
	ON_BN_CLICKED(IDC_size256, Onsize256)
	ON_BN_CLICKED(IDC_size512, Onsize512)
	ON_BN_CLICKED(IDC_slide100, Onslide100)
	ON_BN_CLICKED(IDC_slide150, Onslide150)
	ON_BN_CLICKED(IDC_slide200, Onslide200)
	ON_BN_CLICKED(IDC_slide300, Onslide300)
	ON_BN_CLICKED(IDC_slide400, Onslide500)
	ON_BN_CLICKED(IDC_slide50, Onslide50)
	ON_BN_CLICKED(IDC_Rectangular, OnRectangular)
	ON_BN_CLICKED(IDC_Hamming, OnHamming)
	ON_BN_CLICKED(IDC_slide700, Onslide1600)
	ON_BN_CLICKED(IDC_slide600, Onslide1000)
	ON_BN_CLICKED(IDC_slide500, Onslide700)
	ON_BN_CLICKED(IDC_12000Cut, On12000Cut)
	ON_BN_CLICKED(IDC_6000cut, On6000cut)
	ON_BN_CLICKED(IDC_9000cut, On9000cut)
	ON_BN_CLICKED(IDC_nocut, Onnocut)
	ON_BN_CLICKED(IDC_size4096, Onsize4096)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canaldialog message handlers
BOOL canaldialog::OnInitDialog() 
{
	//set dialog defaults
	CDialog::OnInitDialog();
	CheckDlgButton(IDC_size1024,1);
	CheckDlgButton(IDC_Hamming,1);
	CheckDlgButton(IDC_slide200,1);
	CheckDlgButton(IDC_12000Cut,1);
	//also add variable defaults
	power2 = 1024;
	slider = 200;
	//1 = rectangular, 2 = Hamming
	windowtype = 2;
	hifreq = 12000;
	return TRUE;
}

////////////////////////////////MESSAGE HANDLERS/////////////////////////
void canaldialog::Onsize1024() 
{power2=1024;}

void canaldialog::Onsize128() 
{power2=128;}

void canaldialog::Onsize2048() 
{power2=2048;}

void canaldialog::Onsize256() 
{power2=256;}

void canaldialog::Onsize512() 
{power2=512;}

void canaldialog::Onslide100() 
{slider=100;}

void canaldialog::Onslide150() 
{slider=150;}

void canaldialog::Onslide200() 
{slider=200;}

void canaldialog::Onslide300() 
{slider=300;}

void canaldialog::Onslide500() 
{slider=500;}

void canaldialog::Onslide50() 
{slider=50;}

void canaldialog::Onslide700() 
{slider = 700;}

void canaldialog::Onslide1000() 
{slider = 1000;}

void canaldialog::Onslide1600() 
{slider = 1600;}

void canaldialog::OnRectangular() 
{windowtype = 1;}

void canaldialog::OnHamming() 
{windowtype = 2;}

void canaldialog::On12000Cut() 
{hifreq = 12000;}

void canaldialog::On6000cut() 
{hifreq = 6000;}

void canaldialog::On9000cut() 
{hifreq = 9000;}

void canaldialog::Onnocut() 
{hifreq = 20000;}

void canaldialog::Onsize4096() 
{power2 = 4096;}
