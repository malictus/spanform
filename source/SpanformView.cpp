//SpanformView.cpp
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#include "stdafx.h"
#include "Spanform.h"
#include "SpanformDoc.h"
#include "SpanformView.h"
#include "cinfo.h"
#include "Display.h"

//we need this to get pitch arrays
extern CSpanformApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////DEFINES////////////////////////////////////////////////////
/////////////////////////////////PLACEMENT DEFINES
//amount out from origin to start all drawing
#define d_frameh		12
#define d_framev		12
//amount to pad between horizontal start of color and start of text
#define d_pad1			4
//amount to pad between end of text and start of tick mark
#define d_pad2			5
//length of one 'tick' mark
#define d_tick			6
//amount down between start of rectangle color and start of bitmap
#define d_pad3			30
//amount between end of bitmap and end of drawing space (x dimension)
#define d_pad4			1
//amount in y dimension between tick and start of text
#define d_pad5			3
//amount in y dimension between bottom of text and end of drawing space
#define d_pad6			3
//amount past drawing area in x dimension to extend scrolling
#define d_pad7			10
//amount past drawing area in y dimension to extend scrolling
#define d_pad8			10
//length in x direction to back up before tick to write time text
#define d_backup		3
/////////////////////////////////COLOR DEFINES
//background color for screen window itself
#define d_framecolor	RGB(230,218,210)
//amount past end to stop background color
#define d_frameh2		25
#define d_framev2		34
/////////////////////////////////////drawing placement defines
//position for horizontal start of bitmap
#define d_bith			85
//position for vertical start of bitmap
#define d_bitv			25
//minimum distance between successive time marks
#define d_timespace		100
//same for pitch if using letterview
#define d_pitchspace	12
//vertical distance below tick to write seconds text
#define d_more			3
//distance before start of bitmap to start writing pitch text
#define d_pitchback		60
//distance as above with letters instead (not as far back)
#define d_pitchbacklett	32
//offset above actual tick to write pitch text
#define d_pitchoff		9
//amount of white space on right side for scroll
#define d_otherh		20
//amount of white space on bottom side for scroll
#define d_otherv		27
/////////////////////////////////////color defines
//background bitmap color - white for now
#define d_back			RGB(255, 255, 255)
//background for BW display
#define d_backBW		RGB(255, 255, 255)
//second down below black - purple
#define d_2color		RGB(255, 0, 255)
//third down - blue
#define d_3color		RGB(0, 0, 180)
//fourth down - yelllow
#define d_4color		RGB(225, 225, 0)
/////////////////////////////////////bytelist defines
//bytelist values above this will be in drawn in black
#define d_black			210
//second down value below black
#define d_second		180
//third down
#define d_third			150
//fourth down
#define d_fourth		120
/////////////////////////////////////other
//at max threshold, sounds below this level (out of 255) will be ignored
#define d_vol			10
/////////////////////////////////////////////////////////////////////////////
// CSpanformView

IMPLEMENT_DYNCREATE(CSpanformView, CScrollView)

BEGIN_MESSAGE_MAP(CSpanformView, CScrollView)
	//{{AFX_MSG_MAP(CSpanformView)
	ON_COMMAND(ID_SPANFORM_SHOWFILEINFO, OnSpanformShowfileinfo)
	ON_COMMAND(ID_SPANFORM_DISPLAY, OnSpanformDisplay)
	ON_COMMAND(ID_AUDIO_PLAY, OnAudioPlay)
	ON_COMMAND(ID_AUDIO_STOP, OnAudioStop)
	ON_COMMAND(ID_AUDIO_ASSOCIATE, OnAudioAssociate)
	ON_COMMAND(ID_AUDIO_RESTART, OnAudioRestart)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(MM_MCINOTIFY, CSpanformView::OnMciNotify)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	//handler for audio finished
	//timer
	//ON_MESSAGE(WM_TIMER, CSpanformView::OnTimey)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSpanformView constructor
CSpanformView::CSpanformView(){}
//destructor
CSpanformView::~CSpanformView()
{
	//delete bitmaps to free up room
	actbit.DeleteObject();
	mybit.DeleteObject();
	//audio cleanup (stop playing, close document)
	Audiocleanup();
}

BOOL CSpanformView::PreCreateWindow(CREATESTRUCT& cs)
{return CView::PreCreateWindow(cs);}

//OnInitialUpdate - DO THIS STUFF FIRST
void CSpanformView::OnInitialUpdate() 
{
	//parent call
	CScrollView::OnInitialUpdate();
	CClientDC myclint(this);
	//get doc info
	CSpanformDoc* pDoc = GetDocument();
	
	//check version number, load into view variable
	versionOK = pDoc->getversion();
	//if bad version number, abort whole thing
	if (versionOK != 1)
		AfxThrowArchiveException(CArchiveException::badIndex);
	timeyid = 0;
	//create original bitmap object
	mybit.CreateCompatibleBitmap(&myclint, pDoc->getnumframes(), pDoc->getframesize());
	//create actual bitmap to be drawn, THIS ONE WILL GET OVERWRITTEN IN RESTRETCH 
	actbit.CreateCompatibleBitmap(&myclint, pDoc->getnumframes(), pDoc->getframesize());
	//draw original bitmap
	redobit();
	//draw streched version to fit the screen size chosen
	restretch();
	//open audio file associated with this file, if possible
	Audioopen();
}

void CSpanformView::OnSpanformShowfileinfo() 
{
	CSpanformDoc* pDoc = GetDocument();
	cinfo fileinfo(pDoc);
	fileinfo.DoModal();
}

