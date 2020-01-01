////////////////////////////////////////////////////////////////////////////////
//  fMenuScript :  fenêtre permettant d'éditer des fichiers texts comme les   //
//				   scripts d'animations *.ina ou les fichiers *.id			  //
////////////////////////////////////////////////////////////////////////////////


//--- Inclusions C++ Builder ------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop
#include "fMenuScript.h"
#include "fPrincipale.h"
#include "consts.h"
#include "fWorkStation.h"
#include "stdio.h"
#include "fBarOutils.h"
#include "GameDebug.h"

//--- Variables C++ Builder--------------------------------------------------
#pragma resource "*.dfm"
TFormScriptEditor *FormScriptEditor;

//--- Mes variables --------------------------------------------------------
enum eScriptMode { DESACTIVATE, NEW_SCRIPT_ANIMATION };
eScriptMode ScriptMode;

AnsiString IDScriptFilename;		// nom et chemin du fichier script à sauver
int typePERSO = PERSO;


//=== Constructeur de la fiche ===============================================//
__fastcall TFormScriptEditor::TFormScriptEditor(TComponent* Owner)
	: TForm(Owner)
{
    // Positionnement de la fiche sur la fiche MDI principale
    Left = 0; Top = 418;
    Width = 577; Height = 274;
    // Mode désactivé
    ScriptMode = DESACTIVATE;
    ScrNew->Enabled = false;
    ScrSave->Enabled = false;
    ScrSaveAs->Enabled = false;
    ScrWizard->Enabled = false;
}
//----------------------------------------------------------------------------//

//=== Crée un nouveau fichier script d'animation =============================//
void __fastcall TFormScriptEditor::FichierAnimation1Click(TObject *Sender)
{  // Active la fenêtre
   RichEditScript->Enabled = true;
   RichEditScript->Clear();  // Efface son contenu
   // Initialise le mode d'ouverture
   ScriptMode = NEW_SCRIPT_ANIMATION;
   // Permet de sauver le fichier  => fichier de même nom que le répertoire
   ScrSaveAs->Enabled = true;
   // Permet de charger un wizard
   ScrWizard->Enabled = true;
   // Remplit l'en-tête du fichier *.ina
   AnsiString EnTete = "// Animations et transitions de "+IDName;
   RichEditScript->Lines[0].Text = EnTete;
   RichEditScript->Enabled = true;
   RichEditScript->ReadOnly = false;
}
//----------------------------------------------------------------------------//

//== Sauve le fichier sous un nom à déterminer ===============================//
void __fastcall TFormScriptEditor::ScrSaveAsClick(TObject *Sender)
{  // Essaye de trouver un nom par defaut
   IDScriptFilename = IDPathFiles;
   int index;
   while ((index = IDScriptFilename.AnsiPos("\\")) != 0)
   {  IDScriptFilename.Delete(1, index);
   }
   // Ouvre la boîte de dialogue
   SaveDialogScriptAs->FilterIndex = 1;
   SaveDialogScriptAs->InitialDir = IDPathFiles;
   SaveDialogScriptAs->FileName = IDScriptFilename;
   SaveDialogScriptAs->Execute();
   // Récupère le nom du fichier
   IDScriptFilename = SaveDialogScriptAs->FileName;
   if (IDScriptFilename=="")
   {	MessageBoxA(NULL,"Vous n'avez pas rentre de nom de fichier valide","Sauvegarde annulee",1);
   }
   if (IDScriptFilename.AnsiPos(".ina")==0)
   {	// Rajoute l'extension du fichier
        IDScriptFilename += ".ina";
   }

   // Sauvegarde le fichier texte ligne par ligne
   // Ouvre le fichier
   FILE *f;
   AnsiString filename = IDScriptFilename;
   if ((f = fopen(filename.c_str(),"wb"))==NULL)
           MessageBoxA(NULL,filename.c_str(),"Impossible de créer le fichier :",1);
   AnsiString ligne;
   ligne = RichEditScript->Text;
   fwrite(ligne.c_str(),ligne.Length(), 1, f);
   fclose(f);

   // Permet la sauvegarde classique
   ScrSave->Enabled = true;
   // Permet de compiler le script
   WokStation->BtnLoadScript->Enabled = true;

   // Conserve le short-name du fichier .ina
   AnsiString st;
   st = IDScriptFilename.LowerCase();
   st.Delete(st.Pos(".ina"),strlen(".ina"));
   char essai [128];
   strcpy(essai,(IDPathFiles.LowerCase()).c_str());
   st.Delete(st.Pos(essai),strlen(essai));
   st.Delete(st.Pos("\\"),strlen("\\"));
   IDScriptShortFilename = st;
}
//----------------------------------------------------------------------------//

