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
__published:	// Composants gérés par l'EDI
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
	
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TFormPath(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TFormPath *FormPath;
//---------------------------------------------------------------------------
#endif
