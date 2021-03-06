////////////////////////////////////////////////////////////////////////////////
// Donn�es de l'image du d�cors 											  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PARTIE COMMUNE AVEC L'EDITEUR DE DECORS									  //
////////////////////////////////////////////////////////////////////////////////
#ifndef ImDecors2
#define ImDecors2

#include "consts.h"
#include "DDLib.h"
#include "GameObject.h"		// Classe de base de DecorsObjet

//=== Classes utilis�es ===
class AnimSprite;			// #include "Animall.h"
class AnimTransition;		// #include "Animall.h"
class TAnimTileDecors;		// ce fichier
class TFileVirtual;			// fichier "virtuel"

#define MaxChar 15			// nb max de caract�res ds 1 cha�ne


// **** VERSION 0.9 de la STRUCTURE TRAPEZE ***
enum TrapTERRAIN { INCONNU=0, SABLE, TERRE, HERBE, GRAVIERS, BOIS, MARBRE, RACINE,
                   TRONC, MUR, OBJET };
const int TrapTERRAINNb = 11;
extern const char TrapTERRAINText [TrapTERRAINNb][20];

// **** VERSION 0.95 de la strucuture TRAPEZE (09/02/99) ***
enum eTrapCOLLIDE { PARTIELLE = 0, TOTALE, ZONE_PIED, AUCUNE };
const int TrapCOLLIDENb = 3;
extern const char TrapCOLLIDEText [TrapCOLLIDENb][20];

// **** Version 0.98 de la strucuture TRAPEZE (09/07/99) ***
// Chps de bits indiquant de quelle mani�re on doit afficher le tile du d�cors
#define TILEDECORS_DRAW_OPAQUE		0			// Sans rien de sp�cifique
#define TILEDECORS_DRAW_SHADOW		1			// Tiens compte de l'ombrage                                     	


// Structure d'un Trap�ze (version 0.98)
// ATTENTION sous Visual sizeof(Trapeze) = 76 et chez Borland sizeof(Trapeze) = 72
struct Trapeze
{
    int TrapU, TrapV;					// Coords du trap�ze par rapport au trap�ze principal
    //*** Nouveau � partir de la version 0.9 ***
    int Hauteur;						// Hauteur de l'objet pr�sent sur la case / au sol
    TrapTERRAIN	Terrain;				// Type de terrain pr�sent sur le trap�ze
    BOOL Accessible;					// indique si on peut y marcher oui ou non
    //*** Nouveau � partir de la version 0.95 ***
	// Rmq : sous Visual sizeof(eTrapCOLLIDE) = 4 et chez Borland sizeof(eTrapCOLLIDE) = 1 
    eTrapCOLLIDE TrapCollide;           // Type de collisions � effectuer sur ce trap�ze
    //*** Nouveau � partir de la version 0.97 ***
    bool DrawTrapezekey;                // indique si ce trap�ze est un �l�ment clef de l'affichage
    int  lamX1, lamY1, lamX2, lamY2;    // lamelle � afficher pour ce trap�ze
    //*** Nouveau depuis la version 0.98 ***
    int reserved[8];
};


//===  TILE DU DECORS ========================================================//
struct TileDecors
{
    int  DecorsNum;				// n� de r�f�rence servant � tout objet s'y r�f�rencant
    char Nom [MaxChar];	      	// Nom du fichier
    int  Width, Height;			// Taille du tile
    unsigned int RVB;	        // Composante de la couleur transparente
    unsigned char NumRVB;		// n� de couleur transparente ds la palette
    int  NbTrapeze;				// Nombre de trap�zes s'y rattachant
    int  TrapPrincX, TrapPrincY;	// Coordonn�es du coin sup�rieur du trap�ze principal par rapport au coin sup�rieur-gauche du tile <=> Centre de gravit� du SpriteTile
    // TrapezTab[0] fait r�f�rence au trap�ze principal
    Trapeze *TrapezTab;			// Tableau dynamique pointant sur un trap�ze
    int DecorsCornerX, DecorsCornerY;	// ne sert que pour l'�diteur : sait o� replacer le tile

