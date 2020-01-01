//---------------------------------------------------------------------------
//	 Editeur de carte pour mon Projet avec Jojo & Arpege
//
//===========================================================================
#include <vcl\vcl.h>
#pragma hdrstop
#define INITGUID        // Pour pouvoir toucher aux IID_IDirectDrawSurface4

#include "fNewMap.h"		// fiche de création d'une nouvelle carte
#include "MapLib.h"
#include "uMapView.h"
#include "fEditLibDecors.h"
#include "fEditLibLevel.h"
#include "consts.h"
#include "fMapOutils.h"
#include "fLibOutils.h"
#include "fObjOutils.h"
#include "fPrefer.h"
#include "fPathFinding.h"       // Fiche de la barre d'outils de recherche de chemins
#include "IDMap.h"              // Gère les Identificateurs des noms  présent dans le jeu
#include "Animall.h"
#include "IDMap.h"				// Librairie de gestion des ID
#include "ScnScript.h"			// Gestion des scripts scénarisés
#include "MapZone.h"			// Gère les zones au sol
#include "GameDebug.h"
#include "fLibOutils.h"			// Barre d'outils
#include "Editor.h"				// Header
                        
//----------------------------------------------------------------------------//
#pragma resource "*.dfm"
// Fiche principale ou se déroulera toute la création des niveaux & gestion des tiles

TEditeur *Editeur;
char MapExt [] = ".niv";

int ConfigType = ConfigNormal;						// type de config visuelles
TColor ConfigBoxColor  = ColorBoxNormal,			// couleur des boîtes de textes
	   ConfigBoxColor2 = ColorBoxNormal2;

AnsiString CheminEditor;




//-- Constructeur ------------------------------------------------------------//
__fastcall TEditeur::TEditeur(TComponent* Owner)
	: TForm(Owner)
{    levelLoaded = false;     // Aucun niveau n'est charge au demarrage
}
//----------------------------------------------------------------------------//

// Création d'une nouvelle carte =============================================//
void __fastcall TEditeur::NouvelleCarte1Click(TObject *Sender)
{
    // fait apparaître la boite de dialogue
    NewCarte->ShowModal();
    if (!NewCarte->isMapCreated()) return;
    
    PathFinding->InitPF();   // Initialise l'algo de recherche de chemin pour cette nouvelle carte

    // MaJ de l'interface graphique
    SauverCarte1->Enabled = true;   // Permet de sauver cette nouvelle carte
    levelLoaded = true;             // Niveau dispo
    // Permet de selectionner une librairie de tiles graphiques
    LibOutils->BitBtnOuvrirLib->Enabled = true;
}
//----------------------------------------------------------------------------//

//== Souris déplacée sur la fiche maître =====================================//
void __fastcall TEditeur::FormMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y)
{
   // Regarde si le pointer sort de la fenêtre de représentation du jeu
   if  ( (MapView->ScrollHor!=0) || (MapView->ScrollVert!=0))
     {  MapView->ScrollHor = MapView->ScrollVert = 0; };

}
//----------------------------------------------------------------------------//

//== Edition de la librairie des Décors ======================================//
void __fastcall TEditeur::Editerlibrairiedesdcors1Click(TObject *Sender)
{
    EditLibDecors->ShowModal();		// fait apparaître la fiche
}
//----------------------------------------------------------------------------//

