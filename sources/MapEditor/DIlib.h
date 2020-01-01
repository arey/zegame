////////////////////////////////////////////////////////////////////////////////
//  Unité : "DIlib.h" : librairie de fonctions associées à DirectInput	      //
//   	  Copyright (C) 1998 Antoine Rey     antoine.rey@insa-lyon.fr  	      //
//     					     antrey@club-internet.fr          //
//  Version	     :  0.18                                                  //
//  Création     :  8/07/98                                                   //
//  Last Upgrade : 16/04/99						      //
////////////////////////////////////////////////////////////////////////////////
#ifndef hDILib
#define hDILib

#include "DInput.h"			// Bibliothèque Microft Direct Input
//[ACF VISUAL]#include "DDLib.h"

// Supers marcos
#define KEYDOWN(name,key) (name[key] & 0x80)  // regarde si une touche est enfoncée
// Gestion des événements des touches du clavier & du joypad
#define EVENTKEY_RELACHE   0  // touche relâchée
#define EVENTKEY_APPUYE    1  // touche venant juste d'être enfoncée
#define EVENTKEY_ENFONCEE  2  // touche enfoncée
#define EVENTKEY_RELACHEE  3  // tocuhe venant juste d'être relâchée

// états des TOUCHES  enfoncées ou relâchées  (enfoncées == TRUE)
extern BOOL Key_UP, Key_DOWN, Key_LEFT, Key_RIGHT,			// 4 flèches
	// pavé numérique
	Key_npE, Key_npSE, Key_npS, Key_npSO, Key_npO, Key_npNO, Key_npN, Key_npNE,
    // Espace, Contrôle et Alternative
	Key_SPACE, Key_LSHIFT, Key_LCTRL,
        // Escape
        Key_ESCAPE,
        // Touches caractères
        Key_K,
        // Chiffres
        Key_1, Key_2, Key_3,
		// Ponctuation & autres
		Key_BACKSLASH, 
        // 6 touches utilisés par le GammaControl
        Key_INSERT, Key_DELETE, Key_PAGEUP, Key_PAGEDOWN, Key_END, Key_HOME, Key_BACKSPACE;

// états plus affinés des TOUCHES rien=0; appuyée=1; enfoncée=2; relâchée=3;
extern BYTE Key_ENTER, Key_PLUS, Key_MOINS, Key_F1, Key_F2, Key_F3, Key_F4, Key_F5, Key_F6, Key_F7,
            Key_F8, Key_F12, Key_LALT, Key_PAUSE;


// TDIRECTINPUT ////////////////////////////////////////////////////////////////
// Objet permettant de gérer : clavier, souris et joypad					  //
class TDirectInput
{
	// VARIABLES
	public:
    	LPDIRECTINPUT		 pDI;   	// Objet Direct Input
    private:
        //--- Gestion du clavier ---
      	LPDIRECTINPUTDEVICE      pDIKeyBoard;	// Objet référencant le Clavier
        char Keys[256];				// Buffer de l'activation des touches du clavier
        bool bKeyBoard;	  			// Utilise-t-on un clavier ?
        bool bKeyboardActif;                    // Est-il actif ?
        BYTE TouchesPlus(BYTE Touch,BYTE TouchConst);	// Reçoit l'état des Tocuhes+

        //--- Gestion du Joypad ---
        LPDIRECTINPUTDEVICE2     pDIJoypad;     // Objet référencant le Joystick
        bool bJoypad;                           // Utilise-t-on un Joypad ?
        bool bJoypadActif;                      // Est-il actif
       	long axeX, axeY, axeZ;                  // Etat des 3 axes du joystick
		BYTE joyButtons[32];                    // Etat des 32 boutons du joystick
        BYTE JoyBoutonsPlus(BYTE Bouton, BYTE BoutonConst);

        HWND  DIHandle;   // Donne accès au handle de fenêtre de la fiche (fenêtre) principale

	// FONCTIONS
    public:
       	TDirectInput(HWND Hand);			// Constructeur
        ~TDirectInput(void);				// Destructeur
        void CreateDevice(BOOL bClavier,BOOL bSouris,BOOL bJoy);	// Active les devices demandés

        void AnalyseKeys();				// Reçoit et analyse les touches intéressantes

        void ReActive();                                        // Réactive tous les périphériques
        void ReActive(bool bClavier, bool bSouris, bool bJoy);  // Réactive le périph. précisé
        void UnActive();                                        // Désactive tous les périphériques
        void UnActive(bool bClavier, bool bSouris, bool bJoy);  // Désactive le périph. précisé
};
// Objet Principal du jeu
extern TDirectInput *DI;
////////////////////////////////////////////////////////////////////////////////


// Fonctions Annexes ---------------------------------------------------------//
extern void TraceErrorDI(HRESULT hErr,int Gravite);	// s'occupe es erreurs DirectInput
extern void MessageError(char *from,char *Erreur,int Gravite); // vient de "DDLib.cpp"


#endif
