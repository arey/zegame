////////////////////////////////////////////////////////////////////////////////
//  Unit�	: ZeGameMain.cpp												  //
//	R�le	: Fonctions principales du jeu : initialisations, fermetures ...  //	 
////////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------- include
#include "ZeNain.h"				// Fichier commun � tout le projet
#include "consts.h"				// Constantes globales au projet
#include "DDLib.h"              // Fonctions DirectDraw
#include "D3DLib.h"             // Fonctions Direct3D
#include "DDLibAsm.h"           // Fonctions DirectDraw �crites en assembleur
#include "DILib.h"              // Librairie maison DirectInput
#include "DSLib.h"				// Librairie maison DirectSound	
#include "GestFile.h"           // V�rifie et Charge les fichiers n�cessaires
#include "Animall.h"            // Relit les *.ina
#include "PersoLib.h"           // D�finitions, Actions & Etats des personnages
#include "GameDebug.h"			// Debugger maison
#include "SpritesLib.h"			// Gestion des sprites "classiques"
#include "IDMap.h"     	        // Gestion des identificateurs du jeu
#include "PNJLib.h"             // S'occupe des PNJ
#include "MapLib.h"				// Structure d'une carte
#include "FPath.h"              // Algo de recherche de chemin (Path Finding)
#include "TimeClock.h"			// Gestion de l'horloge du jeu
#include "ScnScript.h"			// Gestion des scripts sc�naris�s
#include "GameObjectList.h"		// Liste de tous les GameObject du jeu
#include "ScnEvents.h"			// Gestionnaire d'�v�nements
#include "MemLib.h"				// Lib. regroupant divers canaux de communication entre modules
#include "FileManager.h"		// Lib. de gestion du fichier g�n�ral
#include "ImageFile.h"			// Diff�rents formats d'images
#include "FileVirtual.h"		// Fichiers virtuels (dans un .pak, en m�moire ou tout simplement sur le disk)
#include "ImageUtil.h"			// Routines maison pour charger les bitmaps dans des surfaces DirectDraw
#include "INILib.h"				// Lib. de gestion des fichiers .ini 
#include "ZoneManager.h"		// Gestionnaire des Zones au sol
#include "ScnGame.h"			// Fonctionnalit�s li�es au sc�nario
#include "SoundLib.h"			// Manager des bruitages du jeu
#pragma hdrstop
#include <direct.h>				// Manipulations de r�pertoires
#include "ZeGameWnd.h"			// Classe de la fen�tre du jeu
#include "ZeGame.h"				// Header du fichier principal du projet
#include "ZeGameMain.h"			// Header du fichier

// ----------------------------------------------------------------------- define
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_CHAR		   256				// Nb max de car. ds une cha�ne
#define TIMER_FRAMERATE		51				// ID du timer calculant le nb d'images/sec.

// -------------------------------------------------------------------- variables
LPVOID pMemApplication = NULL;				// Adresse m�moire r�serv�e
const int MemAppliReserved = 64000000;      // R�serve 64Mo de m�moire
BOOL	AJOUT_PNJ  = TRUE;					// Ajoute 2 Ogres autonomes au level
BOOL	DEMO_TOURNANTE = FALSE;				// Le H�ro devient un PNJ et se ballade sur la carte
ePeriodeMode PERIODE_MODE = PERIODE_JOUR;	// Gestion du passage du jour � la nuit
BOOL FFlippingEnabled;
BOOL Animating;

char	FrameRate[20];						// Cha�ne indiquant le frame-razte actuel
int		FrameCount = 0;		  				// A chaque fois qu'une image appara�t, ce compteur incr�mente
int		FrameDelay = 2000;					// temps (en ms) entre 2 prises de Frame Rate



// --------------------------------------------- fonctions impl�ment�es plus loin
void CALLBACK FrameRateTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);


/////////////////////////////////////////////////////////////////////////////////
// Fonctions Principales													   //	
/////////////////////////////////////////////////////////////////////////////////

//=== fonction Main() du jeu ==================================================//
void MainZeGame()
{	// Initialisations au d�marrage du jeu : chargement de librairies, directdraw ...
	InitPack_StartGame();

	// Chargement du niveau s�lectionn�
	InitPack_StartLevel();

	// Let's Go
	bGameActive = true;
}
//-----------------------------------------------------------------------------//

