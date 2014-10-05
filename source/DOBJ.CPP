//dobj.cpp
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved
//contains information about the spanform file format and handling, 'create' menu function
//FFT algorithm in this code adapted from code written by Don Cross, used by permission

#include "stdafx.h"
#include "dobj.h"
//progress dialog header
#include "progdlg.h"
//analysis dialog header
#include "canaldialog.h"

//Serialization macro (ALWAYS USES VERSION 1 for now)
IMPLEMENT_SERIAL(cdobj, CObject, 1)

//constructor - sets default values for all document variables
cdobj::cdobj()
{
	//the following values should ALWAYS be overwritten in the creation routines
	sfversion = 0;
	numchan = 1;
	srate = 22050;
	bittie = 8;
	filein = "ERROR";
	fileinn = "ERROR";
	numsamps = 1;
	lengthy = 1;
	numframes = 0;
	numbits = 0;
	framesize = 0;
	//the following values correspond to the default values of the analysis dialog
	power2 = 1024;
	slider = 200;
	spanwindow = 2;
	hifreq = 12000;
	//display defaults - not changed in creation but may be changed later with display
	clr = TRUE;
	sensitive = 4;
	noisy = 4;
	horr = 10;
	vertt = 10;
	lett = FALSE;
	expo = TRUE;
	//no default arrays for bytelist or vollist arrays
}

//Serialize()
void cdobj::Serialize(CArchive& ar)
{
	//call base object
	CObject::Serialize(ar);
	//serialize the lists of BYTEs
	bytelist.Serialize(ar);
	vollist.Serialize(ar);
	//serialize the other variables
	if (ar.IsStoring())
	{
		ar << sfversion;
		ar << numchan;
		ar << srate;
		ar << bittie;
		ar << filein;
		ar << fileinn;
		ar << numsamps;
		ar << lengthy;
		ar << power2;
		ar << slider;
		ar << numframes;
		ar << spanwindow;
		ar << numbits;
		ar << framesize;
		ar << hifreq;
		ar << clr;
		ar << sensitive;
		ar << noisy;
		ar << horr;
		ar << vertt;
		ar << lett;
		ar << expo;
	}
	else
	{
		ar >> sfversion;
		ar >> numchan;
		ar >> srate;
		ar >> bittie;
		ar >> filein;
		ar >> fileinn;
		ar >> numsamps;
		ar >> lengthy;
		ar >> power2;
		ar >> slider;
		ar >> numframes;
		ar >> spanwindow;
		ar >> numbits;
		ar >> framesize;
		ar >> hifreq;
		ar >> clr;
		ar >> sensitive;
		ar >> noisy;
		ar >> horr;
		ar >> vertt;
		ar >> lett;
		ar >> expo;
	}
}

////////////////////////////////////FILE CREATE FUNCTIONS////////////////////////////////
//       THE FOLLOWING FUNCTIONS CREATE THE ACTUAL ANALYSIS FILE FROM A WAV FILE	   //
/////////////////////////////////////////////////////////////////////////////////////////

