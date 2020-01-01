////////////////////////////////////////////////////////////////////////////////
// TileDecors.cpp : Editeur d'images se d�coupant en trap�zes et servant de   //
//					d�cors au RPG                                             //
////////////////////////////////////////////////////////////////////////////////

//=== Donn�es ================================================================//
#include <vcl\vcl.h>
#include <stdio.h>
#include <math.h>
#include "TileDecors.h"
#include "ImDecors.h"
#include "ImageZoom.h"

float TileDecVersion = 0.98;	        // Version du Programme

int ImageWidth  = 350,			// Image o� sera trac�e la grille
    ImageHeight = 364;

int GrilleX = ImageWidth / 2,
    GrilleY = ImageHeight /2;

int Zoom = 2;

int DecorsCornerX, DecorsCornerY;	// Position du coin sup�rieur gauche du d�cors sur la grille
unsigned int CoulTransRVB;		// composante de la Couleur Transparente
AnsiString  TileDecExt = ".dec";	// extension de fichier

// Donn�es g�om�triques d'un trap�ze
int ISO_HEIGHT 		= 21-1,
    ISO_WIDTH  		= 39-1,
    ISO_HEIGHT_HALF = 10,
    ISO_WIDTH_HALF  = 19;

int ColorFond = clPurple;

AnsiString ficTravail;			// nom de fichier sur lequel on bosse (sans l'extension *.bmp/*.dec)

Graphics::TBitmap *MonBitmap;           //  MonBitmap rep�sente l'image o� se situe la grille
TRect gScreen;                          //  Surface de cette m�me image

//  Permet de savoir directement � quelle �tape de l'�dition on est
#define Etape_NOTHING           0	// on peut tout faire
#define Etape_PLACEDECORS       1	// On place l'objet
#define Etape_TRAPEZEPRINCIPAL  2	// On place le trap�ze initial
#define Etape_AUTRESTRAPEZE	3	// On place les autres trap�zes
#define Etape_COULTRANSPARENTE  4	// On doit choisir la couleur de fond
#define Etape_INFOTRAPEZE       5       // On doit s�lectionner un trap�ze pour lui donner quelques infos
#define Etape_SELECTZONE        6       // On doit s�lectionner une zone du sprite
#define Etape_SELECTZONECERCLE  7       // On doit s�lectionner une zone circulaire autour du sprite

int Etape = Etape_NOTHING;		// Suivant l'�tape en cours, des fonctions sont appel�es

BOOL DecorsPlace = FALSE;		// indique si l'objet est plac�
int DecorsX, DecorsY;			// Position de la souris sur l'image D�cors
int circleCenterX, circleCenterY;       // Coord. du centre du cercle
int circleRadius;                       // Rayon du m�me cercle

#pragma resource "*.dfm"
TForm1 *Form1;				// Fiche principale

AnsiString CheminDecorsTile;    	// chemin d'acc�s de la lib

// Carte des trap�zes
int NbSelectTrap;			// nb de trap�zes s�lectionn�s
int NbTabTrap;				// nb de trapeze de chaque cot� du tableau
int TrapPrincX, TrapPrincY;		// !!! Coordonn�es du coin sup�rieur du trap�ze principal par rapport au coin sup�rieur-gauche de l'IMAGE ET NON DE LA GRILLE COMME DS LA STRUCT
int TrapPrincU, TrapPrincV;		// Coordonn�es du trap�ze Principal ds the grille
int TrapSelectU, TrapSelectV;		// Coordonn�es du trap�ze s�lectionn� ds the grille

// Coordonn�es de certains points
int ZoneX1, ZoneY1, ZoneX2, ZoneY2;     // Coordonn�es des points d'une lamelle d'un sprite


// Structure compl�mentaire utilis� uniquement par cet �diteur
struct  TrapezeEditor
	{
	Trapeze Trap;    	        // vrai trap�ze
        BOOL Principal;			// est-ce le trap principal ?
        BOOL Selectionne;		// est-il s�lectionn� ?
    };

TrapezeEditor **TabTrap;		// Tableau dynamique des trap�zes
Trapeze TrapPrincipal;			// Acces direct au trapeze principal



////////////////////////////////////////////////////////////////////////////////
// Fonctions Annexes														  //
////////////////////////////////////////////////////////////////////////////////

//== Transforme les composantes ==============================================//
unsigned long RGB24(unsigned long Red,unsigned long Green,unsigned long Blue)
{
    static unsigned long col;
    col = Red;    col<<=8;
    col |= Green; col<<=8;
    col |= Blue;
    return col;			// format 0x00RRVVBB
}
//============================================================================//


////////////////////////////////////////////////////////////////////////////////
//== Fonctions de la FICHE PRINCPALE =========================================//
////////////////////////////////////////////////////////////////////////////////

//=  Constructeur ============================================================//
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
    MonBitmap = NULL;
    circleCenterX = circleCenterY = -1;

    // Dimesion de l'application
    Height = 630;
    Width = 560;

    // Image de repr�sentation de la grille
    ImageGrille->Width  = ImageWidth;
    ImageGrille->Height = ImageHeight;
    gScreen.Left = 0; gScreen.Top = 0;
    gScreen.Right = ImageWidth;
    gScreen.Bottom = ImageHeight;

    ImageGrille->Canvas->Brush->Color = ColorFond;
 	ImageGrille->Canvas->FillRect(gScreen);

    // Charge l'image
    if (MonBitmap!=NULL) delete MonBitmap;
    MonBitmap=new Graphics::TBitmap;
//    MonBitmap->LoadFromFile("murref2.bmp");
    MonBitmap->Canvas->CopyMode = cmSrcCopy;

    // allocation du Tableau dynamique
    NbTabTrap = ImageWidth / ( ISO_WIDTH );
	TabTrap = new TrapezeEditor* [NbTabTrap+1];  	// lignes
    for (int i=0 ; i < NbTabTrap + 1; i++) TabTrap[i] = new TrapezeEditor [NbTabTrap+1];  // colonnes

    // Chemin de l'application
    CheminDecorsTile = Application->ExeName;
    CheminDecorsTile.Delete(CheminDecorsTile.Pos("CREATEDECORS.EXE"),strlen("CREATEDECORS.EXE"));

    ResetAll(Owner);

    // Param�tres des infos sur les trap�zes
    ComboBoxSurface->Clear();
    for (int i =0; i<TrapTERRAINNb; i++)
     { ComboBoxSurface->Items->Add(TrapTERRAINText[i]);
     }
    ComboBoxCollide->Clear();
    for (int i =0; i<TrapCOLLIDENb; i++)
     { ComboBoxCollide->Items->Add(TrapCOLLIDEText[i]);
     }

}
//----------------------------------------------------------------------------//

//=== Trace un rectangle =====================================================//
void __fastcall TForm1::DrawRectangle(int x1, int y1, int x2, int y2, int c)
{   ImageGrille->Canvas->Pen->Color = 0x00DDAA00;
    ImageGrille->Canvas->Pen->Mode = pmCopy;
    POINT PDepart;
    PDepart.x = x1;  PDepart.y = y1;
    ImageGrille->Canvas->PenPos = PDepart;
    ImageGrille->Canvas->LineTo(x1, y2);
    PDepart.x = x1;  PDepart.y = y2;
    ImageGrille->Canvas->PenPos = PDepart;
    ImageGrille->Canvas->LineTo(x2, y2);
    PDepart.x = x2;  PDepart.y = y2;
    ImageGrille->Canvas->PenPos = PDepart;
    ImageGrille->Canvas->LineTo(x2, y1);
    PDepart.x = x2;  PDepart.y = y1;
    ImageGrille->Canvas->PenPos = PDepart;
    ImageGrille->Canvas->LineTo(x1, y1);
}
//----------------------------------------------------------------------------//

