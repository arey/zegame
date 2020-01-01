////////////////////////////////////////////////////////////////////////////////
// ImDécors.cpp : rassemble les informations d'un sprite du décors            //
// Conçu pour l'éditeur de niveau, rénové pour le jeu						  //
//  => Il faudra peut-être faire du ménage                                    //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------- include
#ifdef CBUILDER
#include <vcl\vcl.h>
#endif
#include <stdio.h>
#include "ZeNain.h"
#include "consts.h"
#include "SpritesLib.h"
#include "D3DLib.h"
#include "DDLibAsm.h"
#include "GameDebug.h"
#include "DDUtil.h"
#include "Animall.h"
#include "PersoLib.h"
#include "MemLib.h"				// Lib. regroupant divers canaux de communication entre modules
#include "FileVirtual.h"		// Fichiers virtuels (dans un .pak, en mémoire ou tout simplement sur le disk)
#include "ImageUtil.h"			// Lib. de chargements d'images dans des surfaces DirectDraw
#include "ImDecors2.h"			// Header de la classe
#pragma hdrstop


// Librairie en cours d'utilisation : que ce soit ds la fiche de librairie ou
// dans l'éditeur
TileDecors     *TDSelect = NULL;	// Décors-Tile sélectionné ds la liste
DecorsObjet    *DOSelect = NULL;	// Objet-Décors sélectionné ds la liste ou sur la grille
DecorsObjetLib *DOLib = NULL;		// Librairie des Objet-Décors de l'éditeur
DecorsTileLib  *DTLib = NULL;		// Librairie des Décors-Tiles de l'éditeur
TLevelTileLib  *DULib = NULL;		// Librairie des Décors-Tiles de l'éditeur (2éme version)


double LibTileVersion = 1.01;		// version de la librairie Tile  (utilisé lorsque l'on rajoute des nouveaux éléments ds la liste)
AnsiString  TileDecExt = ".dec";	// extension de fichier décors
const char LibDecExt []  =  ".LiD"; 	// extension des librairies de décors
float TileDecVersion = 0.98f;		// Version du Programme : dernière mise à jour le 27/10/98

const char TrapTERRAINText [TrapTERRAINNb][20] = { "Inconnu", "Sable", "Terre", "Herbe", "Graviers",
      "Bois", "Marbre", "Racine", "Tronc", "Mur", "Objet" };

