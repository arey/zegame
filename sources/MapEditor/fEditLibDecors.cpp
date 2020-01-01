//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fEditLibDecors.h"
#include "ImDecors2.h"
#include "editor.h"
#include "consts.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TEditLibDecors *EditLibDecors;

TileDecors    *TileDecSelect;			// Decors s�lectionn� ds la liste des fichiers
DecorsTileLib *DTLibEdit;				// librairie des d�cors

AnsiString Repertory;
AnsiString RepertoryLiD;

//---------------------------------------------------------------------------
__fastcall TEditLibDecors::TEditLibDecors(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------

//== Pr�pare la fiche qd celle-ci est ouverte ================================//
void __fastcall TEditLibDecors::FormShow(TObject *Sender)
{
    // Alloue the m�moire
    TileDecSelect = new TileDecors;
    NbTileLabel->Caption = 0;
    TileLibName->Text = "";
    TileList->Clear();
    // Pr�pare la librairie
    DTLibEdit = new DecorsTileLib;

    //Ajuste les param�tres de la fiche : couleurs, polices ...
    TileLibName->Color = ConfigBoxColor2;

    // Sauvegarde le chemin en cours
    Repertory = FileListDecors->Directory;
    RepertoryLiD = OpenFile->InitialDir;
}
//---------------------------------------------------------------------------

//== Changement de fichiers s�lectionn�s =====================================//
void __fastcall TEditLibDecors::FileListDecorsChange(TObject *Sender)
{
    if (FileListDecors->ItemIndex!=-1)		// si un fichier est s�lectionn�
     {
         // On charge les valeurs du fichier dans TileDecSelect
         AnsiString nom = FileListDecors->FileName;
         // MAPEDITOR LoadTileDecors(TileDecSelect,nom.c_str(),1);
         LoadTileDecors(TileDecSelect,nom.c_str(), true);
         // Montre l'image associ�e
         nom = TileDecSelect->Nom;
         nom+=".bmp";
         TileImage->Picture->LoadFromFile(nom);
         ImageX->Caption = TileImage->Picture->Width;		// fait beau
         ImageY->Caption = TileImage->Picture->Height;
	 };

}
//---------------------------------------------------------------------------

//== Ajoute un nouveau d�cors � la Lib =======================================//
void __fastcall TEditLibDecors::TileAddClick(TObject *Sender)
{
   if (FileListDecors->ItemIndex!=-1)		// si un fichier est s�lectionn�
    {
      // v�rifie si le Tile n'existe d�j� pas ds la liste
      if (DTLibEdit->ExisteTile(TileDecSelect->Nom))
       {  MessageBoxA(NULL,"Ce Tile existe d�j� dans la liste ","Warning !",0); return; };


       DTLibEdit->Ajouter(TileDecSelect);
       TileList->Items->Add(TileDecSelect->Nom);  // Ajout de ce Til � la TlistBox
       TileDecSelect = new TileDecors;			  // m�moire occup�e => on doit donc en rer�server
	   NbTileLabel->Caption = DTLibEdit->NbTile;
    };


}
//---------------------------------------------------------------------------

//== Sauve la librairie ======================================================//
void __fastcall TEditLibDecors::SauverLib1Click(TObject *Sender)
{
    OpenFile->InitialDir = CheminEditor + RepertoryLiD;
	if (TileLibName->Text!="")
     {
		DTLibEdit->Nom = TileLibName->Text;
    	DTLibEdit->SaveToFile();
        MessageBoxA(NULL,DTLibEdit->Nom.c_str(),"Fichier sauvegard�",0);
     }
    else MessageBoxA(NULL,"Donner un nom � votre librairie","Sauvegarde impossible",0);
}
//---------------------------------------------------------------------------

//== Charge une Librairie ====================================================//
void __fastcall TEditLibDecors::ChargerLib1Click(TObject *Sender)
{
    OpenFile->InitialDir = CheminEditor + RepertoryLiD;
    OpenFile->Execute();
    AnsiString nom = OpenFile->FileName;

    if (DTLibEdit!=NULL) delete DTLibEdit;
    DTLibEdit = new DecorsTileLib;
    DTLibEdit->LoadFile(nom);
    // Rentre dans le bon r�pertoire
    FILE *f;
    if ((f = fopen(nom.c_str(),"rb"))==NULL)
      {	MessageBoxA(NULL,"Impossible de charger le fichier :",nom.c_str(),1);
        return; };

    char n[15];
    fread(n, 15, 1, f);
    fclose(f);
    AnsiString LiDName = n;
	FileListDecors->Directory = Repertory;
	FileListDecors->Directory += "\\";
	FileListDecors->Directory += cPathTiles;
    AnsiString Repertoire;
    for (int j=0; j < cNbTypeTiles; j++)
      {
        for (int i=0; i < cNbTypeUnivers; i++)
         { AnsiString strSearch = cUniversName[i];
           strSearch += cTypeTileName[j];
           if (LiDName == strSearch)
             {  if ((j>= cIndexSols) && (j < cIndexSols+cNbTilesSols))
                  { Repertoire = "Sols\\"; }
                else if ((j>= cIndexDecors) && (j < cIndexDecors+cNbTilesDecors))
                  { Repertoire = "Decors\\"; }
                Repertoire += cTypeTileName[j];
            //    Repertoire += "\\";

             }
         }
      }
    FileListDecors->Directory += Repertoire;
    FileListDecors->Update();

	// Ajoute les �l�ments � la liste
	NbTileLabel->Caption = DTLibEdit->NbTile;
    TileList->Clear();
    TileLibName->Text = DTLibEdit->Nom;

   	TileDecors *ptr;
    ptr = DTLibEdit->DecorsTileList;
    if (ptr!=NULL) while (ptr->Suiv!=NULL) ptr = ptr->Suiv;
    while (ptr!=NULL)
      {
         TileList->Items->Add(ptr->Nom);
      	 ptr = ptr->Prec;   // redescend
      };
}
//---------------------------------------------------------------------------

//== Nouvelle Librairie ======================================================//
void __fastcall TEditLibDecors::NouvelleLib1Click(TObject *Sender)
{
	if (DTLibEdit!=NULL) { delete DTLibEdit; DTLibEdit = new DecorsTileLib; };
    TileList->Clear();
    TileLibName->Text="";
    NbTileLabel->Caption = 0;
}
//---------------------------------------------------------------------------

//== Element de la Liste s�lectionn� =========================================//
void __fastcall TEditLibDecors::TileListClick(TObject *Sender)
{
    int num = TileList->ItemIndex+1;  // la tListbox commence � 0
    BOOL Trouve = FALSE;
    TileDecors *ptr;
    ptr = DTLibEdit->DecorsTileList;
    while ((ptr!=NULL) && (!Trouve))
      {
         if (ptr->NumDsList == num) Trouve = TRUE;
         else ptr = ptr->Suiv;
      };
     if (Trouve)
      {
      	AnsiString BmpName;
        BmpName+= ptr->TilePath;
        BmpName+= ptr->Nom;
        BmpName+=".bmp";
    	TileImage->Picture->LoadFromFile(BmpName.c_str());
        ImageX->Caption = TileImage->Picture->Width;		// fait beau
        ImageY->Caption = TileImage->Picture->Height;

      };
}
//---------------------------------------------------------------------------

//== Supprime un �l�ment de la liste s�lectionn� =============================//
void __fastcall TEditLibDecors::TileSuprClick(TObject *Sender)
{
    // Recherche l'�l�ment � supprimer
    if (TileList->ItemIndex != -1)  // si y'en a 1 de s�lectionn�
     {
      	// recherche l'�l�ment � supprimer
       int num = TileList->ItemIndex+1;  // la tListbox commence � 0
       TileDecors *ptr ;
       ptr = DTLibEdit->DecorsTileList;
       if (ptr->NumDsList == num)  // si c'est le 1er Tile
         {
            TileList->Items->Delete(num-1);
            DTLibEdit->DecorsTileList = DTLibEdit->DecorsTileList->Suiv;
            if (DTLibEdit->DecorsTileList!=NULL) DTLibEdit->DecorsTileList->Prec = NULL;
            delete ptr;
         }
       else  // sinon il est ds la chaine
         {  while (ptr->NumDsList!=num) ptr = ptr->Suiv;
            TileList->Items->Delete(num-1);
            TileDecors* ptr2 = ptr->Prec;
            if (ptr2 != NULL) ptr2->Suiv = ptr->Suiv;
            if (ptr->Suiv!=NULL) ptr->Suiv->Prec = ptr2;
            delete ptr;
         };
       // Puis rafraichir les n� de tous les Tiles ainsi que le nb de Tile de la Lib
       int NbTile2 = --DTLibEdit->NbTile;
       NbTileLabel->Caption = DTLibEdit->NbTile;
       ptr = DTLibEdit->DecorsTileList;
       while (ptr != NULL)
        {  ptr->NumDsList = NbTile2 --;
           ptr = ptr->Suiv;
        };
     }
     else MessageBoxA(NULL,"Aucun �l�ment s�lectionn�","Warning !",0); return;
}
//---------------------------------------------------------------------------

//== Referme la fiche ========================================================//
void __fastcall TEditLibDecors::Quitter1Click(TObject *Sender)
{
	Close();	
}
//---------------------------------------------------------------------------