//= Lors du mvt de la souris =================================================//
void __fastcall TForm1::ImageGrilleMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y)
{
    // Affiche des donn�es
    gCoordX->Text = X;
    gCoordY->Text = Y;

    // On Calcule les Coordonn�es du trap�ze dans lequel la souris se trouve
    float GrilleU, GrilleV;
    float RealX = X - GrilleX;
    float RealY = Y - GrilleY;
    GrilleU = (RealX * ISO_HEIGHT_HALF + RealY * ISO_WIDTH_HALF) / (2 * ISO_HEIGHT_HALF * ISO_WIDTH_HALF);
    GrilleV = ( - RealX * ISO_HEIGHT_HALF + RealY * ISO_WIDTH_HALF) / (2 * ISO_HEIGHT_HALF * ISO_WIDTH_HALF);
    // Pour r�gler le probl�me des arrondis des chiffres n�gatifs

    gTrapU->Text = floor(GrilleU);
    gTrapV->Text = floor(GrilleV);

    // Raffiche tout les trucs de l'�cran
    ImageGrille->Canvas->Brush->Color = ColorFond;
    ImageGrille->Canvas->FillRect(gScreen);

    if (Etape == Etape_PLACEDECORS) AfficheDecors(Sender, X - ImaDec->Width/2, Y - ImaDec->Height);
    else if (DecorsPlace) AfficheDecors(Sender,DecorsCornerX , DecorsCornerY);
    AfficheGrille(Sender);

    if (Etape == Etape_TRAPEZEPRINCIPAL) SelectTrapeze(Sender,0);
    if (Etape == Etape_AUTRESTRAPEZE)    SelectTrapeze(Sender,0);
    if (Etape == Etape_INFOTRAPEZE)      SelectTrapeze(Sender,0);

    if (Etape == Etape_SELECTZONE)
    {  // Dessine un rectangle
       if ((ZoneX1!=0) && (ZoneY1!=0))
       {  DrawRectangle(ZoneX1, ZoneY1, X, Y, 0x00DDAA00);
       }
    }
    else  // On doit s�lectionner un Cercle autour du sprite
    if (Etape == Etape_SELECTZONECERCLE)
    {  if ((circleCenterX!=-1) && (circleCenterY!=-1))
       {  circleRadius = sqrt((X-circleCenterX)*(X-circleCenterX)
                          +(Y-circleCenterY)*(Y-circleCenterY));    // Calcul du rayon du cercle
          DrawCircle(circleCenterX, circleCenterY, circleRadius, 0x0505DD);
       }
    }

    // Balance tout ds la fen�tre "Zooming"
    Zooming->ImageGrilleZ->Width = ImageWidth*Zoom;
    Zooming->ImageGrilleZ->Height = ImageHeight*Zoom;
    Zooming->ImageGrilleZ->Canvas->Draw(0,0 , ImageGrille->Picture->Graphic);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::AfficheGrille(TObject *Sender)
{

 // Affichage de la grille
    // D�termine le nombre de trap�ze trac� de chaque c�t� :
    int NbTrap = ImageWidth / ( ISO_WIDTH );

    ImageGrille->Canvas->Pen->Color = 0x0000FF00;
    ImageGrille->Canvas->Pen->Mode = pmMerge;
    ImageGrille->Canvas->Pen->Width = 1;	// [LAPTOP] => 2

 	POINT PDepart, PArrivee;
    PDepart.x = GrilleX;  PDepart.y = GrilleY;
    PArrivee.x = GrilleX  - NbTrap * ISO_WIDTH_HALF;
    PArrivee.y = GrilleY  + NbTrap * ISO_HEIGHT_HALF;
    ImageGrille->Canvas->PenPos = PDepart;

    // Trace les lignes des V
    for (int i=0; i<NbTrap+1; i++)
    {
       ImageGrille->Canvas->LineTo(PArrivee.x, PArrivee.y);
       PDepart.x += ISO_WIDTH_HALF;
       PDepart.y += ISO_HEIGHT_HALF;
       PArrivee.x += ISO_WIDTH_HALF;
       PArrivee.y += ISO_HEIGHT_HALF;
       ImageGrille->Canvas->PenPos = PDepart;
    }

    // Trace les lignes des U
    PDepart.x = GrilleX;  PDepart.y = GrilleY;
    PArrivee.x = GrilleX  + NbTrap * ISO_WIDTH_HALF;
    PArrivee.y = GrilleY  + NbTrap * ISO_HEIGHT_HALF;
    ImageGrille->Canvas->PenPos = PDepart;
      for (int i=0; i<NbTrap+1; i++)
    {
       ImageGrille->Canvas->LineTo(PArrivee.x, PArrivee.y);
       PDepart.x -= ISO_WIDTH_HALF;
       PDepart.y += ISO_HEIGHT_HALF;
       PArrivee.x -= ISO_WIDTH_HALF;
       PArrivee.y += ISO_HEIGHT_HALF;
       ImageGrille->Canvas->PenPos = PDepart;
    }

//    for (int i=0; i <5 ; i++) TabTrap[2][i].Selectionne = TabTrap[2][i].Principal = TRUE;

    // Scrutte toute la mini-map pour afficher les trap�zes
    for (int u=0 ; u < NbTabTrap + 1 ; u++)
    	for (int v=0 ; v < NbTabTrap + 1 ; v++)
               if (TabTrap[u][v].Selectionne)
               {   // Trap�ze principal ou secondaire
                   if (TabTrap[u][v].Principal) SelectTrapeze2(Sender, 1, u, v);
		   else SelectTrapeze2(Sender, 2, u, v);
                   // Zone d'affichage du trap�ze
                   if (TabTrap[u][v].Trap.DrawTrapezekey)
                   {  DrawRectangle(
                          TabTrap[u][v].Trap.lamX1 + DecorsCornerX,
                          TabTrap[u][v].Trap.lamY1 + DecorsCornerY,
                          TabTrap[u][v].Trap.lamX2 + DecorsCornerX,
                          TabTrap[u][v].Trap.lamY2 + DecorsCornerY,
                          0x00CCBB00);
                      SelectTrapeze2(Sender, 3, u, v);    
                   }   
               };

    // Affiche le cercle des collisions
    if (DecTile.CircleCollision)
    {  DrawCircle(DecTile.CircleCenterX, DecTile.CircleCenterY, DecTile.CircleRadius, 0x0202CC);
    }
};


void __fastcall TForm1::AfficheDecors(TObject *Sender,int MouseX,int MouseY)
{
	ImageGrille->Canvas->Draw(MouseX ,MouseY , MonBitmap);
};


void __fastcall TForm1::PlaceObjBtnClick(TObject *Sender)
{
    if (Etape == Etape_PLACEDECORS) Etape = Etape_NOTHING;
    else
      {
        Etape = Etape_PLACEDECORS;
        DecorsPlace = FALSE;
      };
}
//------------------------------------------------------------------------------

//== Choisir un fichier *.bmp ================================================//
void __fastcall TForm1::ImageBtnClick(TObject *Sender)
{

    SelectDialog->InitialDir = CheminDecorsTile + "Tiles\\";
    SelectDialog->FilterIndex = 1;  // *.bmp
    SelectDialog->Execute();
    AnsiString nom = SelectDialog->FileName;
    ficTravail = nom;   // Sauvegarde le fichier sur lequel on travaille
    ficTravail.Delete(ficTravail.Pos("."),strlen(".bmp"));
    if (nom == "")
      { MessageBoxA(NULL,"Aucun fichier choisi","Warning !",0); return; };
    nom.Delete(1, CheminDecorsTile.Length());    // �limine le chemin
    MessageBoxA(NULL,"DEBUGGAGE","DEBUG 1",0);
    // Afficher l'image s�lectionn� et ses caract�ristques
    // Charge l'image
    MonBitmap=new Graphics::TBitmap;					// Brush de l'image
    MonBitmap->LoadFromFile(nom);
    MonBitmap->Canvas->CopyMode = cmSrcCopy;
    ImaDec->Picture->LoadFromFile(nom); // Affiche l'image s�lectionn�
    DecorsWidth->Text = ImaDec->Picture->Width;
    DecorsHeight->Text = ImaDec->Picture->Height;
    nom.LowerCase();
    nom.Delete(nom.Pos(".bmp"),4);nom.Delete(nom.Pos(".BMP"),4);
    nom.Delete(nom.Pos("Tiles\\"),6);  // Supprime du chemin le rep 'Tiles\'
    // Supprime tous les autres r�pertoires du chemin
    bool find = true;
    while (find)
    {  int iter = nom.Pos("\\");
       if (iter!=0) nom.Delete(1,iter);
       else find = false;
    }

    ImageName->Text = nom;
};
//---------------------------------------------------------------------------

//== Clic sur la grille ======================================================//
void __fastcall TForm1::ImageGrilleClick(TObject *Sender)
{
	// Si l'on est en "Placer l'objet", on scrute la position de l'objet sur l'�cran
    if (Etape == Etape_PLACEDECORS)
      {
	DecorsCornerX =	gCoordX->Text.ToInt() -ImaDec->Width/2;
        DecorsCornerY =	gCoordY->Text.ToInt() -ImaDec->Height;
        DecorsPlace = TRUE;
        Etape = Etape_NOTHING;
      }
    else if (Etape == Etape_TRAPEZEPRINCIPAL)
      {
        Etape = Etape_AUTRESTRAPEZE;
        Help->Caption ="S�lectionner les autres trap�zes de l'objet";
        // Coordonn�es du coin sup�rieur du triangle principal
        int PosU = gTrapU->Text.ToInt();
        int PosV =  gTrapV->Text.ToInt();

        if ((PosU<0) || (PosU>NbTabTrap) || (PosV<0) || (PosV>NbTabTrap))
            { MessageBoxA(NULL,"Trap�ze s�lectionn� non ds la grille","Warning !",0); return; };

        TrapSelectU = TrapPrincU = PosU;
        TrapSelectV = TrapPrincV = PosV;
        TrapPrincX = (PosU - PosV) * ISO_WIDTH_HALF    + GrilleX;
        TrapPrincY = (PosU + PosV) * ISO_HEIGHT_HALF   + GrilleY;

        // Remplit la mini-map
        TrapPrincipal.TrapU = TrapPrincipal.TrapV = 0;
        // on s'occupe pas du rest pour l'insant ...
        TabTrap[PosU][PosV].Trap = TrapPrincipal;
        TabTrap[PosU][PosV].Principal = TRUE;
        TabTrap[PosU][PosV].Selectionne = TRUE;

        NbSelectTrap++;
        gTrapNb->Text = NbSelectTrap;

        // Infos trap�ze s�lectionn�
        EditTrapU->Text = 0;
        EditTrapV->Text = 0;
        NumEdit->Text = NbSelectTrap;
        LabelTypTrap->Caption = "Principal";

        // Remplit les bo�tes de dialogues avec les infos du trap�ze s�lectionn�
        EditHauteur->Text = TabTrap[TrapSelectU][TrapSelectV].Trap.Hauteur = 0;
        ComboBoxSurface->ItemIndex = TabTrap[TrapSelectU][TrapSelectV].Trap.Terrain = INCONNU;
        CheckBoxMarcher->Checked = !(TabTrap[TrapSelectU][TrapSelectV].Trap.Accessible = TRUE);
        ComboBoxCollide->ItemIndex = TabTrap[TrapSelectU][TrapSelectV].Trap.TrapCollide = PARTIELLE;
        CBDrawTrapezeKey->Checked = TabTrap[TrapSelectU][TrapSelectV].Trap.DrawTrapezekey;
      }
   else if (Etape == Etape_AUTRESTRAPEZE)
      {
        // Coordonn�es du coin sup�rieur du triangle principal
        int PosU = gTrapU->Text.ToInt();
        int PosV =  gTrapV->Text.ToInt();

        if ((PosU<0) || (PosU>NbTabTrap) || (PosV<0) || (PosV>NbTabTrap))
            { MessageBoxA(NULL,"Trap�ze s�lectionn� non ds la grille","Warning !",0); return; };
 		if ((PosU == TrapPrincU) && (PosV == TrapPrincV))
         	{ MessageBoxA(NULL,"Vous ne pouvez remplacer ce trap�ze principal","Warning !",0); return; };

        TrapSelectU = PosU;
        TrapSelectV = PosV;
        int TrapX = (PosU - PosV) * ISO_WIDTH_HALF    + GrilleX;
 	    int TrapY = (PosU + PosV) * ISO_HEIGHT_HALF   + GrilleY;

	    // Remplit la structure des infos si vides
    	if (!TabTrap[PosU][PosV].Selectionne)
           {
            TabTrap[PosU][PosV].Trap.TrapU = PosU - TrapPrincU;
            TabTrap[PosU][PosV].Trap.TrapV = PosV - TrapPrincV;
            TabTrap[PosU][PosV].Principal = FALSE;
            TabTrap[PosU][PosV].Selectionne = TRUE;
            NbSelectTrap++;
            gTrapNb->Text = NbSelectTrap;
            // Infos trap�ze s�lectionn�
            EditTrapU->Text = TabTrap[PosU][PosV].Trap.TrapU;
            EditTrapV->Text = TabTrap[PosU][PosV].Trap.TrapV;
            NumEdit->Text   = NbSelectTrap;
            LabelTypTrap->Caption = "Secondaire";

            // Remplit les bo�tes de dialogues avec les infos du trap�ze s�lectionn�
            EditHauteur->Text = TabTrap[TrapSelectU][TrapSelectV].Trap.Hauteur = 0;
            ComboBoxSurface->ItemIndex = TabTrap[TrapSelectU][TrapSelectV].Trap.Terrain = INCONNU;
            CheckBoxMarcher->Checked = !(TabTrap[TrapSelectU][TrapSelectV].Trap.Accessible = TRUE);
            ComboBoxCollide->ItemIndex = TabTrap[TrapSelectU][TrapSelectV].Trap.TrapCollide = PARTIELLE;
            CBDrawTrapezeKey->Checked = TabTrap[TrapSelectU][TrapSelectV].Trap.DrawTrapezekey;
           }
        else
           {
            TabTrap[PosU][PosV].Selectionne = FALSE;
            NbSelectTrap--;
            gTrapNb->Text = NbSelectTrap;
            EditTrapU->Text ="";
            EditTrapV->Text = "";
            NumEdit->Text   = "";
            LabelTypTrap->Caption = "";
           };
      }
   else if (Etape == Etape_INFOTRAPEZE)
      {
        // Coordonn�es du coin sup�rieur du triangle principal
        int PosU = gTrapU->Text.ToInt();
        int PosV =  gTrapV->Text.ToInt();

        if ((PosU<0) || (PosU>NbTabTrap) || (PosV<0) || (PosV>NbTabTrap))
            { MessageBoxA(NULL,"Trap�ze s�lectionn� non ds la grille","Warning !",0); return; };

        TrapSelectU = PosU;
        TrapSelectV = PosV;
        int TrapX = (PosU - PosV) * ISO_WIDTH_HALF    + GrilleX;
        int TrapY = (PosU + PosV) * ISO_HEIGHT_HALF   + GrilleY;

        // Remplit la structure des infos si vides
    	if (TabTrap[PosU][PosV].Selectionne)
           {
            // Infos trap�ze s�lectionn�
            EditTrapU->Text = TabTrap[PosU][PosV].Trap.TrapU;
            EditTrapV->Text = TabTrap[PosU][PosV].Trap.TrapV;
            NumEdit->Text   = NbSelectTrap;

            // Remplit les bo�tes de dialogues avec les infos du trap�ze s�lectionn�
            EditHauteur->Text = TabTrap[TrapSelectU][TrapSelectV].Trap.Hauteur;
            ComboBoxSurface->ItemIndex = TabTrap[TrapSelectU][TrapSelectV].Trap.Terrain;
            CheckBoxMarcher->Checked = !(TabTrap[TrapSelectU][TrapSelectV].Trap.Accessible);
            ComboBoxCollide->ItemIndex = TabTrap[TrapSelectU][TrapSelectV].Trap.TrapCollide;
            CBDrawTrapezeKey->Checked = TabTrap[TrapSelectU][TrapSelectV].Trap.DrawTrapezekey;

            PlaceObjBtn->Enabled = FALSE;
            TrapBtn->Enabled = FALSE;
            TrapBtn2->Enabled = FALSE;
            InfoBtn->Enabled = FALSE;
            InfosBtn->Enabled = TRUE;
           }
        else
              { MessageBoxA(NULL,"Trapeze n'appartenant pas � l'objet","Warning !",0); return; };
     }
}
//---------------------------------------------------------------------------

//== Lorsque la souris se d�place sur la Fiche Principale ====================//
void __fastcall TForm1::FormMouseMove(TObject *Sender, TShiftState Shift, int X,
	int Y)
{
    // On rafraichit l'image
    gCoordX->Text = "";
    gCoordY->Text = "";
    gTrapU->Text = "";
    gTrapV->Text = "";

    // Raffiche tout les trucs de l'�cran
    ImageGrille->Canvas->Brush->Color = ColorFond;
 	ImageGrille->Canvas->FillRect(gScreen);

    if (DecorsPlace) AfficheDecors(Sender, DecorsCornerX, DecorsCornerY);
    AfficheGrille(Sender);
}
//---------------------------------------------------------------------------

//== S�lection d'un trap�ze ==================================================//
void __fastcall TForm1::SelectTrapeze(TObject *Sender,int effet)
{
	// Trace un trap�ze � partir des coordonn�es de la souris
    // => cherche le coin sup�rieur � partir de GrilU et GrilV
    int CoinX, CoinY;
    int PosU, PosV;
    if ((gTrapU->Text != "") && (gTrapV->Text != ""))
    		{  PosU = gTrapU->Text.ToInt();  PosV = gTrapV->Text.ToInt(); }
    else return;

    CoinX = (PosU - PosV) * ISO_WIDTH_HALF    + GrilleX;
    CoinY = (PosU + PosV) * ISO_HEIGHT_HALF   + GrilleY;

	POINT p; p.x = CoinX; p.y = CoinY;
    ImageGrille->Canvas->PenPos = p;
    ImageGrille->Canvas->Pen->Color = clRed;
//    ImageGrille->Canvas->Pen->Mode = pmMergePenNot;
    ImageGrille->Canvas->Pen->Width = 2;
    ImageGrille->Canvas->LineTo(p.x-ISO_WIDTH_HALF, p.y+ISO_HEIGHT_HALF);
    p.x-=ISO_WIDTH_HALF;  p.y+=ISO_HEIGHT_HALF;
    ImageGrille->Canvas->LineTo(p.x+ISO_WIDTH_HALF, p.y+ISO_HEIGHT_HALF);
    p.x+=ISO_WIDTH_HALF;  p.y+=ISO_HEIGHT_HALF;
    ImageGrille->Canvas->LineTo(p.x+ISO_WIDTH_HALF, p.y-ISO_HEIGHT_HALF);
    p.x+=ISO_WIDTH_HALF;  p.y-=ISO_HEIGHT_HALF;
    ImageGrille->Canvas->LineTo(p.x-ISO_WIDTH_HALF, p.y-ISO_HEIGHT_HALF);

    // retour � la normale
    ImageGrille->Canvas->Pen->Mode = pmCopy;
    ImageGrille->Canvas->Pen->Width = 1;
};
//----------------------------------------------------------------------------//

//== Autre S�lection d'un trap�ze ============================================//
void __fastcall TForm1::SelectTrapeze2(TObject *Sender,int Coul,int PosU,int PosV )
{
    int CoinX = (PosU - PosV) * ISO_WIDTH_HALF    + GrilleX;
    int CoinY = (PosU + PosV) * ISO_HEIGHT_HALF   + GrilleY;

	POINT p; p.x = CoinX; p.y = CoinY;
    ImageGrille->Canvas->PenPos = p;
    switch (Coul)
     {   case 1 : ImageGrille->Canvas->Pen->Color = clBlue;
     			  ImageGrille->Canvas->Pen->Width = 2;
                  ImageGrille->Canvas->Pen->Mode = pmMergePenNot; break;
         case 2 : ImageGrille->Canvas->Pen->Color = clYellow;
     			  ImageGrille->Canvas->Pen->Width = 2;
                  ImageGrille->Canvas->Pen->Mode = pmMergePenNot; break;
         case 3 : ImageGrille->Canvas->Pen->Color = clRed;
     			  ImageGrille->Canvas->Pen->Width = 1;
                  ImageGrille->Canvas->Pen->Mode = pmCopy; break;
     };
    ImageGrille->Canvas->LineTo(p.x-ISO_WIDTH_HALF, p.y+ISO_HEIGHT_HALF);
    p.x-=ISO_WIDTH_HALF;  p.y+=ISO_HEIGHT_HALF;
    ImageGrille->Canvas->LineTo(p.x+ISO_WIDTH_HALF, p.y+ISO_HEIGHT_HALF);
    p.x+=ISO_WIDTH_HALF;  p.y+=ISO_HEIGHT_HALF;
    ImageGrille->Canvas->LineTo(p.x+ISO_WIDTH_HALF, p.y-ISO_HEIGHT_HALF);
    p.x+=ISO_WIDTH_HALF;  p.y-=ISO_HEIGHT_HALF;
    ImageGrille->Canvas->LineTo(p.x-ISO_WIDTH_HALF, p.y-ISO_HEIGHT_HALF);

    // retour � la normale
    ImageGrille->Canvas->Pen->Mode = pmCopy;
    ImageGrille->Canvas->Pen->Width = 1;
};
//----------------------------------------------------------------------------//

//== Fiche Principale Over ===================================================//
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	// d�salloue la m�moire
	for (int i=0; i < NbTabTrap; i++) delete[] TabTrap[i];	// lignes
    delete[] TabTrap;										// colonnes
}
//----------------------------------------------------------------------------//


