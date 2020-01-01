//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <stdio.h>

#pragma hdrstop

#include "consts.h"
#include "fMenuBankGrav.h"
#include "fPrincipale.h"
#include "DDLibEditor.h"
#include "fMenuBank.h"


//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TFormGrilleGrav *FormGrilleGrav;

// Fichier *.spi lu
FILE *SpiFile2;

// informations sur la banque
FrInfo  FrameInfo;

// informations relatives à chaque Sprite de la banque
SprInfo *SpriteInfo;

int NbAnim;					// Nb d'animation : 1 ou 8
int NbSpritesPerAnim;       // assez parlant !
int NumAnim;				// numéro de l'anim courante
int NumSprite;				// numéro du sprite courant
int Zoom = 1;				// zoom
int MouseX, MouseY;			// coordonnées de la souris
bool PlaceGravAllAnim = false;          // Place-t-il le pt pour toute l'anim en cours?

//---------------------------------------------------------------------------
__fastcall TFormGrilleGrav::TFormGrilleGrav(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormGrilleGrav::FormShow(TObject *Sender)
{
	// Ouvre quelques trucs sympas
    NbAnim = FSurNbAnim->Caption.ToInt();
    NbSpritesPerAnim = FSurNbSprite->Caption.ToInt();
    FCentX->Text = -1;
    FCentY->Text = -1;
    NumAnim = 1;
    NumSprite = indiceSpriteIni;
    LabelZoom->Caption = Zoom;

    // Charge le fichier spi
    AnsiString tilename = CheminApplication + cPathTemp + Designation->Caption;
    tilename += ".spi";
    if ((SpiFile2 = fopen(tilename.c_str(),"rb"))==NULL)
                MessageBox(0,tilename.c_str(),"Error de lecture de fichier",1);
    fread(&FrameInfo,sizeof(FrameInfo),1,SpiFile2);

    // Alloue le place nécessaire pour charger toutes les structures SprInfo
    SpriteInfo = new SprInfo [FrameInfo.NbSprites];
    for (int i=0 ; i < FrameInfo.NbSprites ; i++)
     {
       fread(&SpriteInfo[i],sizeof(SprInfo),1,SpiFile2);
       SpriteInfo[i].GravX = SpriteInfo[i].GravY = -1;		// ini le tout
     }

    fclose(SpiFile2);
    SpriteScrollChange(Sender);			// dessine l'image

    // Change les barres
    SpriteScroll->Min = indiceSpriteIni;
    SpriteScroll->Max = NbSpritesPerAnim -1 + indiceSpriteIni;
    SpriteScroll->Position = indiceSpriteIni;
}
//---------------------------------------------------------------------------
void __fastcall TFormGrilleGrav::SpriteScrollChange(TObject *Sender)
{
    const char Prefix[8][3] = {"e" , "se", "s", "so", "o", "no", "n", "ne" };
    const int PrefixSize[8] = { 1, 2, 1, 2, 1, 2, 1, 2 };


	NumSprite = SpriteScroll->Position;

    // Charge le fichier bmp
    AnsiString tilename = CheminApplication + cPathTemp;
  switch (NbAnim)
    {  case (1) :
    	{ tilename += BmpName->Caption;
            switch (BmpLectureType)
            {  case CARS_8  :
               case ZEROS_2 : if (NumSprite<10) tilename += "0"; break;
               case ZEROS_3 :
                 { if (NumSprite<10) tilename += "00";
                   else if (NumSprite<100) tilename +="0"; } break;
            }
        }; break;

       case (8) :
        { tilename += Prefix[NumAnim-1];
          tilename += BmpName->Caption;
          switch (BmpLectureType)
               {  case CARS_8  :
                    { if (PrefixSize[NumAnim-1]==1) tilename += "0";
                      if (NumSprite<10) tilename += "0";
                    } break;
                  case ZEROS_2 :
                    { if (NumSprite<10) tilename += "0";
                    } break;
                  case ZEROS_3 :
                    { if (NumSprite<10) tilename += "00";
                      else if (NumSprite<100) tilename +="0";
                    } break;
               }
        }; break;
    }
    tilename += NumSprite;
    tilename += ".bmp";
	PartOfGrille->Picture->Bitmap->LoadFromFile(tilename);


    // Affiche le sprite courant
	int indice = (NumSprite-indiceSpriteIni) + (NumAnim-1)*NbSpritesPerAnim;
    FNumFrame->Caption = NumSprite + 1 + indiceSpriteIni;
	TRect dest;
    dest.Left = 0;
	dest.Top = 0;

	dest.Right = SpriteInfo[indice].Width * Zoom;
	dest.Bottom = SpriteInfo[indice].Height * Zoom;
//    SpriteImage->Width = SpriteInfo[indice].Width * Zoom;
//    SpriteImage->Height = SpriteInfo[indice].Height * Zoom;

    // Dessine l'image
    SpriteImage->Canvas->FillRect(SpriteImage->Canvas->ClipRect);	// d'abord le vide
	SpriteImage->Canvas->StretchDraw(dest,PartOfGrille->Picture->Graphic);

    // Place le centre de gravité, si celui-ci est déjà placé
    int grX = SpriteInfo[indice].GravX;
    int grY = SpriteInfo[indice].GravY;
    if ((grX!=-1) && (grY!=-1))
     {
        POINT p;
        SpriteImage->Canvas->Pen->Width = Zoom;
        SpriteImage->Canvas->Pen->Color = (Graphics::TColor) 0x00FF0000;		// bleu ???
        // ligne horizontale
        p.x = grX*Zoom - 3*Zoom;   p.y = grY*Zoom + Zoom/2;
        SpriteImage->Canvas->PenPos = p;
        SpriteImage->Canvas->LineTo(grX*Zoom + 3*Zoom, grY*Zoom + Zoom/2);
        // ligne verticale
        p.x = grX*Zoom + Zoom/2;  p.y = grY*Zoom - Zoom*2;
        SpriteImage->Canvas->PenPos = p;
        SpriteImage->Canvas->LineTo(grX*Zoom + Zoom/2, grY*Zoom + Zoom*3);
     }
}
//---------------------------------------------------------------------------
void __fastcall TFormGrilleGrav::TrackBarZoomChange(TObject *Sender)
{
	Zoom = TrackBarZoom->Position;
    LabelZoom->Caption = Zoom;
    SpriteScrollChange(Sender);			// redessine l'image ds ce zoom là
}
//---------------------------------------------------------------------------
void __fastcall TFormGrilleGrav::SpriteImageMouseMove(TObject *Sender,
	TShiftState Shift, int X, int Y)
{
    // Choppe les coordonnées de la souris et les transforme d'aprés le zoom
    MouseX = X / Zoom;
    MouseY = Y / Zoom;
	EditMouseX->Text = MouseX;
	EditMouseY->Text = MouseY;
}
//---------------------------------------------------------------------------
void __fastcall TFormGrilleGrav::SpriteImageClick(TObject *Sender)
{
    // Place le nouveau centre de gravité pour toute l'anim courante
    if (PlaceGravAllAnim)
     {
       for (int indice = (NumAnim-1)*NbSpritesPerAnim ; indice < NumAnim*NbSpritesPerAnim ; indice++)
        if ((SpriteInfo[indice].GravX==-1) && (SpriteInfo[indice].GravY==-1))
         {
           SpriteInfo[indice].GravX = MouseX;
           SpriteInfo[indice].GravY = MouseY;
          };
        PlaceGravAllAnim = false;
     }
    else
    {
      // Place le nouveau centre de gravité
      int indice = (NumSprite-indiceSpriteIni) + (NumAnim-1)*NbSpritesPerAnim;
      SpriteInfo[indice].GravX = MouseX;
      SpriteInfo[indice].GravY = MouseY;
    }

    SpriteScrollChange(Sender);			// redessine l'image ds ce zoom là
}
//---------------------------------------------------------------------------
void __fastcall TFormGrilleGrav::BitBtn1Click(TObject *Sender)
// Animation suivante
{
	// Vérifie qu'il a remplit tous les centres de gravités
    for (int i=(NumAnim-1)*NbSpritesPerAnim ; i < NumAnim*NbSpritesPerAnim ; i++)
      { if ((SpriteInfo[i].GravX==-1) && (SpriteInfo[i].GravY==-1))
      		{  MessageBox(0,"Warning !","Vous n'avez pas désigné tous les centres",0);
               return;
            }
      }

	if (NumAnim == NbAnim)	// on arrive au bout => sauve et quitte
      {
         // Sauve le fichier spi
         AnsiString tilename = CheminApplication + cPathTemp + Designation->Caption;
         tilename += ".spi";
         if ((SpiFile2 = fopen(tilename.c_str(),"wb"))==NULL)
                     MessageBox(0,tilename.c_str(),"Error d'écriture de fichier",0);
         fwrite(&FrameInfo,sizeof(FrameInfo),1,SpiFile2);
         for (int i=0 ; i < FrameInfo.NbSprites ; i++)
            fwrite(&SpriteInfo[i],sizeof(SprInfo),1,SpiFile2);

         // on libère tout
		 delete [] SpriteInfo;
	     fclose(SpiFile2);

         MessageBox(0,tilename.c_str(),"Sauvegarde réussie",0);
		 Close();
      }
    else					// sinon, on est ds le cas où NbAnim=8
      {  NumAnim++;
         NumSprite = indiceSpriteIni;
         SpriteScroll->Position = NumSprite;
         fNumAnim->Caption = NumAnim;
         FNumFrame->Caption = NumSprite;

         SpriteScrollChange(Sender);			// redessine l'image ds ce zoom là
      };
}
//---------------------------------------------------------------------------
void __fastcall TFormGrilleGrav::BitBtn2Click(TObject *Sender)
{
     // Revient à l'animation précédente arrière pour faire des modifs
     if (NumAnim != 1)        // si c'est pas la 1ère
     {   NumAnim--;
         NumSprite = 1;
         SpriteScroll->Position = NumSprite;
         fNumAnim->Caption = NumAnim;
         FNumFrame->Caption = NumSprite;

         SpriteScrollChange(Sender);			// redessine l'image ds ce zoom là
      };
}
//---------------------------------------------------------------------------

//=== Place le point au même endroite paour toute l'anim =====================//
// => ne modifie que les points non définis
void __fastcall TFormGrilleGrav::BitBtn3Click(TObject *Sender)
{
    // Place le prochaine point comme centre de gravité pour toute l'anim courante
    PlaceGravAllAnim = true;
}
//---------------------------------------------------------------------------