//alter display characteristics through dialog
void CSpanformView::OnSpanformDisplay() 
{
	BITMAP bm;
	double hratt;
	double vratt;
	//retrieve existing info to feed in to display
	CSpanformDoc* pDoc = GetDocument();
	//create display dialog
	CDisplay dis;
	//create temp vars for the ones which require bitmap to be redrawn
	BOOL tempclr = pDoc->docdobj.clr;
	int tempsens = pDoc->docdobj.sensitive;
	int tempnoisy = pDoc->docdobj.noisy;
	BOOL tempexpo = pDoc->docdobj.expo;
	//create temp vars in case result is too big
	int horror = pDoc->docdobj.horr;
	int vertor = pDoc->docdobj.vertt;
	//show dialog, using data from doc, change doc variables based on input
	if (dis.gogo(pDoc) == false)
	{
		//canceled, no doc info written
		return;
	}
	//if certain variables have changed, bitmap must be redrawn
	if ((tempexpo != pDoc->docdobj.expo) || (tempclr != pDoc->docdobj.clr) || (tempsens != pDoc->docdobj.sensitive) || (tempnoisy != pDoc->docdobj.noisy))
		redobit();
	//set doc flag to dirty
	pDoc->SetModifiedFlag();
	//redo scrolling area
	mybit.GetObject(sizeof(bm), &bm);
	hratt = bm.bmWidth*((double)(pDoc->docdobj.horr) / 10);
	vratt = bm.bmHeight*((double)(pDoc->docdobj.vertt) / 10);
	CSize spansize((int)hratt + d_bith + d_otherh + d_frameh2, (int)vratt + d_bitv + d_otherv + d_framev2);
	SetScrollSizes(MM_LOENGLISH, spansize);
	//restretch area, in case sizes have been changed
	restretch();
	//redraw entire area
	Invalidate();
	UpdateWindow();
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////BITMAP ROUTINES/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//redobit - draw or redraw original sized bitmap
void CSpanformView::redobit()
{
	CClientDC myclint(this);
	CDC mydc;
	double frac;
	double nois;
	int counter = 0;
	int counter2 = 0;
	int counter3 = 0;
	BYTE tr = 0;
	BYTE vol = 0;
	bool dont = false;
	double hratt;
	double vratt;
	int bottom;
	int lastone;
	int laster;

	//might take a while to load bitmap!
	CWaitCursor wait;
	//get doc info
	CSpanformDoc* pDoc = GetDocument();
	//if bad version number, abort bitmap creation
	if (versionOK != 1)
		return;
	//set scroll info
	BITMAP bm;
	mybit.GetObject(sizeof(bm), &bm);
	hratt = bm.bmWidth*((double)(pDoc->docdobj.horr) / 10);
	vratt = bm.bmHeight*((double)(pDoc->docdobj.vertt) / 10);
	CSize spansize((int)hratt + d_bith + d_otherh + d_frameh2, (int)vratt + d_bitv + d_otherv + d_framev2);
	SetScrollSizes(MM_LOENGLISH, spansize);
	//select bitmap
	mydc.CreateCompatibleDC(&myclint);
	mydc.SelectObject(&mybit);
	/////////////////////DRAW BITMAP
	//factor for sensitivity, frac is now a number between .1 and 1 
	frac = (((double)(pDoc->docdobj.sensitive))+1)/10;
	//reverse it - now lowest sensitivity is 1 highest is .1
	frac = 1.1 - frac;
	//factor for noise threshold; now it's between 0 and 1
	nois = ((double)(pDoc->docdobj.noisy)/9);
	bottom = pDoc->getframesize();
	//paint background color
	if (pDoc->docdobj.clr == TRUE)
		mydc.FillSolidRect(0, 0, pDoc->getnumframes(), pDoc->getframesize(), d_back);
	else
		mydc.FillSolidRect(0, 0, pDoc->getnumframes(), pDoc->getframesize(), d_backBW);
	
	if (pDoc->docdobj.expo == FALSE)
	//linear version
	{
		//fill in pixels where needed
		while (counter < pDoc->getnumframes())
		{
			//bytelist continues on until end, so only counter2 is reset here
			counter2 = 0;
			dont = false;
			vol = pDoc->docdobj.vollist[counter];
			if (vol < d_vol*nois)
			{
				//don't try to draw
				dont = true;
			}
			while (counter2 < pDoc->getframesize())
			{
				//get byte value for this pixel (between 0 and 255)
				tr = pDoc->docdobj.bytelist[counter3];
				if (dont == true)
					goto nexter;
				////////////DRAW A COLOR/////////////////////////////
				if (pDoc->docdobj.clr == FALSE)
				{
					if (tr > (d_fourth*frac))
						mydc.SetPixelV(counter, bottom-counter2, RGB(0,0,0));
				}
				else
				{
					//should speed it up...
					if (tr < (d_fourth*frac))
						goto nexter;
					if (tr > (d_black*frac))
					{
						//black
						mydc.SetPixelV(counter, bottom-counter2, RGB(0,0,0));
						goto nexter;
					}
					if ((tr > (d_second*frac)) && (tr <= (d_black*frac)))
					{
						//red
						mydc.SetPixelV(counter, bottom-counter2, d_2color);
						goto nexter;
					}
					if ((tr > (d_third*frac)) && (tr <= (d_second*frac)))
					{
						//green
						mydc.SetPixelV(counter, bottom-counter2, d_3color);
						goto nexter;
					}
					//yellow
					mydc.SetPixelV(counter, bottom-counter2, d_4color);
				}
nexter:
				counter2 = counter2 + 1;
				counter3 = counter3 + 1;
			}
		counter = counter + 1;
		}
	}
	else
	//exponential version
	{
		double numocts;
		double otherocts;
		double timey;
		int countcount;
		numocts = log((double)pDoc->docdobj.gethifreq())/log(2);
		otherocts = log ((double)pDoc->getsamprate()/pDoc->getpower2())/log(2);
		numocts = numocts - otherocts;
		//create an array to hold the Hz data for each pixel
		CArray<int, int&> freqlist;
		CArray<int, int&> linlist;
		CArray<int, int&> countme;
		freqlist.SetSize(pDoc->getframesize());
		linlist.SetSize(pDoc->getframesize());
		countme.SetSize(pDoc->getframesize());
		int freqcounter = 0;
		while (freqcounter < (pDoc->getframesize()))
		{
			timey = ((double)freqcounter / (double)pDoc->getframesize());
			timey = numocts * timey;
			timey = ((double)pDoc->getsamprate()/pDoc->getpower2()) * (pow(2, timey));
			freqlist[freqcounter] = (int)timey;
			freqcounter = freqcounter + 1;
		}
		//create an array to hold where the each bytelist data belongs (linear)
		freqcounter = 0;
		while (freqcounter < (pDoc->getframesize()))
		{
			timey = (double)(pDoc->getsamprate()*((double)(freqcounter)+1)/(double)pDoc->getpower2());
			linlist[freqcounter] = (int)timey;
			freqcounter = freqcounter + 1;
		}
		//create the cross reference array
		freqcounter = 0;
		while (freqcounter < (pDoc->getframesize()))
		{
			countcount = 0;
			while (linlist[freqcounter] > freqlist[countcount])
			{
				countcount = countcount + 1;
				if (countcount >= (pDoc->getframesize()-1))
					goto stopme;
			}
stopme:
		countme[freqcounter] = countcount;
		freqcounter = freqcounter+1;
		}
		//big drawing loop
		counter = 0;
		while (counter < pDoc->getnumframes())
		{
			//bytelist continues on until end, so only counter3 is not reset here
			counter2 = 0;
			dont = false;
			vol = pDoc->docdobj.vollist[counter];
			if (vol < d_vol*nois)
			{
				//don't try to draw
				dont = true;
			}
			//read in bytelist for frame and correlate to Hertz
			while (counter2 < pDoc->getframesize())
			{
				//get byte value for this pixel (between 0 and 255)
				tr = pDoc->docdobj.bytelist[counter3];
				if (dont == true)
					goto pexter;
				
				////////////DRAW A COLOR/////////////////////////////
				if (pDoc->docdobj.clr == FALSE)
				{
					if (tr > (d_fourth*frac))
					{
						if (counter2 == 0)
							mydc.SetPixelV(counter, bottom-countme[counter2], RGB(0,0,0));
						else
						{
							lastone = countme[counter2]-countme[(counter2)-1];
							if (lastone == 0)
								lastone = 1;
							laster = 0;
							while (laster < lastone)
							{
								mydc.SetPixelV(counter, bottom-countme[counter2]+laster, RGB(0,0,0));
								laster = laster + 1;
							}
						}
					}
				}
				else
				{
					//should speed it up...
					if (tr < (d_fourth*frac))
						goto pexter;
					if (tr > (d_black*frac))
					{
						if (counter2 == 0)
							mydc.SetPixelV(counter, bottom-countme[counter2], RGB(0,0,0));
						else
						{
							lastone = countme[counter2]-countme[(counter2)-1];
							if (lastone == 0)
								lastone = 1;
							laster = 0;
							while (laster < lastone)
							{
								mydc.SetPixelV(counter, bottom-countme[counter2]+laster, RGB(0,0,0));
								laster = laster + 1;
							}
						}
						goto pexter;
					}
					if ((tr > (d_second*frac)) && (tr <= (d_black*frac)))
					{
						if (counter2 == 0)
							mydc.SetPixelV(counter, bottom-countme[counter2], d_2color);
						else
						{
							lastone = countme[counter2]-countme[(counter2)-1];
							if (lastone == 0)
								lastone = 1;
							laster = 0;
							while (laster < lastone)
							{
								mydc.SetPixelV(counter, bottom-countme[counter2]+laster, d_2color);
								laster = laster + 1;
							}
						}
						goto pexter;
					}
					if ((tr > (d_third*frac)) && (tr <= (d_second*frac)))
					{
						if (counter2 == 0)
							mydc.SetPixelV(counter, bottom-countme[counter2], d_3color);
						else
						{
							lastone = countme[counter2]-countme[(counter2)-1];
							if (lastone == 0)
								lastone = 1;
							laster = 0;
							while (laster < lastone)
							{
								mydc.SetPixelV(counter, bottom-countme[counter2]+laster, d_3color);
								laster = laster + 1;
							}
						}
						goto pexter;
					}
					//yellow
					{
						if (counter2 == 0)
							mydc.SetPixelV(counter, bottom-countme[counter2], d_4color);
						else
						{
							lastone = countme[counter2]-countme[(counter2)-1];
							if (lastone == 0)
								lastone = 1;
							laster = 0;
							while (laster < lastone)
							{
								mydc.SetPixelV(counter, bottom-countme[counter2]+laster, d_4color);
								laster = laster + 1;
							}
						}
					}
				}
pexter:
				counter2 = counter2 + 1;
				counter3 = counter3 + 1;
			}
		counter = counter + 1;
		}
	}
	mydc.DeleteDC();
}


//restretch - redraw actual stretched bitmap that will appear on screen
void CSpanformView::restretch()
{
	double hratt;
	double vratt;
	CDC mycdc;
	CDC srcdc;
	BITMAP bm;
	BITMAP srcbm;
	CClientDC myclint(this);
	
	//might take a while for really big ones
	CWaitCursor wait;
	//get doc info
	CSpanformDoc* pDoc = GetDocument();
	//get sizes for stretched version of bitmap
	mybit.GetObject(sizeof(bm), &bm);
	hratt = bm.bmWidth*((double)(pDoc->docdobj.horr) / 10);
	vratt = bm.bmHeight*((double)(pDoc->docdobj.vertt) / 10);
	//actual drawn bitmap may change sizes, so delete old one first...
	actbit.DeleteObject();
	//now create new one
	if ((actbit.CreateCompatibleBitmap(&myclint, (int)(hratt), (int)(vratt))) == 0)
		AfxMessageBox("ERROR!");
	//get ready to draw the transfer
	mycdc.CreateCompatibleDC(NULL);
	mycdc.SelectObject(&actbit);
	actbit.GetObject(sizeof(bm), &bm);
	srcdc.CreateCompatibleDC(NULL);
	srcdc.SelectObject(&mybit);
	mybit.GetObject(sizeof(srcbm), &srcbm);
	if ((mycdc.StretchBlt(0,0,bm.bmWidth,bm.bmHeight,&srcdc,0,0,srcbm.bmWidth,srcbm.bmHeight,SRCCOPY))==0)
		AfxMessageBox("Error creating bitmap! File may be too large to be shown on this computer.");
	//release memory
	mycdc.DeleteDC();
	srcdc.DeleteDC();
}

////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////SCREEN DRAWING ROUTINE////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//actual drawing routine for screen drawing
void CSpanformView::OnDraw(CDC* pDC)
{
	//vars
	CSpanformDoc* pDoc;
	CDC mycdc;
	BITMAP otherbm;
	CString max;
	CSize maxs;
	CSize maxtime;
	CPoint topleft;
	CPoint bitstart;
	CPoint bottright;
	CSize scroller;
	CRect* prect;
	double ratio;
	long counter;
	double timey;
	long othercount;
	int thirdcount;
	long present;
	long longtime;
	char stringer[50];
	CString hor = "ERROR";
	unsigned long curpos;

	////////////////////////SETUP/////////////////////////////////////
	
	//get document info
	pDoc = GetDocument();
	//set up bitmap; mycdc must be deleted at end of routine
	mycdc.CreateCompatibleDC(NULL);
	//draw the stretched bitmap into cdc
	mycdc.SelectObject(&actbit);
	//read size info into otherbm object
	actbit.GetObject(sizeof(otherbm), &otherbm);
	//set map mode for screen drawing
	pDC->SetMapMode(MM_TEXT);
	//select screen font
	pDC->SelectStockObject(ANSI_VAR_FONT);
	//select text drawing mode
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_LEFT | TA_TOP);
	//max set to longest freq display possible in this mode
	if (pDoc->docdobj.lett == FALSE)
		max = "22222Hz";
	else
		max = "C00s";
	//we'll use maxs to align certain things in the view
	 maxs = pDC->GetTextExtent(max);
	 //now set max to maximum time
	max = "00m00s";
	maxtime = pDC->GetTextExtent(max);
	//setup drawing points; first the top&leftmost point for all drawing
	topleft.x = d_frameh;
	topleft.y = d_framev;
	//next, the point to start drawing of bitmap, computed from text data
	bitstart.x = d_frameh+d_pad1+maxs.cx+d_pad2+d_tick;
	bitstart.y = d_framev+d_pad3;
	//globals for mouse movements
	startme = bitstart.x;
	starme = bitstart.x + otherbm.bmWidth;
	//next, the bottom&rightmost point for all drawing
	bottright.x = bitstart.x + otherbm.bmWidth + d_pad4+maxtime.cx;
	bottright.y = bitstart.y + otherbm.bmHeight + d_tick + d_pad5 + maxs.cy + d_pad6;
	//next, set scroll sizes
	CSize spansize(bottright.x+d_pad7, bottright.y+d_pad8);
	SetScrollSizes(MM_TEXT, spansize);

	////////////////////////ACTUAL DRAWING///////////////////////////
	//draw a 'frame' color for the whole pic
	prect = new CRect(topleft, bottright);
	pDC->FillSolidRect(prect, d_framecolor);
	delete prect;
	//draw a (black) outer rectangle around the whole thing
	pDC->MoveTo(topleft);
	pDC->LineTo(bottright.x, topleft.y);
	pDC->LineTo(bottright);
	pDC->LineTo(topleft.x, bottright.y);
	pDC->LineTo(topleft);
	//draw a black rectangle around just the bitmap
	pDC->MoveTo(bitstart.x-1, bitstart.y-1);
	pDC->LineTo(bitstart.x-1, bitstart.y+otherbm.bmHeight+1);
	pDC->LineTo(bitstart.x+otherbm.bmWidth+1, bitstart.y+otherbm.bmHeight+1);
	pDC->LineTo(bitstart.x+otherbm.bmWidth+1, bitstart.y-1);
	pDC->LineTo(bitstart.x-1, bitstart.y-1);
	//draw bitmap
	pDC->BitBlt(bitstart.x, bitstart.y, otherbm.bmWidth+bitstart.x, otherbm.bmHeight+bitstart.y, &mycdc, 0, 0, SRCCOPY);
	
	/////////////////////////draw horizontal (time) graph
	ratio = (double)pDoc->getnumframes()/(double)otherbm.bmWidth;
	counter = 0;
	othercount = 0;
	present = 0;
	while (counter < otherbm.bmWidth)
	{
		//timey calculated to seconds at current position
		timey = ((double)(counter*pDoc->getslider())/(double)pDoc->getsamprate())*ratio;
		longtime = (long)timey;
		//if new second reached and at least maxtime.cx away...
		if ((longtime > present) && (othercount > maxtime.cx))
		{
			//draw vertical 'tick'
			pDC->MoveTo(bitstart.x+counter, bitstart.y+otherbm.bmHeight);
			pDC->LineTo(bitstart.x+counter, bitstart.y+otherbm.bmHeight+d_tick);
			//find time at this point, draw it
			long minute = longtime/60;
			long sects = longtime - (minute*60);
			if (minute == 0)
			{
				_ltoa(longtime, stringer, 10);
				hor = stringer;
				hor = hor + "s";
			}
			else
			{
				_ltoa(minute, stringer, 10);
				hor = stringer;
				hor = hor + "m";
				_ltoa(sects, stringer, 10);
				hor = hor + stringer;
				hor = hor + "s";
			}
			pDC->TextOut(bitstart.x+counter-d_backup, bitstart.y+otherbm.bmHeight+d_tick+d_pad5, hor);
			present = longtime;
			othercount = 0;
		}
		else
		{
			//necessary to work properly
			if (longtime > present)
				present = longtime;
		}
		counter = counter + 1;
		othercount = othercount + 1;
	}
	
	////////////////////////////////draw vertical (pitch) graph
	//reset text drawing alignment
	pDC->SetTextAlign(TA_RIGHT);
	//set start values
	ratio = (double)pDoc->getframesize()/(double)otherbm.bmHeight;
	counter = 0;
	thirdcount = 0;
	double numocts;
	double otherocts;
	if (pDoc->docdobj.lett == FALSE)
	{
		//print actual frequency version
		while (counter < otherbm.bmHeight)
		{
			//pitchcalc, expo version
			if (pDoc->docdobj.expo == TRUE)
			{
				//longtime now computed to display pitch info
				numocts = log((double)pDoc->docdobj.gethifreq())/log(2);
				otherocts = log ((double)pDoc->getsamprate()/pDoc->getpower2())/log(2);
				numocts = numocts - otherocts;
				timey = (double)counter / (double)otherbm.bmHeight;
				timey = numocts * timey;
				timey = ((double)pDoc->getsamprate()/pDoc->getpower2()) * (pow(2, timey));
				longtime = (long)timey;
			}
			else
			//linear version
			{
				//longtime now computed to display pitch info
				timey = (double)(pDoc->getsamprate()*((double)(counter*ratio)+1)/(double)pDoc->getpower2());
				longtime = (long)timey;
			}
			//draw horizontal 'tick'
			pDC->MoveTo(bitstart.x-1, bitstart.y+otherbm.bmHeight-counter);
			pDC->LineTo(bitstart.x-1-d_tick, bitstart.y+otherbm.bmHeight-counter);
			//find pitch at this point, draw it
			_ltoa(longtime, stringer, 10);
			hor = stringer;
			hor= hor + "Hz";
			pDC->TextOut(d_frameh+d_pad1+maxs.cx, bitstart.y+otherbm.bmHeight-counter-(maxs.cy/2), hor);
			//allow enough separation between them to look good
			counter = counter + (maxs.cy*3);
		}
	}
	else
	{
		//letter names version
		//force it to print the first time
		othercount = maxs.cy + 4;
		//cycle until we reach end of bitmap, or C10, whichever comes first
		while ((counter < otherbm.bmHeight) && (thirdcount < 9))
		{
			//pitchcalc, expo version
			if (pDoc->docdobj.expo == TRUE)
			{
				//longtime now computed to display pitch info
				numocts = log((double)pDoc->docdobj.gethifreq())/log(2);
				otherocts = log ((double)pDoc->getsamprate()/pDoc->getpower2())/log(2);
				numocts = numocts - otherocts;
				timey = (double)counter / (double)otherbm.bmHeight;
				timey = numocts * timey;
				timey = ((double)pDoc->getsamprate()/pDoc->getpower2()) * (pow(2, timey));
				longtime = (long)timey;
			}
			else
			//linear version
			{
				//longtime now computed to display pitch info
				timey = (double)(pDoc->getsamprate()*((double)(counter*ratio)+1)/(double)pDoc->getpower2());
				longtime = (long)timey;
			}
			if ((longtime > theApp.pitch[thirdcount]) && (othercount > (maxs.cy+3)))
			{
				//draw horizontal 'tick'
				pDC->MoveTo(bitstart.x-1, bitstart.y+otherbm.bmHeight-counter);
				pDC->LineTo(bitstart.x-1-d_tick, bitstart.y+otherbm.bmHeight-counter);
				//draw pitch string for here
				pDC->TextOut(d_frameh+d_pad1+maxs.cx, bitstart.y+otherbm.bmHeight-counter-(maxs.cy/2), theApp.spitch[thirdcount]);
				//cleanup
				othercount = 0;
				thirdcount = thirdcount + 1;
			}
			else
			{
				//necessary to work properly
				if (longtime > theApp.pitch[thirdcount])
				{
					thirdcount = thirdcount + 1;
				}
			}
			counter = counter + 1;
			othercount = othercount + 1;
		}
	}
	//draw cursor for audio
	if (audioopen == TRUE)
	{
		MCI_STATUS_PARMS sett;
		sett.dwItem = MCI_STATUS_POSITION;
		mciSendCommand(openParams.wDeviceID, MCI_STATUS, MCI_WAIT|MCI_STATUS_ITEM|MCI_STATUS_POSITION, (DWORD)(LPMCI_STATUS_PARMS)&sett);
		curpos = sett.dwReturn;
		//curpos now represents place in file in milliseconds
		//how many milliseconds is the last frame of file?
		ratio = (double)pDoc->getnumframes()/(double)otherbm.bmWidth;
		double lasty = ((double)(otherbm.bmWidth*pDoc->getslider())/(double)pDoc->getsamprate())*ratio;
		lasty = lasty*1000;
		unsigned long lasttime = (unsigned long)(lasty);
		//how far over should we draw?
		double curratio = (double)curpos/(double)lasttime;
		//adjust incase it goes past end of analysis file (if analysis file was stopped)
		if (curratio > 1)
			curratio = 1;
		curratio = otherbm.bmWidth*curratio;
		pDC->MoveTo(bitstart.x+(int)(curratio), bitstart.y-2);
		pDC->LineTo(bitstart.x+(int)(curratio), bitstart.y-23);
		pDC->MoveTo(bitstart.x+1+(int)(curratio), bitstart.y-2);
		pDC->LineTo(bitstart.x+1+(int)(curratio), bitstart.y-23);
		// set rect for invalidation
		ret.bottom = bitstart.y-1;
		ret.top = bitstart.y-23;
		ret.left = 0;
		ret.right = bitstart.x+1+otherbm.bmWidth+3;
		//added in 0.80; adjust to make it work
		CClientDC aDC(this);
		OnPrepareDC(&aDC);
		aDC.LPtoDP(&ret);
	}
	//cleanup
	mycdc.DeleteDC();
}

