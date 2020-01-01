////////////////////////////////////////////////////////////////////////////////
// fPrincipal.cpp : Fenêtre Principale de l'éditeur des Persos & Anims		  //
//                                                                            //
// Création : 14/11/1998													  //
// Version : 0.01 beta														  //
////////////////////////////////////////////////////////////////////////////////

#include <vcl\vcl.h>
#include "fPrincipale.h"
#pragma hdrstop
//#define INITGUID

#include "consts.h"				// constantes universelles
#include "fMenuBank.h"			// Menu de création d'une nouvelle banque d'images
#include "fMenuIDName.h"		// Boîte de saisie d'un identificateur
#include "fMenuIDPath.h"		// Boîte de saisie d'un répertoire où tous les fichiers relatifs à l'ID vont être stockés
#include "fBarOutils.h"			// Fiche de barre d'outils
#include "fMenuScript.h"		// Fiche d'éditeur de scripts
#include "SpritesLib.h"			// Gestion des anims superposables "classiques"
#include "GameDebug.h"          // Librairie maison de debuggage
#include "IDMap.h"		        // Gestion des identificateurs du jeu

// A inclure dans l'éditeur
std::IDMap *IDPatronPersos = NULL;  // Map des ID des patrons de personnages
std::IDMap *IDAnimation = NULL;	    // Map des identificateurs des mouvements d'animation
std::IDMap *IDPersosType = NULL;    // Map des ID des Types de sprites différents
std::IDMap *IDMembres = NULL;       // map des ID recensants les différents membres du corps humain
// A inclure dans le jeu (leture des .ina)
std::IDMap *IDBmpFrame = NULL;      // Map des ID de toutes les frames bmp du jeu
std::IDMap *IDTypeObjSprite = NULL; // Map des ID des types d'objets (ex: epee1) servant dans la superposition d'anims
std::IDMap *IDTiles = NULL;         // Map des ID des tiles du jeu

AnsiString IDName;					// Nom d'un identificateur
AnsiString IDPathFiles;				// Chemin relatifs à l'identificateur
AnsiString IDScriptShortFilename;   // nom sans extension du fichier script à sauver

#include "Animall.h"				// Gestion des personnages & des animations
SpriteTilePersoLib  *STLGuy = NULL; // Liste des animations et bitmaps du personnages sur lequel on travaille
SpriteTilePersoLib  *STLDec = NULL; // Liste des animations et bitmaps du décors/objet sur lequel on travaille

#include "ImDecors2.h"
#include "PersoLib.h"
// Définition des personnages
Personnage 		*Perso;				// Personnage sur lequel on travaille
Character       *Charact;           // Modele de personnage
DecorsObjet     *DecObj;            // Decors/Objets sur lequel on travaille

// Mode dans lequel se trouve l'utilisateur
enum tModeEdit { VIERGE ,NEW_PERSO_MODEL, NEW_DECORS_MODEL };
tModeEdit UserMode = VIERGE;	// Mode initial = Vierge

tAnimDirection AnimDirection = AUCUNE;        // Direction initialiale
tAnimationObjet AnimationObjet = OBJET_NONE;  // Scripts d'anims initialement chargé
AnsiString INFOSFileName;                     // Nom court du fichier sur lequel on travaille actuellement : hero1, pig1, ogre ...

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TFormPrinc *FormPrinc;

AnsiString CheminApplication;	// chemin de l'exécutable
int ScreenViewX = 1024,		// fenêtre représentant la vue du jeu
    ScreenViewY = 680;

bool ScriptCompiled;            // Indique si le script est chargé en mémoire
int  aNumAnimation;		// Numéro de l'animation sélectionnée
tPlayingAnimationState aPlayingAnimationState;     // indique l'état de l'animation en cours
bool aSpriteSuperposeeToScreen = false;            // indique si il faut afficher un sprite supperposable
bool aTileDecorsToScreen = false;                  // Indique si l'on doit afficher le TileDecors sur la fAnimView
bool aTileDecorsIsVisible = false;                 // Indique si le TileDecors est visible ou non
int aSpriteSuperposeeNumero;                       // n° du sprite de la lib des sprites superp du perso à afficher
bool aSpriteSuperposeeMoving = false;              // Indique si l'on doit déplacer un sprite supperposable
SpriteTile* aSpriteSuperposeeTileEnd = NULL;       // Pointeur sur le sprite de fin d'anim (à remonter)
SpriteTile* aSpriteSuperposeeTileDeb = NULL;       // Pointeur sur le sprite de debut d'anim (à remonter ds le sens inverse)
int aSpritePersoNumAnim = 0;                       // n° du sprite du perso lors du déplacement de l'anim superposable
//--- Anims superposables classiques ---
SpriteTile* aSpriteClassik = NULL;				   // Pointeur sur la sprite à afficher
int aNumAnimClassik = -1; 						   // N° de l'anim classique à afficher
int aIndexAnimClassik = -1;			  	    	   // Index de cette même anim dans le tableau "peNumClassikAnim" de l'objet
bool aAnimClassikMoving = false; 				   // Indique si l'on doit déplacer un sprite supperposable classique
bool aSpriteClassikToScreen = false;               // Indique si il faut afficher un sprite supperposable
bool aSpriteClassikPlaying = false; 			   // Indique si l'animation est en route

