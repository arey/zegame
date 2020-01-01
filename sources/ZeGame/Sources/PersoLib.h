////////////////////////////////////////////////////////////////////////////////
//  File		:	PersoLib.h												  //
//                                                                            //
//	Description :   Classe de base gerant les personnages : Hero, PNJ, Streums//
//                                                                            //
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@ifrance.com		                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef PersoLibH
#define PersoLibH

//---------------------------------------------------------------------- Include
#ifdef CBUILDER
#include <stdio.h>
#endif
#include "DDLib.h"			// Librairie DirectDraw maison
#include "GameObject.h"		// Classe de base de DecorsObjet
#include <list>				// Librairie STL de listes doublements chain�es
#include "PhysicObject.h"	// Modelisation physique d'un objet quelconque
#include "consts.h"

//------------------------------------------------------------- classes externes
class AnimSprite;
class AnimTransition;
class SpriteTilePersoLib;
class SpriteFrameList;
class SpriteTileLib;
class DecorsObjet;


// types de personnages rencontr�s
const int NbCharacters = 5;
#define HERO1			0
#define OGRE1			1
#define NAINRENEGAT1	2
#define MOINE1			3
#define NAIN1			4
#define PERSO			5  // Ind�fini <=> � �viter

// types d'armes que peut utiliser le personnage
const int NbWeapons = 3;
#define PETOIRE1   2 	
#define EPEE1      1
#define NOWEAPON   0

class ClisteObjetFrame;  // Classe d�finie lus loin
class TFileVirtual;		 // Fichiers virtuels (en m�moire ou dans un .pak)
class MapZone;			 // Zone au sol

// Types de collisions utilis�s lors des tests
enum eCollideMode { COLLIDE_PIXEL, COLLIDE_ZONE };
extern eCollideMode COLLIDE_MODE;    // collisions utilisant la zone-perso au sol


//========================= Classe CHARACTER =================================//
// -> informations et images *.bmp sur un mod�le de personnage : OGRE, HERO ...
// Il n'y a qu'une instance de cette classe par type de personnages pour �viter
// les redondances d'informations et d'images qui sont similaires pour les m�mes
// types de personnages.
class Character
{   public:
        int  CharacterType;   		// Indique le type de mod�le qu'il repr�sente :  HERO, OGRE, NAIN ...
        int iCharacterStatus;		// Perso isol� ou non
		Character(int characType, int status=0);    // Constructeur

		enum { CHARACTER_FREE=0, CHARACTER_STATUE };
		
        int peNbAnim;  		          	// nb d'animations ds la librairie
        AnimSprite** AnimSpriteLib;	  	// Tableau dynamique de ttes les animations  ("Animall.cpp")
        int peNbTrans;			  		// nb de transitions ds la librairie
        AnimTransition** AnimTransLib;	// Tableau dynamique de ttes les transitions entre anims  ("Animall.cpp")

        // Librairie des sprites
        SpriteTilePersoLib* STLPersonnage;
		// Charge l'*.ina contenant toutes les informations sur les Animations et les Transitions
  		void LoadIna(const char* InaName,SpriteTilePersoLib **STLPerso);

        //=== New depuis l'�diteur de persos du 18/12/98 ===
        enum eTypePerso { NONE=-1, UNKNOW=0, HUMAIN=1 };
        eTypePerso typePerso;
        // Sauvegarde toutes les informations sur les sprites d'un personnage donn�
        void SaveToFileInfoSprites(FILE &fichier);
        // Charge toutes les informations sur les sprites d'un personnage donn�
        void LoadFromFileInfoSprites(FILE &fichier);
		void LoadFromFileInfoSprites(TFileVirtual &fichier);
        // Charge tout le contenu du fichier .mdl contenant toutes les informations
        // de chaque tiles des anims du persos ...
        void LoadMdl(const char* MdlName);

