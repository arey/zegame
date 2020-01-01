////////////////////////////////////////////////////////////////////////////////
//  Unit�  "D3DLib.h"  : Librarie regroupant diverses fonctions pour Direct3D //
//                       Immediate Mode rendering cr��es intialement pour     //
//                       g�rer des effets de transparences et de lumi�res.    //
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
extern bool SCREEN_DIRECT3D;	// Utilisation de Direct3D ou �mulation logicielle

// TD3DSCREEN //////////////////////////////////////////////////////////////////
// Classe permettant la gestion d'un moteur 3D                                //
class TD3DScreen : public TDirectScreen
{ public:
    LPDIRECT3D3          pD3D;             // Objet principal IDirect3D
    LPDIRECT3DDEVICE3    pD3DDevice;       // Device utilis� pour le rendu sur le back buffer
    LPDIRECT3DVIEWPORT3  pD3DViewport;     // ViewPort d�terminant comment la sc�ne 3D va �tre projet�e sur l'�cran
    LPDIRECT3DMATERIAL3	 pD3DMaterial;	   // Propri�t�s de rendu des textures
    D3DDEVICEDESC devDescHEL, devDescHAL;  // Descriptions des fonctions 3D disponibles ne hardware et software
    LPD3DRECT            pD3DViewportRect; // Rectangle d�limtant le viewport

    bool supportAlphaBlend;                // Indique si l'alpha-blending est support�
    bool supportAlphaTest;                 // Indique si l'alpha-testing est support�
    bool Hardware3D;                       // Utilise-t-on le hardware de la carte 3D

   public:
    TD3DScreen(HWND i_Hand);               // Constructeur r�up�rant le Handle de la fen�tre
	~TD3DScreen();						   // Destructeur
    void InitD3D();                        // Initialise les objets relatifs � Direct3D
    void CleanD3D();                       // D�salloue tous les objets dynamiques de cette classe
    void SearchDevice(DWORD flags);        // V�rifie si il y'a au moins un device pr�sent
    LPDIRECT3DDEVICE3 Get3DDevice();       // Renvoie un pointeur sur le device3D de cet objet Direct3D
    void InitRenderScene3D();              // Fonction � appeler avant de rendre une sc�ne 3D
    void CloseRenderScene3D();             // A appeler apr�s avoir rendu la sc�ne
    void Set3DRenderState(bool alpha_blending, bool alpha_testing, bool specular);  // Param�trage du rendu 3D
    void TraceErrorD3D(HRESULT hErr,int Gravite);	// Traite les erreurs Direct3D
	virtual void ToggleFullScreenMode();			// Permutation du mode FullScreen <-> Windowed
};
////////////////////////////////////////////////////////////////////////////////

// Objet principal g�rant une fen�tre Direct3D � l'�cran
extern TD3DScreen *D3DS;

//=== Appel� lors de l'intialisation, cette fonction v�rifie que le device poss�de un minimum des capacit�s souhait�es ===
HRESULT ConfirmDevice_AphaBlending( DDCAPS* pddDriverCaps, D3DDEVICEDESC* pd3dDeviceDesc );

//----------------------------------------------------------------------------//
// Flags utilis�s pour la m�thode SearchDevice() de la classe TD3DScreen      //
//----------------------------------------------------------------------------//
#define D3DLIB_ALPHABLENDING  0x00000001   // Utilisation de Direct3D pour de l'alphablending

// SPRITE3DTILE ////////////////////////////////////////////////////////////////
// Regroupe, en plus des informations classiques d'un sprite 2D, divers       //
// param�tres de rendu pour obtenir des effets sp�ciaux tel l'alpha-blending  //
// ou des effets de lumi�res en gouraud-shading.                              //
class Sprite3DTile : public SpriteTile
{    // Coordonn�es du sprite dans la texture calcul�es d'apr�s : CoordX + Width & CoordY + Height
     float TexU1, TexV1, TexU2, TexV2;
     // Coordonn�es du sprite sur la DDSurface 2D
     int ScrX1, ScrY1, ScrX2, ScrY2;

     D3DCOLOR cColor, cSpecular;         // Lumi�res pour tous les points du sprite
     D3DTLVERTEX vSpriteVertices[4];     // Les 4 vertices d�limitant le sprite pour le rendu de Direct3D
     LPDIRECT3DDEVICE3 pDevice;          // Pointe vers le device utilis� lors du rendu du sprite 3D
     LPDIRECT3DTEXTURE2 pTexture;        // Pointe vers la surface utilis�e comme texture
	 DDSurface* pDDSurface;				 // 2nd Pointeur vers la surface DirectDraw associ�e	

