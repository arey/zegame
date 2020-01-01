//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include "ImDecors2.h"
#include "consts.h"
#include "MapLib.h"
#include "uMapView.h"
#include "Animall.h"
#pragma hdrstop
#include "editor.h"
#include "ScnScript.h"		// Librairie de gestion des scénaris
#include "fIDOutils.h"		// Fenêtre de sélection d'ID
#include "fObjOutils.h"		// Son Header

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TObjOutils *ObjOutils;
//---------------------------------------------------------------------------
__fastcall TObjOutils::TObjOutils(TComponent* Owner)
	: TForm(Owner)
{
    Top = 211; Left = 664;
    Width = 350; Height = 480;
    WindowState = wsNormal; 		// par défaut
    DOLib = new DecorsObjetLib;     // Crée la Lib
    DOSelect = new DecorsObjet;		// Crée l'objet sélectionné
}
//---------------------------------------------------------------------------

//== Sélectionne un trapèze ds l'image passé en param. =======================//
void SelectTrapeze(TImage *Imag, int x, int y, int Coul)
{
	POINT p; p.x = x; p.y = y;
    Imag->Canvas->PenPos = p;
    switch (Coul)
     {   case 1 : Imag->Canvas->Pen->Color = clBlue;
     			  Imag->Canvas->Pen->Width = 2;
                  Imag->Canvas->Pen->Mode = pmMergePenNot; break;
         case 2 : Imag->Canvas->Pen->Color = clYellow;
     			  Imag->Canvas->Pen->Width = 2;
                  Imag->Canvas->Pen->Mode = pmMergePenNot; break;
     };
    Imag->Canvas->LineTo(p.x-ISO_WIDTH_HALF, p.y+ISO_HEIGHT_HALF);
    p.x-=ISO_WIDTH_HALF;  p.y+=ISO_HEIGHT_HALF;
    Imag->Canvas->LineTo(p.x+ISO_WIDTH_HALF, p.y+ISO_HEIGHT_HALF);
    p.x+=ISO_WIDTH_HALF;  p.y+=ISO_HEIGHT_HALF;
    Imag->Canvas->LineTo(p.x+ISO_WIDTH_HALF, p.y-ISO_HEIGHT_HALF);
    p.x+=ISO_WIDTH_HALF;  p.y-=ISO_HEIGHT_HALF;
    Imag->Canvas->LineTo(p.x-ISO_WIDTH_HALF, p.y-ISO_HEIGHT_HALF);

    // retour à la normale
    Imag->Canvas->Pen->Mode = pmCopy;
    Imag->Canvas->Pen->Width = 1;
};
//----------------------------------------------------------------------------//

//== Upgrade l'affichage de la fiche lorsqu'un nouvel objet est sélectionné ==//
void __fastcall TObjOutils::UpgradeFicheObj()
{
    // Remplit les renseignements
    AnsiString Title =" Objet de Décors :  ";
    Title+=DOSelect->DecTileObj->Nom;
    Caption = Title;		// titre de la fiche
    Numero->Caption = DOSelect->DecTileObj->DecorsNum;
    DecorsHeight->Text = DOSelect->DecTileObj->Height;
    DecorsWidth->Text = DOSelect->DecTileObj->Width;
    NbTrapeze->Caption = DOSelect->DecTileObj->NbTrapeze;

    // Charge l'image dans sa représentation TImage
    AnsiString BitmapName = cPathTiles;
    BitmapName+=DOSelect->DecTileObj->TilePath;
    BitmapName+= DOSelect->DecTileObj->Nom;
    BitmapName+=".bmp";
	DecObjImage->Picture->LoadFromFile(CheminEditor + BitmapName);
    // Sélectionne les trapèzes
    int pX, pY, dX, dY;
    pX = DOSelect->DecTileObj->TrapPrincX;
    pY = DOSelect->DecTileObj->TrapPrincY;
    for (int i=DOSelect->DecTileObj->NbTrapeze -1 ; i>=0 ; i--)  // ordre décroissant
      {
        if (i==0) SelectTrapeze(DecObjImage, pX, pY, 1);		// Trap. principal
        else		// Trapèzes secondaires.
          { ViewMap->TransUVtoXY( DOSelect->DecTileObj->TrapezTab[i].TrapU,
           				 DOSelect->DecTileObj->TrapezTab[i].TrapV, &dX, &dY );
            SelectTrapeze(DecObjImage, pX + dX, pY + dY , 2);
          };
      };

    // Affiche l'image redimensionnée à l'écran
    DecObjShow->Picture = DecObjImage->Picture;

    // Remplit la ComboBox des Etats Courants
    CBEtatCourant->Clear();
    CBEtatCourant->Enabled = false;
    if (DOSelect->DecorsAnime)
    {  for (int i=0; i < DOSelect->Element->peNbAnim; i++)
       {   CBEtatCourant->Items->Add(IDAnimation->CleID(DOSelect->Element->AnimSpriteLib[i]->Numero));
       }
       CBEtatCourant->Enabled = true;
    }

    // Affiche les infos du GameObject dont il dérive
	if (DOSelect->GetObjectStyle() == GAMEOBJECT_FIXED)
    {   // Objet important ayant un n°ID fixé
    	CBGameObjectKnown->Checked = true;
    	int numIDRecup = DOSelect->GetID();
    	GameObjectID->Caption = numIDRecup;
    	GameObjectNameID->Caption = IDGameDecors->CleID(numIDRecup);
        numIDRecup = DOSelect->GetScriptID();
        ScriptID->Caption = numIDRecup;
        if (numIDRecup == 0)
    	{	ScriptNameID->Caption = "<none>";
        } else
        {	ScriptNameID->Caption = (GameScripts->GetScriptMap())->CleID(numIDRecup);
        }
    } else
    {	CBGameObjectKnown->Checked = false;
    	GameObjectID->Caption = "<none>";
    	GameObjectNameID->Caption = "<none>";
        ScriptID->Caption = 0;
        ScriptNameID->Caption = "<none>";
    }
}
//----------------------------------------------------------------------------//