////
////////////////////////////////PRINTING///////////////////////////////////////////////
////

//////////////////////////////VARIABLES FOR PRINTING!//////////////////////////////////

//GLOBALS
//	pDC			-	DC for printer
//	pInfo		-	print information struct
//	maxfreq		-	maximum space required for frequency text, in inch 100ths
//	maxtime		-	maximum space required for time text, in inch 100ths
//	pbitstart	-	upper-right point of start of bitmap printing, in inch 100ths
//	horzi		-	space for bitmap window per page, x dimension, in inch 100ths
//	verzi		-	space for bitmap window per page, y dimension, in inch 100ths
//	xer			-	portion of bitmap to be printed on each page, x dimension
//	yer			-	portion of bitmap to be printed on each page, y dimension

//LOCALS - On Begin Printing
//	pDoc		-	Document Pointer
//	otherbm		-   BITMAP used to store size information for actbit
//	eachpage	-	amount of non-bitmap space needed on each page. in inch 100ths
//	lefri		-	used for internal setup of pages
//	topbot		-	used for internal setup of pages

//LOCALS - On Print
//	pDoc		-	Document Pointer
//	prick		-	Rectangle to draw portion of bitmap into
//	counterx	-	used to calculate which portion of bitmap to read for each page
//	countery	-	used to calculate which portion of bitmap to read for each page
//	xpos		-	used to calculate which portion of bitmap to read for each page
//	ypos		-	used to calculate which portion of bitmap to read for each page
//	mycdc		-	used to transfer bitmap

