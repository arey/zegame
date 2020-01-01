//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fLibOutils.h"
#include "consts.h"
#include "ImDecors2.h"
#include "fObjOutils.h"
#include "uMapView.h"
#include "MapLib.h"
#include "fLibOutils.h"
#include "Editor.h"
#include "fMapOutils.h"
#include "fDesignOutils.h"
#include "fDesignOutils.h"
#include "fPersosOutils.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TLibOutils *LibOutils;

int NumDecorsSelect;	// n� ds la lib du d�cors s�lectionn�


//---------------------------------------------------------------------------
//== Constructeur de la fiche ================================================//
__fastcall TLibOutils::TLibOutils(TComponent* Owner)
	: TForm(Owner)
{   // Place la fiche
    Top = 0; Left = 664;
    Width = 350; Height = 211;
    TypeLibSelect = Lib_DECORS;          // par d�faut
    DecorsShow = FondShow = TRUE;		 //    "
    PersosShow = EventsShow = FALSE;     //    "
    DTLib = NULL;
    DULib = NULL;

    // S'occupe de remplir les onglets
    TabControlSolTile->Tabs->Clear();
    TabControlDecorsTile->Tabs->Clear();    
    for (int i=cIndexSols ; i < cIndexSols+cNbTilesSols ; i++)
       TabControlSolTile->Tabs->Add(cTypeTileName[i]);
    for (int i=cIndexDecors ; i < cIndexDecors+cNbTilesDecors ; i++)
       TabControlDecorsTile->Tabs->Add(cTypeTileName[i]);
}
//---------------------------------------------------------------------------

//== Ouvre la librairie s�lectionn�es ========================================//
void __fastcall TLibOutils::BitBtnOuvrirLibClick(TObject *Sender)
{   // On ne peut choisir une librairie de tiles que lorsqu'un niveau a ete cree
    if (MapNiveau==NULL) { MessageBoxA(NULL,"Cr�� d'abord une carte","Impossible de charger la Lib",0); return; };
    // Une fois que l'on a choisi une librairie pour un niveau, on ne peut plus changer
    if (DULib!=NULL)
    {	MessageBoxA(NULL, "Vous avez deja selectionne une librairie de tils pour ce niveau", "Chargement de la Lib impossible", 0);
        BitBtnOuvrirLib->Enabled = false; return;
    };

	OpenDialogLib->InitialDir = CheminEditor + cPathLevels;
	OpenDialogLib->FilterIndex = TypeLibSelect;
	OpenDialogLib->Execute();
    AnsiString NameLib = OpenDialogLib->FileName;
    if (NameLib!="")  // chargement
     {
        switch (TypeLibSelect)
          {
          	 case (Lib_DECORS):
          /*    {
                if (DTLib!=NULL) delete DTLib;
                DTLib = new DecorsTileLib;
                DTLib->LoadFile(NameLib);
                EditLibName->Text = DTLib->Nom;
                EditLibNbSprite->Text = DTLib->NbTile;
                strcpy(Map->DTLibName, DTLib->Nom.c_str());
                // Puis alloue les surfaces DDraw et charge les bitmaps dedans
                DTLib->AlloueDDSurface();
                // Et s�lectionne le 1er des Tiles <=> dernier de la liste
                TDSelect = DTLib->DecorsTileList;
                if (TDSelect!=NULL) while (TDSelect->Suiv!=NULL) TDSelect = TDSelect->Suiv;
                NumDecorsSelect = TDSelect->NumDsList;
                NameElement->Caption = TDSelect->Nom;
                NumElement->Caption = TDSelect->NumDsList;

                // Dans la fiche "fObjOutils" remplit les trucs
                // 1er Tile s�lectionn� = 1er Objet de la liste
                DOSelect->DecorsLibName = NameLib;
                DOSelect->DecTileObj = TDSelect;
                ObjOutils->UpgradeFicheObj();
                // Nouvelle liste d'objet
                if (DOLib!=NULL) delete DOLib;
                DOLib = new DecorsObjetLib;
              }; break;*/
            case (Lib_SOLS):
             {
                if (DULib!=NULL) delete DULib;
                DULib = new TLevelTileLib;

                DULib->LoadFile(NameLib);
                EditLibName->Text = DULib->Nom;
              //  EditLibNbSprite->Text = DULib->NbTotalTiles;
               	EditLibNbSprite->Text = DULib->NbTilesPerType[DULib->MiniLibSelect];
         	    strcpy(MapNiveau->DTLibName, DULib->Nom.c_str());
                // Puis alloue les surfaces DDraw et charge les bitmaps dedans
        		DULib->AlloueDDSurface();
                // Et s�lectionne le 1er des Tiles <=> dernier de la liste
                TDSelect = DULib->TileMiniLibList[DULib->MiniLibSelect];
                if (TDSelect!=NULL) while (TDSelect->Suiv!=NULL) TDSelect = TDSelect->Suiv;
                if (TDSelect!=NULL)
                  {
                	NumDecorsSelect = TDSelect->NumDsList;
                	NameElement->Caption = TDSelect->Nom;
                	NumElement->Caption = TDSelect->NumDsList;

                // Dans la fiche "fObjOutils" remplit les trucs
                // 1er Tile s�lectionn� = 1er Objet de la liste
             //   DOSelect->DecorsLibName = NameLib;
                	DOSelect->DecTileObj = TDSelect;
                  }  
                ObjOutils->UpgradeFicheObj();
                // Nouvelle liste d'objet
                if (DOLib!=NULL) delete DOLib;
                DOLib = new DecorsObjetLib;


             }; break;
          };
     };
}
//---------------------------------------------------------------------------

