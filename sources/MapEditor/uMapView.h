//---------------------------------------------------------------------------
#ifndef uMapViewH
#define uMapViewH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Menus.hpp>
//---------------------------------------------------------------------------

class TMapView : public TForm
{
__published:	// Composants g�r�s par l'EDI
	TScrollBar *ScrollViewHor;
	TScrollBar *ScrollViewVert;
	TTimer *TimerFlip;
	TPopupMenu *PopupMenuMapView;
	TMenuItem *PoserObjet;
	TMenuItem *SlectionnerObjet;
	TMenuItem *SupprimerObjet;
	void __fastcall FormClick(TObject *Sender);

	void __fastcall ScrollViewHorScroll(TObject *Sender, TScrollCode ScrollCode,
	int &ScrollPos);
	void __fastcall ScrollViewVertScroll(TObject *Sender, TScrollCode ScrollCode,
	int &ScrollPos);
	void __fastcall TimerFlipTimer(TObject *Sender);

	
	void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	
	
	void __fastcall PoserObjetClick(TObject *Sender);
	void __fastcall SlectionnerObjetClick(TObject *Sender);
	void __fastcall SupprimerObjetClick(TObject *Sender);
	
	void __fastcall FormDblClick(TObject *Sender);
	void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
	void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y);
        
	
private:	// D�clarations de l'utilisateur
public:		// D�clarations de l'utilisateur
	__fastcall TMapView(TComponent* Owner);
    void __fastcall UpGradeScreen();			// effectue quelques petits changements
    void __fastcall ChangeViewScreen();			// change la vue d'�cran : plein-�cran / portion
    int ScrollHor , ScrollVert;					// D�calage hor & vertic du scrolling
    int LastMouseViewX, LastMouseViewY;			// derni�re position de la souris en mode Scrolling
    BOOL MIDDLE_SCROLLING;						// Indique si le btn du milieu est enfonc� ou non
    int RefreshFreq;						 	// Taux de rafra�chissement de la fen�tre
};
//---------------------------------------------------------------------------
extern TMapView *MapView;
extern int ScrollViewRate;							// Scroll Rate des d�filements

//--- Fonctions d'affichage -----
void DrawViewTrap(int U, int V, int Col, int Type);
void DrawViewText(int U, int V, int Col, LPCTSTR Texte);
void DrawCircle(double x, double y, double rayon, int Col);
void DrawRectangle(int tab[4][2], int Col);

//--- Fonctions de convertion suppl�mentaires ---
// -> import�es de PersoLib.h
void ConvNivXY2UV(double X, double Y, double &U, double &V);
void ConvNivUV2XY(double U, double V, double &X, double &Y);
void ConvNivUVtoPeXY(double U, double V, int &pex, int &pey);
void ConvNivPeXYtoUV(int pex, int pey, double &U, double &V);
void ConvNivXYtoAll(double X, double Y, double &U, double &V, int &pex, int &pey);
void ConvNivUVtoAll(double U, double V, double &X, double &Y, int &pex, int &pey);
void TransUVtoXY(int U, int V, int *X, int *Y);
// -> Implant�es sp�cialement depuis les Zones au sol
void ConvPeXYtoNivXY(int pex, int pey, double &x, double &y);
void ConvNivXYtoPeXY(double x, double y, int &pex, int &pey);

//---------------------------------------------------------------------------
#endif
