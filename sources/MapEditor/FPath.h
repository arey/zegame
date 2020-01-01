////////////////////////////////////////////////////////////////////////////////
// Unité   : FPath.h (créée le 25/03/1999)                                    //
// Rôle    : Recherche de chemin à partir du fameux algorithme A*             //
// Utilité : Recherche d'un chemin entre 2 points disposés sur une grille qui //
//           est parsemée d'obstacles.                                        //
//                                                                            //
// Sources : Inspiré du Path-Finding d'Amit J.Patel                           //
//           -> http://theroy.standford.edu/~amitp/GameProgramming            //
////////////////////////////////////////////////////////////////////////////////
#ifndef FPathH
#define FPathH

//--- Unité à inclure ---
#include <vector>                      // Tableaux dynamiques de la STL
#include <algorithm>                   // Algorithmes de la STL
#ifdef CBUILDER
#include <function>
#elif VISUAL
#include <functional>
#endif
#include "MapLib.h"                    // Maps de mon jeu

//--- define, constantes, enums & variables ---
#define WITH_STATS                     // utilisation des statistiques
#define LONGUEUR_MAXIMUM_CHEMIN 10000  // longueur maximale autorisée d'un chemin
#define TYPE_DIRECTION_GRILLE 1        // Direction de type MapDirection
#define TYPE_DIRECTION_MAP    2        // Direction de type MapDirection2

//--- Directions dans lesquelles on peut se déplacer sur la grille ---
//=== Directions horizontales et verticales ===
const int NbMapDirections = 4;
enum MapDirection { G_GAUCHE = 0, G_HAUT, G_DROITE, G_BAS, G_NONE };
//--- Direction opposée à celle passée en paramètre (ex : Inv(G_GAUCHE)==G_DROITE)
inline MapDirection InverseDirection(MapDirection dir)
{  return MapDirection( (2+int(dir)) % 4 );
}
//=== Directions horizontales, verticales et en diagonales ===
const int NbMapDirections2 = 8;
enum MapDirection2 { M_GAUCHE = 0, M_GAUCHAUT, M_HAUT, M_DROITHAUT, M_DROITE,
                     M_DROITBAS, M_BAS, M_GAUCHBAS, M_NONE };
//--- Direction opposée à celle passée en paramètre (ex : Inv(G_GAUCHE)==G_DROITE)
inline MapDirection2 InverseDirection(MapDirection2 dir)
{  return MapDirection2( (4+int(dir)) % 8 );
}

//--- Systèmes de coordonnées utilisées pour se repérer sur la grille --------//
struct GridCoord
{  int x;
   int y;
   GridCoord():x(0),y(0) { };
   GridCoord(int i_x, int i_y):x(i_x),y(i_y) { };
};
//--- Opérateur d'égalité entre 2 cases ---
inline bool operator == (const GridCoord& gc1, const GridCoord& gc2)
{ return ((gc1.x==gc2.x) && (gc1.y==gc2.y));
}
//--- Opérateur d'ingéalite entre 2 cases ---
inline bool operator != (const GridCoord& gc1, const GridCoord& gc2)
{ return ((gc1.x!=gc2.x) || (gc1.y!=gc2.y));
}

//--- Renvoie la direction à prendre pour aller de A à B ---------------------//
// -> les points doivent être positionnés verticalement ou horizontalement
// (pour MapDirection) et même en diagonale (pour MapDirection2)
extern void SearchDirection(const GridCoord &a, const GridCoord &b, MapDirection&  mp);
extern void SearchDirection(const GridCoord &a, const GridCoord &b, MapDirection2& mp);

typedef std::vector<GridCoord> PathVector;

//=== Structure PFCase du même genre qu'un TCase =============================//
// -> recensse des infos sur chaque case de la grille pour le path-finding :  //
// * la direction permet de savoir de quelle case on vient et permet donc de  //
//   savoir si oui ou non on est déjà passé sur cette case et permet aussi de //
//   trouvé le chemin du point d'arrivée vers le point de départ,             //
// * f nous donne une estimation du cout total du chemin et si il est dans    //
//   les Open pour f >= 0,                                                    //
// * g donne le cout du chemin déjà effectué en ce point et si il est a dans  //
//   les Open ou dans les Closed                                              //
struct PFCase
{   MapDirection  direction1:4;   // Direction vers la case la moins couteuse
    MapDirection2 direction2:5;
    int  f:28;                    // Cout envisagé pour le parcours
    int  g:27;                    // Cout du parcours déjà effectué
    // Constructeur
    PFCase():direction1(G_NONE),direction2(M_NONE),f(-1),g(-1) { };
};
//============================================================================//

