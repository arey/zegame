////////////////////////////////////////////////////////////////////////////////
//  Unit� : "DIlib.h" : librairie de fonctions associ�es � DirectInput	      //
//   	  Copyright (C) 1998 Antoine Rey     antoine.rey@insa-lyon.fr  	      //
//     					     antrey@club-internet.fr          //
//  Version	     :  0.18                                                  //
//  Cr�ation     :  8/07/98                                                   //
//  Last Upgrade : 16/04/99						      //
////////////////////////////////////////////////////////////////////////////////
#ifndef hDILib
#define hDILib

#include "DInput.h"			// Biblioth�que Microft Direct Input
//[ACF VISUAL]#include "DDLib.h"

// Supers marcos
#define KEYDOWN(name,key) (name[key] & 0x80)  // regarde si une touche est enfonc�e
// Gestion des �v�nements des touches du clavier & du joypad
#define EVENTKEY_RELACHE   0  // touche rel�ch�e
#define EVENTKEY_APPUYE    1  // touche venant juste d'�tre enfonc�e
#define EVENTKEY_ENFONCEE  2  // touche enfonc�e
#define EVENTKEY_RELACHEE  3  // tocuhe venant juste d'�tre rel�ch�e

// �tats des TOUCHES  enfonc�es ou rel�ch�es  (enfonc�es == TRUE)
extern BOOL Key_UP, Key_DOWN, Key_LEFT, Key_RIGHT,			// 4 fl�ches
	// pav� num�rique
	Key_npE, Key_npSE, Key_npS, Key_npSO, Key_npO, Key_npNO, Key_npN, Key_npNE,
    // Espace, Contr�le et Alternative
	Key_SPACE, Key_LSHIFT, Key_LCTRL,
        // Escape
        Key_ESCAPE,
        // Touches caract�res
        Key_K,
        // Chiffres
        Key_1, Key_2, Key_3,
		// Ponctuation & autres
		Key_BACKSLASH, 
        // 6 touches utilis�s par le GammaControl
        Key_INSERT, Key_DELETE, Key_PAGEUP, Key_PAGEDOWN, Key_END, Key_HOME, Key_BACKSPACE;

// �tats plus affin�s des TOUCHES rien=0; appuy�e=1; enfonc�e=2; rel�ch�e=3;
extern BYTE Key_ENTER, Key_PLUS, Key_MOINS, Key_F1, Key_F2, Key_F3, Key_F4, Key_F5, Key_F6, Key_F7,
            Key_F8, Key_F12, Key_LALT, Key_PAUSE;


// TDIRECTINPUT ////////////////////////////////////////////////////////////////
// Objet permettant de g�rer : clavier, souris et joypad					  //
class TDirectInput
{
	// VARIABLES
	public:
    	LPDIRECTINPUT		 pDI;   	// Objet Direct Input
    private:
        //--- Gestion du clavier ---
      	LPDIRECTINPUTDEVICE      pDIKeyBoard;	// Objet r�f�rencant le Clavier
        char Keys[256];				// Buffer de l'activation des touches du clavier
        bool bKeyBoard;	  			// Utilise-t-on un clavier ?
        bool bKeyboardActif;                    // Est-il actif ?
        BYTE TouchesPlus(BYTE Touch,BYTE TouchConst);	// Re�oit l'�tat des Tocuhes+

        //--- Gestion du Joypad ---
        LPDIRECTINPUTDEVICE2     pDIJoypad;     // Objet r�f�rencant le Joystick
        bool bJoypad;                           // Utilise-t-on un Joypad ?
        bool bJoypadActif;                      // Est-il actif
       	long axeX, axeY, axeZ;                  // Etat des 3 axes du joystick
		BYTE joyButtons[32];                    // Etat des 32 boutons du joystick
        BYTE JoyBoutonsPlus(BYTE Bouton, BYTE BoutonConst);

        HWND  DIHandle;   // Donne acc�s au handle de fen�tre de la fiche (fen�tre) principale

	// FONCTIONS
    public:
       	TDirectInput(HWND Hand);			// Constructeur
        ~TDirectInput(void);				// Destructeur
        void CreateDevice(BOOL bClavier,BOOL bSouris,BOOL bJoy);	// Active les devices demand�s

        void AnalyseKeys();				// Re�oit et analyse les touches int�ressantes

        void ReActive();                                        // R�active tous les p�riph�riques
        void ReActive(bool bClavier, bool bSouris, bool bJoy);  // R�active le p�riph. pr�cis�
        void UnActive();                                        // D�sactive tous les p�riph�riques
        void UnActive(bool bClavier, bool bSouris, bool bJoy);  // D�sactive le p�riph. pr�cis�
};
// Objet Principal du jeu
extern TDirectInput *DI;
////////////////////////////////////////////////////////////////////////////////


// Fonctions Annexes ---------------------------------------------------------//
extern void TraceErrorDI(HRESULT hErr,int Gravite);	// s'occupe es erreurs DirectInput
extern void MessageError(char *from,char *Erreur,int Gravite); // vient de "DDLib.cpp"


#endif