//=== Quitte proprement le jeu ================================================//
void CloseZeGame()
{	// On arr�te de faire tourner le jeu
	bGameActive = false;
	
	// Ferme le niveau en cours
	ClosePack_EndLevel();

	// Quitte l'application
	ClosePack_EndGame();
}
//-----------------------------------------------------------------------------//

//=== Initialisations au d�marrage du jeu =====================================//
void InitPack_StartGame()
{	
	// Mode Pause
	Animating = FALSE;

	// R�cup�re le Handle de la fen�tre principale de l'appli
	#ifdef CBUILDER
	hWndApplication = WindowGame->Handle;
	#elif VISUALC
	hWndApplication = AfxGetMainWnd()->GetSafeHwnd();
	#endif

	// Ouverture du fichier global au jeu
	InitGame_FileManager();
	
	// R�cup�re le chemin o� se trouve l'ex�cutable du jeu
	char pathExec[MAX_CHAR];
	_getcwd(pathExec, MAX_CHAR);
	strcat(pathExec, "\\");
	CheminApplication = pathExec;

	// Puis commence par lire de multiples param�tres de config ds ZeGame.ini
	InitPak_Ini();

	// Tiens compte des param�tres souhait�s par le joueur et obtenus
	// � partir de la bo�te de dialogue du d�marrage
    SCREEN_WIDTH   = CFG_SCREEN_WIDTH;
    SCREEN_HEIGHT  = CFG_SCREEN_HEIGHT;
    SCREEN_BITS    = CFG_SCREEN_BITS;
    AJOUT_PNJ      = CFG_AJOUT_PNJ;
    PERIODE_MODE   = CFG_PERIODE_MODE;
    DEMO_TOURNANTE = CFG_DEMO_TOURNANTE;

	// Initialisation du g�n�rateur de nombres al�atoires
	srand( (unsigned) time(NULL) );

	// Mise en route le Debugger maison
	ZeGameDebug = new GameDebug("debug.log");

	//--- Chargement des biblioth�ques d'ID ---
	#ifdef NEW_FILE_MANAGER
	AnsiString tempGamePath = CheminApplication; CheminApplication = "";
	#endif
	// 1.b]  membres du corps humain :
    if (IDMembres!=NULL) { delete (IDMembres); IDMembres=NULL; };
    IDMembres = new std::IDMap;
	IDMembres->LireFileID(CheminApplication+cIDPersosMembresFilename);
	// 1.c] frames bmp du jeu :
    if (IDBmpFrame!=NULL) { delete (IDBmpFrame); IDBmpFrame=NULL; };
    IDBmpFrame = new std::IDMap;
    IDBmpFrame->LireFileID(CheminApplication+cIDBmpFrameFilename);
    // 1.d] types d'objets (ex: epee1) servant dans la superposition d'anims
    if (IDTypeObjSprite!=NULL) { delete (IDTypeObjSprite); IDTypeObjSprite=NULL; };
    IDTypeObjSprite = new std::IDMap;
    IDTypeObjSprite->LireFileID(CheminApplication+cIDTypeObjSpriteFilename);
    IDPatronPersos = new std::IDMap;
    IDPatronPersos->LireFileID(CheminApplication+cPersosPatronFilename);
    // 1.e] Charge les ID d'animation
    IDAnimation = new std::IDMap;
    IDAnimation->LireFileID(CheminApplication+cIDAnimationFilename);
    // 1.f] Charge les ID des objets anim�s
    IDObjets = new std::IDMap;
    IDObjets->LireFileID(CheminApplication+cIDObjetsFilename);
    // 1.g] Identificateurs des diff�rents TILES
    if (IDTiles!=NULL) { delete (IDTiles); IDTiles=NULL; };
    IDTiles = new std::IDMap;
    IDTiles->LireFileID(CheminApplication+cIDTilesFilename);
	// 1.h] Identificateurs des M�moires des GameObjects
	if (IDMemory!=NULL) { delete (IDMemory); IDMemory=NULL; };
    IDMemory = new std::IDMap;
    IDMemory->LireFileID(CheminApplication+cIDMemoryFilename);
	// 1.i] IDentificateurs des Zones du Jeu
	if (IDMapZones!=NULL) { delete (IDMapZones); IDMapZones=NULL; };
    IDMapZones = new std::IDMap;
    IDMapZones->LireFileID(CheminApplication+cIDMapZonesFilename);
	// 1.j] IDentificateurs des Bruitages du jeu
	if (IDSoundEffect!=NULL) { delete (IDSoundEffect); IDSoundEffect=NULL; };
    IDSoundEffect = new std::IDMap;
    IDSoundEffect->LireFileID(CheminApplication+cIDSoundEffectFilename);

	#ifdef NEW_FILE_MANAGER
	CheminApplication = tempGamePath;
	#endif
	
	//--- Initialisations des librairies DirectX ---
	D3DS = new TD3DScreen(hWndApplication);		// Direct3D
    D3DS->SearchDevice(D3DLIB_ALPHABLENDING);
	DS = D3DS;									// DirectDraw
	D3DS->InitDDraw();
	D3DS->InitD3D();
    DI = new TDirectInput(hWndApplication);		// DirectInput
	DI->CreateDevice(true ,false ,true);
	InitGame_DirectSound(DS->DDHandle);			// DirectSound

	//--- Charge l'image de fond de chargement du jeu ---
	DS->LoadBitmap("loading.bmp");
	DS->FlipSurfaces();
	
	//--- Installe les effets sp�ciaux ---
    D3D_SE = new TD3DSpecialEffect(); // sur le P166 : 0x90000020 et dur le PII : 0x60000060
    D3D_SE->CreateFilter(0x60000060, 0x0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, D3DS->pD3DDevice);
    D3D_SE->SetState(TD3DSpecialEffect::HOUR_DAY);

	InitGame_GameScriptList();	// Chargement de la liste de scripts du jeu 

	//--- Cr�ation d'une horloge calculant toutes les FrameDelay ms le nb d'images/sec ---
	SetTimer(hWndApplication, NULL, FrameDelay, FrameRateTimerProc);
}
//-----------------------------------------------------------------------------//

