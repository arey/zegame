////////////////////////////////////////////////////////////////////////////////
//  Unit�  "DDLib.h"  : Library des principales fonctions de DirectDraw       //
//  Copyright (C) 1998 Antoine Rey    arey@insa-lyon.fr                       //
//                                    antrey@club-internet.fr                 //
//                                                                            //
//  Version      : beta 1.00                                                  //
//  Creation     : 03/10/1998 17h48                                           //
//  Last upgrade : 03/10/1998                                                 //
//                                                                            //
//	=> Commencement du header de "DDlib.cpp"			      				  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef DDLib
#define DDLib

//---------------------------------------------------------------------- include
#include "DDraw.h"      // Bibliohot�que Microsoft DirectDraw
#include <vector>       // tableau dynamique provenant de la librairie STL
#ifdef CBUILDER
#include <stdio>
#endif


//----------------------------------------------------------------------- define
#define	DDLIB_COLOR_TRANSPARENT	0x00FF00FF
#define	DDLIB_COLOR_TRANSLUENT	0x000F000F

// r�solution d'affichage
extern int SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BITS;
extern bool SCREEN_FULLSCREEN;
const enum SURFACEMEM { VIDEO_MEMORY, SYSTEM_MEMORY, TEXTURE_MEMORY };

class DDSurface;			// d�finie par la suite
struct IDirect3DDevice3;
class TFileVirtual;

// TDIRECTSCREEN /////////////////////////////////////////////////////////////////////
// Objet permettant la gestion de toutes mes routines graphiques en FULLSCREEN Mode //
// => DirectDraw Screen Graphics Control                                             //
class TDirectScreen
{
    public:                                     // VARIABLES
     // Variables purement DirectX
    	LPDIRECTDRAW4			pDD;	        //  Objet DirectDraw
        DDSurface           	*pDDSPrimary;  	//  Surface affich�e au 1er plan
        DDSurface               *pDDSBack;      //  Back Buffer pour le flipping
        LPDIRECTDRAWPALETTE     pDDPal;		    //  Palette (si utilis�e ?)
        LPDIRECTDRAWCLIPPER 	pDDClipper;	    //  Clipping

        // Autres variables ...
        HWND    DDHandle;			// Donne acc�s au handle de fen�tre de la fiche (fen�tre) principale
        BOOL    FullScreen;			// Plein Ecran
        BOOL    ModeX;				// Alloue t'on en ModeX
        BOOL    Hardware;			// Utilise-t-on l'acc�l�ration vid�o en Hard ?
        BOOL	HardwareClipping;	// Clipping r�alis� par la carte graphique
        int     Screen_Width,		// Param�tres de la r�solution
                Screen_Height,
                Screen_Bits;

		RECT	g_rcViewport;       // Position et taille de la fen�tre pour blitter
		RECT	g_rcScreen;         // d'une surface � l'�cran (en windowed-mode)
		RECT    g_rcWindow;         


    public:
    	TDirectScreen() { };                                   // FONCTIONS
    	TDirectScreen(HWND Handle);	            // Constructeur (Application->Handle ou Form1->Handle)
		~TDirectScreen(void);		    		// Destructeur � faire

        void     InitDDraw();                           // Initialisation de DirectDraw
        BOOL     LoadBitmap(const char* BitmapName);    // Charge un *.bmp de le back buffer
        BOOL     LoadPalette(const char* BitmapName);   // Charge la palette d'un bmp
        HRESULT  FlipSurfaces();                        // Surface flipping
        void     Fill(DWORD ColFill);                   // Remplit l'�cran d'une seule couleur
		void	 UpdateWindowPos();						// MaJ de la taille et de la position de la fen�tre
		virtual void ToggleFullScreenMode();			// Permutation du mode FullScreen <-> Windowed

        //** Nouveau **
        void     BackLine(int X1, int Y1, int X2, int Y2, int Col);  // trace une ligne
        void 	 BackISOTrapeze(int X, int Y, int Col, int Typr);    // trap�ze en fonctions des donn�es isom�triques

