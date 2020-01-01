////////////////////////////////////////////////////////////////////////////////
// Données de l'image du décors 											  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PARTIE COMMUNE AVEC L'EDITEUR DE DECORS									  //
////////////////////////////////////////////////////////////////////////////////
#ifndef ImDecors2
#define ImDecors2

#include "consts.h"
#include "DDLib.h"
#include "Animall.h"
#define MaxChar 15			// nb max de caractères ds 1 chaîne


// **** VERSION 0.9 de la STRUCTURE TRAPEZE ***
enum TrapTERRAIN { INCONNU=0, SABLE, TERRE, HERBE, GRAVIERS, BOIS, MARBRE, RACINE,
                   TRONC, MUR, OBJET };
const int TrapTERRAINNb = 11;
extern const char TrapTERRAINText [TrapTERRAINNb][20];

// **** VERSION 0.95 de la strucuture TRAPEZE (09/02/99) ***
enum eTrapCOLLIDE { PARTIELLE = 0, TOTALE };
const int TrapCOLLIDENb = 2;
extern const char TrapCOLLIDEText [TrapCOLLIDENb][20];



// Structure d'un Trapèze
struct Trapeze
{   int TrapU, TrapV;			// Coords du trapèze par rapport au trapèze principal

    //*** Nouveau à partir de la version 0.9 ***
    int Hauteur;			// Hauteur de l'objet présent sur la case / au sol
    TrapTERRAIN	Terrain;		// Type de terrain présent sur le trapèze
    BOOL Accessible;                    // indique si on peut y marcher oui ou non
    //*** Nouveau à partir de la version 0.95 ***
    eTrapCOLLIDE TrapCollide;           // Type de collisions à effectuer sur ce trapèze
    //*** Nouveau à partir de la version 0.97 ***
    bool DrawTrapezekey;                // indique si ce trapèze est un élément clef de l'affichage
    int  lamX1, lamY1, lamX2, lamY2;    // lamelle à afficher pour ce trapèze
};
//*** FIN DE LA NEW VERSION ***


//===  TILE DU DECORS ========================================================//
struct TileDecors
{
    int  DecorsNum;			// n° de référence servant à tout objet s'y référencant
    char Nom [MaxChar];	          	// Nom du fichier
    int  Width, Height;			// Taille du tile
    unsigned int RVB;	            	// Composante de la couleur transparente
    unsigned char NumRVB;		// n° de couleur transparente ds la palette
    int  NbTrapeze;			// Nombre de trapèzes s'y rattachant
    int  TrapPrincX, TrapPrincY;	// Coordonnées du coin supérieur du trapèze principal par rapport au coin supérieur-gauche du tile <=> Centre de gravité du SpriteTile
    // TrapezTab[0] fait référence au trapèze principal
    Trapeze *TrapezTab;			// Tableau dynamique pointant sur un trapèze
    int DecorsCornerX, DecorsCornerY;	// ne sert que pour l'éditeur : sait où replacer le tile

    // Nouveau :  utilisé ds la liste
    TileDecors *Suiv, *Prec;		// Pointe sur les éléments suivant et précédent de liste
    int NumDsList;			// n° dans la liste

    // Nouveau : utilisé ds DDRaw
    DDSurface *TileSurface;		// Surface DirectDraw créée uniquement pour ce sprite
    // Nouveau 2 : utilisé par la lib de level
    int LibIndex; 			// Index de la mini-lib ds la level-lib
    // Nouveau 3 : chemin du tiles
    AnsiString TilePath;

    //--- New depuis le 20/03/99 - version 0.97 ---
    bool TotalDraw;                     // Indique si l'objet doit s'afficher en une seule fois ou par tranches verticales
};

void LoadTileDecors(TileDecors *TileLoading, char Name[30]);
//----------------------------------------------------------------------------//



////////////////////////////////////////////////////////////////////////////////
// 1ères NOUVEAUTES  														  //
////////////////////////////////////////////////////////////////////////////////

//== Librairie des tiles de décors ===========================================//
class DecorsTileLib
{
  public:
  // variables
    AnsiString Nom;		       	// nom de la librairie = nom du fichier
    int NbTile;		       	        // nombre de tile de la librairie
    TileDecors  *DecorsTileList;	// pointe sur le dernier élément de la liste
  // fonctions
    DecorsTileLib(void);		// constructeur
    ~DecorsTileLib(void);		// destructeur
    void DeleteAllLib();		// efface toute la librairie
    void Ajouter(TileDecors *TileAdd);	// ajoute en dernier à la liste le fichier de décors
    BOOL ExisteTile(AnsiString SameName); // vérifie si le Tile n'existe déjà pas
    void SaveToFile();			// sauve ds le fichier de même nom d'extension *.LiD
    void LoadFile(AnsiString FileToLoad); // charge un fichier *.LiD

    // Nouveau  : Alloue les surfaces DirectDraw et charge les images
    void AlloueDDSurface();
    // Nouveau2 : Indique si c'est une librarie de Tile-Décors ou de Tile-Fond
    int TypeLib;
};