//=== Charge le squelette d'un script d'animation à la suite du script =======//
void __fastcall TFormScriptEditor::ScrWizardAnimationClick(TObject *Sender)
{  // Charge le squelette ds le RichEdit à partir du fichier
   // Ouvre le fichier
   FILE *f;
   AnsiString filename = CheminApplication + '\\' + cPersosAnimationSqueletteFilename;
   if ((f = fopen(filename.c_str(),"r"))==NULL)
        {   MessageBoxA(NULL,filename.c_str(),"Impossible de lire le fichier :",1);
            exit(0);  }
   char cligne [512];
   // Scrute le fichier ligne par ligne
   while (fgets(cligne, 256, f) != NULL)  // tant que le fichier n'est pas fini
   {  AnsiString lin = cligne;
      lin.SetLength(lin.Length()-1);	  // supprime le caractère de fin de ligne
      RichEditScript->Lines->Add(lin);
   }
   fclose(f);
   // Il faudrait remettre les scroll-bars bien comme il faut
   RichEditScript->Realign();
   RichEditScript->Repaint();
}
//----------------------------------------------------------------------------//

//=== Charge le fichier dans le RichEdit =====================================//
void __fastcall TFormScriptEditor::ShowFile(AnsiString filename)
{ // Charge le squelette ds le RichEdit à partir du fichier
   // Ouvre le fichier
   FILE *f;
   if ((f = fopen(filename.c_str(),"r"))==NULL)
        {   MessageBoxA(NULL,filename.c_str(),"Impossible de lire le fichier :",1);
            exit(0);  }
   char cligne [256];
   // Scrute le fichier ligne par ligne
   while (fgets(cligne, 256, f) != NULL)  // tant que le fichier n'est pas fini
   {  AnsiString lin = cligne;
      lin.SetLength(lin.Length()-1);	  // supprime le caractère de fin de ligne
      RichEditScript->Lines->Add(lin);
   }
   fclose(f);

   // Met en place le contrôle TRichEdit
   RichEditScript->Realign();
   RichEditScript->Repaint();
   RichEditScript->Enabled = true;
   RichEditScript->ReadOnly = false;
}
//----------------------------------------------------------------------------//

//== Donne le focus à la fiche du script =====================================//
void __fastcall TFormScriptEditor::FichierScriptClick(TObject *Sender)
{	FormScriptEditor->SetFocus();
}
//----------------------------------------------------------------------------//

//=== Sauvegarde le fichier *.ina dans le répertoire =========================//
void __fastcall TFormScriptEditor::ScrSaveClick(TObject *Sender)
{
   FILE *f;
   AnsiString filename = IDScriptFilename;
   if ((f = fopen(filename.c_str(),"wb"))==NULL)
           MessageBoxA(NULL,filename.c_str(),"Impossible de créer le fichier :",1);
   AnsiString ligne = RichEditScript->Text;
   fwrite(ligne.c_str(),ligne.Length(), 1, f);
   fclose(f);
}
//----------------------------------------------------------------------------//