//== Bouton Trap�ze Principal ================================================//
void __fastcall TForm1::TrapBtnClick(TObject *Sender)
{
    if ((Etape == Etape_NOTHING) || (Etape == Etape_INFOTRAPEZE))
     {
	    Etape = Etape_TRAPEZEPRINCIPAL;
        Help->Caption ="S�lectionner le trap�ze principal sur la grille";
        gTrapNb->Text = NbSelectTrap;
     };

}
//---------------------------------------------------------------------------

//== Bouton Infos Trapeze ====================================================//
void __fastcall TForm1::InfoBtnClick(TObject *Sender)
{
     Etape = Etape_INFOTRAPEZE;
     Help->Caption ="S�lectionner un trap�ze sur la grille";
     gTrapNb->Text = NbSelectTrap;

     // initialise par d�faut �
     EditHauteur->Text = 0;
     ComboBoxSurface->ItemIndex = 0;
     CheckBoxMarcher->Checked = FALSE;
     ComboBoxCollide->ItemIndex = 0;
     CBDrawTrapezeKey->Checked = FALSE;
}
//---------------------------------------------------------------------------

// clAqua,  clDkGray, clGray, lLime, clLtGray, clMaroon, clNavy, clOlive, clRed, clSilver, clTeal, clWhite
//== Chgt de couleur de fond =================================================//
void __fastcall TForm1::FondPurple1Click(TObject *Sender)
{ ColorFond = clPurple; };
//---------------------------------------------------------------------------
void __fastcall TForm1::FondBlue1Click(TObject *Sender)
{ ColorFond = clBlue; };
//---------------------------------------------------------------------------
void __fastcall TForm1::FondPink1Click(TObject *Sender)
{ ColorFond = clFuchsia; };
//---------------------------------------------------------------------------
void __fastcall TForm1::FondYellow1Click(TObject *Sender)
{ ColorFond = clYellow; };
//---------------------------------------------------------------------------
void __fastcall TForm1::FondBlack1Click(TObject *Sender)
{ ColorFond = clBlack; };
//---------------------------------------------------------------------------
void __fastcall TForm1::FondGreen1Click(TObject *Sender)
{ ColorFond = clGreen; };
//---------------------------------------------------------------------------
void __fastcall TForm1::FondWhite1Click(TObject *Sender)
{ ColorFond = clWhite; };
//---------------------------------------------------------------------------
void __fastcall TForm1::FondOlive1Click(TObject *Sender)
{ ColorFond = clOlive; };
//---------------------------------------------------------------------------
void __fastcall TForm1::FondRed1Click(TObject *Sender)
{ ColorFond = clRed; };
//---------------------------------------------------------------------------