extern DecorsTileLib *DTLib;				// librairie sélectionnée des décors utilisé ds l'éditeur
extern TileDecors    *TDSelect;				// Objet Sélectionné

//== OBJET DU DECORS =========================================================//
class DecorsObjet
{
   public:
    unsigned int NumObjet;		// n° attribué lors du positionement de l'objet sur la grille
 //   AnsiString DecorsLibName;	// nom de la librairie d'où vient l'image
    TileDecors *DecTileObj;		// Pointe sur l'image graphique associée à l'objet
    int PosU, PosV;			// position de l'objet (Trap Principal) sur la grille
    int ObjEtage;			// Indique l'étage auquel il appartient  (PRINCIPAL / -1 / +1 )
    int ObjLayer;			// Indique le layer auquel il appartient (SOLS ou DECORS)

    int ObjDessine;			// Indique si l'objet à déjà été dessiné à l'écran
    // Infos Supplémentaires uniques à chauqe objet
    // ............
    // ............
    // ............

    DecorsObjet *Suiv, *Prec;		// Pointe sur les éléments suivant et précédent de liste
//  int NumDsList;			// n° dans la liste
    DecorsObjet();			// Constructeur

    //--- Animations & Etats : new depuis le 1er mai 1999 ---
    int peEtat;   		  // ARRET, OUVRIR, BRISER ...
                              // Rmq : ARRET1 correspond au bitmap représentant l'objet
    int  peFutEtat;		  	  // futur état souhaité
    int  peOldEtat;           // ancien état de l'objet/décors
    int  peDirection;		  // 0 à 8 orientations possibles : or_N, or_NE, or_S ...
    int  peFutDirection;	  // future direction souhaitée
    int  peNumSprite;		  // n° du sprite de l'animation
    int  peNbSprite;		  // nb de sprties de l'animation courante
    int  peSprNumLib;		  // n° du sprite ds sa librairie

    TAnimTileDecors *Element;     // Element
    void SetAnimTileDecors(TAnimTileDecors *element);

    void ResultAction();
    void peBouge();               //
    int  getNumAnim();            // Renvoie le n° de l'anim courante

    //--- Animations superposables "classiques" ---
    int* peNumClassikAnim;	  // Tableau indiquant à quel sprite on en est pour chacune des animations superposables (-1 si pas enclenchées) de la structure "ClassikAnimInfos"
};
//----------------------------------------------------------------------------//


//== Librairie des Objets de décors ==========================================//
class DecorsObjetLib
{
  public:
    int NbTileObj;			// Nombre d'objets de décors ds cette Librairie
    DecorsObjet *DecorsObjList;         // Pointe sur le dernier objet de la liste
    int LastNum;			// N° courant de l'objet à placer sur la grille
    // fonctions
    DecorsObjetLib(void);			// Constructeur
    ~DecorsObjetLib(void);			// Destructeur
    void DeleteAllLib();			// efface toute la lib
    void AddDecObj(DecorsObjet *DecorsObjAdd);	// Ajoute un objet à la liste
    void SupprDecObj(DecorsObjet *ptr);         // Supprime un objet à la liste
    DecorsObjet* SearchObj(unsigned int Numero);

    void ObjetNonDessine();		// reset toutes les valeures ObjDessine
};
//----------------------------------------------------------------------------//

extern DecorsObjetLib *DOLib;
extern DecorsObjet    *DOSelect;      	// Objet-Décors sélectionné ds la liste ou sur la grille



////////////////////////////////////////////////////////////////////////////////
// 2nde NOUVEAUTES  														  //
////////////////////////////////////////////////////////////////////////////////

//== Librairie Universelle des Tiles (décors+ fonds) =========================//
//  => Utilise les DecorsTileLib
//  => for the moment,
class TUniversalTileLib
{
  public:
    DecorsTileLib **UnivLib;	 // Tableau dynamique de librairie gérant les *.LiD
    int NbTypeUnivers;		     // Nb d'univers : grottes, plaines, villes ...
    int NbTypeTiles;		     // Nb de Sols & Décors ds l'univers

    TUniversalTileLib();	     // Constructeur
    ~TUniversalTileLib();	     // Destrcuteur
    void AlloueLib();		     // Alloue le Tableau dynamique librairie **UnivLib
    void DesaloueLib();          // Désalloue le **UnivLib
};
//----------------------------------------------------------------------------//

//== Librairie Des Tiles d'un Level ==========================================//
class TLevelTileLib
{
  public:
    AnsiString Nom;					// nom de la librairie = nom du fichier
    int NbTypeTiles;				// Nb de type de tiles (arbres, murs, fleurs ...)
    int NbTotalTiles;				// Nb total de tiles de tout type
    int  *NbTilesPerType;			// Nb de tiles par type
    int  *TypeTileLib;				// type des libs (fonds, décors ...)
    TileDecors  **TileMiniLibList;	// Pointe sur le dernier élément de la liste