//=== Initialisations au d�marrage d'un niveau du jeu =========================//
void InitPack_StartLevel()
{	
	//--- Initialise les fonctionnalit�s li�es au Sc�nario du jeu ---
	InitGame_GameScenario();
	
	//--- Init de la liste de tous les GameObject du Jeu ---
	InitGame_GameObjectList();
	
	//--- Liste officielle de la gestion des frames du niveau ---
	SFL = new SpriteFrameList(); 
	
	//--- Cr�ation puis chargement de la banque de sprites ne faisant ni partie des objets, ni des persos ---
	STGL = new SpriteTileGameLib();	
    STGL->LoadFileSpriteIna("AllSprites");

	//--- Charge les frames contenant les sprites des persos : ogre + h�ro ---
	LoadFrames();	

	//--- Ajoute ds le niveau le H�ro (autonome si c'est la d�mo tournante) ---
    if (DEMO_TOURNANTE)
    { Hero = new PersoPNJ(HERO1);
    } else
    { Hero = new Personnage(HERO1);
	}
	Hero->SetFixedID(0);	// HERO
	GOList->AddGameObject(Hero);
	LGP.AddPerso(Hero);

	//--- Ajoute ensuite 2 PNJ ---
	if (AJOUT_PNJ)
    { Personnage *pnj = new PersoPNJ(OGRE1);
      pnj->SetPosition(6,7);
	  pnj->SetScriptID(14);		// Assigne le script "OgreKiSeBallade" � cet ogre
      LGP.AddPerso(pnj);
	  GOList->AddGameObject(pnj);
	  pnj = new PersoPNJ(OGRE1);
      pnj->SetPosition(16,5);
	  pnj->SetScriptID(14);		// OgreKiSeBallade
      LGP.AddPerso(pnj);
	  GOList->AddGameObject(pnj);
    }

	
	//--- [ACF] Modifications suivant le Niveau ---
	if (LevelName == "New.niv")
	{	Hero->SetCoords(32,36);
	}

	//--- Charge les donn�es de la carte du niveau ---
    LoadMapNiveau(MapNiveau, CheminApplication + cPathLevels + LevelName);
    ViewMap->OldScreenX = ViewMap->ScreenX = Hero->peX - ViewMap->ScreenViewX/2;
    ViewMap->OldScreenY = ViewMap->ScreenY = Hero->peY - ViewMap->ScreenViewY/2;

	//--- Initialise l'algo de Recherche de chemin pour ce niveau l� ---
    TMap *pMap = MapNiveau->PrincEtage->LayerDecors;
    // Rmq : Pour le Layer du Sol, on aurait pu prendre pMap = Etage->LayerSols;
    MapInitRechercheChemin(*pMap);

	//--- Initialisation de l'EventsManager	---
	InitGame_EventsManager();

	//--- Initialisation du gestionnaire des Zones au sol ---
	InitPack_ZoneManager(MapNiveau->pMapZoneList);
	
	//--- Initialisation des canaux de communications ---
	InitGame_MemLib();
	
	//--- D�marre la gestion du Timer/Flipping ---
    FlipRefresh = new TRefresh();
    if (REFRESH_MODE == REFRESH_FIXED) FlipRefresh->RefreshFixed = TRUE;
    FlipRefresh->NewFps(REFRESH_FPS);	// ch'tits calculs

	//--- D�marre l'horloge ---
    InitPack_GameHistory();	
}
//-----------------------------------------------------------------------------//