//== Ajoute des infos au dernier Trap�ze s�lectionn� =========================//
void __fastcall TForm1::InfosBtnClick(TObject *Sender)
{
    // Remplit les conditions
    TabTrap[TrapSelectU][TrapSelectV].Trap.Hauteur  = EditHauteur->Text.ToInt();
    TabTrap[TrapSelectU][TrapSelectV].Trap.Terrain = ComboBoxSurface->ItemIndex;
    TabTrap[TrapSelectU][TrapSelectV].Trap.Accessible = !CheckBoxMarcher->Checked;
    TabTrap[TrapSelectU][TrapSelectV].Trap.TrapCollide = ComboBoxCollide->ItemIndex;
    TabTrap[TrapSelectU][TrapSelectV].Trap.DrawTrapezekey = CBDrawTrapezeKey->Checked;

    // revient � l'�tape de choix du trap�ze � informer
    Etape == Etape_INFOTRAPEZE;
    PlaceObjBtn->Enabled = TRUE;
    TrapBtn->Enabled = TRUE;
    TrapBtn2->Enabled = TRUE;
    InfoBtn->Enabled = TRUE;
    InfosBtn->Enabled = FALSE;
}
//---------------------------------------------------------------------------

//== Action s�lectionn�e =====================================================//
void __fastcall TForm1::ComboBoxActionChange(TObject *Sender)
{
	// exemple :
//    ListBoxActions->Items->Add(ComboBoxAction->Text);

}
//---------------------------------------------------------------------------

