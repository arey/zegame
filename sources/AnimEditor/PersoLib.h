//---------------------------------------------------------------------------
#ifndef PersoLibH
#define PersoLibH
#include "MapLib.h"
#include "AnimAll.h"

// types de personnages rencontrés
const int NbCharacters = 3;
#define HERO1			0
#define OGRE1			1
#define NAINRENEGAT1	2
#define PERSO   		3  // Indéfini <=> à éviter

// types d'armes que peut utiliser le personnage
const int NbWeapons = 2;
#define EPEE1      1
#define NOWEAPON   0

class ClisteObjetFrame;  // Classe définie lus loin

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
        int  CharacterType;   // Indique le type de modèle qu'il représente :  HERO, OGRE, NAIN ...
        Character(int characType);      // Constructeur

        int peNbAnim;  		          // nb d'animations ds la librairie
        AnimSprite** AnimSpriteLib;	  // Tableau dynamique de ttes les animations  ("Animall.cpp")
        int peNbTrans;			  // nb de transitions ds la librairie
        AnimTransition** AnimTransLib;    // Tableau dynamique de ttes les transitions entre anims  ("Animall.cpp")

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
        // Charge tout le contenu du fichier .mdl contenant toutes les informations
        // de chaque tiles des anims du persos ...
        void LoadMdl(const char* MdlName);
        bool SaveMdl(const char* MdlName);

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


//========================== Classe PERSONNAGE ===============================//
// Attributs de tous les personnages du jeu quelque soit leur fonction : Hero,
// PNJ, monstres ...
class  Personnage
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
        bool peAllCollisions;             // valable pour le Héro mais pas pour les PNJ

        // Vitesses & Etats
        int peOldEtat;                    // ancien état du personnage
        int peCollideEtat;                // état dans lequel on a détecté une collision
        double peSpeed;                   // vitesse actuelle du personnage
        double peFutSpeed;                // vitesse souhaitée
        double peOldSpeed;                // vitesse passée du perso
        double pedVit;                    // Accélération partielle calculée à partir de la viteses souhaitée et de la vitesse actuelle lors d'une transition
        bool peAccelerate;                // Accélère ou déccélère
        bool peSpeedTrans;                // Changement de vitesse oui/non

        // Coordonées des pieds perso sur la carte :
        // - par rapport au sommet de la carte en losange du niveau dessiné
        int peX,peY;                   // en pixels
        int peU, peV;                  // en nombre de losanges
        double pedX,pedY;	       // facteur de déplacement

        // - par rapport au sommet de la carte 2D du niveau vue du dessus
        double PersoNivX, PersoNivY;   // en mètres
        double PersoNivU, PersoNivV;   // en nombre de cases du tableau 2D

        //=== New depuis la superposition de sprites du 24/02/99 ===
        int ArmeEnMain;                     // Pour l'instant : arme qu'utilise le perso

	//--- Fonctions & Procédures privées ---

      	// Transfert des informations de l'action courante vers le perso :
        //  => peSprNumLib, peNbSprite ...
        void ResultAction(void);
    private:
        // Calcul du déplacement du perso dù à divers Facteurs
        //  -  Deplacement "bidon" du personnage à partir des coordonnées directes sur la carte ISO
        void Deplacement(void);
        //  -  Deplacement "réaliste" du personnage à partir des coordonnées sur la carte réelle
        void DeplacementRealist(void);
        // Effectue les réglages de transitions entre 2 anims
        void ChangementAnim(void);

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
        void PersoNivUV2XV();        // Convertion des PersoNiv(U,V) en PersoNiv(X,Y)
        void PersoNivXYtoPeXY();     // Convertion des PersoNiv(X,Y) en pe(X,Y)

        // outils de détection des collisions
        typedef enum { HAUT_GAUCHE, HAUT_DROIT, BAS_DROIT, BAS_GAUCHE } eCorner;
        typedef enum { HAUT, BAS } eVertical;
        typedef enum { GAUCHE, DROITE } eHorizontal;
        bool DetectColis_Decors();   // Detecte les collisions éventuelles avec le décors
        //  Calcul les coords (U,V) des points (px,py) et retourne le coin du losange dans lequel il se trouve
        void ConvertXYCtoUVC(int px, int py,int &pu, int &pv, eVertical &vert, eHorizontal &horiz,
                             bool strictGauche, bool strictBas);

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

#endif