//=== D�sallocations avant de quitter l'application ===========================//
void ClosePack_EndGame()
{	
	//--- Lib�ration de l'horloge calculant le nb d'images/sec ---
	KillTimer(hWndApplication, TIMER_FRAMERATE);
	
	//--- Supprime le gestionnaire d'Effets Sp�ciaux ---
	delete D3D_SE; D3D_SE = NULL;	
	
	//--- D�sallocation des librairies DirectX ---
	// a] DirectSound
	CloseGame_DirectSound();
	// b] Gestion du clavier/joypad/souris par DirectInput
	delete DI; DI = NULL;
	// c] Objet directX principal g�rant DirectDraw/Direct3D
	delete D3DS; D3DS = NULL;	

	CloseGame_GameScriptList();		// Fermeture des scripts du jeu
	
	//--- D�sallocation des biblioth�ques d'ID ---
	// a] Identificateurs des diff�rents TILES
    delete IDTiles;	IDTiles = NULL;
	// b] ID des objets anim�s
    delete IDObjets; IDObjets = NULL;
	// c] ID des animations
	delete IDAnimation; IDAnimation = NULL;
	// d] ID des patrons de personnages
	delete IDPatronPersos; IDPatronPersos = NULL;
	// e] ID des types d'objets
	delete IDTypeObjSprite; IDTypeObjSprite = NULL;
	// f] ID des frames bmp du jeu
	delete IDBmpFrame; IDBmpFrame = NULL;
	// g]  ID des membres du corps humain
	delete IDMembres; IDMembres = NULL;
	// h] ID des M�moires des GameObjects
	delete IDMemory;  IDMemory=NULL;
	// i] ID des zones au sol
	delete IDMapZones;	IDMapZones = NULL;
	// j] ID des bruitages du jeu
	delete IDSoundEffect;	IDSoundEffect = NULL;
    
	// Lib�ration du Debugger maison
	delete ZeGameDebug; ZeGameDebug = NULL;

	// Fermeture du fichier global au jeu
	CloseGame_FileManager();
}
//-----------------------------------------------------------------------------//

//=== D�sallocations lors de la fin d'un niveau ===============================//
void ClosePack_EndLevel()
{	
	//--- Met fin � l'horloge ---
    DisablePack_GameHistory();
	
	//--- Supprime le Timer/Flipping ---
    delete FlipRefresh; FlipRefresh = NULL;

	//--- Destruction des canaux de communications ---
	CloseGame_MemLib();

	//--- Destruction du gestionnaire des Zones au sol ---
	DisablePack_ZoneManager();
	
	//--- Ferme en souplesse l'EventsManager ---
	CloseGame_EventsManager();		

	//--- Supprime le gestionnaire de pathfinding ---
	MapCloseRechercheChemin();
	
	//--- D�charge le niveau ---
	UnLoadMapNiveau(MapNiveau);

	//--- Supprime tous les PNJ du niveau ---
	LGP.Vide();

	//--- D�charge les mod�les de persos ---
	GestFile_UnloadFrames();

	//--- On vide la liste des sprites ne faisant en particulier ni partie des objets, ni des persos ---
	delete STGL; STGL = NULL;

	//--- On vide la liste des SpriteFrames du niveau ---
	delete SFL;	SFL = NULL;

	//--- On vide la liste de tous les GameObject du Jeu ---
	CloseGame_GameObjectList();

	//--- D�saloue toutes fonctionnalit�s li�es au Sc�nario du jeu ---
	CloseGame_GameScenario();
}
//-----------------------------------------------------------------------------//

