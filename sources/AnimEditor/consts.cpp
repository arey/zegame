////////////////////////////////////////////////////////////////////////////////
// Constantes Globales 		 												  //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Fichier regroupant toutes les constantes requises par le jeu				  //
////////////////////////////////////////////////////////////////////////////////

#include "Consts.h"

const char cUniversName [5][20] =
		{"Grotte", "Plaine", "Forêt", "Ville", "Carrière"};

const char cTypeTileName [7][20] =
 		{"sNatur", "sArchi", "Inter", "dNatur", "Objet", "Mur", "dArchi" };

const AnsiString cPathTiles  = "Tiles\\";
const AnsiString cPathLevels = "Levels\\";
const AnsiString cPathPersos = "Persos\\";
const AnsiString cPathObjets = "Objets\\";   // Répertoire où se trouvent les fichiers d'amis des objets et des décors du jeu

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
const AnsiString cIDTilesFilename = "Tiles\\IDTiles.id";



//*** Nom et chemin des fichiers contenant les squelettes de script *.ina ***
// fichier contenant le modèle du perso
const AnsiString cPersosAnimationSqueletteFilename = "Persos\\PersoSquelette.ina";

//*** Normes des modèles d'identificateurs ***
// modèle d'identificateur des personnages
const AnsiString cPersosPatronID = "ID_PERSOMODELE_";


//*** Chemin des fichiers relatifs à un modèle ***

// chemin par défaut où tous les fichiers relatifs aux modèle vont être sauvegardés
const AnsiString cPersoPatronPath = cPathPersos;




