//Spanform.cpp
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#include "stdafx.h"
#include "Spanform.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "SpanformDoc.h"
#include "SpanformView.h"
#include "dobj.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpanformApp

BEGIN_MESSAGE_MAP(CSpanformApp, CWinApp)
	//{{AFX_MSG_MAP(CSpanformApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_CREATE, OnFileCreate)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	//new commands send to empty dummy, just in case
	ON_COMMAND(ID_FILE_NEW, CSpanformApp::MyDummyNew)
	ON_COMMAND(ID_FILE_OPEN, CSpanformApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)

END_MESSAGE_MAP()

//constructor
CSpanformApp::CSpanformApp(){}

//the one and only CSpanformApp object
CSpanformApp theApp;

//InitInstance - Set Up Initial Stuff
BOOL CSpanformApp::InitInstance()
{
	//show splash screen
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	//setup up registry
	SetRegistryKey(_T("Spanform Application"));
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_SPANFOTYPE, RUNTIME_CLASS(CSpanformDoc),
		RUNTIME_CLASS(CChildFrame), RUNTIME_CLASS(CSpanformView));
	AddDocTemplate(pDocTemplate);
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	//drag/drop open
	m_pMainWnd->DragAcceptFiles(TRUE);
	// Enable Files To Be Opened from windows by double clicking them
	EnableShellOpen();
	RegisterShellFileTypes(FALSE);
	
	//read in pitch arrays
	pitch[0] = 33;
	spitch[0] = "C1";
	pitch[1] = 65;
	spitch[1] = "C2";
	pitch[2] = 131;
	spitch[2] = "C3";
	pitch[3] = 262;
	spitch[3] = "C4";
	pitch[4] = 523;
	spitch[4] = "C5";
	pitch[5] = 1047;
	spitch[5] = "C6";
	pitch[6] = 2093;
	spitch[6] = "C7";
	pitch[7] = 4186;
	spitch[7] = "C8";
	pitch[8] = 8372;
	spitch[8] = "C9";
	//set playing parameters to FALSE since we just started
	weplaying = FALSE;
	weopen = FALSE;
	// Parse command line for standard shell commands, DDE, file open
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();
	return TRUE;
}

//about box dialog class
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSpanformApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSpanformApp message handlers
//empty file in case new message still gets sent
void CSpanformApp::MyDummyNew(){}

//////////////////////////////'CREATE' HANDLER///////////////////////////////////////////
//start the create process going
void CSpanformApp::OnFileCreate()
{
	bool whatup;
	//first order of business is to create a cdobj object, then just let class finish it
	cdobj* mycreate = new cdobj();
	//if not out of memory, let cdobj make the file, otherwise show error and abort
	if (mycreate != NULL)
		whatup = mycreate->letsgo();
	else 
	{
		AfxMessageBox("Not enough memory to create a file! Command aborted.", MB_OK, 0);
		return;
	}
	//file should be created and closed now, time to delete pointer
	delete mycreate;
}

//used by splash screen component
BOOL CSpanformApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;
	return CWinApp::PreTranslateMessage(pMsg);
}

void CSpanformApp::OnFileOpen() 
{
	//string for open dialog	
	const char BASED_CODE onlyzed[] = "Spanform Analysis Files (*.zed)|*.zed||";
	//whether user canceled or not
	int huh = 0;
	//create the local CFileDialog Object
	CFileDialog* selekt = new CFileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, onlyzed, NULL);
	if (selekt==NULL)
	{
		AfxMessageBox("Out of memory. Command aborted.", MB_OK, 0);
		return;
	}
	//display the dialog
	huh = selekt->DoModal();
	if (huh==IDCANCEL)
	{
		//user canceled or error - abort
		delete selekt;
		return;
	}
	CString opener = selekt->GetFileName();
	CString ext = selekt->GetFileExt();
	if (ext != "zed")
	{
		AfxMessageBox("Invalid File Format. Please choose another file,");
		delete selekt;
		return;
	}
	OpenDocumentFile(opener);
	delete selekt;
}
