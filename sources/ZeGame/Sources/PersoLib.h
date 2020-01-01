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
#include <list>				// Librairie STL de listes doublements chainées
#include "PhysicObject.h"	// Modelisation physique d'un objet quelconque
#include "consts.h"

//------------------------------------------------------------- classes externes
class AnimSprite;
class AnimTransition;
class SpriteTilePersoLib;
class SpriteFrameList;
class SpriteTileLib;
class DecorsObjet;


// types de personnages rencontrés
const int NbCharacters = 5;
#define HERO1			0
#define OGRE1			1
#define NAINRENEGAT1	2
#define MOINE1			3
#define NAIN1			4
#define PERSO			5  // Indéfini <=> à éviter

// types d'armes que peut utiliser le personnage
const int NbWeapons = 3;
#define PETOIRE1   2 	
#define EPEE1      1
#define NOWEAPON   0

class ClisteObjetFrame;  // Classe définie lus loin
class TFileVirtual;		 // Fichiers virtuels (en mémoire ou dans un .pak)
class MapZone;			 // Zone au sol

// Types de collisions utilisés lors des tests
enum eCollideMode { COLLIDE_PIXEL, COLLIDE_ZONE };
extern eCollideMode COLLIDE_MODE;    // collisions utilisant la zone-perso au sol


//========================= Classe CHARACTER =================================//
// -> informations et images *.bmp sur un modèle de personnage : OGRE, HERO ...
// Il n'y a qu'une instance de cette classe par type de personnages pour éviter
// les redondances d'informations et d'images qui sont similaires pour les mêmes
// types de personnages.
class Character
{   public:
        int  CharacterType;   		// Indique le type de modèle qu'il représente :  HERO, OGRE, NAIN ...
        int iCharacterStatus;		// Perso isolé ou non
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

        //=== New depuis l'éditeur de persos du 18/12/98 ===
        enum eTypePerso { NONE=-1, UNKNOW=0, HUMAIN=1 };
        eTypePerso typePerso;
        // Sauvegarde toutes les informations sur les sprites d'un personnage donné
        void SaveToFileInfoSprites(FILE &fichier);
        // Charge toutes les informations sur les sprites d'un personnage donné
        void LoadFromFileInfoSprites(FILE &fichier);
		void LoadFromFileInfoSprites(TFileVirtual &fichier);
        // Charge tout le contenu du fichier .mdl contenant toutes les informations
        // de chaque tiles des anims du persos ...
        void LoadMdl(const char* MdlName);

        //=== New depuis la superposition de sprites du 24/02/99 ===
        int nbMembres;                      // Nombres de membres du personnage
        ClisteObjetFrame *ObjetFrame;       // Tableau de nbMembres listes d'sObjetFrame
        int nbObjetFrames;                  // Nombres de frames superposées
        SpriteFrameList *FrameSuperpList;   // Liste des frames superposées concernant ce personnage
        SpriteTileLib *STLFrameSuperp;      // Librairie des sprites du perso
        void AllocateObjetBmp(int NumObjet);    // Charge les bmp relié à cet objet
        void DesallocateObjetBmp(int NumObjet); // Désalloue la mémoire occupée par les bmp représentant cet objet
   protected:
        Character();    // Constructeur par défaut interdit
};
//--- Liste de tous les modèles de personnages du jeu ---
extern Character* ListeCharacters[NbCharacters];
extern void  ChangeListeCharactersVideoMode();		// Chgt de mode vidéo en plein cours du jeu


//=== Structure indiquant si une case aux alentours du perso ================//
//    est oui ou non occupée  par un objet du décors
struct CaseOccupee
{   bool occupee;               // indique si cette structure contient un objet du décors valide
	int posU;					// position de l'objet/décors se trouvant devant le perso
	int posV;
    DecorsObjet *decors;		// ptr sur ce décors
    DWORD decID;				// n°ID de ce décors
    double dist;				// Distance (en nb de cases) entre le perso et le décors
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
        Character*  ModelePerso;       // Modele de personnage utilisé pour sa représentation
        int         ModelePersoType;   // Indique le type de modèle actuellement pointé : HERO, OGRE ...

        // Animations & Etats
    	int peEtat;   			  // MARCHE, COURS, ARRET ...
		int peFutEtat;			  // futur état souhaité
        int peDirection;		  // 8 orientations : or_N, or_NE, or_S ...
        int peFutDirection;		  // future direction souhaitée
        int peNumSprite;		  // n° du sprite de l'animation
        int peNbSprite;			  // nb de sprties de l'animation courante
        int peSprNumLib;		  // n° du sprite ds sa librairie
        bool peAllCollisions;     // valable pour le Héro mais pas pour les PNJ
		float peAnimSpeed;		  // Vitesse d'animation	
		float peNumSpriteInterm;  // N° de sprite intermédiaire