//=== Boucle principale du jeu ================================================//
void MainLoop()
{	
	//--- Lecture et analyse du clavier ---
	DI->AnalyseKeys();
    SpecialKeys();
	if (!bGameActive) return;	// Si le joueur a quitt�

	// MaJ des SoundBuffer de la carte audio
	DSMuzikLib->MuzikManage();

	if (bGamePause) return;		// Pause : pas la peine d'aller plus loin !

    // MaJ de l'heure du jeu, des conditions climatiques, saisonni�res ...
    UpdatePack_GameHistory();

	// MaJ du gestionnaire des zones au sol
	UpdatePack_ZoneManager();

	// Gestionnaire des scripts sc�naritiques de chaque GameObject
	GOList->ScenarioManager();

    //--- MaJ du H�ros ---
	if (bPerso) 
	{	Hero->Update(); 
	} else 
	{	Ogre->Update();
	}

    //--- MaJ de tous les personnages nons joueurs ---
	LGP.Update();

	//--- MaJ des objets anim�s ---
    DOLib->Update();

    //--- Calcul de ce qui se trouve ds le champs de vision de la cam�ra ---
	ViewMap->Upgrade();
	ViewMap->AfficheViewCamera();
	if (NIGHT_FILTER) NightFilter();	// Filtre de nuit

    //--- Effets sp�ciaux ---
    if (PERIODE_MODE == PERIODE_JOUR_NUIT)
    { D3D_SE->UpdateSE();
      D3DS->InitRenderScene3D();
      D3D_SE->RenderFilter(true);
      D3DS->CloseRenderScene3D();
    }

    //--- S'occuppe du Frame Rate ---
    if (FRAME_RATE)
    {	DS->pDDSBack->TextSurf(20,10,0xFF00,FrameRate);
    }
    
	FrameCount++;	// une frame de plus affich�e

    //--- Aide graphique ---
	if (FONCTION_HELP) AfficheHelp();                     // Affichage de l'aide si activ�e
    if (FONCTION_DEBUG_HERO)  DEBUGPANEL_PERSO(Hero);     // Affichage du panneau de debuggage si activ�

    //--- Si le nb de fps est limit�, on attend ---
    if (FlipRefresh->RefreshFixed == TRUE)
      while (!FlipRefresh->FlipFrame()) {  };

    //--- Basculement des surfaces primaires et secondaires ---
	DS->FlipSurfaces();
}
//-----------------------------------------------------------------------------//