const char TrapCOLLIDEText [TrapCOLLIDENb][20] = {"Partielle", "Totale", "Zone_Pied"};

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
	#ifndef NEW_FILE_MANAGER
	FILE *f;
	#else
	TFileVirtual *file = NULL;
	#endif	// NEW_FILE_MANAGER

	char FileToLoadCommon[512];
	unsigned char terrain;
	char temp2[2];
	char temp1[3];

	#ifndef NEW_FILE_MANAGER
	AnsiString FileToLoad = Name;
    FileToLoad = CheminApplication + cPathTiles + FileToLoad;
	#ifdef CBUILDER
	strcpy(FileToLoadCommon, FileToLoad.c_str());
	#else
	strcpy(FileToLoadCommon, LPCSTR(FileToLoad));
	#endif
	if ((f = fopen(LPCSTR(FileToLoadCommon),"rb"))==NULL)
	{    	MessageBoxA(NULL, FileToLoadCommon, "Impossible de charger le fichier :", 1);
    }
	#else
	#ifdef CBUILDER
	strcpy(FileToLoadCommon, cPathTiles.c_str());
	#else
	strcpy(FileToLoadCommon, LPCSTR(cPathTiles));
	#endif
	strcat(FileToLoadCommon, Name); 
	file = fopenFV(FileToLoadCommon);
	if (file == NULL) 
	{	MessageError("ImDecors2.cpp","LoadTileDecors - Impossible de lire le .dec",99); 
		_asm int 3;
	}
	#endif	// NEW_FILE_MANAGER
	
	// Lit les données du .dec
	float DecVersion;
	#ifndef NEW_FILE_MANAGER
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
	#else
	freadFV(&DecVersion, sizeof(float), file);
    if (DecVersion < TileDecVersion) { MessageBoxA(NULL,"Version ancienne de sauvegarde","Problème de lecture",1); fcloseFV(file); return; };
    freadFV(&TileLoading->DecorsNum, sizeof(int), file);
	freadFV(&TileLoading->Nom, sizeof(TileLoading->Nom), file);
    freadFV(&TileLoading->Width, sizeof(int), file);
    freadFV(&TileLoading->Height, sizeof(int), file);
    freadFV(&TileLoading->RVB, sizeof(unsigned int), file);
    freadFV(&TileLoading->NumRVB, sizeof(unsigned char), file);
    freadFV(&TileLoading->NbTrapeze, sizeof(int), file);
    freadFV(&TileLoading->TrapPrincX, sizeof(int), file);
    freadFV(&TileLoading->TrapPrincY, sizeof(int), file);
	#endif // NEW_FILE_MANAGER
    
	
	TileLoading->TrapezTab = new Trapeze[TileLoading->NbTrapeze];
    for (int i=0; i < TileLoading->NbTrapeze; i++)
	{	
		#ifndef VISUALC	// alignement sur 4 octets
		// Chez Borland, la struct 'Trapeze' a une taille de 72 octets
		#ifndef NEW_FILE_MANAGER
		fread(&TileLoading->TrapezTab[i], sizeof(Trapeze), 1, f);
		#else
		freadFV(&TileLoading->TrapezTab[i], sizeof(Trapeze), file);
		#endif	// NEW_FILE_MANAGER
		#else	// alignement sur 8 octets
		// Chez Microsoft, elle a une taille de 76 octets
		#ifndef NEW_FILE_MANAGER
		fread(&TileLoading->TrapezTab[i].TrapU, sizeof(int), 1, f);
		fread(&TileLoading->TrapezTab[i].TrapV, sizeof(int), 1, f);
		fread(&TileLoading->TrapezTab[i].Hauteur, sizeof(int), 1, f);
		fread(&terrain, sizeof(unsigned char), 1, f);
		fread(&temp1, sizeof(temp1), 1,f);
		TileLoading->TrapezTab[i].Terrain = (TrapTERRAIN) terrain;
		fread(&TileLoading->TrapezTab[i].Accessible, sizeof(BOOL), 1, f);
		fread(&terrain, sizeof(unsigned char), 1, f);
		TileLoading->TrapezTab[i].TrapCollide = (eTrapCOLLIDE) terrain;		
		fread(&temp2, sizeof(temp2), 1,f);
		fread(&TileLoading->TrapezTab[i].DrawTrapezekey, sizeof(bool), 1, f);
		fread(&TileLoading->TrapezTab[i].lamX1, sizeof(int), 1, f);
		fread(&TileLoading->TrapezTab[i].lamY1, sizeof(int), 1, f);
		fread(&TileLoading->TrapezTab[i].lamX2, sizeof(int), 1, f);
		fread(&TileLoading->TrapezTab[i].lamY2, sizeof(int), 1, f);
		fread(&TileLoading->TrapezTab[i].reserved, sizeof(int)*8, 1, f);
		#else
		freadFV(&TileLoading->TrapezTab[i].TrapU, sizeof(int), file);
		freadFV(&TileLoading->TrapezTab[i].TrapV, sizeof(int), file);
		freadFV(&TileLoading->TrapezTab[i].Hauteur, sizeof(int), file);
		freadFV(&terrain, sizeof(unsigned char), file);
		freadFV(&temp1, sizeof(temp1), file);
		TileLoading->TrapezTab[i].Terrain = (TrapTERRAIN) terrain;
		freadFV(&TileLoading->TrapezTab[i].Accessible, sizeof(BOOL), file);
		freadFV(&terrain, sizeof(unsigned char), file);
		TileLoading->TrapezTab[i].TrapCollide = (eTrapCOLLIDE) terrain;		
		freadFV(&temp2, sizeof(temp2), file);
		freadFV(&TileLoading->TrapezTab[i].DrawTrapezekey, sizeof(bool), file);
		freadFV(&TileLoading->TrapezTab[i].lamX1, sizeof(int), file);
		freadFV(&TileLoading->TrapezTab[i].lamY1, sizeof(int), file);
		freadFV(&TileLoading->TrapezTab[i].lamX2, sizeof(int), file);
		freadFV(&TileLoading->TrapezTab[i].lamY2, sizeof(int), file);
		freadFV(&TileLoading->TrapezTab[i].reserved, sizeof(int)*8, file);
		#endif	// NEW_FILE_MANAGER
		#endif	// VISUALC
	}
	#ifndef NEW_FILE_MANAGER
    fread(&TileLoading->DecorsCornerX, sizeof(int), 1, f);
    fread(&TileLoading->DecorsCornerY, sizeof(int), 1, f);
    fread(&TileLoading->TotalDraw, sizeof(bool), 1, f);
    //--- Nouveau depuis la version 0.98 ---
    fread(&TileLoading->CircleCollision, sizeof(TileLoading->CircleCollision), 1, f);
    fread(&TileLoading->CircleCenterX, sizeof(TileLoading->CircleCenterX), 1, f);
    fread(&TileLoading->CircleCenterY, sizeof(TileLoading->CircleCenterY), 1, f);
    fread(&TileLoading->CircleRadius, sizeof(TileLoading->CircleRadius), 1, f);
	fread(&TileLoading->DrawMethode, sizeof(TileLoading->DrawMethode), 1, f);
    fread(&TileLoading->reserved, sizeof(TileLoading->reserved), 1, f);
    fclose(f); // fin de lecture du fichier
	#else
	freadFV(&TileLoading->DecorsCornerX, sizeof(int), file);
    freadFV(&TileLoading->DecorsCornerY, sizeof(int), file);
    freadFV(&TileLoading->TotalDraw, sizeof(bool), file);
    //--- Nouveau depuis la version 0.98 ---
    freadFV(&TileLoading->CircleCollision, sizeof(TileLoading->CircleCollision), file);
    freadFV(&TileLoading->CircleCenterX, sizeof(TileLoading->CircleCenterX), file);
    freadFV(&TileLoading->CircleCenterY, sizeof(TileLoading->CircleCenterY), file);
    freadFV(&TileLoading->CircleRadius, sizeof(TileLoading->CircleRadius), file);
	freadFV(&TileLoading->DrawMethode, sizeof(TileLoading->DrawMethode), file);
    freadFV(&TileLoading->reserved, sizeof(TileLoading->reserved), file);
    fcloseFV(file); // fin de lecture du fichier
	#endif	// NEW_FILE_MANAGER
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
		#ifdef CBUILDER
		if ( strcmp(ptr->Nom, SameName.c_str()) == 0) return TRUE;
		#else
		if ( strcmp(ptr->Nom, LPCSTR(SameName)) == 0) return TRUE;
		#endif
        ptr = ptr->Suiv;
     };
    return FALSE;
};
//----------------------------------------------------------------------------//

