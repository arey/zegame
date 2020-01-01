//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MapLib.h"				// acc�s � MapNiveau
#include "MapZone.h"			// classes des zones au sol
#include "fLibOutils.h"			// pour avoir acc�s au genre de design
#include "fIDOutils.h"			// fen�tre de s�lection/cr�ation d'ID
#include "consts.h"				// �a peut �tre utile les constantes
#include "ScnScript.h"			// Gestion des scripts sc�naris�s
#include "fDesignOutils.h"  	// son en-t�te
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormDesignOutils *FormDesignOutils;

//=== Constructeur ===========================================================//
__fastcall TFormDesignOutils::TFormDesignOutils(TComponent* Owner)
	: TForm(Owner)
{   Top = 211; Left = 664;
    Width = 350; Height = 480;
    WindowState = wsMinimized; 		// par d�faut
    iTypeDesignLevel = DESIGN_LEVEL_NONE;
    iModeDesignLevel = DESIGN_LEVEL_MODE_NONE;
    // Au d�part, aucune zone de s�lectionn�e
    iObjSelected = -1;
   	pMapZone = NULL;
}
//----------------------------------------------------------------------------//

//=== MaJ de la barre de LevelDesign =========================================//
void __fastcall TFormDesignOutils::UpdateView()
{
    // - Si aucune zone n'est s�lectionn�e, on s�lectionne la 1�re
    if ((iObjSelected == -1) && (MapNiveau!=NULL) && (MapNiveau->pMapZoneList->GetNbZones() != 0))
    {	// Recherche de la zone pr�c�dente
        MapZone* zone = MapNiveau->pMapZoneList->GetFirstZone();
        if (zone == NULL) return;
        // Nouvelle zone s�lectionn�e
        pMapZone = zone;
        iObjSelected = pMapZone->GetID();
        NameElement->Caption = IDMapZones->CleID(iObjSelected);
        NumElement->Caption = iObjSelected;
    }

    // - MaJ des Labels de la fiche
	if (LibOutils->RBZones->Checked)
	{	LabelDesignNumber->Caption = "n� de  la zone";
    	LabelBarreTitle->Caption =  "Zone";
        iTypeDesignLevel = DESIGN_LEVEL_ZONES;

        if (iObjSelected != -1)
        {	NameElement->Caption = pMapZone->GetName();
			NumElement->Caption = pMapZone->GetID();
            NumeroZone->Caption = pMapZone->GetID();
           	NomZone->Caption = IDMapZones->CleID(pMapZone->GetID());
        } else
        {	NameElement->Caption = "";
			NumElement->Caption = "";
            NumeroZone->Caption = "";
            NomZone->Caption = "";
        }
	} else
	if (LibOutils->RBChemins->Checked)
    {	LabelDesignNumber->Caption = "n� du chemin";
    	LabelBarreTitle->Caption =  "Chemin";
	    iTypeDesignLevel = DESIGN_LEVEL_CHEMINS;
    } else
	{	LabelDesignNumber->Caption = "n�";
    	LabelBarreTitle->Caption =  "Element";
        iTypeDesignLevel = DESIGN_LEVEL_NONE;
    }

    // - Compl�ment d'information sur la zone s�lectionn�e
    UpdateZoneInfos();
	// - MaJ du panneau des contr�les
    UpdateControles();
    UpdateListeObjets();
}
//----------------------------------------------------------------------------//

//=== Cr�ation d'une nouvelle zone ===========================================//
void __fastcall TFormDesignOutils::BtnNewZoneClick(TObject *Sender)
{	if (!Editeur->levelLoaded) return;

	// Si on vient juste d'en cr�er on se casse
    if ((iModeDesignLevel  & DESIGN_LEVEL_MODE_ZONES_CREATION) == DESIGN_LEVEL_MODE_ZONES_CREATION) return;

	// On cr�e une zone suivant le type s�lectionn�
	iModeDesignLevel = DESIGN_LEVEL_MODE_ZONES_CREATION;
	StatusBarLevelDesign->SimpleText = "Nouvelle zone en cours de cr�ation";
    iObjSelected = -1;

    // Zone Rectangulaire
	if (RBZoneRect->Checked)
    {	pMapZone = new MapZoneRect();
    } else
    // Zone Circulaire
   	if (RBZoneCircle->Checked)
    {	pMapZone = new MapZoneCircle();
    }

    BtnCreeZoneAnnuler->Enabled = true;
    BitBtnChooseGameObjectID->Enabled = true;
    BitBtnTracerZone->Enabled = true;
    BtnNewZone->Enabled = false;
}
//----------------------------------------------------------------------------//

