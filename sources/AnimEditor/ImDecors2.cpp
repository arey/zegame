////////////////////////////////////////////////////////////////////////////////
// ImDécors.cpp : rassemble les informations d'un sprite du décors            //
// Conçu pour l'éditeur de niveau, rénové pour le jeu						  //
//  => Il faudra peut-être faire du ménage                                    //
////////////////////////////////////////////////////////////////////////////////
#include <vcl\vcl.h>
#include <stdio.h>

// Données ===================================================================//
#include "consts.h"
#include "ImDecors2.h"
#include "DDUtil.h"
#include "DDLibEditor.h"
#include "fPrincipale.h"
#include "GameDebug.h"

// Librairie en cours d'utilisation : que ce soit ds la fiche de librairie ou
// dans l'éditeur
TileDecors     *TDSelect;			// Décors-Tile sélectionné ds la liste
DecorsObjet    *DOSelect;			// Objet-Décors sélectionné ds la liste ou sur la grille
DecorsObjetLib *DOLib;				// Librairie des Objet-Décors de l'éditeur
DecorsTileLib  *DTLib;				// Librairie des Décors-Tiles de l'éditeur
TLevelTileLib  *DULib;				// Librairie des Décors-Tiles de l'éditeur (2éme version)


double LibTileVersion = 1.01;		// version de la librairie Tile  (utilisé lorsque l'on rajoute des nouveaux éléments ds la liste)
AnsiString  TileDecExt = ".dec";	// extension de fichier décors
const char LibDecExt []  =  ".LiD"; 	// extension des librairies de décors
float TileDecVersion = 0.97;		// Version du Programme : dernière mise à jour le 27/10/98

const char TrapTERRAINText [TrapTERRAINNb][20] = { "Inconnu", "Sable", "Terre", "Herbe", "Graviers",
      "Bois", "Marbre", "Racine", "Tronc", "Mur", "Objet" };

const char TrapCOLLIDEText [TrapCOLLIDENb][20] = {"Partielle", "Totale" };

//--- Liste de tous les modèles d'objets/décors du jeu ---
TAnimTileDecors** ListeObjetsDecors;
int NbMaxTileDecors;   

//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Fonctions Annexes														  //
////////////////////////////////////////////////////////////////////////////////

//== Charge un *.dec dans le TileDecors spécifié =============================//
void LoadTileDecors(TileDecors *TileLoading, char Name[30])
{
    AnsiString FileToLoad = Name;
    FileToLoad = CheminApplication + cPathTiles + FileToLoad;

    FILE *f;
    if ((f = fopen(FileToLoad.c_str(),"rb"))==NULL)
    	MessageBoxA(NULL,FileToLoad.c_str(),"Impossible de charger le fichier :",1);
    float DecVersion;
    fread(&DecVersion, sizeof(float), 1, f);
    if (DecVersion < TileDecVersion) { MessageBoxA(NULL,"Version ancienne de sauvegarde","Problème de lecture",1); fclose(f); return; };
    fread(&TileLoading->DecorsNum, sizeof(int), 1, f);
    fread(&TileLoading->Nom, sizeof(TileLoading->Nom), 1, f);
    fread(&TileLoading->Width, sizeof(int), 1, f);
    fread(&TileLoading->Height, sizeof(int), 1, f);
    fread(&TileLoading->RVB, sizeof(unsigned int), 1, f);
    fread(&TileLoading->NumRVB, sizeof(unsigned char), 1, f);
    fread(&TileLoading->NbTrapeze, sizeof(int), 1, f);
    fread(&TileLoading->TrapPrincX, sizeof(int), 1, f);
    fread(&TileLoading->TrapPrincY, sizeof(int), 1, f);
    TileLoading->TrapezTab = new Trapeze[TileLoading->NbTrapeze];
    for (int i=0; i < TileLoading->NbTrapeze; i++)
      fread(&TileLoading->TrapezTab[i], sizeof(Trapeze), 1, f);
    fread(&TileLoading->DecorsCornerX, sizeof(int), 1, f);
    fread(&TileLoading->DecorsCornerY, sizeof(int), 1, f);
    fread(&TileLoading->TotalDraw, sizeof(bool), 1, f);
    fclose(f); // fin de lecture du fichier
};
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : DECORSTILELIB											  //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
DecorsTileLib::DecorsTileLib(void)
{
    Nom = "";
    NbTile = 0;
    DecorsTileList = NULL;
    TypeLib = 0;
};
//----------------------------------------------------------------------------//