    // Nouveau :  utilis� ds la liste
    TileDecors *Suiv, *Prec;		// Pointe sur les �l�ments suivant et pr�c�dent de liste
    int NumDsList;			// n� dans la liste

    // Nouveau : utilis� ds DDRaw
    DDSurface *TileSurface;		// Surface DirectDraw cr��e uniquement pour ce sprite
    // Nouveau 2 : utilis� par la lib de level
    int LibIndex; 			// Index de la mini-lib ds la level-lib
    // Nouveau 3 : chemin du tiles
    AnsiString TilePath;

    //--- New depuis le 20/03/99 - version 0.97 ---
    bool TotalDraw;                     // Indique si l'objet doit s'afficher en une seule fois ou par tranches verticales

    //--- New depuis le 08/06/99 - version 0.98 ---
    bool CircleCollision;               // Indique si l'on doit utiliser une collision circulaire
    int  CircleCenterX, CircleCenterY;  // Coords du centre du cercle relatives � (TrapPrincX,TrapPrincY)
    int  CircleRadius;                  // Rayon du cercle

    int  DrawMethode;					// Indique si l'on doit afficher ce tile avec de l'ombre, de la transparence ...

    int reserved[19];                   // Octets r�serv�s pour plus tard
};

void LoadTileDecors(TileDecors *TileLoading, char Name[30]);
//----------------------------------------------------------------------------//



////////////////////////////////////////////////////////////////////////////////
// 1�res NOUVEAUTES  														  //
////////////////////////////////////////////////////////////////////////////////

//== Librairie des tiles de d�cors ===========================================//
class DecorsTileLib
{
  public:
  // variables
    AnsiString Nom;		       		// nom de la librairie = nom du fichier
    int NbTile;		       	        // nombre de tile de la librairie
    TileDecors  *DecorsTileList;	// pointe sur le dernier �l�ment de la liste
  // fonctions
    DecorsTileLib(void);		// constructeur
    ~DecorsTileLib(void);		// destructeur
    void DeleteAllLib();		// efface toute la librairie
    void Ajouter(TileDecors *TileAdd);	  // ajoute en dernier � la liste le fichier de d�cors
    BOOL ExisteTile(AnsiString SameName); // v�rifie si le Tile n'existe d�j� pas
    void SaveToFile();					  // sauve ds le fichier de m�me nom d'extension *.LiD
    void LoadFile(AnsiString FileToLoad); // charge un fichier *.LiD

    // Nouveau  : Alloue les surfaces DirectDraw et charge les images
    void AlloueDDSurface();
    // Nouveau2 : Indique si c'est une librarie de Tile-D�cors ou de Tile-Fond
    int TypeLib;
};

extern DecorsTileLib *DTLib;			  // librairie s�lectionn�e des d�cors utilis� ds l'�diteur
extern TileDecors    *TDSelect;			  // Objet S�lectionn�

//== OBJET DU DECORS =========================================================//
class DecorsObjet : public GameObject
{
   public:
    unsigned int NumObjet;		// n� attribu� lors du positionement de l'objet sur la grille
 //   AnsiString DecorsLibName;	// nom de la librairie d'o� vient l'image
    TileDecors *DecTileObj;		// Pointe sur l'image graphique associ�e � l'objet
    int PosU, PosV;				// position de l'objet (Trap Principal) sur la grille
    int peOldPosU, peOldPosV;   // ancienne position de l'objet
    int ObjEtage;				// Indique l'�tage auquel il appartient  (PRINCIPAL / -1 / +1 )
    int ObjLayer;				// Indique le layer auquel il appartient (SOLS ou DECORS)

    int ObjDessine;				// Indique si l'objet � d�j� �t� dessin� � l'�cran
    // Infos Suppl�mentaires uniques � chauqe objet
    // ............
    // ............
    // ............

    DecorsObjet *Suiv, *Prec;		// Pointe sur les �l�ments suivant et pr�c�dent de liste
//  int NumDsList;			// n� dans la liste
    DecorsObjet();			// Constructeur

