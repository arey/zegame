////////////////////////////////////////////////////////////////////////////////
//  Unité  "D3DLib.h"  : Librarie regroupant diverses fonctions pour Direct3D //
//                       Immediate Mode rendering créées intialement pour     //
//                       gérer des effets de transparences et de lumières.    //
//                                                                            //
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//                                                                            //
//  Version      : beta 0.01                                                  //
//  Creation     : 10/04/1999                                                 //
//  Last upgrade : 10/04/1999                                                 //
//                                                                            //
// Rmq : s'appuye sur ma librairie DirectDraw DDLib.h                         //
////////////////////////////////////////////////////////////////////////////////
#ifndef D3DLibH
#define D3DLibH

// --------------------------------------------------------------------- include
#include "DDLib.h"				 // Libraire DirectDraw
#include "D3D.h"                 // Librairie SDK de Direct3D

// -------------------------------------------------------------------- variable
extern bool SCREEN_DIRECT3D;	// Utilisation de Direct3D ou émulation logicielle

// TD3DSCREEN //////////////////////////////////////////////////////////////////
// Classe permettant la gestion d'un moteur 3D                                //
class TD3DScreen : public TDirectScreen
{ public:
    LPDIRECT3D3          pD3D;             // Objet principal IDirect3D
    LPDIRECT3DDEVICE3    pD3DDevice;       // Device utilisé pour le rendu sur le back buffer
    LPDIRECT3DVIEWPORT3  pD3DViewport;     // ViewPort déterminant comment la scène 3D va être projetée sur l'écran
    LPDIRECT3DMATERIAL3	 pD3DMaterial;	   // Propriétés de rendu des textures
    D3DDEVICEDESC devDescHEL, devDescHAL;  // Descriptions des fonctions 3D disponibles ne hardware et software
    LPD3DRECT            pD3DViewportRect; // Rectangle délimtant le viewport

    bool supportAlphaBlend;                // Indique si l'alpha-blending est supporté
    bool supportAlphaTest;                 // Indique si l'alpha-testing est supporté
    bool Hardware3D;                       // Utilise-t-on le hardware de la carte 3D

   public:
    TD3DScreen(HWND i_Hand);               // Constructeur réupérant le Handle de la fenêtre
	~TD3DScreen();						   // Destructeur
    void InitD3D();                        // Initialise les objets relatifs à Direct3D
    void CleanD3D();                       // Désalloue tous les objets dynamiques de cette classe
    void SearchDevice(DWORD flags);        // Vérifie si il y'a au moins un device présent
    LPDIRECT3DDEVICE3 Get3DDevice();       // Renvoie un pointeur sur le device3D de cet objet Direct3D
    void InitRenderScene3D();              // Fonction à appeler avant de rendre une scène 3D
    void CloseRenderScene3D();             // A appeler aprés avoir rendu la scène
    void Set3DRenderState(bool alpha_blending, bool alpha_testing, bool specular);  // Paramétrage du rendu 3D
    void TraceErrorD3D(HRESULT hErr,int Gravite);	// Traite les erreurs Direct3D
	virtual void ToggleFullScreenMode();			// Permutation du mode FullScreen <-> Windowed
};
////////////////////////////////////////////////////////////////////////////////

// Objet principal gérant une fenêtre Direct3D à l'écran
extern TD3DScreen *D3DS;

//=== Appelé lors de l'intialisation, cette fonction vérifie que le device possède un minimum des capacités souhaitées ===
HRESULT ConfirmDevice_AphaBlending( DDCAPS* pddDriverCaps, D3DDEVICEDESC* pd3dDeviceDesc );

//----------------------------------------------------------------------------//
// Flags utilisés pour la méthode SearchDevice() de la classe TD3DScreen      //
//----------------------------------------------------------------------------//
#define D3DLIB_ALPHABLENDING  0x00000001   // Utilisation de Direct3D pour de l'alphablending

// SPRITE3DTILE ////////////////////////////////////////////////////////////////
// Regroupe, en plus des informations classiques d'un sprite 2D, divers       //
// paramètres de rendu pour obtenir des effets spéciaux tel l'alpha-blending  //
// ou des effets de lumières en gouraud-shading.                              //
class Sprite3DTile : public SpriteTile
{    // Coordonnées du sprite dans la texture calculées d'aprés : CoordX + Width & CoordY + Height
     float TexU1, TexV1, TexU2, TexV2;
     // Coordonnées du sprite sur la DDSurface 2D
     int ScrX1, ScrY1, ScrX2, ScrY2;

     D3DCOLOR cColor, cSpecular;         // Lumières pour tous les points du sprite
     D3DTLVERTEX vSpriteVertices[4];     // Les 4 vertices délimitant le sprite pour le rendu de Direct3D
     LPDIRECT3DDEVICE3 pDevice;          // Pointe vers le device utilisé lors du rendu du sprite 3D
     LPDIRECT3DTEXTURE2 pTexture;        // Pointe vers la surface utilisée comme texture
	 DDSurface* pDDSurface;				 // 2nd Pointeur vers la surface DirectDraw associée	