        //=== New depuis la superposition de sprites du 24/02/99 ===
        int nbMembres;                      // Nombres de membres du personnage
        ClisteObjetFrame *ObjetFrame;       // Tableau de nbMembres listes d'sObjetFrame
        int nbObjetFrames;                  // Nombres de frames superpos�es
        SpriteFrameList *FrameSuperpList;   // Liste des frames superpos�es concernant ce personnage
        SpriteTileLib *STLFrameSuperp;      // Librairie des sprites du perso
        void AllocateObjetBmp(int NumObjet);    // Charge les bmp reli� � cet objet
        void DesallocateObjetBmp(int NumObjet); // D�salloue la m�moire occup�e par les bmp repr�sentant cet objet
   protected:
        Character();    // Constructeur par d�faut interdit
};
//--- Liste de tous les mod�les de personnages du jeu ---
extern Character* ListeCharacters[NbCharacters];
extern void  ChangeListeCharactersVideoMode();		// Chgt de mode vid�o en plein cours du jeu


//=== Structure indiquant si une case aux alentours du perso ================//
//    est oui ou non occup�e  par un objet du d�cors
struct CaseOccupee
{   bool occupee;               // indique si cette structure contient un objet du d�cors valide
	int posU;					// position de l'objet/d�cors se trouvant devant le perso
	int posV;
    DecorsObjet *decors;		// ptr sur ce d�cors
    DWORD decID;				// n�ID de ce d�cors
    double dist;				// Distance (en nb de cases) entre le perso et le d�cors
    // Constructeur
    CaseOccupee():	occupee(false), posU(-1), posV(-1), decors(NULL), decID(0), dist(-1) { };
};
//----------------------------------------------------------------------------//


//========================== Classe PERSONNAGE ===============================//
// Attributs de tous les personnages du jeu quelque soit leur fonction : Hero,
// PNJ, monstres ...
class  Personnage : public GameObject, public PhysicObject
{       //--- Variables & Constantes publiques ---
     public:
        Character*  ModelePerso;       // Modele de personnage utilis� pour sa repr�sentation
        int         ModelePersoType;   // Indique le type de mod�le actuellement point� : HERO, OGRE ...

        // Animations & Etats
    	int peEtat;   			  // MARCHE, COURS, ARRET ...
		int peFutEtat;			  // futur �tat souhait�
        int peDirection;		  // 8 orientations : or_N, or_NE, or_S ...
        int peFutDirection;		  // future direction souhait�e
        int peNumSprite;		  // n� du sprite de l'animation
        int peNbSprite;			  // nb de sprties de l'animation courante
        int peSprNumLib;		  // n� du sprite ds sa librairie
        bool peAllCollisions;     // valable pour le H�ro mais pas pour les PNJ
		float peAnimSpeed;		  // Vitesse d'animation	
		float peNumSpriteInterm;  // N� de sprite interm�diaire

        // Vitesses & Etats
        int peOldEtat;            // ancien �tat du personnage
        int peCollideEtat;        // �tat dans lequel on a d�tect� une collision
        int peCollideDirection;	  // Direction dans laquelle on vient de d�tecter une collision
		bool peHasJustCollide;	  // Indique si la frame d'avant on a subi une collision	
        bool peChangeAnim;        // indique si l'on vient de changer d'anim
        double peSpeed;           // vitesse actuelle du personnage
        double peFutSpeed;        // vitesse souhait�e
		int	peUserFutSpeed;		  // % de la vitesse souhaitee a laquelle le joueur veut aller
		int	peOldUserFutSpeed;	  // ancienne valeur
        double peOldSpeed;        // vitesse pass�e du perso
        double pedVit;            // Acc�l�ration partielle calcul�e � partir de la viteses souhait�e et de la vitesse actuelle lors d'une transition
        bool peAccelerate;        // Acc�l�re ou d�cc�l�re
        bool peSpeedTrans;        // Changement de vitesse oui/non

        // Coordon�es des pieds perso sur la carte :
        // - par rapport au sommet de la carte en losange du niveau dessin�
        int peX,peY;              // en pixels
        int peU, peV;             // en nombre de losanges
        double pedX,pedY;	      // facteur de d�placement

        // - par rapport au sommet de la carte 2D du niveau vue du dessus
        double PersoNivX, PersoNivY;   // en m�tres
        double PersoNivU, PersoNivV;   // en nombre de cases du tableau 2D

        //=== New depuis la superposition de sprites du 24/02/99 ===
        int ArmeEnMain;                // Pour l'instant : arme qu'utilise le perso

        CaseOccupee	 FrontObject;	   // Objet potentiel se trouvant devant le perso

