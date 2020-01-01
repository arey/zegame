#ifndef DDLibEditor
#define DDLibEditor

//#include "DDraw.h"
#include "DDLib.h"

// TDIRECTSCREEN_EDITOR ////////////////////////////////////////////////////////
//   routines de gestion du mode graphique en windowed-mode					  //
//   =>  similarit� avec TDirectScreen de DDLib.cpp                           //
class TDirectScreen_Editor
{
    public:                                         // VARIABLES
        // Variables purement DirectX
    	LPDIRECTDRAW4		pDD;		    //  Objet DirectDraw
        DDSurface           	*pDDSPrimary;  	    //  Surface affich�e au 1er plan
        DDSurface               *pDDSBack;          //  Back Buffer pour le flipping
        LPDIRECTDRAWCLIPPER	pDDClipper;	    // Permet de n'afficher que sur une fiche

        // Autres variables ...
        HWND    DDHandle;       // Donne acc�s au handle de fen�tre de la fiche (fen�tre) principale
        int     Screen_Width,   // Param�tres de la r�solution
                Screen_Height,
                Screen_Bits;

    public:                                          // FONCTIONS
    	TDirectScreen_Editor(HWND Handle);	     // Constructeur (Application->Handle ou Form1->Handle)
 	~TDirectScreen_Editor(void);	             // Destructeur � faire

         // Initialisation de DirectDraw
        void     InitDDraw(int ScrWidth, int ScrHeight);
        HRESULT  FlipSurfaces();                     // Surface flipping en windowed-mode

        void 	 PutBackPixel(int X, int Y, unsigned int Color);
        void     Fill(DWORD ColFill);                                // Remplit l'�cran d'une seule couleur
        void     BackLine(int X1, int Y1, int X2, int Y2, int Col);  // trace une ligne ??? zarbi
        void     BackLine2(int x1, int y1, int x2, int y2, int Col); // trace une ligne avec Brensenhalm
        void 	 BackISOTrapeze(int X, int Y, int Col, int Typr);    // trap�ze en fonctions des donn�es isom�triques
        void 	 BackZoomISOTrapeze(int X, int Y, int Col, int Type, int Z);  // trap�ze en fonctions des donn�es isom�triques + du zoom

        void     PutPoint(int X, int Y, int Zoom, unsigned int Color);     // Affiche un carr� remplit
        void     PutCarre(int X, int Y, int longueur, unsigned int Color); // m�me chose
        void     PutCroix(int X, int Y, int longueur, unsigned int Color); // Dessine une croix
        // Affiche un rectangle � l'�cran
        void     PutRectangle(int x1, int y1, int x2, int y2, int epaisseur, unsigned int Color);
        // Affiche un rectangle � l'�cran avec clipping
        void     PutRectangleClip(int x1, int y1, int x2, int y2, int epaisseur, unsigned int Color);
        // Affiche un rectangle orient� suivant 8 directions : 0 <=> Ouest, 1 <=> S-O, ..., 7 <=> Nord-Ouest avec gestion du clipping
        void     PutRectangleOrienteClip(int x1, int y1, int x2, int y2, int epaisseur, unsigned int Color, int orientation);
};

// DirectScreen View Editor : fen�tre repr�sentant la vue du jeu
extern TDirectScreen_Editor  *DS_VE;
// DirectScreen Map Editor : fen�tre repr�sentant la vue de dessus
extern TDirectScreen_Editor  *DS_ME;
// DirectScreen View Editor : fen�tre repr�sentant la vue d'une animation
extern TDirectScreen_Editor  *DS_VA;

#endif