//== Sauve la librairie ds le fichier ayant son nom ==========================//
void DecorsTileLib::SaveToFile()
{
    FILE *f;
    char fNomCommon[512];
	strcpy(fNomCommon, Nom);
	strcat(fNomCommon, LibDecExt);

    if ((f = fopen(fNomCommon,"wb"))==NULL)
    	MessageBoxA(NULL,"Impossible de sauver le fichier :", fNomCommon, 1);
    // Sauvegarde les trucs importants
    char n [15];
    memset(n,' ', 15);
    strcpy(n,fNomCommon);
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
   	#ifndef NEW_FILE_MANAGER
	FILE *f;
	#else
	TFileVirtual *file = NULL;
	#endif	// NEW_FILE_MANAGER

	#ifdef CBUILDER
	#ifndef NEW_FILE_MANAGER
    if ((f = fopen(FileToLoad.c_str(),"rb"))==NULL)
      {	MessageBoxA(NULL,FileToLoad.c_str(),"Impossible de charger le fichier :",1);
        return; };
	#else	// NEW_FILE_MANAGER
	file = fopenFV(FileToLoad.c_str());
	if (file == NULL) 
	{	MessageBoxA(NULL,FileToLoad.c_str(),"Impossible de charger le fichier :",1);
        return; 
	}
	#endif	// NEW_FILE_MANAGER
	#else	// CBUILDER
	#ifndef NEW_FILE_MANAGER
	if ((f = fopen(LPCSTR(FileToLoad),"rb"))==NULL)
      {	MessageBoxA(NULL,LPCSTR(FileToLoad),"Impossible de charger le fichier :",1);
        return; };

	#else	// NEW_FILE_MANAGER
	file = fopenFV((char*) LPCSTR(FileToLoad));
	if (file == NULL) 
	{	MessageBoxA(NULL,LPCSTR(FileToLoad),"Impossible de charger le fichier :",1);
        return; 
	}
	#endif	// NEW_FILE_MANAGER
	#endif	// CBUILDER

    char n[15];
    #ifndef NEW_FILE_MANAGER
	fread(n, 15, 1, f); Nom = n;
    fread(&NbTile, sizeof(int), 1, f);
	#else
	freadFV(n, 15, file); Nom = n;
    freadFV(&NbTile, sizeof(int), file);
	#endif	// NEW_FILE_MANAGER

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
           #ifndef NEW_FILE_MANAGER
		   fread(NewTile->Nom, sizeof(NewTile->Nom), 1 ,f);
		   #else
		   freadFV(NewTile->Nom, sizeof(NewTile->Nom), file);
		   #endif	// NEW_FILE_MANAGER	
		   
           char n[512];
		   strcpy(n, NewTile->Nom);
		   strcat(n,".dec");           
		   LoadTileDecors(NewTile, n);

           // Ajout de ce Tile à la liste
           NewTile->Suiv = DecorsTileList;
           NewTile->Prec = NULL;
           if (DecorsTileList!=NULL) DecorsTileList->Prec = NewTile;
           DecorsTileList = NewTile;
      };
	#ifndef NEW_FILE_MANAGER
	fclose(f);
	#else
	fcloseFV(file);
	#endif	// NEW_FILE_MANAGER	
}
//----------------------------------------------------------------------------//

