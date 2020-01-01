////////////////////////////////////////////////////////////////////////////////
// Constantes Globales 		 												  //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Fichier regroupant toutes les constantes requises par le jeu				  //
////////////////////////////////////////////////////////////////////////////////

#include "ZeNain.h"			// Fichier commun à tout le projet
#include "Consts.h"			// Header

const char cUniversName [5][20] =
		{"Grotte", "Plaine", "Forêt", "Ville", "Carrière"};

const char cTypeTileName [7][20] =
 		{"sNatur", "sArchi", "Inter", "dNatur", "Objet", "Mur", "dArchi" };


// Répertoire où se trouvent :
const AnsiString cPathTiles  = "Tiles\\";
const AnsiString cPathLevels = "Levels\\";
const AnsiString cPathPersos = "Persos\\";
const AnsiString cPathObjets = "Objets\\";	// - les fichiers d'amis des objets et des décors du jeu
const char cPathSounds[]	 = "Sound\\";	// - les sons & les musiques au format .wav

// Repertoire des suites d'images *.bmp à convertir en banque
const AnsiString cPathTemp	 = "Temp\\";


//*** Nom et chemin des fichier contenant les identificateurs *.id ***
// fichier contenant les id des patrons des persos
const AnsiString cPersosPatronFilename = "Persos\\IDPersosPatron.id";
// fichier contenant les id des types d'animations
const AnsiString cIDAnimationFilename = "Persos\\IDAnimations.id";
// fichier contenant les id des types de personnages
const AnsiString cIDPersosTypeFilename = "Persos\\IDPersosType.id";
// fichier contenant les id correspondants aux membres d'un être humain
const AnsiString cIDPersosMembresFilename ="Persos\\IDMembres.id";
// fichier contenant les id correspondants aux types de sprites des objets que
// l'on peut mettre : dans la main, aux pieds, à la ceinture ... d'un perso
const AnsiString cIDTypeObjSpriteFilename ="Persos\\IDTypeObjSprite.id";
// fichier contenant les id de toutes les frames du jeu relatives aux persos et au format bmp
const AnsiString cIDBmpFrameFilename = "Persos\\IDBmpFrame.id";
// fichier contenant les id de tous objets/décors/sols du jeu ayant des animations
const AnsiString cIDObjetsFilename = "Persos\\IDObjets.id";
// fichier contenant les id de tous les tiles
const AnsiString cIDTilesFilename = "World\\IDTiles.id";
// Nom du fichier contenant les n°ID de tous les décors importants du jeu
const AnsiString cIDGameDecorsFilename ="World\\GameDecors.id";	
// Nom du fichier contenant les n°ID de tous les personnages importants du jeu
const AnsiString cIDGamePersosFilename ="World\\GamePersonnages.id";
// Nom du fichier contenant les n°ID de tous les scripts importants du jeu, ainsi que le nom des fichiers scripts .zgs
const AnsiString cIDGameScriptsFilename ="World\\GameScripts.id";
// fichier contenant les id de toutes les mémoires du jeu
const AnsiString cIDMemoryFilename = "World\\GameMemory.id";
// fichier contenant les id de toutes les zones au sol du jeu
const AnsiString cIDMapZonesFilename = "World\\GameZones.id";         



//*** Nom et chemin des fichiers contenant les squelettes de script *.ina ***
// fichier contenant le modèle du perso
const AnsiString cPersosAnimationSqueletteFilename = "Persos\\PersoSquelette.ina";

//*** Normes des modèles d'identificateurs ***
// modèle d'identificateur des personnages
const AnsiString cPersosPatronID = "ID_PERSOMODELE_";


//*** Chemin des fichiers relatifs à un modèle ***
// chemin par défaut où tous les fichiers relatifs aux modèle vont être sauvegardés
const AnsiString cPersoPatronPath = cPathPersos;

// Chemin + Nom des fichiers .ini du jeu
const char cIniZeGame[] = "ZeGame.ini";		 // Fichier .ini principal 




BOOL		 bPerso = TRUE;
eBlitMode    BLIT_MODE    = CALCUL_TAB;	    // Indique la méthode d'affichage du sol (reclacule tout, ou seulement ce qui a changé ?)
eAffichMode  AFFICH_MODE  = LAYER_ALL;	    // Indique quels layers doit-on afficher
eRefreshMode REFRESH_MODE = REFRESH_FIXED;  // mode raffale
int 	     REFRESH_FPS  = 33;				// 33 images / sec
BOOL 	     FONCTION_HELP = FALSE;			// Affiche oui/non l'aide à l'écran
BOOL         FONCTION_DEBUG_HERO = FALSE;   // Affiche oui/non le panneau de debuggage pour le héro
#ifdef DEBUG
BOOL	     FRAME_RATE = TRUE;				// montre le frame rate à l'écran qu'en version Debug
#else
BOOL	     FRAME_RATE = FALSE;
#endif
eControlMode CONTROL_MODE = CONTROL_CLAVIER_JOY;


AnsiString CheminApplication;
HWND	hWndApplication = NULL;				// handle de la fenêtre principale de l'appli
bool bGameActive = false;					// Indique si le jeu est chargé et est prêt à tourner
bool bGamePause = false;					// Indique si le jeu est actuellement en pause

bool         SHADOW_PERSO = TRUE;           // Affiche l'ombre transparente des personnages
bool         NIGHT_FILTER = FALSE;          // Filtre de nuit enclenché

// Version de le sauvegarde des *.LiL (fichier descriptif d'un niveau)
const float VersLiL = 2.5f;

//--- Librairie de numéro d'ID ---
std::IDMap *IDPatronPersos = NULL;  // Map des ID des patrons de personnages
std::IDMap *IDAnimation = NULL;	    // Map des identificateurs des mouvements d'animation
std::IDMap *IDPersosType = NULL;    // Map des ID des Types de sprites différents
std::IDMap *IDMembres = NULL;       // map des ID recensants les différents membres du corps humain
// A inclure dans le jeu (leture des .ina)
std::IDMap *IDBmpFrame = NULL;      // Map des ID de toutes les frames bmp du jeu
std::IDMap *IDTypeObjSprite = NULL; // Map des ID des types d'objets (ex: epee1) servant dans la superposition d'anims
std::IDMap *IDObjets = NULL;        // Map des ID des objets/décors/... animés
std::IDMap *IDTiles = NULL;         // Map des ID des tiles du jeu
std::IDMap *IDMemory = NULL;		// Map des ID des mémoires
std::IDMap *IDMapZones = NULL;      // Map des ID des zones au sol

//-------------------------------------------------- Paramètres de configs du jeu
// Initialisés à partir de la boîte de dialogue de démarrage
int CFG_SCREEN_WIDTH, CFG_SCREEN_HEIGHT, CFG_SCREEN_BITS; // Résolution Hauteur x Largeur x Bits/Pixel
AnsiString LevelName;			   // Nom du niveau *.niv choisi
ePeriodeMode CFG_PERIODE_MODE;     // Activation du assage du jour à la nuit
bool CFG_AJOUT_PNJ;                // Ajout de 2 PNJ
bool CFG_DEMO_TOURNANTE;           // Le Héro avance tout seul
bool CFG_GAME_RELEASE;			   // Supprime certains bug persistants



