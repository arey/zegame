#ifndef DDLibEditorH
#define DDLibEditorH

#include "DDraw.h"
#include "DDLib.h"

// TDIRECTSCREEN_EDITOR ////////////////////////////////////////////////////////
//   routines de gestion du mode graphique en windowed-mode					  //
//   =>  similarité avec TDirectScreen de DDLib.cpp                           //
class TDirectScreen_Editor : public TDirectScreen
{
    public:                                         // VARIABLES
        // Variables purement DirectX
     /*	LPDIRECTDRAW4			pDD;	    //  Objet DirectDraw
        DDSurface           	*pDDSPrimary;  	    //  Surface affichée au 1er plan
        DDSurface               *pDDSBack;          //  Back Buffer pour le flipping
        LPDIRECTDRAWCLIPPER		pDDClipper; // Permet de n'afficher que sur une fiche

        // Autres variables ...
        HWND    DDHandle;       // Donne accès au handle de fenêtre de la fiche (fenêtre) principale
        int     Screen_Width,   // Paramètres de la résolution
                Screen_Height,
                Screen_Bits; */

    public:                                         // FONCTIONS
    	TDirectScreen_Editor(HWND Handle);	    // Constructeur (Application->Handle ou Form1->Handle)
 	~TDirectScreen_Editor(void);	            // Destructeur à faire

         // Initialisation de DirectDraw
        void     InitDDraw(int ScrWidth, int ScrHeight);
        HRESULT  FlipSurfaces();                    // Surface flipping en windowed-mode

        void 	 PutBackPixel(int X, int Y, unsigned int Color);
        void     PutBackText(int X, int Y, unsigned int Color, LPCTSTR Texte);
        void     Fill(DWORD ColFill);                               // Remplit l'écran d'une seule couleur
        void     BackLine(int X1, int Y1, int X2, int Y2, int Col); // trace une ligne
        void 	 BackISOTrapeze(int X, int Y, int Col, int Typr);   // trapèze en fonctions des données isométriques
		void 	 BackCircle(int X, int Y, int R, int Col);			// Cercle         
};

// DirectScreen View Editor : fenêtre représentant la vue du jeu
extern TDirectScreen_Editor  *DS_VE;
// DirectScreen Map Editor : fenêtre représentant la vue de dessus
extern TDirectScreen_Editor  *DS_ME;

#endif // DDLibEditorH