bool essaiDebug = false;
//=== Ouvre un script d'animation des personnages ============================//
// => forme provisoire pour augmenter la rapidité d'édition
// => A JARTER PAR LA SUITE OU A TOTALEMENT MODIFIER
void __fastcall TFormScriptEditor::OpenAndLoadScript(AnsiString ScriptName)
{  IDScriptShortFilename = ScriptName;   // Assigne le nom du fichier script à ouvrir

   // 0) Sauvegarde de la pile
   tPlayingAnimationState ctemp = aPlayingAnimationState;
   aPlayingAnimationState = OFF;

   // PROVIENT DE FANIMVIEW
   // 1) On charge les identificateurs à partir du fichier
//   if (IDPatronPersos!=NULL) { delete (IDPatronPersos); IDPatronPersos=NULL; };
   IDPatronPersos = new std::IDMap;
   IDPatronPersos->LireFileID(CheminApplication+cPersosPatronFilename);

   // 5) Charge les ID d'animation
//   if (IDAnimation!=NULL) { delete (IDAnimation); IDAnimation=NULL; };
   IDAnimation = new std::IDMap;
   IDAnimation->LireFileID(CheminApplication+cIDAnimationFilename);
  // Charge par défaut un fichier
    // *** 1) Compilation du Script ***
    // Initialise la SpriteTileLib


    if (STLGuy!=NULL) delete STLGuy;
    STLGuy = new SpriteTilePersoLib;
    // Character
    Charact = new Character(typePERSO);
    Charact->LoadIna(IDScriptShortFilename.c_str(),&STLGuy);
    STLGuy->CreateSprIndex();
    Charact->STLFrameSuperp->CreateSprIndex();

    // Personnage
    Perso = new Personnage(typePERSO);
    Perso->ModelePerso = Charact;
    Perso->ModelePerso->STLPersonnage = STLGuy;
    // Charge les *.bmp et *.spi

    // Let's go
    ScriptCompiled = true;

    // *** 2) Initialise la WorkStation ***
    // On Ajoute à la Combobox, le nom des anims
    WokStation->CBListAnims->Clear();
    for (int i=0; i < Charact->peNbAnim ; i++)
     { WokStation->CBListAnims->Items->Add(Charact->AnimSpriteLib[i]->Nom);
     }

    // On ajoute à une ListBox le nom des membres
    WokStation->ListBoxMembres->Clear();
    for (int i=0; i <= IDMembres->MaxAttribut() ; i++)
     { if (IDMembres->IsElement(i))
       { WokStation->ListBoxMembres->Items->Add(IDMembres->CleID(i));
       }
     }

    // Speed Boutons du pavé des points cardinaux
    BarOutils->SpdBtnNO->Enabled = true;
    BarOutils->SpdBtnO->Enabled = true;
    BarOutils->SpdBtnSO->Enabled = true;
    BarOutils->SpdBtnS->Enabled = true;
    BarOutils->SpdBtnSE->Enabled = true;
    BarOutils->SpdBtnE->Enabled = true;
    BarOutils->SpdBtnNE->Enabled = true;
    BarOutils->SpdBtnN->Enabled = true;
    BarOutils->SpdBtnMiddle->Enabled = false;

    // Charge le contenu du fichier .mdl contenant toutes les informations concernant les
    // tiles de l'anim : zone au sol, pts divers, ...
    AnsiString chem2 = CheminApplication+cPathPersos+IDScriptShortFilename+'\\'+IDScriptShortFilename+".mdl";
    Charact->LoadMdl(chem2.c_str());

    aPlayingAnimationState = ctemp;   // Restitution de la pile.

    WokStation->LabelTypePersosObjets->Caption = "Liste des types de persos";
    AnimationObjet = OBJET_PERSOS;            // Mode édition d'un perso
    INFOSFileName = IDScriptShortFilename;    // Sauve le fichier

    // Affiche le script .ina à l'écran
    ShowFile(CheminApplication+cPathPersos+IDScriptShortFilename+'\\'+IDScriptShortFilename+".ina");
	// Permet la sauvegarde classique et par renommage
	ScrSave->Enabled = true;
	ScrSaveAs->Enabled = true;
}
//----------------------------------------------------------------------------//

