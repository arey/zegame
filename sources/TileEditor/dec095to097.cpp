////////////////////////////////////////////////////////////////////////////////
// Convertion des fichiers *.dec de la version 0.80 � la version 0.90         //
////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "dec097to098.h"
#include "ImDecors.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TForm1 *Form1;

float OldVersion = 0.97,
      NewVersion = 0.98;


//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
    NbDecLabel->Caption = DecListBox->Items->Count;
}
//---------------------------------------------------------------------------

//== Convertion automatique des fichiers *.dec du r�pertoire courant =========//
void __fastcall TForm1::Convertion()
{
   if (DecListBox->Items->Count==0)
    { MessageBoxA(NULL,"Aucun fichier *.dec dans ce r�pertoire ...","Aucune convertion possible",0);
      return;
    }

   // Parcours de tous les fichiers du r�pertoire sp�cifi�
   for (int NumFichier = 0; NumFichier <  DecListBox->Items->Count; NumFichier++)
   {
      NumLabel->Caption = NumFichier+1;

      AnsiString FileName = DecListBox->Items->Strings[NumFichier];
      AnsiString BackupName = FileName;
      BackupName.Delete(BackupName.Pos(".dec"),strlen(".dec"));
      BackupName+=".$$$";
      rename(FileName.c_str(),BackupName.c_str());
      // Charge toute la structure TileDecors
      FILE *f;
      FILE *fnew;
      if ((f = fopen(BackupName.c_str(),"rb"))==NULL)
         MessageBoxA(NULL,"Impossible de lire le fichier :",BackupName.c_str(),1);
      if ((fnew = fopen(FileName.c_str(),"wb"))==NULL)
         MessageBoxA(NULL,"Impossible de cr�er le fichier :",FileName.c_str(),1);

      // Lecture du fichier � convertir
      TileDecors *DecTile2;
      DecTile2 = new TileDecors;
      float DecVersion;
      fread(&DecVersion, sizeof(float), 1, f);
      if (DecVersion != OldVersion) { MessageBoxA(NULL,"Version non valide pour la conversion","Convertion impossible",1); fclose(f);  }
      else
      {
      fread(&DecTile2->DecorsNum, sizeof(int), 1, f);
      fread(&DecTile2->Nom, sizeof(DecTile2->Nom), 1, f);
      fread(&DecTile2->Width, sizeof(int), 1, f);
      fread(&DecTile2->Height, sizeof(int), 1, f);
      fread(&DecTile2->RVB, sizeof(unsigned int), 1, f);
      fread(&DecTile2->NumRVB, sizeof(unsigned char), 1, f);
      fread(&DecTile2->NbTrapeze, sizeof(int), 1, f);
      fread(&DecTile2->TrapPrincX, sizeof(int), 1, f);
      fread(&DecTile2->TrapPrincY, sizeof(int), 1, f);

      // Ecriture du fichier sans changement
      fwrite(&NewVersion, sizeof(float), 1, fnew);        //*** Nouvelle Version ***
      fwrite(&DecTile2->DecorsNum, sizeof(int), 1, fnew);
      fwrite(&DecTile2->Nom, sizeof(DecTile2->Nom), 1, fnew);
      fwrite(&DecTile2->Width, sizeof(int), 1, fnew);
      fwrite(&DecTile2->Height, sizeof(int), 1, fnew);
      fwrite(&DecTile2->RVB, sizeof(unsigned int), 1, fnew);
      fwrite(&DecTile2->NumRVB, sizeof(unsigned char), 1, fnew);
      fwrite(&DecTile2->NbTrapeze, sizeof(int), 1, fnew);
      fwrite(&DecTile2->TrapPrincX, sizeof(int), 1, fnew);
      fwrite(&DecTile2->TrapPrincY, sizeof(int), 1, fnew);

      // Lecture du fichier � convertir : STRUCTURE TRAPEZE
      DecTile2->TrapezTab = new Trapeze[DecTile2->NbTrapeze];
      OldTrapeze TrapOld;
      for (int i=0; i < DecTile2->NbTrapeze; i++)
        { // Lecture de l'ancien format de fichier
          fread(&TrapOld,sizeof(OldTrapeze), 1, f);

          // Convertion de l'ancien en nouveau format
          DecTile2->TrapezTab[i].TrapU = TrapOld.TrapU;
          DecTile2->TrapezTab[i].TrapV = TrapOld.TrapV;
          DecTile2->TrapezTab[i].Hauteur = TrapOld.Hauteur;
          DecTile2->TrapezTab[i].Terrain = TrapOld.Terrain;
          DecTile2->TrapezTab[i].Accessible = TrapOld.Accessible;

          //*** Changements � partir de la version 0.95 ***
          DecTile2->TrapezTab[i].TrapCollide = PARTIELLE;        // par d�faut

          //*** Changements � partir de la version 0.97 ***
          DecTile2->TrapezTab[i].DrawTrapezekey = false;         // par d�faut
          DecTile2->TrapezTab[i].lamX1 = 0;
          DecTile2->TrapezTab[i].lamY1 = 0;
          DecTile2->TrapezTab[i].lamX2 = 0;
          DecTile2->TrapezTab[i].lamY2 = 0;
        }
      // Sauvegarde de la nouvelle STRUCTURE TRAPEZE
      for (int i=0; i < DecTile2->NbTrapeze; i++)
         fwrite(&DecTile2->TrapezTab[i], sizeof(Trapeze), 1, fnew);

      // Lecture du fichier de fichier � convertir
      fread(&DecTile2->DecorsCornerX, sizeof(int), 1, f);
      fread(&DecTile2->DecorsCornerY, sizeof(int), 1, f);

      //*** Changements � partir de la version 0.97 ***
      DecTile2->TotalDraw = true;   // par d�faut

      // Sauvegarde de ces 2 attributs
      fwrite(&DecTile2->DecorsCornerX, sizeof(int), 1, fnew);
      fwrite(&DecTile2->DecorsCornerY, sizeof(int), 1, fnew);
      fwrite(&DecTile2->TotalDraw, sizeof(bool), 1, fnew);

      fclose(f);   // efface le fichier
      fclose(fnew); // fin d'�criture du fichier

      // lib�re the mem
      delete DecTile2->TrapezTab;
      delete DecTile2;
      }
   };

   MessageBoxA(NULL,"Convertion r�ussie","YES !",0);
};

void __fastcall TForm1::UpgradeBtnClick(TObject *Sender)
{
   Convertion();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DirectoryListBoxChange(TObject *Sender)
{
     NbDecLabel->Caption = DecListBox->Items->Count;
}
//---------------------------------------------------------------------------