    //--- Animations & Etats : new depuis le 1er mai 1999 ---
    int peEtat;   		  // ARRET, OUVRIR, BRISER ...
                                  // Rmq : ARRET1 correspond au bitmap repr�sentant l'objet
    int  peCollideEtat;           // �tat dans lequel on a d�tect� une collision
    int  peFutEtat;		  		  // futur �tat souhait�
    int  peOldEtat;               // ancien �tat de l'objet/d�cors
    int  peDirection;		  	  // 0 � 8 orientations possibles : or_N, or_NE, or_S ...
    int  peFutDirection;	  	  // future direction souhait�e
    int  peNumSprite;		  	  // n� du sprite de l'animation
    int  peNbSprite;		  	  // nb de sprties de l'animation courante
    int  peSprNumLib;		  	  // n� du sprite ds sa librairie
    int  peNumAnim;               // n� sp�cial de l'animation courante
	bool peChangeAnim;            // indique si l'on vient de changer d'anim

	float peAnimSpeed;			  // Vitesse d'animation	
	float peNumSpriteInterm;	  // N� de sprite interm�diaire

    bool peShowTile;              // Indique si l'on doit afficher un TileDecors ou un Sprite
    TileDecors *SecondTile;       // Tile repr�sentant l'�l�ment courant
    TileDecors *OldSecondTile;    // ch'tite suavegarde lors du passage d'un TileDecors � un autre
    int  peTileDecorsAssoc;       // n� du TileDecors associ� � l'�l�ment courant (provient de 'Element->AnimInfos[anm].numTileDecorsAssoc')
    int  peOldTileDecorsAssoc;    // ancien n� du TileDecors associ� � l'�l�ment courant
	int  iEndAnimSynchro;		  // indique si l'on doit pr�venir par mutex qu'une anim est finie

    TAnimTileDecors *Element;     // Element
    void SetAnimTileDecors(TAnimTileDecors *element);    
    bool DecorsAnime;             // Indique si c'est un objet anim� (<=> Element != NULL)

  private:
    void ResultAction();          // Anime les d�cors
  public:
    void peBouge();
    void Update();                // G�re provisoirement l'IA des d�cors
    void Fut2PresEtat();          // G�re les transitions de mvts des anims
    int  getNumAnim();            // Renvoie le n� de l'anim courante

    //--- Animations superposables "classiques" ---
    int* peNumClassikAnim;	  // Tableau indiquant � quel sprite on en est pour chacune des animations superposables (-1 si pas enclench�es) de la structure "ClassikAnimInfos"
	void RenderClassikAnims(int ScrX, int ScrY);		// S'occupe d'afficher � l'�cran les animations superpos�es    
};
//----------------------------------------------------------------------------//


//== Librairie des Objets de d�cors ==========================================//
class DecorsObjetLib
{
  public:
    int NbTileObj;					// Nombre d'objets de d�cors ds cette Librairie
    DecorsObjet *DecorsObjList;     // Pointe sur le dernier objet de la liste
    int LastNum;					// N� courant de l'objet � placer sur la grille
    // fonctions
    DecorsObjetLib(void);			// Constructeur
    ~DecorsObjetLib(void);			// Destructeur
    void DeleteAllLib();			// efface toute la lib
    void AddDecObj(DecorsObjet *DecorsObjAdd);	// Ajoute un objet � la liste
    void SupprDecObj(DecorsObjet *ptr);         // Supprime un objet � la liste
    DecorsObjet* SearchObj(unsigned int Numero);

    void ObjetNonDessine();			// reset toutes les valeures ObjDessine
    void Update();                  // Met � jour les objets contenus ds cette liste

    //--- Nuveau depuis le 13/05/1999 ---
    // Liste des d�cors ayant subits des modifications se r�percutant dans d'autres modules
    std::vector <DecorsObjet*> DecorsUpgraded;
};
//----------------------------------------------------------------------------//

