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


// Donn�es g�om�triques d'un trap�ze
const int ISO_HEIGHT 		= 21-1,
	  ISO_WIDTH  		= 39-1,
    	  ISO_HEIGHT_HALF = 10,
    	  ISO_WIDTH_HALF  = 19,
          ISO_DENOMINATEUR = 2 * ISO_HEIGHT_HALF * ISO_WIDTH_HALF;

// Donn�es sur les tailles des diff�rentes fen�tres de l'�diteur
extern int ScreenViewX,    // fen�tre repr�sentant la vue du jeu
           ScreenViewY;

const int EditorWidth  = 1024,	    // r�solution ds laquelle doit �tre lanc� l'�diteur
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


// Type de trac� du trap�ze
const int TrapTYPE_SIMPLE  = 0,
	  TrapTYPE_DOUBLE  = 1;

// Type �num�r� des Points cardinaux :
//  - Direction sur l'�cran  : vue 3D ISO
typedef enum {View_O=0, View_SO, View_S, View_SE, View_E, View_NE, View_N, View_NO } eScreenDirection;
//  - Direction sur la carte : vue de haut
typedef enum {Map_SO=0, Map_S, Map_SE, Map_E, Map_NE, Map_N, Map_NO, Map_O } eMapDirection;
// => Il y'a une correspondance entre ces points : lorsque le personnage se deplace
//    vers le Nord dans la vue ISO, il se deplace vers le Nord-Ouest sur la carte.

// Type de Librairie s�lectionn�e
const int Lib_NULL 	 = 0,
	  Lib_DECORS = 1,
	  Lib_SOLS   = 2,
      Lib_PERSOS = 3,
      Lib_EVENTS = 4,
      Lib_DESIGN = 5;



// Param�tres de configuration des couleurs, polices ... du look, quoi !
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

// Donn�es utilis�es ds "ImDecors2.h" => Lib universelle & co
const int cNbTypeUnivers = 2,			// Nb d'univers : grottes, plaines, villes ...
          cNbTypeTiles = 7;			// Nb de Sols & D�cors ds l'univers

extern const char cUniversName [5][20];

extern const char cTypeTileName [7][20];

const int cIndexSols   = 0,       // Index & Nombre de D�cors, Sols ...
	  cNbTilesSols = 3;
const int cIndexDecors   = 3,
	  cNbTilesDecors = 3;

// Donn�es utilis�es ds DecorsObjet de "ImDecors2.h"
const int Etage_PRINCIPAL = 1;
const int Layer_SOLS	  = 1,
	  Layer_DECORS	  = 2;

// R�pertoire o� se trouvent :
extern const AnsiString cPathTiles;
extern const AnsiString cPathLevels;
extern const AnsiString cPathPersos;
extern const AnsiString cPathObjets;	// - les fichiers d'amis des objets et des d�cors du jeu
extern const char cPathSounds[];		// - les sons & les musiques au format .wav
extern const char cPathMuzik[];			// - les musiques au format .wav

// Repertoire des suites d'images *.bmp � convertir en banque
extern const AnsiString cPathTemp;


// Nom et chemin des fichier contenant les identificateurs *.id
extern const AnsiString cPersosPatronFilename;	  // fichier contenant les id des patrons des persos
extern const AnsiString cIDAnimationFilename;     // fichier contenant les id des types d'animations
extern const AnsiString cIDPersosTypeFilename;    // fichier contenant les id des types de personnages
extern const AnsiString cIDPersosMembresFilename; // fichier contenant les id correspondants aux mebres d'un �tre humain
extern const AnsiString cIDTypeObjSpriteFilename; // fichier contenant les id correspondants aux types de sprites des objets que l'on peut mettre : dans la main, aux pieds, � la ceinture ... d'un perso
extern const AnsiString cIDBmpFrameFilename;      // fichier contenant les id de toutes les frames du jeu relatives aux persos et au format bmp  
extern const AnsiString cIDObjetsFilename;        // fichier contenant les id de tous objets/d�cors/sols du jeu ayant des animations
extern const AnsiString cIDTilesFilename;         // fichier contenant les id de tous les tiles
extern const AnsiString cIDMemoryFilename;        // fichier contenant les id de toutes les m�moires du jeu
extern const AnsiString cIDGameDecorsFilename;	  // Nom du fichier contenant les n�ID de tous les d�cors importants du jeu
extern const AnsiString cIDGamePersosFilename;	  // Nom du fichier contenant les n�ID de tous les personnages importants du jeu
extern const AnsiString cIDGameScriptsFilename;	  // Nom du fichier contenant les n�ID de tous les scripts importants du jeu, ainsi que le nom des fichiers scripts .zgs
extern const AnsiString cIDMapZonesFilename;	  // Nom du fichier contenant les n�ID de toutes les zones au sol du jeu
extern const AnsiString cIDSoundEffectFilename;	  // Nom du fichier contenant les n�ID des differents bruitages