//=== Placer une zone sur la carte ===========================================//
void __fastcall TFormDesignOutils::BitBtnTracerZoneClick(TObject *Sender)
{   iModeDesignLevel |= DESIGN_LEVEL_MODE_ZONES_TRACER;
	StatusBarLevelDesign->SimpleText = "Placer la zone sur la carte";
    if (ViewMap!=NULL) ViewMap->Pointeur = Pointeur_PLACERZONE;
}
//----------------------------------------------------------------------------//

//=== Affiche les informations de la zone s�lectionn�e =======================//
void __fastcall TFormDesignOutils::UpdateZoneInfos()
{	if (pMapZone != NULL)
    {	MapZoneCircle* circle = NULL;
    	MapZoneRect* rect = NULL;
    	double x, y;

    	switch (pMapZone->GetType())
	    {	case (MAPZONE_TYPE_CIRCLE):
				LabelType->Caption = "Cercle";
                circle = (MapZoneCircle*) pMapZone;
                if (circle->GetRadius() != -1)
                {   // - Rayon du cercle
                	LabelParam1->Caption = "Rayon : ";
                	LabelParam1Text->Caption = (float) ((int) (circle->GetRadius()*100)) /100;
                    // - Centre du cercle
                    circle->GetCenter(x, y);
                    LabelParam3->Caption = "Centre X : ";
                   	LabelParam3Text->Caption = (float) ((int) (x*1000)) /1000;
                    LabelParam4->Caption = "Centre Y : ";
                	LabelParam4Text->Caption = (float) ((int) (y*1000)) /1000;
                }
            	break;
	        case (MAPZONE_TYPE_RECT):
				LabelType->Caption = "Rectangle";
                rect = (MapZoneRect*) pMapZone;
                if (rect->GetWidth() != 0)
                {	// - Origine du rectangle
                   	LabelParam1->Caption = "Origine X : ";
                	LabelParam1Text->Caption = (float) ((int) (rect->GetOrigineX()*100)) /100;
                   	LabelParam2->Caption = "Origine Y : ";
                	LabelParam2Text->Caption = (float) ((int) (rect->GetOrigineY()*100)) /100;
					// - Dimensions
                    LabelParam3->Caption = "Longueur : ";
                   	LabelParam3Text->Caption = (float) ((int) (rect->GetWidth()*1000)) /1000;
                    LabelParam4->Caption = "hauteur : ";
                	LabelParam4Text->Caption = (float) ((int) (rect->GetHeight()*1000)) /1000;
                }
            	break;
            default:
            	LabelType->Caption = "";
        }
    }
}
//----------------------------------------------------------------------------//

