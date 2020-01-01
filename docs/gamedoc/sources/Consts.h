////////////////////////////////////////////////////////////////////////////////
// Constantes Globales 		 												  //
////////////////////////////////////////////////////////////////////////////////

#ifndef Consts
#define Consts

//---------------------------------------------------------------------- include
#ifdef BORLAND
#include <vcl\vcl.h>		// AnsiString
#elif VISUALC
//#include <afx.h>			// CString
#endif

#include "IDMap.h"



//----------------------------------------------------------------------- typedef
typedef unsigned int uint;


// Données géométriques d'un trapèze
const int ISO_HEIGHT 		= 21-1,
	  ISO_WIDTH  		= 39-1,
    	  ISO_HEIGHT_HALF = 10,
    	  ISO_WIDTH_HALF  = 19,
          ISO_DENOMINATEUR = 2 * ISO_HEIGHT_HALF * ISO_WIDTH_HALF;

// Données sur les tailles des différentes fenêtres de l'éditeur
extern int ScreenViewX,    // fenêtre représentant la vue du jeu
           ScreenViewY;

const int EditorWidth  = 1024,	    // résolution ds laquelle doit être lancé l'éditeur
	  EditorHeight =  768,
          EditorBits   =   16;


const int SurfaceMaxWidth = 640;	// largeur maximale d'un bitmap

// Pointeur de la souris sur la grille
const int Pointeur_NOTHING      = 0,
	  Pointeur_PLACERDECORS = 1,
          Pointeur_SELECTDECORS = 2,
          Pointeur_PEINDRESOLS  = 3,
          Pointeur_GOMMESOLS    = 4,
      	// - Pour le pathfinding
          Pointeur_SELECTPOINT  = 5,
    	// - Pour le game-design
          Pointeur_PLACERZONE   = 6,
          Pointeur_SELECTZONE   = 7,
          Pointeur_SELECTOBJECT = 8;


// Type de tracé du trapèze
const int TrapTYPE_SIMPLE  = 0,
	  TrapTYPE_DOUBLE  = 1;

// Type énuméré des Points cardinaux :
//  - Direction sur l'écran  : vue 3D ISO
typedef enum {View_O=0, View_SO, View_S, View_SE, View_E, View_NE, View_N, View_NO } eScreenDirection;
//  - Direction sur la carte : vue de haut
typedef enum {Map_SO=0, Map_S, Map_SE, Map_E, Map_NE, Map_N, Map_NO, Map_O } eMapDirection;
// => Il y'a une correspondance entre ces points : lorsque le personnage se deplace
//    vers le Nord dans la vue ISO, il se deplace vers le Nord-Ouest sur la carte.

// Type de Librairie sélectionnée
const int Lib_NULL 	 = 0,
	  Lib_DECORS = 1,
	  Lib_SOLS   = 2,
      Lib_PERSOS = 3,
      Lib_EVENTS = 4,
      Lib_DESIGN = 5;



// Paramètres de configuration des couleurs, polices ... du look, quoi !
const int ConfigTonio  = 1,
	  ConfigNormal = 2;

#ifdef MAPEDITOR
const TColor ColorBoxTonio   = clInactiveCaption,
		     ColorBoxNormal  = clWhite,
             ColorBoxTonio2  = clActiveCaption,
             ColorBoxNormal2 = clAqua;
#elif VISUALC
extern CString CheminApplication;
#endif

// Données utilisées ds "ImDecors2.h" => Lib universelle & co
const int cNbTypeUnivers = 2,			// Nb d'univers : grottes, plaines, villes ...
          cNbTypeTiles = 7;			// Nb de Sols & Décors ds l'univers

extern const char cUniversName [5][20];

extern const char cTypeTileName [7][20];

const int cIndexSols   = 0,       // Index & Nombre de Décors, Sols ...
	  cNbTilesSols = 3;
const int cIndexDecors   = 3,
	  cNbTilesDecors = 3;

// Données utilisées ds DecorsObjet de "ImDecors2.h"
const int Etage_PRINCIPAL = 1;
const int Layer_SOLS	  = 1,
	  Layer_DECORS	  = 2;

// Répertoire où se trouvent :
extern const AnsiString cPathTiles;
extern const AnsiString cPathLevels;
extern const AnsiString cPathPersos;
extern const AnsiString cPathObjets;	// - les fichiers d'amis des objets et des décors du jeu
extern const char cPathSounds[];		// - les sons & les musiques au format .wav
extern const char cPathMuzik[];			// - les musiques au format .wav

// Repertoire des suites d'images *.bmp à convertir en banque
extern const AnsiString cPathTemp;