//== Efface tout le contenu de la librairie
void DecorsTileLib::DeleteAllLib()
{
    TileDecors *ptr;
    ptr = DecorsTileList;
    while (ptr!=NULL)
     {
     	DecorsTileList = DecorsTileList->Suiv;
        delete ptr;
        ptr = DecorsTileList;
     };
    Nom="";
    NbTile = 0;
    DecorsTileList = NULL;
};
//----------------------------------------------------------------------------//

//== Destructeur =============================================================//
DecorsTileLib::~DecorsTileLib(void)
{
    DeleteAllLib();
};
//----------------------------------------------------------------------------//

//== Ajoute un fichier à la librairie ========================================//
void DecorsTileLib::Ajouter(TileDecors *TileAdd)
{
    NbTile++;  // un tile de plus ...
    TileAdd->NumDsList = NbTile;

    // Ajout de ce Tile à la liste
    TileAdd->Suiv = DecorsTileList;
    TileAdd->Prec = NULL;
    if (DecorsTileList!=NULL) DecorsTileList->Prec = TileAdd;
    DecorsTileList = TileAdd;
};
//----------------------------------------------------------------------------//

//== Vérifie si le Tile n'existe déjà pas par son nom ========================//
BOOL DecorsTileLib::ExisteTile(AnsiString SameName)
{
    TileDecors *ptr;
    ptr = DecorsTileList;
    while (ptr!=NULL)
     {
     	if ( strcmp(ptr->Nom,SameName.c_str()) == 0) return TRUE;
        ptr = ptr->Suiv;
     };
    return FALSE;
};
//----------------------------------------------------------------------------//

//== Sauve la librairie ds le fichier ayant son nom ==========================//
void DecorsTileLib::SaveToFile()
{
    FILE *f;
    AnsiString fNom = Nom;  fNom+=LibDecExt;
    if ((f = fopen(fNom.c_str(),"wb"))==NULL)
    	MessageBoxA(NULL,"Impossible de sauver le fichier :",fNom.c_str(),1);
    // Sauvegarde les trucs importants
    char n [15];
    memset(n,' ', 15);
    strcpy(n,Nom.c_str());
    fwrite(n, 15, 1, f);
    fwrite(&NbTile, sizeof(int), 1, f);

    TileDecors *ptr;
    ptr = DecorsTileList;
	// Sauve dans le sens contraire => cherche le dernier pointer
    if (ptr!=NULL) while (ptr->Suiv!=NULL) ptr = ptr->Suiv;
    while (ptr!=NULL)
      {
         fwrite(ptr->Nom, sizeof(ptr->Nom), 1 ,f);
      	 ptr = ptr->Prec;   // redescend
      };

    fclose(f);
};
//----------------------------------------------------------------------------//

//== Charge un fichier *.LiD =================================================//
void DecorsTileLib::LoadFile(AnsiString FileToLoad)
{
    FILE *f;
    if ((f = fopen(FileToLoad.c_str(),"rb"))==NULL)
      {	MessageBoxA(NULL,FileToLoad.c_str(),"Impossible de charger le fichier :",1);
        return; };

    char n[15];
    fread(n, 15, 1, f); Nom = n;
    fread(&NbTile, sizeof(int), 1, f);

    int NbTile2 = 0;
    for (int i=0; i < NbTile ; i++)
      {
        	// nouvel élément à la liste
           TileDecors*  NewTile;
           NewTile = new TileDecors;

           NbTile2++;  // un tile de plus ...
           NewTile->NumDsList = NbTile2;

           // ini des chaines
           memset(NewTile->Nom,' ',MaxChar);
           fread(NewTile->Nom, sizeof(NewTile->Nom), 1 ,f);
           AnsiString n = NewTile->Nom; n+=".dec";
           LoadTileDecors(NewTile, n.c_str());

           // Ajout de ce Tile à la liste
           NewTile->Suiv = DecorsTileList;
           NewTile->Prec = NULL;
           if (DecorsTileList!=NULL) DecorsTileList->Prec = NewTile;
           DecorsTileList = NewTile;
      };
   fclose(f);
};
//----------------------------------------------------------------------------//

//== Alloue et charges les images ds les surfaces des différents Tiles =======//
void DecorsTileLib::AlloueDDSurface()
{   // Défile ds le cortège des tiles
    TileDecors *ptr;
    ptr = DecorsTileList;
    while (ptr!=NULL)
     {
      	ptr->TileSurface = new DDSurface(&DS_VA->pDD, ptr->Width, ptr->Height, VIDEO_MEMORY);
        AnsiString BitmapName = ptr->Nom; BitmapName+=".bmp";
        if (DDReLoadBitmap(ptr->TileSurface->pDDSurf, BitmapName.c_str())!=DD_OK) Application->Terminate();
        ptr->TileSurface->ColorKey(RGBto16(ptr->RVB),RGBto16(ptr->RVB));
        // ptr->TileSurface->Draw(DS_VE->pDDSPrimary);
        ptr = ptr->Suiv;
     };
};
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : DECORSOBJETLIB											  //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
DecorsObjetLib::DecorsObjetLib(void)
{
    NbTileObj = 0;
    DecorsObjList = NULL;
    LastNum = 0;
};
//----------------------------------------------------------------------------//