//== Alloue et charges les images ds les surfaces des différents Tiles =======//
void DecorsTileLib::AlloueDDSurface()
{   // Défile ds le cortège des tiles
    TileDecors *ptr;
    ptr = DecorsTileList;
    while (ptr!=NULL)
     {
      	ptr->TileSurface = new DDSurface(DS, ptr->Width, ptr->Height, VIDEO_MEMORY);
        char BitmapName[512];
		strcpy(BitmapName, ptr->Nom);
		strcat(BitmapName, ".bmp");
		if (DDReLoadBitmap(ptr->TileSurface->pDDSurf, BitmapName)!=DD_OK) 
		{	
			#ifdef CBUILDER
			Application->Terminate();
			#else
			#ifdef DEBUG
			_asm int 3;
			#else
			exit(0);
			#endif // DEBUG
			#endif // CBUILDER
		}
        ptr->TileSurface->ColorKey(RGBto16(ptr->RVB),RGBto16(ptr->RVB));
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
    DecorsObjet *ptr = DecorsObjList;
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

//=== Met à zéro toutes les valeures ObjDessine ==============================//
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

//=== Fait 'vivre' tous les objets ===========================================//
void DecorsObjetLib::Update()
{ //--- Anime les objets animés ---
  DecorsObjet *ptr = DecorsObjList;
  while (ptr!=NULL)
  {  if (ptr->DecorsAnime)
     {  
		ptr->Update();   // <=> Keyb2FutEtat()
        ptr->Fut2PresEtat();
        ptr->peBouge();
     }
     ptr = ptr->Suiv;
  }
}
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
	delete NbTilesPerType; NbTilesPerType = NULL;
	delete TypeTileLib; TypeTileLib = NULL;
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
			#ifdef BUILDER
			if ( strcmp(ptr->Nom, SameName.c_str()) == 0) return TRUE;
			#else
			if ( strcmp(ptr->Nom, LPCSTR(SameName)) == 0) return TRUE;
			#endif
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
    char fNom[512];
	strcpy(fNom, Nom);
	strcat(fNom, ".LiL");
    if ((f = fopen(fNom,"wb"))==NULL)
    	MessageBoxA(NULL,"Impossible de sauver le fichier :",fNom,1);

    // Sauvegarde les trucs importants
    char n [15];							// Nom de la Lib
    memset(n,' ', 15);
    strcpy(n,Nom);
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
	#ifdef CBUILDER
    if ((f = fopen(FileName.c_str(),"rb"))==NULL)
      {	MessageBoxA(NULL,FileName.c_str(),"Impossible de charger le fichier :",1);
        return; };
	#else
	if ((f = fopen(LPCSTR(FileName),"rb"))==NULL)
      {	MessageBoxA(NULL,LPCSTR(FileName),"Impossible de charger le fichier :",1);
        return; };
	#endif

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
		   #ifdef CBUILDER
		   LoadTileDecors(NewTile, n.c_str());
		   #else		   
		   LoadTileDecors(NewTile, (char*) LPCSTR(n));
		   #endif   		   

           // Ajout de ce Tile à la liste
	       NewTile->Suiv = TileMiniLibList[i];
     	   NewTile->Prec = NULL;
           if (TileMiniLibList[i]!=NULL) TileMiniLibList[i]->Prec = NewTile;
           TileMiniLibList[i] = NewTile;

          }
       }

   fclose(f);
};
//----------------------------------------------------------------------------//

//== Alloue et charges les images ds les surfaces des différents Tiles =======//
void TLevelTileLib::AlloueDDSurface()
{	char bmpName[256];
    // Défile ds le cortège des tiles
    TileDecors *ptr;
    for (int u=0; u < cNbTypeTiles ; u++)
    {
      ptr = TileMiniLibList[u];
      while (ptr!=NULL)
       {
          ptr->TileSurface = new DDSurface(DS, ptr->Width, ptr->Height, VIDEO_MEMORY);
          
		  #ifndef NEW_FILE_MANAGER	
		  AnsiString BitmapName = ptr->Nom; BitmapName+=".bmp";
          BitmapName = CheminApplication + cPathTiles + ptr->TilePath + BitmapName;
		  #ifdef CBUILDER
		  if (DDReLoadBitmap(ptr->TileSurface->pDDSurf, BitmapName.c_str())!=DD_OK) Application->Terminate();
		  #else
		  if (DDReLoadBitmap(ptr->TileSurface->pDDSurf, LPCSTR(BitmapName))!=DD_OK)
		  {	
			#ifdef DEBUG
			_asm int 3;
			#else
			exit(0);
			#endif // DEBUG
		  }
  		  #endif	// CBUILDER
		  #else
		  #ifdef CBUILDER
		  strcpy(bmpName, cPathTiles.c_str());
		  #else
		  strcpy(bmpName, LPCSTR(cPathTiles));	
		  #endif	// CBUILDER
		  strcat(bmpName, ptr->TilePath);
		  strcat(bmpName, ptr->Nom);
		  strcat(bmpName, ".bmp");
		  DDReLoadAnyBitmapFromFile(ptr->TileSurface->pDDSurf, bmpName);
		  #endif	// NEW_FILE_MANAGER

          ptr->TileSurface->ColorKey(RGBto16(ptr->RVB),RGBto16(ptr->RVB));
          ptr = ptr->Suiv;
       }
   }
}
//----------------------------------------------------------------------------//

//=== Fonction appelée après un chgt de Mode Vidéo ===========================//
void TLevelTileLib::ChangeTileVideoMode()
{	HRESULT hr;
	TileDecors* tile;
	LPDIRECTDRAWSURFACE4 surface = NULL;
	
	// Test les surfaces DirectDraw de la liste, et (si il le faut)
	// recrée les surfaces et recharge les bitmaps
	for (int u=0; u < cNbTypeTiles ; u++)
    {	
		tile = TileMiniLibList[u];
		while (tile != NULL)
		{	
			surface = tile->TileSurface->pDDSurf;
			hr = surface->IsLost();
			if (hr == DDERR_SURFACELOST)
			{	hr = surface->Restore();
			}

			if (hr == DDERR_WRONGMODE)
			{	// Libère la surface
				surface->Release();
				// Puis on la réalloue
				tile->TileSurface = new DDSurface(DS, tile->Width, tile->Height, VIDEO_MEMORY);
				// Et l'on recharge le bmp
				AnsiString BitmapName = tile->Nom; BitmapName+=".bmp";
		        BitmapName = CheminApplication + cPathTiles + tile->TilePath + BitmapName;
				#ifdef CBUILDER
				if (DDReLoadBitmap(tile->TileSurface->pDDSurf, BitmapName.c_str())!=DD_OK) Application->Terminate();
				#else
				if (DDReLoadBitmap(tile->TileSurface->pDDSurf, LPCSTR(BitmapName))!=DD_OK)
				{
					#ifdef DEBUG
					_asm int 3;
					#else
					exit(0);
					#endif // DEBUG
				}
				#endif
				tile->TileSurface->ColorKey(RGBto16(tile->RVB),RGBto16(tile->RVB));
			}
			
			tile = tile->Suiv;	// Tile suivant
		}
	}
}
//----------------------------------------------------------------------------//



