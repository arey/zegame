////////////////////////////////////////////////////////////////////////////////
//  Unité "MapZone.h" : gestion des diverses zones pouvant être tracées sur	  //
//					    la carte d'un niveau								  //
//                                                                            //
//	Description :  Classe mère de tous les objets du jeu, que ce soit les     //
//				   personnages, les décors ou des objets.					  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : 1.0                                                        //
//  Creation     : 16/01/2000 		                                          //
//  Last upgrade : 31/01/2000                                                 //
////////////////////////////////////////////////////////////////////////////////

#ifndef MapZoneH
#define MapZoneH

//---------------------------------------------------------------------- Include
#include <map>			// Librairie STL de gestion de dictionnaires à clé unique
#include <fstream.h>    // Gestionnaire de fichiers du C++

//----------------------------------------------------------------- Define & enum
#define MAX_CHAR_ZONE	64

enum 
{	MAPZONE_TYPE_NONE = 0,
	MAPZONE_TYPE_RECT,
	MAPZONE_TYPE_CIRCLE
};

class TFileVirtual;

////////////////////////////////////////////////////////////////////////////////
// Classe MAPZONE
// classe abstraite dont dériveront les différents types de zone que l'on 
// pourra avoir dans le jeu
class MapZone
{ // ---------------------------------------------------------- Attributs privés
  protected:
	int iType;						// Type de zone
	int iMapZoneID;					// Identificateur de la zone
	char szMapName[MAX_CHAR_ZONE];	// Nom de la zone
	
	int iNbJointObject;				// Nb d'objets associés à cette zone
	int* pNumJointObject;			// Numéros de ces objets
	
  // -------------------------------------------------------- Méthodes publiques
  public:
	MapZone();								// Constructeur par défaut
	virtual ~MapZone();						// Destructeur
	bool SaveHeader(ofstream& file);		// Sauve les paramètres d'en-tête
	bool SaveHeader(FILE& file);			//	 "	  "		"			"
	bool LoadHeader(ifstream& file);		// Charge les paramètres d'en-tête
	bool LoadHeader(TFileVirtual& file);	//	 "	   "	 "			 "
	bool LoadHeader(FILE& file);			//	 "	   "	 "			 "
    bool SetName(char* name);				// Attribue un nom à la Zone
    // Fonctions relatives au tableau des Objets Associés à la zone
    bool AddObject2Zone(int objID);			// Ajoute un objet au tableau
    bool DelObject2Zone(int objID);			// Supprime cet objet du tableau

	//--- Fonctions virtuelles pures ---
	virtual bool isPointInZone(double x, double y) = 0; 	// Vrai si le point est dans la zone
	virtual	bool SaveIntoFile(ofstream& file) = 0;			// Sauvegarde ces informations dans un fichier
	virtual bool SaveIntoFile(FILE& file) = 0;				//	 "		  "			"	   "	  "		"
	virtual bool LoadFromFile(ifstream& file) = 0;			// Charge ses informations à partir d'un fichier
	virtual bool LoadFromFile(TFileVirtual& file) = 0;		//	 "		  "			"	   "	  "		"
	virtual bool LoadFromFile(FILE& file) = 0;				//	 "		  "			"	   "	  "		"
    virtual double GetOrigineX() = 0;
    virtual double GetOrigineY() = 0 ;
    virtual void SetOrigineX(double oX) = 0;
    virtual void SetOrigineY(double oY) = 0 ;
    virtual void MoveX(double dx) = 0;
    virtual void MoveY(double dy) = 0;

	//--- Fonctions inline ---
	inline GetType() { return iType; }			// Renvoi le type de zone
	inline SetID(int id) { iMapZoneID = id; }	// Attribue un n°ID à la zone
	inline GetID() { return iMapZoneID; }		// Récupère le n°ID de la zone
	inline int GetNbJointObject() { return iNbJointObject; }
	inline int GetObjectID(int ind) { return pNumJointObject[ind]; }
    inline char* GetName() { return szMapName; }
    inline int* GetJointObjectTab() { return pNumJointObject; }
};
//======================= Fin de la classe MAPZONE ============================//


////////////////////////////////////////////////////////////////////////////////
// Classe MAPZONERECT
// Zone rectangulaire dans le repère du jeu  (losnage à l'écran)
class MapZoneRect : public MapZone
{ // ---------------------------------------------------------- Attributs privés
  protected:
	double	dDepX, dDepY;			// Pt en haut à gauche du rectangle (dans les coordonnées du monde)
	double	dWidth, dHeight;		// Dimensions du rectangle

  // -------------------------------------------------------- Méthodes publiques
  public:
	MapZoneRect();							// Constructeur par défaut
	virtual ~MapZoneRect();					// Destructeur

	void SetZone(double x, double y, double w, double h);	// Attribue les coordonées de la zone rectangulaire
	void GetOrigine(double &x, double &y);					// Renvoi les coordonnées du pt (DepX, DepY)    

	//--- Fonctions virtuelles ---
	bool isPointInZone(double x, double y);	// Vrai si le point est dans la zone
	bool SaveIntoFile(ofstream& file);		// Sauvegarde ces informations dans un fichier
	bool SaveIntoFile(FILE& file);			//	 "		  "			"	   "	  "		"
	bool LoadFromFile(ifstream& file);		// Charge ses informations à partir d'un fichier
	bool LoadFromFile(TFileVirtual& file);	//	 "		  "			"	   "	  "		"
	bool LoadFromFile(FILE& file);			//	 "		  "			"	   "	  "		"