//---------------------------------------------------------------------------
__fastcall TFormPrinc::TFormPrinc(TComponent* Owner)
	: TForm(Owner)
{

    // Chemin dans lequel a été lancé cette application
    CheminApplication = Application->ExeName;
    CheminApplication.UpperCase();
    CheminApplication.Delete(CheminApplication.Pos("ANIMEDITOR.EXE"),strlen("ANIMEDITOR.EXE"));

    // Aucun personnage chargé au départ
    STLGuy = NULL;
    Perso = NULL;

     // Met en route le Debugger maison
    ZeGameDebug = new GameDebug("animeditor.log");

    // Script non compilé
    ScriptCompiled = false;
    aNumAnimation = 0;
    aPlayingAnimationState = OFF;

   // On charge les identificateurs à partir du fichier
   // 1.a]  patrons des persos :
   if (IDPatronPersos!=NULL) { delete (IDPatronPersos); IDPatronPersos=NULL; };
   IDPatronPersos = new std::IDMap;
   IDPatronPersos->LireFileID(CheminApplication+cPersosPatronFilename);
   // 1.b]  membres du corps humain :
   if (IDMembres!=NULL) { delete (IDMembres); IDMembres=NULL; };
   IDMembres = new std::IDMap;
   IDMembres->LireFileID(CheminApplication+cIDPersosMembresFilename);

   // A inclure dans le jeu (leture des .ina)
   // 1.c] frames bmp du jeu :
   if (IDBmpFrame!=NULL) { delete (IDBmpFrame); IDBmpFrame=NULL; };
   IDBmpFrame = new std::IDMap;
   IDBmpFrame->LireFileID(CheminApplication+cIDBmpFrameFilename);
   // 1.d] types d'objets (ex: epee1) servant dans la superposition d'anims
   if (IDTypeObjSprite!=NULL) { delete (IDTypeObjSprite); IDTypeObjSprite=NULL; };
   IDTypeObjSprite = new std::IDMap;
   IDTypeObjSprite->LireFileID(CheminApplication+cIDTypeObjSpriteFilename);

   // 1.e] Identificateurs des différents type de sprites possibles
   if (IDPersosType!=NULL) { delete (IDPersosType); IDPersosType=NULL; };
   IDPersosType = new std::IDMap;
   IDPersosType->LireFileID(CheminApplication+cIDPersosTypeFilename);

   // 1.f] Identificateurs des différents TILES
   if (IDTiles!=NULL) { delete (IDTiles); IDTiles=NULL; };
   IDTiles = new std::IDMap;
   IDTiles->LireFileID(CheminApplication+cIDTilesFilename);

   // 1.g) Charge les ID d'animation
   if (IDAnimation!=NULL) { delete (IDAnimation); IDAnimation=NULL; };
   IDAnimation = new std::IDMap;
   IDAnimation->LireFileID(CheminApplication+cIDAnimationFilename);
}
//---------------------------------------------------------------------------
void __fastcall TFormPrinc::Nouvellebanque1Click(TObject *Sender)
{
	// Affiche le Menu de création d'une nouvelle banque d'image
    MenuNewBank->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormPrinc::ModelepersonnageClick(TObject *Sender)
{  // Nouveau patron de personnage à créer
	AnsiString MdlName;
    int index;
   // 1) On charge les identificateurs à partir du fichier
   // ....  déjà chargées ....

   // 2) On demande à l'utilisateur un nom d'identificateur
   FormIdentifier->Label->Caption = "Donnez un nom d'identificateur non utilisé";
   FormIdentifier->EditIDName->Text = cPersosPatronID;
   IDName = "";
   do
   { FormIdentifier->ShowModal();
     IDName = FormIdentifier->EditIDName->Text;

     // Vérifie si l'identificateur n'existe déjà pas dans la map
     if (IDPatronPersos->IsCle(IDName.c_str()))
     {	MessageBoxA(NULL,"Identificateur existant déjà !",IDName.c_str(),1);
     }

     // Vérifie si l'identificateur n'est pas celui fourni
     if (IDName == cPersosPatronID)
     {	MessageBoxA(NULL,"Vous devez choisir un autre nom d'ID","Warning !",1);
     }
   }
   while ((FormIdentifier->sortieOK) && (IDPatronPersos->IsCle(IDName.c_str())) && (IDName == cPersosPatronID));
   if (!FormIdentifier->sortieOK)
   {	goto StopAll;
   }
   // on a donc le nom d'un nouvel identificateur
   IDPatronPersos->AjoutClef(IDName);
   typePERSO = IDPatronPersos->NumID(IDName);

   // 3) Choix du répertoire de sauvegarde des fichiers relatifs modèle
   FormPath->DirectoryListBox->Directory = CheminApplication + cPathPersos;
   FormPath->ShowModal();
   IDPathFiles = FormPath->DirectoryListBox->Directory;	// Récupère le bon répertoire

   // 4) Indique le nouveau mode utilisé
   UserMode = NEW_PERSO_MODEL;

   // 5) Choix du nom du fichier principal du modele de Perso (.mdl)
   // Calcul d'un nom par defaut
   MdlName = IDPathFiles;
   while ((index = MdlName.AnsiPos("\\")) != 0)
   {  MdlName.Delete(1, index);
   }
   // Ouvre la boîte de dialogue
   SaveDialogScriptAs->FilterIndex = 1;
   SaveDialogScriptAs->InitialDir = IDPathFiles;
   SaveDialogScriptAs->FileName = MdlName;
   SaveDialogScriptAs->Execute();
   // Récupère le nom complet du fichier (full path + name)
   MdlName = SaveDialogScriptAs->FileName;
   if (MdlName=="")
   {	MessageBoxA(NULL,"Vous n'avez pas rentre de nom de fichier valide","Sauvegarde annulee",1);
   }
   if (MdlName.AnsiPos(".mdl")==0)
   {	// Rajoute l'extension du fichier
        MdlName += ".mdl";
   }

   // 6) Modifie certains paramètres et boîtes de dialogues des autres fiches
   BarOutils->Title->Caption = "Modèle de Personnage";
   BarOutils->LabelID->Caption = IDName;
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
   // Menu "Script" accessible => Déconne
   FormScriptEditor->ScrNew->Enabled = true;

    // 8) Creation du template et d'un perso en heritant
	// Character
    if (STLGuy!=NULL) delete STLGuy;
    STLGuy = new SpriteTilePersoLib;
    Charact = new Character(typePERSO);
    // Personnage
    Perso = new Personnage(typePERSO);
    Perso->ModelePerso = Charact;
    Perso->ModelePerso->STLPersonnage = STLGuy;

    // 9) Sauvegarde du .mdl
    Perso->ModelePerso->SaveMdl(MdlName.c_str());

   // 10) On travaille dorenavant avec un Personnage
   AnimationObjet = OBJET_PERSOS;

 StopAll:
}
//---------------------------------------------------------------------------

//=== Essai de sauvegarde de l'architecture des infos des sprites ============//
void __fastcall TFormPrinc::InfosSprites1Click(TObject *Sender)
{ // crée un fichier en écriture dans lequel on va sauver toutes les infos concernant le décors ou le personnage
  FILE *f;
  AnsiString chem;
  if (AnimationObjet == OBJET_PERSOS)     // Cas d'un perso
  { chem = CheminApplication+cPathPersos+INFOSFileName+".mdl";
  	Perso->ModelePerso->SaveMdl(chem.c_str());
  } else
  if (AnimationObjet == OBJET_DECORS)     // Cas d'un décors
  {  chem = CheminApplication+cPathObjets+INFOSFileName+".mdl";
     if ((f = fopen(chem.c_str(),"wb"))==NULL)
     { MessageBoxA(NULL,"Impossible de créer le fichier : ",chem.c_str(),1); return; }
     // Sauve les infos de bases
     // a) N° de version du mdl
     fwrite(&TAnimTileDecors::MdlVersion,sizeof(TAnimTileDecors::MdlVersion), 1, f);
     // b) Informations des anims
     DecObj->Element->SaveToFileAnimInfos(*f);
     DecObj->Element->SaveToFileAnimClassik(*f);   // Infos des anims "classiques"
     fclose(f);
  }
}
//---------------------------------------------------------------------------

//== Essai de chargement de l'architecture des infos des sprites =============//
void __fastcall TFormPrinc::InfosSprites2Click(TObject *Sender)
{ FILE *f;
  // Ouvre le fichier binaire en lecture
  AnsiString chem = CheminApplication+cPathPersos+"NainReneg\\NainReneg.mdl";
   if ((f = fopen(chem.c_str(),"rb"))==NULL)
           MessageBoxA(NULL,"Impossible de lire le fichier",chem.c_str(),1);
  Perso->ModelePerso->LoadFromFileInfoSprites(*f);
  fclose(f);
}
//----------------------------------------------------------------------------//

//=== Libère tout ce qui a été alloué ========================================//
void __fastcall TFormPrinc::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    delete STGL;			// Libère les anims & sprites 
    delete ZeGameDebug;		// Stop le Debugger maison

}
//---... bonne blague ...-----------------------------------------------------//


//=== Rends actif l'editeur de script afin de derouler son menu ==============//
void __fastcall TFormPrinc::ActiveScriptEditor(TObject *Sender)
{   if (!FormScriptEditor->Focused())
	{  	FormScriptEditor->Show();
    }
}
//----------------------------------------------------------------------------//


