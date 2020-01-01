//---------------------------------------------------------------------------
#ifndef fMenuIDPathH
#define fMenuIDPathH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\FileCtrl.hpp>
#include <vcl\Buttons.hpp>
//---------------------------------------------------------------------------
class TFormPath : public TForm
{
__published:	// Composants g�r�s par l'EDI
	TDirectoryListBox *DirectoryListBox;
	TDriveComboBox *DriveComboBox1;
	TBitBtn *BitBtnOK;
	TBitBtn *BitBtnAnnuler;
	TLabel *Label1;
	TLabel *PathLabel;
	TEdit *EditNewRep;
	TLabel *Label2;
	TBitBtn *BitBtnNewRep;
	void __fastcall BitBtnNewRepClick(TObject *Sender);
	
private:	// D�clarations de l'utilisateur
public:		// D�clarations de l'utilisateur
	__fastcall TFormPath(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TFormPath *FormPath;
//---------------------------------------------------------------------------
#endif