//letsgo() is called to start the creation process
bool cdobj::letsgo()
{
	//strings for open and save dialogs
	const char BASED_CODE onlywav[] = "WAV Files (*.wav)|*.wav||";	
	const char BASED_CODE onlyzed[] = "Spanform Analysis Files (*.zed)|*.zed||";
	const char BASED_CODE zedstring[] = "zed";
	CString stringer;
	CString outs;
	//whether user canceled or not
	int huh = 0;
	
	//create the local CFileDialog Object
	CFileDialog* selekt = new CFileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, onlywav, NULL);
	if (selekt==NULL)
	{
		AfxMessageBox("Out of memory. Command aborted.", MB_OK, 0);
		return false;
	}
	//display the dialog
	huh = selekt->DoModal();
	if (huh==IDCANCEL)
	{
		//user canceled or error - abort
		delete selekt;
		return false;
	}
	//get the WAV file name (minus extension) to set up default name in save box
	stringer = selekt->GetFileTitle();
	//append the correct file extension
	stringer = stringer + ".zed";
	//create and show the Save File Object
	CFileDialog* pik = new CFileDialog(FALSE, zedstring, stringer, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, onlyzed);
	if (pik==NULL)
	{
		delete selekt;
		AfxMessageBox("Out of memory. Command aborted.", MB_OK, 0);
		return false;
	}
	huh = pik->DoModal();
	if (huh==IDCANCEL)
	{
		delete pik;
		delete selekt;
		return false;
	}
	//check to see if .zed is the extension
	stringer = pik->GetFileExt();
	if (!(stringer == "zed"))
	//wrong extension, show dialog and get out
	{
		delete pik;
		delete selekt;
		AfxMessageBox("The name you chose for your analysis file has the wrong extension. Use the '.zed' extension (for example, sound.zed).", MB_OK, 0);
		return false;
	}
	//get output path name - we'll need it later
	outs = pik->GetPathName();
	//set global document parameters as well
	filein = selekt->GetPathName();
	fileinn = selekt->GetFileName();
	//set file version to 1
	sfversion = 1;
	//delete the CFileDialog objects and move on
	delete pik;
	delete selekt;
	//////////////////////////////////////
	//FOUR STEPS FOR CREATION TO WORK...//
	//////////////////////////////////////
	//STEP 1 - check WAV file for compatibility, read in WAV info to object
	if (checkWAV(filein) != true)
	{
		//WAV file is not compatible; abort
		return false;
	}
	//STEP 2 - show analysis options dialog
	if (showanal() != true)
	{
		//user canceled
		return false;
	}
	//STEP 3 - actually do the FFT (the long step!)
	if (makeFFT(filein) != true)
	{
		//FFT creation failed, abort without creating a file
		return false;
	}
	//STEP 4 - write the file to disk
	//save the analysis object on disk
	if (makeanew(outs) != true)
	{
		//unknown error (can't happen here right now!)
		return false;
	}
	//done - hurray!
	return true;
}

