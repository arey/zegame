////////////////////////////////////////////////////////////////////////////////
//  File		:	ZeGame.h												  //
//                                                                            //
//	Description :   Main header file for the ZEGAME application				  //
//                                                                            //
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZEGAME_H__1BE35DA4_4769_11D3_9087_0048543CA1CC__INCLUDED_)
#define AFX_ZEGAME_H__1BE35DA4_4769_11D3_9087_0048543CA1CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CZeGameApp:
// See ZeGame.cpp for the implementation of this class
//

class CZeGameApp : public CWinApp
{
public:
	CZeGameApp();				// Construceur par défaut
	virtual ~CZeGameApp();		// Destructeur

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZeGameApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CZeGameApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZEGAME_H__1BE35DA4_4769_11D3_9087_0048543CA1CC__INCLUDED_)