//== Bouton Choix d'une couleur transparente =================================//
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	Help->Caption = "S�lectionner la couleur de fond sur le D�cors";
    Etape = Etape_COULTRANSPARENTE;
}
//---------------------------------------------------------------------------

//== Choix de la coul transparente ===========================================//
void __fastcall TForm1::ImaDecClick(TObject *Sender)
{

  if (Etape == Etape_COULTRANSPARENTE)
   {
	TColor c;
    c = ImaDec->Canvas->Pixels[DecorsX][DecorsY];
    CoulTransRVB = c;
    unsigned char c2;
    CompB->Text = (unsigned char)  CoulTransRVB;
    CompV->Text = (unsigned char) (CoulTransRVB >> 8);
    CompR->Text = (unsigned char) (CoulTransRVB >> 16);
    Etape = Etape_NOTHING;
    Help->Caption = "Penser � indiquer le n� de couleur";
   };
}
//---------------------------------------------------------------------------

//== Mvt de la souris sur l'image "D�cors" ===================================//
void __fastcall TForm1::ImaDecMouseMove(TObject *Sender, TShiftState Shift,
	int X, int Y)
{   DecorsX = X; DecorsY = Y; };
//---------------------------------------------------------------------------

//== Sauve l'image D�cors ====================================================//
void __fastcall TForm1::SauverDecors2Click(TObject *Sender)
{
    // Faudra v�rifier les erreurs sur la convertion de cha�nes
    // ...
    // V�rifie si Toutes les bo�tes de dialogue sont convenablement remplies
    if (Numero->Text=="")
    	{ MessageBoxA(NULL,"Num�ro d'identification non d�fini","Sauvegarde Impossible !",0); return; };
    if (ImageName->Text=="")
    	{ MessageBoxA(NULL,"Image de D�cors non choisie","Sauvegarde Impossible !",0); return; };
    if ( (CompR->Text=="") || (CompV->Text=="") || (CompB->Text==""))
    	{ MessageBoxA(NULL,"Couleur transparente non identifi�e ","Sauvegarde Impossible !",0); return; };
    if (Numero->Text=="")
    	{ MessageBoxA(NULL,"Aucun num�ro de rentr�","Sauvegarde Impossible !",0); return; };
    if (!DecorsPlace)
    	{ MessageBoxA(NULL,"Placer le d�cors sur la grille","Sauvegarde Impossible !",0); return; };
    if (NbSelectTrap == 0)
    	{ MessageBoxA(NULL,"Aucun trap�ze s�lectionn�","Sauvegarde Impossible !",0); return; };
    if (numPal->Text=="")
    if (MessageBoxA(NULL,"Le num�ro de couleur n'est pas d�fini,  voulez-vous continuer ?","Attention !",1)==IDNO) return;


    // Remplit les informations de DectTile
    DecTile.DecorsNum = Numero->Text.ToInt();
    strcpy(DecTile.Nom,ImageName->Text.c_str());
    DecTile.Width 	  = DecorsWidth->Text.ToInt();
    DecTile.Height 	  = DecorsHeight->Text.ToInt();
    DecTile.RVB 	  = CoulTransRVB;
    if (numPal->Text!="") DecTile.NumRVB = numPal->Text.ToInt(); else DecTile.NumRVB = 0;
    DecTile.NbTrapeze = NbSelectTrap;
    DecTile.TrapPrincX = TrapPrincX   - DecorsCornerX;
    DecTile.TrapPrincY = TrapPrincY   - DecorsCornerY;
    DecTile.DecorsCornerX = DecorsCornerX;
    DecTile.DecorsCornerY = DecorsCornerY;
    DecTile.TotalDraw = !CBAffichagePartiel->Checked;
    // s'occupe des trap�zes de la grille
    DecTile.TrapezTab = new Trapeze[NbSelectTrap];
    DecTile.TrapezTab[0] = TrapPrincipal;   // Convention : le [0] est le principal
    int Compt = 1;
    for (int u=0 ; u < NbTabTrap + 1 ; u++)       // scrute toute la grille
    	for (int v=0 ; v < NbTabTrap + 1 ; v++)
           if (TabTrap[u][v].Selectionne)
             {
              if (!TabTrap[u][v].Principal)
                     DecTile.TrapezTab[Compt++] = TabTrap[u][v].Trap;
              else if (TabTrap[u][v].Principal)
                     DecTile.TrapezTab[0] = TabTrap[u][v].Trap; };

    // Sauvegarde toute la structure TileDecors
    FILE *f;
    AnsiString yo = ficTravail + TileDecExt;
    char* fName = yo.c_str();
    if ((f = fopen(fName,"wb"))==NULL)
           MessageBoxA(NULL,"Impossible de sauver le fichier :",fName,1);
    fwrite(&TileDecVersion, sizeof(float), 1, f);
    fwrite(&DecTile.DecorsNum, sizeof(int), 1, f);
    fwrite(&DecTile.Nom, sizeof(DecTile.Nom), 1, f);
    fwrite(&DecTile.Width, sizeof(int), 1, f);
    fwrite(&DecTile.Height, sizeof(int), 1, f);
    fwrite(&DecTile.RVB, sizeof(unsigned int), 1, f);
    fwrite(&DecTile.NumRVB, sizeof(unsigned char), 1, f);
    fwrite(&DecTile.NbTrapeze, sizeof(int), 1, f);
    fwrite(&DecTile.TrapPrincX, sizeof(int), 1, f);
    fwrite(&DecTile.TrapPrincY, sizeof(int), 1, f);
    for (int i=0; i < NbSelectTrap; i++)
      {
        DecTile.TrapezTab[i].TrapU = 01;
        DecTile.TrapezTab[i].TrapV = 01;
        DecTile.TrapezTab[i].Hauteur = 0x22;
        DecTile.TrapezTab[i].Terrain = (int) 0x33333333;
        DecTile.TrapezTab[i].Accessible = (int) 0x44;
        DecTile.TrapezTab[i].TrapCollide = (int) 0x55;
        DecTile.TrapezTab[i].DrawTrapezekey = (int) 0x66;
        DecTile.TrapezTab[i].lamX1 = (int) 0x07070707;
        DecTile.TrapezTab[i].lamY1 = (int) 0x08080808;
        DecTile.TrapezTab[i].lamX2 = (int) 0x07070707;
        DecTile.TrapezTab[i].lamY2 = (int) 0x08080808;
        for (int j=0 ; j < 8 ; j++) DecTile.TrapezTab[i].reserved[j] = -2;
      	fwrite(&DecTile.TrapezTab[i], sizeof(Trapeze), 1, f);
      }
    fwrite(&DecTile.DecorsCornerX, sizeof(int), 1, f);
    fwrite(&DecTile.DecorsCornerY, sizeof(int), 1, f);
    fwrite(&DecTile.TotalDraw, sizeof(bool), 1, f);
    //--- Nouveau depuis la version 0.98 ---
    fwrite(&DecTile.CircleCollision, sizeof(DecTile.CircleCollision), 1, f);
    fwrite(&DecTile.CircleCenterX, sizeof(DecTile.CircleCenterX), 1, f);
    fwrite(&DecTile.CircleCenterY, sizeof(DecTile.CircleCenterY), 1, f);
    fwrite(&DecTile.CircleRadius, sizeof(DecTile.CircleRadius), 1, f);
    fwrite(&DecTile.DrawMethode, sizeof(DecTile.DrawMethode), 1, f);
    fwrite(&DecTile.reserved, sizeof(DecTile.reserved), 1, f);
    fclose(f);

    MessageBoxA(NULL,fName,"Sauvegarde R�ussie",0);
}
//---------------------------------------------------------------------------

