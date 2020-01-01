////////////////////////////////////////////////////////////////////////////////
// Unité   : FPath.cpp (créée le 25/03/1999)                                  //
// Rôle    : Recherche de chemin à partir du fameux algorithme A*             //
// Utilité : Recherche d'un chemin entre 2 points disposés sur une grille qui //
//           est parsemée d'obstacles.                                        //
//                                                                            //
// Sources : Inspiré du Path-Finding d'Amit J. Patel                          //
//           -> http://theroy.standford.edu/~amitp/GameProgramming            //
////////////////////////////////////////////////////////////////////////////////

//--- Fichiers à inclure ---
#include "ZeNain.h"		// Fichier commun à tout le projet
#include "FPath.h"      // Header du fichier cpp

std::greater<Node> compGreater;

////////////////////////////////////////////////////////////////////////////////
// Classe PFMAP                                                               //
////////////////////////////////////////////////////////////////////////////////

//=== Destructeur ============================================================//
TPFMap::~TPFMap()
{  if (Carte!=NULL) DesalloueCarte();    // libère la mémoire des TFCases
}
//----------------------------------------------------------------------------//

//=== Initialise le Tableau dynamique des TFCases ============================//
void TPFMap::AlloueCarte(int i_width, int i_height)
{   if (Carte!=NULL) DesalloueCarte();  // Libère l'espace occupé
    // Dimensions de la carte
    MapWidth  = i_width;
    MapHeight = i_height;
    // Alloue la mémoire nécessaire
    Carte = new PFCase* [MapWidth];  									 // lignes
    for (int i=0 ; i < MapWidth; i++) Carte[i] = new PFCase [MapHeight];  // colonnes
};
//----------------------------------------------------------------------------//

//=== Libère la mémoire du tableau dynamique =================================//
void TPFMap::DesalloueCarte()
{  for (int i=0; i < MapWidth; i++) delete[] Carte[i];	    // lignes
   delete[] Carte;					    // colonnes
   Carte = NULL;
};
//----------------------------------------------------------------------------//

//--- Seule instance du jeu de la classe PFMap ---
TPFMap *PFMap = NULL;

////////////////////////////////////////////////////////////////////////////////
// Fonctions diverses utilisables par l'algo A*                               //
////////////////////////////////////////////////////////////////////////////////

//=== Récupère le Node le plus intéressant du Container ======================//
inline void get_FirstNode(Container &c, Node &n)
{  n = c.front();                        // Récupère le 1er Node du Container
   std::pop_heap(c.begin(), c.end(),compGreater);  // Réordonne le Container : enlève le 'root' élément des numéros du Heap et update certains Nodes
   c.pop_back();                         // Efface le dernier élément du Container
}
//----------------------------------------------------------------------------//

