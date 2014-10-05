//cinfo.cpp
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#include "StdAfx.h"
#include "Spanform.h"
#include "SpanformDoc.h"
#include "cinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cinfo dialog - reads in document info to dialog fields

cinfo::cinfo(CSpanformDoc* pDoc, CWnd* pParent)
	: CDialog(cinfo::IDD, pParent)
{
	//buffer for reading in strings
	char mybuffer[50];
	
	//{{AFX_DATA_INIT(cinfo)
	m_infoname = _T("");
	m_infosamprate = _T("");
	m_infobits = _T("");
	m_infochannels = _T("");
	m_infolength = _T("");
	m_infoanalframes = _T("");
	m_infocutoff = _T("");
	m_infoframesize = _T("");
	m_infopower2 = _T("");
	m_infosampunits = _T("");
	m_infoslide = _T("");
	m_infowindow = _T("");
	//}}AFX_DATA_INIT

	//set dialog texts according to document
	m_infoname = pDoc->getname();
	_ltoa((pDoc->getsamprate()), mybuffer, 10);
	m_infosamprate = mybuffer;
	_itoa((pDoc->getbits()), mybuffer, 10);
	m_infobits = mybuffer;
	_itoa((pDoc->getchan()), mybuffer, 10);
	m_infochannels = mybuffer;
	int mins = (int)(pDoc->getlengthy()/60);
	float secs = pDoc->getlengthy() - (mins*60);
	char otherbuffer[50];
	_itoa(mins, otherbuffer, 10);
	gcvt(secs, 4, mybuffer);
	m_infolength = otherbuffer;
	m_infolength = m_infolength + "m";
	m_infolength = m_infolength + mybuffer;
	m_infolength = m_infolength + "s";
	_ltoa((pDoc->getnumframes()), mybuffer, 10);
	m_infoanalframes = mybuffer;
	if (pDoc->gethifreq() == 0)
		m_infocutoff = "None.";
	else
	{
	_itoa((pDoc->gethifreq()), mybuffer, 10);
	m_infocutoff = mybuffer;
	}
	_itoa((pDoc->getframesize()), mybuffer, 10);
	m_infoframesize = mybuffer;
	_itoa((pDoc->getpower2()), mybuffer, 10);
	m_infopower2 = mybuffer;
	_ltoa((pDoc->getnumsamps()), mybuffer, 10);
	m_infosampunits = mybuffer;
	_itoa((pDoc->getslider()), mybuffer, 10);
	m_infoslide = mybuffer;
	switch (pDoc->getspanwindow())
	{
	case 1:
		m_infowindow = "Rectangular";
		break;
	case 2:
		m_infowindow = "Hamming";
		break;
	default:
		m_infowindow = "Unknown";
		break;
	}
}

void cinfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cinfo)
	DDX_Text(pDX, IDC_infoname, m_infoname);
	DDX_Text(pDX, IDC_infosamprate, m_infosamprate);
	DDX_Text(pDX, IDC_infobits, m_infobits);
	DDX_Text(pDX, IDC_infochannels, m_infochannels);
	DDX_Text(pDX, IDC_infolength, m_infolength);
	DDX_Text(pDX, IDC_infoanalframes, m_infoanalframes);
	DDX_Text(pDX, IDC_infocutoff, m_infocutoff);
	DDX_Text(pDX, IDC_infoframesize, m_infoframesize);
	DDX_Text(pDX, IDC_infopower2, m_infopower2);
	DDX_Text(pDX, IDC_infosampunits, m_infosampunits);
	DDX_Text(pDX, IDC_infoslide, m_infoslide);
	DDX_Text(pDX, IDC_infowindow, m_infowindow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cinfo, CDialog)
	//{{AFX_MSG_MAP(cinfo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cinfo message handlers