//checkWAV - check the wave file for compatibility
bool cdobj::checkWAV(CString myfilein)
{
	//multimedia file handles and structs
	HMMIO wavhand = NULL;
	MMCKINFO inchunky;
	MMCKINFO inchunkier;
	PCMWAVEFORMAT pcmstuf;
	
	//file path has already been verified, so we can go right to work!
	//recast filein string as a LPSTR; this seems to be the only way to do it!
	LPSTR fileinalt = new TCHAR[myfilein.GetLength()+1];
	if (fileinalt == NULL)
	{
		AfxMessageBox("Out of memory. Command aborted.", MB_OK, 0);
		return false;
	}
	//copy from myfilein to fileinalt
	_tcscpy(fileinalt, myfilein);
	//open the WAV file
	wavhand = mmioOpen(fileinalt, NULL, MMIO_ALLOCBUF | MMIO_READ);
	//won't need this anymore
	delete fileinalt;
	//file couldn't be opened; abort
	if (wavhand==NULL)
	{
		AfxMessageBox("WAV file could not be opened due to an unknown error. Command aborted.", MB_OK, 0);
		return false;
	}
	////////////////////////WAVHAND MUST NOW BE CLOSED BEFORE FINISHING//////////////////////
	//descend into the 'RIFF' chunk of the WAV file
	if (mmioDescend(wavhand, &inchunky, NULL, 0) != 0)
	{
		//chunk not found, something is screwy
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file is missing vital information. Command aborted.", MB_OK, 0);
		return false;
	}
	//making sure the input file is a real WAV file and not something else
	if ((inchunky.ckid != FOURCC_RIFF) || (inchunky.fccType != mmioFOURCC('W','A','V','E')))
	{
		mmioClose(wavhand, 0);
		AfxMessageBox("Input file is not an actual WAV file. Command aborted.", MB_OK, 0);
		return false;
	}
	//find the 'fmt' chunk of the WAV file
	inchunkier.ckid = mmioFOURCC('f','m','t',' ');
	if (mmioDescend(wavhand, &inchunkier, &inchunky, MMIO_FINDCHUNK)!= 0)
	{
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file is missing vital information. Command aborted.", MB_OK, 0);
		return false;
	}
	//check to make sure format chunk is long enough
	if ((inchunkier.cksize)<((long)sizeof(pcmstuf)))
	{
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file cannot be opened. Command aborted.", MB_OK, 0);
		return false;
	}
	//read 'fmt' chunk into pcmstuf
	if (mmioRead(wavhand, (HPSTR)&pcmstuf, (long)sizeof(pcmstuf))!=(long)sizeof(pcmstuf))
	{
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file cannot be opened. Command aborted.", MB_OK, 0);
		return false;
	}
	//ascend back out of chunk
	if (mmioAscend(wavhand, &inchunkier, 0) != 0)
	{
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file cannot be opened. Command aborted.", MB_OK, 0);
		return false;
	}
	//check to make sure WAV file is in PCM (uncompressed) form
	if (pcmstuf.wf.wFormatTag != WAVE_FORMAT_PCM)
	{
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file must be in PCM (noncompressed) format for analysis to work. Command aborted.", MB_OK, 0);
		return false;
	}
	//check number of channels of WAV file
	numchan = (int)pcmstuf.wf.nChannels;
	if ((numchan != 1) && (numchan != 2))
	{
		//file is not 1 or 2 (is this possible?)
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file must be stereo or mono only for analysis to work. Command aborted.", MB_OK, 0);
		return false;
	}
	//check the sample rate
	srate = (long)pcmstuf.wf.nSamplesPerSec;
	if (srate < 22050)
	{
		//low sample rate
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file uses a low-quality sampling rate. Spanform needs files sampled at 22050 Hz or higher. Command aborted.", MB_OK, 0);
		return false;
	}
	//check bits per sample
	bittie = (int)pcmstuf.wBitsPerSample;
	if ((bittie != 8) && (bittie!= 16))
	{
		//file isn't 8-bit or 16-bit)
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file must be 8-bit or 16-bit. Command aborted.", MB_OK, 0);
		return false;
	}
	//WAV file checks out OK, move on (will reopen wavhand elsewhere...)
	mmioClose(wavhand, 0);
	return true;
}

//showanal - show the analysis options dialog box
bool cdobj::showanal()
{
	int val;
	canaldialog	opts;

	val = opts.DoModal();
	if (val != IDOK)
	{
		//user canceled - don't change values
		return false;
	}
	//set object variables to the settings returned by the dialog
	slider = opts.slider;
	power2 = opts.power2;
	spanwindow = opts.windowtype;
	hifreq = opts.hifreq;
	return true;
}

