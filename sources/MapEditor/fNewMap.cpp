////////////////////////////////////////////////////////////////////////////////
// Fiche de création d'une nouvelle carte									  //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fNewMap.h"
#include "MapLib.h"
#include "uMapView.h"
#include "consts.h"
#include "fMapOutils.h"
#include "editor.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TNewCarte *NewCarte;
//---------------------------------------------------------------------------
__fastcall TNewCarte::TNewCarte(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------

//-- Nouvelle Carte de crée --------------------------------------------------//
void __fastcall TNewCarte::NewMapBtnClick(TObject *Sender)
{
	// Test si tout est bien remplie
	if (MapName->Text=="")  { MessageBoxA(NULL,"Nom de la carte non définie","Création impossible",0); return; };
    if (MapNumber->Text=="")  { MessageBoxA(NULL,"Numéro de la carte non définie","Création impossible",0); return; };
    if ((MapWidth->Text=="") || (MapHeight->Text==""))
       { MessageBoxA(NULL,"Dimensions non définies","Création impossible",0); return; };

    // Alloue la place
    // *** Nouveau : MapNiveau ***
    if (MapNiveau!=NULL) delete MapNiveau;
    MapNiveau = new TMapNiveau;
    MapNiveau->SetInfos(MapNumber->Text.ToInt(), MapName->Text.c_str() );
    MapNiveau->PrepareLevel(MapWidth->Text.ToInt(), MapHeight->Text.ToInt() );

    // Calcule certaines infos de la MapView
    ViewMap->CalcInfos(MapNiveau->NivWidth, MapNiveau->NivHeight);
    ViewMap->PosCoinUV();

    // Positionne les Scrolls Bars correctement
    MapView->ScrollViewHor->SetParams(0, -ViewMap->MapPixWidth/2-ScreenViewX/2, ViewMap->MapPixWidth/2-ScreenViewX/2);
    MapView->ScrollViewVert->SetParams(0, -ScreenViewY/2, ViewMap->MapPixHeight-ScreenViewY/2);
    ViewMap->ScreenX = - ScreenViewX/2;
    ViewMap->ScreenY = 0;

    // Donne les Map-infos à la barre d'Outils
    MapOutils->MapName->Text   = MapName->Text;
    MapOutils->MapNumber->Text = MapNumber->Text;
    MapOutils->MapWidth->Text  = MapWidth->Text;
    MapOutils->MapHeight->Text = MapHeight->Text;

    bMapCreated = true;
    Close();	// on ferme le tout
};
//---------------------------------------------------------------------------

//== Ajuste les paramètres de la fiche : couleurs, polices ... ===============//
void __fastcall TNewCarte::FormShow(TObject *Sender)
{
	MapName->Color   = ConfigBoxColor2;
    MapNumber->Color = ConfigBoxColor2;
    MapWidth->Color  = ConfigBoxColor;
    MapHeight->Color = ConfigBoxColor;

    bMapCreated = false;
}
//---------------------------------------------------------------------------