//== Destructeur =============================================================//
DecorsObjetLib::~DecorsObjetLib(void)
{
    DeleteAllLib();
};
//----------------------------------------------------------------------------//

//== Efface tout le contenu de la librairie
void DecorsObjetLib::DeleteAllLib()
{
    DecorsObjet *ptr;
    ptr = DecorsObjList;
    while (ptr!=NULL)
     {
     	DecorsObjList = DecorsObjList->Suiv;
        delete ptr;
        ptr = DecorsObjList;
     };
    NbTileObj = 0;
    DecorsObjList = NULL;
};
//----------------------------------------------------------------------------//

//== Rajoute un objet à la liste =============================================//
void DecorsObjetLib::AddDecObj(DecorsObjet *DecorsObjAdd)
{
    NbTileObj++;  // un tile de plus ...

    LastNum++;	  // n° de l'objet ajouté à la liste
    DecorsObjAdd->NumObjet = LastNum;
    DecorsObjAdd->ObjDessine = 0;		// pas encore dessiné

    // Ajout de ce Tile à la liste
    DecorsObjAdd->Suiv = DecorsObjList;
    DecorsObjAdd->Prec = NULL;
    if (DecorsObjList!=NULL) DecorsObjList->Prec = DecorsObjAdd;
    DecorsObjList = DecorsObjAdd;

};
//----------------------------------------------------------------------------//

//== Renvoie l'objet recherché à partir de son numéro ========================//
DecorsObjet* DecorsObjetLib::SearchObj(unsigned int Numero)
{
    DecorsObjet *ptr;
    ptr = DecorsObjList;
    while (ptr!=NULL)
     {
        if (ptr->NumObjet == Numero) return ptr;
        ptr = ptr->Suiv;
     };
   return NULL;
};
//----------------------------------------------------------------------------//

//== Supprime l'objet de la liste ============================================//
void DecorsObjetLib::SupprDecObj(DecorsObjet *ptr)
{
    if (ptr->Prec == NULL) // c'est le 1er élément de la liste
      {  DOLib->DecorsObjList = ptr->Suiv;
      	 if (ptr->Suiv!=NULL) ptr->Suiv->Prec = NULL;
         delete ptr;
      }
    else	// en plein ds la chaîne
      {
       	if (ptr->Suiv!=NULL) ptr->Suiv->Prec = ptr->Prec;
        ptr->Prec->Suiv = ptr->Suiv;
        delete ptr;
      };
    NbTileObj--;
};
//----------------------------------------------------------------------------//

//==  Met à zéro toutes les valeures ObjDessine ==============================//
void DecorsObjetLib::ObjetNonDessine()
{
    DecorsObjet *ptr;
    ptr = DecorsObjList;
    while (ptr!=NULL)
     {
        ptr->ObjDessine = 0;
        ptr = ptr->Suiv;
     };
};
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : TUNIVERSALTILELIB										  //
////////////////////////////////////////////////////////////////////////////////

//== Constructeur ============================================================//
TUniversalTileLib::TUniversalTileLib()
{
    NbTypeUnivers = 0;
    NbTypeTiles	  = 0;
    UnivLib = NULL;
};
//----------------------------------------------------------------------------//

//== Initialise le Tableau dynamique des Librairies de décors/fonds ==========//
void TUniversalTileLib::AlloueLib()
{
    // utilise les paramètres définis ds "consts.h"
    NbTypeUnivers = cNbTypeUnivers;
    NbTypeTiles   = cNbTypeTiles;

    // Alloue la mémoire nécessaire
	UnivLib = new DecorsTileLib* [NbTypeUnivers]; 					 // lignes
    for (int i=0 ; i < NbTypeUnivers; i++) UnivLib[i] = new DecorsTileLib [NbTypeTiles];  // colonnes

    // Initialise les données des cases
    for (int x=0; x < NbTypeUnivers; x++)
    	for (int y=0; y < NbTypeTiles; y++)
           {
              // Charge la librairie d'aprés la combinaison de nom
              AnsiString FileName;
              FileName  = cUniversName[x];		// Univers + TypedeTiles
              FileName += cTypeTileName[y];
              FileName +=".LiD";
           	  UnivLib[x][y].LoadFile(FileName);

              // Ajoute quelque trucs sur le type de Décors/Fonds
              if ( (y >= cIndexSols) && (y < cIndexSols+cNbTilesSols))
                 UnivLib[x][y].TypeLib = Lib_SOLS;
              else if ( (y >= cIndexDecors) && (y < cIndexDecors+cNbTilesDecors))
                 UnivLib[x][y].TypeLib = Lib_DECORS;
           }
};
//----------------------------------------------------------------------------//