//makeFFT - the actual creation of the analysis file
bool cdobj::makeFFT(CString myfilein)
{
	//multimedia file handles and structs
	HMMIO wavhand = NULL;
	MMCKINFO inchunky;
	MMCKINFO inchunkier;
	//starts as number of bytes in WAV file, decremented to (near) zero
	long biggie;
	//the number of bytes in each sample unit
	short bytesinchunk;
	//the array of values to be changed
	CArray<double, double&> inarray;
	//an individual value for the array
	double cand = 0;
	//helper var, used in 16-bit stereo only
	double candalt = 0;
	int counter = 0;
	//raw byte buffer - 4 is the highest we'll need (for 16-bit stereo)
	char mybuf[4];
	//maximum value in raw output array - used to convert all vars to between 1 and 255
	double maxie;
	//maximum volume level in a frame - used to calculate vollist entry
	int volmaxie;
	//the progress dialog and its variables, including messaging vars
	Cprogdlg myprog;
	MSG* pMsg;
	CString analtext;
	char buf[10];
	
	//////////////////////////////STEP 1 - OPEN WAV FILE/////////////////////////////////////
	LPSTR fileinalt = new TCHAR[myfilein.GetLength()+1];
	if (fileinalt == NULL)
	{
		AfxMessageBox("Out of memory. Command aborted.", MB_OK, 0);
		return false;
	}
	_tcscpy(fileinalt, myfilein);
	wavhand = mmioOpen(fileinalt, NULL, MMIO_EXCLUSIVE | MMIO_READ);
	delete fileinalt;
	if (wavhand==NULL)
	{
		AfxMessageBox("WAV file could not be opened due to an unknown error. Command aborted.", MB_OK, 0);
		return false;
	}
	//WAV file (wavhand) is now open, must close later...
	if (mmioDescend(wavhand, &inchunky, NULL, 0) != 0)
	{
		//chunk not found, something is screwy
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file is missing vital information. Command aborted.", MB_OK, 0);
		return false;
	}
	//descend into the 'data' chunk for raw sound, since we already know the 'fmt' info
	inchunkier.ckid = mmioFOURCC('d','a','t','a');
	if (mmioDescend(wavhand, &inchunkier, &inchunky, MMIO_FINDCHUNK)!= 0)
	{
		mmioClose(wavhand, 0);
		AfxMessageBox("WAV file is missing vital information. Command aborted.", MB_OK, 0);
		return false;
	}
	///////////////////////////////STEP 2 - SET INITIAL VALUES///////////////////////////////
	numframes = 0;
	//set biggie to total number of bytes of sample data
	biggie = inchunkier.cksize;
	//set numsamps to reflect 'real' array size, adjust for bit rate and number of channels
	bytesinchunk = (bittie/8) * numchan;
	numsamps = biggie/bytesinchunk;
	//we can now figure length of file in seconds (lengthy, global variable)
	lengthy = ((float)numsamps)/((float)srate);
	//find numbits, used for internal FFT processing, based on power2
	numbits = 0;
	for (;;)
	{
		if (power2 & (1 << numbits))
			break;
		numbits = numbits + 1;
	}
	inarray.SetSize(power2);
	volmaxie = 0;
	//find framesize, including adjustment for hi freq cutoff
	framesize = 0;
	counter = 1;
	while ((counter < (power2/2)) && (((srate*counter)/power2) < hifreq))
	{
		framesize = framesize + 1;
		counter = counter + 1;
	}
	//////////////////////////STEP 3 - SETUP AND SHOW PROGRESS DIALOG////////////////////////
	myprog.Create(IDD_PROG);
	analtext = "Analyzing: " + fileinn; 
	myprog.SetWindowText(analtext);
	//check to see if this file will be too huge, cancel if it will be
	if ((numsamps/slider)>(short)(numsamps/slider))
	{
		mmioClose(wavhand, 0);
		AfxMessageBox("This file cannot be processed with these settings, because the resulting file would be too large. Try increasing the 'slide' setting.", MB_OK, 0);
		return false;
	}
	//set range of progress dialog control
	myprog.progger.SetRange(0, (short)(numsamps/slider));
	myprog.progger.SetPos(0);
	myprog.progger.SetStep(1);
	myprog.ShowWindow(SW_SHOW);
	myprog.UpdateWindow();
	////////////////////////////////STEP 4 - THE BIG LOOP////////////////////////////////////
	//first let's make sure we can process at least one frame, if not, abort
	if (biggie < (power2*bytesinchunk))
		{
			mmioClose(wavhand, 0);
			AfxMessageBox("File is too short to create even one frame of analysis. Command aborted.", MB_OK, 0);
			return false;
		}
	//'infinite' loop
	for (;;)
	{
		//do we have enough bytes left in file for another frame?, if not, we're done!
		if (biggie < (power2*bytesinchunk))
		{
			mmioClose(wavhand, 0);
			return true;
		}
		//reset inner loop counter and volume info
		counter = 0;
		volmaxie = 0;
		///////////////////////////////INNER (ONE FRAME) LOOP
		while (counter < power2)
		{
			//read in enough bytes to process one candidate
			if ((mmioRead(wavhand, mybuf, bytesinchunk)) < 1)
			{
				mmioClose(wavhand, 0);
				AfxMessageBox("Error - end of file reached prematurely. File will be saved.", MB_OK, 0);
				//can still save what we have, so keep it
				return true;
			}
			/////////////////GET ONE CANDIDATE TO READ IN
			//switch statement speeds things a bit...
			switch (numchan+bittie)
			{
			case 9:
				//8-bit mono version
				cand = (double)(mybuf[0]);
				if (cand < 0)
					cand = 256 + cand;
				break;
			case 10:
				//8-bit stereo version
				cand = (double)(mybuf[0]);
				cand = (cand + (double)(mybuf[1]))/2;
				if (cand < 0)
					cand = 256 + cand;
				break;
			case 17:
			//16-bit mono version
				cand = (double)(mybuf[1]);
				cand = cand*256;
				cand = cand + (double)(mybuf[0]);
				break;
			case 18:
				//16-bit stereo
				cand = (double)(mybuf[1]);
				cand = cand*256;
				cand = cand + (double)(mybuf[0]);
				candalt = (double)(mybuf[3]);
				candalt = candalt*256;
				candalt = candalt + (double)(mybuf[2]);
				cand = (cand + candalt) / 2;
				break;
			default:
				mmioClose(wavhand, 0);
				AfxMessageBox("Unknown error. Command aborted.", MB_OK, 0);
				return false;
			}
			/////////////////CANDIDATE FOUND
			//find max volume level
			if (cand > volmaxie)
				volmaxie = (int)cand;
			//adjust variable if window is not rectangular
			if (spanwindow == 2)
			{
				//hamming
				cand = cand * (0.54 - (0.46 * cos(2*3.14159265358979323846*counter/(power2-1))));
			}
			//add our candidate to the inarray
			inarray[counter] = cand;
			//increment inner counter, and go back to do it again...
			counter = counter + 1;
		}
		////////////////////////END INNER LOOP
		////////////////////////CHECK DIALOG
		pMsg = &AfxGetThread()->m_msgCur;
		// pump message, but quit on WM_QUIT
		while (::PeekMessage(pMsg, NULL, NULL, NULL, PM_NOREMOVE))
		{
			if (!AfxGetThread()->PumpMessage())
			{
				AfxPostQuitMessage(0);
				return false;
			}
		}
		LONG lIdle = 0;
		while (AfxGetApp()->OnIdle(lIdle++ ));
		if (myprog.progstatus == 1)
		{
			//keep
			mmioClose(wavhand, 0);
			return true;
		}
		if (myprog.progstatus == 2)
		{
			//cancel
			mmioClose(wavhand, 0);
			return false;
		}
		/////////////////////////////CONVERT TO FFT DATA
		if(changeit(inarray) == false)
		{
			//conversion failed
			mmioClose(wavhand, 0);
			AfxMessageBox("Problems converting samples. Command aborted.", MB_OK, 0);
			return false;
		}
		////////////////////////CONVERT INARRAY TO BYTELIST DATA
		//first, find max value in inarray
		maxie = 1;
		//first frame (n[1]) is sometimes too large (not sure why..?) starts at 2 instead
		int n = 2;
		while (n < (power2/2))
		{
			if (inarray[n] > maxie)
				maxie = inarray[n];
			n = n + 1;
		}
		//because first frame is sometimes too big (unknown reason?)
		bytelist.Add(1);
		//next, write values to bytelist
		n = 2;
		//go until hifreq is reached
		while ((n < (power2/2)) && (((srate*n)/power2) < hifreq))
		{
			//convert to a number between 0 and 255 and add to byte array
			inarray[n] = (inarray[n]/maxie)*255;
			bytelist.Add((BYTE)(inarray[n]));
			n = n + 1;
		}
		//write vol maximum for this frame
		if (bittie == 16)
			volmaxie = (int)(((double)volmaxie/32767)*255);
		else
			volmaxie = (volmaxie - 128) * 2;
		vollist.Add((BYTE)(volmaxie));
		////////////////////////OTHER CLEANUP
		//increment frame count
		numframes = numframes + 1;
		//move startposition for file read
		mmioSeek(wavhand, 0-((power2-slider)*bytesinchunk), SEEK_CUR);
		//decrement total number of samples for loop
		biggie = biggie - (slider*bytesinchunk);
		//increment progress dialog
		myprog.progger.StepIt();
		//write progress data to dialog, subtracting power2 works!
		_ltoa(((biggie/bytesinchunk)-power2), buf, 10);
		myprog.m_unitleft = buf;
		_ltoa(numframes*slider, buf, 10);
		myprog.m_unitanalyzed = buf;
		myprog.UpdateData(FALSE);
	//EMD OUTER LOOP
	}
	//shouldn't ever get here, but compiler complains if this isn't here...
	return true;
}
	