//prepare printing - normal call since number of pages required is not yet known
BOOL CSpanformView::OnPreparePrinting(CPrintInfo* pInfo)
{return DoPreparePrinting(pInfo);}

//prep for printing, count number of pages, etc
void CSpanformView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CSpanformDoc* pDoc;
	BITMAP otherbm;
	CSize eachpage;
	float lefri;
	float topbot;
	
	//get doc info
	pDoc = GetDocument();
	//select default printer font
	pDC->SelectStockObject(DEVICE_DEFAULT_FONT);
	//select text drawing mode
	pDC->SetTextAlign(TA_LEFT | TA_TOP);
	pDC->SetMapMode(MM_TEXT);
	//compute longest possible length for text
	if (pDoc->docdobj.lett == FALSE)
		maxfreq = pDC->GetTextExtent("22222Hz");
	else
		maxfreq = pDC->GetTextExtent("C00s");
	//now compute maximum time length
	maxtime = pDC->GetTextExtent("00m00s");
	//convert computed units
	pDC->SetMapMode(MM_LOENGLISH);
	pDC->DPtoLP(&maxfreq);
	pDC->DPtoLP(&maxtime);
	//set point of start of bitmap
	pbitstart.x = maxfreq.cx+d_pad2+d_tick;
	pbitstart.y = d_pad3;
	//calculate the non-bitmap portion of each page to be used
	eachpage.cx = pbitstart.x + d_pad4 + maxtime.cx;
	eachpage.cy = pbitstart.y + d_tick + d_pad5 + maxtime.cy + d_pad6;
	//get the resolution of the printer
	lefri = (float)pDC->GetDeviceCaps(HORZRES);
	topbot = (float)pDC->GetDeviceCaps(VERTRES);
	lefri = lefri/(pDC->GetDeviceCaps(LOGPIXELSX));
	topbot = topbot/(pDC->GetDeviceCaps(LOGPIXELSY));
	lefri = lefri*100;
	topbot = topbot*100;
	lefri = lefri - eachpage.cx;
	topbot = topbot - eachpage.cy;
	//read values into class vars
	horzi = (int)lefri;
	verzi = (int)topbot;
	//if paper size is too small to print at all, abort
	if ((horzi < 1) || (verzi < 1))
	{
		AfxMessageBox("Printing Aborted. Adjust paper size and try again.");
		pInfo->SetMaxPage(0);
		return;
	}
	//read size info for bitmap into otherbm object
	actbit.GetObject(sizeof(otherbm), &otherbm);
	//calculate total number of pages needed to print entire bitmap
	if ((otherbm.bmWidth % horzi) == 0)
		pagex = (int)(otherbm.bmWidth/horzi);
	else
		pagex = (int)(otherbm.bmWidth/horzi) + 1;
	if ((otherbm.bmHeight % verzi) == 0)
		pagey = (int)(otherbm.bmHeight/verzi);
	else
		pagey = (int)(otherbm.bmHeight/verzi) + 1;
	//set number of pages to be printed
	pInfo->SetMaxPage(pagex*pagey);
	//set portion of bitmap to be printed on each page
	xer = (otherbm.bmWidth/pagex);
	yer = (otherbm.bmHeight/pagey);
}