//=== Cherche un chemin de persos, puis ouvre et charge le script ============//
void __fastcall TFormScriptEditor::OuvrirScriptAnimationClick(
      TObject *Sender)
{  // Préléminaires : on récupère le nom raccourci du script à ouvrir : Nain, Ogre, Hero ...
   DialogOuvrirScript->InitialDir = CheminApplication + cPathPersos;
   if (!DialogOuvrirScript->Execute()) return;
   AnsiString temp = DialogOuvrirScript->FileName;
   AnsiString chem = CheminApplication + cPathPersos;
   temp = temp.LowerCase();  // Convertit tout en minuscules
   chem = chem.LowerCase();
   temp.Delete(temp.Pos(chem.c_str()),chem.Length());   // Supprime le chemin de départ
   temp.Delete(temp.Pos(".ina"),strlen(".ina"));        // Supprime l'extension .ina
   temp.SetLength(temp.Pos("\\")-1);                    // Conserve seulement le chemin

   OpenAndLoadScript(temp);   // Ouvre & Charge le script
}
//----------------------------------------------------------------------------//

//=== Chargement des différents persos déjà existant =========================//
void __fastcall TFormScriptEditor::Hero1Click(TObject *Sender)
{ typePERSO = HERO1;
  OpenAndLoadScript("Hero"); // Ouvre & Charge le script du Héro
}
void __fastcall TFormScriptEditor::Ogre1Click(TObject *Sender)
{ typePERSO = OGRE1;
  OpenAndLoadScript("Ogre"); // Ouvre & Charge le script de l'Ogre
}
void __fastcall TFormScriptEditor::Nain1Click(TObject *Sender)
{  typePERSO = PERSO;
   OpenAndLoadScript("NainReneg"); // Ouvre & Charge le script du NainReneg
}
//----------------------------------------------------------------------------//

