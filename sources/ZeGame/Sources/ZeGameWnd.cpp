////////////////////////////////////////////////////////////////////////////////
//  Unité "ZeGameWndH.cpp": Implémentation de la classe gérant la fenêtre	  //
//							principale du jeu dérivant de la classe MFC CWnd  //
//																			  //		
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.10                                                  //
//  Creation     : 12/08/1999												  //
//  Last upgrade : none                                                       //
////////////////////////////////////////////////////////////////////////////////
//

// --------------------------------------------------------------------- include
#include "ZeNain.h"				// Fichier commun à tout le projet
#include "consts.h"				// Constantes globales à tout le projet
#include "ZeGame.h"				// Header du fichier principal du projet
#include "ZeGameMain.h"			// Fonctions principales d'ouverture/fermeture du jeu
#include "DDLib.h"				// Librairie maison de routines DirectDraw
#include "ZeGameWnd.h"			// Header de la classe

// ---------------------------------------------------------------------- define
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ------------------------------------------------------------------- variables
ZeGameWnd *ZeGameWindow = NULL;		// Unique instance de la fenêtre principale du jeu


/////////////////////////////////////////////////////////////////////////////////
// Classe ZeGameWnd															   //
/////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ============================================================//
ZeGameWnd::ZeGameWnd()
{	bWindowActive = true;		
}
//-----------------------------------------------------------------------------//

//=== Destructeur =============================================================//
ZeGameWnd::~ZeGameWnd()
{
}
//-----------------------------------------------------------------------------//


//=== Fonction callback de traitement des messages de la fenêtre principale ===//
LRESULT ZeGameWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
		// La fenêtre principale du jeu reprends le focus
		case WM_SETFOCUS :
			if (!bWindowActive)
			{	bWindowActive = true;
				bGamePause = false;
				GamePaused();
			}
			return 0L;

		// Perte du focus windows
		case WM_KILLFOCUS:
			if (bWindowActive)
			{	bWindowActive = false;
				bGamePause = true;
				GamePaused();
			}
			return 0L;

		// Fermeture de l'appli
		case WM_CLOSE:
			bGamePause = true;
			GamePaused();
			CloseZeGame();	// Désalloue tout			
			break;
			
		// Déplacement de la fenêtre
		case WM_MOVE:
			// Récupération de la nouvelle position de la fenêtre
            if ((bWindowActive) && (bGameActive) && (!DS->FullScreen))
            {	DS->UpdateWindowPos();
            }
			bGamePause = false;
			GamePaused();
			break;
		
		case WM_MOVING:
			bGamePause = true;
			GamePaused();
			break;

		// Changement de taille de la fenêtre
		case WM_SIZE:
            // Vérifie si l'on a pas perdue la fenêtre
            if ((wParam==SIZE_MAXHIDE) || (wParam==SIZE_MINIMIZED))
			{	bWindowActive = false;
				bGamePause = true;
				GamePaused();
			} else
			{	bWindowActive = true;
				bGamePause = false;
				GamePaused();
			}
            break;

		// Permutation Windowed-mode <-> FullScreen-mode
		case WM_TOGGLEFULLSCREEN:	
			if ((bWindowActive) && (bGameActive))
			{	bGameActive = false;
				ToggleFullScreenMode();
				bGameActive = true;
			}
			return 0L;
		
		// Rafraîchissement de la fenêtre
		case WM_PAINT:        
            if ((DS!=NULL) && (!DS->FullScreen) && (bWindowActive))
            {	while (true)
				{	HRESULT hr;
					hr = DS->pDDSPrimary->pDDSurf->Blt(&DS->g_rcScreen, DS->pDDSBack->pDDSurf,
													   &DS->g_rcViewport, DDBLT_WAIT, NULL);
                    if (hr == DD_OK) break;
                    if (hr == DDERR_SURFACELOST)
                    {   hr = DS->pDDSPrimary->pDDSurf->Restore();
                        if (hr != DD_OK ) break;
                    }
                    if (hr != DDERR_WASSTILLDRAWING) break;
                }
            }
            break;

		// Met le jeu en pause si la fenêtre est minimisée ou si elle n'est plus au dessus de toutes les autres
		case WM_ACTIVATEAPP:
            bWindowActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
			bGamePause = !bWindowActive;
			GamePaused();
            return 0L;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}
//-----------------------------------------------------------------------------//


/////////////////////// Fin de la classe ZeGameWnd //////////////////////////////


BEGIN_MESSAGE_MAP(ZeGameWnd, CWnd)
	//{{AFX_MSG_MAP(ZeGameWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