//== Lorsqu'un des Turbos-Boutons est cliqu� =================================//
void __fastcall TLibOutils::SpeedButtonDecorsClick(TObject *Sender)
{
	// On �tablit lequel est enfonc�
   if (SpeedButtonDecors->Down) TypeLibSelect = Lib_DECORS;
   else if (SpeedButtonFonds->Down)  TypeLibSelect = Lib_SOLS;
   else if (SpeedButtonPersos->Down) TypeLibSelect = Lib_PERSOS;
   else if (SpeedButtonEvents->Down) TypeLibSelect = Lib_EVENTS;
   else if (SpeedButtonDesign->Down) TypeLibSelect = Lib_DESIGN;

	// Par d�faut, on affiche ces contr�les
    PanelElements->Show();
    LabelTypePlan->Show();
    EditLibName->Show();
    EditLibNbSprite->Show();
    BitBtnOuvrirLib->Show();
    LabelNomLib->Show();
    LabelNbSprites->Show();
    GBDesign->Hide();

   // Affiche la fiche correspondante et ferme les autres
   switch (TypeLibSelect)
    {
   	 case  (Lib_DECORS) :
       	  ObjOutils->WindowState = wsNormal;
          LabelTypePlan->Caption = "D�cors";
          TabControlSolTile->Hide();
          GBPersos->Hide();
          TabControlDecorsTile->Show();
      	  if (DULib!=NULL) TabControlDecorsTileChange(Sender);
          MapOutils->PeindreBitBtn->Enabled = FALSE;
          MapOutils->GommeBitBtn->Enabled = FALSE;
          FormDesignOutils->WindowState = wsMinimized;
          FormPersosOutils->WindowState = wsMinimized;
        break;

     case  (Lib_SOLS)	:
          ObjOutils->WindowState = wsNormal;
          LabelTypePlan->Caption = "Sols";
          TabControlSolTile->Show();
          GBPersos->Hide();
          TabControlDecorsTile->Hide();
      	  if (DULib!=NULL) TabControlSolTileChange(Sender);
          MapOutils->PeindreBitBtn->Enabled = TRUE;   // possibilit� de peindre
      //    MapOutils->GommeBitBtn->Enabled = TRUE;	  // BUG !!
          FormDesignOutils->WindowState = wsMinimized;
          GBPersos->Hide();
          FormPersosOutils->WindowState = wsMinimized;
        break;

     case (Lib_DESIGN) :
       	  ObjOutils->WindowState = wsMinimized;
          LabelTypePlan->Caption = "Design";
          // On cache vraiment tout ce qu'on peut
          TabControlSolTile->Hide();
          TabControlDecorsTile->Hide();
          GBPersos->Hide();
          MapOutils->PeindreBitBtn->Enabled = FALSE;
          MapOutils->GommeBitBtn->Enabled = FALSE;
          PanelElements->Hide();
          LabelTypePlan->Hide();
          EditLibName->Hide();
          EditLibNbSprite->Hide();
          BitBtnOuvrirLib->Hide();
          LabelNomLib->Hide();
          LabelNbSprites->Hide();
          FormPersosOutils->WindowState = wsMinimized;
          // Montre qd m�me ce qui est sp�cifique au Level Design
          GBDesign->Show();
          FormDesignOutils->WindowState = wsNormal;
          FormDesignOutils->UpdateView();
     	break;

     // Edition des personnages
     case (Lib_PERSOS) :
          LabelTypePlan->Caption = "Characters";
          // On cache vraiment tout ce qu'on peut qui ne concerne pas la
          // manipulation de personnages
       	  ObjOutils->WindowState = wsMinimized;
          FormDesignOutils->WindowState = wsMinimized;
          TabControlSolTile->Hide();
          TabControlDecorsTile->Hide();
          MapOutils->PeindreBitBtn->Enabled = FALSE;
          MapOutils->GommeBitBtn->Enabled = FALSE;
          PanelElements->Hide();
          LabelTypePlan->Hide();
          EditLibName->Hide();
          EditLibNbSprite->Hide();
          BitBtnOuvrirLib->Hide();
          LabelNomLib->Hide();
          LabelNbSprites->Hide();
          // Montre � pr�sent ce qui est sp�cifique aux Persos Design
          GBPersos->Show();
          FormPersosOutils->WindowState = wsNormal;
          // FormPersosOutils->UpdateView();
        break;

     default :   // cache toutes les fiches
         ObjOutils->WindowState = wsMinimized;
         FormPersosOutils->WindowState = wsMinimized;
         FormDesignOutils->WindowState = wsMinimized;
         break;
    };
}
//---------------------------------------------------------------------------

