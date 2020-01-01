//---------------------------------------------------------------------------
#ifndef fAnimViewH
#define fAnimViewH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>

#include "DDLib.h"
#include "ImDecors2.h"
#include "SpritesLib.h"

//---------------------------------------------------------------------------
class TAnimView : public TForm
{
__published:	// Composants g�r�s par l'EDI
	TTimer *TimerFlip;
	TScrollBar *ScrollViewVert;
	TScrollBar *ScrollViewHor;
	void __fastcall TimerFlipTimer(TObject *Sender);
	
	void __fastcall ScrollViewVertScroll(TObject *Sender, TScrollCode ScrollCode,
	int &ScrollPos);
	void __fastcall ScrollViewHorScroll(TObject *Sender, TScrollCode ScrollCode,
	int &ScrollPos);
        
        
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        
        
        
        void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        
        
        
	
private:	// D�clarations de l'utilisateur
public:		// D�clarations de l'utilisateur
	__fastcall TAnimView(TComponent* Owner);
    void __fastcall UpGradeScreen();	      // effectue quelques petits changements lors du rafra�chissement

    void __fastcall RecadreView();            // recadre la vue en tenant compte du format de l'animation en cours

    int RefreshFreq;			      // Taux de rafra�chissement de la fen�tre
    int ScrollHor , ScrollVert;		      // D�calage hor & vertic du scrolling};

    // D�finitions & Positions des vues, sprites ...
    float Zoom;				      // valeur du zoom
    int animWidth, animHeight;                // Dimensions de l'animation en cours
    int viewWidth, viewHeight;                // Dimensions de la vue
    int ScreenX, ScreenY;		      // Position du bord sup�rieur-droit de l'�cran ds cette vue
    int spriteX, spriteY;                     // Position du bord sup�rieur-droit du sprite ds cette vue

    // Position de la souris
    int mouseScreenX, mouseScreenY;           // Position de la souris sur la fiche
    int mouseGravX, mouseGravY;               // Position de la souris dans la vue
    void __fastcall MouseInfos();             // Calcule la position de la souris

    // Infos que les sprites visibles � l'�cran
    void __fastcall SpriteInfos();            // Remplit la vue avec toutes les infos du sprite disponibles

    enum eworkMode { RECTANGLE, RECTANGLE_ORIENTE, POINT, SPHERE, NONE };
    eworkMode workMode;                       // Indique ce que l'on peut faire avec la souris
    enum eworkZone { ZONESOL, PT_GRAV_SOL, PT_PIED_G1, PT_PIED_G2, PT_PIED_D1, PT_PIED_D2, ZONEPIED, AUCUN };
    eworkZone workZone;                       // Indique quelle zone du perso on est entrain de pr�lever
    int P1x, P1y, P2x, P2y, Rayon;            // Points utilis�s pour dessiner une zone

};
//---------------------------------------------------------------------------

extern TAnimView *AnimView;

extern int ScrollViewRate; // Frame rate du rafraichissement de la fen�tre d'animation
extern int eScreenViewX, eScreenViewY;			// R�solution globale de la vue directdraw
extern int eScreenVisibleX, eScreenVisibleY;	// Fen�tre directdraw visible

// Nouvelle routine d'affichage de sprites expr�s pour th �ditor ///////////////
extern void SpriteDrawEditor(int ScX,int ScY,double Zoom,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso);
extern void SpriteDecorsDrawEditor(int scX2,int scY2,double Zoom, DDSurface *pSurface,DecorsObjet *drawObj);
extern void TileDecorsDrawEditor(int scX2,int scY2,double Zoom,TileDecors* tileDec,DDSurface *pSurface);
extern void ClassikSpriteDrawEditor(int scX2,int scY2,double Zoom,int NumSprite, DDSurface *pSurface,SpriteTileGameLib *stgl);
//---------------------------------------------------------------------------
#endif
