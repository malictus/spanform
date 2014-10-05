//StdAfx.h
//Part of Spanform, by James Halliday, copyright 2000, all rights reserved
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently

#if !defined(AFX_STDAFX_H__B9125B06_DA5C_11D2_A630_D5FD7E869135__INCLUDED_)
#define AFX_STDAFX_H__B9125B06_DA5C_11D2_A630_D5FD7E869135__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
//don't need this
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
//my added include files
//for multimedia
#include <mmsystem.h>
//for arrays
#include <afxtempl.h>
//for sin function
#include <math.h>


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B9125B06_DA5C_11D2_A630_D5FD7E869135__INCLUDED_)