     void RefreshVertices();             // Recalcule le tableau des vertices d'aprés la position du sprite sur l'écran
     void RefreshPosition();             // Recalcule une partie du tableau des vertices que peut affecter le déplacement d'un sprite
  public:
     Sprite3DTile();                     // Constructeur par défaut
     // Rajoute au sprite des informations utiles à son rendu 3D
     void Create3DSprite(LPDIRECT3DDEVICE3 pDev, D3DCOLOR i_col, D3DCOLOR i_spec, float tu1, float tv1, float tu2, float tv2, DDSurface &pFrame);
     // Fonctions permettant de positionner le sprite sur l'écran
     void SetScreenCoord(int x, int y);                     // garde ses dimensions
     void SetScreenCoord(int x1, int y1, int x2, int y2);   // redimensionné pour l'occasion
     void SetScreenCoord(int x, int y, float zoom);         // zoom
     // Fonctions permettant de positionner le sprite sur sa texture
     void SetTextureCoord(float u1, float v1, float u2, float v2);
     // Fonctions permettant de redéfinir les paramètres de couleurs de chacun des points de la texture indépendamment
     void SetVertexColor(int numVertex, D3DCOLOR col, D3DCOLOR spec);
     // Fonctions affichant le sprite à l'écran
     void SpriteRender();
     void SpriteRender(int x, int y);
	 void SetDirect3DDevice(LPDIRECT3DDEVICE3 pDev);	// Cht de device3D
};
////////////////////////////////////////////////////////////////////////////////

// TColRGBA ////////////////////////////////////////////////////////////////////
// Stock dans des nombres réels les 4 composantes d'une couleur pouvant       //
// prendre des valeurs de 0 à 1.                                              //
class TColRGBA
{ public:
    TColRGBA() {}                    // Constructeur par défaut
    TColRGBA(float i_r, float i_g, float i_b, float i_a = 0)
      { red = i_r; green = i_g; blue = i_b; alpha = i_a; }
    TColRGBA(DWORD dwCol)
      { SetColor(dwCol); }
    void SetColor(DWORD dwCol)
      {  red   = (float) RGBA_GETRED(dwCol)   / 255;
	 green = (float) RGBA_GETGREEN(dwCol) / 255;
	 blue  = (float) RGBA_GETBLUE(dwCol)  / 255;
	 alpha = (float) RGBA_GETALPHA(dwCol) / 255;
      }
    void SetColor(float i_r, float i_g, float i_b, float i_a = 0)
      { red = i_r; green = i_g; blue = i_b; alpha = i_a; }
    bool IsEgale(TColRGBA c)
      {	return ( c.red == red && c.green == green && c.blue == blue && c.alpha == alpha );
      }
    bool operator == (TColRGBA c)
      {	return ( c.red == red && c.green == green && c.blue == blue && c.alpha == alpha );
      }
    bool IsSimilaire(TColRGBA c)
      {  return (     c.red   >= red  -0.01  &&  c.red   <= red   +0.01
                  &&  c.green >= green-0.01  &&  c.green <= green +0.01
                  &&  c.blue  >= blue -0.01  &&  c.blue  <= blue  +0.01
                  &&  c.alpha >= alpha-0.01  &&  c.alpha <= alpha +0.01 );
      }
    D3DCOLOR GetColor()
      { return D3DRGBA(red, green, blue ,alpha); }

    friend class TD3DSpecialEffect;

  protected:
    float red, green ,blue ,alpha;
};
////////////////////////////////////////////////////////////////////////////////


// TD3DSPECIALEFFECT ///////////////////////////////////////////////////////////
// Regroupe tout un tas d'effets "spéciaux" utilisant les compétences de      //
// Direct3D : effets de nuit, de brouillard, lens flare, ...                  //
class TD3DSpecialEffect
{ public:
    const enum eHOUR { HOUR_NONE, HOUR_DAY, HOUR_NIGHT, HOUR_FOG };
  private:
    //--- Attributs des filtres : nuit, jour et brouillard ---
    D3DTLVERTEX vScreenFilter[4];     // Filtre recouvrant une partie de l'écran
    TColRGBA colNight;                // Couleur du filtre transluent pour simuler la nuit
    TColRGBA colFog;                  //    "          "         "             "   le brouillard
    TColRGBA colDay;                  //    "          "          "            "   le jour
    TColRGBA actualColor;             // Couleur actuelle du filtre
    TColRGBA nextColor;               // Couleur vers laquelle on se dirige
    TColRGBA deltaColor;              // Intervalle de couleur
    int filX, filY;                   // Position du filtre sur l'écran
    int filWidth, filHeight;          // Dimensions du filtre
    eHOUR seFutHour;                  // Indique la période de la journée vers laquelle on migre
    eHOUR sePresHour;                 // Indique la période de la journée dans laquelle on se trouve
    bool transition;                  // Indique si l'on est en train de changer de périodes journalières
    LPDIRECT3DDEVICE3 pDevice3D;      // Device 3D vers lequel va s'appliquer le filtre

    void RefreshColorFilter();        // Change les couleurs du filtre
  public:
    TD3DSpecialEffect();              // Constructeur par défaut
    //--- Fonctions des filtres ---
    void CreateFilter(DWORD night, DWORD fog, int x, int y, int w, int h, LPDIRECT3DDEVICE3 pDev);  // Créé le filtre avec ces paramètres
    void SetState(eHOUR state);       // Change la période de la journé brusquement
    void ChangeState(eHOUR state);    // Change de période en fading
    void UpdateSE();                  // Update le filtre
    void RenderFilter(bool backup = false);		// Applique le filtre sur l'écran
	void SetDevice3D(LPDIRECT3DDEVICE3 pDev);	// Chgt de device 3D
};
////////////////////////////////////////////////////////////////////////////////

// Objet principal gérant les effets spéciaux du jeu
extern TD3DSpecialEffect *D3D_SE;


//----------------------------------------------------------------------------//
#endif   // D3DLibH