        // BOOL CheckSurfaces();                        // Regarde si la surface est perdue et la r�cup�re ds ce cas
        // UINT FlipSurfaces();                         // Surface flipping

        //--- Gamma Control DirectX 6 : depuis No�l 98 ---
        LPDIRECTDRAWGAMMACONTROL pDDGammaControl;       // objet directdraw GammaControl
        DDGAMMARAMP  lpGammaRamp;                       // structure contenant la table de WORD rouge, vert & bleu
        void ChangeGammaRamp();                         // change visuellement le gamma ramp
        void SetGammaRamp(double& Rcoeff, double& Vcoeff, double& Bcoeff);       // assigne les coefficients
        double RedCoeff, GreenCoeff, BlueCoeff;         // Coefficients servant � calculer la table
};

extern TDirectScreen  *DS;

// DDSURFACE ///////////////////////////////////////////////////////////////////
// Objet g�rant toutes les surfaces possibles :                               //
//       - Surface principale                                                 //
//       - Surface Back Buffer                                                //
//       - Surface Off-Screen                                                 //
//##ModelId=38A4889802F8
class DDSurface
{
    public:                                             // FONCTIONS
        DDSurface(TDirectScreen *pScreen,int Width,int Height,SURFACEMEM surfMem = SYSTEM_MEMORY); // Constructeur
        DDSurface(void);
        DDSurface(int Width, int Height);
        ~DDSurface();											// Destructeur
        BOOL CheckSurfaces();                           // V�rifie si la surface n'a pas �t� perdue
        bool Restaure();                                // Restaure la surface perdue
        void Draw(DDSurface* pDDSurfDest);              // Copie la surface ds une autre
        void DrawNoKey(DDSurface* pDDSurfDest);			// m�me chose sans tenir compte de la Colorkey
        void DrawNoKeyXY(DDSurface* pDDSurfDest, int X, int Y);
        void DrawFast(DDSurface* pDDSurfDest,int PosX,int PosY);  // Copie la surface en 10% + vite
        void DrawFastCK(DDSurface* pDDSurfDest,int PosX,int PosY);  // Copie la surface en 10% + vite + ColorKey
        void Lock();                                    // Renvoit un pointer vers la surface ds DDSurfDesc
        void Lock2();                                   // Renvoit un pointer vers la surface ds DDSurfDesc
        void UnLock();                                  // D�v�rouille la surface
        void PutPixel(int X,int Y,unsigned int Col);    // Affiche un pixel
        unsigned int GetPixel(int X,int Y);             // R�cup�re la valuer d'un pixel
        void ColorKey(DWORD c1,DWORD c2);               // D�fini l'intervalle du color key 16 bits
        void TextSurf(int X,int Y,COLORREF Col,LPCTSTR Texte);  // Ecrit un text en (X,Y)
        void ChangeFont(const char* Nome,int Width,int Height,int Attributes); // Change la police
        void LoadBitmap2Surf(const char* BitmapName);
        void Fill(DWORD colFill);										// efface l'�cran avec <colFill>
        void Line(int X1, int Y1, int X2, int Y2, int Col);				// Trace un segment entre 2 points
        void PutCroix(int X1, int Y1, int largeur, int longueur, int Col);  // Trace une croix

        // Nouveau depuis le 27/02/99 (pour pouvoir cr�er une surface sans allouer de m�moire)
        void AllocateMemory(LPDIRECTDRAW4 *pDD2, SURFACEMEM surfMem);        // Allloue de la m�moire pour la DirectDrawSurface
        void AllocateMemory(TDirectScreen *pScreen, SURFACEMEM surfMem);
    public:                                             // VARIABLES
        LPDIRECTDRAWSURFACE4   pDDSurf;                 // Surface IDirectDrawSurface3
        DDSURFACEDESC2         DDSurfDesc;              // Infos sur la surface

        int PixWidth;                                   // Dimensions de la surface
        int PixHeight;
        HDC SurfDC;                                     // GDI compatible handle
        RECT SrcRect;
        RECT DestRect;
        DWORD ColKey1;                                  // Interval de couleurs
        DWORD ColKey2;                                  // => Color Key
        HFONT SurfFont;                                 // Police de caract�re