//=== Annule la cr�ation d'une nouvelle Zone =================================//
void __fastcall TFormDesignOutils::BtnCreeZoneAnnulerClick(TObject *Sender)
{	// V�rifie si une zone a bien �t� cr�e
    if (((iModeDesignLevel & DESIGN_LEVEL_MODE_ZONES_CREATION) != DESIGN_LEVEL_MODE_ZONES_CREATION) || (pMapZone == NULL)) return;

    // On supprime la zone
    ViewMap->ObjZoneSelect = FALSE;
    iObjSelected = -1;
    delete pMapZone;  pMapZone = NULL;

    // Vide tous les champs de texte
    LabelType->Caption = "";
   	LabelParam1->Caption = "";
   	LabelParam1Text->Caption = "";
   	LabelParam2->Caption = "";
    LabelParam2Text->Caption = "";
	LabelParam3->Caption = "";
    LabelParam3Text->Caption = "";
    LabelParam4->Caption = "";
    LabelParam4Text->Caption = "";
    CBTracerZone->Checked = false;
    CBChoisirNumero->Checked = false;    

    // Si aucune zone n'est s�lectionn�e, on s�lectionne la 1�re
    if ((iObjSelected == -1) && (MapNiveau!=NULL) && (MapNiveau->pMapZoneList->GetNbZones() != 0))
    {	// Recherche de la 1�re zone
        MapZone* zone = MapNiveau->pMapZoneList->GetFirstZone();
        if (zone != NULL)
        {   // Nouvelle zone s�lectionn�e
        	pMapZone = zone;
        	iObjSelected = pMapZone->GetID();
        	NameElement->Caption = IDMapZones->CleID(iObjSelected);
        	NumElement->Caption = iObjSelected;
        }
    }

    if (ViewMap!=NULL) ViewMap->Pointeur = Pointeur_NOTHING;
    iModeDesignLevel = DESIGN_LEVEL_MODE_NONE;
    BtnCreeZoneAnnuler->Enabled = false;
    BitBtnChooseGameObjectID->Enabled = false;
    BitBtnTracerZone->Enabled = false;
    BtnNewZone->Enabled = true;
}
//----------------------------------------------------------------------------//

//=== R�cup�re un ID � associer � la zone ====================================//
void __fastcall TFormDesignOutils::BitBtnChooseGameObjectIDClick(
      TObject *Sender)
{   if (!Editeur->levelLoaded) return;

	IDOutils->LetsGo(IDMapZones);
    int numIDRecup = IDOutils->GetIDSelect();

    // V�rifie si l'on n'a pas d�j� utilis� cet ID pour une autre zone
    if ((numIDRecup != -1) && (MapNiveau->pMapZoneList->IsZone(numIDRecup)))
    {	ViewMap->ObjZoneSelect = FALSE;
		StatusBarLevelDesign->SimpleText = "N� zone d�j� utilis�";
        numIDRecup = -1;    	
    }

    if (numIDRecup != -1)
    {   NumeroZone->Caption = numIDRecup;
    	NomZone->Caption = IDMapZones->CleID(numIDRecup);
        CBChoisirNumero->Checked = true;
        // MaJ de l'ID de la zone
        iObjSelected = numIDRecup;
        pMapZone->SetID(numIDRecup);
        pMapZone->SetName(IDMapZones->CleID(numIDRecup).c_str());
        StatusBarLevelDesign->SimpleText = "N� de la zone s�lectionn�e";
        // V�rifie si l'on ne peut pas enable le btn "Cr�er" zone
        if (CBTracerZone->Checked && CBChoisirNumero->Checked)
        {	BtnCreerZone->Enabled = true;
        }
        ViewMap->ObjZoneSelect = TRUE;
	} else
	{	ViewMap->ObjZoneSelect = FALSE;
    }
}
//----------------------------------------------------------------------------//

//=== Zone au sol ajout�e � la liste =========================================//
void __fastcall TFormDesignOutils::BtnCreerZoneClick(TObject *Sender)
{	// Ajoute la zone nouvellement cr�e � la liste des zones au sol
	MapNiveau->pMapZoneList->AddZone(pMapZone);
	// MaJ de l'�tat interne des variables
    if (ViewMap!=NULL) ViewMap->Pointeur = Pointeur_NOTHING;
    iModeDesignLevel = DESIGN_LEVEL_MODE_NONE;
	// MaJ de l'IHM
	BtnCreerZone->Enabled = false;
    BtnCreeZoneAnnuler->Enabled = false;
    CBTracerZone->Checked = false;
    CBChoisirNumero->Checked = false;
    BitBtnChooseGameObjectID->Enabled = false;
    BitBtnTracerZone->Enabled = false;
    BtnNewZone->Enabled = true;
    // S�lection de cette zone dans la liste
    iModeDesignLevel = DESIGN_LEVEL_MODE_ZONES_SELECTION;
	NameElement->Caption = IDMapZones->CleID(iObjSelected);
    NumElement->Caption = iObjSelected;
	// MaJ du panneau des contr�les
    UpdateControles();
    UpdateListeObjets();
}
//----------------------------------------------------------------------------//