//=== Ouvre un script d'animation pour les décors ============================//
void __fastcall TFormScriptEditor::OpenAndLoadScriptDecors(AnsiString ScriptName)
{  IDScriptShortFilename = ScriptName;   // Assigne le nom du fichier script à ouvrir

   // 0) Sauvegarde de la pile
   tPlayingAnimationState ctemp = aPlayingAnimationState;
   aPlayingAnimationState = OFF;

   // 1) On charge les identificateurs à partir du fichier
   IDPatronPersos = new std::IDMap;
   IDPatronPersos->LireFileID(CheminApplication+cPersosPatronFilename);

   // 2) Charge les ID d'animation
   IDAnimation = new std::IDMap;
   IDAnimation->LireFileID(CheminApplication+cIDAnimationFilename);

   // 3) Chargement de la librairie des TILES du niveau
   if (DULib!=NULL) delete DULib;
   DULib = new TLevelTileLib;
   AnsiString NameLib = "new.LiL";
   DULib->LoadFile(CheminApplication + cPathLevels + NameLib);
   DULib->AlloueDDSurface();     // Puis alloue les surfaces DDraw et charge les bitmaps dedans

   // Charge par défaut un fichier
   // *** 1) Compilation du Script ***
   // Initialise la SpriteTileLib

    // Alloue les objets relatifs aux décors sur lesquels on va travailler
    if (DecObj!=NULL) delete DecObj;
    DecObj = new DecorsObjet;
    DecObj->Element = new TAnimTileDecors;
    STLDec = DecObj->Element->STLObjets;

    // Charge les *.bmp et *.spi
    DecObj->Element->LoadIna(IDScriptShortFilename);
    STLDec->CreateSprIndex();
    // MaJ des animations superposables
    DecObj->SetAnimTileDecors(DecObj->Element);
    // DecObj->STLFrameSuperp->CreateSprIndex();

    // Let's go
    ScriptCompiled = true;

    // *** 2) Initialise la WorkStation ***
    // On Ajoute à la Combobox, le nom des anims
    WokStation->CBListAnims->Clear();
    for (int i=0; i < DecObj->Element->peNbAnim ; i++)
    { WokStation->CBListAnims->Items->Add(DecObj->Element->AnimSpriteLib[i]->Nom);
    }

    // Speed Boutons du pavé des points cardinaux
    // => Que le Sud pour les Décors
    BarOutils->SpdBtnNO->Enabled = false;
    BarOutils->SpdBtnO->Enabled = false;
    BarOutils->SpdBtnSO->Enabled = false;
    BarOutils->SpdBtnS->Enabled = true;
    BarOutils->SpdBtnSE->Enabled = false;
    BarOutils->SpdBtnE->Enabled = false;
    BarOutils->SpdBtnNE->Enabled = false;
    BarOutils->SpdBtnN->Enabled = false;
    BarOutils->SpdBtnMiddle->Enabled = false;


    // Charge le contenu du fichier .mdl contenant toutes les informations concernant les
    // tiles de l'anim : zone au sol, pts divers, ...
    AnsiString chem2 = CheminApplication+cPathObjets+IDScriptShortFilename+".mdl";
    DecObj->Element->LoadMdl(chem2.c_str());

    // Affiche le script .ina à l'écran
    ShowFile(CheminApplication+cPathObjets+IDScriptShortFilename+".ina");

    aPlayingAnimationState = ctemp;   // Restitution de la pile.
    WokStation->LabelTypePersosObjets->Caption = "Liste des types de décors";
    AnimationObjet = OBJET_DECORS;              // Travaille sur un décors
    INFOSFileName = IDScriptShortFilename;      // Conserve le fichier sur lequel on travaille

     // On ajoute à la seconde CB le noms des anims "simples" superposables
    WokStation->ListBoxAnims->Clear();
    for (int i=0 ; i < DecObj->Element->peNbClassikAnim ; i++)
    {  WokStation->ListBoxAnims->Items->Add(IDAnimation->CleID(DecObj->Element->ClassikAnimInfos[i].iAnimID));
       TRACEDBG("NumAnim : %d\n",DecObj->Element->ClassikAnimInfos[i].iAnimID);	 
    }

    // Permet la sauvegarde classique et par renommage
   ScrSave->Enabled = true;
   ScrSaveAs->Enabled = true;
}
//----------------------------------------------------------------------------//


//=== Ouverture d'un script de décors ========================================//
void __fastcall TFormScriptEditor::Chercher1Click(TObject *Sender)
{ // Préléminaires : on récupère le nom raccourci du script à ouvrir : Porte, Cochon, Bocal ...
   DialogOuvrirScript->InitialDir = CheminApplication + cPathObjets;
   if (!DialogOuvrirScript->Execute()) return;
   AnsiString temp = DialogOuvrirScript->FileName;
   AnsiString chem = CheminApplication + cPathObjets;
   temp = temp.LowerCase();  // Convertit tout en minuscules
   chem = chem.LowerCase();
   temp.Delete(temp.Pos(chem.c_str()),chem.Length());   // Supprime le chemin de départ
   temp.Delete(temp.Pos(".ina"),strlen(".ina"));        // Supprime l'extension .ina

   OpenAndLoadScriptDecors(temp);   // Ouvre & Charge le script du décors
}
//----------------------------------------------------------------------------//

//=== Chargement DIRECT de scripts ===========================================//
void __fastcall TFormScriptEditor::Pig1Click(TObject *Sender)
{  // Ouvre & Charge le script d'un PIG1
   OpenAndLoadScriptDecors("Pig1"); }
void __fastcall TFormScriptEditor::Bocal1Click(TObject *Sender)
{  // Ouvre & Charge le script d'un BOCAL1
   OpenAndLoadScriptDecors("Bocal1"); }
//----------------------------------------------------------------------------//