//=== Etude des touches sp�ciales =============================================//
void SpecialKeys()
{
    if (Key_ESCAPE) 
	{ 
		#ifdef CBUILDER
		WindowGame->Close(); Application->Terminate(); 
		#else
		bGameActive = false;
		SendMessage(hWndApplication, WM_CLOSE, 0, 0);		
		#endif
		return;
	} 

	if (Key_PAUSE==1)
	{	bGamePause = !bGamePause;
		GamePaused();
		if (bGamePause) return;
	}

	if ((Key_LALT==2) && (Key_ENTER==1))
	{	// Permutation de mode �cran : FullScreen <-> Windowed-mode
		#ifdef VISUALC
		PostMessage(hWndApplication, WM_TOGGLEFULLSCREEN, 0, 0);
		return;
		#endif
	}
	else
	if (Key_ENTER==1)
       {     // transmutation
        if (!bPerso) { Hero->peX = Ogre->peX; Hero->peY = Ogre->peY; }
        else         { Ogre->peX = Hero->peX; Ogre->peY = Hero->peY; };
    	bPerso=!bPerso;
        ViewMap->NewCamera = true;   // Repositionne la cam�ra sur le personnage
       };
    if (Key_F2==1) FRAME_RATE = !FRAME_RATE;             // Affichage des frames rate
    if (Key_F1==1)  FONCTION_HELP = !FONCTION_HELP;	  // Affichage de l'aide
    if (Key_F3==1)										 // chgt de mode de rafraichissement
      if (REFRESH_MODE == REFRESH_FIXED)   // fix�
         { REFRESH_MODE = REFRESH_MAX;
           FlipRefresh->RefreshFixed = FALSE;
         }
      else								  // rafale
		{  REFRESH_MODE = REFRESH_FIXED;
           FlipRefresh->RefreshFixed = TRUE;
         }
    if (Key_F4==1)										// Affichage des layers modifi�
       switch (AFFICH_MODE)
        {
          case (LAYER_ALL)    : AFFICH_MODE = LAYER_SOL;    break;
          case (LAYER_SOL)    : AFFICH_MODE = LAYER_DECORS; break;
          case (LAYER_DECORS) : AFFICH_MODE = LAYER_ALL;    break;
        };
    
	#ifdef DEBUG	// Bloqu� dans la release
	if (Key_F5==1)										// Affichage diff�rent du sol
       switch (BLIT_MODE)
        {
          case (RECALCULE)	     : BLIT_MODE = DOUBLE_BUFFERING_SM; break;
          case (DOUBLE_BUFFERING_SM) : BLIT_MODE = CALCUL_TAB; 	  	break;
          case (CALCUL_TAB) 	     : BLIT_MODE = RECALCULE; 	  	break;
        };
    ViewMap->BlitMode = BLIT_MODE;
	#endif

	#ifdef DEBUG	// Bloqu� dans la release
	if (Key_F6==1)
       switch (COLLIDE_MODE)
       {  case (COLLIDE_PIXEL) : COLLIDE_MODE = COLLIDE_ZONE;  break;
          case (COLLIDE_ZONE)  : COLLIDE_MODE = COLLIDE_PIXEL; break;
       }
	#endif

    if (Key_F7 == EVENTKEY_APPUYE)
       switch (CONTROL_MODE)
       {  case (CONTROL_CLAVIER)     :
          {   DI->ReActive(false, false, true);
              CONTROL_MODE = CONTROL_CLAVIER_JOY;
          } break;
          case (CONTROL_CLAVIER_JOY) :
          {    DI->UnActive(false, false, true);
               CONTROL_MODE = CONTROL_CLAVIER;
          } break;
       };

    if (Key_F8 == EVENTKEY_APPUYE)
       switch (PERIODE_MODE)
       {  case (PERIODE_JOUR)       : PERIODE_MODE = PERIODE_JOUR_NUIT;  break;
          case (PERIODE_JOUR_NUIT)  : PERIODE_MODE = PERIODE_JOUR; break;
       };
    if (Key_F12 == EVENTKEY_APPUYE) FONCTION_DEBUG_HERO = !FONCTION_DEBUG_HERO;       // Panneau de debuggage

    bool GamaChange = false;
    if (Key_BACKSPACE) { DS->RedCoeff = DS->GreenCoeff = DS->BlueCoeff = 1.0; GamaChange = true; } // R�initialise le tout
    if (Key_INSERT) { DS->RedCoeff -= 0.01; GamaChange = true; };
    if (Key_DELETE) { DS->RedCoeff += 0.01; GamaChange = true; };
    if (Key_PAGEUP)   { DS->GreenCoeff -= 0.01; GamaChange = true; };
    if (Key_PAGEDOWN) { DS->GreenCoeff += 0.01; GamaChange = true; };
    if (Key_HOME)     { DS->BlueCoeff -= 0.01; GamaChange = true; };
    if (Key_END)      { DS->BlueCoeff += 0.01; GamaChange = true; };

    // Change la table du GammaControl
    if (GamaChange)
      { DS->SetGammaRamp(DS->RedCoeff,DS->GreenCoeff,DS->BlueCoeff);
        DS->ChangeGammaRamp();
      }

    // Augmente/diminue le nb de frames / sec
    if (Key_PLUS==1)  { REFRESH_FPS++; }
    if (Key_MOINS==1) { REFRESH_FPS--;}
    if (REFRESH_FPS<5) REFRESH_FPS = 5;
    FlipRefresh->NewFps(REFRESH_FPS);

    // Change d'arme
    if (Key_1)
    {   if (bPerso) { // D�salloue la m�moire utilis�e par l'ancienne arme
                      Hero->ModelePerso->DesallocateObjetBmp(Hero->ArmeEnMain);
                      // Charge les bmp concernant la nouvelle arme
                      Hero->ModelePerso->AllocateObjetBmp(NOWEAPON);
                      Hero->ArmeEnMain = NOWEAPON; }
        else        { // D�salloue la m�moire utilis�e par l'ancienne arme
                      Ogre->ModelePerso->DesallocateObjetBmp(Ogre->ArmeEnMain);
                      // Charge les bmp concernant la nouvelle arme
                      Ogre->ModelePerso->AllocateObjetBmp(NOWEAPON);
                      Ogre->ArmeEnMain = NOWEAPON; }
    } else
    if (Key_2)
    {   if (bPerso) { // D�salloue la m�moire utilis�e par l'ancienne arme
                      Hero->ModelePerso->DesallocateObjetBmp(Hero->ArmeEnMain);
                      // Charge les bmp concernant la nouvelle arme
                      Hero->ModelePerso->AllocateObjetBmp(EPEE1);
                      Hero->ArmeEnMain = EPEE1; }
        else        { // D�salloue la m�moire utilis�e par l'ancienne arme
                      Ogre->ModelePerso->DesallocateObjetBmp(Ogre->ArmeEnMain);
                      // Charge les bmp concernant la nouvelle arme
                      Ogre->ModelePerso->AllocateObjetBmp(EPEE1);
                      Ogre->ArmeEnMain = EPEE1; }
    } else
	if (Key_3)
    {   if (bPerso) { // D�salloue la m�moire utilis�e par l'ancienne arme
                      Hero->ModelePerso->DesallocateObjetBmp(Hero->ArmeEnMain);
                      // Charge les bmp concernant la nouvelle arme
                      Hero->ModelePerso->AllocateObjetBmp(PETOIRE1);
                      Hero->ArmeEnMain = PETOIRE1; }
        else        { // D�salloue la m�moire utilis�e par l'ancienne arme
                      Ogre->ModelePerso->DesallocateObjetBmp(Ogre->ArmeEnMain);
                      // Charge les bmp concernant la nouvelle arme
                      Ogre->ModelePerso->AllocateObjetBmp(PETOIRE1);
                      Ogre->ArmeEnMain = PETOIRE1; }
    }

}
//----------------------------------------------------------------------------//

