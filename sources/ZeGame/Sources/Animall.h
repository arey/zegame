#ifndef Animall
#define Animall

#include "DDLib.h"

// Définitions des états
#define MARCHE1		0
#define	ARRET1		1
#define	PRENDCOUP1	2
#define TOURNER1	3
#define TOURNER2	4
#define DEMITOUR1	5
#define DONNECOUP1	6
#define MEURT1		7
#define	MORT1		8
#define COURSE1         9
#define STOPNET1       10         // le perso doit stopper net lorsqu'il y a collisions
#define STAND1         11
#define COUPEPEE1      12
#define MANGE1         13
#define FINIMANGER1    14
#define BRISE1         15
#define BRISE2         16
#define CASSER1        17
#define CASSER2        18
#define TIRER1         19
#define SORTIR1        20
#define SORTI1	       21
#define REBUILD1	   22
#define TRAVAIL1       23
#define TRAVAIL2       24
#define COUPHACHE1	   25
#define	SAUTE1		   26
#define RESSUSCITE1	   27	
#define NOSTATE      9999

// Définitions des 8 Orientations
#define or_O   0
#define or_SO  1
#define or_S   2
#define or_SE  3
#define or_E   4
#define or_NE  5
#define or_N   6
#define or_NO  7


// Défini les caractéristiques d'une animation
//  * les anims on toutes un nb d'image fixe pour les 8 directions
//  *  pour le type 1, les positions se suivent toujours (contrairement au type 2)
class AnimSprite
{
	public:
	AnimSprite(void);		// Constructeur;

	public :
    	char Nom [12];			//  nom de l'animation
        int  Numero;			//  n° remplacant le nom
        int  Type;				//  type de l'animation  1, 2, 3 ou 4
        int  NbSprites;			//  nombre d'images de l'animation pour 1 orientaion donc x8
		float fAnimSpeed;		//  Vitesse de l'anim (1 = vitesse normale)  [New 10/02/00]

        // Type 1, le plus classique
        int FirstSprite;   		// n° image de départ;

        // Type 2, pour anims éparpillées "arrêt", "tourner à l'arrêt"
        int* SprRepertoire[8];          // tableau de pointers int ressençant les  NbSprites n° des anims
};

// Défini les caractéristiques d'uns transition entre 2 anims
// ImageA = ImageB = -1 <=> n  -> transition à tout moment possible
class AnimTransition
{
	public:
	AnimTransition(void);   	// Constructeur
    public:
    	int  NumActCour;		// n° action présente
        int  NumActFut;			// n° future action
        int  ImageA,ImageB;		// n°image départ -> n°image arrivée  (-1 correspond au cas n)
        int  NumActInterm;		// n° action intermédiaire (souvent = NumActFut);
};

class Character;

//=== Charge un fichier texte .ina d'animations des PERSOS ===
void LoadFileIna(const char* InaName,AnimSprite*** pASL, AnimTransition*** pATL,
                 int* pNbAnim,int* pNbTrans,SpriteTilePersoLib **STLPerso,
                 Character* Individu);

class TAnimTileDecors;
//=== Charge un fichier texte .ina d'animations des DECORS ===
void LoadFileInaDecors(const char* InaNameTresShort,AnimSprite*** pASL, AnimTransition*** pATL,int &pNbAnim,int &pNbTrans,SpriteTilePersoLib **STLObjets, TAnimTileDecors *elem);

//--- Fonctions annexes ---
void EtatToString(int etat, char* texte);     // Convertit le n° de l'Etat en chaîne de caractère
int ValEtat(const char* phrase);              // Convertie la chaîne de caractère de l'état en n°   


#endif
