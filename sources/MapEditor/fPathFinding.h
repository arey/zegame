//---------------------------------------------------------------------------
#ifndef fPathFindingH
#define fPathFindingH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Buttons.hpp>

#include "FPath.h"                   // procédures de Path Finding

//---------------------------------------------------------------------------
class TPathFinding : public TForm
{
__published:	// Composants gérés par l'EDI
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
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
        __fastcall TPathFinding(TComponent* Owner);

        void __fastcall SendCaseCoord(int cU, int cV);
        void __fastcall InitPF();

        enum ePFPoint { PF_DEPART, PF_ARRIVEE };  // 2 types de points : départ & arrivée
        ePFPoint PFPoint;                         // Point à sélectionner
        int depU,depV, arrU, arrV;                // Coordonnées de ces 2 points
        bool pfDep, pfArr;                        // Les points ont-ils été saisis ?
        PathVector   CheminTrouve;                // Chemin à trouver
        PFStatistics PFStats;                     // Statistiques résultantes
        bool pfChemTrouve;                        // Indique que l'on peut afficher ce chemin
};
//---------------------------------------------------------------------------
extern TPathFinding *PathFinding;
//---------------------------------------------------------------------------
#endif
