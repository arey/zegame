////////////////////////////////////////////////////////////////////////////////
// MapLib.h  :  Définitions de tous les structures d'une carte				  //
////////////////////////////////////////////////////////////////////////////////
#ifndef MapLib_H
#define MapLib_H

#include "ImDecors2.h"

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


/*== Données d'un trapèze codées sur 32 bits =================================//
typedef struct
{
    unsigned int  NumObjet;     		// n° de l'objet auquel appartient cette case
    BOOL	  Principal;			// indique si cette case est le trapèze principal d'un objet (Rmq : c'est con d'utiliser 8bits pour çà)
    unsigned char Infos;	  		// ??? infos supplémentaires à déterminer ???
} TCase;
//----------------------------------------------------------------------------*/

class MapZoneList;

//== Données d'un trapèze codées sur 32 bits =================================//
typedef struct sCase
{   DecorsObjet   *ptrObjet;                    // encore mieux ke le n° => son pointeur
//    BOOL	  Principal;			// indique si cette case est le trapèze principal d'un objet (Rmq : c'est con d'utiliser 8bits pour çà => faire un flag)
//    BOOL          Accessible;                 // indique si le perso peut marcher dessus
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
    sCase():Infos(0),ptrObjet(NULL) { };
    // note : pas trés optimisé -> donc à OPTIMISER une fois le jeu de tests accomplis
} TCase;
//----------------------------------------------------------------------------//

//== Classe d'une Carte ======================================================//
class TMap
{
  public:
     TCase **Carte;				// Tableau dynamique des cases
     int   MapWidth, MapHeight;		        // Dimension de la carte en nombre de trapèzes
  public:
     TMap();				        // Constructeur
     ~TMap();					// Destructeur
     void AlloueCarte(int Width, int Height);	// Initialise le tableau de cases
     void DessaloueCarte();			// Libère la mémoire
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
     void AfficheMap();					    // Affiche la carte à l'écran
     void AfficheFastMap();				    // Affiche toute la carte plus rapidement
     void AfficheSelectDecors();	// Affiche le décors sur le pointeur de la souris
     void AfficheObjDecors(DecorsObjet *DecObj, int U, int V);  // Affiche le décors

     // Infos de la souris
     int SourisX, SourisY;			// Coords (X,Y) de la souris sur la FICHE   (change à chaque évènement)
     int MouseX, MouseY;            // Coords (X,Y) de la souris sur la GRILLE
     int MouseU, MouseV;			// Coords (U,V) de la souris sur la grille
     int Pointeur;					// indique le type de pointage auquel on a affaire
     BOOL ObjDecorsSelectionne;	    // indique si le pointeur sélectionne qqchose
     BOOL ObjDecorsSelect;			// indique si l'objet-décors visible est sélectionné
     BOOL ObjDecorsPlacerDessus;	// indique si l'objet à placer ne se trouve pas déjà par dessus un trapèze d'un objet
     BOOL ObjPeindre;				// indique si le bouton est abaissé pour peindre
     BOOL ObjGomme;					// permet de gommer les tiles du sols
     BOOL ObjTracerZone;           	// indique si le bouton gauche est abaissé pour tracer une zone
     BOOL ObjZoneSelect;			// Vrai si une zone est sélectionnée
     void MouseInfos();				// Calcule ses différentes coordonnées

     // Infos sur l'affichage de la carte
     BOOL VIEWTRAPPRINC;				// Affiche oui ou non le trapèze principal à l'écran

     //--- New depuis le 13/05/1999 ---
     int ScreenViewX, ScreenViewY;
     void Upgrade();                // Met à jour la carte : en particulier avec les objets animés du décors rafraichis
     int OldScreenX, OldScreenY;		           // anciennes valeures de (ScreenX,ScreenY)
     int OldScreenU, OldScreenV;		           //    "        "            "   U  "    V          
};
//----------------------------------------------------------------------------//
extern TViewMap *ViewMap;

//** NOUVEAU *****************************************************************//

//== Classe d'un étage de la Map =============================================//
class TEtage
{
   public:
     //*** Nouveau ***
     BOOL Reference;				// indique si c'est l'étage de référence
     int  Altitude;				// Altitude de l'étage ( Reference = 0)
     // Dimension de l'étage en nombre de trapèzes => à consever car les autres étages pourront être + petits ke le principal
     int EtageWidth, EtageHeight;
     int EtageU, EtageV;			// Coordonnées des étages secondaires / principal (0,0)

     // Layers composants un étage
     TMap *LayerSols;				// Layer du sol/fond
     TMap *LayerDecors;				// Layer des décors

     // fonctions
     TEtage();					// Constructeur
     ~TEtage();					// Destructeur
     void AlloueCarte(int Width, int Height);	// Initialise le tableau de cases
};

class TMapNiveau
{
   public:
     //*** Ancien ***
     int   MapNum;              // N° de la carte
     char  Name [15];		// Nom de la carte
     char  DTLibName [15];	// Nom de la librairie des décors

     TEtage  *PrincEtage;   	// EtagePrincipal pour toutes les références
     int   NbEtage;		// Nb d'étage, 1 minimum == PrincEtage
     int   NivWidth, NivHeight; // Dimensions de l'étage principal

     TMapNiveau();              // Constructeur
     ~TMapNiveau();             // Destructeur
     void SetInfos(int num,char nom[15]);		 // Fournie des données à la carte
     void PrepareLevel(int PrincWidth, int PrincHeight); // Prépare la carte au chargement

	 // [NEW] depuis le 17/01/2000
	 MapZoneList	*pMapZoneList;		// Liste des zones pouvant se trouver sur la carte
};

extern TMapNiveau *MapNiveau;


//=== New depuis le 25/03/1999 : utilisé dans algo de Path-Finding ===========//


#endif
