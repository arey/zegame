////////////////////////////////////////////////////////////////////////////////
// MapLib.h  :  Définitions de tous les structures d'une carte				  //
// 	=> version bis rénovée pour le jeu (provient de l'éditeur)				  //
////////////////////////////////////////////////////////////////////////////////
#ifndef MapLib
#define MapLib

//#include <windows.h>
#include <map>
#include "ImDecors2.h"
#include "PersoLib.h"
#include "DDLib.h"

/*               (0,0)
                  /\
                 /  \
           _____/\__/\_________
          |    /  \/  \        |
          |   /\  /\  /\       |
          |  /  \/  \/  \      |
          | /\  /\  /\  /\     |
          |/  \/  \/  \/  \    |
          |\  /\  /\  /\  /    |
          | \/  \/  \/  \/     |
          |__\__/\__/\__/______|
              \/  \/  \/
               \  /\  /
                \/  \/
                 \  /
                  \/
*/


//== Données d'un trapèze codées sur 32 bits =================================//
typedef struct
{   DecorsObjet   *ptrObjet;                    // encore mieux ke le n° => son pointeur
//    BOOL	  Principal;			// indique si cette case est le trapèze principal d'un objet (Rmq : c'est con d'utiliser 8bits pour çà => faire un flag)
//    BOOL          Accessible;                   // indique si le perso peut marcher dessus
  protected :
    unsigned char Infos;			// Infos principales sur certains bits :
  public :
    // - bit 0 : BOOL Principal   => indique si cette case est le trapèze principal d'un objet
    // - bit 1 : BOOL Accessible  => indique si le perso peut marcher dessus
    bool isPrincipal()   { return Infos &  0x01; }
    void setPrincipal(bool s) { if (s) Infos |= 0x01; else Infos &= 0xFE;}
    bool isAccessible()  { return Infos & 0x02; }
    void setAccessible(bool s) { if (s) Infos |= 0x02; else Infos &= 0xFD; }
    void resetInfos() { Infos = 0; }

    eTrapCOLLIDE TrapCollide;                   // Type de collsisions à effectuer
    // note : pas trés optimisé -> donc à OPTIMISER une fois le jeu de tests accomplis
} TCase;
//----------------------------------------------------------------------------//

//== Utilisé pour le traçage des layers ==
enum eAffichMode { LAYER_SOL, LAYER_DECORS, LAYER_ALL };
enum eBlitMode   { RECALCULE , DOUBLE_BUFFERING_SM , CALCUL_TAB };

//== Classe d'une Carte ======================================================//
class TMap
{
  public:
     TCase **Carte;					// Tableau dynamique des cases
     int   MapWidth, MapHeight;		// Dimension de la carte en nombre de trapèzes
  public:
  	 TMap();						// Constructeur
     ~TMap();						// Destructeur
     void AlloueCarte(int Width, int Height);	// Initialise le tableau de cases
     void DessaloueCarte();						// Libère la mémoire
};
//----------------------------------------------------------------------------//
//extern TMap *Map;

//== Classe de la vue ur la carte ============================================//
class TViewMap
{
   private:
     int NbTrapPerScreenWidth, 		// Nombre de trapèzes par longeur et
         NbTrapPerScreenHeight;         // largeur d'écran
   public:
     int MapPixWidth, MapPixHeight;  	// Nombres de pixels par hauteur de carte
     int ScreenX, ScreenY;		// Coordonnées du bord de l'écran sur la carte en Pixels
     int ScreenU, ScreenV;		// Coordonnées du bord de l'écran sur la carte en Trapèzes
     int dx, dy;			// offset de déplacement à partir du trapèze du coin d'écran

     TViewMap();		        // Constructeur;
     void CalcInfos(int MapTrapWidth, int MapTrapHeight);   // Calcule des différents trucs
     void PosCoinUV();					    // Transforme les coordonnées (X,Y) en (U,V)
     void TransUVtoXY(int U, int V, int *X, int *Y); 	    // changement de repère ISO->Cart.
     void TransXYtoUV(int X, int Y, int *U, int*V);  	    // changement de repère Cart.->ISO
     int  TransUVtoY(int U, int V);                         // renvoit le Y correspondant au coords (U,V)   
     void AfficheFastMap(DDSurface *pScreen, eAffichMode Layer, int ScrX, int ScrY); // Affiche toute la carte plus rapidement
     void AfficheMap(DDSurface *pScreen, eAffichMode Layer, int ScrX, int ScrY);     // Affiche toute la carte plus rapidement
     void AfficheObjDecors(DecorsObjet *DecObj, DDSurface *pScreen, int ScrX, int ScrY, int keyTrap = -1);  // Affiche le décors

