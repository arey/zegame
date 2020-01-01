//---------------------------------------------------------------------------
#ifndef fPathFindingH
#define fPathFindingH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Buttons.hpp>

#include "FPath.h"                   // proc�dures de Path Finding

//---------------------------------------------------------------------------
class TPathFinding : public TForm
{
__published:	// Composants g�r�s par l'EDI
        TBitBtn *DepartBtn;
        TBitBtn *ArriveeBtn;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *depULabel;
        TLabel *depVLabel;
        TLabel *arrULabel;
        TLabel *arrVLabel;
        TBitBtn *FindPathBtn;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *LongueurLabel;
        TLabel *CoutLabel;
        TLabel *NODES;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *RechercheLabel;
        TLabel *AjouteLabel;
        TLabel *VisiteLabel;
        TLabel *Label10;
        TLabel *RealLongLabel;
        TLabel *Label12;
        TCheckBox *CBComplet;
        void __fastcall DepartBtnClick(TObject *Sender);
        void __fastcall ArriveeBtnClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FindPathBtnClick(TObject *Sender);
        void __fastcall CBCompletClick(TObject *Sender);
private:	// D�clarations de l'utilisateur
public:		// D�clarations de l'utilisateur
        __fastcall TPathFinding(TComponent* Owner);

        void __fastcall SendCaseCoord(int cU, int cV);
        void __fastcall InitPF();

        enum ePFPoint { PF_DEPART, PF_ARRIVEE };  // 2 types de points : d�part & arriv�e
        ePFPoint PFPoint;                         // Point � s�lectionner
        int depU,depV, arrU, arrV;                // Coordonn�es de ces 2 points
        bool pfDep, pfArr;                        // Les points ont-ils �t� saisis ?
        PathVector   CheminTrouve;                // Chemin � trouver
        PFStatistics PFStats;                     // Statistiques r�sultantes
        bool pfChemTrouve;                        // Indique que l'on peut afficher ce chemin
};
//---------------------------------------------------------------------------
extern TPathFinding *PathFinding;
//---------------------------------------------------------------------------
#endif