//== Libère la mémoire du tableau dynamique ==================================//
void TUniversalTileLib::DesaloueLib()
{
    for (int i=0; i < NbTypeUnivers; i++) delete[] UnivLib[i]; // lignes
    delete[] UnivLib;								       // colonnes
};
//----------------------------------------------------------------------------//

//== Destructeur =============================================================//
TUniversalTileLib::~TUniversalTileLib()
{
	if (UnivLib!=NULL) DesaloueLib();		// libère la mémoire des cases
};
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : TLEVELTILELIB										      //
////////////////////////////////////////////////////////////////////////////////

// == Constructeur ===========================================================//
TLevelTileLib::TLevelTileLib()
{
    Nom = "";
    NbTypeTiles = 0;
    NbTotalTiles = 0;
    MiniLibSelect = cIndexDecors;  // par défaut ds l'éditeur
    //  Indique si la mini-lib de tiles est une lib de Décors ou de Sols
    //  et combien ds tiles y'a-t-il par mini-lib  (0 à l'ouverture);
    //  et crée une mini-lib vide
    NbTilesPerType = new int [cNbTypeTiles];
    TypeTileLib    = new int [cNbTypeTiles];
    TileMiniLibList = new TileDecors* [cNbTypeTiles];
    for (int i=0; i < cNbTypeTiles; i++)
      { if ( (i >= cIndexSols) && (i < cIndexSols+cNbTilesSols))
           TypeTileLib[i] = Lib_SOLS;
        else if ( (i >= cIndexDecors) && (i < cIndexDecors+cNbTilesDecors))
           TypeTileLib[i] = Lib_DECORS;
        NbTilesPerType[i] = cIndexDecors;
        TileMiniLibList[i] = NULL;
      };
};
//----------------------------------------------------------------------------//

//== ajoute en dernier de la mini-lib le tile de décors/sols =================//
void TLevelTileLib::Ajouter(TileDecors *TileAdd, int index)
{
    NbTilesPerType[index]++;  // un tile de plus ds la mini-lib...
    NbTotalTiles++;
    TileAdd->NumDsList = NbTilesPerType[index];
    TileAdd->LibIndex  = index;

    // Ajout de ce Tile à la liste
    TileAdd->Suiv = TileMiniLibList[index];
    TileAdd->Prec = NULL;
    if (TileMiniLibList[index]!=NULL) TileMiniLibList[index]->Prec = TileAdd;
    TileMiniLibList[index] = TileAdd;
};
//----------------------------------------------------------------------------//

//== Destructeur =============================================================//
TLevelTileLib::~TLevelTileLib()
{
	if (TileMiniLibList!=NULL) DeleteAllLevelLib();		// libère la mémoire des cases
};
//----------------------------------------------------------------------------//

//== Supprime UNE SEULE des mini-libs ========================================//
void TLevelTileLib::EffaceOneLevelLib(int index)
{
    TileDecors *ptr;
    ptr = TileMiniLibList[index];
    while (ptr!=NULL)
         {
      	    TileMiniLibList[index] = TileMiniLibList[index]->Suiv;
            delete ptr;
            ptr = TileMiniLibList[index];
         };
    TileMiniLibList[index] = NULL;
    TypeTileLib[index] = Lib_NULL;
    NbTilesPerType[index] = 0;
};
//----------------------------------------------------------------------------//

//== Libère la mémoire du tableau dynamique ==================================//
void TLevelTileLib::DeleteAllLevelLib()
{
    // Efface chaque mini-lib thématique
    for (int i=0; i < cNbTypeTiles; i++) EffaceOneLevelLib(i);

    // Redémare à zéro quelques trucs
    Nom = "";
    NbTypeTiles = 0;
    NbTotalTiles = 0;
};
//----------------------------------------------------------------------------//

//== Vérifie si le Tile n'existe déjà pas par son nom ds tte la Lib ==========//
BOOL TLevelTileLib::ExisteTile(AnsiString SameName)
{
    TileDecors *ptr;
    for (int i=0; i < cNbTypeTiles; i++)
     {  ptr = TileMiniLibList[i];
        while (ptr!=NULL)
         {
            if ( strcmp(ptr->Nom,SameName.c_str()) == 0) return TRUE;
            ptr = ptr->Suiv;
         };
      };
    return FALSE;
};
//----------------------------------------------------------------------------//