//== Avance ds la liste des �l�ments ==========================================//
void __fastcall TLibOutils::BitBtnReculClick(TObject *Sender)
{
  switch (TypeLibSelect)
   {
   	 case (Lib_DECORS):
     case (Lib_SOLS)  :
     if (TDSelect!=NULL)
      {
		if (TDSelect->Suiv!=NULL) TDSelect = TDSelect->Suiv;
        NameElement->Caption = TDSelect->Nom;
        NumElement->Caption = TDSelect->NumDsList;
   	  // Nouvel Objet s�lectionn�
     //   DOSelect->DecorsLibName = DULib->Nom;
        DOSelect->DecTileObj = TDSelect;
        ObjOutils->UpgradeFicheObj();
      }; break;
   };
}
//---------------------------------------------------------------------------

//== Recul ds la liste des �l�ments ==========================================//
void __fastcall TLibOutils::BitBtnAvanceClick(TObject *Sender)
{
  switch (TypeLibSelect)
   {
   	 case (Lib_DECORS):
     case (Lib_SOLS)  :
     if (TDSelect!=NULL)
      {
        if (TDSelect->Prec!=NULL) TDSelect = TDSelect->Prec;
        NameElement->Caption = TDSelect->Nom;
        NumElement->Caption = TDSelect->NumDsList;
   	  // Nouvel Objet s�lectionn�
      //  DOSelect->DecorsLibName = DULib->Nom;
        DOSelect->DecTileObj = TDSelect;
        ObjOutils->UpgradeFicheObj();
      }; break;
   };

}
//---------------------------------------------------------------------------

//== Regarde qq trucs lorsque la souris traine dessus   ======================//
void __fastcall TLibOutils::BitBtnAvanceMouseMove(TObject *Sender,
	TShiftState Shift, int X, int Y)
{  // Regarde si le pointer sort de la fen�tre de repr�sentation du jeu
   if  ( (MapView->ScrollHor!=0) || (MapView->ScrollVert!=0))
     {  MapView->ScrollHor = MapView->ScrollVert = 0; };
}
//---------------------------------------------------------------------------
//== Lorsque la souris passe dessus, d�sactive qq trucs ======================//
void __fastcall TLibOutils::FormMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y)
{   // Regarde si le pointer sort de la fen�tre de repr�sentation du jeu
   if  ( (MapView->ScrollHor!=0) || (MapView->ScrollVert!=0))
     {  MapView->ScrollHor = MapView->ScrollVert = 0; };
}
//---------------------------------------------------------------------------