        // Nouveau depuis le 12/04/99 (pour pouvoir charger des textures sans allouer de m�moire)
        bool isAllocateTexture;   // La texture est-elle allou�e ? (par IDirect3DTexture2::Load)
        bool isTextureDirect3D;   // Indique si cette surface peut �tre utilis�e comme texture par Direct3D

};

extern unsigned long RGBto16(unsigned long col); // Transforme (8:8:8) en (5:6:5)

// Constitution du fichier "*.spi"  => SPrites Informations
// Donn�es utilis�es lors de sa relecture
typedef struct  // En-t�te du fichier  *.spi <=> Description de la frame
    {   char Description [12];   // Nom du fichier
        int NbSprites;           // Nombre de sprites
        int  CoulFond,           // Couleur de fond pour la transparence
             CoulOmbr;           // Couleur de l'ombre pour l'opacit�
        unsigned int cfRGB,coRGB;   // Composantes des couleurs pr�c�dentes
    } FrInfo;

typedef struct  // 1 structure pour chaque sprite de la frame
    {
        int Num,                //  Num�ro du sprite en cours
            Width,Height,       //  Taille (X,Y) du sprite
            CoordX,CoordY,      // Position du sprite sur le SpriteScreen
            GravX,GravY;        // Position du centre de gravit� (X,Y)
    } SprInfo;

//============================= D�finition des Classes =======================//

// D�claration d'une classe que l'on utilise dans SpriteTile mais qui est
// d�finie plus bas
class CGeneralSpriteInfos;

//  SPRITEFRAME ////////////////////////////////////////////////////////////////////////
// Simple surface contenant divers Tiles de diff�rentes taille rep�r�e par un num�ro  //
class SpriteFrame : public DDSurface
{

  public:
    // Constructeur
    SpriteFrame(TDirectScreen *pScreen,int Width,int Height,SURFACEMEM surfMem = SYSTEM_MEMORY);
    // Constructeur n'allouant pas la m�moire pour IDirectDrawSurface
    SpriteFrame(int Width,int Height);
	// Destructeur
	virtual ~SpriteFrame();

    // Pour cr�er la frame, il faut le nom du *.bmp et du *.spi associ�s et un num�ro l'identifiant
    void CreateFrame(TDirectScreen *Screen,void* STLperso, const char* BitmapName,const char* SpiName,int NbFrame,SpriteFrame *New_Frame, int i_IDFrame = 0,bool BmpToMem=true);

  public:
    int NumFrame;               // n� de la frame
    char Description [12];      // Nom du fichier
    int NbSprites;              // Nombre de sprites qu'il contient
    int CoulFond,CoulOmbr;      // n� Couleur de fond et de l'ombre
    unsigned int cfRGB,coRGB;   // composantes des couleurs

    //*** New depuis le 25/02/99 : depuis l'ajout des anims superposables ***
    int IDFrame;                // n� d'ID de la frame
    bool BmpLoadIntoMemory;     // indique si le .bmp a �t� charg� en m�moire
    bool BmpAlwaysIntoMemory;   // indique si le .bmp doit toujours r�sider en m�moire
    char *BitmapPathName;       // chemin + nom du bitmap (au cas o� faudra le charger plus tard)
    int Semaphore;              // Lorsqu'il est � 0, personne ne s'en sert.
    // Si un perso se sert de l'image il incr�mente ce compteur; et quand il a fini avec, il le d�cr�mente.

	//*** New depuis le 26/06/99
    int iStartSpriteIndex;		// Index de d�but du 1er sprite de cette frame dans sa librairie    

    void ReserveMemoryAndLoadBmp(LPDIRECTDRAW4 *pDD2);   // Charge le bmp dans la IDirectDrawSurface
    void DesallocateMemory(LPDIRECTDRAW4 *pDD2);         // D�salloue la m�moire utilis�e par le bmp
	void ChangeVideoMode(LPDIRECTDRAW4 *pDD4);			 // Changement de Mode Vid�o

    
    SpriteFrame *SF_Suiv;       // Pointe vers la Frame Suivante
    SpriteFrame *SF_Prec;       // Pointe vers la Frame le pr�cedant
};
////////////////////////////////////////////////////////////////////////////////