//=== Renvoie un pointeur sur le Tile ayant ce n°ID ==========================//
// Nouveau depuis le 05/05/99
TileDecors* TLevelTileLib::getTileDecors(int numID)
{   TileDecors *ptr;
    for (int i=0; i < cNbTypeTiles; i++)
     {  ptr = TileMiniLibList[i];
        while (ptr!=NULL)
         {  if (ptr->DecorsNum == numID) return ptr;
            ptr = ptr->Suiv;
         };
      };
    return NULL;
}
//----------------------------------------------------------------------------//

//== Sauvegarde ds un fichier *.LiL toute la Lib du Level ====================//
//  => fichier au format binaire pour plus de compréhensions /*.LiD
void TLevelTileLib::SaveToFile()
{
    FILE *f;
    AnsiString fNom = Nom;  fNom+=".LiL";
    if ((f = fopen(fNom.c_str(),"wb"))==NULL)
    	MessageBoxA(NULL,"Impossible de sauver le fichier :",fNom.c_str(),1);

    // Sauvegarde les trucs importants
    char n [15];							// Nom de la Lib
    memset(n,' ', 15);
    strcpy(n,Nom.c_str());
    fwrite(n, 15, 1, f);
    fwrite(&NbTypeTiles , sizeof(int), 1, f);
    fwrite(&NbTotalTiles, sizeof(int), 1, f);

    // Puis sauvegarde chaque mini-lib une part une
    TileDecors *ptr;
	for (int i=0; i < cNbTypeTiles; i++)
     {
      ptr = TileMiniLibList[i];
      fwrite(&NbTilesPerType[i], sizeof(int), 1, f);
      fwrite(&TypeTileLib[i], sizeof(int), 1, f);
      // Sauve dans le sens contraire => cherche le dernier pointer
      if (ptr!=NULL) while (ptr->Suiv!=NULL) ptr = ptr->Suiv;
      while (ptr!=NULL)
        {
           fwrite(ptr->Nom, sizeof(ptr->Nom), 1 ,f);
           ptr = ptr->Prec;   // redescend
        };
     };
    fclose(f);
};
//----------------------------------------------------------------------------//

//== Charge le fichier *.LiL ds la structure du Level ========================//
void TLevelTileLib::LoadFile(AnsiString FileName)
{
    FILE *f;
    if ((f = fopen(FileName.c_str(),"rb"))==NULL)
      {	MessageBoxA(NULL,FileName.c_str(),"Impossible de charger le fichier :",1);
        return; };

    char n[15];
    fread(n, 15, 1, f); Nom = n;
    fread(&NbTypeTiles, sizeof(int), 1, f);
    fread(&NbTotalTiles, sizeof(int), 1, f);

    // Puis charge chaque mini-lib une part une
    TileDecors *NewTile;
    for (int i=0; i < NbTypeTiles ; i++)
      {
        // Charge les infos de la Mini-Lib
        fread(&NbTilesPerType[i], sizeof(int), 1, f);
        fread(&TypeTileLib[i], sizeof(int), 1, f);

        int NbTile2 = 0;
        for (int u=0 ; u < NbTilesPerType[i] ; u++)
         {
           // nouvel Mini-Lib à la liste
           NewTile = new TileDecors;

           NbTile2++;  // un tile de plus ...
           NewTile->NumDsList = NbTile2;

           // ini des chaines
           memset(NewTile->Nom,' ',MaxChar);
           fread(NewTile->Nom, sizeof(NewTile->Nom), 1 ,f);
           AnsiString n;
           switch (TypeTileLib[i])
             {
                case (Lib_DECORS) : n ="Decors\\"; break;
                case (Lib_SOLS)   : n ="Sols\\";   break;
                default			  : n ="Sols\\";
             };
           n += cTypeTileName[i];  n += "\\";
           NewTile->TilePath = n;
           n += NewTile->Nom;	// nom du fichier *.dec
           n +=".dec";			// extension *.dec
           LoadTileDecors(NewTile, n.c_str());

           // Ajout de ce Tile à la liste
	       NewTile->Suiv = TileMiniLibList[i];
     	   NewTile->Prec = NULL;
           if (TileMiniLibList[i]!=NULL) TileMiniLibList[i]->Prec = NewTile;
           TileMiniLibList[i] = NewTile;

          };
       };

   fclose(f);
};
//----------------------------------------------------------------------------//