		//=== New 9/1/0 (provisoire) -> gestion des tps de r�ponse ===
		unsigned int	uLastActionUse;			// Heure � laquelle le joueur a derni�rement fait une action 'utiliser'
		unsigned int	uTimeIntActionUse;		// Tps de r�ponse (en ms) entre 2 actions 'utiliser' � la suite
		unsigned int	uLastActionHit;			// Heure � laquelle le joueur a derni�rement fait une action 'frapper'
		unsigned int	uTimeIntActionHit;		// Tps de r�ponse (en ms) entre 2 actions 'frapper' � la suite

		//=== New 12/04/2000 : conservation des zones ===
		std::list<int> lInsideZone;

		int  iEndAnimSynchro;		  // indique si l'on doit pr�venir par mutex qu'une anim est finie

		//=== New 16/09/2000 (provisoire) -> gestion des collisions ===
		unsigned int	uLastCollisionTime;		// Heure � laquelle le joueur est entre en collision avec quelque chose
		unsigned int	uTimeIntCollision;		// Temps minimum entre 2 collisions


	//--- Fonctions & Proc�dures priv�es ---
    private:
      	// Transfert des informations de l'action courante vers le perso :
        //  => peSprNumLib, peNbSprite ...
        void ResultAction(void);
        // Calcul du d�placement du perso d� � divers Facteurs
        //  -  Deplacement "bidon" du personnage � partir des coordonn�es directes sur la carte ISO
        void Deplacement(void);
        //  -  Deplacement "r�aliste" du personnage � partir des coordonn�es sur la carte r�elle
        void DeplacementRealist(void);
        // Effectue les r�glages de transitions entre 2 anims
        void ChangementAnim(void);
		// Comportement du joueur en cas de collisions
		void collisionDetected(void);

		float computeAnimSpeed(int userFutSpeed);

		//--- Methodes heritees de PhysicGame ---
	public:	
		// Calcul des degats causes et deduction des points de vie
		virtual void hitBy(Personnage* opponent);

        //--- Fonctions & Proc�dures publiques ---
    public:
		Personnage(int PersoType);	        // Constructeur : doit donner le type de sprites � utiliser
        void SetPosition(double x, double y);   // Positionne le perso sur le niveau
        // D�fini le futur �tat du perso � partir des touches du clavier et de l'�tat courant
        virtual void Keyb2FutEtat(void);
        // Met � jour le perso, qu'il soit joueur ou non joueur
        virtual void Update();
        // D�fini les nouveaux �tats � partir de ceux demand�s
        void Fut2PresEtat(void);
        void peBouge();			        // Bouge le personnage
        void peFastDraw(DDSurface *pScreen);    // Affiche le perso sur la surface <pscreen>

        //=== New depuis le 17/01/99 ===
        // Va chercher les coordonn�es du point repr�sentant le centre de gravit�
        // du personnage projet� au sol
        int GravSolX();
        int GravSolY();

        // outils de convertion des divers coordonn�es du perso
        void PersoNivXY2UV();        // Convertion des PersoNiv(X,Y) en PersoNiv(U,V)
        void PersoNivUV2XY();        // Convertion des PersoNiv(U,V) en PersoNiv(X,Y)
        void PersoNivXYtoPeXY();     // Convertion des PersoNiv(X,Y) en pe(X,Y)
        void PersoPeXYtoNivXY();	 // Convertion de pe(X,Y) en PersoNiv(x,Y)

        // outils de d�tection des collisions
        typedef enum { HAUT_GAUCHE, HAUT_DROIT, BAS_DROIT, BAS_GAUCHE } eCorner;
        typedef enum { HAUT, BAS } eVertical;
        typedef enum { GAUCHE, DROITE } eHorizontal;
        // Detecte les collisions �ventuelles avec le d�cors
        bool DetectColis_Decors(double &newPersoNivX, double &newPersoNivY, double &newPersoNivU, double &newPersoNivV, int &newPeX, int &newPeY, int &newPeU, int &newPeV);
        // Detecte les collisions avec les petits d�cors circulaires
        bool DetectColis_DecorsCircle(double &newPersoNivX, double &newPersoNivY, double &newPersoNivU, double &newPersoNivV, int &newPeX, int &newPeY, int &newPeU, int &newPeV);
		// Detection des collisions avec les autres personnages
		bool DetectColis_Personnages(double &newPersoNivX, double &newPersoNivY, double &newPersoNivU, double &newPersoNivV, int &newPeX, int &newPeY, int &newPeU, int &newPeV);