// Nom et chemin des fichier contenant les identificateurs *.id
extern const AnsiString cPersosPatronFilename;	  // fichier contenant les id des patrons des persos
extern const AnsiString cIDAnimationFilename;     // fichier contenant les id des types d'animations
extern const AnsiString cIDPersosTypeFilename;    // fichier contenant les id des types de personnages
extern const AnsiString cIDPersosMembresFilename; // fichier contenant les id correspondants aux mebres d'un être humain
extern const AnsiString cIDTypeObjSpriteFilename; // fichier contenant les id correspondants aux types de sprites des objets que l'on peut mettre : dans la main, aux pieds, à la ceinture ... d'un perso
extern const AnsiString cIDBmpFrameFilename;      // fichier contenant les id de toutes les frames du jeu relatives aux persos et au format bmp  
extern const AnsiString cIDObjetsFilename;        // fichier contenant les id de tous objets/décors/sols du jeu ayant des animations
extern const AnsiString cIDTilesFilename;         // fichier contenant les id de tous les tiles
extern const AnsiString cIDMemoryFilename;        // fichier contenant les id de toutes les mémoires du jeu
extern const AnsiString cIDGameDecorsFilename;	  // Nom du fichier contenant les n°ID de tous les décors importants du jeu
extern const AnsiString cIDGamePersosFilename;	  // Nom du fichier contenant les n°ID de tous les personnages importants du jeu
extern const AnsiString cIDGameScriptsFilename;	  // Nom du fichier contenant les n°ID de tous les scripts importants du jeu, ainsi que le nom des fichiers scripts .zgs
extern const AnsiString cIDMapZonesFilename;	  // Nom du fichier contenant les n°ID de toutes les zones au sol du jeu
extern const AnsiString cIDSoundEffectFilename;	  // Nom du fichier contenant les n°ID des differents bruitages

// Nom et chemin des fichiers contenant les squelettes de script *.ina
extern const AnsiString cPersosAnimationSqueletteFilename; // fichier contenant le modèle du perso

// Normes des modèles d'identificateurs
extern const AnsiString cPersosPatronID;   		// modèle d'identificateur des personnages

// Chemin des fichiers relatifs à un modèle
extern const AnsiString cPersoPatronPath;       // chemin par défaut où tous les fichiers relatifs aux modèle vont être sauvegardés

// Chemin + Nom des fichiers .ini du jeu
extern const char cIniZeGame[];					// Fichier .ini principal 

// Nombre de membres que possède le corps humain
const int cNbMembresDuCorpsHumains = 10;
// Nombre de types de sprites superposables
const int cNbTypeObjSprite = 1;


enum eRefreshMode { REFRESH_FIXED, REFRESH_MAX };
enum ePeriodeMode { PERIODE_JOUR, PERIODE_JOUR_NUIT };  // Gestion du passage du jour à la nuit

//== Utilisé pour le traçage des layers ==
enum eAffichMode { LAYER_SOL, LAYER_DECORS, LAYER_ALL };
enum eBlitMode   { RECALCULE , DOUBLE_BUFFERING_SM , CALCUL_TAB };

//--- Gestion des entrées : clavier & Joypad ---
enum eControlMode { CONTROL_CLAVIER, CONTROL_CLAVIER_JOY, CONTROL_JOY };
extern eControlMode CONTROL_MODE;


extern BOOL bPerso;
extern eBlitMode    BLIT_MODE;
extern eAffichMode  AFFICH_MODE;
extern eRefreshMode REFRESH_MODE;
extern int REFRESH_FPS;

extern HWND	hWndApplication;		// handle de la fenêtre principale de l'appli
extern bool bGameActive;			// Indique si le jeu est chargé et est prêt à tourner
extern bool bGamePause;				// Indique si le jeu est actuellement en pause

extern bool SHADOW_PERSO;           // Affiche l'ombre transparente des personnages
extern bool NIGHT_FILTER;           // Filtre de nuit


extern BOOL FRAME_RATE;				// Affiche le frame rate à l'écran
extern BOOL FONCTION_HELP;		    // Affiche oui/non l'aide à l'écran
extern BOOL FONCTION_DEBUG_HERO;	// Affiche oui/non le panneau de debuggage pour le héro

// Version de le sauvegarde des *.LiL (fichier descriptif d'un niveau)
extern const float VersLiL;


//--- n°ID --- 
extern std::IDMap *IDPatronPersos;   // Map des identificateurs de gestion de patrons de persos
extern std::IDMap *IDAnimation;	     // Map des identificateurs des mouvements d'animation
extern std::IDMap *IDPersosType;     // Map des ID des Types de personnages différents
extern std::IDMap *IDMembres;        // map des ID recensants les différents membres du corps humain
extern std::IDMap *IDBmpFrame;       // Map des ID de toutes les frames bmp du jeu
extern std::IDMap *IDTypeObjSprite;  // Map des ID des types d'objets (ex: epee1) servant dans la superposition d'anims
extern std::IDMap *IDObjets;         // Map des ID des objets/décors/... animés
extern std::IDMap *IDTiles;          // Map des ID des tiles du jeu
extern std::IDMap *IDMemory;		 // Map des ID des mémoires
extern std::IDMap *IDMapZones;       // Map des ID des zones au sol
extern std::IDMap *IDSoundEffect;    // Map des ID des bruitages du jeu
// Map des ID proveant de ScnScript.cpp
//extern std::IDMap *IDGameDecors;	 // Map des ID des différents décors importants du jeu
//extern std::IDMap *IDGamePersos;	 // Map des ID des différents personnages importants du jeu

//-------------------------------------------------- Paramètres de configs du jeu
// Initialisés à partir de la boîte de dialogue de démarrage
extern int CFG_SCREEN_WIDTH, CFG_SCREEN_HEIGHT, CFG_SCREEN_BITS; // Résolution Hauteur x Largeur x Bits/Pixel
extern AnsiString LevelName;			  // Nom du niveau *.niv choisi
extern ePeriodeMode CFG_PERIODE_MODE;     // Activation du assage du jour à la nuit
extern bool CFG_AJOUT_PNJ;                // Ajout de 2 PNJ
extern bool CFG_DEMO_TOURNANTE;           // Le Héro avance tout seul
extern bool CFG_GAME_RELEASE;			  // Supprime certains bug persistants


#endif