//== Alloue et charges les images ds les surfaces des différents Tiles =======//
void TLevelTileLib::AlloueDDSurface()
{
    // Défile ds le cortège des tiles
    TileDecors *ptr;
    for (int u=0; u < cNbTypeTiles ; u++)
    {
      ptr = TileMiniLibList[u];
      while (ptr!=NULL)
       {
          ptr->TileSurface = new DDSurface(&DS_VA->pDD, ptr->Width, ptr->Height, VIDEO_MEMORY);
          AnsiString BitmapName = ptr->Nom; BitmapName+=".bmp";

          BitmapName = CheminApplication + cPathTiles + ptr->TilePath + BitmapName;

          if (DDReLoadBitmap(ptr->TileSurface->pDDSurf, BitmapName.c_str())!=DD_OK) Application->Terminate();
          ptr->TileSurface->ColorKey(RGBto16(ptr->RVB),RGBto16(ptr->RVB));
          ptr = ptr->Suiv;
       };
   };
};
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : DECORSOBJET 										      //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
DecorsObjet::DecorsObjet()
{   NumObjet = 0;	// n° non attribué
    ObjDessine = 0;     // pas encore affiché

    peEtat = peFutEtat = peOldEtat = ARRET1;
    peNumSprite = peNbSprite = peSprNumLib = 0;
    Element = NULL;
    peNumClassikAnim = NULL;
};
//----------------------------------------------------------------------------//

//=== Renvoie le n° de l'anim courante =======================================//
int DecorsObjet::getNumAnim()
{ int anm = 0;
  while (anm < Element->peNbAnim)
  { if (Element->AnimSpriteLib[anm]->Numero==peEtat) return anm;
    anm++;
  };
  return -1;  // n° impossible
}
//----------------------------------------------------------------------------//


//=== Fonction d'animation des décors d'aprés leurs états ====================//
void DecorsObjet::ResultAction()
{
    // Trouve le n° de l' ACTION COURANTE
    int anm = -1,  i = 0;
    while ( (i < Element->peNbAnim) && (anm==-1) )
      {  if (Element->AnimSpriteLib[i]->Numero==peEtat) anm = i;
      	 i++;  };

    // renvoie les informations de l'ACTION
    peNbSprite=Element->AnimSpriteLib[anm]->NbSprites;
    // Si l'animation a changée, il se peut que l'on doit revenir au départ
    if (peNumSprite>=peNbSprite) peNumSprite=0;

    // renvoie le n° du sprite correspondant
    switch (Element->AnimSpriteLib[anm]->Type)
       {
         case 0 : // Image BMP d'un TileDecors
            {  Element->peShowTile = true;
               // Place le TileDecors correspondant dans 'MainTile'
               Element->MainTile = DULib->getTileDecors(Element->AnimSpriteLib[anm]->FirstSprite);
            }; break;
       	 case 1 : // 8 positions de caméra
            {  peSprNumLib = peNumSprite + peNbSprite * peDirection
                             + Element->AnimSpriteLib[anm]->FirstSprite;
               Element->peShowTile = false;
            }; break;
       	 case 3 : // 1 position de caméra
            {  peSprNumLib = peNumSprite + Element->AnimSpriteLib[anm]->FirstSprite;
               Element->peShowTile = false;
            }; break;
   	 case 2 : // n° des sprites réparties dans un tableau
            {  peSprNumLib = Element->AnimSpriteLib[anm]->SprRepertoire[peDirection][peNumSprite];
               Element->peShowTile = false;
            }; break;
         case 4 : // n° des sprites réparties dans un tableau : 1 suele position de caméra
            {  peSprNumLib = Element->AnimSpriteLib[anm]->SprRepertoire[0][peNumSprite];
               Element->peShowTile = false;
            }; break;

         default : peSprNumLib = 0;   // au pire des cas
       };
};
//----------------------------------------------------------------------------//

//=== Anim (et fait avancer) les décors ======================================//
void DecorsObjet::peBouge()
{
    // Transfert des informations de l'action courante vers le perso
    ResultAction();

    // Calcul le déplacement et affecte les nouvelles coordonnées au perso
    // DeplacementRealist();
    // Deplacement();

    // Si l'on arrive à la fin de l'animation et rien n'a changé, => on revient à son début
    if (++peNumSprite>=peNbSprite) peNumSprite=0;
}
//----------------------------------------------------------------------------//