     // Infos de la souris => On s'en sert pas ici (...now...)
     int SourisX, SourisY;    		// Coords (X,Y) de la souris sur la FICHE   (change à chaque évènement)
     int MouseX, MouseY;                // Coords (X,Y) de la souris sur la GRILLE
     int MouseU, MouseV;      		// Coords (U,V) de la souris sur la grille
     int Pointeur;	                // indique le type de pointage auquel on a affaire
     void MouseInfos();			// Calcule ses différentes coordonnées

     //*** New ***
     // Taille de l'écran (utilisé pour, par exemple, ne pas afficher une partie du décors sous l'interface
     int ScreenViewX, ScreenViewY;

     DDSurface *pDDSBackFond1, *pDDSBackFond2;	   // permet le scrolling
     BOOL NewCamera;				   // indique si l'on a changé de vue => on recalcule le sols
     int OldScreenX, OldScreenY;		   // anciennes valeures de (ScreenX,ScreenY)
     int OldScreenU, OldScreenV;		   //    "        "            "   U  "    V
     void AfficheViewCamera();			   // affiche la vue globale de la caméra
     eBlitMode BlitMode;			   // différents types d'affichage
     void CompleteMap(DDSurface *pScreen, int dx, int dy, int ScrX, int ScrY);	// complète le layer manquant
};
//----------------------------------------------------------------------------//
extern TViewMap *ViewMap;


//== Classe d'un étage de la Map =============================================//
class TEtage
{
   public:
     //*** Nouveau ***
     BOOL Reference;					// indique si c'est l'étage de référence
     int  Altitude;						// Altitude de l'étage ( Reference = 0)
     // Dimension de l'étage en nombre de trapèzes => à consever car les autres étages pourront être + petits ke le principal
     int EtageWidth, EtageHeight;
     int EtageU, EtageV;				// Coordonnées des étages secondaires / principal (0,0)

     // Layers composants un étage
     TMap *LayerSols;					// Layer du sol/fond
     TMap *LayerDecors;					// Layer des décors

     // fonctions
     TEtage();							// Constructeur
     ~TEtage();							// Destructeur
     void AlloueCarte(int Width, int Height);	// Initialise le tableau de cases
};

class TMapNiveau
{
   public:
     //*** Ancien ***
     int   MapNum;                  // N° de la carte
     char  Name [15];				// Nom de la carte
     char  DTLibName [15];			// Nom de la librairie des décors

     TEtage  *PrincEtage;   	// EtagePrincipal pour toutes les références
     int   NbEtage;				// Nb d'étage, 1 minimum == PrincEtage
     int   NivWidth, NivHeight; // Dimensions de l'étage principal

     TMapNiveau();              // Constructeur
     ~TMapNiveau();             // Destructeur
     void SetInfos(int num,char nom[15]);				 // Fournie des données à la carte
     void PrepareLevel(int PrincWidth, int PrincHeight); // Prépare la carte au chargement
};
extern TMapNiveau *MapNiveau;

//** NOUVEAU *****************************************************************//

//== Fonctions Chargement de map *.niv =======================================//
extern void LoadMapNiveau(TMapNiveau *MapNiv, AnsiString NivName);

//== Classe du RAFRAICHISSEMENT ==============================================//
class TRefresh
{
  public:
    BOOL   RefreshFixed;			// Indique si le taux de rafraichissment est fixé
    double FramePerSecond;			// Nb de frame/sec souhaité (en sec)
    DWORD  FrameInterval;			// Interval de temps entre 2 rafraichissement (en ms)

    int	BestFreqAvail;				// Possibilité d'avoir une meilleur fonction que GetTickCount()
    LARGE_INTEGER BestFreq;			// requis par QueryPerformanceCounter
    DWORD LastFrameTiming;			// Temps lors du dernier rafraichissement  (en ms)