////////////////////////////////////////////////////////////////////////////////
// Fonction Objet : DECORSOBJET 										      //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
DecorsObjet::DecorsObjet():
	GameObject(START_ID_ANY_DECORS)
{   
	NumObjet = 0;		// n° non attribué
    ObjDessine = 0;     // pas encore affiché

    peEtat = peFutEtat = peOldEtat = ARRET1;       // à l'arrêt
    peDirection = peFutDirection =  or_S;          // orienté vers le sud
    peNumSprite = peNbSprite = peSprNumLib = 0;
    peOldPosU = peOldPosV = 0;
    peCollideEtat = NOSTATE;
    peNumAnim = 0;
    peChangeAnim = false;
    peShowTile = true;
    Element = NULL;
    SecondTile = NULL;
    OldSecondTile = NULL;
    DecorsAnime = false;
    peNumClassikAnim = NULL;
	iEndAnimSynchro = -1;		// Aucune synchronisation de fin d'anim

	peAnimSpeed = 1.0;			// Vitesse d'animation par défaut
	peNumSpriteInterm = 0;
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
    int anm = peNumAnim;
    // Si l'animation a changée, il se peut que l'on doit revenir au départ
    if (peNumSprite>=peNbSprite)
	{	peNumSprite=0;
		peNumSpriteInterm = 0;
		if (iEndAnimSynchro != -1) 
		{	MemMutexLib->FreeMutex(iEndAnimSynchro);
			iEndAnimSynchro = -1;
		}
	}

    // renvoie le n° du sprite correspondant
    switch (Element->AnimSpriteLib[anm]->Type)
       {
         case 0 : // Image BMP d'un TileDecors
            {  peShowTile = true;
               peSprNumLib = 0;
               // On vérifie si l'on n'a pas changé de TileDecors de référence
               if (peChangeAnim)
               {  peOldTileDecorsAssoc = peTileDecorsAssoc;
                  peTileDecorsAssoc = Element->AnimInfos[anm].numTileDecorsAssoc;
                  if (peOldTileDecorsAssoc!=peTileDecorsAssoc)
                  { // Place le TileDecors correspondant dans 'SecondTile'
                    OldSecondTile = SecondTile;
                    SecondTile = DULib->getTileDecors(Element->AnimSpriteLib[anm]->FirstSprite);
                    // Modifie les coordonnées du trap. principale du TileDecors
                    //if (peEtat!=ARRET1)
                    {  peOldPosU = PosU; peOldPosV = PosV;
                       PosU -= Element->AnimInfos[anm].dX;
                       PosV -= Element->AnimInfos[anm].dY;
                    }
                    // Objet changé
                    DOLib->DecorsUpgraded.push_back(this);
                  }
               }
            }; break;
       	 case 1 : // 8 positions de caméra
            {  peSprNumLib = peNumSprite + peNbSprite * peDirection
                             + Element->AnimSpriteLib[anm]->FirstSprite;
               peShowTile = false;
            }; break;
       	 case 3 : // 1 position de caméra
            {  peSprNumLib = peNumSprite + Element->AnimSpriteLib[anm]->FirstSprite;
               peShowTile = false;
            }; break;
   		 case 2 : // n° des sprites réparties dans un tableau : 8 positions de caméra
            {  peSprNumLib = Element->AnimSpriteLib[anm]->SprRepertoire[peDirection][peNumSprite];
               peShowTile = false;
            }; break;
         case 4 : // n° des sprites réparties dans un tableau : 1 suele position de caméra
            {  peSprNumLib = Element->AnimSpriteLib[anm]->SprRepertoire[0][peNumSprite];
               peShowTile = false;
            }; break;

         default : peSprNumLib = 0; peShowTile = true; // au pire des cas
       };
};
//----------------------------------------------------------------------------//

//=== Anim (et fait avancer) les décors ======================================//
void DecorsObjet::peBouge()
{
	
    // Calcul le déplacement et affecte les nouvelles coordonnées au perso
    // DeplacementRealist();
    // Deplacement();

    // Transfert des informations de l'action courante vers le perso
    ResultAction();

    // Incohérences !!
    if ((peNumAnim==0) && (peEtat==ARRET1) && (!peShowTile))
	{	
		#ifdef CBUILDER
		Application->Terminate();
		#else
	#ifdef DEBUG
		_asm int 3;
		#else
		exit(0);
		#endif // DEBUG
		#endif
	}


/*    else
    { // On vérifie si l'on n'a pas changé de TileDecors de référence
      peOldTileDecorsAssoc = peTileDecorsAssoc;
      peTileDecorsAssoc = Element->AnimInfos[peNumAnim].numTileDecorsAssoc;
    }*/
};
//----------------------------------------------------------------------------//

