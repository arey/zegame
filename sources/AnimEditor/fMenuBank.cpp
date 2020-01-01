//---------------------------------------------------------------------------
#include "consts.h"
#include "fPrincipale.h"
#include "DDLib.h"
#pragma hdrstop
#include "fMenuBankGrav.h"
#include "fMenuBank.h"
#include "fMenuBankGrille.h"

#include <math.h>
#include <stdio.h>

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TMenuNewBank *MenuNewBank;

// Type de banque
enum eBanqueType { POINT1, POINT8 };
eBanqueType BanqueType;

eBmpType  BmpLectureType;

const char Prefix[8][3] = {"e" , "se", "s", "so", "o", "no", "n", "ne" };
const int PrefixSize[8] = { 1, 2, 1, 2, 1, 2, 1, 2 };

// Fichier *.spi
FILE *spiFile;

// Indice du 1er bmp de la série de sprite
int indiceSpriteIni;

//  MonTile repésente l'image de l'anim sur lequel je travaille actuellement
Graphics::TBitmap *MonTile;

//---------------------------------------------------------------------------
__fastcall TMenuNewBank::TMenuNewBank(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMenuNewBank::BitBtn1Click(TObject *Sender)
{
    // Recrée une image toute propre
  //  if (FormImageGrille->Grille->Picture!=NULL)
  //  { delete FormImageGrille->Grille->Picture; }
    FormImageGrille->Grille->Picture = new TPicture;

    // Vérifie si tt est remplie pour pas faire de bétise
    if ((ETName->Text=="") || (ETNbFrame->Text=="") || (ETColFond->Text=="")
      || (ETColOmb->Text=="")) MessageBox(0,"Veuillez remplir les p''tites cacases","Error",0);
    else
    {
    //*** 0) Regarde si l'on souhaite optimizer l'espace disk & mémoire ***
    //       ou si l'on a affaire à une texture 256x256
    int SpaceOptimization = CheckBoxSpaceOptimization->Checked;
    bool Texture256x256 = CBTexture3D->Checked;

    //*** 0') Regarde comment doit-on lire la série de bmp ***
    if      (RadioButton1->Checked) BmpLectureType = CARS_8;
    else if (RadioButton2->Checked) BmpLectureType = ZEROS_2;
    else if (RadioButton3->Checked) BmpLectureType = ZEROS_3;

    //*** 0") Prélève l'indice de début pour fMenuBankGrav.cpp ***
    indiceSpriteIni = ETFirst->Text.ToInt();

    //*** 1) Ecrit l'en-tête du fichier SPI ***

    // Choppe la description
    strcpy(FrameInfo.Description,ETName->Text.c_str());

    // Regarde quel type de banque c'est
    if (Point1->Checked) BanqueType = POINT1;
    else			   	 BanqueType = POINT8;

    // puis calcule le nombre de sprites d'aprés le type
    switch (BanqueType)
    {   case POINT1 : FrameInfo.NbSprites = ETNbFrame->Text.ToInt();  break;
    	case POINT8 : FrameInfo.NbSprites = ETNbFrame->Text.ToInt() * 8;  break;
    }

    // le n° de couleur de fond et CoulOmbr et leurs composantes
    FrameInfo.CoulFond=ETColFond->Text.ToInt();
    FrameInfo.CoulOmbr=ETColOmb->Text.ToInt();

    unsigned int r,g,b;
    r = ETcf_R->Text.ToInt();
    g = ETcf_G->Text.ToInt();
    b = ETcf_B->Text.ToInt();
    FrameInfo.cfRGB = RGB24(r,g,b);

    r = ETco_R->Text.ToInt();
    g = ETco_G->Text.ToInt();
    b = ETco_B->Text.ToInt();
    FrameInfo.coRGB = RGB24(r,g,b);

    // Sauvegarde l'en-tête du fichier SPI
    AnsiString SpiFileName = CheminApplication + cPathTemp + FrameInfo.Description;
    SpiFileName += ".spi";
    if ((spiFile = fopen(SpiFileName.c_str(),"wb"))==NULL)
                MessageBox(0,SpiFileName.c_str(),"Error de création de fichier",1);
    fwrite(&FrameInfo,sizeof(FrameInfo),1,spiFile);

    //*** 2) Calcule la position de chaque tileanim ***

    // Dimension initiale de la grille
	FormImageGrille->Grille->Width = 0;
	FormImageGrille->Grille->Height = 0;

    // Interval
    int NbSpritePerTile = ETNbFrame->Text.ToInt();


    // Alloue la place nécessaire
    MonTile=new Graphics::TBitmap;
    switch (BanqueType)
    { case POINT1 :
      {
         int first = ETFirst->Text.ToInt();
         int iter = first;
         int iterZero = 0;

         // Charge le 1er bmp
         AnsiString tilename = cPathTemp + ETBmp->Text;
         switch (BmpLectureType)
         {  case CARS_8  :
         	case ZEROS_2 : if (iter<10) tilename += "0"; break;
            case ZEROS_3 :
              { if (iter<10) tilename += "00";
              	else if (iter<100) tilename +="0"; } break;
         }

         tilename += iter;
         tilename += ".bmp";
         MonTile->LoadFromFile(tilename);

         // Choppe la résolution
         int swidth  = SpriteInfo.Width  = MonTile->Width;
         int sheight = SpriteInfo.Height = MonTile->Height;

         // Les place bêtement les uns à la suite des autres
         int NbSpritePerLargeur;
         if (Texture256x256)
         { NbSpritePerLargeur = 512 / swidth;
         } else
         { NbSpritePerLargeur = SurfaceMaxWidth / swidth;
         }

         if (NbSpritePerLargeur == 0)	// Impossible
	      {   MessageBox(0,"ERROR !","Image trop grande",0); return; }

         int NbSpritePerHauteur =NbSpritePerTile / NbSpritePerLargeur;
         if ((SurfaceMaxWidth % (NbSpritePerLargeur*swidth)) != 0) NbSpritePerHauteur++;
         if (NbSpritePerLargeur > NbSpritePerTile)
           {  NbSpritePerLargeur = NbSpritePerTile;
           	  NbSpritePerHauteur = 1;
           }

         // Optimise la place disque & mémoire
         if (NbSpritePerHauteur!=1)
          {
			 float fval = NbSpritePerTile;
             fval = fval / NbSpritePerHauteur;
             int eval = ceil(fval);
             if (eval != NbSpritePerLargeur)
               {
                   NbSpritePerLargeur = eval;
                   fval = NbSpritePerTile;
                   fval = fval / eval;
                   NbSpritePerHauteur = ceil(fval);
               }
          }

         // taille de la grille
         if (Texture256x256)
         {  FormImageGrille->Grille->Width  =  512;
            FormImageGrille->Grille->Height =  512;
         } else
         {  FormImageGrille->Grille->Width  =  NbSpritePerLargeur * swidth;
            FormImageGrille->Grille->Height =  NbSpritePerHauteur * sheight;
         }


         // Copie le 1er tileanim déjà chargé
         //    a) dans l'image de la grille
         int cX = 0, cY = 0;
         SpriteInfo.Num = iterZero;
         SpriteInfo.CoordX = cX;
         SpriteInfo.CoordY = cY;
	 FormImageGrille->Grille->Canvas->Draw(cX ,cY , MonTile);

         //    b) dans le fichier spi
     	 fwrite(&SpriteInfo,sizeof(SpriteInfo),1,spiFile);
         // rmq : il manque les GravX & GravY;

         // Copie les tileanims les uns à la suite des autres
         // et sauvegarde leurs infos ds le SPI
         int i = 1;
         cX += swidth;
         iterZero++;
         iter++;
       	for (int j = 0; j < NbSpritePerHauteur ; j++)
          {

              for  ( ; (i < NbSpritePerLargeur) && (iterZero < NbSpritePerTile) ; i++)
              {
              	// Charge le 1er bmp
                AnsiString tilename = cPathTemp + ETBmp->Text;
                switch (BmpLectureType)
                {  case CARS_8  :
                   case ZEROS_2 : if (iter<10) tilename += "0"; break;
                   case ZEROS_3 :
                     { if (iter<10) tilename += "00";
                       else if (iter<100) tilename +="0"; } break;
                }

                tilename += iter;
                tilename += ".bmp";
                MonTile->LoadFromFile(tilename);

                SpriteInfo.Num = iterZero;
                SpriteInfo.CoordX = cX;
                SpriteInfo.CoordY = cY;
                SpriteInfo.Width  = MonTile->Width;
                SpriteInfo.Height  = MonTile->Height;

                FormImageGrille->Grille->Canvas->Draw(cX ,cY , MonTile);
                fwrite(&SpriteInfo,sizeof(SpriteInfo),1,spiFile);

                iter++;
                iterZero++;
                cX += swidth;
              }
             i = 0;
			 cX = 0;
             cY += sheight;
          }
      }; break;
      case POINT8 :
      { if (SpaceOptimization)
         {
          // Ligne du bitmap où il faut commencer à dessiner les prochains sprites de l'anim
          int GrilleLargeurMax = 0;		// largeur maximale de la grille
          int GrilleHauteurMax = 0;
          FormImageGrille->Grille->Width = 0;
          FormImageGrille->Grille->Height = 0;

          int cursX = 0, cursY = 0;       // Position du coin supérieur-droit sur le bmp du prochain sprite
          int ligneHauteur = 0;           // Hauteur de la ligne courante
          int iterZero = 0;

          // Prévoit la largeur maximale
          FormImageGrille->Grille->Width  = SurfaceMaxWidth;
          // Prévoit au pif ...
          FormImageGrille->Grille->Height =  4 * 200 *8 * 2;
/*         TRect a;
          a.Left = a.Top = 0;
          a.Bottom = FormImageGrille->Grille->Height;
          a.Right = FormImageGrille->Grille->Width;
          FormImageGrille->Grille->Canvas->Pen->Color = 0x00FFFFFF;
          FormImageGrille->Grille->Canvas->FillRect(a);
*/
          for (int noanim=0 ; noanim < 8 ; noanim++)
          {
           int first = ETFirst->Text.ToInt();
           int iter = first;

           for (int i=0 ; i < NbSpritePerTile ; i++)
            {
               // Charge le bmp
               AnsiString tilename = cPathTemp;
               tilename += Prefix[noanim];
               tilename += ETBmp->Text;
               switch (BmpLectureType)
               {  case CARS_8  :
                    { if (PrefixSize[noanim]==1) tilename += "0";
                      if (iter<10) tilename += "0";
                    } break;
                  case ZEROS_2 :
                    { if (iter<10) tilename += "0";
                    } break;
                  case ZEROS_3 :
                    { if (iter<10) tilename += "00";
                      else if (iter<100) tilename +="0";
                    } break;
               }
               tilename += iter;
               tilename += ".bmp";
               MonTile->LoadFromFile(tilename);

               // Choppe la résolution
               int swidth  = SpriteInfo.Width  = MonTile->Width;
               int sheight = SpriteInfo.Height = MonTile->Height;

               // Regarde où faut-il placer ce sprite
               if (cursX + swidth > SurfaceMaxWidth) // on change de ligne
                  { cursX = 0; cursY += ligneHauteur;
                    GrilleHauteurMax += ligneHauteur;
                    ligneHauteur = 0;
                   }

               if (sheight>ligneHauteur) ligneHauteur = sheight;

               SpriteInfo.Num = iterZero;
               SpriteInfo.CoordX = cursX;
               SpriteInfo.CoordY = cursY;
               SpriteInfo.Width  = MonTile->Width;
               SpriteInfo.Height  = MonTile->Height;

               FormImageGrille->Grille->Canvas->Draw(cursX ,cursY , MonTile);
               fwrite(&SpriteInfo,sizeof(SpriteInfo),1,spiFile);

               cursX += swidth;
               if (cursX>GrilleLargeurMax) GrilleLargeurMax = cursX;

               iterZero++;
               iter++;
            };
          }
          GrilleHauteurMax +=ligneHauteur;
          // redimensionne le bitmap à la bonne hauteur (ki avait été sur-évalué)
          FormImageGrille->Grille->Picture->Bitmap->Height = GrilleHauteurMax;
          FormImageGrille->Grille->Picture->Bitmap->Width  = GrilleLargeurMax;
         }
        else
         {

		  // Ligne du bitmap où il faut commencer à dessiner les prochains sprites de l'anim
          int NumLigne = 0;
          int GrilleLargeurMax = 0;		// largeur maximale de la grille
          FormImageGrille->Grille->Width = 0;
          FormImageGrille->Grille->Height = 0;
          for (int noanim=0 ; noanim < 8 ; noanim++)
          {
           int first = ETFirst->Text.ToInt();
           int iter = first;
           int iterZero = 0;

           // Charge le 1er bmp
           AnsiString tilename = cPathTemp;
           tilename += Prefix[noanim];
           tilename += ETBmp->Text;
           switch (BmpLectureType)
           {  case CARS_8  :
                { if (PrefixSize[noanim]==1) tilename += "0";
                  if (iter<10) tilename += "0";
                } break;
              case ZEROS_2 :
                { if (iter<10) tilename += "0";
                } break;
              case ZEROS_3 :
                { if (iter<10) tilename += "00";
                  else if (iter<100) tilename +="0";
                } break;
           }
           tilename += iter;
           tilename += ".bmp";
           MonTile->LoadFromFile(tilename);

           // Choppe la résolution
           int swidth  = SpriteInfo.Width  = MonTile->Width;
           int sheight = SpriteInfo.Height = MonTile->Height;

           // Les place bêtement les uns à la suite des autres
           int NbSpritePerLargeur = SurfaceMaxWidth / swidth;
           if (NbSpritePerLargeur == 0)	// Impossible
                {   MessageBox(0,"ERROR !","Image trop grande",0); return; }

           int NbSpritePerHauteur = ceil( (double) NbSpritePerTile / (double) NbSpritePerLargeur);

           if (NbSpritePerLargeur > NbSpritePerTile)
             {  NbSpritePerLargeur = NbSpritePerTile;
             }

           // taille de la grille
           if (NbSpritePerLargeur * swidth > GrilleLargeurMax)
                   GrilleLargeurMax = NbSpritePerLargeur * swidth;
           // Prévoit la largeur maximale
           FormImageGrille->Grille->Width  = SurfaceMaxWidth;
           // Prévoit 2 fois p^lus de ce qu'il faudrait
           FormImageGrille->Grille->Height =  NbSpritePerHauteur * sheight *8 * 2;


           // Copie le 1er tileanim déjà chargé
           //    a) dans l'image de la grille
           int cX = 0;
           int cY = NumLigne;
           SpriteInfo.Num = iterZero;
           SpriteInfo.CoordX = cX;
           SpriteInfo.CoordY = cY;
           FormImageGrille->Grille->Canvas->Draw(cX ,cY , MonTile);

           //    b) dans le fichier spi
               fwrite(&SpriteInfo,sizeof(SpriteInfo),1,spiFile);
           // rmq : il manque les GravX & GravY;

           // Copie les tileanims les uns à la suite des autres
           // et sauvegarde leurs infos ds le SPI
          int i = 1;
          cX += swidth;
          iterZero++;
          iter++;
          for (int j = 0; j < NbSpritePerHauteur ; j++)
            {

                for  ( ; (i < NbSpritePerLargeur) && (iterZero < NbSpritePerTile) ; i++)
                {
                  // Charge le 1er bmp
                  AnsiString tilename = cPathTemp;
                  tilename += Prefix[noanim];
                  tilename += ETBmp->Text;
                  switch (BmpLectureType)
                  {  case CARS_8  :
                       { if (PrefixSize[noanim]==1) tilename += "0";
                         if (iter<10) tilename += "0";
                       } break;
                     case ZEROS_2 :
                       { if (iter<10) tilename += "0";
                       } break;
                     case ZEROS_3 :
                       { if (iter<10) tilename += "00";
                         else if (iter<100) tilename +="0";
                       } break;
                  }
                  tilename += iter;
                  tilename += ".bmp";
                  MonTile->LoadFromFile(tilename);

                  SpriteInfo.Num = iterZero;
                  SpriteInfo.CoordX = cX;
                  SpriteInfo.CoordY = cY;
                  SpriteInfo.Width  = MonTile->Width;
                  SpriteInfo.Height  = MonTile->Height;

                  FormImageGrille->Grille->Canvas->Draw(cX ,cY , MonTile);
                  fwrite(&SpriteInfo,sizeof(SpriteInfo),1,spiFile);

                  iter++;
                  iterZero++;
                  cX += swidth;
                }
               i = 0;
               cX = 0;
               cY += sheight;
             }
          NumLigne += sheight*NbSpritePerHauteur;
          }
          // redimensionne le bitmap à la bonne hauteur (ki avait été sur-évalué)
          FormImageGrille->Grille->Picture->Bitmap->Height = NumLigne;
          FormImageGrille->Grille->Picture->Bitmap->Width  = GrilleLargeurMax;
        }
      }; break;

    };
    fclose(spiFile);
    }
    // Sauve le grand bmp
    AnsiString bmpname = cPathTemp + FrameInfo.Description + ".bmp";
//	FormImageGrille->Grille->Picture->Bitmap->Palette = MonTile->Palette;
    FormImageGrille->Grille->Picture->Bitmap->SaveToFile(bmpname);


    // Remplit quelques boîtes de dialogue ds "FormGrilleGrav"
        switch (BanqueType)
    {   case POINT1 : { FormGrilleGrav->FSurNbAnim->Caption = "1";
    					FormGrilleGrav->FSurNbSprite->Caption = FrameInfo.NbSprites / 1; } break;
    	case POINT8 : { FormGrilleGrav->FSurNbAnim->Caption = "8";
        				FormGrilleGrav->FSurNbSprite->Caption = FrameInfo.NbSprites / 8; } break;
    }
    FormGrilleGrav->Designation->Caption = FrameInfo.Description;
    FormGrilleGrav->BmpName->Caption = ETBmp->Text;

    FormImageGrille->ShowModal();

   delete MonTile;
}
//---------------------------------------------------------------------------