    TLevelTileLib();				    // Constructeur
    ~TLevelTileLib();				    // Destructeur
    void Ajouter(TileDecors *TileAdd, int index);   // ajoute en dernier de la mini-lib le tile de décors/sols
    void EffaceOneLevelLib(int index);
    void DeleteAllLevelLib();			    // efface toute les mini-libs
    BOOL ExisteTile(AnsiString SameName);   // vérifie si le Tile n'existe déjà pas quelque-part ds la Lib
    void SaveToFile();  	                // sauve ds le fichier de même nom d'extension *.LiL <=> LIb.du Level
    void LoadFile(AnsiString FileName);     // Charge le fichier "FileName".LiL

    // Nouveau pour l'éditeur de map
    int MiniLibSelect;			// indique le n° de Mini-Lib actuellement sélectionné
    void AlloueDDSurface();		// charge les *.bmp correspondant à la Lib ds des surfaces DirectDraw

    // Nouveau depuis le 05/05/99
    TileDecors* getTileDecors(int numID);  // Renvoie un pointeur sur le Tile ayant ce n°ID
};

extern TLevelTileLib *DULib;	       	   // librairie sélectionnée des décors utilisé ds l'éditeur
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// 3ième NOUVEAUTES  depuis le 1er mai 1999 				      //
////////////////////////////////////////////////////////////////////////////////
class ClisteObjetFrame;  // Classe définie dans PersoLib.h

//--- Complément d'informations données graphiquement sur une animation ------//
struct SAnimInfos
{  int numTileDecorsAssoc;      // n° du TileDecors associé à cette animation
   int dX, dY;                  // différences de coordonnées à corriger
};
//----------------------------------------------------------------------------//

//--- Informations sur les anims superposables "classiques" ------------------//
// => fumée, explosions, sang ki gicle ...
struct SClassikAnimInfos
{	int	iAnimID;				// Identifiant de l'animation associé à cet objet
	int dX, dY;					// différences de coordonnées entre 2 centres de gravité
    int iMainAnimID;			// [facultatif] n° de l'anim spécifique sur laquelle on applique cette anim superposable (-1 si la MainAnim courante n'a pas d'influence)
    int reserved[2];            // [facultatif] en réserve ... on sait jamais ...
    SClassikAnimInfos():		// Constructeur
     iAnimID(-1), dX(0), dY(0), iMainAnimID(-1) { };
};

//=== Classe gérant les animations des TileDecors ============================//
// Rmq :  Cette classe présente des similitudes avec la classe Character
class TAnimTileDecors
{  public:
        TAnimTileDecors();                 // Constructeur par défaut

        //--- Anims & transitions ---
        int peNbAnim;  		               // nb d'animations ds la librairie
        AnimSprite** AnimSpriteLib;	   	   // Tableau dynamique de ttes les animations  ("Animall.cpp")
        int peNbTrans;			   		   // nb de transitions ds la librairie
        AnimTransition** AnimTransLib;     // Tableau dynamique de ttes les transitions entre anims  ("Animall.cpp")

        SpriteTilePersoLib* STLObjets;     // Librairie des sprites des anims
        void LoadIna(AnsiString InaName);  // Charge l'*.ina contenant toutes les informations sur les Animations et les Transitions

        //--- Anims superposées ---
        int nbObjetFrames;                 // Nombres de frames superposées
        SpriteFrameList *FrameSuperpList;  // Liste des frames superposées concernant ce personnage
        SpriteTileLib *STLFrameSuperp;     // Librairie des sprites du perso

        //--- Infos supplémentaires à AnimSprite aux sujets des animations ---
        TileDecors  *MainTile;             // Tile représentant l'élément courant
        bool peShowTile;                   // Indique si l'on doit afficher un TileDecors ou un Sprite
        SAnimInfos  *AnimInfos;            // Infos complémentaires
        static float AnimInfosVersion;     // Version de la structure SAnimsInfos
        void SaveToFileAnimInfos(FILE &fichier);   // Sauvegarde ces infos
        void LoadFromFileAnimInfos(FILE &fichier); // Charge ces infos
        void LoadMdl(const char* MdlName); // Charge les informations relatives à ce type de Décors
        static float MdlVersion;           // Version actuelle du fichier *.Mdl

        //*** FOR NOW ***
        int nbMembres;                     // Nombres de membres du personnage
        ClisteObjetFrame *ObjetFrame;      // Tableau de nbMembres listes d'sObjetFrame

        //--- New depuis le 29/06/99 ---
        SClassikAnimInfos*	ClassikAnimInfos;		 // Infos servant à placer une anim superposable "classique" sur le décors
        int	peNbClassikAnim;						 // Nb d'anims superposables "clasqqiues"
        SClassikAnimInfos* GetClassikAnim(int id);	 // Renvoie un poiteur sur la structure ayant ce n° d'id
        void SaveToFileAnimClassik(FILE &fichier);   // Sauvegarde ces infos
        bool LoadFromFileAnimClassik(FILE &fichier); // Charge ces infos
};
//----------------------------------------------------------------------------//

//--- Liste de tous les modèles d'objets/décors du jeu ---
extern TAnimTileDecors** ListeObjetsDecors;
extern int NbMaxTileDecors;


#endif
