////////////////////////////////////////////////////////////////////////////////
// Constantes Globales 		 												  //
////////////////////////////////////////////////////////////////////////////////

#ifndef Consts
#define Consts

#include <vcl\vcl.h>

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
          Pointeur_GOMMESOLS    = 4;

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
          Lib_EVENTS = 4;


// Param�tres de configuration des couleurs, polices ... du look, quoi !
const int ConfigTonio  = 1,
	  ConfigNormal = 2;

/*const TColor ColorBoxTonio   = clInactiveCaption,
		     ColorBoxNormal  = clWhite,
             ColorBoxTonio2  = clActiveCaption,
             ColorBoxNormal2 = clAqua;
*/

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

extern const AnsiString cPathTiles;
extern const AnsiString cPathLevels;
extern const AnsiString cPathPersos;
extern const AnsiString cPathObjets;   // R�pertoire o� se trouvent les fichiers d'amis des objets et des d�cors du jeu

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

// Nom et chemin des fichiers contenant les squelettes de script *.ina
extern const AnsiString cPersosAnimationSqueletteFilename; // fichier contenant le mod�le du perso


// Normes des mod�les d'identificateurs
extern const AnsiString cPersosPatronID;   		// mod�le d'identificateur des personnages


// Chemin des fichiers relatifs � un mod�le
extern const AnsiString cPersoPatronPath;       // chemin par d�faut o� tous les fichiers relatifs aux mod�le vont �tre sauvegard�s

// Nombre de membres que poss�de le corps humain
const int cNbMembresDuCorpsHumains = 10;
// Nombre de types de sprites superposables
const int cNbTypeObjSprite = 1;


#endif
