// ZeGame.cpp : Defines the class behaviors for the application.
//

// --------------------------------------------------------------------- include
#include "ZeNain.h"				// Fichier commun à tout le projet
#include "DDLib.h"				// Librairie maison de routines DirectDraw
#include "consts.h"				// Constantes communes au projet
#include "ZeGameWnd.h"			// Fenêtre principale de l'appli
#include "ZeGameMain.h"			// Main du jeu
#include "fBeginDlg.h"			// Boîte de dialogue du démarrage
#include "dxutil.h"				// Acces aux registres windows
#include <direct.h>				// Manipulations de répertoires
#include "ZeGame.h"				// Header de la classe

// ---------------------------------------------------------------------- define
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern GUID g_guidZeGame;


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
	
	// On commence par lire de multiples paramètres de config ds ZeGame.ini
	InitPak_Ini();

	// On lit aussi les informations presentes dans le Registre Windows
	RegCreateKeyEx( HKEY_CURRENT_USER, ZEGAME_KEY, 0, NULL,
                    REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
                    &g_hZegameRegKey, NULL );
	
	DXUtil_WriteGuidRegKey(g_hZegameRegKey,TEXT("Guid"), g_guidZeGame);
	char temp[1024];
	_getcwd(temp, 1000);
	DXUtil_WriteStringRegKey( g_hZegameRegKey, TEXT("Path"), temp);
	DXUtil_ReadStringRegKey(g_hZegameRegKey, TEXT("Player Name"),temp, MAX_PATH, TEXT("unknown"));
	g_sPlayerName = temp;
	DXUtil_ReadBoolRegKey(g_hZegameRegKey, TEXT("Multiplayer"), (int*) &g_bPlayOnline, FALSE);
	
	//--- Boîte de dialogue de démarrage du jeu ---
	fBeginDlg begDlg;
	if (begDlg.LetsShow())
	{	begDlg.DoModal();
		if (!begDlg.LetsGo()) return FALSE;
	}

	DXUtil_WriteStringRegKey( g_hZegameRegKey, TEXT("Player Name"),(char*) (LPCSTR) g_sPlayerName);
	DXUtil_WriteBoolRegKey( g_hZegameRegKey, TEXT("Multiplayer"), g_bPlayOnline);
	
	//--- Création de la fenêtre du jeu ---
	ZeGameWindow = new ZeGameWnd();
	int sizeX = CFG_SCREEN_WIDTH  + GetSystemMetrics(SM_CXSIZEFRAME)*2;
	int sizeY = CFG_SCREEN_HEIGHT + GetSystemMetrics(SM_CYSIZEFRAME)*2 + GetSystemMetrics(SM_CYMENU);
	ZeGameWindow->CreateEx(
			0, 
			AfxRegisterWndClass(CS_DBLCLKS), 
			" ZeGame - alpha 1.75", 
			WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 
			0, 0, 
			sizeX, sizeY,
			NULL, 
			0);	
	
	ZeGameWindow->CenterWindow();
	m_pMainWnd = ZeGameWindow ;

	// Ch'tite tête de nain en icone
	HICON m_hIcon = LoadIcon(MAKEINTRESOURCE(IDI_ZEGAME));
	SendMessage(ZeGameWindow->m_hWnd, WM_SETICON, ICON_BIG,   (LPARAM) m_hIcon );  // Set big icon
    SendMessage(ZeGameWindow->m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM) m_hIcon );  // Set small icon
	// Ca plante sous Win2000 : ZeGameWindow->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), true);
		
	SetCursor(NULL);	// Cache le curseur souris

	//--- Charge et lance le jeu ---
	MainZeGame();
	
	SetCursor(NULL);	// Recache le curseur souris (encore une fois :-)

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