extern DecorsObjetLib *DOLib;
extern DecorsObjet    *DOSelect;      	// Objet-D�cors s�lectionn� ds la liste ou sur la grille



////////////////////////////////////////////////////////////////////////////////
// 2nde NOUVEAUTES  														  //
////////////////////////////////////////////////////////////////////////////////

//== Librairie Universelle des Tiles (d�cors+ fonds) =========================//
//  => Utilise les DecorsTileLib
//  => for the moment,
class TUniversalTileLib
{
  public:
    DecorsTileLib **UnivLib;	     // Tableau dynamique de librairie g�rant les *.LiD
    int NbTypeUnivers;		     // Nb d'univers : grottes, plaines, villes ...
    int NbTypeTiles;		     // Nb de Sols & D�cors ds l'univers

    TUniversalTileLib();	     // Constructeur
    ~TUniversalTileLib();	     // Destrcuteur
    void AlloueLib();		     // Alloue le Tableau dynamique librairie **UnivLib
    void DesaloueLib();              // D�salloue le **UnivLib
};
//----------------------------------------------------------------------------//

//== Librairie Des Tiles d'un Level ==========================================//
class TLevelTileLib
{
  public:
    AnsiString Nom;			// nom de la librairie = nom du fichier
    int NbTypeTiles;			// Nb de type de tiles (arbres, murs, fleurs ...)
    int NbTotalTiles;			// Nb total de tiles de tout type
    int  *NbTilesPerType;		// Nb de tiles par type
    int  *TypeTileLib;			// type des libs (fonds, d�cors ...)
    TileDecors  **TileMiniLibList;	// Pointe sur le dernier �l�ment de la liste

    TLevelTileLib();				    // Constructeur
    ~TLevelTileLib();				    // Destructeur
    void Ajouter(TileDecors *TileAdd, int index);   // ajoute en dernier de la mini-lib le tile de d�cors/sols
    void EffaceOneLevelLib(int index);
    void DeleteAllLevelLib();			    // efface toute les mini-libs
    BOOL ExisteTile(AnsiString SameName);           // v�rifie si le Tile n'existe d�j� pas quelque-part ds la Lib
    void SaveToFile();  	                    // sauve ds le fichier de m�me nom d'extension *.LiL <=> LIb.du Level
    void LoadFile(AnsiString FileName);             // Charge le fichier "FileName".LiL

    // Nouveau pour l'�diteur de map
    int MiniLibSelect;			// indique le n� de Mini-Lib actuellement s�lectionn�
    void AlloueDDSurface();		// charge les *.bmp correspondant � la Lib ds des surfaces DirectDraw
	void ChangeTileVideoMode();	// Changement de mode-vid�o : restauration et rechargement des surfaces

    // Nouveau depuis le 05/05/99
    TileDecors* getTileDecors(int numID);     // Renvoie un pointeur sur le Tile ayant ce n�ID
};

extern TLevelTileLib *DULib;	       	// librairie s�lectionn�e des d�cors utilis� ds l'�diteur
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// 3i�me NOUVEAUTES  depuis le 1er mai 1999 				      //
////////////////////////////////////////////////////////////////////////////////
class ClisteObjetFrame;  // Classe d�finie dans PersoLib.h

//--- Compl�ment d'informations donn�es graphiquement sur une animation ------//
struct SAnimInfos
{  int numTileDecorsAssoc;  // n� du TileDecors associ� � cette animation
   int dX, dY;              // diff�rences de coordonn�es � corriger (soit (dX,dY) pour les anims, soit (dU,dV) pour les TileDecors) 
};
//----------------------------------------------------------------------------//

//--- Informations sur les anims superposables "classiques" ------------------//
// => fum�e, explosions, sang ki gicle ...
struct SClassikAnimInfos
{	int	iAnimID;				// Identifiant de l'animation associ� � cet objet
	int dX, dY;					// diff�rences de coordonn�es entre 2 centres de gravit�
    int iMainAnimID;			// [facultatif] n� de l'anim sp�cifique sur laquelle on applique cette anim superposable (-1 si la MainAnim courante n'a pas d'influence)
    int reserved[2];            // [facultatif] en r�serve ... on sait jamais ...
    SClassikAnimInfos():		// Constructeur
     iAnimID(-1), dX(0), dY(0), iMainAnimID(-1) { };
};