//== Ouvrir un fichier Decors ================================================//
void __fastcall TForm1::Nou1Click(TObject *Sender)
{
    ResetAll(Sender);

    SelectDecorsDialog->InitialDir = CheminDecorsTile + "Tiles\\";
    SelectDecorsDialog->FilterIndex = 1;  // *.dec
    SelectDecorsDialog->Execute();
    AnsiString nom = SelectDecorsDialog->FileName;
    if (nom == "")
      { MessageBoxA(NULL,"Aucun fichier choisi","Warning !",0); return; };
    // nom du fichier de travail en cours
    ficTravail = nom;
    ficTravail.Delete(ficTravail.Pos("."),strlen(".dec"));

    // Charge toute la structure TileDecors
    FILE *f;
    if ((f = fopen(nom.c_str(),"rb"))==NULL)
           MessageBoxA(NULL,"Impossible de charger le fichier :",nom.c_str(),1);
    TileDecors *DecTile2;
    DecTile2 = new TileDecors;
    float DecVersion;
    fread(&DecVersion, sizeof(float), 1, f);
    if (DecVersion < TileDecVersion) { MessageBoxA(NULL,"Version ancienne de sauvegarde","Probl�me de lecture",1); fclose(f); return; };
    fread(&DecTile2->DecorsNum, sizeof(int), 1, f);
    fread(&DecTile2->Nom, sizeof(DecTile2->Nom), 1, f);
    fread(&DecTile2->Width, sizeof(int), 1, f);
    fread(&DecTile2->Height, sizeof(int), 1, f);
    fread(&DecTile2->RVB, sizeof(unsigned int), 1, f);
    fread(&DecTile2->NumRVB, sizeof(unsigned char), 1, f);
    fread(&DecTile2->NbTrapeze, sizeof(int), 1, f);
    fread(&DecTile2->TrapPrincX, sizeof(int), 1, f);
    fread(&DecTile2->TrapPrincY, sizeof(int), 1, f);
    DecTile2->TrapezTab = new Trapeze[DecTile2->NbTrapeze];
    for (int i=0; i < DecTile2->NbTrapeze; i++)
      fread(&DecTile2->TrapezTab[i], sizeof(Trapeze), 1, f);
    fread(&DecTile2->DecorsCornerX, sizeof(int), 1, f);
    fread(&DecTile2->DecorsCornerY, sizeof(int), 1, f);
    fread(&DecTile2->TotalDraw, sizeof(bool), 1, f);
    //--- Nouveau depuis la version 0.98 ---
    fread(&DecTile.CircleCollision, sizeof(DecTile.CircleCollision), 1, f);
    fread(&DecTile.CircleCenterX, sizeof(DecTile.CircleCenterX), 1, f);
    fread(&DecTile.CircleCenterY, sizeof(DecTile.CircleCenterY), 1, f);
    fread(&DecTile.CircleRadius, sizeof(DecTile.CircleRadius), 1, f);
    fread(&DecTile.DrawMethode, sizeof(DecTile.DrawMethode), 1, f);
    fread(&DecTile.reserved, sizeof(DecTile.reserved), 1, f);
    fclose(f); // fin de lecture du fichier

    // S'occupe de tout mettre � jour
    // bo�tes de dialogues
    Numero->Text = DecTile2->DecorsNum;
    numPal->Text = DecTile2->NumRVB;
    ImageName->Text = DecTile2->Nom;
    DecorsWidth->Text = DecTile2->Width;
    DecorsHeight->Text = DecTile2->Height;
    CompB->Text = (unsigned char)  DecTile2->RVB;
    CompV->Text = (unsigned char) (DecTile2->RVB >> 8);
    CompR->Text = (unsigned char) (DecTile2->RVB >> 16);
    gTrapNb->Text = DecTile2->NbTrapeze;
    CBZoneCirculaire->Checked = DecTile.CircleCollision;
    CBShadow->Checked = ((DecTile.DrawMethode & TILEDECORS_DRAW_SHADOW) == TILEDECORS_DRAW_SHADOW);

    // variables primordiales
    CoulTransRVB = DecTile2->RVB;
    NbSelectTrap = DecTile2->NbTrapeze;
    DecorsCornerX = DecTile2->DecorsCornerX;
    DecorsCornerY = DecTile2->DecorsCornerY;
    TrapPrincX = DecTile2->TrapPrincX + DecorsCornerX;
    TrapPrincY = DecTile2->TrapPrincY + DecorsCornerY;
    TrapPrincipal = DecTile2->TrapezTab[0];
    // Traveaux sur les images
    if (MonBitmap!=NULL) delete MonBitmap;
    MonBitmap=new Graphics::TBitmap;  		// Brush de l'image;
    AnsiString fic = ficTravail;
    fic += ".bmp";
    MonBitmap->LoadFromFile(fic);
    MonBitmap->Canvas->CopyMode = cmSrcCopy;
    ImaDec->Picture->LoadFromFile(fic); // Affiche l'image s�lectionn�

    // Tableau dynamique des trap�zes
    int RealX = TrapPrincX - GrilleX;
    int RealY = TrapPrincY - GrilleY;
    TrapPrincU = (RealX * ISO_HEIGHT_HALF + RealY * ISO_WIDTH_HALF) / (2 * ISO_HEIGHT_HALF * ISO_WIDTH_HALF);
    TrapPrincV = ( - RealX * ISO_HEIGHT_HALF + RealY * ISO_WIDTH_HALF) / (2 * ISO_HEIGHT_HALF * ISO_WIDTH_HALF);
    TabTrap[TrapPrincU][TrapPrincV].Trap = TrapPrincipal;
    TabTrap[TrapPrincU][TrapPrincV].Principal = TRUE;
    TabTrap[TrapPrincU][TrapPrincV].Selectionne = TRUE;
    for (int i=1; i < NbSelectTrap; i++)
      { int tu = TrapPrincU + DecTile2->TrapezTab[i].TrapU;
        int tv = TrapPrincV + DecTile2->TrapezTab[i].TrapV;
        TabTrap[tu][tv].Trap = DecTile2->TrapezTab[i];
        TabTrap[tu][tv].Principal = FALSE;
        TabTrap[tu][tv].Selectionne = TRUE;
      };

    // Derni�re mise � jour
    DecorsPlace = TRUE;
    Etape = Etape_NOTHING;
    Help->Caption = "";
    CBAffichagePartiel->Checked = !DecTile2->TotalDraw;     

    // lib�re the mem
    delete DecTile2->TrapezTab;
    delete DecTile2;
}
//---------------------------------------------------------------------------