//=== Renvoie la case voisine ds le suystème Grille ==========================//
// -> diagonales non incluses
GridCoord NodeVoisin(GridCoord gc, MapDirection mp)
{  switch (mp)
   { case (G_GAUCHE) : return GridCoord(gc.x-1,gc.y);
     case (G_DROITE) : return GridCoord(gc.x+1,gc.y);
     case (G_HAUT)   : return GridCoord(gc.x,gc.y-1);
     case (G_BAS)    : return GridCoord(gc.x,gc.y+1);
   }
   return GridCoord();
}
//=== Case voisine ds le système Map =========================================//
// -> diagonales incluses
GridCoord NodeVoisin(GridCoord gc, MapDirection2 mp)
{ switch (mp)
   { // Directions Horizontales
     case (M_GAUCHE) : return GridCoord(gc.x-1,gc.y);
     case (M_DROITE) : return GridCoord(gc.x+1,gc.y);
     // Directions Verticales
     case (M_HAUT)   : return GridCoord(gc.x,gc.y-1);
     case (M_BAS)    : return GridCoord(gc.x,gc.y+1);
     // Directions en Diagonales
     case (M_GAUCHAUT)  : return GridCoord(gc.x-1,gc.y-1);
     case (M_DROITHAUT) : return GridCoord(gc.x+1,gc.y-1);
     case (M_GAUCHBAS)  : return GridCoord(gc.x-1,gc.y+1);
     case (M_DROITBAS)  : return GridCoord(gc.x+1,gc.y+1);
   }
   return GridCoord();
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Classe AEtoile                                                             //
////////////////////////////////////////////////////////////////////////////////

//=== Destructeur ============================================================//
template <class Heuristik>
AEtoile<Heuristik>::~AEtoile()
{  // ??? dirs.data ???

   //--- Réinitialise les éléments utilisés du PFMap pour une prochaine utilisation du A* ---
   for (Container::iterator i = contOpen.begin(); i != contOpen.end() ; ++i)
   {  // Tout d'abord, on modifie les Nodes faisant du container OPEN
      GridCoord c = (*i).cN;                       // Coordonnées du Node
      PFMap->Carte[c.x][c.y].direction1 = G_NONE;  // Plus aucune direction
      PFMap->Carte[c.x][c.y].direction2 = M_NONE;
      PFMap->Carte[c.x][c.y].f = -1;               // Ne font plus parti des OPEN
      PFMap->Carte[c.x][c.y].g = -1;
   }
   for (Container::iterator j = contVisite.begin() ; j!= contVisite.end(); ++j)
   {  // Ensuite, on modifie les Nodes déjà visités
      GridCoord c = (*j).cN;                       // Coordonnées du Node
      PFMap->Carte[c.x][c.y].direction1 = G_NONE;  // Plus aucune direction
      PFMap->Carte[c.x][c.y].direction2 = M_NONE;
      PFMap->Carte[c.x][c.y].g = -1;               // Ne font plus parti des VISITES
      // Rmq : f est toujours à -1 (normalement...)
   }
}
//----------------------------------------------------------------------------//

//=== Renvoi la position dans les OPEN du Node positionne à cet endroit ======//
template <class Heuristik>
Container::iterator AEtoile<Heuristik>::FindInOpen(GridCoord gc)
{  // On s'assure que le Node fait bien parti du container des OPEN
   if (PFMap->inMap(gc) && InOpen(gc))
   {  // On recherche alors où il se trouve exactement ds les OPEN
      for (Container::iterator i=contOpen.begin() ; i!=contOpen.end() ; ++i)
      {  stats.nodes_recherches++;   // 1 Node recherche de plus
         if (gc == (*i).cN)
         { return i;
         }
      }
   }
   return contOpen.end();  // On ne l'a pas trouvé ???????
}
//----------------------------------------------------------------------------//


//=== Fonction Principale de recherche du soit-disant meilleur chemin ========//
template <class Heuristik>
void AEtoile<Heuristik>::ChercheChemin(std::vector<GridCoord>& chemin, bool linearise)
{  //-- Variables locales à l'algo --
   Node N;
   //-- Initialisations --
   {
   N.cN = A;                                      // Point de départ
   N.gval = 0;                                    // Coût encore nul
   N.hval = heuristique.Dist(map,A,B);            // Estimation de départ du coût du trajet
   contOpen.push_back(N);                         // 1er Node ds le OPEN
   PFMap->Carte[A.x][A.y].f = N.gval + N.hval;    // Initialisation de la PFMap
   PFMap->Carte[A.x][A.y].g = N.gval;
   stats.nodes_ajoutes++;                         // 1 Node de plus ajoute au contenair OPEN
   }

   //-- C'est parti : on visite tous les 'meilleurs' nodes jusqu'à avoir trouvé le chemin --
   while (!contOpen.empty())
   {  get_FirstNode(contOpen, N);                 // Récupère le 'meilleur' Node des OPEN
      PFMap->Carte[N.cN.x][N.cN.y].f = -1;        // Ce Node ne fait désormais plus parti des OPEN
      contVisite.push_back(N);                    // Le rajoute aux VISITES
      stats.nodes_supprimes++;                    // 1 Node de moins dans le contenair des OPEN

      // Vérifie si l'on n'est pas arrivé au bout
      if (N.cN==B) break;   // C'est GAGNE !

      // Si çà fait trop longtemps que l'on cherche <=> Chemin trop longs à décomposer
      if (stats.nodes_supprimes >= heuristique.AbortPath)
      {  // On prends quand même le meilleur des Nodes
         for (Container::iterator i = contOpen.begin(); i!=contOpen.end() ; ++i)
         {  // On utilise pour celà la fonction  H*2 + G
            if ((*i).hval*2 + (*i).gval < N.hval*2 + N.gval)
            { N = *i; }
         }
         B = N.cN;  //  Nouvelle position finale du chemin
         break;     // C'est FINI !
      }

      // On scrute tous les Nodes aux alentours directs de N
      // -> pour ne pas toujours tourner dans le même sens, on choisi 'aléatoirement'
      //    dans la grille un Node de départ et un sens de rotation
      MapDirection SensTrigo1[NbMapDirections]  = { G_DROITE, G_HAUT, G_GAUCHE, G_BAS };
      MapDirection SensMontre1[NbMapDirections] = { G_GAUCHE, G_HAUT, G_DROITE, G_BAS };
      MapDirection* directions1;
      MapDirection2 SensTrigo2[NbMapDirections2]  = { M_DROITE, M_DROITHAUT, M_HAUT, M_GAUCHAUT, M_GAUCHE, M_GAUCHBAS, M_BAS, M_DROITBAS };
      MapDirection2 SensMontre2[NbMapDirections2] = { M_GAUCHE, M_GAUCHAUT, M_HAUT, M_DROITHAUT, M_DROITE, M_DROITBAS, M_BAS, M_GAUCHBAS };
      MapDirection2* directions2;
      if ((N.cN.x-N.cN.y) % 2 == 0)
      {  directions1 = SensTrigo1;
         directions2 = SensTrigo2;
      } else
      {  directions1 = SensMontre1;
         directions2 = SensMontre2;
      }
      //-- On étudie chacun des voisins du node courant --
      int NbRealMapDirections = 0;
      switch (heuristique.TypeDirection)
      {  case (TYPE_DIRECTION_GRILLE) : NbRealMapDirections = NbMapDirections;  break;
         case (TYPE_DIRECTION_MAP)    : NbRealMapDirections = NbMapDirections2; break;
      }
      for (int cpt = 0; cpt < NbRealMapDirections ; cpt++)
      { // Récupère une directions
        MapDirection  d1 = directions1[MapDirection(cpt)];
        MapDirection2 d2 = directions2[MapDirection2(cpt)];

        // Prends le Node voisin
        GridCoord gc;
        switch (heuristique.TypeDirection)
        {  case (TYPE_DIRECTION_GRILLE) : gc = NodeVoisin(N.cN,d1);  break;
           case (TYPE_DIRECTION_MAP)    : gc = NodeVoisin(N.cN,d2);  break;
        }

        // Si ce point est hors de la carte, on le saute
        if (!PFMap->inMap(gc)) continue;

        // On calcule le coût pour aller du Node N au Node N2
        int cout = 0;
        switch (heuristique.TypeDirection)
        {  case (TYPE_DIRECTION_GRILLE) : cout = heuristique.Cout(map,N.cN,(int) d1,gc); break;
           case (TYPE_DIRECTION_MAP)    : cout = heuristique.Cout(map,N.cN,(int) d2,gc);  break;
        }

        Node N2;       // Nouveau node voisin de celui dont on s'occupe
        N2.cN = gc;
        N2.gval = N.gval + cout;
        N2.hval = heuristique.Dist(map,gc,B);
        // Si ce point sur la carte n'avait pas déjà été visité
        if (   (    (PFMap->Carte[gc.x][gc.y].direction1 == G_NONE)
                 && (heuristique.TypeDirection == TYPE_DIRECTION_GRILLE))
             || (    (PFMap->Carte[gc.x][gc.y].direction2 == M_NONE)
                 && (heuristique.TypeDirection == TYPE_DIRECTION_MAP)))
        {  // On le remplit donc
           switch (heuristique.TypeDirection)
           {  case (TYPE_DIRECTION_GRILLE) :
                 PFMap->Carte[gc.x][gc.y].direction1 = InverseDirection(d1);   // Direction d'où l'on vient
                 break;
              case (TYPE_DIRECTION_MAP) :
                 PFMap->Carte[gc.x][gc.y].direction2 = InverseDirection(d2);  // Direction d'où l'on vient
                 break;
           }
           PFMap->Carte[gc.x][gc.y].f = N2.gval + N2.hval;
           PFMap->Carte[gc.x][gc.y].g = N2.gval;
           contOpen.push_back(N2);                            // Rajoute ce Node ds les OPEN
           std::push_heap(contOpen.begin(), contOpen.end(), compGreater);  // Réordonne l'arbre binaire
           stats.nodes_ajoutes++;                             // 1 Node de plus ajoute
        } else
        { // Si ce point avait déjà été marqué : il appartient aux OPEN ou aux CLOSE
          if (InOpen(gc))
          {  // Si il est dans les OPEN, on regarde lequel des 2 nodes est le meilleur en comparant leurs coûts
             if (N2.gval < PFMap->Carte[gc.x][gc.y].g)
             {  // On recherche la position de N2 dans les OPEN
                Container::iterator iN2 = FindInOpen(gc);
                // On remplace les valeurs de l'ancien N par les valeurs de N2
                PFMap->Carte[gc.x][gc.y].direction1 = InverseDirection(d1);
                PFMap->Carte[gc.x][gc.y].direction2 = InverseDirection(d2);
                PFMap->Carte[gc.x][gc.y].f = N2.gval + N2.hval;
                PFMap->Carte[gc.x][gc.y].g = N2.gval;
                (*iN2).gval = N2.gval;
                // D'aprés les règles de balancement d'un arbre : on optimise le rebalancement
                std::push_heap(contOpen.begin(), iN2+1, compGreater);
             }
          } // Si c'était ds les CLOSE, plus la peine de faire quoi ke ce soit
        }
      }
   } // Fin du While

   // Vérifie si l'on a trouvé un chemin dans les normes
   if ((N.cN == B) && (N.gval < LONGUEUR_MAXIMUM_CHEMIN))
   {  stats.chem_cout = N.gval;    // Coût d'un chemin
      // On retourne le chemin trouvé dans le vector 'chemin'
      // En le parcourant de la fin vers le début
      GridCoord g = B;
      if (!linearise)   // On met à la suite des autres toutes les étapes
      {   while ( g != A )
          {  MapDirection  dir1 = PFMap->Carte[g.x][g.y].direction1;
             MapDirection2 dir2 = PFMap->Carte[g.x][g.y].direction2;
             chemin.push_back(g);
             switch (heuristique.TypeDirection)
             {  case (TYPE_DIRECTION_GRILLE) : g = NodeVoisin(g,dir1); break;
                case (TYPE_DIRECTION_MAP)    : g = NodeVoisin(g,dir2); break;
             }
             stats.chem_long++;              // On ajouste la longueur du chemin trouvé
          }
      } else
      { // On "compresse" le chemin en ne mettant que les points intermédaires
          // bool newDirec = true;
          MapDirection  oldDir1 = G_NONE;
          MapDirection2 oldDir2 = M_NONE;
          while ( g != A )
          {  MapDirection  dir1 = PFMap->Carte[g.x][g.y].direction1;
             MapDirection2 dir2 = PFMap->Carte[g.x][g.y].direction2;
             switch (heuristique.TypeDirection)
             {  case (TYPE_DIRECTION_GRILLE) :
                    if (dir1!=oldDir1) chemin.push_back(g);
                    g = NodeVoisin(g,dir1);
                    break;
                case (TYPE_DIRECTION_MAP) :
                    if (dir2!=oldDir2) chemin.push_back(g);
                    g = NodeVoisin(g,dir2);
                    break;
             }
             stats.chem_long++;              // On ajouste la longueur du chemin trouvé
             oldDir1 = dir1;
             oldDir2 = dir2;
          }
      }
      chemin.push_back(A);   // On rajoute le point de départ A
   }
   else
   {  // Aucun chemin potable trouvé
      // MessageBoxA(NULL,"Aucun chemin de potable trouvé","Dommage...",1);
      stats.chem_long = 0;  // <=> Pas trouvé
   }

   // On complète quelques stats avant de sortir
   stats.nodes_laisses = contOpen.size();
   stats.nodes_visites = contVisite.size();
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Fonctions Principales utilisées par le jeu                                 //
////////////////////////////////////////////////////////////////////////////////

//=== Fonction de RechercheChemin ============================================//
PFStatistics  RechercheChemin(TMap &map, GridCoord gc1, GridCoord gc2, PathVector& path, bool linearise)
{  // On utilise l'heuristique (1)
   Heuristique2 H;
   // On crée une instance de A*
   AEtoile<Heuristique2> Expert(gc1, gc2, map, H);

   // Avant de rechercher un chemin, on s'assure qu'il n'est pas farfelu
   if (H.Cout(map, gc1, M_NONE, gc2) >= LONGUEUR_MAXIMUM_CHEMIN)
   {  return Expert.stats;  // Too far away
   }
   else if (gc1==gc2)
   { return Expert.stats; } // 2 mêmes points
   

   Expert.ChercheChemin(path, linearise);
   return Expert.stats;
}
//----------------------------------------------------------------------------//

//=== Fonction MapInitRechercheChemin ========================================//
void MapInitRechercheChemin(TMap &map)
{ // Désalloue la carte si elle était déjà utilisée
  if (PFMap==NULL) delete PFMap;
  // Puis alloue de la mem à cette carte
  PFMap = new TPFMap();
  PFMap->AlloueCarte(map.MapWidth, map.MapHeight);
}
//----------------------------------------------------------------------------//

//=== Supprime le gestionnaire de pathfinding ================================//
void MapCloseRechercheChemin()
{	delete PFMap;
	PFMap = NULL;
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Fonctions Heuristiques                                                     //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur de Heuristique 2 ==========================================//
Heuristique2::Heuristique2():AbortPath(1000000)
{ StepCoutHor  = 10;   // Coût pour passer d'une case à une autre adjacente
  StepCoutDiag = 14;   // Coût pour se rendre sur une case en diagonale
  HeurDiv  =  6;       // Permet d'accroître l'heuristique quand on a déjà parcourru pas mal de chemin
  TypeDirection = TYPE_DIRECTION_MAP;   // On peut de déplacer en diagonale (8 directions possibles)
}
//----------------------------------------------------------------------------//

//=== Constructeur de Heuristique 2 ==========================================//
Heuristique1::Heuristique1():AbortPath(1000000)
{ StepCout  = 10;   // Coût pour passer d'une case à une autre adjacente
  HeurDiv  =  6;       // Permet d'accroître l'heuristique quand on a déjà parcourru pas mal de chemin
  TypeDirection = TYPE_DIRECTION_GRILLE;   // 4 directions (N, S, O, E)
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Fonctions Annexes                                                          //
////////////////////////////////////////////////////////////////////////////////

//--- Renvoie la direction à prendre pour aller de A à B ---------------------//
void SearchDirection(const GridCoord &a, const GridCoord &b, MapDirection&  mp)
{ int dx = b.x - a.x;
  int dy = b.y - a.y;
  if (dx > 0) mp = G_DROITE;
  else if (dx < 0) mp = G_GAUCHE;
  else if (dy > 0) mp = G_BAS;
  else if (dy < 0) mp = G_HAUT;
  else mp = G_NONE;   // 2 même points
}
void SearchDirection(const GridCoord &a, const GridCoord &b, MapDirection2& mp)
{ int dx = b.x - a.x;
  int dy = b.y - a.y;
  if (dy==0) // direction verticale
  { if (dx > 0) mp = M_DROITE;
    else if (dx < 0) mp = M_GAUCHE;
    else mp = M_NONE; // 2 même points
  } else
  if (dx==0) // direction horizontale
  { if (dy > 0) mp = M_BAS;
    else if (dy < 0) mp = M_HAUT;
  } else // direction diagonale
  { if ((dx > 0) && (dy > 0)) mp = M_DROITBAS;
    else if ((dx > 0) && (dy < 0)) mp = M_DROITHAUT;
    else if ((dx < 0) && (dy < 0)) mp = M_GAUCHAUT;
    else mp = M_GAUCHBAS;  // il ne reste plus que çà !
  }
}
//----------------------------------------------------------------------------//