// Rmq : défini provisoirement pour l'IA
#define PIG1     0
#define BOCAL1   1
#define LOCO	 2
#define ROBOT1	 3

//=== Gère PROVISOIREMENT l'IA des DECORS ====================================//
void DecorsObjet::Update()
{
   
	
   // Fait avancer les anims superposables "classiques"
   for (int i=0 ; i < Element->peNbClassikAnim ; i++)
   {  if (peNumClassikAnim[i] != -1)
      {	peNumClassikAnim[i]++;	// frame suivante
        // Vérifie s'il ne faut pas arrêter l'animation
      	AnimSprite*	anim = STGL->GetAnimSprite(Element->ClassikAnimInfos[i].iAnimID);
      	if (peNumClassikAnim[i] >= anim->NbSprites) peNumClassikAnim[i] = -1;
      }
   }

   // I.A. provisoire
   switch (Element->numAnimTileDecorsID)
   { case PIG1 : // Cas du PIG1 : si il est inactif -> on essaye de le faire manger
       if ((peEtat==ARRET1) && (peFutEtat==ARRET1) && (peFutDirection==peDirection))
       { if ((rand() % 120) == 99) peFutEtat=MANGE1;
       }
       break;
	case ROBOT1 :
/*	   if ((peEtat == ARRET1) && (rand() % 100) == 49) peFutEtat=DONNECOUP1;
	   else if ((peEtat == MORT1) && (rand() % 40) == 39) peEtat=ARRET1;*/
	   break;
/*     case BOCAL1 :
       if ((peEtat==ARRET1) && (peFutEtat==ARRET1) && (peFutDirection==peDirection))
       { if (random(300) == 99) peFutEtat=BRISE1;
         else if (random(300) == 99) peFutEtat=BRISE2;
       }
       break;*/
	 /*case LOCO : // Lui fait cracher de la fumée
       for (int i=0 ; i < Element->peNbClassikAnim ; i++)
       {	if (Element->ClassikAnimInfos[i].iAnimID == 1000)	// FUMEELOCO1	 1000
       		{   if ((rand() % 40)==0)
            	if (peNumClassikAnim[i] == -1)
            	{	peNumClassikAnim[i] = 0;	// Début de l'anim
                }
            }
       }
       break;*/
   }
}
//----------------------------------------------------------------------------//

// Transitions FUTUR ETAT -> ETAT PRESENT ------------------------------------//
// RMQ : Repomper complètement sur 'void Personnage::Fut2PresEtat(void)'
// à partir des Directions Futures & Présentes du perso, regarde les transitions
// possibles données par le *.ina et les effectuent si possible
void DecorsObjet::Fut2PresEtat()
{  peChangeAnim = false;
   bool sort = false;

   //  if (peFutEtat!=peEtat) // PAS FORCEMMENT  => ex: MEURT1-MORT1 à l'AIR DE MARCHER
   // scrute toutes les transitions possibles
   for ( int i=0 ; (i < Element->peNbTrans) && (!sort); i++)
   {  // --- TOUT DROIT --
      if (   (peFutEtat!=peEtat)     // Si l'état voulu n'est plus le même que précédemment
          && (Element->AnimTransLib[i]->NumActFut==peFutEtat)
          && ( Element->AnimTransLib[i]->NumActCour==peEtat)
          && (    (Element->AnimTransLib[i]->ImageA==peNumSprite)
               || (Element->AnimTransLib[i]->ImageA== -1)   // cas où ImageA = "n"
             ) )
      {   // on le fait changer d'état
          peOldEtat = peEtat;          // conserve l'ancien état
          peEtat = Element->AnimTransLib[i]->NumActInterm;
          peNumSprite = Element->AnimTransLib[i]->ImageB;
          peNumAnim = getNumAnim();    // Récupère le n° de l'anim
          peNbSprite = Element->AnimSpriteLib[peNumAnim]->NbSprites;
          peChangeAnim = true;
          sort = true;
		  peAnimSpeed = Element->AnimSpriteLib[peNumAnim]->fAnimSpeed;
		  peNumSpriteInterm = (float) peNumSprite;
      }
   }
 
   // Si pas de transition, on avance dans l'anim
   if (!peChangeAnim)
   {	// On prends ici en compte la vitesse d'animation
	   peNumSpriteInterm += peAnimSpeed;
	   peNumSprite = (int) peNumSpriteInterm;
   }

   for (i=0 ; (i < Element->peNbTrans) && (!sort); i++)
	{
      // CHGT AUTONOME DE TRANSITION NumActFut==NumActCour   (ex: MEURT1->MORT1)
      if (     (peFutEtat==peEtat)
           &&  (Element->AnimTransLib[i]->NumActCour==peEtat)
           &&  (Element->AnimTransLib[i]->NumActFut==Element->AnimTransLib[i]->NumActCour)
           &&  (    (Element->AnimTransLib[i]->ImageA+1==peNumSprite)
                 || (Element->AnimTransLib[i]->ImageA==-1) ))
         { peOldEtat = peEtat;                       // conserve l'ancien état
           peEtat = Element->AnimTransLib[i]->NumActInterm;
           peFutEtat = peEtat;
           peNumSprite = Element->AnimTransLib[i]->ImageB;
           peChangeAnim = true;
           sort = true;
           peNumAnim = getNumAnim();    // Récupère le n° de l'anim
           peNbSprite = Element->AnimSpriteLib[peNumAnim]->NbSprites;
		   peAnimSpeed = Element->AnimSpriteLib[peNumAnim]->fAnimSpeed;
		   peNumSpriteInterm = (float) peNumSprite;
         }
   }

   // Si l'on arrive à la fin de l'animation et rien n'a changé, => on revient à son début
   if (!peChangeAnim)
   {	if (peNumSprite>=peNbSprite) 
		{	peNumSprite=0;
		    peNumSpriteInterm = 0;
			if (iEndAnimSynchro != -1) 
			{	MemMutexLib->FreeMutex(iEndAnimSynchro);
				iEndAnimSynchro = -1;
			}	  	
		}
	}

};
//----------------------------------------------------------------------------//