    //--- Fonctions inline ---
    inline double GetWidth() { return dWidth; }
    inline double GetHeight() { return dHeight; }
    inline double GetOrigineX() { return dDepX; }
    inline double GetOrigineY() { return dDepY; }
    inline void SetOrigineX(double oX) { dDepX = oX ;}
    inline void SetOrigineY(double oY) { dDepY = oY ;}
    inline void MoveX(double dx) { dDepX += dx; }
    inline void MoveY(double dy) { dDepY += dy; }
};
//===================== Fin de la classe MAPZONERECT ==========================//

////////////////////////////////////////////////////////////////////////////////
// Classe MAPZONECIRCLE
class MapZoneCircle : public MapZone
{ // ---------------------------------------------------------- Attributs privés
  protected:
	double	dCenterX, dCenterY;		// Coordonnées du centre du cercle
	double	dRadius;				// Rayon de ce cercle
	double  dSqrRadius;				// Rayon au carré
	
  // -------------------------------------------------------- Méthodes publiques
  public:
	MapZoneCircle();						// Constructeur par défaut
	virtual ~MapZoneCircle();				// Destructeur

	void SetZone(double x, double y, double r);		// Attribue les coordonées de cette zone circulaire
    void SetCenter(double &x, double &y);			// Attribue le centre du cercle
    void SetRadius(double &r);						// ainsi que son rayon
    void GetCenter(double &x, double &y);			// Renvoi les coordonnées du centre du cercle

	//--- Fonctions virtuelles ---
	bool isPointInZone(double x, double y);	// Vrai si le point est dans la zone
	bool SaveIntoFile(ofstream& file);		// Sauvegarde ces informations dans un fichier
	bool SaveIntoFile(FILE& file);			//	 "		  "			"	   "	  "		"
	bool LoadFromFile(ifstream& file);		// Charge ses informations à partir d'un fichier
	bool LoadFromFile(TFileVirtual& file);	//	 "		  "			"	   "	  "		"
	bool LoadFromFile(FILE& file);			//	 "		  "			"	   "	  "		"

    //--- Fonctions inline ---
    inline double GetRadius() { return dRadius; }
    inline double GetOrigineX() { return dCenterX; }
    inline double GetOrigineY() { return dCenterY; }
    inline void SetOrigineX(double oX) { dCenterX = oX ;}
    inline void SetOrigineY(double oY) { dCenterY = oY ;}
    inline void MoveX(double dx) { dCenterX += dx; }
    inline void MoveY(double dy) { dCenterY += dy; }
};
//==================== Fin de la classe MAPZONECIRCLE =========================//

typedef std::map<int, MapZone*> mapMapZones;
typedef std::map<int, MapZone*>::iterator iterMapZones;

////////////////////////////////////////////////////////////////////////////////
// Classe MAPZONELIST
// Liste de toutes les zones d'un niveau
class MapZoneList
{ // ---------------------------------------------------------- Attributs privés
  protected:
	std::map<int, MapZone*>	mMapZones;		// Dictionnaire contenant toutes les zones du niveau
	std::map<int, MapZone*>::iterator iter;	// Itérateur parourant le dico
	
  // -------------------------------------------------------- Méthodes publiques
  public:
	MapZoneList();						// Constructeur par défaut
	~MapZoneList();						// Destructeur

	void AddZone(MapZone* zone);		// Rajoute une zone à la liste
    void DelZone(int zoneID);			// Supprime cette zone de la liste
    MapZone* GetFirstZone();			// Renvoi la 1ère zone contenue ds la liste
    MapZone* GetNextZone(int curID); 	// Renvoi la zone suivant celle ayant ce n°ID
    MapZone* GetPrevZone(int curID);	// Renvoi la zone précédent celle ayant ce n°ID
    MapZone* GetFirstZoneOn(double &x, double &y);				// Renvoi la 1ère zone étant sur le point (x,y)
    MapZone* GetNextZoneOn(double &x, double &y, int curID);	// Renvoi la zone suivante à celle n°curID se trouvant en (x,y)

	//--- Fonctions de lecture/écriture dans divers types de fichiers ---
	bool SaveZonesToFile(ofstream& file);		// Sauve la liste des zones dans un fichier
	bool SaveZonesToFile(FILE& file);			//	 "		  "			"	  "	  "		"
	bool LoadZonesFromFile(ifstream& file);		// Charge toutes les zones contenues dans un fichier
	bool LoadZonesFromFile(TFileVirtual& file);	//	 "		  "			"	  "	  "		"
	bool LoadZonesFromFile(FILE& file);			//	 "		  "			"	  "	  "		"

	//--- Fonctions inline ---
	inline MapZone*	GetZone(int id) { return mMapZones[id]; }	// Renvoi un pointeur ves la zone ayant ce n°
	inline bool IsZone(int id) { return mMapZones.find(id) != mMapZones.end(); }	// Vrai si la zone appartient à la liste
	inline mapMapZones& GetMapZonesMap() { return mMapZones; }
	inline int GetNbZones() { return mMapZones.size(); }
};
//===================== Fin de la classe MAPZONELIST =========================//


//------------------------------------------------------------- Fonctions annexes
#endif