    TRefresh();						// Constructeur
    DWORD GetTime();				// Renvoie le temps écoulé depuis le démarrage de l'ordi (en ms)
    void  NewFps(double fps);		// calcule tout ce qui est relatif au frame/sec
    BOOL  FlipFrame();				// Vérifie si l'on peut changer de frame
};
extern TRefresh *FlipRefresh;

//== Classe de Scrolling du sol ==============================================//
class TSolScroll
{
  public:
    int NbSizeBloc;					// Découpage d'un écran en Blocs : nb de blocs / hauteur
    int BlocWidth, BlocHeight;		// Dimension d'un bloc
    int ResWidth, ResHeight;		// Résolution actuelle			

    int TabScreenX, TabScreenY; 	// Coords du coin supérieur-gauche du tableau des blocs sur la map
    DDSurface  ***TabBloc;			// Tableau dynamique des surfaces [BlocWidth+2, BlocHeight+2]

    int Tabi, Tabj;					// Référence du bloc ds lequel appartient le coin supérieur-gauche de MapView (0 ou 1)
    int BlocU, BlocV;				// Position de MapView ds le bloc (Tabi,Tabj)
    BOOL SpecialU, SpecialV;        // indique si l'on est ds un cas particulier => BlocU=0 ou BlocV=0

    TSolScroll(int nbblocs, int screenW, int screenH);	// Constructeur
    ~TSolScroll();										// Destructeur
    void DrawTabBloc(int tabscrX, int tabscrY);				// Remplit les DDSurfaces de TabBloc;
    void DrawBloc(int blocI, int blocJ);						// Recalcule juste un bloc
    void Tab2Screen(int PosX, int PosY);				// Dessine la Tab à l'écran
    void Tab2Left();									// Décalement de la table à gauche
    void Tab2Right();									// Décalement de la table à droite
    void Tab2Bottom();									// Décalement de la table à droite
    void Tab2Top();										// Décalement de la table à droite
};
extern TSolScroll *SolScroll;
//============================================================================//


//=== Classe TYBuffer ========================================================//
//=> Permet de trier une liste d'objets à afficher dans l'ordre : persos,
//   décors, ...
class  Personnage;   // Classe utilisée mais définie dans un autre module (PersoLib.h)
class TYBuffer
{ public:
    // Type d'objets rencontrés dans le YBuffer;
    enum eTypeObjY { UNKNOW, OBJDECORS, PERSOS };

  private:
    // Structure simulant une classe abstraite
    typedef struct tObjetAffichable
    { eTypeObjY    typeObjY;          // Type d'objet à afficher
      DecorsObjet* ptrDecorsObjet;
      Personnage*  ptrPersonnage;
      int KeyTrap;                    // N° d'un trapèze clé du décors

      // Constructeurs inlines
      tObjetAffichable():                      // Objet indéfini
        typeObjY(UNKNOW),ptrDecorsObjet(NULL),ptrPersonnage(NULL),KeyTrap(-1) {};
      tObjetAffichable(DecorsObjet* i_ptr):    // Decors
        typeObjY(OBJDECORS),ptrDecorsObjet(i_ptr),ptrPersonnage(NULL),KeyTrap(-1) {};
      tObjetAffichable(DecorsObjet* i_ptr, int i_keyTrap):  // Decors découpé en lamelles
        typeObjY(OBJDECORS),ptrDecorsObjet(i_ptr),ptrPersonnage(NULL),KeyTrap(i_keyTrap) {};
      tObjetAffichable(Personnage* i_ptr):     // perso
        typeObjY(PERSOS),ptrDecorsObjet(NULL),ptrPersonnage(i_ptr),KeyTrap(-1) {};
    } sObjetAffichable;

    // Défini une liste triée sur Y d'objets à afficher
    typedef std::multimap<int,sObjetAffichable> tFileObjetAffichable;
    tFileObjetAffichable YFile;

  public:
    void Clear();                                    // Efface la YFile  
    void AddDecors(int cY, DecorsObjet* pDecors);   // Ajoute à la liste un décors
    void AddPersos(int cY, Personnage*  pPersos);   // Ajoute à la liste un perso
    void Affiche(DDSurface *pScreen);                // Affiche à l'écran tous les objets de la liste                                  
};
extern TYBuffer YBuffer;  // Seule instance de cette classe 
//============================================================================//

#endif
