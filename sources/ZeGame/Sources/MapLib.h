////////////////////////////////////////////////////////////////////////////////
//  File		:	MapLib.h												  //
//                                                                            //
//	Description :   Définitions de tous les structures d'une carte			  //
// 				   => version bis rénovée pour le jeu (provient de l'éditeur) //
//                                                                            //
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef MapLib
#define MapLib

#include <map>
#include "ImDecors2.h"
#include "DDLib.h"

//------------------------------------------------------------- classes externes
class Personnage;		// classe définie dans "PersoLib.h"
class DecorsObjet;		// classe   "		"  "ImDecors2.h"
class MapZoneList;		// liste des zones de la cartes défnies dans "MapZone.h"


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
struct TCase
{   DecorsObjet   *ptrObjet;        // encore mieux ke le n° => son pointeur
//    BOOL	  Principal;			// indique si cette case est le trapèze principal d'un objet (Rmq : c'est con d'utiliser 8bits pour çà => faire un flag)
//    BOOL          Accessible;     // indique si le perso peut marcher dessus
  protected :
    unsigned char Infos;			// Infos principales sur certains bits :
  public :
    // - bit 0 : BOOL Principal   => indique si cette case est le trapèze principal d'un objet
    // - bit 1 : BOOL Accessible  => indique si le perso peut marcher dessus
    bool isPrincipal()   { return (Infos &  0x01) == 0x01; }
    void setPrincipal(bool s) { if (s) Infos |= 0x01; else Infos &= 0xFE;}
    bool isAccessible()  { return (Infos & 0x02) == 0x02; }
    void setAccessible(bool s) { if (s) Infos |= 0x02; else Infos &= 0xFD; }
    void resetInfos() { Infos = 0; }

    eTrapCOLLIDE TrapCollide;       // Type de collsisions à effectuer
    // note : pas trés optimisé -> donc à OPTIMISER une fois le jeu de tests accomplis
	// Constructeur par défaut
	TCase::TCase():ptrObjet(NULL) {  }
};
//----------------------------------------------------------------------------//


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

//=== Gère la position et le déplacement de la caméra ========================//
// Crée le 02/03/2000
class TCamera
{ //------------------------------------------------------------------ Attributs
  public:
	// Contôle de la caméra
	enum { CAMERA_MODE_DEPLACEMENT_HERO, CAMERA_MODE_DEPLACEMENT_SCENARIO };
	int iCameraDeplaceMode;		// Mode de déplacement de la caméra 
  
  private:
	float fCameraX, fCameraY;	// Position de la caméra sur l'écran (haut à gauche)
	float fCamDestX, fCamDestY;	// Point où doit se rendre la caméra
	enum { CAMERA_MODE_MOVE_LINEAR, CAMERA_MODE_MOVE_CURVE };
	int iCameraMoveMode;		// Méthode de déplacement : linéaire, curviligne ...
	bool bMoving;				// Indique si la caméra est en train de se déplacer
	
  //------------------------------------------------------------------- Méthodes
  public:
	TCamera();					// Constructeur par défaut
	// Déplacement brutal de la caméra en (posX, posY)
	void SetNewPosition(float posX, float posY);
	// Déplacement de la caméra en douceur vers (newX, newY)
	void MoveCamera(float newX, float newY, int time, int mode = CAMERA_MODE_MOVE_LINEAR);

};
//----------------------------------------------------------------------------//


//== Classe de la vue ur la carte ============================================//
class TViewMap : public TCamera
{
   private:
     int NbTrapPerScreenWidth, 		    // Nombre de trapèzes par longeur et
         NbTrapPerScreenHeight;         // largeur d'écran
   public:
     int MapPixWidth, MapPixHeight;  	// Nombres de pixels par hauteur de carte
     int ScreenX, ScreenY;		        // Coordonnées du bord de l'écran sur la carte en Pixels
     int ScreenU, ScreenV;		        // Coordonnées du bord de l'écran sur la carte en Trapèzes
     int dx, dy;			            // offset de déplacement à partir du trapèze du coin d'écran

