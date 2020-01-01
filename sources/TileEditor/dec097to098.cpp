////////////////////////////////////////////////////////////////////////////////
// Convertion des fichiers *.dec de la version 0.80 à la version 0.90         //
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

//== Convertion automatique des fichiers *.dec du répertoire courant =========//
void __fastcall TForm1::Convertion()
{
   if (DecListBox->Items->Count==0)
    { MessageBoxA(NULL,"Aucun fichier *.dec dans ce répertoire ...","Aucune convertion possible",0);
      return;
    }

   // Parcours de tous les fichiers du répertoire spécifié
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
         MessageBoxA(NULL,"Impossible de créer le fichier :",FileName.c_str(),1);

      // Lecture du fichier à convertir
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

      // Lecture du fichier à convertir : STRUCTURE TRAPEZE
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

          //*** Changements à partir de la version 0.95 ***
          DecTile2->TrapezTab[i].TrapCollide = TrapOld.TrapCollide;

          //*** Changements à partir de la version 0.97 ***
          DecTile2->TrapezTab[i].DrawTrapezekey = TrapOld.DrawTrapezekey;
          DecTile2->TrapezTab[i].lamX1 = TrapOld.lamX1;
          DecTile2->TrapezTab[i].lamY1 = TrapOld.lamY1;
          DecTile2->TrapezTab[i].lamX2 = TrapOld.lamX2;
          DecTile2->TrapezTab[i].lamY2 = TrapOld.lamY2;

          //*** Changements à partir de la version 0.98 ***
          for (int i=0 ; i < 8 ; i++) DecTile2->reserved[i] = 0;    // par défaut
        }
      // Sauvegarde de la nouvelle STRUCTURE TRAPEZE
      for (int i=0; i < DecTile2->NbTrapeze; i++)
         fwrite(&DecTile2->TrapezTab[i], sizeof(Trapeze), 1, fnew);

      // Lecture du fichier de fichier à convertir
      fread(&DecTile2->DecorsCornerX, sizeof(int), 1, f);
      fread(&DecTile2->DecorsCornerY, sizeof(int), 1, f);
      fread(&DecTile2->TotalDraw, sizeof(bool), 1, f);

      //*** Changements à partir de la version 0.97 ***
      DecTile2->TotalDraw = true;   // par défaut
      // Sauvegarde les 3 anciens attributs
      fwrite(&DecTile2->DecorsCornerX, sizeof(int), 1, fnew);
      fwrite(&DecTile2->DecorsCornerY, sizeof(int), 1, fnew);
      fwrite(&DecTile2->TotalDraw, sizeof(bool), 1, fnew);

      // Sauvegarde les 5 nouveaux attributs  depuis la version 0.98
      DecTile2->CircleCollision = false;
      DecTile2->CircleCenterX = DecTile2->CircleCenterY = -1;
      DecTile2->CircleRadius = 0;
      DecTile2->DrawMethode = TILEDECORS_DRAW_OPAQUE;
      fwrite(&DecTile2->CircleCollision, sizeof(DecTile2->CircleCollision), 1, fnew);
      fwrite(&DecTile2->CircleCenterX, sizeof(DecTile2->CircleCenterX), 1, fnew);
      fwrite(&DecTile2->CircleCenterY, sizeof(DecTile2->CircleCenterY), 1, fnew);
      fwrite(&DecTile2->CircleRadius, sizeof(DecTile2->CircleRadius), 1, fnew);
      fwrite(&DecTile2->DrawMethode, sizeof(DecTile2->DrawMethode), 1, fnew);
      fwrite(&DecTile2->reserved, sizeof(DecTile2->reserved), 1, fnew);

      fclose(f);   // efface le fichier
      fclose(fnew); // fin d'écriture du fichier

      // libère the mem
      delete DecTile2->TrapezTab;
      delete DecTile2;
      }
   };

   MessageBoxA(NULL,"Convertion réussie","YES !",0);
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