#define SPRITE_TILE_2D		1
#define SPRITE_TILE_3D		2

// SPRITETILE //////////////////////////////////////////////////////////////////
// Comporte toutes les informations relatives � un sprite                     //
class  SpriteTile
{   public:
      SpriteTile(void);
      void DrawFast(int ScrX,int ScrY,DDSurface* pSurface); // Affiche le sprite

    public:
      int NumSprite;            // N� de R�f�rence du Sprite
      int TypeSprite;			// Cat�gorie de sprite auxquel appartient ce sprite 2D ou 3D ?
      int CoordX,CoordY;        // Position du sprite sur la frame;
      int Width,Height;         // Dimensions du Sprite
      int GravX,GravY;          // Centre de gravit�
      int CoulFond,CoulOmbr;    // n� de couleur de l'ombre et du fond
      unsigned int cfRGB,coRGB; // composantes des couleurs
      RECT SrcRect;				// Rectangle o� se trouve le sprite sur sa frame
	  unsigned char AlphaBlend;	// Facteur de transluence du sprite

      SpriteTile *Suiv;         // Pointe vers le Sprite suivant
      SpriteTile *Prec;         // Pointe vers le Sprite pr�c�dent
      SpriteFrame *pFrame;      // Pointe vers la Frame sur lequel il se trouve
      int NumFrame;             // n� de cet frame (au cas o�)
};
////////////////////////////////////////////////////////////////////////////////

// SPRITETILEPERSO /////////////////////////////////////////////////////////////
class SpriteTilePerso : public SpriteTile
{  //*** New : editeur de perso 14/12/98 ***
   public:
      SpriteTilePerso();
      CGeneralSpriteInfos  *GSprInf;   // Informations g�n�rales sur le sprite : collisions ...
};
////////////////////////////////////////////////////////////////////////////////

// SPRITELIB ///////////////////////////////////////////////////////////////////
// Liste de tous les sprites                                                  //
class SpriteTileLib
{
  public:
    SpriteTileLib(void);		// Constructeur par d�faut
	virtual ~SpriteTileLib();	// Destructeur
    // Ajoute un Sprite � la liste
    virtual void AddSprite(SprInfo InfSprite,int CF,int CO,unsigned int RGBcf,unsigned int RGBco
                   ,int NumFr,SpriteFrame *pFr);
    // Cr�e le tableau dynamique de pointers en utilisant NbSprites
    void CreateSprIndex(void);

	void AddSpriteTile3D(SpriteTile* sprite3D);			// Ajoute un sprite 3D � la liste vSprite3DList
	void ChangeD3DDevice(IDirect3DDevice3* device3D);	// Changement du device3D de toute la liste

  public:

    SpriteTile *SL_List;      // D�but de la liste
    int NbSprites;            // Nombre de sprites
    SpriteTile** SprIndex;    // Tableau dynamique de pointers indexant chaque sprite d'apr�s leur n�

    // Ajout le 05/12/98
    unsigned int nbFrames;    // nb de frames utilis�es par la lib de sprites
    char **fileInaBmpName;    // tableau contenant le nom court des fichiers utilis�s

	// Ajout le 21/08/99 : pour g�rer le cht dynamique fullscreen/windowed mode
	std::vector<SpriteTile*> vSprite3DList;		// Liste de tous les sprites 3D utilis�e afin de changer leur device 3D
};


class SpriteTilePersoLib : public SpriteTileLib
{
  public:
    // Ajoute un Sprite � la liste
    void AddSprite(SprInfo InfSprite,int CF,int CO,unsigned int RGBcf,unsigned int RGBco
                   ,int NumFr,SpriteFrame *pFr);
};


