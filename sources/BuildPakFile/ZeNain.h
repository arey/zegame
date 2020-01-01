////////////////////////////////////////////////////////////////////////////////
// ZeNain.h : header à inclure dans chaque .cpp du projet :					  //
//	-> permet de faire cohabiter les sources sous Visual C++ et C++Builder	  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ZeNainH
#define ZeNainH

#ifdef VISUALC
#pragma warning(disable : 4786 4100)
#endif

//------------------------------------------------------------------------ include
#ifdef CBUILDER
#include "vcl.h"
#endif

//------------------------------------------------------------------------- macros
#ifdef VISUALC
#include <afx.h>
#define AnsiString		CString
#endif

#ifdef CBUILDER
#define	LPCSTR(a)		a.c_str()
#endif

#define	USE_PARAM(a)	(a = a)

//-------------------------------------------------------------------------- define
#define ZENAIN_MAX_CHAR			512
#define WM_TOGGLEFULLSCREEN		WM_USER+5		// Message personnalisé servant à basculer entre le Fullscreen et le Windowed mode

#endif	// ZeNainH