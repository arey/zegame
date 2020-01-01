////////////////////////////////////////////////////////////////////////////////
// Donn�es de l'image du d�cors 											  //
////////////////////////////////////////////////////////////////////////////////

#define MaxChar 15			// nb max de caract�res ds 1 cha�ne


enum TrapTERRAIN { INCONNU=0, SABLE, TERRE, HERBE, GRAVIERS, BOIS, MARBRE, RACINE,
                   TRONC, MUR, OBJET };
const int TrapTERRAINNb = 11;
extern const char TrapTERRAINText [TrapTERRAINNb][20];


enum eTrapCOLLIDE { PARTIELLE = 0, TOTALE = 1, AUCUNE };
const int TrapCOLLIDENb = 2;
extern const char TrapCOLLIDEText [TrapCOLLIDENb][20];


// Structure d'un Trap�ze del version 0.98
struct Trapeze
{
    int TrapU, TrapV;			// Coords du trap�ze par rapport au trap�ze principal

    //*** Nouveau � partir de la version 0.9 ***
    int Hauteur;			// Hauteur de l'objet pr�sent sur la case / au sol
    TrapTERRAIN	Terrain;		// Type de terrain pr�sent sur le trap�ze
    BOOL Accessible;                    // indique si on peut y marcher oui ou non
    //*** Nouveau � partir de la version 0.95 ***
    eTrapCOLLIDE TrapCollide;           // Type de collisions � effectuer sur ce trap�ze
    //*** Nouveau � partir de la version 0.97 ***
    bool DrawTrapezekey;                // indique si ce trap�ze est un �l�ment clef de l'affichage
    int  lamX1, lamY1, lamX2, lamY2;    // lamelle � afficher pour ce trap�ze
    //*** Nouveau depuis la version 0.98 ***
    int reserved[8];
};

// **** Version 0.98 de la strucuture TRAPEZE (09/07/99) ***
// Chps de bits indiquant de quelle mani�re on doit afficher le tile du d�cors
#define TILEDECORS_DRAW_OPAQUE		0			// Sans rien de sp�cifique
#define TILEDECORS_DRAW_SHADOW		1			// Tiens compte de l'ombrage

// Vieille structure d'un Trap�ze de la version 0.90
struct OldTrapeze
{
    int TrapU, TrapV;			// Coords du trap�ze par rapport au trap�ze principal

    //*** Nouveau � partir de la version 0.9 ***
    int Hauteur;			// Hauteur de l'objet pr�sent sur la case / au sol
    TrapTERRAIN	Terrain;		// Type de terrain pr�sent sur le trap�ze
    BOOL Accessible;                    // indique si on peut y marcher oui ou non
    //*** Nouveau � partir de la version 0.95 ***
    eTrapCOLLIDE TrapCollide;           // Type de collisions � effectuer sur ce trap�ze
    //*** Nouveau � partir de la version 0.97 ***
    bool DrawTrapezekey;                // indique si ce trap�ze est un �l�ment clef de l'affichage
    int  lamX1, lamY1, lamX2, lamY2;    // lamelle � afficher pour ce trap�ze
};

//  TILE DU DECORS
struct TileDecors
{
    int  DecorsNum;		        // n� de r�f�rence servant � tout objet s'y r�f�rencant
    char Nom [MaxChar];		        // Nom du fichier
    int  Width, Height;			// Taille du tile
    unsigned int RVB;                   // Composante de la couleur transparente
    unsigned char NumRVB;		// n� de couleur transparente ds la palette
    int  NbTrapeze;			// Nombre de trap�zes s'y rattachant
    int  TrapPrincX, TrapPrincY;	// Coordonn�es du coin sup�rieur du trap�ze principal par rapport au coin sup�rieur-gauche du tile
    // TrapezTab[0] fait r�f�rence au trap�ze principal
    Trapeze *TrapezTab;			// Tableau dynamique pointant sur un trap�ze
    int DecorsCornerX, DecorsCornerY;	// ne sert que pour l'�diteur : sait o� replacer le tile

    //--- New depuis le 20/03/99 - version 0.97 ---
    bool TotalDraw;                     // Indique si l'objet doit s'afficher en une seule fois ou par tranches verticales

    //--- New depuis le 08/06/99 - version 0.98 ---
    bool CircleCollision;               // Indique si l'on doit utiliser une collision circulaire
    int  CircleCenterX, CircleCenterY;  // Coords du centre du cercle relatives � (TrapPrincX,TrapPrincY)
    int  CircleRadius;                  // Rayon du cercle
    int  DrawMethode;					// Indique si l'on doit afficher ce tile avec de l'ombre, de la transparence ...

    int reserved[19];                   // Octets r�serv�s pour plus tard
};


extern TileDecors DecTile;

