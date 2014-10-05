//dobj.h
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved

#ifndef dobj_h
#define dobj_h

class cdobj : public CObject
{
//macro for serialization
DECLARE_SERIAL(cdobj)

//////////////////////SPANFORM DOC VARIABLES/////////////////////////
protected:
	short sfversion;
	short numchan;
	long srate;
	int bittie;
	long numsamps;
	float lengthy;
	int power2;
	int slider;
	long numframes;
	int spanwindow;
	unsigned int numbits;
	int framesize;
	int hifreq;
	
//these vars must be accessed publicly
public:
	//growable array of BYTES (unsigned nums from 1 to 255) for analysis array
	CArray<BYTE, const BYTE&> bytelist;
	//list of volumes for each frame
	CArray<BYTE, const BYTE&> vollist;
	//true if color display, false otherwise
	BOOL clr;
	//senisitivity, between 0 and 9
	int sensitive;
	//noise threshold, between 0 and 9
	int noisy;
	//variables for horizontal and vertical ratios
	int horr;
	int vertt;
	//true if letters displayed, false otherwise
	BOOL lett;
	//true if viewing exponentially, false otherwise
	BOOL expo;
	CString filein;
	CString fileinn;
	
public:
	//constructor - no parameters right now
	cdobj();
	//donothing destructor
	~cdobj(){}
	//Serialize function
	void Serialize(CArchive& ar);
	///////////////////////////////CREATE FUNCTIONS//////////////////////
	//starts the create process
	bool letsgo();
	//check WAV file for compatibility and write info to object
	bool checkWAV(CString myfilein);
	//make the FFT array itself
	bool makeFFT(CString myfilein);
	bool changeit(CArray<double, double&>& inarray);
	//internal
	unsigned int reversebits(unsigned int index);
	//write analysis file to disk
	bool makeanew(CString myfileout);
	//show the analysis options dialog and retrieve information from it
	bool showanal();
	////////////////////////RETRIEVAL FUNCTIONS//////////////////////////
	short getversion()
	{return sfversion;}
	CString getpath()
	{return filein;}
	CString getname()
	{return fileinn;}
	short getchan()
	{return numchan;}
	int getbits()
	{return bittie;}
	long getsamprate()
	{return srate;}
	float getlengthy()
	{return lengthy;}
	long getnumsamps()
	{return numsamps;}
	int getpower2()
	{return power2;}
	int getslider()
	{return slider;}
	int getspanwindow()
	{return spanwindow;}
	long getnumframes()
	{return numframes;}
	int getframesize()
	{return framesize;}
	int gethifreq()
	{return hifreq;}
};

#endif
