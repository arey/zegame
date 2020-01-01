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
__published:	// Composants gérés par l'EDI
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
        
        
        
	
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TAnimView(TComponent* Owner);
    void __fastcall UpGradeScreen();	      // effectue quelques petits changements lors du rafraîchissement

    void __fastcall RecadreView();            // recadre la vue en tenant compte du format de l'animation en cours

    int RefreshFreq;			      // Taux de rafraîchissement de la fenêtre
    int ScrollHor , ScrollVert;		      // Décalage hor & vertic du scrolling};

    // Définitions & Positions des vues, sprites ...
    float Zoom;				      // valeur du zoom
    int animWidth, animHeight;                // Dimensions de l'animation en cours
    int viewWidth, viewHeight;                // Dimensions de la vue
    int ScreenX, ScreenY;		      // Position du bord supérieur-droit de l'écran ds cette vue
    int spriteX, spriteY;                     // Position du bord supérieur-droit du sprite ds cette vue

    // Position de la souris
    int mouseScreenX, mouseScreenY;           // Position de la souris sur la fiche
    int mouseGravX, mouseGravY;               // Position de la souris dans la vue
    void __fastcall MouseInfos();             // Calcule la position de la souris

    // Infos que les sprites visibles à l'écran
    void __fastcall SpriteInfos();            // Remplit la vue avec toutes les infos du sprite disponibles

    enum eworkMode { RECTANGLE, RECTANGLE_ORIENTE, POINT, SPHERE, NONE };
    eworkMode workMode;                       // Indique ce que l'on peut faire avec la souris
    enum eworkZone { ZONESOL, PT_GRAV_SOL, PT_PIED_G1, PT_PIED_G2, PT_PIED_D1, PT_PIED_D2, ZONEPIED, AUCUN };
    eworkZone workZone;                       // Indique quelle zone du perso on est entrain de prélever
    int P1x, P1y, P2x, P2y, Rayon;            // Points utilisés pour dessiner une zone

};
//---------------------------------------------------------------------------

extern TAnimView *AnimView;

extern int ScrollViewRate; // Frame rate du rafraichissement de la fenêtre d'animation
extern int eScreenViewX, eScreenViewY;			// Résolution globale de la vue directdraw
extern int eScreenVisibleX, eScreenVisibleY;	// Fenêtre directdraw visible

// Nouvelle routine d'affichage de sprites exprés pour th éditor ///////////////
extern void SpriteDrawEditor(int ScX,int ScY,double Zoom,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso);
extern void SpriteDecorsDrawEditor(int scX2,int scY2,double Zoom, DDSurface *pSurface,DecorsObjet *drawObj);
extern void TileDecorsDrawEditor(int scX2,int scY2,double Zoom,TileDecors* tileDec,DDSurface *pSurface);
extern void ClassikSpriteDrawEditor(int scX2,int scY2,double Zoom,int NumSprite, DDSurface *pSurface,SpriteTileGameLib *stgl);
//---------------------------------------------------------------------------
#endif