// SPRITEFRAMELIST /////////////////////////////////////////////////////////////
// Liste de toutes les frames charg�es                                        //
class SpriteFrameList
{
  public:
    SpriteFrameList(void);      // Constructeur par d�faut
	~SpriteFrameList(void);		// Destructeur
    SpriteFrame* AddFrame(TDirectScreen *Screen,void* STLperso,const char* BitmapName,const char* SpiName, int i_IDFrame=0,bool BmpToMem=true, bool texture = false); // Ajoute une frame � la liste
	SpriteFrame* GetFrame(int numIDFrame);
	void Clear();					// Supprime toutes les SpriteFrame de la liste
	void ChangeSpriteFrameVideoMode(LPDIRECTDRAW4 *pDD4);		// Chgt de Mode Vid�o

  public:
    SpriteFrame *SF_List;       // D�but de la liste
    int NbFrame;                // Nombre de frames
};
////////////////////////////////////////////////////////////////////////////////

// Permet de coder une couleur sur 16 bits (5:6:5) ----------------------------//
typedef struct
{
    unsigned long Blue;         // cod� sur 5 bits  => max de 31
    unsigned long Green;        // cod� sur 6 bits  => max de 63
    unsigned long Red;          // cod� sur 5 bits  => max de 31
} tHighColor;


extern SpriteFrameList *SFL;  // Liste officielle de la gestion des frames

extern void SpriteDrawFast(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso);
extern void Sprite3DDraw(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso);
extern void SpriteDraw(int ScX,int ScY,int NumSprite,DDSurface *pSurface,SpriteTileLib* STLperso);
extern int GravX(int NumSprite,SpriteTilePersoLib* STLperso);
extern int GravY(int NumSprite,SpriteTilePersoLib* STLperso);
extern unsigned long RGBto16(unsigned long Red,unsigned long Green,unsigned long Blue);
extern unsigned long RGB24(unsigned long Red,unsigned long Green,unsigned long Blue);

////////////////////////////////////////////////////////////////////////////////
// Compl�ments d'informations sur les sprites :                               //
// collisions ...                                                             //
// => utilis� � partir de l'�diteur de persos du 14/12/98                     //
////////////////////////////////////////////////////////////////////////////////

//=== Zone rectangulaire du sol ==============================================//
// => le 1er point est celui en haut � gauche
struct ZoneRect
{      // 2 points
       int x1, y1;     // haut � gauche
       int x2, y2;     // bas � droite
       bool Enabled;
       ZoneRect() { x1 = y1 = x2 = y2 = -1; Enabled = false;};    // Constructeur
       // Permet de remplir les points du rectangle rapidement
       void remplitRect(int u1, int v1, int u2, int v2)
           { x1 = u1; x2 = u2; y1 = v1; y2 = v2; Enabled = true; };
       // Sauvegarde les 4 coordonn�es des 2 points du rectangle
       void SaveToFile(FILE &fichier);
       // Charge les 4 coordonn�es des 2 points du rectangle
       void LoadFromFile(FILE &fichier);
	   void LoadFromFile(TFileVirtual &fichier);
};

//=== Zone rectangulaire orient� 8 directions ================================//
// => Orientation des points commencant par celui le plus haut puis en suivant
//    le sens des aiguilles d'une montre.
struct ZoneOrientRect
{      // 4 points
       int x1, y1, x2, y2;
       int x3, y3, x4, y4;
       bool Enabled;
       // Constructeur
       ZoneOrientRect() { x1 = y1 = x2 = y2 = x3 = y3 = x4 = y4 = -1; Enabled = false;};
       // Permet de remplir les points du rectangle rapidement
       void remplitRect(int u1, int v1, int u2, int v2, int u3, int v3, int u4, int v4)
           { x1 = u1; x2 = u2; y1 = v1; y2 = v2;
             x3 = u3; x4 = u4; y3 = v3; y4 = v4;  Enabled = true; };
       // Sauvegarde les 4 coordonn�es des 2 points du rectangle
       void SaveToFile(FILE &fichier);
       // Charge les 4 coordonn�es des 2 points du rectangle
       void LoadFromFile(FILE &fichier);
	   void LoadFromFile(TFileVirtual &fichier);
};

