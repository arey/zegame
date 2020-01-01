////////////////////////////////////////////////////////////////////////////////
//  Unité "ImageFile.h"														  //	                                                                            //
//	Description : Classes d'images de différents formats				      //
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
#include <fstream.h>    // Gestionnaire de fichiers de la STL

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
class TStdImage
{   //--------------------------------------------------- Attributs de la classe
  protected:	
	int iImageType;			// Format de l'image initial : bmp, tga, jpg ...
	void *lpData;			// Données binaires de l'image (8,16,24 ou 32 bits)
	void *lpPal;			// Palette de couleur (Images 8bits)
	int iWidth, iHeight;	// Longeur et Hauteur de l'image
	int iBitPerPix;			// Nb de bits par pixel
	int iBpW;				// Nb d'OCTETS par longueur d'image

	friend class TImageLoader;  // Déclaration d'une classe amie
		
 	//---------------------------------------------------- Méthodes de la classe
  public:
	TStdImage();			// Constructeur par défaut
    virtual ~TStdImage();	// Destructeur
	virtual void InitImage(int w, int h) = 0;	// Alloue l'espace nécessaire aux données de l'image
	
	// --- MaJ de paramètres de l'image ---
	inline void SetImageType(int it) { iImageType = it; }
	
	// --- Renvoit des paramètres de l'image ---
	virtual void* GetDataAddr();
	virtual void* GetPalAddr();
	inline int GetWidth() { return iWidth; }
	inline int GetHeight() { return iHeight; }
	inline int GetBitPerPix() { return iBitPerPix; }
	inline int GetPitch() { return iBpW; }	
};
/////////////////////////////// FIN de TSTDIMAGE ///////////////////////////////

// TIMAGE8B ////////////////////////////////////////////////////////////////////
class TImage8b : public TStdImage
{   //--------------------------------------------------- Attributs de la classe
	
 	//---------------------------------------------------- Méthodes de la classe
  public:
	TImage8b();						// Constructeur par défaut
	TImage8b(int w, int h);			// 2nd Constructeur
    virtual ~TImage8b();			// Destructeur
	virtual void InitImage(int w, int h);	// Alloue l'espace nécessaire aux données de l'image
	virtual bool Copy(TImage16b* image);	// Copie cette image dans une surface directX
};
/////////////////////////////// FIN de TIMAGE8B ////////////////////////////////

// TIMAGE16B ///////////////////////////////////////////////////////////////////
class TImage16b : public TStdImage
{   //--------------------------------------------------- Attributs de la classe
	
 	//---------------------------------------------------- Méthodes de la classe
  public:
	TImage16b();						// Constructeur par défaut
	TImage16b(int w, int h);			// 2nd Constructeur 
	TImage16b(TImage8b* image);			// 3ieme Constructeur : copie l'image se trouvant dans l'image 8bits
	virtual ~TImage16b();				// Destructeur	
	virtual void InitImage(int w, int h);	// Alloue l'espace nécessaire aux données de l'image    
};
////////////////////////////// FIN de TIMAGE16B ////////////////////////////////


// TIMAGELOADER ////////////////////////////////////////////////////////////////
class TImageLoader
{	//--------------------------------------------------- Attributs de la classe
	int iMaxHeaderSize;		// Taille maximum de l'en-tête à précharger pour déterminier le format de l'image

 	//---------------------------------------------------- Méthodes de la classe
  public:
	TImageLoader();				// Constructeur par défaut
    virtual ~TImageLoader();	// Destructeur

	virtual int GetRecognizeHeader(unsigned char* data);	// Renvoie le type de l'image si il l'a reconnu
	virtual TStdImage *LoadAnyImage(ifstream *fp);	// Crée et charge une image et renvoie son ptr
	static TStdImage *LoadImageBmp(ifstream *fp);	// Charge une image Bmp en mémoire
};
//////////////////////////// FIN de TIMAGELOADER ///////////////////////////////

//----------------------------------------- Instance unique de la classe externe

//----------------------------------------------------------- Fonctions externes




#endif	// ImageFileH