     void RefreshVertices();             // Recalcule le tableau des vertices d'apr�s la position du sprite sur l'�cran
     void RefreshPosition();             // Recalcule une partie du tableau des vertices que peut affecter le d�placement d'un sprite
  public:
     Sprite3DTile();                     // Constructeur par d�faut
     // Rajoute au sprite des informations utiles � son rendu 3D
     void Create3DSprite(LPDIRECT3DDEVICE3 pDev, D3DCOLOR i_col, D3DCOLOR i_spec, float tu1, float tv1, float tu2, float tv2, DDSurface &pFrame);
     // Fonctions permettant de positionner le sprite sur l'�cran
     void SetScreenCoord(int x, int y);                     // garde ses dimensions
     void SetScreenCoord(int x1, int y1, int x2, int y2);   // redimensionn� pour l'occasion
     void SetScreenCoord(int x, int y, float zoom);         // zoom
     // Fonctions permettant de positionner le sprite sur sa texture
     void SetTextureCoord(float u1, float v1, float u2, float v2);
     // Fonctions permettant de red�finir les param�tres de couleurs de chacun des points de la texture ind�pendamment
     void SetVertexColor(int numVertex, D3DCOLOR col, D3DCOLOR spec);
     // Fonctions affichant le sprite � l'�cran
     void SpriteRender();
     void SpriteRender(int x, int y);
	 void SetDirect3DDevice(LPDIRECT3DDEVICE3 pDev);	// Cht de device3D
};
////////////////////////////////////////////////////////////////////////////////

// TColRGBA ////////////////////////////////////////////////////////////////////
// Stock dans des nombres r�els les 4 composantes d'une couleur pouvant       //
// prendre des valeurs de 0 � 1.                                              //
class TColRGBA
{ public:
    TColRGBA() {}                    // Constructeur par d�faut
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
// Regroupe tout un tas d'effets "sp�ciaux" utilisant les comp�tences de      //
// Direct3D : effets de nuit, de brouillard, lens flare, ...                  //
class TD3DSpecialEffect
{ public:
    const enum eHOUR { HOUR_NONE, HOUR_DAY, HOUR_NIGHT, HOUR_FOG };
  private:
    //--- Attributs des filtres : nuit, jour et brouillard ---
    D3DTLVERTEX vScreenFilter[4];     // Filtre recouvrant une partie de l'�cran
    TColRGBA colNight;                // Couleur du filtre transluent pour simuler la nuit
    TColRGBA colFog;                  //    "          "         "             "   le brouillard
    TColRGBA colDay;                  //    "          "          "            "   le jour
    TColRGBA actualColor;             // Couleur actuelle du filtre
    TColRGBA nextColor;               // Couleur vers laquelle on se dirige
    TColRGBA deltaColor;              // Intervalle de couleur
    int filX, filY;                   // Position du filtre sur l'�cran
    int filWidth, filHeight;          // Dimensions du filtre
    eHOUR seFutHour;                  // Indique la p�riode de la journ�e vers laquelle on migre
    eHOUR sePresHour;                 // Indique la p�riode de la journ�e dans laquelle on se trouve
    bool transition;                  // Indique si l'on est en train de changer de p�riodes journali�res
    LPDIRECT3DDEVICE3 pDevice3D;      // Device 3D vers lequel va s'appliquer le filtre

    void RefreshColorFilter();        // Change les couleurs du filtre
  public:
    TD3DSpecialEffect();              // Constructeur par d�faut
    //--- Fonctions des filtres ---
    void CreateFilter(DWORD night, DWORD fog, int x, int y, int w, int h, LPDIRECT3DDEVICE3 pDev);  // Cr�� le filtre avec ces param�tres
    void SetState(eHOUR state);       // Change la p�riode de la journ� brusquement
    void ChangeState(eHOUR state);    // Change de p�riode en fading
    void UpdateSE();                  // Update le filtre
    void RenderFilter(bool backup = false);		// Applique le filtre sur l'�cran
	void SetDevice3D(LPDIRECT3DDEVICE3 pDev);	// Chgt de device 3D
};
////////////////////////////////////////////////////////////////////////////////

// Objet principal g�rant les effets sp�ciaux du jeu
extern TD3DSpecialEffect *D3D_SE;


//----------------------------------------------------------------------------//
#endif   // D3DLibH
