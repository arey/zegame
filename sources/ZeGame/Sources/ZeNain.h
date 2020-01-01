////////////////////////////////////////////////////////////////////////////////
//  File		:	ZeNain.h												  //
//                                                                            //
//	Description :   Header a include dans chaque source .cpp du projet		  //
//                                                                            //
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ZeNainH
#define ZeNainH

#ifdef VISUALC
#pragma warning(disable : 4786 4100 4018 4663)  
#endif

//------------------------------------------------------------------------ include
#include "StdAfx.h"

//------------------------------------------------------------------------- macros
#ifdef VISUALC
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