//=== Classe g�rant les animations des TileDecors ============================//
// Rmq :  Cette classe pr�sente des similitudes avec la classe Character
class TAnimTileDecors
{  public:
        TAnimTileDecors(int num_ID);       // Constructeur
        int numAnimTileDecorsID;           // n� didentification du AnimTileDecors : PIG1, VASE1 ...

        //--- Anims & transitions ---
        int peNbAnim;  		           	   // nb d'animations ds la librairie
        AnimSprite** AnimSpriteLib;	   	   // Tableau dynamique de ttes les animations  ("Animall.cpp")
        int peNbTrans;			   		   // nb de transitions ds la librairie
        AnimTransition** AnimTransLib;     // Tableau dynamique de ttes les transitions entre anims  ("Animall.cpp")

        SpriteTilePersoLib* STLObjets;     // Librairie des sprites des anims
        void LoadIna(AnsiString InaName);  // Charge l'*.ina contenant toutes les informations sur les Animations et les Transitions

        //--- Anims superpos�es ---
        int nbObjetFrames;                 // Nombres de frames superpos�es
        SpriteFrameList *FrameSuperpList;  // Liste des frames superpos�es concernant ce d�cors
        SpriteTileLib *STLFrameSuperp;     // Librairie des sprites du d�cors

        //--- Infos suppl�mentaires � AnimSprite aux sujets des animations ---
        TileDecors  *MainTile;             // Tile principal repr�sentant l'objet
        int NoDecorsNumAssoc;              // N� du TileDecors principalement associ� � cette anim
        SAnimInfos  *AnimInfos;            // Infos compl�mentaires
        static float AnimInfosVersion;     // Version de la structure SAnimsInfos
        void SaveToFileAnimInfos(FILE &fichier);   // Sauvegarde ces infos
        void LoadFromFileAnimInfos(FILE &fichier); // Charge ces infos
		void LoadFromFileAnimInfos(TFileVirtual &fichier); 
        void LoadMdl(const char* MdlName); // Charge les informations relatives � ce type de D�cors
        static float MdlVersion;           // Version actuelle du fichier *.Mdl

        //*** FOR NOW ***
        int nbMembres;                     // Nombres de membres du personnage
        ClisteObjetFrame *ObjetFrame;      // Tableau de nbMembres listes d'sObjetFrame

        //--- New depuis le 29/06/99 ---
        SClassikAnimInfos*	ClassikAnimInfos;		 // Infos servant � placer une anim superposable "classique" sur le d�cors
        int	peNbClassikAnim;						 // Nb d'anims superposables "clasqqiues"
        int GetClassikAnimIndex(int iAnmID);		 // Renvoie l'index de l'anim n�iAnmID 
        SClassikAnimInfos* GetClassikAnim(int id);	 // Renvoie un poiteur sur la structure ayant ce n� d'id
        void SaveToFileAnimClassik(FILE &fichier);   // Sauvegarde ces infos
        bool LoadFromFileAnimClassik(FILE &fichier); // Charge ces infos
		bool LoadFromFileAnimClassik(TFileVirtual &fichier);

   private:
        TAnimTileDecors();                 // Constructeur par d�faut inaccessible
};
//----------------------------------------------------------------------------//

//--- Liste de tous les mod�les d'objets/d�cors du jeu ---
extern TAnimTileDecors** ListeObjetsDecors;
extern int NbMaxTileDecors;
// Renvoie le 1er TAnimTileDecors trouv� dans 'ListeObjetsDecors' associ� au n� de TileDecors.DecorsNum
// Rmq : s�rement provisoire
TAnimTileDecors* getAnimTileDecors(int numTileDecors);
extern void  ChangeAnimTileDecorsVideoMode(); // Chgt de mode vid�o en plein cours du jeu



#endif