//=== Classe PFMap du même genre que TMap ====================================//
// -> On doit allouer l'espace nécessaire à la carte du niveau lors du        //
//    chargement d'un niveau d'aprés ses dimensions.                          //
//    Une fois cet mémoire allouée, on la conserve jusqu'au prochain niveau   //
class TPFMap
{   // Propriétés de la classe
    int   MapWidth, MapHeight;	      // Dimension de la carte en nombre de PFCases
  public:
    PFCase **Carte;		      // Tableau dynamique des cases

    // Méthodes générales de la classe
    TPFMap():MapWidth(0),MapHeight(0),Carte(NULL) {};    // Constructeur
    ~TPFMap();					         // Destructeur
    void AlloueCarte(int i_width, int i_height);         // Alloue l'espace nécessaire
    void DesalloueCarte();			         // Libère la mémoire

    // Méthodes utilisées dans l'algo de PathFinding
    bool inMap(int x, int y)                             // Indique si ce point est valide
    { return ((x>=0) && (y>=0) && (x<MapWidth) && (y<MapHeight)); }
    bool inMap(GridCoord gc)                             // même méthode surchargée
    { return ((gc.x>=0) && (gc.y>=0) && (gc.x<MapWidth) && (gc.y<MapHeight)); }
};
//============================================================================//

//=== NODE structure + opérateurs ============================================//
// -> Case de la carte associé à un coût
struct Node
{  GridCoord cN;    // Coordonnées du Node sur la grille
   int gval;        // Coût du chemin déjà parcouru
   int hval;        // Heuristique : coût estimé jusqu'au point final
   // Constructeur
   Node():gval(0),hval(0),cN(0,0) { };
};
//--- Opérateur de comparaison entre 2 nodes redéfinis ---
// -> permet de trier les éléments (Node) d'un Container
inline bool operator < (const Node& n1, const Node& n2)
{ // n1 < n2 est vrai si le cout total estimé f1 = g1 + h1  est < f2 = g2 + h2
  return ((n1.gval+n1.hval) < (n2.gval+n2.hval));
}
inline bool operator > (const Node& n1, const Node& n2)
{ // n1 > n2 est vrai si le cout total estimé f1 = g1 + h1  est > f2 = g2 + h2
  return ((n1.gval+n1.hval) > (n2.gval+n2.hval));
}
//--- Opérateur d'égalité entre 2 nodes ---
inline bool operator == (const Node& n1, const Node& n2)
{ // n1 == n2 si ils ont strictement tous leurs attributs confondus
  return ((n1.gval==n2.gval) && (n1.hval==n1.gval) && (n1.cN==n2.cN));
}
GridCoord NodeVoisin(GridCoord gc, MapDirection  mp); // Case voisine ds le système Grille
GridCoord NodeVoisin(GridCoord gc, MapDirection2 mp); // Case voisine ds le système Map

//============================================================================//

//=== Structure recenssant les statistiques sur le chemin trouvé =============//
struct PFStatistics
{   // Tout un tas de stats sur les cases intervenus dans l'A*
    int nodes_recherches;
    int nodes_ajoutes;
    int nodes_supprimes;
    int nodes_visites;
    int nodes_laisses;
    // Quelques infos sur le chemin trouvé
    int chem_long;
    int chem_cout;
    // Constructeur mettant tout à 0
    PFStatistics():
            nodes_recherches(0), nodes_ajoutes(0), nodes_supprimes(0),
            nodes_visites(0), nodes_laisses(0), chem_long(0), chem_cout(0) { };
};
//============================================================================//

