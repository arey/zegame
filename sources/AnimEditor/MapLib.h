////////////////////////////////////////////////////////////////////////////////
// MapLib.h  :  D�finitions de tous les structures d'une carte				  //
// 	=> version bis r�nov�e pour le jeu (provient de l'�diteur)				  //
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


//== Donn�es d'un trap�ze cod�es sur 32 bits =================================//
typedef struct
{   DecorsObjet   *ptrObjet;                    // encore mieux ke le n� => son pointeur
//    BOOL	  Principal;			// indique si cette case est le trap�ze principal d'un objet (Rmq : c'est con d'utiliser 8bits pour �� => faire un flag)
//    BOOL          Accessible;                   // indique si le perso peut marcher dessus
  protected :
    unsigned char Infos;			// Infos principales sur certains bits :
  public :
    // - bit 0 : BOOL Principal   => indique si cette case est le trap�ze principal d'un objet
    // - bit 1 : BOOL Accessible  => indique si le perso peut marcher dessus
    bool isPrincipal()   { return Infos &  0x01; }
    void setPrincipal(bool s) { if (s) Infos |= 0x01; else Infos &= 0xFE;}
    bool isAccessible()  { return Infos & 0x02; }
    void setAccessible(bool s) { if (s) Infos |= 0x02; else Infos &= 0xFD; }
    void resetInfos() { Infos = 0; }

    eTrapCOLLIDE TrapCollide;                   // Type de collsisions � effectuer
    // note : pas tr�s optimis� -> donc � OPTIMISER une fois le jeu de tests accomplis
} TCase;
//----------------------------------------------------------------------------//

//== Utilis� pour le tra�age des layers ==
enum eAffichMode { LAYER_SOL, LAYER_DECORS, LAYER_ALL };
enum eBlitMode   { RECALCULE , DOUBLE_BUFFERING_SM , CALCUL_TAB };

//== Classe d'une Carte ======================================================//
class TMap
{
  public:
     TCase **Carte;					// Tableau dynamique des cases
     int   MapWidth, MapHeight;		// Dimension de la carte en nombre de trap�zes
  public:
  	 TMap();						// Constructeur
     ~TMap();						// Destructeur
     void AlloueCarte(int Width, int Height);	// Initialise le tableau de cases
     void DessaloueCarte();						// Lib�re la m�moire
};
//----------------------------------------------------------------------------//
//extern TMap *Map;

//== Classe de la vue ur la carte ============================================//
class TViewMap
{
   private:
     int NbTrapPerScreenWidth, 		// Nombre de trap�zes par longeur et
         NbTrapPerScreenHeight;         // largeur d'�cran
   public:
     int MapPixWidth, MapPixHeight;  	// Nombres de pixels par hauteur de carte
     int ScreenX, ScreenY;		// Coordonn�es du bord de l'�cran sur la carte en Pixels
     int ScreenU, ScreenV;		// Coordonn�es du bord de l'�cran sur la carte en Trap�zes
     int dx, dy;			// offset de d�placement � partir du trap�ze du coin d'�cran

     TViewMap();		        // Constructeur;
     void CalcInfos(int MapTrapWidth, int MapTrapHeight);   // Calcule des diff�rents trucs
     void PosCoinUV();					    // Transforme les coordonn�es (X,Y) en (U,V)
     void TransUVtoXY(int U, int V, int *X, int *Y); 	    // changement de rep�re ISO->Cart.
     void TransXYtoUV(int X, int Y, int *U, int*V);  	    // changement de rep�re Cart.->ISO
     int  TransUVtoY(int U, int V);                         // renvoit le Y correspondant au coords (U,V)   
     void AfficheFastMap(DDSurface *pScreen, eAffichMode Layer, int ScrX, int ScrY); // Affiche toute la carte plus rapidement
     void AfficheMap(DDSurface *pScreen, eAffichMode Layer, int ScrX, int ScrY);     // Affiche toute la carte plus rapidement
     void AfficheObjDecors(DecorsObjet *DecObj, DDSurface *pScreen, int ScrX, int ScrY, int keyTrap = -1);  // Affiche le d�cors