//=== Zone pr�c�dente ========================================================//
void __fastcall TFormDesignOutils::BitBtnReculClick(TObject *Sender)
{   if (iObjSelected==-1) return;
	// Recherche de la zone pr�c�dente
	MapZone* zone = NULL;
    zone = MapNiveau->pMapZoneList->GetPrevZone(iObjSelected);
    if (zone == NULL) return;
	// Nouvelle zone s�lectionn�e
	pMapZone = zone;
	iObjSelected = pMapZone->GetID();
  	NameElement->Caption = IDMapZones->CleID(iObjSelected);
    NumElement->Caption = iObjSelected;
    UpdateZoneInfos();
    UpdateListeObjets();
}
//----------------------------------------------------------------------------//

//=== Zone suivante ==========================================================//
void __fastcall TFormDesignOutils::BitBtnAvanceClick(TObject *Sender)
{   if (iObjSelected==-1) return;
	// Recherche de la zone pr�c�dente
	MapZone* zone = NULL;
    zone = MapNiveau->pMapZoneList->GetNextZone(iObjSelected);
    if (zone == NULL) return;
	// Nouvelle zone s�lectionn�e
	pMapZone = zone;
	iObjSelected = pMapZone->GetID();
  	NameElement->Caption = IDMapZones->CleID(iObjSelected);
    NumElement->Caption = iObjSelected;
    UpdateZoneInfos();
    UpdateListeObjets();
}
//----------------------------------------------------------------------------//

//=== MaJ des contr�les ======================================================//
void __fastcall TFormDesignOutils::UpdateControles()
{	if (iObjSelected == -1)
	{	// On d�sactive les cont�les
    	PosZoneX->Enabled = false;
        PosZoneY->Enabled = false;
		BtnSupprimerZone->Enabled = false;
    } else
    {	// Affichage des contr�les possibles
    	PosZoneX->Enabled = true;
        PosZoneY->Enabled = true;
		BtnSupprimerZone->Enabled = true;
    }
}
//----------------------------------------------------------------------------//

//=== D�placement horizontal de la zone ======================================//
void __fastcall TFormDesignOutils::PosZoneXClick(TObject *Sender,
      TUDBtnType Button)
{	static lastSens = 0;
	// Avance d'un pas vers � gauche ou � droite
   	double offset = (double) PosZoneX->Position / 100;
   	pMapZone->MoveX(offset);
    // R�gle l'acc�l�ration
    int currentSens = (PosZoneX->Position > 0) ? 1 : 2;
    if (lastSens == currentSens)
    {	PosZoneX->Increment++;
    } else
    {	lastSens = currentSens;
        PosZoneX->Increment = 5;
    }
    // On r�inialise le tout
    PosZoneX->Position = 0;
}
//----------------------------------------------------------------------------//

//=== D�placement vertical de la zone ========================================//
void __fastcall TFormDesignOutils::PosZoneYClick(TObject *Sender,
      TUDBtnType Button)
{	static lastSens = 0;
	// Avance d'un pas vers � gauche ou � droite
   	double offset = (double) PosZoneY->Position / 100;
   	pMapZone->MoveY(offset);
    // R�gle l'acc�l�ration
    int currentSens = (PosZoneY->Position > 0) ? 1 : 2;
    if (lastSens == currentSens)
    {	PosZoneY->Increment++;
    } else
    {	lastSens = currentSens;
        PosZoneY->Increment = 5;
    }
    // On r�inialise le tout
    PosZoneY->Position = 0;
}
//----------------------------------------------------------------------------//

//=== Supprime la fosse s�lectionn�e =========================================//
void __fastcall TFormDesignOutils::BtnSupprimerZoneClick(TObject *Sender)
{	if (iObjSelected==-1) return;
    // On d�selectionne tout
	iObjSelected=-1;
	ViewMap->ObjZoneSelect = FALSE;
	// On supprime la zone de la liste
	MapNiveau->pMapZoneList->DelZone(pMapZone->GetID());
    pMapZone = NULL;
    // MaJ de l'IHM
    UpdateView();
    UpdateControles();
    UpdateZoneInfos();
    UpdateListeObjets();
    if (ViewMap!=NULL) ViewMap->Pointeur = Pointeur_NOTHING;
    iModeDesignLevel = DESIGN_LEVEL_MODE_NONE;
}
//----------------------------------------------------------------------------//