        // Vitesses & Etats
        int peOldEtat;            // ancien état du personnage
        int peCollideEtat;        // état dans lequel on a détecté une collision
        int peCollideDirection;	  // Direction dans laquelle on vient de détecter une collision
		bool peHasJustCollide;	  // Indique si la frame d'avant on a subi une collision	
        bool peChangeAnim;        // indique si l'on vient de changer d'anim
        double peSpeed;           // vitesse actuelle du personnage
        double peFutSpeed;        // vitesse souhaitée
		int	peUserFutSpeed;		  // % de la vitesse souhaitee a laquelle le joueur veut aller
		int	peOldUserFutSpeed;	  // ancienne valeur
        double peOldSpeed;        // vitesse passée du perso
        double pedVit;            // Accélération partielle calculée à partir de la viteses souhaitée et de la vitesse actuelle lors d'une transition
        bool peAccelerate;        // Accélère ou déccélère
        bool peSpeedTrans;        // Changement de vitesse oui/non

        // Coordonées des pieds perso sur la carte :
        // - par rapport au sommet de la carte en losange du niveau dessiné
        int peX,peY;              // en pixels
        int peU, peV;             // en nombre de losanges
        double pedX,pedY;	      // facteur de déplacement

        // - par rapport au sommet de la carte 2D du niveau vue du dessus
        double PersoNivX, PersoNivY;   // en mètres
        double PersoNivU, PersoNivV;   // en nombre de cases du tableau 2D

        //=== New depuis la superposition de sprites du 24/02/99 ===
        int ArmeEnMain;                // Pour l'instant : arme qu'utilise le perso

        CaseOccupee	 FrontObject;	   // Objet potentiel se trouvant devant le perso

		//=== New 9/1/0 (provisoire) -> gestion des tps de réponse ===
		unsigned int	uLastActionUse;			// Heure à laquelle le joueur a dernièrement fait une action 'utiliser'
		unsigned int	uTimeIntActionUse;		// Tps de réponse (en ms) entre 2 actions 'utiliser' à la suite
		unsigned int	uLastActionHit;			// Heure à laquelle le joueur a dernièrement fait une action 'frapper'
		unsigned int	uTimeIntActionHit;		// Tps de réponse (en ms) entre 2 actions 'frapper' à la suite

		//=== New 12/04/2000 : conservation des zones ===
		std::list<int> lInsideZone;

		int  iEndAnimSynchro;		  // indique si l'on doit prévenir par mutex qu'une anim est finie

		//=== New 16/09/2000 (provisoire) -> gestion des collisions ===
		unsigned int	uLastCollisionTime;		// Heure à laquelle le joueur est entre en collision avec quelque chose
		unsigned int	uTimeIntCollision;		// Temps minimum entre 2 collisions


	//--- Fonctions & Procédures privées ---
    private:
      	// Transfert des informations de l'action courante vers le perso :
        //  => peSprNumLib, peNbSprite ...
        void ResultAction(void);
        // Calcul du déplacement du perso dù à divers Facteurs
        //  -  Deplacement "bidon" du personnage à partir des coordonnées directes sur la carte ISO
        void Deplacement(void);
        //  -  Deplacement "réaliste" du personnage à partir des coordonnées sur la carte réelle
        void DeplacementRealist(void);
        // Effectue les réglages de transitions entre 2 anims
        void ChangementAnim(void);
		// Comportement du joueur en cas de collisions
		void collisionDetected(void);

		float computeAnimSpeed(int userFutSpeed);

		//--- Methodes heritees de PhysicGame ---
	public:	
		// Calcul des degats causes et deduction des points de vie
		virtual void hitBy(Personnage* opponent);

        //--- Fonctions & Procédures publiques ---
    public:
		Personnage(int PersoType);	        // Constructeur : doit donner le type de sprites à utiliser
        void SetPosition(double x, double y);   // Positionne le perso sur le niveau
        // Défini le futur état du perso à partir des touches du clavier et de l'état courant
        virtual void Keyb2FutEtat(void);
        // Met à jour le perso, qu'il soit joueur ou non joueur
        virtual void Update();
        // Défini les nouveaux états à partir de ceux demandés
        void Fut2PresEtat(void);
        void peBouge();			        // Bouge le personnage
        void peFastDraw(DDSurface *pScreen);    // Affiche le perso sur la surface <pscreen>

        //=== New depuis le 17/01/99 ===
        // Va chercher les coordonnées du point représentant le centre de gravité
        // du personnage projeté au sol
        int GravSolX();
        int GravSolY();

