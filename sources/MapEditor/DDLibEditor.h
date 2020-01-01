#ifndef DDLibEditorH
#define DDLibEditorH

#include "DDraw.h"
#include "DDLib.h"

// TDIRECTSCREEN_EDITOR ////////////////////////////////////////////////////////
//   routines de gestion du mode graphique en windowed-mode					  //
//   =>  similarit� avec TDirectScreen de DDLib.cpp                           //
class TDirectScreen_Editor : public TDirectScreen
{
    public:                                         // VARIABLES
        // Variables purement DirectX
     /*	LPDIRECTDRAW4			pDD;	    //  Objet DirectDraw
        DDSurface           	*pDDSPrimary;  	    //  Surface affich�e au 1er plan
        DDSurface               *pDDSBack;          //  Back Buffer pour le flipping
        LPDIRECTDRAWCLIPPER		pDDClipper; // Permet de n'afficher que sur une fiche

        // Autres variables ...
        HWND    DDHandle;       // Donne acc�s au handle de fen�tre de la fiche (fen�tre) principale
        int     Screen_Width,   // Param�tres de la r�solution
                Screen_Height,
                Screen_Bits; */

    public:                                         // FONCTIONS
    	TDirectScreen_Editor(HWND Handle);	    // Constructeur (Application->Handle ou Form1->Handle)
 	~TDirectScreen_Editor(void);	            // Destructeur � faire

         // Initialisation de DirectDraw
        void     InitDDraw(int ScrWidth, int ScrHeight);
        HRESULT  FlipSurfaces();                    // Surface flipping en windowed-mode

        void 	 PutBackPixel(int X, int Y, unsigned int Color);
        void     PutBackText(int X, int Y, unsigned int Color, LPCTSTR Texte);
        void     Fill(DWORD ColFill);                               // Remplit l'�cran d'une seule couleur
        void     BackLine(int X1, int Y1, int X2, int Y2, int Col); // trace une ligne
        void 	 BackISOTrapeze(int X, int Y, int Col, int Typr);   // trap�ze en fonctions des donn�es isom�triques
		void 	 BackCircle(int X, int Y, int R, int Col);			// Cercle         
};

// DirectScreen View Editor : fen�tre repr�sentant la vue du jeu
extern TDirectScreen_Editor  *DS_VE;
// DirectScreen Map Editor : fen�tre repr�sentant la vue de dessus
extern TDirectScreen_Editor  *DS_ME;

#endif // DDLibEditorH


