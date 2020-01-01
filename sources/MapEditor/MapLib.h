////////////////////////////////////////////////////////////////////////////////
// MapLib.h  :  D�finitions de tous les structures d'une carte				  //
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


/*== Donn�es d'un trap�ze cod�es sur 32 bits =================================//
typedef struct
{
    unsigned int  NumObjet;     		// n� de l'objet auquel appartient cette case
    BOOL	  Principal;			// indique si cette case est le trap�ze principal d'un objet (Rmq : c'est con d'utiliser 8bits pour ��)
    unsigned char Infos;	  		// ??? infos suppl�mentaires � d�terminer ???
} TCase;
//----------------------------------------------------------------------------*/

class MapZoneList;

//== Donn�es d'un trap�ze cod�es sur 32 bits =================================//
typedef struct sCase
{   DecorsObjet   *ptrObjet;                    // encore mieux ke le n� => son pointeur
//    BOOL	  Principal;			// indique si cette case est le trap�ze principal d'un objet (Rmq : c'est con d'utiliser 8bits pour �� => faire un flag)
//    BOOL          Accessible;                 // indique si le perso peut marcher dessus
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
    sCase():Infos(0),ptrObjet(NULL) { };
    // note : pas tr�s optimis� -> donc � OPTIMISER une fois le jeu de tests accomplis
} TCase;
//----------------------------------------------------------------------------//

//== Classe d'une Carte ======================================================//
class TMap
{
  public:
     TCase **Carte;				// Tableau dynamique des cases
     int   MapWidth, MapHeight;		        // Dimension de la carte en nombre de trap�zes
  public:
     TMap();				        // Constructeur
     ~TMap();					// Destructeur
     void AlloueCarte(int Width, int Height);	// Initialise le tableau de cases
     void DessaloueCarte();			// Lib�re la m�moire
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
     void AfficheMap();					    // Affiche la carte � l'�cran
     void AfficheFastMap();				    // Affiche toute la carte plus rapidement
     void AfficheSelectDecors();	// Affiche le d�cors sur le pointeur de la souris
     void AfficheObjDecors(DecorsObjet *DecObj, int U, int V);  // Affiche le d�cors

     // Infos de la souris
     int SourisX, SourisY;			// Coords (X,Y) de la souris sur la FICHE   (change � chaque �v�nement)
     int MouseX, MouseY;            // Coords (X,Y) de la souris sur la GRILLE
     int MouseU, MouseV;			// Coords (U,V) de la souris sur la grille
     int Pointeur;					// indique le type de pointage auquel on a affaire
     BOOL ObjDecorsSelectionne;	    // indique si le pointeur s�lectionne qqchose
     BOOL ObjDecorsSelect;			// indique si l'objet-d�cors visible est s�lectionn�
     BOOL ObjDecorsPlacerDessus;	// indique si l'objet � placer ne se trouve pas d�j� par dessus un trap�ze d'un objet
     BOOL ObjPeindre;				// indique si le bouton est abaiss� pour peindre
     BOOL ObjGomme;					// permet de gommer les tiles du sols
     BOOL ObjTracerZone;           	// indique si le bouton gauche est abaiss� pour tracer une zone
     BOOL ObjZoneSelect;			// Vrai si une zone est s�lectionn�e
     void MouseInfos();				// Calcule ses diff�rentes coordonn�es

     // Infos sur l'affichage de la carte
     BOOL VIEWTRAPPRINC;				// Affiche oui ou non le trap�ze principal � l'�cran

     //--- New depuis le 13/05/1999 ---
     int ScreenViewX, ScreenViewY;
     void Upgrade();                // Met � jour la carte : en particulier avec les objets anim�s du d�cors rafraichis
     int OldScreenX, OldScreenY;		           // anciennes valeures de (ScreenX,ScreenY)
     int OldScreenU, OldScreenV;		           //    "        "            "   U  "    V          
};
//----------------------------------------------------------------------------//
extern TViewMap *ViewMap;

//** NOUVEAU *****************************************************************//

//== Classe d'un �tage de la Map =============================================//
class TEtage
{
   public:
     //*** Nouveau ***
     BOOL Reference;				// indique si c'est l'�tage de r�f�rence
     int  Altitude;				// Altitude de l'�tage ( Reference = 0)
     // Dimension de l'�tage en nombre de trap�zes => � consever car les autres �tages pourront �tre + petits ke le principal
     int EtageWidth, EtageHeight;
     int EtageU, EtageV;			// Coordonn�es des �tages secondaires / principal (0,0)

     // Layers composants un �tage
     TMap *LayerSols;				// Layer du sol/fond
     TMap *LayerDecors;				// Layer des d�cors

     // fonctions
     TEtage();					// Constructeur
     ~TEtage();					// Destructeur
     void AlloueCarte(int Width, int Height);	// Initialise le tableau de cases
};

class TMapNiveau
{
   public:
     //*** Ancien ***
     int   MapNum;              // N� de la carte
     char  Name [15];		// Nom de la carte
     char  DTLibName [15];	// Nom de la librairie des d�cors

     TEtage  *PrincEtage;   	// EtagePrincipal pour toutes les r�f�rences
     int   NbEtage;		// Nb d'�tage, 1 minimum == PrincEtage
     int   NivWidth, NivHeight; // Dimensions de l'�tage principal

     TMapNiveau();              // Constructeur
     ~TMapNiveau();             // Destructeur
     void SetInfos(int num,char nom[15]);		 // Fournie des donn�es � la carte
     void PrepareLevel(int PrincWidth, int PrincHeight); // Pr�pare la carte au chargement

	 // [NEW] depuis le 17/01/2000
	 MapZoneList	*pMapZoneList;		// Liste des zones pouvant se trouver sur la carte
};

extern TMapNiveau *MapNiveau;


//=== New depuis le 25/03/1999 : utilis� dans algo de Path-Finding ===========//


#endif
