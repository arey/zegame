////////////////////////////////////////////////////////////////////////////////
//  Unit� "ImageFile.h"														  //	                                                                            //
//	Description : Classes d'images de diff�rents formats				      //
//                                                                            //
//  Copyright (C) 1999 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : beta 0.01                                                  //
//  Creation     : 27/11/1999 		                                          //
//  Last upgrade : <none>	                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef ImageFileH
#define ImageFileH

//---------------------------------------------------------------------- Include
#ifdef MICROSOFT
#pragma warning(disable : 4100 4663 4018)  
#endif

//------------------------------------------------------------------------ Class
class TFileVirtual;


//----------------------------------------------------------------------- Define
#define IMAGE_BPP_8			 8
#define IMAGE_BPP_16		16
#define IMAGE_BPP_24		24
#define IMAGE_BPP_32		32
#define IMAGE_BPP_DIRECTX	16

#define TYPE_IMAGE_UNKNOW	0
#define TYPE_IMAGE_BMP		1
#define TYPE_IMAGE_TGA		2
#define TYPE_IMAGE_GIF		3
#define TYPE_IMAGE_JPG		4
#define TYPE_IMAGE_PCX		5
#define TYPE_IMAGE_PNG		6
#define TYPE_IMAGE_TIF		7
#define TYPE_IMAGE_LBM		8

#define IMAGE_HEADER_SIZE_BMP	2

class TImageDirectX;
class TImage16b;

// TSTDIMAGE ///////////////////////////////////////////////////////////////////
//##ModelId=38A488C00398
class TStdImage
{   //--------------------------------------------------- Attributs de la classe
  protected:	
	//##ModelId=38A488C0039D
	int iImageType;			// Format de l'image initial : bmp, tga, jpg ...
	//##ModelId=38A488C0039C
	void *lpData;			// Donn�es binaires de l'image (8,16,24 ou 32 bits)
	//##ModelId=38A488C0039B
	void *lpPal;			// Palette de couleur (Images 8bits)
	int iWidth, iHeight;	// Longeur et Hauteur de l'image
	//##ModelId=38A488C0039A
	int iBitPerPix;			// Nb de bits par pixel
	//##ModelId=38A488C00399
	int iBpW;				// Nb d'OCTETS par longueur d'image

	friend class TImageLoader;  // D�claration d'une classe amie
		
 	//---------------------------------------------------- M�thodes de la classe
  public:
	//##ModelId=38A488C003CF
	TStdImage();			// Constructeur par d�faut
	//##ModelId=38A488C003CE
    virtual ~TStdImage();	// Destructeur
	//##ModelId=38A488C003CB
	virtual void InitImage(int w, int h) = 0;	// Alloue l'espace n�cessaire aux donn�es de l'image
	
	// --- MaJ de param�tres de l'image ---
	//##ModelId=38A488C003A4
	inline void SetImageType(int it) { iImageType = it; }
	
	// --- Renvoit des param�tres de l'image ---
	//##ModelId=38A488C003A3
	virtual void* GetDataAddr();
	//##ModelId=38A488C003A2
	virtual void* GetPalAddr();
	//##ModelId=38A488C003A1
	inline int GetWidth() { return iWidth; }
	//##ModelId=38A488C003A0
	inline int GetHeight() { return iHeight; }
	//##ModelId=38A488C0039F
	inline int GetBitPerPix() { return iBitPerPix; }
	//##ModelId=38A488C0039E
	inline int GetPitch() { return iBpW; }	
};
/////////////////////////////// FIN de TSTDIMAGE ///////////////////////////////

// TIMAGE8B ////////////////////////////////////////////////////////////////////
//##ModelId=38A488A801A4
class TImage8b : public TStdImage
{   //--------------------------------------------------- Attributs de la classe
	
 	//---------------------------------------------------- M�thodes de la classe
  public:
	//##ModelId=38A488A801AC
	TImage8b();						// Constructeur par d�faut
	//##ModelId=38A488A801AD
	TImage8b(int w, int h);			// 2nd Constructeur
	//##ModelId=38A488A801AB
    virtual ~TImage8b();			// Destructeur
	//##ModelId=38A488A801A8
	virtual void InitImage(int w, int h);	// Alloue l'espace n�cessaire aux donn�es de l'image
	//##ModelId=38A488A801A6
	virtual bool Copy(TImage16b* image);	// Copie cette image dans une surface directX
};
/////////////////////////////// FIN de TIMAGE8B ////////////////////////////////

// TIMAGE16B ///////////////////////////////////////////////////////////////////
//##ModelId=38A4888001AE
class TImage16b : public TStdImage
{   //--------------------------------------------------- Attributs de la classe
	
 	//---------------------------------------------------- M�thodes de la classe
  public:
	//##ModelId=38A4888001EF
	TImage16b();						// Constructeur par d�faut
	//##ModelId=38A4888001F0
	TImage16b(int w, int h);			// 2nd Constructeur 
	//##ModelId=38A4888001F3
	TImage16b(TImage8b* image);			// 3ieme Constructeur : copie l'image se trouvant dans l'image 8bits
	//##ModelId=38A4888001EE
	virtual ~TImage16b();				// Destructeur	
	//##ModelId=38A4888001EB
	virtual void InitImage(int w, int h);	// Alloue l'espace n�cessaire aux donn�es de l'image    
};
////////////////////////////// FIN de TIMAGE16B ////////////////////////////////

// TIMAGE24B ///////////////////////////////////////////////////////////////////
//##ModelId=38A4889C00E6
class TImage24b : public TStdImage
{   //--------------------------------------------------- Attributs de la classe
	
 	//---------------------------------------------------- M�thodes de la classe
  public:
	//##ModelId=38A4889C00EE
	TImage24b();						// Constructeur par d�faut
	//##ModelId=38A4889C00EF
	TImage24b(int w, int h);			// 2nd Constructeur 
	//##ModelId=38A4889C00ED
	virtual ~TImage24b();				// Destructeur	
	//##ModelId=38A4889C00EA
	virtual void InitImage(int w, int h);	// Alloue l'espace n�cessaire aux donn�es de l'image    
	//##ModelId=38A4889C00E8
	virtual bool Copy(TImage16b* image);	// Copie cette image dans une surface directX
};
////////////////////////////// FIN de TIMAGE24B ////////////////////////////////


// TIMAGELOADER ////////////////////////////////////////////////////////////////
//##ModelId=38A488750118
class TImageLoader
{	//--------------------------------------------------- Attributs de la classe
	//##ModelId=38A488750119
	int iMaxHeaderSize;		// Taille maximum de l'en-t�te � pr�charger pour d�terminier le format de l'image

 	//---------------------------------------------------- M�thodes de la classe
  public:
	//##ModelId=38A488750121
	TImageLoader();				// Constructeur par d�faut
	//##ModelId=38A488750120
    virtual ~TImageLoader();	// Destructeur

	//##ModelId=38A48875011E
	virtual int GetRecognizeHeader(unsigned char* data);	// Renvoie le type de l'image si il l'a reconnu
	//##ModelId=38A48875011C
	virtual TStdImage *LoadAnyImage(TFileVirtual *fp);		// Cr�e et charge une image et renvoie son ptr
	//##ModelId=38A48875011A
	static TStdImage *LoadImageBmp(TFileVirtual *fp);		// Charge une image Bmp en m�moire
};
//////////////////////////// FIN de TIMAGELOADER ///////////////////////////////

//----------------------------------------- Instance unique de la classe externe

//----------------------------------------------------------- Fonctions externes




#endif	// ImageFileH