//== Affichage de l'aide =====================================================//
void AfficheHelp()
{
	// autre texte
    int sig = 10;
	#ifdef DEBUG
    char   RefreshText[20];			// texte
	gcvt( GameHistory->GetTime(), sig, RefreshText);
	DS->pDDSBack->TextSurf(15, 25, RGB24(200,200,5),RefreshText);
	#endif
    DS->pDDSBack->TextSurf(15, 40, RGB24(200,200,5),"F1       active/d�sactive l'Aide");
    DS->pDDSBack->TextSurf(15, 60, RGB24(200,200,5),"F2       montre le Frame Rate");
    DS->pDDSBack->TextSurf(15, 80, RGB24(200,200,5),"F3       change le mode rafraichissement : fix� ou rafale");
    DS->pDDSBack->TextSurf(15,100, RGB24(200,200,5),"F4       affichage les layers");
    DS->pDDSBack->TextSurf(15,120, RGB24(200,200,5),"F5       affichage du sol complet/partiel (bloqu�)");
    DS->pDDSBack->TextSurf(15,140, RGB24(200,200,5),"+/-      change le fps");
    DS->pDDSBack->TextSurf(15,165, RGB24(200,200,5),"ESCAPE   pour sortir de la d�mo");
    DS->pDDSBack->TextSurf(15,355, RGB24(200,200,5),"Inser, Suppr, Fin ...   changer le Gamma du moniteur");
    DS->pDDSBack->TextSurf(15,375, RGB24(200,200,5),"F6       change le mode de collisions (bloqu�)");
    DS->pDDSBack->TextSurf(15,395, RGB24(200,200,5),"F7       active/d�sactive le joypad");
    DS->pDDSBack->TextSurf(15,415, RGB24(200,200,5),"F8       active/d�sactive le passage du jour � la nuit");

    DS->pDDSBack->TextSurf(15,185, RGB24(5,5,200),"<- ->                         de d�placer");
    DS->pDDSBack->TextSurf(15,205, RGB24(5,5,200),"Space / Bouton B    utiliser/fouiller un objet");
    DS->pDDSBack->TextSurf(15,225, RGB24(5,5,200),"LCtrl / Bouton A       frapper");
    DS->pDDSBack->TextSurf(15,245, RGB24(5,5,200),"K (kill)                      crever");
    DS->pDDSBack->TextSurf(15,265, RGB24(5,5,200),"LShift / Bouton C     courir");
	DS->pDDSBack->TextSurf(15,285, RGB24(5,5,200),"W                            sauter");
    DS->pDDSBack->TextSurf(15,305, RGB24(5,5,200),"LAlt / Bouton D        changer d'arme");
	DS->pDDSBack->TextSurf(15,325, RGB24(5,5,200),"1,2 ou 3                   s�lectionner arme");    
} 
//----------------------------------------------------------------------------//