// Nom et chemin des fichiers contenant les squelettes de script *.ina
extern const AnsiString cPersosAnimationSqueletteFilename; // fichier contenant le mod�le du perso

// Normes des mod�les d'identificateurs
extern const AnsiString cPersosPatronID;   		// mod�le d'identificateur des personnages

// Chemin des fichiers relatifs � un mod�le
extern const AnsiString cPersoPatronPath;       // chemin par d�faut o� tous les fichiers relatifs aux mod�le vont �tre sauvegard�s

// Chemin + Nom des fichiers .ini du jeu
extern const char cIniZeGame[];					// Fichier .ini principal 

// Nombre de membres que poss�de le corps humain
const int cNbMembresDuCorpsHumains = 10;
// Nombre de types de sprites superposables
const int cNbTypeObjSprite = 1;


enum eRefreshMode { REFRESH_FIXED, REFRESH_MAX };
enum ePeriodeMode { PERIODE_JOUR, PERIODE_JOUR_NUIT };  // Gestion du passage du jour � la nuit

//== Utilis� pour le tra�age des layers ==
enum eAffichMode { LAYER_SOL, LAYER_DECORS, LAYER_ALL };
enum eBlitMode   { RECALCULE , DOUBLE_BUFFERING_SM , CALCUL_TAB };

//--- Gestion des entr�es : clavier & Joypad ---
enum eControlMode { CONTROL_CLAVIER, CONTROL_CLAVIER_JOY, CONTROL_JOY };
extern eControlMode CONTROL_MODE;


extern BOOL bPerso;
extern eBlitMode    BLIT_MODE;
extern eAffichMode  AFFICH_MODE;
extern eRefreshMode REFRESH_MODE;
extern int REFRESH_FPS;

extern HWND	hWndApplication;		// handle de la fen�tre principale de l'appli
extern bool bGameActive;			// Indique si le jeu est charg� et est pr�t � tourner
extern bool bGamePause;				// Indique si le jeu est actuellement en pause

extern bool SHADOW_PERSO;           // Affiche l'ombre transparente des personnages
extern bool NIGHT_FILTER;           // Filtre de nuit


extern BOOL FRAME_RATE;				// Affiche le frame rate � l'�cran
extern BOOL FONCTION_HELP;		    // Affiche oui/non l'aide � l'�cran
extern BOOL FONCTION_DEBUG_HERO;	// Affiche oui/non le panneau de debuggage pour le h�ro

// Version de le sauvegarde des *.LiL (fichier descriptif d'un niveau)
extern const float VersLiL;


//--- n�ID --- 
extern std::IDMap *IDPatronPersos;   // Map des identificateurs de gestion de patrons de persos
extern std::IDMap *IDAnimation;	     // Map des identificateurs des mouvements d'animation
extern std::IDMap *IDPersosType;     // Map des ID des Types de personnages diff�rents
extern std::IDMap *IDMembres;        // map des ID recensants les diff�rents membres du corps humain
extern std::IDMap *IDBmpFrame;       // Map des ID de toutes les frames bmp du jeu
extern std::IDMap *IDTypeObjSprite;  // Map des ID des types d'objets (ex: epee1) servant dans la superposition d'anims
extern std::IDMap *IDObjets;         // Map des ID des objets/d�cors/... anim�s
extern std::IDMap *IDTiles;          // Map des ID des tiles du jeu
extern std::IDMap *IDMemory;		 // Map des ID des m�moires
extern std::IDMap *IDMapZones;       // Map des ID des zones au sol
extern std::IDMap *IDSoundEffect;    // Map des ID des bruitages du jeu
// Map des ID proveant de ScnScript.cpp
//extern std::IDMap *IDGameDecors;	 // Map des ID des diff�rents d�cors importants du jeu
//extern std::IDMap *IDGamePersos;	 // Map des ID des diff�rents personnages importants du jeu

//-------------------------------------------------- Param�tres de configs du jeu
// Initialis�s � partir de la bo�te de dialogue de d�marrage
extern int CFG_SCREEN_WIDTH, CFG_SCREEN_HEIGHT, CFG_SCREEN_BITS; // R�solution Hauteur x Largeur x Bits/Pixel
extern AnsiString LevelName;			  // Nom du niveau *.niv choisi
extern ePeriodeMode CFG_PERIODE_MODE;     // Activation du assage du jour � la nuit
extern bool CFG_AJOUT_PNJ;                // Ajout de 2 PNJ
extern bool CFG_DEMO_TOURNANTE;           // Le H�ro avance tout seul
extern bool CFG_GAME_RELEASE;			  // Supprime certains bug persistants


#endif