//=== Bouton de S�lection d'une zone =========================================//
void __fastcall TFormDesignOutils::BtnSelectZoneClick(TObject *Sender)
{	// S�lection d'une Zone au sol d�j� plac�e sur la carte
    ViewMap->Pointeur = Pointeur_SELECTZONE;
}
//----------------------------------------------------------------------------//

//=== MaJ de la liste des objets associ�s � la zone ==========================//
void __fastcall TFormDesignOutils::UpdateListeObjets()
{	if (iObjSelected==-1)
	{	// On d�sactive tout
    	BtnSelectObject->Enabled = false;
        BtnSupprObject->Enabled = false;
    }
    else
    {   // On MaJ la ListBox
        ListBoxObjetsAssocies->Clear();
        int objID = -1;
        for (int i=0 ; i < pMapZone->GetNbJointObject() ; i++)
        {	objID = pMapZone->GetObjectID(i);
            if (IDGameDecors->IsElement(objID))
            {   AnsiString objName = IDGameDecors->CleID(objID);
                ListBoxObjetsAssocies->Items->Add(objName);
            } else
            {   ListBoxObjetsAssocies->Items->Add("UNKNOW");
            }
        }
        // Puis les boutons
        BtnSelectObject->Enabled = true;
    }
}
//----------------------------------------------------------------------------//

//=== S�lection d'un objet � associ� � la Zone ===============================//
void __fastcall TFormDesignOutils::BtnSelectObjectClick(TObject *Sender)
{	ViewMap->Pointeur = Pointeur_SELECTOBJECT;
	StatusBarLevelDesign->SimpleText = "Double cliqu� sur un �l�ment du d�cors";
}
//----------------------------------------------------------------------------//

//=== Ajoute cet object � la zone ============================================//
void __fastcall TFormDesignOutils::AddObjectToZone(DecorsObjet* decors)
{
	int id = decors->GetID();
    if (id==0)
    {	StatusBarLevelDesign->SimpleText = "L'objet s�lectionn� n'a aucun script";
		return;
	}
	pMapZone->AddObject2Zone(id);
	ViewMap->Pointeur = Pointeur_NOTHING;
    UpdateListeObjets();
    StatusBarLevelDesign->SimpleText = "El�ment du d�cors associ� � cette zone";
}
//----------------------------------------------------------------------------//

//=== S�lection d'un nom d'object ============================================//
void __fastcall TFormDesignOutils::ListBoxObjetsAssociesClick(
      TObject *Sender)
{	// Si l'on a s�lectionn� une ligne, on permet de la supprimer
	if (ListBoxObjetsAssocies->ItemIndex != -1)
    {	BtnSupprObject->Enabled = true;
    } else
    {	BtnSupprObject->Enabled = false;
    }
}
//----------------------------------------------------------------------------//

//=== Supression d'un Object associ� � la zone ===============================//
void __fastcall TFormDesignOutils::BtnSupprObjectClick(TObject *Sender)
{   if (ListBoxObjetsAssocies->ItemIndex == -1) return;
	// R�cup�re le nom de l'objet s�lectionn�
    AnsiString objName = ListBoxObjetsAssocies->Items->Strings[ListBoxObjetsAssocies->ItemIndex];
    if (objName != "UNKNOW")
    {    int objID = IDGameDecors->NumID(objName);
        // Puis on le supprime de la liste des zones associ�es � la zone
    	pMapZone->DelObject2Zone(objID);
    } else
    {   // Sinon, c'est que l'object est inexistant, et on efface l'object par son index
        pMapZone->DelObjectPerIndex(ListBoxObjetsAssocies->ItemIndex);
    }
    // Et on MaJ la StringGrid
    UpdateListeObjets();
}
//----------------------------------------------------------------------------//