//== Sauvegarde la Carte =====================================================//
void __fastcall TEditeur::SauverCarte1Click(TObject *Sender)
{
    // Test si une carte est tout d'abord chargee

    // Sauvgarde de la carte
    FILE *f;
    AnsiString fNom = MapNiveau->Name;  fNom+=MapExt;
    fNom = CheminEditor + cPathLevels + fNom;
    if ((f = fopen(fNom.c_str(),"wb"))==NULL)
     { MessageBoxA(NULL,"Impossible de sauver le fichier :",fNom.c_str(),0); return; };

    // Version d'un niveau
	fwrite(&VersLiL, sizeof(float), 1, f);

    // Sauvegarde des paramètres de la carte
    fwrite(MapNiveau->Name, sizeof(MapNiveau->Name), 1, f);
    fwrite(&MapNiveau->MapNum, sizeof(int), 1, f);
    fwrite(&MapNiveau->NivWidth, sizeof(int), 1, f);
    fwrite(&MapNiveau->NivHeight, sizeof(int), 1, f);
    fwrite(MapNiveau->DTLibName, sizeof(MapNiveau->DTLibName), 1, f);
    fwrite(&MapNiveau->NbEtage, sizeof(int), 1, f);		// Nb d'étage du niveau


    // Sauvegarde les objets de la Librairie de décors
    // de l'ETAGE PRINCIPAL
    fwrite(&DOLib->NbTileObj, sizeof(int), 1, f);			// nb d'objet
    DecorsObjet	*ptr = DOLib->DecorsObjList;
    while (ptr!=NULL)
      {
		  fwrite(&ptr->DecTileObj->DecorsNum, sizeof(int), 1, f);  // Sauvegarde le n° de l'image correspondante
          fwrite(&ptr->PosU, sizeof(int), 1, f);            // position de l'objet
          fwrite(&ptr->PosV, sizeof(int), 1, f);
          fwrite(&ptr->ObjEtage, sizeof(int), 1, f);		// Etage auquel il appartient
          fwrite(&ptr->ObjLayer, sizeof(int), 1, f);		// Layer    "    "    "

          //--- Sauvegarde les informations concernant le GameObject ---
          // Rmq : ajouté depuis la version 2.1 le 13/09/99
          ptr->SaveGOInfosToFile(f);             

          // Sauvegarde toutes les autres infos relatives à l'objet-décors
          // ...

          ptr = ptr->Suiv;
      };

    // Enregistre la liste des Zones
	MapNiveau->pMapZoneList->SaveZonesToFile(*f);

    fclose(f);
    MessageBoxA(NULL,fNom.c_str(),"Sauvegarde effectuée",0);
}
//----------------------------------------------------------------------------//