//=== Appel�e ttes les FrameDelay ms afin de calculer le frame rate ==========//
void CALLBACK FrameRateTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	 // Calcule le nombre de frames par seconde
    double FrRate = ((double) (FrameCount*1000)) / (double)FrameDelay;
    FrameCount = 0;
    int sig = 6;
    gcvt( FrRate, sig, FrameRate);
}
//----------------------------------------------------------------------------//

//=== Changementt de mode vid�o ==============================================//
void ToggleFullScreenMode()
{	
	bGamePause = true;
	GamePaused();
	
	// R�initialise le mode-vid�o
	DS->ToggleFullScreenMode();

	// Restaure toutes les surfaces DirectDraw du jeu 
	// -> cr�e les surfaces et recharge les bitmaps si il le faut !
	//  a) de tous les banques de sprites SpriteFrame du jeu
	SFL->ChangeSpriteFrameVideoMode(&DS->pDD);
	//	b) de tous les tiles des d�cors du jeu
	DULib->ChangeTileVideoMode();
	//	c) des surfaces de pr�-calcul des d�cors au sol
	SolScroll->ChangeSolScrollVideoMode(DS->Screen_Width, DS->Screen_Height);	
	ViewMap->ChangeSolScrollVideoMode(DS->Screen_Width, DS->Screen_Height);	
	//  d) des anims superpos�es des personnages et des d�cors
	ChangeListeCharactersVideoMode();
	ChangeAnimTileDecorsVideoMode();

	// Changement de device3D de certaines instances
	//  - � tous les Sprite3DTile d�j� allou�s
	if (STGL != NULL)
	{	STGL->ChangeD3DDevice(D3DS->pD3DDevice);
	}
	//  - aux effets sp�ciaux
	if (D3D_SE != NULL)
	{	D3D_SE->SetDevice3D(D3DS->pD3DDevice);
	}

	bGamePause = false;
	GamePaused();
}
//----------------------------------------------------------------------------//

//=== Pause en plein cours du jeu ============================================//
void GamePaused()
{	if (DSMuzikLib != NULL)	DSMuzikLib->SetPause(bGamePause);
	
	// Synchronise l'horloge du jeu avec l'horloge syst�me apr�s une pause
	if ((!bGamePause) && (GameHistory != NULL))
	{	GameHistory->SynchronizeClockAfterPause();
	}
}
//----------------------------------------------------------------------------//

//=== r�cup�re les param�tres de config ds zegame.ini ========================//
void InitPak_Ini()
{	char options[512];
	
	// R�cup�re les fichiers wav d'ambiance musicale
	TINILib	ini((char*) cIniZeGame);
	
	// 1) Ecran en FullScreen oui ou non ?
	ini.GetStringProfile("Options", "Windowed", options);
	SCREEN_FULLSCREEN = (strcmp(options,"Yes") != 0);

	// 2) Optimis� Direct3D
	ini.GetStringProfile("Options", "Software3D", options);
	SCREEN_DIRECT3D = (strcmp(options,"Yes") != 0);

	// 3) D�mo jouable ?
	ini.GetStringProfile("Options", "Release", options);
	CFG_GAME_RELEASE = (strcmp(options,"Yes") == 0);
}
//----------------------------------------------------------------------------/