//== Efface tout =============================================================//
void __fastcall TForm1::ResetAll(TObject *Sender)
{
    // pr�pare un tableau vierge
    for (int u=0 ; u < NbTabTrap + 1 ; u++)
    	for (int v=0 ; v < NbTabTrap + 1 ; v++)
          {  TabTrap[u][v].Selectionne = FALSE;
             TabTrap[u][v].Principal   = FALSE;
          };

    // bo�tes de dialogues vid�es
    ImageName->Text = ""; Numero->Text = "";
    DecorsWidth->Text =""; DecorsHeight->Text = "";
    numPal->Text = ""; CompR->Text = ""; CompV->Text =""; CompB->Text = "";
    gTrapNb->Text = "";
    NumEdit->Text = ""; EditTrapU->Text = ""; EditTrapV->Text = "";
    Help->Caption = "";
    CBZoneCirculaire->Checked = false;
    CBShadow->Checked = false;

    // Variables r�initialis�es
    DecorsPlace = FALSE;
    NbSelectTrap = 0;
    DecTile.DrawMethode = TILEDECORS_DRAW_OPAQUE;
};
//---------------------------------------------------------------------------

//== On vide Tout ============================================================//
void __fastcall TForm1::SauverDecors1Click(TObject *Sender)
{
    ImaDec->Picture = NULL;    // Un peu subite, mais bon ...
    ResetAll(Sender);
}
//---------------------------------------------------------------------------