//== Arrange quelques trucs lorsque la souris passes dessus ==================//
void __fastcall TObjOutils::FormMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y)
{
	// Regarde si le pointer sort de la fenêtre de représentation du jeu
   if  ( (MapView->ScrollHor!=0) || (MapView->ScrollVert!=0))
     {  MapView->ScrollHor = MapView->ScrollVert = 0; };
}
//---------------------------------------------------------------------------

//== Ajuste les paramètres de la fiche : couleurs, polices ... ===============//
void __fastcall TObjOutils::FormShow(TObject *Sender)
{
    Numero->Color    	= ConfigBoxColor;
    NbTrapeze->Color 	= ConfigBoxColor;
    DecorsHeight->Color = ConfigBoxColor;
    DecorsWidth->Color  = ConfigBoxColor;
}
//----------------------------------------------------------------------------//


//=== Met à jour les paramètres de l'objt sélectionné ========================//
void __fastcall TObjOutils::Update()
{  if (DOSelect!=NULL)
   {  // Etat de l'anim courante
      LabelEtat->Caption = IDAnimation->CleID(DOSelect->peEtat);
      // N° de l'animation
      LabelNumFrame->Caption = DOSelect->peNumSprite;
      // Met à Jour le CBEtatCourant si nécessaire
      if (DOSelect->peChangeAnim)
      { CBEtatCourant->ItemIndex = CBEtatCourant->Items->IndexOf(IDAnimation->CleID(DOSelect->peEtat));
      }                                               


   }
}
//----------------------------------------------------------------------------//

//=== Changement manuel de l'anim de l'objet =================================//
void __fastcall TObjOutils::CBEtatCourantChange(TObject *Sender)
{ if (DOSelect==NULL) return;              // On sait jamais
  if (!DOSelect->DecorsAnime) return;
  DOSelect->peOldEtat = DOSelect->peEtat;
  DOSelect->peEtat = IDAnimation->NumID(CBEtatCourant->Text);
  DOSelect->peFutEtat = DOSelect->peEtat;
  DOSelect->peNumAnim = DOSelect->getNumAnim();    // Récupère le n° de l'anim
  DOSelect->peNbSprite = DOSelect->Element->AnimSpriteLib[DOSelect->peNumAnim]->NbSprites;
  DOSelect->peNumSprite = 0;
  if (DOSelect->Element->AnimSpriteLib[DOSelect->peNumAnim]->Type == 0)
  {  DOSelect->peShowTile = true;
     DOSelect->peSprNumLib = 0;
     DOSelect->peOldTileDecorsAssoc = DOSelect->peTileDecorsAssoc;
     DOSelect->peTileDecorsAssoc = DOSelect->Element->AnimInfos[DOSelect->peNumAnim].numTileDecorsAssoc;
     if (DOSelect->peOldTileDecorsAssoc!=DOSelect->peTileDecorsAssoc)
     {  DOSelect->OldSecondTile = DOSelect->SecondTile;
        DOSelect->SecondTile = DULib->getTileDecors(DOSelect->Element->AnimSpriteLib[DOSelect->peNumAnim]->FirstSprite);
        DOSelect->peOldPosU = DOSelect->PosU;
        DOSelect->peOldPosV = DOSelect->PosV;
        DOSelect->PosU -= DOSelect->Element->AnimInfos[DOSelect->peNumAnim].dX;
        DOSelect->PosV -= DOSelect->Element->AnimInfos[DOSelect->peNumAnim].dY;
        DOLib->DecorsUpgraded.push_back(DOSelect);
     }
  }
  //DOSelect->peChangeAnim = true;
}
//----------------------------------------------------------------------------//

//=== Affichage de la boîte de saisie d'un n°ID de GameObject ================//
void __fastcall TObjOutils::BitBtnChooseGameObjectIDClick(TObject *Sender)
{	int numIDRecup;

	IDOutils->LetsGo(IDGameDecors);
    numIDRecup = IDOutils->GetIDSelect();
    if (numIDRecup != -1)
    {	GameObjectID->Caption = numIDRecup;
    	GameObjectNameID->Caption = IDGameDecors->CleID(numIDRecup);
        CBGameObjectKnown->Checked = true;

        // MaJ de GameObject
        DOSelect->SetFixedID(numIDRecup);
	}
}
//----------------------------------------------------------------------------//

//=== 
void __fastcall TObjOutils::CBGameObjectKnownClick(TObject *Sender)
{	if (!CBGameObjectKnown->Checked)
	{	GameObjectID->Caption = "<none>";
    	GameObjectNameID->Caption = "<none>";
    }
    else
	{	GameObjectID->Caption = "";
    	GameObjectNameID->Caption = "";
    }
}
//----------------------------------------------------------------------------//


//=== Affichage de la boîte de saisie d'un n°ID de Script ====================//
void __fastcall TObjOutils::BitBtnChooseScriptIDClick(TObject *Sender)
{	int numIDRecup;

	IDOutils->LetsGo(GameScripts->GetScriptMap());
    numIDRecup = IDOutils->GetIDSelect();
    if (numIDRecup != -1)
    {	ScriptID->Caption = numIDRecup;
    	ScriptNameID->Caption = (GameScripts->GetScriptMap())->CleID(numIDRecup);
        // MaJ de GameObject
        DOSelect->SetScriptID(numIDRecup);
	}
}
//----------------------------------------------------------------------------//

