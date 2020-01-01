////////////////////////////////////////////////////////////////////////////////
//  Unit� "ZeGameWndH.h": Classe g�rant la fen�tre principale du jeu d�rivant //
//						  de la classe MFC CWnd								  //																			  
//																			  //		
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.10                                                  //
//  Creation     : 12/08/1999												  //
//  Last upgrade : none                                                       //
////////////////////////////////////////////////////////////////////////////////

#ifndef ZeGameWndH
#define ZeGameWndH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ZeGameWnd : public CWnd
{
	//--------------------------------------------------- Attributs de la classe
public:
	bool bWindowActive;	// Indique si c'est cette fen�tre qui d�tient le focus
	
	//--------------------------------------------------------------- Operations
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ZeGameWnd)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	//---------------------------------------------------- M�thodes de la classe
public:
	ZeGameWnd();				// Constructeur par d�faut
	virtual ~ZeGameWnd();		// Destructeur

	//------------------------------------------ Generated message map functions
protected:
	//{{AFX_MSG(ZeGameWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


// Unique instance de la fen�tre principale du jeu
extern ZeGameWnd *ZeGameWindow;	

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // ZeGameWndH
