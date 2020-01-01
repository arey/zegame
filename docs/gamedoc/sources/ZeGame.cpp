// ZeGame.cpp : Defines the class behaviors for the application.
//

// --------------------------------------------------------------------- include
#include "ZeNain.h"				// Fichier commun à tout le projet
#include "DDLib.h"				// Librairie maison de routines DirectDraw
#include "consts.h"				// Constantes communes au projet
#include "ZeGameWnd.h"			// Fenêtre principale de l'appli
#include "ZeGameMain.h"			// Main du jeu
#include "fBeginDlg.h"			// Boîte de dialogue du démarrage	
#include "ZeGame.h"				// Header de la classe

// ---------------------------------------------------------------------- define
/*#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/


/////////////////////////////////////////////////////////////////////////////
// CZeGameApp		 													   //	
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CZeGameApp, CWinApp)
	//{{AFX_MSG_MAP(CZeGameApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZeGameApp construction

CZeGameApp::CZeGameApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CZeGameApp object

CZeGameApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CZeGameApp initialization

BOOL CZeGameApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	//--- Boîte de dialogue de démarrage du jeu ---
	fBeginDlg begDlg;
	begDlg.DoModal();
	if (!begDlg.LetsGo()) return FALSE;

	//--- Création de la fenêtre du jeu ---
	ZeGameWindow = new ZeGameWnd();
	ZeGameWindow->CreateEx(
			0, 
			AfxRegisterWndClass(CS_DBLCLKS), 
			" Nains - alpha 1.62", 
			WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 
			0, 
			0, 
			CFG_SCREEN_WIDTH  + GetSystemMetrics(SM_CXSIZEFRAME)*2, 
			CFG_SCREEN_HEIGHT + GetSystemMetrics(SM_CYSIZEFRAME)*2 + GetSystemMetrics(SM_CYMENU),
			NULL, 
			0);	
	
	#ifndef WIN2000
	ZeGameWindow->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), true);		// Ch'tite tête de nain
	#endif // WIN2000

	m_pMainWnd = ZeGameWindow ;
	
	SetCursor(NULL);	// Cache le curseur souris
	
	//--- Charge et lance le jeu ---
	MainZeGame();
	
	SetCursor(NULL);	// Recache le curseur souris

	return TRUE;
}

//=== Destructeur ============================================================//
CZeGameApp::~CZeGameApp()
{	// Supprime la fenêtre de l'appli
	delete ZeGameWindow;
	m_pMainWnd = ZeGameWindow = NULL;
}
//---------------------------------------------------------------------------//

int CZeGameApp::Run() 
{
	
	MSG  msg;
	while (TRUE)
    {	if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( !GetMessage( &msg, NULL, 0, 0 ) )
            {
                return msg.wParam;
            }
            TranslateMessage(&msg);
             DispatchMessage(&msg);
        }
        else if ((ZeGameWindow->bWindowActive) && (bGameActive))
        {
            MainLoop();   
        }
        else
        {
            WaitMessage();
        }
    }

	return CWinApp::Run();
}


//=== Executé qd il n'y a plus de messages à lire ============================//
BOOL CZeGameApp::OnIdle(LONG lCount) 
{
	if ((ZeGameWindow->bWindowActive) && (bGameActive))
	{	
		MainLoop();   
	}
	
	return CWinApp::OnIdle(lCount);
}
//----------------------------------------------------------------------------//