//changeit() - the actual FFT itself - note that inarray itself is changed to output
//This function is derived from an algorithm by Don Cross.
bool cdobj::changeit(CArray<double, double&>& inarray)
{
	//temporary arrays for FFT creation
	CArray<double, double&> outarray;
	CArray<double, double&> ioutarray;
	//counters, counters, counters!
	int count = 0;
	int count2 = 0;
	unsigned int mark = 0;
	//vars for internal FFT processing
	int blockend = 0;
	int blocksize = 0;
	double deltaangle = 0;
	double alpha = 0;
	double beta = 0;
	double ar, ai;
	int k;
	double tr, ti;
	double deltaar;
	
	//first set sizes for outarray and ioutarray, ioarray will be initialized to 0 here
	outarray.SetSize(power2);
	ioutarray.SetSize(power2);
	//next - reverse bits in array
	count = 0;
	mark = 0;
	while (count < power2)
	{
		mark = reversebits(count);
		outarray[mark] = inarray[count];
		count++;
	}
	//next = actual FFT calculations
	blockend = 1;
	blocksize = 2;
	//big loop
	while (blocksize <= power2)
	{
		deltaangle = (2.0 * 3.14159265358979323846) / (double)blocksize;
		alpha = sin(0.5 * deltaangle);
		alpha = 2.0 * alpha * alpha;
		beta = sin(deltaangle);
		count = 0;
		//medium loop
		while (count < power2)
		{
			ar = 1.0;
			ai = 0.0;
			mark = count;
			count2 = 0;
			//little loop
			while (count2 < blockend)
			{
				k = mark + blockend;
				tr = (ar*outarray[k]) - (ai*ioutarray[k]);
				ti = (ai*outarray[k]) + (ar*ioutarray[k]);
				outarray[k] = outarray[mark] - tr;
				ioutarray[k] = ioutarray[mark] - ti;
				outarray[mark] += tr;
				ioutarray[mark] += ti;
				deltaar = (alpha*ar) + (beta*ai);
				ai -= ((alpha*ai) - (beta*ar));
				ar -= deltaar;
				//little loop incrementers
				mark = mark + 1;
				count2 = count2 + 1;
			}
			//medium loop incrementer
			count += blocksize;
		}
		//big loop incrementers
		blockend = blocksize;
		blocksize <<= 1;
	}
	//FFT DONE
	//copy outarray back to inarray to send back to program, also convert to positive vals
	count = 0;
	//no need to write in more than we need...
	while ((count < (power2/2)) && (((srate*count)/power2) < hifreq))
	{
		//write absolute value!
		inarray[count] = fabs(outarray[count]);
		count = count + 1;
	}
	return true;
}

//reversebits - an internal process of FFT, from an algorithm of Don Cross
unsigned int cdobj::reversebits(unsigned int index)
{
	unsigned i, rev;
	for (i=rev=0; i < numbits; i++)
	{
		rev = (rev << 1) | (index & 1);
		index >>= 1;
	}
	return rev;
}

//makeanew - write the analysis file to disk
bool cdobj::makeanew(CString myfileout)
{
	CFile spanfile;
	//create the output file
	if (!spanfile.Open(myfileout, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive))
	{
		//file couldn't be created; abort
		AfxMessageBox("Error creating analysis file. Command aborted.", MB_OK, 0);
		return false;
	}
	//create the archive to use to save to the output file
	CArchive spanarch(&spanfile, CArchive::store);
	//write all data to file
	Serialize(spanarch);
	//flush and close archive
	spanarch.Close();
	//close output file
	spanfile.Close();
	//we're done, baby!
	return true;
}