////////////////////////////////////////////////////////////////////////////////
// Définitions & Variables utilisés par l'algo A*                             //
////////////////////////////////////////////////////////////////////////////////
// Ensemble de Nodes appelés Container regroupés dans un vector de la STL
typedef std::vector<Node> Container;
extern std::greater<Node> compGreater;
//--- Seule instance du jeu de la classe PFMap ---
extern TPFMap *PFMap;


////////////////////////////////////////////////////////////////////////////////
// Fonctions diverses utilisables par l'algo A*                               //
////////////////////////////////////////////////////////////////////////////////
// Définis dans le fichier FPath.cpp
inline void get_FirstNode(Container &c, Node &n);



//=== Classe de l'algo A* ====================================================//
// Grossièrement, cette classe est constituée de 2 ensembles de Nodes, l'un   //
// appelé OPEN (Contient des cases non visitées) et l'autre CLOSE (Contient   //
// cases déjà visitées).                                                      //
// Dans la boucle principale, on va prendre le meilleur Node dans le contenair//
// OPEN, et mettre tous ces voisins non déjà visités dans le CLOSE.           //
template <class Heuristik>
class AEtoile
{  public:
     //--- Attributs de la classe ---
     PFStatistics stats;            // Statistiques sur le dernier chemins trouvés
     Heuristik& heuristique;        // Fonction Heuristique à utiliser pour calculer le coût d'un chemin
     // les 2 Containers OPEN et CLOSE
     Container contOpen;            // Container OPEN
     Container contVisite;          // Container VISITE = OPEN + CLOSE

     TMap& map;                     // Carte sur laquelle la recheche va se dérouler
     GridCoord A, B;                // Points de départ et d'arrivée

     //--- Méthodes de la classe ---
   protected:
     AEtoile();                     // On peut pas appeler ce constructeur par défaut
   public:
     // Constructeurs et Destructeurs utilisables
     AEtoile(GridCoord a, GridCoord b, TMap& m, Heuristik& h)
        :A(a), B(b), map(m), heuristique(h) {};
     ~AEtoile();

     // Fonction Principale de recherche du soit-disant meilleur chemin
     void ChercheChemin(PathVector& chemin, bool linearise = true);
     // Indique si un Node positionne à tel coordonnées fait parti du container OPEN (fonction ultra rapide se servant du PFMap)
     inline bool InOpen(const GridCoord& gc)
     { return PFMap->Carte[gc.x][gc.y].f != -1; }
     // Renvoi la position dans le container OPEN du Node positionne à cet endroit
     Container::iterator FindInOpen(GridCoord gc);
};
//============================================================================//


//=== 1ère Heuristique codée =================================================//
// -> Travaille sur une grille en considérant que l'on ne puisse pas se
//    déplacer en diagonal.
class Heuristique1
{   //--- Attributs de la Classe ---
    int StepCout; // = 10;   // Coût pour passer d'une case à une autre adjacente
    int HeurDiv;  // =  6;   // Permet d'accroître l'heuristique quand on a déjà parcourru pas mal de chemin
  public:
    int AbortPath;           // Coût maximal d'un chemin
    int TypeDirection;       //  = TYPE_DIRECTION_GRILLE <=> Direction de type MapDirection <=> 4 directions (N, S, O, E)

    // Constructeur par défaut
    Heuristique1();

    //--- Méthodes de la Classe ---
    // Détermine la distance la plus courte entre 2 points de la carte sachant
    // que l'on ne peut pas se déplacer en diagonale
    inline int Dist(TMap &m, const GridCoord &gc1, const GridCoord &gc2)
    {  return ( abs(gc1.x - gc2.x) + abs(gc1.y - gc2.y) )*StepCout;
    }

    // Coût pour avancer d'une case
    // -> doit être supérieur ou égal à la distance entre ces 2 cases
    inline int Cout(TMap &m, const GridCoord& gc1, int i_d,
                    const GridCoord& gc2, int hd = -1)
    {  MapDirection d = (MapDirection) i_d;
	   #ifdef DEBUG
	   USE_PARAM(d);
	   #endif
	   if (hd==-1) hd = HeurDiv;
       // 1) Vérifie si cette case n'est pas occupée par un MUR
       if (!m.Carte[gc2.x][gc2.y].isAccessible()) return LONGUEUR_MAXIMUM_CHEMIN;
       // 2) Vérifie si une unité (Perso, objet, ...) n'occupe pas non plus cette case
       // ...
       // ...
       // 3) Regarde sur quelle genre de terrain on marche (marais, route, sable, ...)
       // -> pénalise ou favorise les déplacements sur ces zones là
       // ...
       // ...

       return StepCout;   // +/- pénalité
    }
};
//============================================================================//