        // outils de convertion des divers coordonnées du perso
        void PersoNivXY2UV();        // Convertion des PersoNiv(X,Y) en PersoNiv(U,V)
        void PersoNivUV2XY();        // Convertion des PersoNiv(U,V) en PersoNiv(X,Y)
        void PersoNivXYtoPeXY();     // Convertion des PersoNiv(X,Y) en pe(X,Y)
        void PersoPeXYtoNivXY();	 // Convertion de pe(X,Y) en PersoNiv(x,Y)

        // outils de détection des collisions
        typedef enum { HAUT_GAUCHE, HAUT_DROIT, BAS_DROIT, BAS_GAUCHE } eCorner;
        typedef enum { HAUT, BAS } eVertical;
        typedef enum { GAUCHE, DROITE } eHorizontal;
        // Detecte les collisions éventuelles avec le décors
        bool DetectColis_Decors(double &newPersoNivX, double &newPersoNivY, double &newPersoNivU, double &newPersoNivV, int &newPeX, int &newPeY, int &newPeU, int &newPeV);
        // Detecte les collisions avec les petits décors circulaires
        bool DetectColis_DecorsCircle(double &newPersoNivX, double &newPersoNivY, double &newPersoNivU, double &newPersoNivV, int &newPeX, int &newPeY, int &newPeU, int &newPeV);
		// Detection des collisions avec les autres personnages
		bool DetectColis_Personnages(double &newPersoNivX, double &newPersoNivY, double &newPersoNivU, double &newPersoNivV, int &newPeX, int &newPeY, int &newPeU, int &newPeV);

        // Vérifie si dans une certaine direction, à partir d'un certains points on peut rencontrer des obstacles
        bool DetectColis_Possibility(double posNivX, double posNivY, int direction);
        //  Calcul les coords (U,V) des points (px,py) et retourne le coin du losange dans lequel il se trouve
        void ConvertXYCtoUVC(int px, int py,int &pu, int &pv, eVertical &vert, eHorizontal &horiz,
                             bool strictGauche, bool strictBas);
        // Détecte si un objet du décors est présent devant lui à un certains rayon (exprimé en nb de cases)
        bool DetectObjet_Rayon(int rayonint, CaseOccupee& BlocObjet);
		
		// Renvoi vrai si l'un des 2 persos (au moins) se dirige dans la direction de l'autre
		bool isMovingTowards(Personnage *perso);
		// Renvoi la direction cardinale la plus coherente entre les positions de 2 personnages
		eMapDirection getDirection(Personnage* dest);

        // Modifie les coordonnées du perso
  		void SetCoords(const double &X,const double &Y, const double &U, const double &V, int pex, int pey, int peu, int pev);
		void SetCoords(double X, double Y);

		// Indique si ce personnage est à l'intérieur d'une zone
		bool isCharacterInZone(MapZone *zone);

		// Fonctions liées aux zones dans lesquelles se trouve le perso
		bool isInInsideZone(int id);		// Est-il dans la zone n°'id'
		void DelInsideZone(int id);			// Efface telle zone
		void AddInsideZone(int id);			// Ajoute une zone
		std::list<int>& GetListInsideZone() { return lInsideZone; }

		Personnage* GetPersoTouche();		// Renvoi un ptr vers le personnage que l'on vient de toucher
		bool isInsideCircle(double& centX, double &centY, double rayon);	// Indique si le personnage se trouve dans ce cercle ou pas
};
//----------------------------------------------------------------------------//

//=== Classe ClisteObjetFrame ================================================//
// Gère une liste d'objets de couple numObjet-numFrame
class ClisteObjetFrame
{ // Structure associant :
  //   - un type d'objet : armes, armure, habit ...
  //   - un n° de frame d'anim superposée (Epee, armure ...)
  //   - le n° de la frame originale
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
  // Recherche du numéro de la frame d'aprés un type d'objet, renvoi :
  //  - 0 si pas trouvé dans la liste
  //  - ou numFramAnimlSuperposee.
  int Search(int numTypObj);
};
//================= Fin de la classe ClisteObjetFrame ========================//


//--- Fonctions de convertion supplémentaires ---
void ConvNivXY2UV(double X, double Y, double &U, double &V);
void ConvNivUV2XY(double U, double V, double &X, double &Y);
void ConvNivUVtoPeXY(double U, double V, int &pex, int &pey);
void ConvNivPeXYtoUV(int pex, int pey, double &U, double &V);
void ConvNivXYtoAll(double X, double Y, double &U, double &V, int &pex, int &pey);
void ConvNivUVtoAll(double U, double V, double &X, double &Y, int &pex, int &pey);
void TransUVtoXY(int U, int V, int *X, int *Y);


extern Personnage *Hero,*Ogre;          // 2 Personnages Joueurs


#endif