//print routine for each page
void CSpanformView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	double ratio;
	long counter;
	double timey;
	long othercount;
	int thirdcount;
	long present;
	long longtime;
	char stringer[50];
	CString hor = "ERROR";
	CSpanformDoc* pDoc;
	int countery;
	int counterx;
	int xpos;
	int ypos;
	CDC mycdc;
	BITMAP otherbm;

	////////////////////////SETUP/////////////////////////////////////
	//get document info
	pDoc = GetDocument();
	//set printer options
	pDC->SelectStockObject(DEVICE_DEFAULT_FONT);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_LEFT | TA_TOP);
	pDC->SetMapMode(MM_LOENGLISH);
	//setup portion of bitmap to read
	if ((pInfo->m_nCurPage % pagex) == 0)
	{
		countery = (int)(pInfo->m_nCurPage / pagex);
		counterx = (int)(pInfo->m_nCurPage / countery);
	}
	else
	{
		countery = (int)(pInfo->m_nCurPage / pagex)+1;
		counterx = (pInfo->m_nCurPage % pagex);
	}
	xpos = xer*(counterx-1);
	ypos = yer*(countery-1);
	//setup rectangle to draw bitmap into
	CRect prick(pbitstart.x, pbitstart.y, pbitstart.x+xer, pbitstart.y+yer);
	pDC->LPtoDP(&prick);
	//set up bitmap; mycdc must be deleted later!
	mycdc.CreateCompatibleDC(NULL);
	//draw the bitmap into cdc
	mycdc.SelectObject(&actbit);
	//print bitmap!
	pDC->SetMapMode(MM_TEXT);
	pDC->StretchBlt(prick.left, -(prick.top), prick.Width(), -(prick.Height()), &mycdc, xpos, ypos, xer, yer, SRCCOPY);
	pDC->SetMapMode(MM_LOENGLISH);
	//delete temp DC
	mycdc.DeleteDC();
	//print a black rectangle around just the bitmap
	pDC->MoveTo(pbitstart.x, -pbitstart.y);
	pDC->LineTo(pbitstart.x, -(pbitstart.y+yer));
	pDC->LineTo(pbitstart.x+xer, -(pbitstart.y+yer));
	pDC->LineTo(pbitstart.x+xer, -pbitstart.y);
	pDC->LineTo(pbitstart.x, -pbitstart.y);
	/////////////////////////draw horizontal (time) graph
	actbit.GetObject(sizeof(otherbm), &otherbm);
	ratio = (double)pDoc->getnumframes()/(double)otherbm.bmWidth;
	counter = 0;
	othercount = 0;
	present = 0;
	while (counter < xer)
	{
		//timey calculated to seconds at current position
		timey = ((double)(counter*pDoc->getslider())/(double)pDoc->getsamprate())*ratio;
		//add amount from x position
		double mor = (counterx-1)*((double)(xer*pDoc->getslider())/(double)pDoc->getsamprate())*ratio;
		timey = timey + mor;
		longtime = (long)timey;
		//if new second reached and at least maxtime.cx away...
		if ((longtime > present) && (othercount > maxtime.cx))
		{
			//draw vertical 'tick'
			pDC->MoveTo(pbitstart.x+counter, -(pbitstart.y+yer));
			pDC->LineTo(pbitstart.x+counter, -(pbitstart.y+yer+d_tick));
			//find time at this point, draw it
			long minute = longtime/60;
			long sects = longtime - (minute*60);
			if (minute == 0)
			{
				_ltoa(longtime, stringer, 10);
				hor = stringer;
				hor = hor + "s";
			}
			else
			{
				_ltoa(minute, stringer, 10);
				hor = stringer;
				hor = hor + "m";
				_ltoa(sects, stringer, 10);
				hor = hor + stringer;
				hor = hor + "s";
			}
			CPoint fixit(pbitstart.x+counter-d_backup, -(pbitstart.y+yer+d_tick+d_pad5));
			pDC->LPtoDP(&fixit);
			pDC->SetMapMode(MM_TEXT);
			pDC->TextOut(fixit.x, fixit.y, hor);
			pDC->SetMapMode(MM_LOENGLISH);
			present = longtime;
			othercount = 0;
		}
		else
		{
			//necessary to work properly
			if (longtime > present)
				present = longtime;
		}
		counter = counter + 1;
		othercount = othercount + 1;
	}
	
	////////////////////////////////draw vertical (pitch) graph
	//reset text drawing alignment
	pDC->SetTextAlign(TA_RIGHT);
	//set start values
	ratio = (double)pDoc->getframesize()/(double)otherbm.bmHeight;
	counter = 0;
	thirdcount = 0;
	double numocts;
	double otherocts;
	if (pDoc->docdobj.lett == FALSE)
	{
		//print actual frequency version
		while (counter < yer)
		{
			if (pDoc->docdobj.expo == TRUE)
			{
				//longtime now computed to display pitch info
				numocts = log((double)pDoc->docdobj.gethifreq())/log(2);
				otherocts = log ((double)pDoc->getsamprate()/pDoc->getpower2())/log(2);
				numocts = numocts - otherocts;
				timey = ((double)((pagey-countery)*(otherbm.bmHeight/pagey))/(double)otherbm.bmHeight);
				timey = timey + ((double)counter/otherbm.bmHeight);
				timey = numocts * timey;
				timey = ((double)pDoc->getsamprate()/pDoc->getpower2()) * (pow(2, timey));
				longtime = (long)timey;
			}
			else
			{
				//longtime now computed to display pitch info
				timey = (double)(pDoc->getsamprate()*((double)(counter*ratio)+1)/(double)pDoc->getpower2());
				double mor = (pagey-countery)*((double)(pDoc->getsamprate()*((double)(yer*ratio)+1)/(double)pDoc->getpower2()));
				timey = timey + mor;
				longtime = (long)timey;
			}
			//draw horizontal 'tick'
			pDC->MoveTo(pbitstart.x-1, -(pbitstart.y+yer-counter));
			pDC->LineTo(pbitstart.x-1-d_tick, -(pbitstart.y+yer-counter));
			//find pitch at this point, draw it
			_ltoa(longtime, stringer, 10);
			hor = stringer;
			hor= hor + "Hz";
			CPoint fixit(pbitstart.x-2-d_tick, -(pbitstart.y+yer-counter-(maxfreq.cy/2)));
			pDC->LPtoDP(&fixit);
			pDC->SetMapMode(MM_TEXT);
			pDC->TextOut(fixit.x, fixit.y, hor);
			pDC->SetMapMode(MM_LOENGLISH);
			//allow enough separation between them to look good
			counter = counter + (maxfreq.cy*3);
		}
	}
	else
	{
		//letter names version
		//force it to print the first time
		othercount = maxtime.cy + 3;
		if (pDoc->docdobj.expo == TRUE)
		{
			//longtime now computed to display pitch info
			numocts = log((double)pDoc->docdobj.gethifreq())/log(2);
			otherocts = log ((double)pDoc->getsamprate()/pDoc->getpower2())/log(2);
			numocts = numocts - otherocts;
			timey = ((double)((pagey-countery)*(otherbm.bmHeight/pagey))/(double)otherbm.bmHeight);
			timey = timey + ((double)counter/otherbm.bmHeight);
			timey = numocts * timey;
			timey = ((double)pDoc->getsamprate()/pDoc->getpower2()) * (pow(2, timey));
			longtime = (long)timey;
		}
		else
		{
			//necessary to make in print on multiple pages correctly
			timey = (double)(pDoc->getsamprate()*((double)(counter*ratio)+1)/(double)pDoc->getpower2());
			double mor = (pagey-countery)*((double)(pDoc->getsamprate()*((double)(yer*ratio)+1)/(double)pDoc->getpower2()));
			timey = timey + mor;
			longtime = (long)timey;
		}
		while (longtime > theApp.pitch[thirdcount+1])
		{
			thirdcount = thirdcount + 1;
		}
		//cycle until we reach end of bitmap, or C10, whichever comes first
		while ((counter < yer) && (thirdcount < 9))
		{
			if (pDoc->docdobj.expo == TRUE)
			{
				//longtime now computed to display pitch info
				numocts = log((double)pDoc->docdobj.gethifreq())/log(2);
				otherocts = log ((double)pDoc->getsamprate()/pDoc->getpower2())/log(2);
				numocts = numocts - otherocts;
				timey = ((double)((pagey-countery)*(otherbm.bmHeight/pagey))/(double)otherbm.bmHeight);
				timey = timey + ((double)counter/otherbm.bmHeight);
				timey = numocts * timey;
				timey = ((double)pDoc->getsamprate()/pDoc->getpower2()) * (pow(2, timey));
				longtime = (long)timey;
			}
			else
			{
				//necessary to make in print on multiple pages correctly
				timey = (double)(pDoc->getsamprate()*((double)(counter*ratio)+1)/(double)pDoc->getpower2());
				double mor = (pagey-countery)*((double)(pDoc->getsamprate()*((double)(yer*ratio)+1)/(double)pDoc->getpower2()));
				timey = timey + mor;
				longtime = (long)timey;
			}
			if ((longtime > theApp.pitch[thirdcount]) && (othercount > (maxtime.cy+3)))
			{
				//draw horizontal 'tick'
				pDC->MoveTo(pbitstart.x-1, -(pbitstart.y+yer-counter));
				pDC->LineTo(pbitstart.x-1-d_tick, -(pbitstart.y+yer-counter));
				//draw pitch string for here
				CPoint fixit(pbitstart.x-2-d_tick, -(pbitstart.y+yer-counter-(maxfreq.cy/2)));
				pDC->LPtoDP(&fixit);
				pDC->SetMapMode(MM_TEXT);
				pDC->TextOut(fixit.x, fixit.y, theApp.spitch[thirdcount]);
				pDC->SetMapMode(MM_LOENGLISH);
				//cleanup
				othercount = 0;
				thirdcount = thirdcount + 1;
			}
			else
			{
				//necessary to work properly
				if (longtime > theApp.pitch[thirdcount])
				{
					thirdcount = thirdcount + 1;
				}
			}
			counter = counter + 1;
			othercount = othercount + 1;
		}
	}
}