//== Effaces la Grille =======================================================//
void __fastcall TForm1::TrapBtn2Click(TObject *Sender)
{
    // pr�pare un tableau vierge
    for (int u=0 ; u < NbTabTrap + 1 ; u++)
    	for (int v=0 ; v < NbTabTrap + 1 ; v++)
          {   TabTrap[u][v].Selectionne = FALSE;
              TabTrap[u][v].Principal   = FALSE;
          };
    NbSelectTrap = 0;
    Etape = Etape_NOTHING;
    gTrapNb->Text = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnZoomClick(TObject *Sender)
{
     Zooming->ImageGrilleZ->Width = ImageWidth*Zoom;
     Zooming->ImageGrilleZ->Height = ImageHeight*Zoom;
     TRect destR;
     destR.Top = 0; destR.Bottom = ImageHeight*Zoom;
     destR.Left = 0; destR.Right = ImageWidth*Zoom;

     Zooming->ImageGrilleZ->Canvas->Draw(0,0 , ImageGrille->Picture->Graphic);
     Zooming->Show();
}
//---------------------------------------------------------------------------

//== ZOOM x2, x4 & x6 ========================================================//
void __fastcall TForm1::SpdBtnx2Click(TObject *Sender)
{    Zoom = 2; BtnZoomClick(Sender);
}
//----------------------------------------------------------------------------//
void __fastcall TForm1::SpdBtnx4Click(TObject *Sender)
{ Zoom = 4; BtnZoomClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpdBtnx6Click(TObject *Sender)
{ Zoom = 6; BtnZoomClick(Sender);
}
//---------------------------------------------------------------------------



//=== Permet la s�lection d'une zone du sprite � l'�cran =====================//
void __fastcall TForm1::SelectZoneBtnClick(TObject *Sender)
{ // V�rifie si l' "affichage partiel" a bien �t� coch�
  if ( (!CBAffichagePartiel->Checked) || (!CBDrawTrapezeKey->Checked))
  { MessageBoxA(NULL,"Affichage partiel du sprite non activ�","Warning !",0);
    return;
  };
  // V�rifie que l'on ne soit pas dans un autre mode d'�tape
  if ((Etape!=Etape_NOTHING) && (Etape != Etape_INFOTRAPEZE))
  { MessageBoxA(NULL,"Vous ne pouvez pas faire 36 choses en m�me temps","Warning !",0);
    return;
  };
  Help->Caption ="S�lectionner une lamelle verticale de sprite qui sera affich�e pour ce trap�ze ci";
  Etape = Etape_SELECTZONE;          // Nouvelle �tape
}
//----------------------------------------------------------------------------//

//=== On s�lectionne le 1er point de la lamelle ==============================//
void __fastcall TForm1::ImageGrilleMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{  // Si l'on doit s�lectionner une zone du sprite
   if (Etape == Etape_SELECTZONE)
   { // Capture le point en haut � gauche
     ZoneX1 = X;
     ZoneY1 = Y;
     // Puis r�ajuste ces coords si d�bordement
     if (X < DecorsCornerX)
         {  ZoneX1 = DecorsCornerX; }
     if (X >= DecorsCornerX + DecorsWidth->Text.ToInt())
         {  ZoneX1 = DecorsCornerX + DecorsWidth->Text.ToInt()-1; }
     if (Y < DecorsCornerY)
         {  ZoneY1 = DecorsCornerY; }
     if (Y >= DecorsCornerY + DecorsHeight->Text.ToInt())
         {  ZoneY1 = DecorsCornerY + DecorsHeight->Text.ToInt()-1; }
   }
   else  // On doit s�lectionner un Cercle autour du sprite
   if (Etape == Etape_SELECTZONECERCLE)
   {  circleCenterX = gCoordX->Text.ToInt();
      circleCenterY = gCoordY->Text.ToInt();
      circleRadius = 0;
   }
}
//----------------------------------------------------------------------------//

//=== On s�lectionne le 2nd point de la lamelle ==============================//
void __fastcall TForm1::ImageGrilleMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{ if (Etape == Etape_SELECTZONE)
  {  // Capture le point en bas � droite
     ZoneX2 = X;
     ZoneY2 = Y;
     // Puis r�ajuste ces coords si d�bordement
     if (X < DecorsCornerX)
         {  ZoneX2 = DecorsCornerX; }
     if (X >= DecorsCornerX + DecorsWidth->Text.ToInt())
         {  ZoneX2 = DecorsCornerX + DecorsWidth->Text.ToInt()-1; }
     if (Y < DecorsCornerY)
         {  ZoneY2 = DecorsCornerY; }
     if (Y >= DecorsCornerY + DecorsHeight->Text.ToInt())
         {  ZoneY2 = DecorsCornerY + DecorsHeight->Text.ToInt()-1; }
     // Changement d'�tape
     Etape = Etape_NOTHING;
     // Choppe les points de la lamelle verticale
     TabTrap[TrapSelectU][TrapSelectV].Trap.lamX1 = ZoneX1 - DecorsCornerX;
     TabTrap[TrapSelectU][TrapSelectV].Trap.lamY1 = ZoneY1 - DecorsCornerY;
     TabTrap[TrapSelectU][TrapSelectV].Trap.lamX2 = ZoneX2 - DecorsCornerX;
     TabTrap[TrapSelectU][TrapSelectV].Trap.lamY2 = ZoneY2 - DecorsCornerY;
     TabTrap[TrapSelectU][TrapSelectV].Trap.DrawTrapezekey = true;
     ZoneX1 = ZoneY1 = 0;
  }
  else  // On doit s�lectionner un Cercle autour du sprite
  if (Etape == Etape_SELECTZONECERCLE)
  {  if ((circleCenterX!=-1) && (circleCenterY!=-1))
     {  circleRadius = sqrt((X-circleCenterX)*(X-circleCenterX)
                       +(Y-circleCenterY)*(Y-circleCenterY));    // Calcul du rayon du cercle
        DrawCircle(circleCenterX, circleCenterY, circleRadius, 0x0505DD);
        DecTile.CircleCenterX = circleCenterX;
        DecTile.CircleCenterY = circleCenterY;
        DecTile.CircleRadius = circleRadius;
        Etape = Etape_NOTHING;  // Changement d'�tape
     }
  }
}
//----------------------------------------------------------------------------//


//=== Reg�n�re les coords principales du d�cors ==============================//
void __fastcall TForm1::BtnReInitClick(TObject *Sender)
{   TrapPrincX = (TrapPrincU - TrapPrincV) * ISO_WIDTH_HALF    + GrilleX;
    TrapPrincY = (TrapPrincU + TrapPrincV) * ISO_HEIGHT_HALF   + GrilleY;
}
//----------------------------------------------------------------------------//

//=== C'est parti pour prendre un cercle =====================================//
void __fastcall TForm1::BtnPrendreCercleClick(TObject *Sender)
{   Etape = Etape_SELECTZONECERCLE;
    circleCenterX = circleCenterY = -1;
    circleRadius = 0;
}
//----------------------------------------------------------------------------//

//=== Dessine un cercle sur l'�cran ==========================================//
void __fastcall TForm1::DrawCircle(int x, int y, int rayon, int color)
{   ImageGrille->Canvas->Pen->Color = (TColor) color;
    ImageGrille->Canvas->Pen->Mode = pmCopy;
    ImageGrille->Canvas->Brush->Style = bsClear;     // cercle non remplit != disque
    ImageGrille->Canvas->Ellipse(x-rayon, y-rayon, x+rayon, y+rayon);

}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//

//=== ComboBox de la zone circulaire =========================================//
void __fastcall TForm1::CBZoneCirculaireClick(TObject *Sender)
{  DecTile.CircleCollision = CBZoneCirculaire->Checked;
   BtnPrendreCercle->Enabled = DecTile.CircleCollision;
}
//----------------------------------------------------------------------------//

//=== S�lection/D�selection de l'ombrage =====================================//
void __fastcall TForm1::CBShadowClick(TObject *Sender)
{	if (CBZoneCirculaire->Checked) DecTile.DrawMethode = TILEDECORS_DRAW_SHADOW;
	else DecTile.DrawMethode ^= TILEDECORS_DRAW_SHADOW;
}
//----------------------------------------------------------------------------//

