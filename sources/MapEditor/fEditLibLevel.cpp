//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fEditLibLevel.h"
#include "ImDecors2.h"
#include "editor.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TLevelLibEditor *LevelLibEditor;

//== mes données ====
TLevelTileLib	   *LevelTL;		// Librairie du level
TUniversalTileLib  *UniversalTL;	// Librairie Complète de tous les Tiles du jeu

int NumTypeTile;			// n° du type de tile sélectionné
int NumTypeUnivers;			// n° du type de librairie sélectionné


//---------------------------------------------------------------------------
__fastcall TLevelLibEditor::TLevelLibEditor(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

//== Cherche le tile sélectionné ds la liste de l'univers ====================//
TileDecors* SearchTileFromUniversList()
{
    int num = LevelLibEditor->UniversList->ItemIndex;
    if (num!=-1)
     {
       num = LevelLibEditor->UniversList->Items->Count - num -1;
       TileDecors* ptr;
       ptr = UniversalTL->UnivLib[NumTypeUnivers][NumTypeTile].DecorsTileList;
       for (int i=0; i<num ; i++) ptr = ptr->Suiv;
       return ptr;
     }
    else return NULL;
};
//----------------------------------------------------------------------------//

//== Cherche le tile sélectionné ds la liste du level	  ====================//
TileDecors* SearchTileFromLevelList()
{
    int num = LevelLibEditor->TypeTileList->ItemIndex;
    if (num!=-1)
     {
       num = LevelLibEditor->TypeTileList->Items->Count - num -1;
       TileDecors* ptr;
       ptr = LevelTL->TileMiniLibList[NumTypeTile];
       for (int i=0; i<num ; i++) ptr = ptr->Suiv;
       return ptr;
     }
    else return NULL;
};
//----------------------------------------------------------------------------//

//== Change le contenue de la liste-Tiles ====================================//
void __fastcall TLevelLibEditor::ChangeListNivTile(int NewNumTypeTile)
{
    TileDecors *ptr;
    ptr = LevelTL->TileMiniLibList[NewNumTypeTile];
    TypeTileList->Clear();
    if (ptr!=NULL)
     { while (ptr->Suiv!=NULL) ptr = ptr->Suiv;
       while (ptr!=NULL)
        {
           TypeTileList->Items->Add(ptr->Nom);
           ptr = ptr->Prec;   // redescend
        };
     };
};
//----------------------------------------------------------------------------//

//== Change le contenue de la liste-univers ==================================//
void __fastcall TLevelLibEditor::ChangeListUnivers(int NewNumTypeUnivers, int NewNumTypeTile)
{
    TileDecors *ptr;
    ptr = UniversalTL->UnivLib[NewNumTypeUnivers][NewNumTypeTile].DecorsTileList;
    NbTileLabel->Caption = UniversalTL->UnivLib[NewNumTypeUnivers][NewNumTypeTile].NbTile;
    UniversList->Clear();
    if (ptr!=NULL)
     { while (ptr->Suiv!=NULL) ptr = ptr->Suiv;
       while (ptr!=NULL)
        {
           UniversList->Items->Add(ptr->Nom);
           ptr = ptr->Prec;   // redescend
        };
     };
};
//----------------------------------------------------------------------------//

//== Charge tout à l'apparition de la fiche ==================================//
void __fastcall TLevelLibEditor::FormShow(TObject *Sender)
{
    LevelTL 	= new TLevelTileLib;
    UniversalTL = new TUniversalTileLib;

    // Charge les éléments de la lib universelle
    UniversalTL->AlloueLib();

    // Alloue les onglets de l'"univers"
    TabControlUnivers->Tabs->Clear();
    for (int i=0; i < cNbTypeUnivers; i++)
      TabControlUnivers->Tabs->Add(cUniversName[i]);
    TabControlUnivers->TabIndex = 0;       // Sélectionne le 1er onglet
    NumTypeUnivers = 0;
    MilieuName->Caption = cUniversName[NumTypeUnivers];

	// S'occupe des infos de l'onglet Decors/Sols Tile-Lib
    TabControlSolTile->Tabs->Clear();
    TabControlDecorsTile->Tabs->Clear();
    for (int i=cIndexSols ; i < cIndexSols+cNbTilesSols ; i++)
       TabControlSolTile->Tabs->Add(cTypeTileName[i]);
    for (int i=cIndexDecors ; i < cIndexDecors+cNbTilesDecors ; i++)
       TabControlDecorsTile->Tabs->Add(cTypeTileName[i]);
    TabControlSolTile->TabIndex = 0;       // Sélectionne le 1er onglet des Sols
    TabControlDecorsTile->TabIndex = -1;
    NumTypeTile = 0;

    // Charge les noms ds la liste d'aprés NumTypeUnivers & NumTypeTile
    ChangeListUnivers(NumTypeUnivers, NumTypeTile);
    ChangeListNivTile(NumTypeTile);

    // Affiche le premier tile de la liste de ce new univers
    if (UniversalTL->UnivLib[NumTypeUnivers][NumTypeTile].DecorsTileList!=NULL)
      {  UniversList->ItemIndex = 0;
         UniversListClick(Sender); };

    // Remplit qq captions ...
    NbTileLevelLabel->Caption = LevelTL->NbTilesPerType[NumTypeTile];
    LevelName->Text = "";
}
//---------------------------------------------------------------------------

//== Lorsqu'on sélectionne un Tile ===========================================//
void __fastcall TLevelLibEditor::UniversListClick(TObject *Sender)
{
	int num = UniversList->ItemIndex+1;  // la tListbox commence à 0
    BOOL Trouve = FALSE;
    TileDecors *ptr;
    ptr = UniversalTL->UnivLib[NumTypeUnivers][NumTypeTile].DecorsTileList;
    while ((ptr!=NULL) && (!Trouve))
      {
         if (ptr->NumDsList == num) Trouve = TRUE;
         else ptr = ptr->Suiv;
      };
     if (Trouve)
      {
      	AnsiString BmpName = cPathTiles;
        if ((NumTypeTile >= cIndexSols) && (NumTypeTile < cIndexSols+cNbTilesSols))
        		{ BmpName += "Sols\\"; }
        else if ((NumTypeTile >= cIndexDecors) && (NumTypeTile < cIndexDecors+cNbTilesDecors))
                { BmpName += "Decors\\"; }
        BmpName += cTypeTileName[NumTypeTile]; BmpName +="\\";
        BmpName += ptr->Nom;
        BmpName+=".bmp";
    	TileImage->Picture->LoadFromFile(CheminEditor + BmpName.c_str());
        ImageX->Caption = TileImage->Picture->Width;		// fait beau
        ImageY->Caption = TileImage->Picture->Height;
        TileImage->Stretch = ((TileImage->Picture->Width > TileImage->Width) || (TileImage->Picture->Height > TileImage->Height));
      };
}
//----------------------------------------------------------------------------//

//== Changement d'onglet des Tiles-Sols ======================================//
void __fastcall TLevelLibEditor::TabControlSolTileChange(TObject *Sender)
{
    NumTypeTile = cIndexSols + TabControlSolTile->TabIndex; // new index
    TabControlDecorsTile->TabIndex = -1;				   	// aucun décors sélectionné
    ChangeListUnivers(NumTypeUnivers, NumTypeTile);  		// affichage du tout
    ChangeListNivTile(NumTypeTile);
    // Affiche le premier tile de la liste de ce new univers
    if (UniversalTL->UnivLib[NumTypeUnivers][NumTypeTile].DecorsTileList!=NULL)
      {  UniversList->ItemIndex = 0;
         UniversListClick(Sender); };
    // Remplit qq captions ...
    NbTileLevelLabel->Caption = LevelTL->NbTilesPerType[NumTypeTile];
}
//----------------------------------------------------------------------------//
//== Changement d'onglet des Tiles-Décors ====================================//
void __fastcall TLevelLibEditor::TabControlDecorsTileChange(TObject *Sender)
{
    NumTypeTile = cIndexDecors + TabControlDecorsTile->TabIndex; // new index
    TabControlSolTile->TabIndex = -1;						// aucun décors sélectionné
    ChangeListUnivers(NumTypeUnivers, NumTypeTile);  		// affichage du tout
    ChangeListNivTile(NumTypeTile);
    // Affiche le premier tile de la liste de ce new univers
    if (UniversalTL->UnivLib[NumTypeUnivers][NumTypeTile].DecorsTileList!=NULL)
      {  UniversList->ItemIndex = 0;
         UniversListClick(Sender); };
    // Remplit qq captions ...
    NbTileLevelLabel->Caption = LevelTL->NbTilesPerType[NumTypeTile];
}
//----------------------------------------------------------------------------//
//==Changement d'onglet des Type-Univers =====================================//
void __fastcall TLevelLibEditor::TabControlUniversChange(TObject *Sender)
{
    NumTypeUnivers = TabControlUnivers->TabIndex;
	ChangeListUnivers(NumTypeUnivers, NumTypeTile);  		// affichage du tout
    MilieuName->Caption = cUniversName[NumTypeUnivers];
    // Affiche le premier tile de la liste de ce new univers
    if (UniversalTL->UnivLib[NumTypeUnivers][NumTypeTile].DecorsTileList!=NULL)
      {  UniversList->ItemIndex = 0;
         UniversListClick(Sender); };
}
//----------------------------------------------------------------------------//

//== Ajoute le Tile à la LevelList ===========================================//
void __fastcall TLevelLibEditor::UniversListDblClick(TObject *Sender)
{
   if (UniversList->ItemIndex!=-1)		// si un fichier est sélectionné
    {
      // vérifie si le Tile n'existe déjà pas ds la liste
      TileDecors *TileDecSelect, *TileDecNew ;
      TileDecSelect = SearchTileFromUniversList();
      if (LevelTL->ExisteTile(TileDecSelect->Nom))
      	{  MessageBoxA(NULL,"Ce Tile existe déjà dans la liste ","Warning !",0); return; };
      TileDecNew = new TileDecors;
      AnsiString TileName; // [MODIF 12/01/2000] = CheminEditor + cPathTiles;
      if ((NumTypeTile >= cIndexSols) && (NumTypeTile < cIndexSols+cNbTilesSols))
              { TileName += "Sols\\"; }
      else if ((NumTypeTile >= cIndexDecors) && (NumTypeTile < cIndexDecors+cNbTilesDecors))
              { TileName += "Decors\\"; }
      TileName  += cTypeTileName[NumTypeTile]; TileName +="\\";
      TileName +=TileDecSelect->Nom;
      TileName += ".Dec";
      // MAPEDITOR LoadTileDecors(TileDecNew, TileName.c_str(),1);
      LoadTileDecors(TileDecNew, TileName.c_str());
      LevelTL->Ajouter(TileDecNew, NumTypeTile);
      TypeTileList->Items->Add(TileDecSelect->Nom);  // Ajout de ce Til à la TlistBox
	   // NbTileLabel->Caption = DTLibEdit->NbTile;

    };
    // Remplit qq captions ...
    NbTileLevelLabel->Caption = LevelTL->NbTilesPerType[NumTypeTile];
}
//----------------------------------------------------------------------------//

//== Supprime un tile sélectionné de la LevelList ============================//
void __fastcall TLevelLibEditor::SupprTileBtnClick(TObject *Sender)
{
	// Recherche l'élément à supprimer
    if (TypeTileList->ItemIndex != -1)  // si y'en a 1 de sélectionné
     {
      // recherche l'élément à supprimer
       int num = TypeTileList->ItemIndex+1;  // la tListbox commence à 0
       TileDecors *ptr ;
       ptr = LevelTL->TileMiniLibList[NumTypeTile];
       if (ptr->NumDsList == num)  // si c'est le 1er Tile
         {
            TypeTileList->Items->Delete(num-1);
            LevelTL->TileMiniLibList[NumTypeTile] = LevelTL->TileMiniLibList[NumTypeTile]->Suiv;
            if (LevelTL->TileMiniLibList[NumTypeTile]!=NULL) LevelTL->TileMiniLibList[NumTypeTile]->Prec = NULL;
            delete ptr;
         }
       else  // sinon il est ds la chaine
         {  while (ptr->NumDsList!=num) ptr = ptr->Suiv;
            TypeTileList->Items->Delete(num-1);
            TileDecors* ptr2 = ptr->Prec;
            if (ptr2 != NULL) ptr2->Suiv = ptr->Suiv;
            if (ptr->Suiv!=NULL) ptr->Suiv->Prec = ptr2;
            delete ptr;
         };
       // Puis rafraichir les n° de tous les Tiles ainsi que le nb de Tile de la Lib
       int NbTile2 = --LevelTL->NbTilesPerType[NumTypeTile];
  //     NbTileLabel->Caption = DTLibEdit->NbTile;
       ptr = LevelTL->TileMiniLibList[NumTypeTile];
       while (ptr != NULL)
        {  ptr->NumDsList = NbTile2 --;
           ptr = ptr->Suiv;
        };
     }
     else { MessageBoxA(NULL,"Aucun élément sélectionné ds la liste du Level","Warning !",0); return; }

     // Remplit qq captions ...
     NbTileLevelLabel->Caption = LevelTL->NbTilesPerType[NumTypeTile];
}
//----------------------------------------------------------------------------//

//== Sélectionne un Tile du Level ============================================//
void __fastcall TLevelLibEditor::TypeTileListClick(TObject *Sender)
{
	int num = TypeTileList->ItemIndex+1;  // la tListbox commence à 0
    BOOL Trouve = FALSE;
    TileDecors *ptr;
    ptr = LevelTL->TileMiniLibList[NumTypeTile];
    while ((ptr!=NULL) && (!Trouve))
      {
         if (ptr->NumDsList == num) Trouve = TRUE;
         else ptr = ptr->Suiv;
      };
     if (Trouve)
      {
        AnsiString BmpName = CheminEditor + cPathTiles; 
        if ((NumTypeTile >= cIndexSols) && (NumTypeTile < cIndexSols+cNbTilesSols))
        		{ BmpName += "Sols\\"; }
        else if ((NumTypeTile >= cIndexDecors) && (NumTypeTile < cIndexDecors+cNbTilesDecors))
                { BmpName += "Decors\\"; }
        BmpName += cTypeTileName[NumTypeTile]; BmpName +="\\";
        BmpName += ptr->Nom;
        BmpName+=".bmp";
    	TileImage->Picture->LoadFromFile(BmpName.c_str());
        ImageX->Caption = TileImage->Picture->Width;		// fait beau
        ImageY->Caption = TileImage->Picture->Height;
        TileImage->Stretch = ((TileImage->Picture->Width > TileImage->Width) || (TileImage->Picture->Height > TileImage->Height));
      };
}
//----------------------------------------------------------------------------//

//== Ajoute ts les tiles de gauche au Level ==================================//
void __fastcall TLevelLibEditor::AjoutAllBtnClick(TObject *Sender)
{
   if (UniversList->Items->Count != 0)  // si y'en a 1 ds la lib
    {
      TileDecors *TileDecSelect, *TileDecNew ;
      TileDecSelect = UniversalTL->UnivLib[NumTypeUnivers][NumTypeTile].DecorsTileList;
      // Parcours toute la liste
      for (int i=0; i < UniversalTL->UnivLib[NumTypeUnivers][NumTypeTile].NbTile ; i++)
       {
       // vérifie si le Tile n'existe déjà pas ds la liste
         if (!LevelTL->ExisteTile(TileDecSelect->Nom))
          {
             TileDecNew = new TileDecors;
             AnsiString TileName; // [MODIF 12/01/2000] = CheminEditor + cPathTiles;
             if ((NumTypeTile >= cIndexSols) && (NumTypeTile < cIndexSols+cNbTilesSols))
                     { TileName += "Sols\\"; }
             else if ((NumTypeTile >= cIndexDecors) && (NumTypeTile < cIndexDecors+cNbTilesDecors))
                     { TileName += "Decors\\"; }
             TileName  += cTypeTileName[NumTypeTile]; TileName +="\\";
             TileName +=TileDecSelect->Nom;
             TileName += ".Dec";
             // MAPEDITOR LoadTileDecors(TileDecNew, TileName.c_str(),1);
             LoadTileDecors(TileDecNew, TileName.c_str());
             LevelTL->Ajouter(TileDecNew, NumTypeTile);
             TypeTileList->Items->Add(TileDecSelect->Nom);  // Ajout de ce Til à la TlistBox
           };
         TileDecSelect = TileDecSelect->Suiv;
       };
    };
   // Remplit qq captions ...
   NbTileLevelLabel->Caption = LevelTL->NbTilesPerType[NumTypeTile];
}
//---------------------------------------------------------------------------

//== Supprime toute une colonne de la Lib ====================================//
void __fastcall TLevelLibEditor::SupprOneColClick(TObject *Sender)
{
   LevelTL->EffaceOneLevelLib(NumTypeTile);
   TypeTileList->Clear();
   // Remplit qq captions ...
   NbTileLevelLabel->Caption = LevelTL->NbTilesPerType[NumTypeTile];
}
//----------------------------------------------------------------------------//

//== Sauvegarde la Librairie du Décors du niveau =============================//
void __fastcall TLevelLibEditor::SauverLib2Click(TObject *Sender)
{
	if (LevelName->Text!="")
      {
        LevelTL->Nom = LevelName->Text;
    	LevelTL->SaveToFile();
        MessageBoxA(NULL,"fichier correctement sauvé","Sauvegarde",0);
      }
    else { MessageBoxA(NULL,"Veuillez rentrer un nom de niveau","Warning !",0); return; };
}
//----------------------------------------------------------------------------//

//== Charge une Librairie de Décors du niveau *.LiL ==========================//
void __fastcall TLevelLibEditor::ChargeLib1Click(TObject *Sender)
{
    // Charge le fichier
    OpenFile->InitialDir = CheminEditor + cPathLevels;
    OpenFile->Execute();
    AnsiString LiLFile = OpenFile->FileName;
	if (LevelTL!=NULL) delete LevelTL;
    LevelTL = new TLevelTileLib;
//    LiLFile += cPathTiles;
    LevelTL->LoadFile(LiLFile);
    // Ajoute les éléments à la liste
	ChangeListNivTile(NumTypeTile);

    // Remplit qq captions ...
    NbTileLevelLabel->Caption = LevelTL->NbTilesPerType[NumTypeTile];
    LevelName->Text = LevelTL->Nom;
}
//----------------------------------------------------------------------------//

//== Nouvelle Librairie Toute Propre =========================================//
void __fastcall TLevelLibEditor::SauverLib1Click(TObject *Sender)
{
  // BUGGEE
   // MessageBoxA(NULL,"Pour creer une nouvelle Lib, utiliser la Lib Vierge et la sauver sous un autre nom !","Alternative",0);
  //  return;
    if (LevelTL!=NULL) delete LevelTL;
    LevelTL = new TLevelTileLib;
    // Ajoute les éléments à la liste
	ChangeListNivTile(NumTypeTile);
    LevelName->Text = "";

    MessageBoxA(NULL,"Penser à donner un nom à votre lib.","Attention ...",0);
}
//----------------------------------------------------------------------------//

//== Exit de la fiche ========================================================//
void __fastcall TLevelLibEditor::Quitter1Click(TObject *Sender)
{ Close(); }
//----------------------------------------------------------------------------//

//== Fermeture de la Fiche ===================================================//
void __fastcall TLevelLibEditor::FormClose(TObject *Sender,
	TCloseAction &Action)
{
   if (LevelTL!=NULL) delete LevelTL;
   if (UniversalTL!=NULL) delete UniversalTL;
}
//----------------------------------------------------------------------------//