void CSpanformView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	//nothing to add here right now...
}

/////////////////////////////////////////////////////////////////////////////
// CSpanformView diagnostics

#ifdef _DEBUG
void CSpanformView::AssertValid() const
{
	CView::AssertValid();
}

void CSpanformView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSpanformDoc* CSpanformView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSpanformDoc)));
	return (CSpanformDoc*)m_pDocument;
}
#endif //_DEBUG

////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////AUDIO FUNCTIONS///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////



//open the audio file associated with this analysis file
void CSpanformView::Audioopen()
{
	if (theApp.weopen == TRUE)
	{
		AfxMessageBox("Another file is already open, so audio features will be disabled for this file.");
		return;
	}
	CSpanformDoc* pDoc = GetDocument();
	//open up the audio file associated with this analysis file
	openParams.lpstrDeviceType = (LPCSTR)MCI_DEVTYPE_WAVEFORM_AUDIO;
	char fillme[225];
	GetShortPathName(pDoc->getpath(), fillme, 225);
	openParams.lpstrElementName = fillme;
	if (mciSendCommand(0, MCI_OPEN, MCI_WAIT|MCI_OPEN_ELEMENT|MCI_OPEN_TYPE|MCI_OPEN_TYPE_ID, (DWORD)(LPVOID)&openParams) == 0)
	{
		audioopen=TRUE;
		theApp.weopen=TRUE;
		//set time to milliseconds mode
		MCI_WAVE_SET_PARMS sm;
		mciSendCommand(openParams.wDeviceID, MCI_SET, MCI_WAIT|MCI_SET_TIME_FORMAT|MCI_FORMAT_MILLISECONDS, (DWORD)(LPVOID)&sm);
	}
	else
	{
		//error, file wasn't opened
		AfxMessageBox("Error opening sound file associated with this file. Audio features will be disabled.");
		audioopen=FALSE;
	}
}

