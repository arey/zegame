////////////////////////////////////////////////////////////////////////////////
// Constantes Globales 		 												  //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Fichier regroupant toutes les constantes requises par le jeu				  //
////////////////////////////////////////////////////////////////////////////////

#include "Consts.h"

const char cUniversName [5][20] =
		{"Grotte", "Plaine", "For�t", "Ville", "Carri�re"};

const char cTypeTileName [7][20] =
 		{"sNatur", "sArchi", "Inter", "dNatur", "Objet", "Mur", "dArchi" };

const AnsiString cPathTiles  = "Tiles\\";
const AnsiString cPathLevels = "Levels\\";
const AnsiString cPathPersos = "Persos\\";
const AnsiString cPathObjets = "Objets\\";   // R�pertoire o� se trouvent les fichiers d'amis des objets et des d�cors du jeu

// Repertoire des suites d'images *.bmp � convertir en banque
const AnsiString cPathTemp	 = "Temp\\";


//*** Nom et chemin des fichier contenant les identificateurs *.id ***
// fichier contenant les id des patrons des persos
const AnsiString cPersosPatronFilename = "Persos\\IDPersosPatron.id";
// fichier contenant les id des types d'animations
const AnsiString cIDAnimationFilename = "Persos\\IDAnimations.id";
// fichier contenant les id des types de personnages
const AnsiString cIDPersosTypeFilename = "Persos\\IDPersosType.id";
// fichier contenant les id correspondants aux membres d'un �tre humain
const AnsiString cIDPersosMembresFilename ="Persos\\IDMembres.id";
// fichier contenant les id correspondants aux types de sprites des objets que
// l'on peut mettre : dans la main, aux pieds, � la ceinture ... d'un perso
const AnsiString cIDTypeObjSpriteFilename ="Persos\\IDTypeObjSprite.id";
// fichier contenant les id de toutes les frames du jeu relatives aux persos et au format bmp
const AnsiString cIDBmpFrameFilename = "Persos\\IDBmpFrame.id";
// fichier contenant les id de tous objets/d�cors/sols du jeu ayant des animations
const AnsiString cIDObjetsFilename = "Persos\\IDObjets.id";
// fichier contenant les id de tous les tiles
const AnsiString cIDTilesFilename = "Tiles\\IDTiles.id";



//*** Nom et chemin des fichiers contenant les squelettes de script *.ina ***
// fichier contenant le mod�le du perso
const AnsiString cPersosAnimationSqueletteFilename = "Persos\\PersoSquelette.ina";

//*** Normes des mod�les d'identificateurs ***
// mod�le d'identificateur des personnages
const AnsiString cPersosPatronID = "ID_PERSOMODELE_";


//*** Chemin des fichiers relatifs � un mod�le ***

// chemin par d�faut o� tous les fichiers relatifs aux mod�le vont �tre sauvegard�s
const AnsiString cPersoPatronPath = cPathPersos;




