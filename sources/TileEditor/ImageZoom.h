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
__published:	// Composants g�r�s par l'EDI
        TImage *ImageGrilleZ;
private:	// D�clarations de l'utilisateur
public:		// D�clarations de l'utilisateur
        __fastcall TZooming(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TZooming *Zooming;
//---------------------------------------------------------------------------
#endif