//cleanup actions (close audio file) when a file is closed
void CSpanformView::Audiocleanup()
{
	//stop audio file if it was playing
	if ((audioopen == TRUE) && (theApp.weplaying == TRUE))
	{
		OnAudioStop();
	}
	//close audio file if it was open
	if (audioopen == TRUE)
	{
		MCI_GENERIC_PARMS genparms;
		mciSendCommand(openParams.wDeviceID, MCI_CLOSE, MCI_WAIT, (DWORD)(LPVOID)&genparms);
		theApp.weplaying = FALSE;
		theApp.weopen = FALSE;
		audioopen = FALSE;
	}
}

//play audio file
void CSpanformView::OnAudioPlay() 
{
	if ((theApp.weplaying == FALSE)&&(audioopen == TRUE))
	{
		CSpanformDoc* pDoc = GetDocument();
		MCI_PLAY_PARMS parms;
		parms.dwCallback = MAKELONG(m_hWnd, 0);
		if (mciSendCommand(openParams.wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&parms)==0)
		{
			theApp.weplaying = TRUE;
			//we're playing sound now, start the timer
			timeyid = SetTimer(1, 100, NULL);
		}
		else
		{
			AfxMessageBox("Error playing wave file.");
		}
	}
}

void CSpanformView::OnAudioStop() 
{
	if ((theApp.weplaying == TRUE)&&(audioopen == TRUE))
	{
		KillTimer(timeyid);
		MCI_GENERIC_PARMS genparms;
		mciSendCommand(openParams.wDeviceID, MCI_STOP, MCI_WAIT, (DWORD)(LPVOID)&genparms);
		theApp.weplaying = FALSE;
		InvalidateRect(ret, FALSE);
		UpdateWindow();
	}
}