//=== Point ==================================================================//
struct ZonePoint
{      // 2 coordonn�es
       int x, y;
       bool Enabled;
       ZonePoint() { x = y = -1; Enabled = false; }
       void remplitPoint(int u, int v)     // Set les points
           { x = u; y = v; Enabled = true; }
       void SaveToFile(FILE &fichier);     // Sauve les points dans un fichier d�j� ouvert
       void LoadFromFile(FILE &fichier);   // Charge les points � partir d'un fichier
	   void LoadFromFile(TFileVirtual &fichier);
};
//----------------------------------------------------------------------------//

//=== Liste d'anim superpos�es pour un type d'objet ==========================//
class ZoneAnimSuperp
{ private:
     typedef struct sanimsuperp
     {  int x, y;                  // coordonn�es relatives par rapport au centre de gravit� du sprite superpos�
        int numIDTypeObjSprite;    // n�ID du type d'objet pouvant se supperposer � ce sprite
        int numIDSpriteSuperpose;  // n�ID du sprite superpose correspondant
        sanimsuperp(int &i_x,int &i_y, int &i_numIDT, int &i_numIDS):   // Constructeur
           x(i_x),y(i_y),numIDTypeObjSprite(i_numIDT),numIDSpriteSuperpose(i_numIDS)  { };
     } sAnimSuperp;

     std::vector <sAnimSuperp> TabAnimSuperp;          // Tableau d'anim superpos�es

  public:
     void Add(int &i_x, int &i_y, int &i_numIDT, int &i_numIDS);   // Ajoute un sprite superposable au vecteur
     void SaveToFile(FILE &fichier);              // Sauvegarde 3*4 entiers par anims superposables + 1 int d'en-t�te
     void LoadFromFile(FILE &fichier);            // Charge les anims superposables dans le vecteur
	 void LoadFromFile(TFileVirtual &fichier);            
     bool Existe(int num_ID);                     // Renvoie vrai si le sAnimSuperp correspondant � ce n� existe
     void GetCoord(int num_ID, int &xs,int &ys);  // Renvoie les coords du sprite de n�num_ID
     void SetCoord(int num_ID, int &xs,int &ys);  // Modifie les coordonn�es du sprite de n�num_ID
     int  GetIDS(int num_IDT);                    // Rennvoie le n�ID du sprite superposable correspondant au n�ID du type d'objet
};
//----------------------------------------------------------------------------//

//=== Renseignements valables pour tous les sprites quel qu'ils soient =======//
// -> d�cors, persos, animos, ...
// => classe utilis�e pour l'h�ritage
// => 100 octets r�serv�s par sprites pour ne pas � avoir � changer de format
//    toutes les 2 secondes
class CGeneralSpriteInfos
{  public :
      CGeneralSpriteInfos();              // Constructeur
      void SaveToFile(FILE &fichier);     // Sauvegarde les informations contenue de cette structure dans le fichier pass� en param�tre
      void LoadFromFile(FILE &fichier);   // Charge les informations contenue de cette structure dans le fichier pass� en param�tre
	  void LoadFromFile(TFileVirtual &fichier); 
      static float Version;               // n� de version

      // Humains, Animaux � 4 pattes, ...
      // Zone d�limitant le contour au sol d'un sprite => utilis� dans les collisions
      ZoneRect ZoneSol;				// taille de 4x4 = 16 octets
      // Point d�signant le centre du sprite au sol => utilis� pour placer le personnage
      ZonePoint GravSol;			// taille de  2x4 =  8 octets
      // Liste des sprites superposables : positions + n�ID
      ZoneAnimSuperp ListAnimSuperp;
      // Zone d�limitant les pieds du personnage => utilis� pour les collisions
      ZoneOrientRect ZonePied;		// taille de 8x4 = 32 octets

      int temp [19];
};
//----------------------------------------------------------------------------//

// S'occupe d'interrompre le programme si probl�mes
void MessageError(char *from,char *Erreur,int Gravite);
void TraceErrorDD(HRESULT hErr,int Gravite);



#endif