//== Ajuste les param�tres de la fiche : couleurs, polices ... ===============//
void __fastcall TLibOutils::FormShow(TObject *Sender)
{
	PanelElements->Color   = ConfigBoxColor;
}
//---------------------------------------------------------------------------

//== Active/D�sactive vue D�cors =============================================//
void __fastcall TLibOutils::SpdBtnDecorsVisClick(TObject *Sender)
{ DecorsShow = !DecorsShow; }
//== Active/D�sactive vue Fond  ==============================================//
void __fastcall TLibOutils::SpdBtnFondsVisClick(TObject *Sender)
{ FondShow = !FondShow; }
//== Active/D�sactive vue Persos  ============================================//
void __fastcall TLibOutils::SpdBtnPersosVisClick(TObject *Sender)
{ PersosShow =!PersosShow; }
//== Active/D�sactive vue Events  ============================================//
void __fastcall TLibOutils::SpdBtnEventsVisClick(TObject *Sender)
{ EventsShow =!EventsShow;}
//----------------------------------------------------------------------------//

//== Changement de Mini-Lib du d�cors ========================================//
void __fastcall TLibOutils::TabControlDecorsTileChange(TObject *Sender)
{   // On s'assure tout d'abord qu'un niveau est bien charge
    if (!Editeur->levelLoaded) return;

    switch (TypeLibSelect)
     {
       case (Lib_SOLS):
       case (Lib_DECORS):
        {
          DULib->MiniLibSelect = TabControlDecorsTile->TabIndex + cIndexDecors;

          TDSelect = DULib->TileMiniLibList[DULib->MiniLibSelect];
          if (TDSelect!=NULL) while (TDSelect->Suiv!=NULL) TDSelect = TDSelect->Suiv;
          if (TDSelect!=NULL)
           {
             NumDecorsSelect = TDSelect->NumDsList;
             NameElement->Caption = TDSelect->Nom;
             NumElement->Caption = TDSelect->NumDsList;
             EditLibNbSprite->Text = DULib->NbTilesPerType[DULib->MiniLibSelect];
           // Nouvel Objet s�lectionn�
          //   DOSelect->DecorsLibName = DULib->Nom;
             DOSelect->DecTileObj = TDSelect;
             ObjOutils->UpgradeFicheObj();
            }
           else  // Pas de fichiers *.dec ds cet onglet
            {
              EditLibNbSprite->Text = "";
              NameElement->Caption = "";
              NumElement->Caption = "";
            }
        }; break;
     };
}
//----------------------------------------------------------------------------//

//== Changement de Mini-Lib du Sol ===========================================//
void __fastcall TLibOutils::TabControlSolTileChange(TObject *Sender)
{   if (!Editeur->levelLoaded) return;

    switch (TypeLibSelect)
     {
       case (Lib_SOLS):
       case (Lib_DECORS):
        {
          DULib->MiniLibSelect = TabControlSolTile->TabIndex + cIndexSols;

          TDSelect = DULib->TileMiniLibList[DULib->MiniLibSelect];
          if (TDSelect!=NULL) while (TDSelect->Suiv!=NULL) TDSelect = TDSelect->Suiv;
          NumDecorsSelect = TDSelect->NumDsList;
          NameElement->Caption = TDSelect->Nom;
          NumElement->Caption = TDSelect->NumDsList;
          EditLibNbSprite->Text = DULib->NbTilesPerType[DULib->MiniLibSelect];
        // Nouvel Objet s�lectionn�
        //  DOSelect->DecorsLibName = DULib->Nom;
          DOSelect->DecTileObj = TDSelect;
          ObjOutils->UpgradeFicheObj();
        }; break;
    };
}
//----------------------------------------------------------------------------//
