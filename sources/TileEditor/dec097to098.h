//---------------------------------------------------------------------------
#ifndef dec097to098H
#define dec097to098H
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\FileCtrl.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// Composants gérés par l'EDI
        TButton *UpgradeBtn;
        TDirectoryListBox *DirectoryListBox;
        TFileListBox *DecListBox;
        TLabel *Label1;
        TLabel *NbDecLabel;
        TLabel *Label2;
        TLabel *NumLabel;
        void __fastcall UpgradeBtnClick(TObject *Sender);
        void __fastcall Convertion(); 
        
        
        
        void __fastcall DirectoryListBoxChange(TObject *Sender);
	
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