     // Infos de la souris => On s'en sert pas ici (...now...)
     int SourisX, SourisY;    		// Coords (X,Y) de la souris sur la FICHE   (change � chaque �v�nement)
     int MouseX, MouseY;                // Coords (X,Y) de la souris sur la GRILLE
     int MouseU, MouseV;      		// Coords (U,V) de la souris sur la grille
     int Pointeur;	                // indique le type de pointage auquel on a affaire
     void MouseInfos();			// Calcule ses diff�rentes coordonn�es

     //*** New ***
     // Taille de l'�cran (utilis� pour, par exemple, ne pas afficher une partie du d�cors sous l'interface
     int ScreenViewX, ScreenViewY;

     DDSurface *pDDSBackFond1, *pDDSBackFond2;	   // permet le scrolling
     BOOL NewCamera;				   // indique si l'on a chang� de vue => on recalcule le sols
     int OldScreenX, OldScreenY;		   // anciennes valeures de (ScreenX,ScreenY)
     int OldScreenU, OldScreenV;		   //    "        "            "   U  "    V
     void AfficheViewCamera();			   // affiche la vue globale de la cam�ra
     eBlitMode BlitMode;			   // diff�rents types d'affichage
     void CompleteMap(DDSurface *pScreen, int dx, int dy, int ScrX, int ScrY);	// compl�te le layer manquant
};
//----------------------------------------------------------------------------//
extern TViewMap *ViewMap;


//== Classe d'un �tage de la Map =============================================//
class TEtage
{
   public:
     //*** Nouveau ***
     BOOL Reference;					// indique si c'est l'�tage de r�f�rence
     int  Altitude;						// Altitude de l'�tage ( Reference = 0)
     // Dimension de l'�tage en nombre de trap�zes => � consever car les autres �tages pourront �tre + petits ke le principal
     int EtageWidth, EtageHeight;
     int EtageU, EtageV;				// Coordonn�es des �tages secondaires / principal (0,0)

     // Layers composants un �tage
     TMap *LayerSols;					// Layer du sol/fond
     TMap *LayerDecors;					// Layer des d�cors

     // fonctions
     TEtage();							// Constructeur
     ~TEtage();							// Destructeur
     void AlloueCarte(int Width, int Height);	// Initialise le tableau de cases
};

class TMapNiveau
{
   public:
     //*** Ancien ***
     int   MapNum;                  // N� de la carte
     char  Name [15];				// Nom de la carte
     char  DTLibName [15];			// Nom de la librairie des d�cors

     TEtage  *PrincEtage;   	// EtagePrincipal pour toutes les r�f�rences
     int   NbEtage;				// Nb d'�tage, 1 minimum == PrincEtage
     int   NivWidth, NivHeight; // Dimensions de l'�tage principal

     TMapNiveau();              // Constructeur
     ~TMapNiveau();             // Destructeur
     void SetInfos(int num,char nom[15]);				 // Fournie des donn�es � la carte
     void PrepareLevel(int PrincWidth, int PrincHeight); // Pr�pare la carte au chargement
};
extern TMapNiveau *MapNiveau;

//** NOUVEAU *****************************************************************//

//== Fonctions Chargement de map *.niv =======================================//
extern void LoadMapNiveau(TMapNiveau *MapNiv, AnsiString NivName);

//== Classe du RAFRAICHISSEMENT ==============================================//
class TRefresh
{
  public:
    BOOL   RefreshFixed;			// Indique si le taux de rafraichissment est fix�
    double FramePerSecond;			// Nb de frame/sec souhait� (en sec)
    DWORD  FrameInterval;			// Interval de temps entre 2 rafraichissement (en ms)

    int	BestFreqAvail;				// Possibilit� d'avoir une meilleur fonction que GetTickCount()
    LARGE_INTEGER BestFreq;			// requis par QueryPerformanceCounter
    DWORD LastFrameTiming;			// Temps lors du dernier rafraichissement  (en ms)

    TRefresh();						// Constructeur
    DWORD GetTime();				// Renvoie le temps �coul� depuis le d�marrage de l'ordi (en ms)
    void  NewFps(double fps);		// calcule tout ce qui est relatif au frame/sec
    BOOL  FlipFrame();				// V�rifie si l'on peut changer de frame
};
extern TRefresh *FlipRefresh;