     TViewMap();		                // Constructeur par défaut
	 ~TViewMap();		                // Destructeur
     void CalcInfos(int MapTrapWidth, int MapTrapHeight);   // Calcule des différents trucs
     void PosCoinUV();					                    // Transforme les coordonnées (X,Y) en (U,V)
     void TransUVtoXY(int U, int V, int *X, int *Y); 	    // changement de repère ISO->Cart.
     void TransXYtoUV(int X, int Y, int *U, int*V);  	    // changement de repère Cart.->ISO
     int  TransUVtoY(int U, int V);                         // renvoit le Y correspondant au coords (U,V)
     void AfficheFastMap(DDSurface *pScreen, eAffichMode Layer, int ScrX, int ScrY); // Affiche toute la carte plus rapidement
     void AfficheMap(DDSurface *pScreen, eAffichMode Layer, int ScrX, int ScrY);     // Affiche toute la carte plus rapidement
     void AfficheObjDecors(DecorsObjet *DecObj, DDSurface *pScreen, int ScrX, int ScrY, int keyTrap = -1);  // Affiche le décors

     // Infos de la souris => On s'en sert pas ici (...now...)
     int SourisX, SourisY;    	 // Coords (X,Y) de la souris sur la FICHE   (change à chaque évènement)
     int MouseX, MouseY;         // Coords (X,Y) de la souris sur la GRILLE
     int MouseU, MouseV;      	 // Coords (U,V) de la souris sur la grille
     int Pointeur;	             // indique le type de pointage auquel on a affaire
     void MouseInfos();			 // Calcule ses différentes coordonnées

     //*** New ***
     // Taille de l'écran (utilisé pour, par exemple, ne pas afficher une partie du décors sous l'interface
     int ScreenViewX, ScreenViewY;
	 void AfficheViewCamera();	// Affiche la vue globale de la caméra

     DDSurface *pDDSBackFond1, *pDDSBackFond2;	   // permet le scrolling
     BOOL NewCamera;				               // indique si l'on a changé de vue => on recalcule le sols
     int OldScreenX, OldScreenY;		           // anciennes valeures de (ScreenX,ScreenY)
     int OldScreenU, OldScreenV;		           //    "        "            "   U  "    V
     eBlitMode BlitMode;			               // différents types d'affichage
     void CompleteMap(DDSurface *pScreen, int dx, int dy, int ScrX, int ScrY);	// complète le layer manquant

     //--- New depuis le 13/05/1999 ---
     void Upgrade();                // Met à jour la carte : en particulier avec les objets animés du décors rafraichis
	 void ChangeSolScrollVideoMode(int newScrW, int newScrH);	// Changement de mode-vidéo : restauration et rechargement des surfaces
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

	 // [NEW] depuis le 17/01/2000
	 MapZoneList	*pMapZoneList;		// Liste des zones pouvant se trouver sur la carte
};
extern TMapNiveau *MapNiveau;

//** NOUVEAU *****************************************************************//

//== Fonctions Chargement de map *.niv =======================================//
extern void LoadMapNiveau(TMapNiveau *MapNiv, AnsiString NivName);
extern void UnLoadMapNiveau(TMapNiveau *MapNiv);

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
    void DrawTabBloc(int tabscrX, int tabscrY);			// Remplit les DDSurfaces de TabBloc;
    void DrawBloc(int blocI, int blocJ);				// Recalcule juste un bloc
    void Tab2Screen(int PosX, int PosY);				// Dessine la Tab à l'écran
    void Tab2Left();									// Décalement de la table à gauche
    void Tab2Right();									// Décalement de la table à droite
    void Tab2Bottom();									// Décalement de la table à droite
    void Tab2Top();										// Décalement de la table à droite
	void ChangeSolScrollVideoMode(int newScrW, int newScrH);	// Changement de mode-vidéo : restauration et rechargement des surfaces
};
extern TSolScroll *SolScroll;
//============================================================================//


//=== Classe TYBuffer ========================================================//
//=> Permet de trier une liste d'objets à afficher dans l'ordre : persos,
//   décors, ...
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
    void Clear();                                   // Efface la YFile
    void AddDecors(int cY, DecorsObjet* pDecors);   // Ajoute à la liste un décors
    void AddPersos(int cY, Personnage*  pPersos);   // Ajoute à la liste un perso
    void Affiche(DDSurface *pScreen);               // Affiche à l'écran tous les objets de la liste
};
extern TYBuffer YBuffer;  // Seule instance de cette classe
//============================================================================//

#endif