//== Chargement d'une carte ==================================================//
void __fastcall TEditeur::OuvrirCarte1Click(TObject *Sender)
{
    OpenMap->InitialDir = CheminEditor + cPathLevels;
    if (!OpenMap->Execute()) return;

    AnsiString fNom = OpenMap->FileName;
    if (fNom!="")  // On peut charger le fichier
     {
        FILE *f;
        if ((f = fopen(fNom.c_str(),"rb"))==NULL)
    		{ MessageBoxA(NULL,"Impossible de charger la carte :",fNom.c_str(),0); return; };

	    // Version d'un niveau
        float Version;
		fread(&Version, sizeof(float), 1, f);
        if (Version > VersLiL) { MessageBoxA(NULL,fNom.c_str(),"Version trop ancienne de l'Editeur !",0); return; };

     //===================== CREATION DE LA CARTE ==============================
        if (MapNiveau!=NULL) delete MapNiveau;
        MapNiveau = new TMapNiveau;

        // Infos sur la carte
        fread(MapNiveau->Name, sizeof(MapNiveau->Name), 1, f);
	fread(&MapNiveau->MapNum, sizeof(int), 1, f);
    	fread(&MapNiveau->NivWidth, sizeof(int), 1, f);
    	fread(&MapNiveau->NivHeight, sizeof(int), 1, f);
    	fread(MapNiveau->DTLibName, sizeof(MapNiveau->DTLibName), 1, f);
        fread(&MapNiveau->NbEtage, sizeof(int), 1, f);		// Nb d'étage du niveau

        // Prepare UNIQUEMENT l'étage PRINCIPAL
	MapNiveau->PrepareLevel(MapNiveau->NivWidth, MapNiveau->NivHeight);
        MapNiveau->SetInfos(MapNiveau->MapNum, MapNiveau->Name );

 	// Calcule certaines infos de la MapView
    	ViewMap->CalcInfos(MapNiveau->NivWidth, MapNiveau->NivHeight);
    	ViewMap->PosCoinUV();
    	// Positionne les Scrolls Bars correctement
    	MapView->ScrollViewHor->SetParams(0, -ViewMap->MapPixWidth/2-ScreenViewX/2, ViewMap->MapPixWidth/2-ScreenViewX/2);
    	MapView->ScrollViewVert->SetParams(0, -ScreenViewY/2, ViewMap->MapPixHeight-ScreenViewY/2);
    	ViewMap->ScreenX = - ScreenViewX/2;
    	ViewMap->ScreenY = 0;
	// Donne les Map-infos à la barre d'Outils
        MapOutils->MapName->Text   = MapNiveau->Name;
        MapOutils->MapNumber->Text = MapNiveau->MapNum;
        MapOutils->MapWidth->Text  = MapNiveau->NivWidth;
        MapOutils->MapHeight->Text = MapNiveau->NivHeight;

     //===================== CHARGEMENT DE LA LIB ==============================
        if (DULib!=NULL) delete DULib;
        DULib = new TLevelTileLib;
        AnsiString NameLib;
        NameLib =  MapNiveau->DTLibName;
        NameLib += ".LiL";
        DULib->LoadFile(CheminEditor + cPathLevels + NameLib);
        LibOutils->EditLibName->Text = DULib->Nom;
      //  EditLibNbSprite->Text = DULib->NbTotalTiles;
        LibOutils->EditLibNbSprite->Text = DULib->NbTilesPerType[DULib->MiniLibSelect];
        strcpy(MapNiveau->DTLibName, DULib->Nom.c_str());
        // Puis alloue les surfaces DDraw et charge les bitmaps dedans
        DULib->AlloueDDSurface();
        // Et sélectionne le 1er des Tiles <=> dernier de la liste
        TDSelect = DULib->TileMiniLibList[DULib->MiniLibSelect];
        if (TDSelect!=NULL) while (TDSelect->Suiv!=NULL) TDSelect = TDSelect->Suiv;
        NumDecorsSelect = TDSelect->NumDsList;
        LibOutils->NameElement->Caption = TDSelect->Nom;
        LibOutils->NumElement->Caption = TDSelect->NumDsList;

        // Dans la fiche "fObjOutils" remplit les trucs
        // 1er Tile sélectionné = 1er Objet de la liste
     //   DOSelect->DecorsLibName = NameLib;
        DOSelect->DecTileObj = TDSelect;
        ObjOutils->UpgradeFicheObj();
        // Nouvelle liste d'objet
        if (DOLib!=NULL) delete DOLib;
        DOLib = new DecorsObjetLib;

       //===================== CHARGEMENT DES DECORS ===========================
     	int NbObjLib;
        fread(&NbObjLib, sizeof(int), 1, f);			// nb d'objet
        DecorsObjet	*ptr;
        for (int n=0; n < NbObjLib ; n++)
         {
              ptr = new DecorsObjet;
              int NumObjDecors;
              fread(&NumObjDecors, sizeof(int), 1, f);		   // n° de l'objet
              fread(&ptr->PosU, sizeof(int), 1, f);            // position de l'objet
              fread(&ptr->PosV, sizeof(int), 1, f);
              fread(&ptr->ObjEtage, sizeof(int), 1, f);		   // Etage auquel il appartient
              fread(&ptr->ObjLayer, sizeof(int), 1, f);		   // Layer    "    "    "

			  //--- Charge les informations concernant le GameObject ---
			  // Rmq : rajouté depuis la version 2.1 le 13/09/99
              if (Version >= 2.1-0.0001)
              {	ptr->LoadGOInfosFromFile(f, Version);
              }

           // charge toutes les autres infos relatives à l'objet-décors
           // ...
//           	  AnsiString debug = NumObjDecors;
//              MessageBoxA(NULL,"DEBUGGAGE",debug.c_str(),0);
           // On cherche sur quelle Image-décors il pointe
           // A partir du n° de l'objet, on parcourt la liste d'objet

             int i=0;
             TileDecors* yo = DULib->TileMiniLibList[i];
             ptr->DecTileObj = NULL;
             while ((ptr->DecTileObj==NULL) && (i < cNbTypeTiles))
                {
                    if (yo!=NULL)
                    {
						 if (yo->DecorsNum == NumObjDecors)
						{	ptr->DecTileObj = yo;
						} else
						{   yo = yo->Suiv;
						}
                    }
                    if (yo==NULL)  { i++; if (i<cNbTypeTiles) yo = DULib->TileMiniLibList[i]; }
                };
			   // Si objet trouvé, (sinon ... ou a-t-il disparu ?)
               if ((i < cNbTypeTiles) && (ptr->DecTileObj!=NULL) && (ptr->DecTileObj->DecorsNum==NumObjDecors))
                {
                // On place l'objet-décors dans la liste
                   DOLib->AddDecObj(ptr);
                // on remplit la Map des indications de l'objet suivant l'Etage & le Lyer auquel il appartient
                   TEtage *Etage;		// On détermine l'étage
                   switch (ptr->ObjEtage)
                    {
                       case (Etage_PRINCIPAL) : Etage = MapNiveau->PrincEtage; break;
                       default : Application->Terminate();
                    };
                    TMap *pMap;			// puis le Layer
                   switch (ptr->ObjLayer)
                    {
                     case (Layer_DECORS): pMap = Etage->LayerDecors; break;
                     case (Layer_SOLS)  : pMap = Etage->LayerSols;	break;
                     default : Application->Terminate();
                    };

                  // Trapèze Principal
                  int PosU = ptr->PosU;
                  int PosV = ptr->PosV;
                  pMap->Carte[PosU][PosV].resetInfos();
                  pMap->Carte[PosU][PosV].setPrincipal(true);
                  pMap->Carte[PosU][PosV].ptrObjet = ptr;
                  pMap->Carte[PosU][PosV].setAccessible(ptr->DecTileObj->TrapezTab[0].Accessible);
                  pMap->Carte[PosU][PosV].TrapCollide = ptr->DecTileObj->TrapezTab[0].TrapCollide;

                  // Trapèze Secondaire
                  for (int i=1;  i < ptr->DecTileObj->NbTrapeze;  i++)
                    { int dU = ptr->DecTileObj->TrapezTab[i].TrapU;
                      int dV = ptr->DecTileObj->TrapezTab[i].TrapV;
                      if ((PosU + dU>=0) && (PosU + dU<MapNiveau->NivWidth) && (PosV + dV>=0) && (PosV + dV<MapNiveau->NivHeight))
                     // if ((dU>=0) && (dU<MapNiveau->NivWidth) && (dV>=0) && (dV<MapNiveau->NivHeight))
                      {
                       if (pMap->Carte[PosU + dU][PosV + dV].isPrincipal() == FALSE)
                       {
                         pMap->Carte[PosU + dU][PosV + dV].resetInfos();
                         pMap->Carte[PosU + dU][PosV + dV].setPrincipal(false);
                         pMap->Carte[PosU + dU][PosV + dV].ptrObjet = ptr;                         
                         pMap->Carte[PosU + dU][PosV + dV].setAccessible(ptr->DecTileObj->TrapezTab[i].Accessible);
                         pMap->Carte[PosU + dU][PosV + dV].TrapCollide = ptr->DecTileObj->TrapezTab[i].TrapCollide;
                       }
                      }
                    };
                }
                else // pas trouvé
                {
             	  delete ptr;
                }

           //*** Nouveau depuis le 11/05/1999 ***
           // => Vérifie si ce n'est pas un objet animé ayant un .ina associé
           TAnimTileDecors* ptrAnimTileDecors;
           ptrAnimTileDecors = getAnimTileDecors(ptr->DecTileObj->DecorsNum);
           if (ptrAnimTileDecors!=NULL)  // Il a une anim associée
           { ptr->SetAnimTileDecors(ptrAnimTileDecors);
             ptr->DecorsAnime = true;
             // Paramètres spéciaux pour objets animés à initialiser
             ptr->peTileDecorsAssoc = ptr->Element->AnimInfos[ptr->peNumAnim].numTileDecorsAssoc;
             ptr->SecondTile = DULib->getTileDecors(ptr->Element->AnimSpriteLib[ptr->peNumAnim]->FirstSprite);
           }
          };

        // On peut charger les diverses zones du niveau
		if (((float) Version) >= 2.5f)
		{	MapNiveau->pMapZoneList->LoadZonesFromFile(*f);
		}

     	fclose(f);
     };

     PathFinding->InitPF();   // Initialise l'algo de recherche de chemin pour cette nouvelle carte

     // MaJ de l'interface graphique
     SauverCarte1->Enabled = true;   // Permet de sauver la carte
     levelLoaded = true;             // Niveau dispo
}
//----------------------------------------------------------------------------//

