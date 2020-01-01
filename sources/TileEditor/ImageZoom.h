//---------------------------------------------------------------------------
#ifndef ImageZoomH
#define ImageZoomH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TZooming : public TForm
{
__published:	// Composants gérés par l'EDI
        TImage *ImageGrilleZ;
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
        __fastcall TZooming(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TZooming *Zooming;
//---------------------------------------------------------------------------
#endif