//=== Assigne à l'objet un patron de décors ==================================//
void DecorsObjet::SetAnimTileDecors(TAnimTileDecors *element)
{   Element = element;

    // Initialise toutes lesanims superposables "classiques" à NULL (<=> -1)
    if (element->peNbClassikAnim != 0)
    {	peNumClassikAnim = new int[element->peNbClassikAnim];
    	memset(peNumClassikAnim, -1, sizeof(peNumClassikAnim));
    }
}
//----------------------------------------------------------------------------//


//=== S'occupe d'afficher à l'écran les animations superposées ===============//
void DecorsObjet::RenderClassikAnims(int ScrX, int ScrY)
{
	// Vérifie si c'est un objet animé
	if ((Element==NULL) || (!DecorsAnime)) return;

	// Calcul la position de l'objet sur l'écran
    int X, Y;
    TransUVtoXY(PosU, PosV, &X, &Y);
    X -= ScrX;
    Y -= ScrY;

	// Pour chacune des animations superposables, on regarde si elle est en route
    int posX, posY;
    int animID;
	
	/*AnimSprite* anim = STGL->AnimsMap[1000];
	int spriteID = anim->FirstSprite + 2;
	SpriteTile* sprite = STGL->SpriteMap[spriteID];
	SpriteDrawTransluenceClip(20, 20, DS->pDDSBack, sprite, 100);*/

	for (int i=0 ; i < Element->peNbClassikAnim ; i++)
    {	if (peNumClassikAnim[i] != -1)
    	{	// On doit l'afficher l'anim
        	animID = Element->ClassikAnimInfos[i].iAnimID;
            posX = X + Element->ClassikAnimInfos[i].dX;
            posY = Y + Element->ClassikAnimInfos[i].dY;
            D3DS->InitRenderScene3D();
	    	D3DS->Set3DRenderState(true, false, false);
            STGL->PlayAnim(animID, peNumClassikAnim[i], posX, posY);
            D3DS->CloseRenderScene3D();
        }
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
TAnimTileDecors::TAnimTileDecors(int num_ID):
   numAnimTileDecorsID(num_ID)
{  peNbAnim = 0;
   peNbTrans = 0;
   AnimSpriteLib = NULL;
   AnimTransLib = NULL;
   nbObjetFrames = 0;
   FrameSuperpList = NULL;
   STLFrameSuperp = NULL;
   peNbClassikAnim = 0;

   MainTile  = NULL;
   NoDecorsNumAssoc = -1;
   AnimInfos = NULL;

   STLObjets = new SpriteTilePersoLib();
}
//----------------------------------------------------------------------------//

//=== Charge les informations sur Transitions & Animations ===================//
void TAnimTileDecors::LoadIna(AnsiString InaName)
{   //  appelle la fonction LoadFileIna de "Animall.cpp"
    char InaNameTresShort[512];
	#ifdef CBUILDER
	strcpy(InaNameTresShort, InaName.c_str());
	#else
	strcpy(InaNameTresShort, LPCSTR(InaName));
	#endif
	LoadFileInaDecors(InaNameTresShort, &AnimSpriteLib, &AnimTransLib, peNbAnim, peNbTrans,&STLObjets,this);
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
   // Ouvre le fichier binaire en lecture
   #ifndef NEW_FILE_MANAGER
   FILE *f;
   if ((f = fopen(MdlName,"rb"))==NULL)
   #else
   TFileVirtual *f;
   if ((f = fopenFV((char*) MdlName))==NULL)
   #endif
   { // Si l'ouverture du fichier à échouée -> on suppose que le fichier n'a pas encore été créé
     MessageBoxA(NULL,"Impossible de lire le fichier : Création d'un nouveau fichier vide",MdlName,1);
     FILE *f;
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
	 #ifndef NEW_FILE_MANAGER
     fread(&Version,sizeof(Version), 1, f);
 	 #else
	 freadFV(&Version,sizeof(Version), f);
	 #endif
     if (Version < MdlVersion)
     {  char v[512];
		char vers[64];
		strcpy(v, "Ancienne version : ");  
		_gcvt(Version, 5, vers);
		strcat(v, vers);
		strcat(v, "/ version courante : "); 
		_gcvt(AnimInfosVersion, 5, vers);
		strcat(v, vers);
	    MessageBoxA(NULL,"Version de la structure sAnimInfos trop ancienne", v, 1);
        return;
     }
     LoadFromFileAnimInfos(*f);     // 1ère moitiée du .mdl
     LoadFromFileAnimClassik(*f);   // Infos des anims "classiques"
	 #ifndef NEW_FILE_MANAGER
     fclose(f);
	 #else
	 fcloseFV(f);
	 #endif
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
    {  
		char v[512];
		char vers[64];
		strcpy(v, "Ancienne version : ");  
		_gcvt(Version, 5, vers);
		strcat(v, vers);
		strcat(v, "/ version courante : "); 
		_gcvt(AnimInfosVersion, 5, vers);
		strcat(v, vers);
	    MessageBoxA(NULL,"Version de la structure sAnimInfos trop ancienne", v, 1);
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
void TAnimTileDecors::LoadFromFileAnimInfos(TFileVirtual &fichier)
{   // a) Version de la structure AnimInfos
    float Version;
	fichier.read(&Version, sizeof(Version));
    if (Version < AnimInfosVersion)
    {  	char v[512];
		char vers[64];
		strcpy(v, "Ancienne version : ");  
		_gcvt(Version, 5, vers);
		strcat(v, vers);
		strcat(v, "/ version courante : "); 
		_gcvt(AnimInfosVersion, 5, vers);
		strcat(v, vers);
	    MessageBoxA(NULL,"Version de la structure sAnimInfos trop ancienne", v, 1);
       return;
    }

    // b) Nombre de structures d'infos <=> nb d'anims actuelles
    int nbInfos = 0; // peut-être différent de 'peNbAnim' courant si l'on a rajouté des anims au décors
    fichier.read(&nbInfos, sizeof(nbInfos));
    // c) Liste des structures d'infos
    for (int num = 0; num < nbInfos ; num++)
    {	fichier.read(&AnimInfos[num].numTileDecorsAssoc, sizeof(AnimInfos[num].numTileDecorsAssoc));
		fichier.read(&AnimInfos[num].dX, sizeof(AnimInfos[num].dX));
		fichier.read(&AnimInfos[num].dY, sizeof(AnimInfos[num].dY));
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
bool TAnimTileDecors::LoadFromFileAnimClassik(TFileVirtual &fichier)
{	// Nb d'anims classiques sauvegardées
	int nbClassikAnim;
	// On vérifie si le fichier a été complété
	if (fichier.eof()) return false;
    
	fichier.read(&nbClassikAnim, sizeof(nbClassikAnim));
	// Puis lit tous les ClassikAnimInfos et regarde si l'on en a besoin
    SClassikAnimInfos infos;
    for (int i=0; i < nbClassikAnim ; i++)
    {	fichier.read(&infos.iAnimID, sizeof(infos.iAnimID));
		fichier.read(&infos.dX, sizeof(infos.dX));
		fichier.read(&infos.dY, sizeof(infos.dY));
		fichier.read(&infos.iMainAnimID, sizeof(infos.iMainAnimID));
		fichier.read(&infos.reserved, sizeof(infos.reserved));
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

//=== Renvoie l'index de l'anim n°iAnmID =====================================//
int TAnimTileDecors::GetClassikAnimIndex(int iAnmID)
{  for (int i=0 ; i < peNbClassikAnim ; i++)
   {	if (ClassikAnimInfos[i].iAnimID == iAnmID)
   		{	return i;
        }
   }
	return -1;		// Anim pas trouvée
}
//----------------------------------------------------------------------------//


///////////////////////// FIN CLASSE TANIMTILEDECORS////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Fonctions ANNEXES                                                          //
////////////////////////////////////////////////////////////////////////////////

// Renvoie le 1er TAnimTileDecors trouvé dans 'ListeObjetsDecors' associé au
// n° de TileDecors.DecorsNum (sûrement provisoire)
TAnimTileDecors* getAnimTileDecors(int numTileDecors)
{ for (int i=0 ; i < NbMaxTileDecors ; i++)
  { if ((ListeObjetsDecors[i]!=NULL) && (ListeObjetsDecors[i]->NoDecorsNumAssoc==numTileDecors)) 
    { return ListeObjetsDecors[i];
    } 
  }
  return NULL;  // Renoie un pointeur NULL si on en a pas trouvé
}

//=== Chgt de mode vidéo en plein cours du jeu ===============================//
void  ChangeAnimTileDecorsVideoMode()
{	for (int i=0 ; i < NbMaxTileDecors ; i++)
	{	if (ListeObjetsDecors[i]->FrameSuperpList != NULL)
		{	ListeObjetsDecors[i]->FrameSuperpList->ChangeSpriteFrameVideoMode(&DS->pDD);
		}
	}
}
//----------------------------------------------------------------------------//