void CSpanformView::OnAudioAssociate() 
{
	CSpanformDoc* pDoc = GetDocument();
	//string for open dialog
	const char BASED_CODE onlywav[] = "WAV Files (*.wav)|*.wav||";	
	//whether user canceled or not
	int huh = 0;
	//create the local CFileDialog Object
	CFileDialog* selekt = new CFileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, onlywav, NULL);
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
	CString stringer;
	//check to see if .WAV is the extension
	stringer = selekt->GetFileExt();
	if ((stringer != "wav")&& (stringer != "WAV"))
	//wrong extension, show dialog and get out
	{
		delete selekt;
		AfxMessageBox("You must choose a WAV file to associate an analysis file with.", MB_OK, 0);
		return;
	}
	//read in path name and file name
	pDoc->docdobj.filein = selekt->GetPathName();
	pDoc->docdobj.fileinn = selekt->GetFileName();
	pDoc->SetModifiedFlag();
	Audiocleanup();
	Audioopen();
	delete selekt;	
}

//process message that sound is over
LONG CSpanformView::OnMciNotify(UINT wFlags, LONG lDevId)
{
	switch(wFlags) 
	{
		case MCI_NOTIFY_SUCCESSFUL:
			theApp.weplaying = FALSE;
			KillTimer(timeyid);
			break;
		case MCI_NOTIFY_FAILURE:
			AfxMessageBox("Notify Failure");
			break;
		case MCI_NOTIFY_SUPERSEDED:
			AfxMessageBox("Notify Superseded");
			break;
	}
	return 0L;
}



//restart audio at beginning of track
void CSpanformView::OnAudioRestart() 
{
	if (audioopen == TRUE)
	{
		MCI_SEEK_PARMS seekparms;
		seekparms.dwTo = 0;
		OnAudioStop();
		mciSendCommand(openParams.wDeviceID, MCI_SEEK, MCI_WAIT|MCI_SEEK_TO_START, (DWORD)(LPVOID)&seekparms);
		Invalidate();
		UpdateWindow();
	}
}


void CSpanformView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ((audioopen == TRUE)&&(theApp.weplaying == FALSE))
	{
		InvalidateRect(ret, FALSE);
		UpdateWindow();
	}
	CScrollView::OnLButtonUp(nFlags, point);
}

//reset position to here
void CSpanformView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CSpanformDoc* pDoc = GetDocument();
	CClientDC aDC(this);
	OnPrepareDC(&aDC);
	aDC.DPtoLP(&point);
	if ((audioopen == TRUE)&&(theApp.weplaying == FALSE))
	{
		if ((point.x >= startme) && (point.x <= starme))
		{
			//set time to this location
			double ratio = (double)pDoc->getnumframes()/((double)(starme-startme));
			double lasty = ((double)((starme-startme)*pDoc->getslider())/(double)pDoc->getsamprate())*ratio;
			lasty = lasty*1000;
			lasty = lasty*((double)(point.x-startme)/(double)(starme-startme));
			unsigned long lasttime = (unsigned long)(lasty);
			MCI_SEEK_PARMS seekme;
			seekme.dwTo = lasttime;
			mciSendCommand(openParams.wDeviceID, MCI_SEEK, MCI_WAIT|MCI_TO, (DWORD)(LPVOID)&seekme);	
		}
	}
	CScrollView::OnLButtonDown(nFlags, point);
}

//timer
void CSpanformView::OnTimer(UINT vall)
{
	KillTimer(vall);
	InvalidateRect(ret, FALSE);
	UpdateWindow();
	timeyid = SetTimer(1, 50, NULL);
}
