////////////////////////////////////////////////////////////////////////////////
// Données de l'image du décors 											  //
////////////////////////////////////////////////////////////////////////////////

#define MaxChar 15			// nb max de caractères ds 1 chaîne


enum TrapTERRAIN { INCONNU=0, SABLE, TERRE, HERBE, GRAVIERS, BOIS, MARBRE, RACINE,
                   TRONC, MUR, OBJET };
const int TrapTERRAINNb = 11;
extern const char TrapTERRAINText [TrapTERRAINNb][20];


enum eTrapCOLLIDE { PARTIELLE = 0, TOTALE = 1, AUCUNE };
const int TrapCOLLIDENb = 2;
extern const char TrapCOLLIDEText [TrapCOLLIDENb][20];


// Structure d'un Trapèze del version 0.98
struct Trapeze
{
    int TrapU, TrapV;			// Coords du trapèze par rapport au trapèze principal

    //*** Nouveau à partir de la version 0.9 ***
    int Hauteur;			// Hauteur de l'objet présent sur la case / au sol
    TrapTERRAIN	Terrain;		// Type de terrain présent sur le trapèze
    BOOL Accessible;                    // indique si on peut y marcher oui ou non
    //*** Nouveau à partir de la version 0.95 ***
    eTrapCOLLIDE TrapCollide;           // Type de collisions à effectuer sur ce trapèze
    //*** Nouveau à partir de la version 0.97 ***
    bool DrawTrapezekey;                // indique si ce trapèze est un élément clef de l'affichage
    int  lamX1, lamY1, lamX2, lamY2;    // lamelle à afficher pour ce trapèze
    //*** Nouveau depuis la version 0.98 ***
    int reserved[8];
};

// **** Version 0.98 de la strucuture TRAPEZE (09/07/99) ***
// Chps de bits indiquant de quelle manière on doit afficher le tile du décors
#define TILEDECORS_DRAW_OPAQUE		0			// Sans rien de spécifique
#define TILEDECORS_DRAW_SHADOW		1			// Tiens compte de l'ombrage

// Vieille structure d'un Trapèze de la version 0.90
struct OldTrapeze
{
    int TrapU, TrapV;			// Coords du trapèze par rapport au trapèze principal

    //*** Nouveau à partir de la version 0.9 ***
    int Hauteur;			// Hauteur de l'objet présent sur la case / au sol
    TrapTERRAIN	Terrain;		// Type de terrain présent sur le trapèze
    BOOL Accessible;                    // indique si on peut y marcher oui ou non
    //*** Nouveau à partir de la version 0.95 ***
    eTrapCOLLIDE TrapCollide;           // Type de collisions à effectuer sur ce trapèze
    //*** Nouveau à partir de la version 0.97 ***
    bool DrawTrapezekey;                // indique si ce trapèze est un élément clef de l'affichage
    int  lamX1, lamY1, lamX2, lamY2;    // lamelle à afficher pour ce trapèze
};

//  TILE DU DECORS
struct TileDecors
{
    int  DecorsNum;		        // n° de référence servant à tout objet s'y référencant
    char Nom [MaxChar];		        // Nom du fichier
    int  Width, Height;			// Taille du tile
    unsigned int RVB;                   // Composante de la couleur transparente
    unsigned char NumRVB;		// n° de couleur transparente ds la palette
    int  NbTrapeze;			// Nombre de trapèzes s'y rattachant
    int  TrapPrincX, TrapPrincY;	// Coordonnées du coin supérieur du trapèze principal par rapport au coin supérieur-gauche du tile
    // TrapezTab[0] fait référence au trapèze principal
    Trapeze *TrapezTab;			// Tableau dynamique pointant sur un trapèze
    int DecorsCornerX, DecorsCornerY;	// ne sert que pour l'éditeur : sait où replacer le tile

    //--- New depuis le 20/03/99 - version 0.97 ---
    bool TotalDraw;                     // Indique si l'objet doit s'afficher en une seule fois ou par tranches verticales

    //--- New depuis le 08/06/99 - version 0.98 ---
    bool CircleCollision;               // Indique si l'on doit utiliser une collision circulaire
    int  CircleCenterX, CircleCenterY;  // Coords du centre du cercle relatives à (TrapPrincX,TrapPrincY)
    int  CircleRadius;                  // Rayon du cercle
    int  DrawMethode;					// Indique si l'on doit afficher ce tile avec de l'ombre, de la transparence ...

    int reserved[19];                   // Octets réservés pour plus tard
};


extern TileDecors DecTile;