//== Classe de Scrolling du sol ==============================================//
class TSolScroll
{
  public:
    int NbSizeBloc;					// D�coupage d'un �cran en Blocs : nb de blocs / hauteur
    int BlocWidth, BlocHeight;		// Dimension d'un bloc
    int ResWidth, ResHeight;		// R�solution actuelle			

    int TabScreenX, TabScreenY; 	// Coords du coin sup�rieur-gauche du tableau des blocs sur la map
    DDSurface  ***TabBloc;			// Tableau dynamique des surfaces [BlocWidth+2, BlocHeight+2]

    int Tabi, Tabj;					// R�f�rence du bloc ds lequel appartient le coin sup�rieur-gauche de MapView (0 ou 1)
    int BlocU, BlocV;				// Position de MapView ds le bloc (Tabi,Tabj)
    BOOL SpecialU, SpecialV;        // indique si l'on est ds un cas particulier => BlocU=0 ou BlocV=0

    TSolScroll(int nbblocs, int screenW, int screenH);	// Constructeur
    ~TSolScroll();										// Destructeur
    void DrawTabBloc(int tabscrX, int tabscrY);				// Remplit les DDSurfaces de TabBloc;
    void DrawBloc(int blocI, int blocJ);						// Recalcule juste un bloc
    void Tab2Screen(int PosX, int PosY);				// Dessine la Tab � l'�cran
    void Tab2Left();									// D�calement de la table � gauche
    void Tab2Right();									// D�calement de la table � droite
    void Tab2Bottom();									// D�calement de la table � droite
    void Tab2Top();										// D�calement de la table � droite
};
extern TSolScroll *SolScroll;
//============================================================================//


//=== Classe TYBuffer ========================================================//
//=> Permet de trier une liste d'objets � afficher dans l'ordre : persos,
//   d�cors, ...
class  Personnage;   // Classe utilis�e mais d�finie dans un autre module (PersoLib.h)
class TYBuffer
{ public:
    // Type d'objets rencontr�s dans le YBuffer;
    enum eTypeObjY { UNKNOW, OBJDECORS, PERSOS };

  private:
    // Structure simulant une classe abstraite
    typedef struct tObjetAffichable
    { eTypeObjY    typeObjY;          // Type d'objet � afficher
      DecorsObjet* ptrDecorsObjet;
      Personnage*  ptrPersonnage;
      int KeyTrap;                    // N� d'un trap�ze cl� du d�cors

      // Constructeurs inlines
      tObjetAffichable():                      // Objet ind�fini
        typeObjY(UNKNOW),ptrDecorsObjet(NULL),ptrPersonnage(NULL),KeyTrap(-1) {};
      tObjetAffichable(DecorsObjet* i_ptr):    // Decors
        typeObjY(OBJDECORS),ptrDecorsObjet(i_ptr),ptrPersonnage(NULL),KeyTrap(-1) {};
      tObjetAffichable(DecorsObjet* i_ptr, int i_keyTrap):  // Decors d�coup� en lamelles
        typeObjY(OBJDECORS),ptrDecorsObjet(i_ptr),ptrPersonnage(NULL),KeyTrap(i_keyTrap) {};
      tObjetAffichable(Personnage* i_ptr):     // perso
        typeObjY(PERSOS),ptrDecorsObjet(NULL),ptrPersonnage(i_ptr),KeyTrap(-1) {};
    } sObjetAffichable;

    // D�fini une liste tri�e sur Y d'objets � afficher
    typedef std::multimap<int,sObjetAffichable> tFileObjetAffichable;
    tFileObjetAffichable YFile;

  public:
    void Clear();                                    // Efface la YFile  
    void AddDecors(int cY, DecorsObjet* pDecors);   // Ajoute � la liste un d�cors
    void AddPersos(int cY, Personnage*  pPersos);   // Ajoute � la liste un perso
    void Affiche(DDSurface *pScreen);                // Affiche � l'�cran tous les objets de la liste                                  
};
extern TYBuffer YBuffer;  // Seule instance de cette classe 
//============================================================================//

#endif