//== Agrandie ou raptissie la fenêtre de visualisation =======================//
void __fastcall TEditeur::VueCarteClick(TObject *Sender)
{
    MapView->Show();
    MapView->ChangeViewScreen();
}
//----------------------------------------------------------------------------//

//== Ouverture de la fiche "Préférences"  ====================================//
void __fastcall TEditeur::Preferenc1Click(TObject *Sender)
{
    Prefer->ShowModal();
}
//----------------------------------------------------------------------------//

//== Quitte le programme =====================================================//
void __fastcall TEditeur::Quitter1Click(TObject *Sender)
{
    Close();
}
//----------------------------------------------------------------------------//

//== Bascule sur la fiche de Lib-Level =======================================//
void __fastcall TEditeur::LibduLevel1Click(TObject *Sender)
{
    LevelLibEditor->ShowModal();
}
//----------------------------------------------------------------------------//
void __fastcall TEditeur::FormShow(TObject *Sender)
{
    // Chemin duqel est lancé l'application
    CheminEditor = Application->ExeName;
    CheminEditor.Delete(CheminEditor.Pos("MAPEDITOR.EXE"),strlen("MAPEDITOR.EXE"));
    CheminEditor.Delete(CheminEditor.Pos("MapEditor.exe"),strlen("MapEditor.exe"));
}
//----------------------------------------------------------------------------//

