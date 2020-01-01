//---------------------------------------------------------------------------
#ifndef fPrincipaleH
#define fPrincipaleH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Menus.hpp>
//----------------------------------------------------------------------------//
// Mes propres inclusions
#include "IDMap.h"					   // Acces à la gestion des identificateurs
#include "Animall.h"
#include "PersoLib.h"
#include "ImDecors2.h"
#include <Dialogs.hpp>
//----------------------------------------------------------------------------//

class TFormPrinc : public TForm
{
__published:	// Composants gérés par l'EDI
	TMainMenu *MainMenu;
	TMenuItem *Files1;
	TMenuItem *Anims1;
	TMenuItem *Nouvellebanque1;
	TMenuItem *Nouveau;
	TMenuItem *Modelepersonnage;
	TMenuItem *ModeleDecors;
	TMenuItem *FileScript;
        TMenuItem *Sauver;
        TMenuItem *InfosSprites1;
        TMenuItem *Charger1;
        TMenuItem *InfosSprites2;
	TSaveDialog *SaveDialogScriptAs;
	void __fastcall Nouvellebanque1Click(TObject *Sender);
	void __fastcall ModelepersonnageClick(TObject *Sender);
        void __fastcall InfosSprites1Click(TObject *Sender);
        void __fastcall InfosSprites2Click(TObject *Sender);
        
        
        
        
        
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ActiveScriptEditor(TObject *Sender);
	
	
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
	__fastcall TFormPrinc(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TFormPrinc *FormPrinc;

extern AnsiString CheminApplication; // Chemin d'où a été exécuté le PersoEditor

extern std::IDMap *IDPatronPersos;   // Map des identificateurs de gestion de patrons de persos
extern std::IDMap *IDAnimation;	     // Map des identificateurs des mouvements d'animation
extern std::IDMap *IDPersosType;     // Map des ID des Types de personnages différents
extern std::IDMap *IDMembres;        // map des ID recensants les différents membres du corps humain
// A inclure dans le jeu (leture des .ina)
extern std::IDMap *IDBmpFrame;       // Map des ID de toutes les frames bmp du jeu
extern std::IDMap *IDTypeObjSprite;  // Map des ID des types d'objets (ex: epee1) servant dans la superposition d'anims
extern std::IDMap *IDTiles;          // Map des ID des tiles du jeu

extern AnsiString IDName;		 // Nom d'un identificateur
extern AnsiString IDPathFiles;		 // Chemin relatifs à l'identificateur
extern AnsiString IDScriptShortFilename; // nom sans extension du fichier script à sauver

extern SpriteTilePersoLib  *STLGuy;     // Liste des animations et bitmaps du personnages sur lequel on travaille
extern SpriteTilePersoLib  *STLDec;     // Liste des animations et bitmaps du décors/objet sur lequel on travaille
extern Personnage 	*Perso;	        // Personnage sur lequel on travaille
extern Character        *Charact;       // Modele de personnage
extern DecorsObjet      *DecObj;         // Decors/Objets sur lequel on travaille

extern bool ScriptCompiled;          // Indique si le script est chargé en mémoire
extern int  aNumAnimation;	     // Numéro de l'animation sélectionnée

// Direction de l'animation suivant les 8 pts cardinaux + 1 pt central + Rien
enum tAnimDirection { O = 0, SO, S, SE, E, NE, N, NO, CENTRAL, AUCUNE };
extern tAnimDirection AnimDirection;

// Etat d'ouverture des scripts d'anims
enum tAnimationObjet { OBJET_DECORS, OBJET_PERSOS, OBJET_NONE };
extern tAnimationObjet AnimationObjet;

// Etat de l'animation en cours
enum tPlayingAnimationState { STOP, PAUSE, PLAY, BACK, FORWARD, OFF };
extern tPlayingAnimationState aPlayingAnimationState;     // Indique l'état de l'animation en cours
extern bool aSpriteSuperposeeToScreen;                    // Indique si il faut afficher un sprite supperposable
extern bool aTileDecorsToScreen;                          // Indique si l'on doit afficher le TileDecors sur la fAnimView
extern bool aTileDecorsIsVisible;                         // Indique si le TileDecors est visible ou non
extern int aSpriteSuperposeeNumero;                       // n° du sprite de la lib des sprites superp du perso à afficher
extern bool aSpriteSuperposeeMoving;                      // Indique si l'on doit déplacer un sprite supperposable
extern SpriteTile* aSpriteSuperposeeTileEnd;              // Pointeur sur le sprite de fin d'anim (à remonter)
extern SpriteTile* aSpriteSuperposeeTileDeb;              // Pointeur sur le sprite de debut d'anim (à remonter ds le sens inverse)
extern int aSpritePersoNumAnim;                           // n° du sprite du perso lors du déplacement de l'anim superposable
extern AnsiString INFOSFileName;                          // Nom court du fichier sur lequel on travaille actuellement : hero1, pig1, ogre ...
//--- Anims superposables classiques ---
extern SpriteTile* aSpriteClassik;						  // Pointeur sur la sprite à afficher
extern int aNumAnimClassik;								  // N° de l'anim classique à afficher
extern int aIndexAnimClassik;							  // Index de cette même anim dans le tableau "peNumClassikAnim" de l'objet
extern bool aAnimClassikMoving;							  // Indique si l'on doit déplacer un sprite supperposable classique
extern bool aSpriteClassikToScreen;                       // Indique si il faut afficher un sprite supperposable
extern bool aSpriteClassikPlaying;						  // Indique si l'animation est en route	

//---------------------------------------------------------------------------
#endif
