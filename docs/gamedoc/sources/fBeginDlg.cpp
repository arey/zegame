////////////////////////////////////////////////////////////////////////////////
//  Unité	 : fBeginDlg.cpp												  //
//	Rôle	 : fiche de configuration lancée au démarrage du jeu			  //	
//	Remarque : même rôle que l'unité fBegin sous C++Builder					  //	
////////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------- include
#include "ZeNain.h"				// Fichier commun à tout le projet
#include "consts.h"				// Variables globales au projet à remplir à partir de cette fiche
#include "ZeGame.h"				// Header du fichier principal du projet
#include "INILib.h"				// Lib. de gestion des fichiers .ini 
#include <direct.h>				// Manipulations de répertoires
#include "fBeginDlg.h"			// Header de la classe

// ---------------------------------------------------------------------- define
/*#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/


/////////////////////////////////////////////////////////////////////////////
// Classe fBeginDlg														   //
/////////////////////////////////////////////////////////////////////////////

//=== Constructeur ========================================================//
fBeginDlg::fBeginDlg(CWnd* pParent)
	: CDialog(fBeginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(fBeginDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// [BUG] Win2000
	#ifndef WIN2000
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	#endif  // WIN2000
	bLetsGo = false;
}
//-------------------------------------------------------------------------//

//=== Initialisations de la fiche modale =================================//
void fBeginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(fBeginDlg)
	DDX_Control(pDX, IDC_CHECK_JOURNUIT, m_CBPeriodeMode);
	DDX_Control(pDX, IDC_CHECK_DEMO, m_CBDemoTournante);
	DDX_Control(pDX, IDC_CHECK_AJOUTPNJ, m_CBPnj);
	DDX_Control(pDX, IDC_EDIT_LEVEL_NAME, m_LevelNameEdit);
	DDX_Control(pDX, IDC_COMBO_RESOLUTION, m_ResolutionCB);
	//}}AFX_DATA_MAP

	SetIcon(m_hIcon, TRUE);			// Set big icon

	//--- Initialisations des paramètres de jeu par défaut ---

	// Récupère quand même quelques paramètres de configuration dans ZeGame.ini
	char options[512];
	TINILib	ini((char*) cIniZeGame);
	//   a) résolution du jeu (résolution du jeu par défaut 640x480x16)
	ini.GetStringProfile("Options", "Resolution", options);
	int resolution = GetResolution(options);
    m_ResolutionCB.SetCurSel(resolution);

	// Niveau par défaut
	options[0] = '\0';
	ini.GetStringProfile("Game", "DefaultLevel", options);
	LevelName = options;
	if (LevelName == "")
	{   LevelName = "big.niv";
	} else
	
	m_LevelNameEdit.SetWindowText(LevelName);

    // Récupère le chemin de l'exécutable
	char pathExec[128];
	_getcwd(pathExec, 128);
	strcat(pathExec, "\\");
	CheminApplication_deb = pathExec;

	// Ajout des 2 Ogres
	m_CBPnj.SetCheck(1);
}
//-------------------------------------------------------------------------//


BEGIN_MESSAGE_MAP(fBeginDlg, CDialog)
	//{{AFX_MSG_MAP(fBeginDlg)
	ON_BN_CLICKED(ID_LETSGO, OnLetsgo)
	ON_BN_CLICKED(IDC_BUTTON_CHARGER, OnButtonCharger)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// fBeginDlg message handlers


//=== Lancement du jeu ====================================================//
void fBeginDlg::OnLetsgo() 
{	
	// Vérifie si un nom de fichier est bien rentré en paramètre
    if ( LevelName == "")	// Prévention ...
    {	MessageBox("Aucun fichier choisi","Warning !",0); 
		return; 
	}
	
    // Passe les paramètres de configuration de résolution au jeu
    switch (m_ResolutionCB.GetCurSel())
    {
        case (0) : // 640x480x16
           { CFG_SCREEN_WIDTH  = 640;CFG_SCREEN_HEIGHT = 480; CFG_SCREEN_BITS = 16; }; break;
        case (1) : // 800x600x16
           { CFG_SCREEN_WIDTH  = 800;CFG_SCREEN_HEIGHT = 600; CFG_SCREEN_BITS = 16; }; break;
        case (2) : // 1024x768x16
           { CFG_SCREEN_WIDTH  = 1024;CFG_SCREEN_HEIGHT = 768; CFG_SCREEN_BITS = 16; }; break;
        case (3) : // 1280x1024x16
           { CFG_SCREEN_WIDTH  = 1280;CFG_SCREEN_HEIGHT = 1024; CFG_SCREEN_BITS = 16; }; break;
        case (4) : // 1600x1200x16
           { CFG_SCREEN_WIDTH  = 1600;CFG_SCREEN_HEIGHT = 1280; CFG_SCREEN_BITS = 16; }; break;
    }


    // Etat du ComboBox du passage du jour à la nuit
    if (m_CBPeriodeMode.GetCheck()) CFG_PERIODE_MODE = PERIODE_JOUR_NUIT;
    else							CFG_PERIODE_MODE = PERIODE_JOUR;

    // Etat du ComboBox rajoutant 2 PNJ
    if (m_CBPnj.GetCheck()) CFG_AJOUT_PNJ = true;
    else					CFG_AJOUT_PNJ = false;

    // Etat du ComboBox de la démo tournante où le héro se déplace tout seul 
    CFG_DEMO_TOURNANTE = m_CBDemoTournante.GetCheck() == 1;

	// Ferme la boîte de dialogue
	bLetsGo = true;	// on peut lancer le jeu
	EndDialog(0);
}
//-------------------------------------------------------------------------//

//=== Ouvre un fichier de niveau ==========================================//
void fBeginDlg::OnButtonCharger() 
{	
	CString pathNiv = CheminApplication_deb + cPathLevels;
	CFileDialog fileDlg(TRUE, "*.niv","" , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						"Level files (*.niv)| *.niv | All files (*.*)|*.*||");
	fileDlg.m_ofn.lpstrInitialDir = pathNiv;
	fileDlg.m_ofn.lpstrTitle = "Niveau à charger";
	fileDlg.DoModal();
	LevelName = fileDlg.GetFileName();
	m_LevelNameEdit.SetWindowText(LevelName);
	// Revient au chemin de l'exécutable
	_chdir(LPCSTR(CheminApplication_deb));
}
//-------------------------------------------------------------------------//

//=== Renvoi la résolution ================================================//
int fBeginDlg::GetResolution(char* res)
{	if (strcmp(res, "640x480x16") == 0 ) return 0;
	else if (strcmp(res, "800x600x16") == 0 ) return 1;
	else if (strcmp(res, "1024x768x16") == 0 ) return 2;
	else if (strcmp(res, "1280x1024x16") == 0 ) return 3;
	else if (strcmp(res, "1600x1200x16") == 0 ) return 4;
	return 0;	// résolution par défaut 640x480x16
}
//-------------------------------------------------------------------------//


//////////////////// Fin de la classe fBeginDlg /////////////////////////////