//=== 2nde Heuristique codée =================================================//
// -> Travaille sur une carte en considérant que l'on puisse se déplacer dans
//    les 8 points cardinaux, ... donc en diagonale.
class Heuristique2
{   //--- Attributs de la Classe ---
    int StepCoutHor;//  = 10; // Coût pour passer d'une case à une autre adjacente
    int StepCoutDiag;// = 14; // Coût pour se rendre sur une case en diagonale
    int HeurDiv;//  =  6;     // Permet d'accroître l'heuristique quand on a déjà parcourru pas mal de chemin
  public:
    int AbortPath;            // Coût maximal d'un chemin
    int TypeDirection; // = TYPE_DIRECTION_MAP; // Direction de type MapDirection2 <=> déplacement en diagonale

    // Constructeur par défaut
    Heuristique2();

    //--- Méthodes de la Classe ---
    // Détermine la distance la plus courte entre 2 points de la carte sachant
    // que l'on ne peut pas se déplacer en diagonale
    inline int Dist(TMap &m, const GridCoord &gc1, const GridCoord &gc2)
    {  int dx = abs(gc1.x - gc2.x);
       int dy = abs(gc1.y - gc2.y);
       int dmin = min(dx,dy);
       int dmax = max(dx,dy);
       return ( dmin*StepCoutDiag + (dmax-dmin)*StepCoutHor );
    };

    // Coût pour avancer d'une case
    // -> doit être supérieur ou égal à la distance entre ces 2 cases
    inline int Cout(TMap &m, const GridCoord& gc1, int i_d,
                    const GridCoord& gc2, int hd = -1)
    {  MapDirection2 d = (MapDirection2) i_d;
	   if (hd==-1) hd = HeurDiv;
       // 1) Vérifie si cette case n'est pas occupée par un MUR
       // Si c'est une direction horizontale ou verticale, on ne regarde seulement
       // que la case d'à côté
       if (!m.Carte[gc2.x][gc2.y].isAccessible()) return LONGUEUR_MAXIMUM_CHEMIN;
       // Mais si c'est une direction en diagonale, on regarde aussi les 2 directions
       // proches
       if ((int(d)% 2) == 1) // Directions en Diagonales
       {  int dx = 0, dy = 0;
          switch (d)
          {  case (M_GAUCHAUT)  : dx = 1;  dy = 1;  break;
             case (M_DROITHAUT) : dx = -1; dy = 1;  break;
             case (M_GAUCHBAS)  : dx = 1;  dy = -1;  break;
             case (M_DROITBAS)  : dx = -1; dy = -1; break;
          }
          if (   !m.Carte[gc2.x+dx][gc2.y].isAccessible()
              || !m.Carte[gc2.x][gc2.y+dy].isAccessible() )
          {  return LONGUEUR_MAXIMUM_CHEMIN;
          }
       }

       // 2) Vérifie si une unité (Perso, objet, ...) n'occupe pas non plus cette case
       // ...
       // ...
       // 3) Regarde sur quelle genre de terrain on marche (marais, route, sable, ...)
       // -> pénalise ou favorise les déplacements sur ces zones là
       // ...
       // ...

       // Cout pour avancer d'une case suivant si l'on se deplace en diagonale ou tout droit
       if ( (int(d) % 2) == 0)
       { return StepCoutHor;   // déplacement horizontal ou vertical
       } else
       { return StepCoutDiag;  // déplacement en diagonal
       }
    };
};
//============================================================================//


//=== Fonction MapInitRechercheChemin ========================================//
void MapInitRechercheChemin(TMap &map);
void MapCloseRechercheChemin();

//=== Fonction de RechercheChemin ============================================//
PFStatistics  RechercheChemin(TMap &map, GridCoord gc1, GridCoord gc2, PathVector& path, bool linearise = true);


#endif