        // V�rifie si dans une certaine direction, � partir d'un certains points on peut rencontrer des obstacles
        bool DetectColis_Possibility(double posNivX, double posNivY, int direction);
        //  Calcul les coords (U,V) des points (px,py) et retourne le coin du losange dans lequel il se trouve
        void ConvertXYCtoUVC(int px, int py,int &pu, int &pv, eVertical &vert, eHorizontal &horiz,
                             bool strictGauche, bool strictBas);
        // D�tecte si un objet du d�cors est pr�sent devant lui � un certains rayon (exprim� en nb de cases)
        bool DetectObjet_Rayon(int rayonint, CaseOccupee& BlocObjet);
		
		// Renvoi vrai si l'un des 2 persos (au moins) se dirige dans la direction de l'autre
		bool isMovingTowards(Personnage *perso);
		// Renvoi la direction cardinale la plus coherente entre les positions de 2 personnages
		eMapDirection getDirection(Personnage* dest);

        // Modifie les coordonn�es du perso
  		void SetCoords(const double &X,const double &Y, const double &U, const double &V, int pex, int pey, int peu, int pev);
		void SetCoords(double X, double Y);

		// Indique si ce personnage est � l'int�rieur d'une zone
		bool isCharacterInZone(MapZone *zone);

		// Fonctions li�es aux zones dans lesquelles se trouve le perso
		bool isInInsideZone(int id);		// Est-il dans la zone n�'id'
		void DelInsideZone(int id);			// Efface telle zone
		void AddInsideZone(int id);			// Ajoute une zone
		std::list<int>& GetListInsideZone() { return lInsideZone; }

		Personnage* GetPersoTouche();		// Renvoi un ptr vers le personnage que l'on vient de toucher
		bool isInsideCircle(double& centX, double &centY, double rayon);	// Indique si le personnage se trouve dans ce cercle ou pas
};
//----------------------------------------------------------------------------//

//=== Classe ClisteObjetFrame ================================================//
// G�re une liste d'objets de couple numObjet-numFrame
class ClisteObjetFrame
{ // Structure associant :
  //   - un type d'objet : armes, armure, habit ...
  //   - un n� de frame d'anim superpos�e (Epee, armure ...)
  //   - le n� de la frame originale
 public:
  typedef struct sobjetframe
  {  int numIDTypeObjet;
     int numFrameAnimSuperposee;
     int numIDFrameOriginal;
     // Constructeur
     sobjetframe(int &i_O,int &i_Fr, int &i_FO):
        numIDTypeObjet(i_O),numFrameAnimSuperposee(i_Fr),numIDFrameOriginal(i_FO) { };
  }  sObjetFrame;

  std::vector <sObjetFrame> TabObjetFrame;  // Tableau d'sObjetFrame

  // nombre d'sObjetFrame dans le tableau
  int NbObjetFrame() { return TabObjetFrame.size(); }
  // insertion d'un couple <objet,Anim> dans le tableau
  void Add(int &i_Objet,int &i_Frame,int &i_FrameOrig)
  {  TabObjetFrame.push_back(sobjetframe(i_Objet,i_Frame,i_FrameOrig));
  };
  // Recherche du num�ro de la frame d'apr�s un type d'objet, renvoi :
  //  - 0 si pas trouv� dans la liste
  //  - ou numFramAnimlSuperposee.
  int Search(int numTypObj);
};
//================= Fin de la classe ClisteObjetFrame ========================//


//--- Fonctions de convertion suppl�mentaires ---
void ConvNivXY2UV(double X, double Y, double &U, double &V);
void ConvNivUV2XY(double U, double V, double &X, double &Y);
void ConvNivUVtoPeXY(double U, double V, int &pex, int &pey);
void ConvNivPeXYtoUV(int pex, int pey, double &U, double &V);
void ConvNivXYtoAll(double X, double Y, double &U, double &V, int &pex, int &pey);
void ConvNivUVtoAll(double U, double V, double &X, double &Y, int &pex, int &pey);
void TransUVtoXY(int U, int V, int *X, int *Y);


extern Personnage *Hero,*Ogre;          // 2 Personnages Joueurs


#endif