//=== Assigne à l'objet un patron de décors ==================================//
void DecorsObjet::SetAnimTileDecors(TAnimTileDecors *element)
{   Element = element;

    // Initialise toutes lesanims superposables "classiques" à NULL (<=> -1)
    if (element->peNbClassikAnim != 0)
    {	peNumClassikAnim = new int[element->peNbClassikAnim];
    	memset(peNumClassikAnim, -1, sizeof(peNumClassikAnim));
    	TRACEDBG("element->peNbClassikAnim : %d  /  sizeof(peNumClassikAnim) : %d\n", element->peNbClassikAnim, sizeof(peNumClassikAnim));
    }
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe TAnimTileDecors                                                     //
////////////////////////////////////////////////////////////////////////////////

//--- Constantes statiques de la classe ---
float TAnimTileDecors::MdlVersion = 0.5;         // N° de version du mdl
float TAnimTileDecors::AnimInfosVersion = 1.0;   // N° de version de la structure SAnimsInfos


//=== Constructeur ===========================================================//
TAnimTileDecors::TAnimTileDecors()
{  peNbAnim = 0;
   peNbTrans = 0;
   AnimSpriteLib = NULL;
   AnimTransLib = NULL;
   nbObjetFrames = 0;
   FrameSuperpList = NULL;
   STLFrameSuperp = NULL;

   MainTile  = NULL;
   peShowTile = false;
   AnimInfos = NULL;

   ClassikAnimInfos = NULL;
   peNbClassikAnim = 0;

   STLObjets = new SpriteTilePersoLib();
}
//----------------------------------------------------------------------------//

//=== Charge les informations sur Transitions & Animations ===================//
void TAnimTileDecors::LoadIna(AnsiString InaName)
{   //  appelle la fonction LoadFileIna de "Animall.cpp"
    AnsiString InaNameTresShort = InaName;
    LoadFileInaDecors(InaNameTresShort.c_str(),&AnimSpriteLib, &AnimTransLib, peNbAnim, peNbTrans,&STLObjets,this);
}
//----------------------------------------------------------------------------//

//=== Charge un fichier MDL ==================================================//
// Charge tout le contenu du fichier .mdl contenant toutes les informations
// de chaque tiles des anims du persos ...
void TAnimTileDecors::LoadMdl(const char* MdlName)
{  // 1) Charge les informations reliant les anims des objets animés entre elles 
   // Alloue la mémoire
   if (AnimInfos!=NULL) delete AnimInfos;
   AnimInfos = new SAnimInfos[peNbAnim];
   // On complète cette structure en l'initialisant à vide
   for (int i=0 ; i < peNbAnim ; i++)
   { AnimInfos[i].numTileDecorsAssoc = i;
     AnimInfos[i].dX = 0;
     AnimInfos[i].dY = 0;
   }
   // On charge les données que l'on a peut-être
   FILE *f;
   // Ouvre le fichier binaire en lecture
   if ((f = fopen(MdlName,"rb"))==NULL)
   { // Si l'ouverture du fichier à échouée -> on suppose que le fichier n'a pas encore été créé
     MessageBoxA(NULL,"Impossible de lire le fichier : Création d'un nouveau fichier vide",MdlName,1);
     if ((f = fopen(MdlName,"wb"))==NULL)
     { MessageBoxA(NULL,"Impossible de créer le fichier : ",MdlName,1);
     } else
     { // Sauve les infos de bases
       // a) N° de version du mdl
       fwrite(&MdlVersion,sizeof(MdlVersion), 1, f);
       // b) Informations des anims
       SaveToFileAnimInfos(*f);
       fclose(f);
     }
   } else
   { // On charge le fichier
     float Version;
     fread(&Version,sizeof(Version), 1, f);
     if (Version < MdlVersion)
     {  AnsiString v = "Ancienne version : ";  v += Version;
         v += "/ version courante : "; v += AnimInfosVersion;
        MessageBoxA(NULL,"Version de la structure sAnimInfos trop ancienne",v.c_str(),1);
        return;
     }
     LoadFromFileAnimInfos(*f);
     LoadFromFileAnimClassik(*f);   // Infos des anims "classiques"
     fclose(f);
  }
}
//----------------------------------------------------------------------------//

//=== Sauve les Infos des anims ==============================================//
void TAnimTileDecors::SaveToFileAnimInfos(FILE &fichier)
{   // a) Version de la structure AnimInfos
    fwrite(&AnimInfosVersion,sizeof(AnimInfosVersion), 1, &fichier);
    // b) Nombre de structures d'infos <=> nb d'anims actuelles
    fwrite(&peNbAnim,sizeof(peNbAnim), 1, &fichier);
    // c) Liste des structures d'infos
    for (int num = 0; num < peNbAnim ; num++)
    {  fwrite(&AnimInfos[num].numTileDecorsAssoc,sizeof(AnimInfos[num].numTileDecorsAssoc), 1, &fichier);
       fwrite(&AnimInfos[num].dX,sizeof(AnimInfos[num].dX), 1, &fichier);
       fwrite(&AnimInfos[num].dY,sizeof(AnimInfos[num].dY), 1, &fichier);
    }
}
//----------------------------------------------------------------------------//

//=== Charge les Infos des anims =============================================//
void TAnimTileDecors::LoadFromFileAnimInfos(FILE &fichier)
{   // a) Version de la structure AnimInfos
    float Version;
    fread(&Version,sizeof(Version), 1, &fichier);
    if (Version < AnimInfosVersion)
    {  AnsiString v = "Ancienne version : ";  v += Version;
        v += "/ version courante : "; v += AnimInfosVersion;
       MessageBoxA(NULL,"Version de la structure sAnimInfos trop ancienne",v.c_str(),1);
       return; 
    }

    // b) Nombre de structures d'infos <=> nb d'anims actuelles
    int nbInfos = 0; // peut-être différent de 'peNbAnim' courant si l'on a rajouté des anims au décors
    fread(&nbInfos,sizeof(nbInfos), 1, &fichier);
    // c) Liste des structures d'infos
    for (int num = 0; num < nbInfos ; num++)
    {  fread(&AnimInfos[num].numTileDecorsAssoc,sizeof(AnimInfos[num].numTileDecorsAssoc), 1, &fichier);
       fread(&AnimInfos[num].dX,sizeof(AnimInfos[num].dX), 1, &fichier);
       fread(&AnimInfos[num].dY,sizeof(AnimInfos[num].dY), 1, &fichier);
    }
}
//----------------------------------------------------------------------------//

//=== Renvoie un poiteur sur la structure ayant ce n° d'id ===================//
SClassikAnimInfos* TAnimTileDecors::GetClassikAnim(int id)
{	for (int i=0 ; i < peNbClassikAnim ; i++)
	{	if (ClassikAnimInfos[i].iAnimID == id) return &ClassikAnimInfos[i];
    }
    return NULL;
}
//----------------------------------------------------------------------------//

//=== Sauve les infos sur les anims superposables "classiques" ===============//
void TAnimTileDecors::SaveToFileAnimClassik(FILE &fichier)
{   // Nb d'anims classiques sauvées
	fwrite(&peNbClassikAnim, sizeof(peNbClassikAnim), 1 ,&fichier);
    // Puis sauvegarde tous les ClassikAnimInfos
    for (int i=0; i < peNbClassikAnim ; i++)
    {	fwrite(&ClassikAnimInfos[i].iAnimID, sizeof(ClassikAnimInfos[i].iAnimID), 1, &fichier);
        fwrite(&ClassikAnimInfos[i].dX, sizeof(ClassikAnimInfos[i].dX), 1, &fichier);
        fwrite(&ClassikAnimInfos[i].dY, sizeof(ClassikAnimInfos[i].dY), 1, &fichier);
        fwrite(&ClassikAnimInfos[i].iMainAnimID, sizeof(ClassikAnimInfos[i].iMainAnimID), 1, &fichier);
        fwrite(&ClassikAnimInfos[i].reserved, sizeof(ClassikAnimInfos[i].reserved), 1, &fichier);
    }
}
//----------------------------------------------------------------------------//

//=== Charge les infos sur les anims superposables "classiques" ==============//
bool TAnimTileDecors::LoadFromFileAnimClassik(FILE &fichier)
{	// Nb d'anims classiques sauvegardées
	int nbClassikAnim;
    int nbOctets;
	nbOctets = fread(&nbClassikAnim, sizeof(nbClassikAnim), 1 ,&fichier);
    if (nbOctets == 0) return false;
    // Puis lit tous les ClassikAnimInfos et regarde si l'on en a besoin
    SClassikAnimInfos infos;
    for (int i=0; i < nbClassikAnim ; i++)
    {	fread(&infos.iAnimID, sizeof(infos.iAnimID), 1, &fichier);
        fread(&infos.dX, sizeof(infos.dX), 1, &fichier);
        fread(&infos.dY, sizeof(infos.dY), 1, &fichier);
        fread(&infos.iMainAnimID, sizeof(infos.iMainAnimID), 1, &fichier);
        fread(&infos.reserved, sizeof(infos.reserved), 1, &fichier);

        // Regarde si l'on a besoin de ces infos
        for (int j=0 ; j < peNbClassikAnim ; j++)
        {	if (ClassikAnimInfos[j].iAnimID == infos.iAnimID)
        	{	ClassikAnimInfos[j].dX = infos.dX;
	            ClassikAnimInfos[j].dY = infos.dY;
            	ClassikAnimInfos[j].iMainAnimID = infos.iMainAnimID;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------//


///////////////////////// FIN CLASSE TANIMTILEDECORS////////////////////////////