//=== Ouverture d'une barre d'outils de PathFinding ==========================//
void __fastcall TEditeur::MenuPathFindingClick(TObject*Sender)
{   PathFinding->Top = 87;
    PathFinding->Left = 0;
    PathFinding->Show();
}
//----------------------------------------------------------------------------//

//=== Constructeur ===========================================================//
//  - charge tous les n°ID dont on a besoin
//  - les .bmp du jeu : décors
void __fastcall TEditeur::FormCreate(TObject *Sender)
{   //--- n°ID ---
    // A inclure dans le jeu (leture des .ina)
    // 1.a] patrons
    IDPatronPersos = new std::IDMap;
    IDPatronPersos->LireFileID(CheminEditor+cPersosPatronFilename);
    // 1.b]  membres du corps humain :
    if (IDMembres!=NULL) { delete (IDMembres); IDMembres=NULL; };
    IDMembres = new std::IDMap;
    IDMembres->LireFileID(CheminEditor+cIDPersosMembresFilename);
    // 1.c] frames bmp du jeu :
    if (IDBmpFrame!=NULL) { delete (IDBmpFrame); IDBmpFrame=NULL; };
    IDBmpFrame = new std::IDMap;
    IDBmpFrame->LireFileID(CheminEditor+cIDBmpFrameFilename);
    // 1.d] types d'objets (ex: epee1) servant dans la superposition d'anims
    if (IDTypeObjSprite!=NULL) { delete (IDTypeObjSprite); IDTypeObjSprite=NULL; };
    IDTypeObjSprite = new std::IDMap;
    IDTypeObjSprite->LireFileID(CheminEditor+cIDTypeObjSpriteFilename);
    // 1.e] Charge les ID d'animation
    IDAnimation = new std::IDMap;
    IDAnimation->LireFileID(CheminEditor+cIDAnimationFilename);
    // 1.f] Charge les ID des objets animés
    IDObjets = new std::IDMap;
    IDObjets->LireFileID(CheminEditor+cIDObjetsFilename);
    // 1.g] Identificateurs des différents TILES
    if (IDTiles!=NULL) { delete (IDTiles); IDTiles=NULL; };
    IDTiles = new std::IDMap;
    IDTiles->LireFileID(CheminEditor+cIDTilesFilename);
    // 1.h] Identificateurs des Mémoires des GameObjects
	if (IDMemory!=NULL) { delete (IDMemory); IDMemory=NULL; };
    IDMemory = new std::IDMap;
    IDMemory->LireFileID(CheminApplication+cIDMemoryFilename);
    // 1.i] IDentificateurs des Zones du Jeu
	if (IDMapZones!=NULL) { delete (IDMapZones); IDMapZones=NULL; };
    IDMapZones = new std::IDMap;
    IDMapZones->LireFileID(CheminApplication+cIDMapZonesFilename);

    // Chargement de la liste de scripts du jeu
   	InitGame_GameScriptList();

     // Mise en route le Debugger maison
	ZeGameDebug = new GameDebug("debug.log");

    // MaJ de l'interface graphique
    SauverCarte1->Enabled = false;   // Impossibilite de sauver le niveau

}
//----------------------------------------------------------------------------//

void __fastcall TEditeur::FormClose(TObject *Sender, TCloseAction &Action)
{
    // Fermeture des scripts du jeu
	CloseGame_GameScriptList();

    delete ZeGameDebug; ZeGameDebug = NULL;
        
	//--- Désallocation des bibliothèques d'ID ---
	// a] Identificateurs des différents TILES
    delete IDTiles;	IDTiles = NULL;
	// b] ID des objets animés
    delete IDObjets; IDObjets = NULL;
	// c] ID des animations
	delete IDAnimation; IDAnimation = NULL;
	// d] ID des patrons de personnages
	delete IDPatronPersos; IDPatronPersos = NULL;
	// e] ID des types d'objets
	delete IDTypeObjSprite; IDTypeObjSprite = NULL;
	// f] ID des frames bmp du jeu
	delete IDBmpFrame; IDBmpFrame = NULL;
	// g]  ID des membres du corps humain
	delete IDMembres; IDMembres = NULL;
}
//----------------------------------------------------------------------------//

