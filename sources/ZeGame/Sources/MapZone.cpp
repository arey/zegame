////////////////////////////////////////////////////////////////////////////////
//  Unit� "MapZone.cpp" : gestion des diverses zones pouvant �tre trac�es sur //
//	 				      la carte d'un niveau								  //
//                                                                            //
//	Description :  Classe m�re de tous les objets du jeu, que ce soit les     //
//				   personnages, les d�cors ou des objets.					  //
//                                                                            //
//  Copyright (C) 2000 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com	                  //
//  Version      : 1.0                                                        //
//  Creation     : 16/01/2000 		                                          //
//  Last upgrade : 31/01/2000                                                 //
////////////////////////////////////////////////////////////////////////////////

#include "ZeNain.h"         //--- Special for Visual C++ 6 MFC ---
#pragma hdrstop
#include "FileVirtual.h"	// Fichiers virtuels (dans un .pak, en m�moire ou tout simplement sur le disk)
#include "MapZone.h"		// Header de la classe


////////////////////////////////////////////////////////////////////////////////
// Classe MAPZONE	                                                          //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
MapZone::MapZone():
	iType(MAPZONE_TYPE_NONE),	// Type de zone non d�finie
	pNumJointObject(NULL),
	iNbJointObject(0)
{	memset(szMapName, 0, MAX_CHAR_ZONE);
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
MapZone::~MapZone()
{	delete [] pNumJointObject; // [BUG] !!!!!!!!!!!
}
//----------------------------------------------------------------------------//

//=== Sauve les param�tres d'en-t�te =========================================//
bool MapZone::SaveHeader(ofstream& file)
{	file.write((char*) &iType, sizeof(iType));
	file.write((char*) &iMapZoneID, sizeof(iMapZoneID));
	file.write((char*) szMapName, MAX_CHAR_ZONE*sizeof(char));
	file.write((char*) &iNbJointObject, sizeof(iNbJointObject));
	// Sauvegarde la liste des num�ros des objets associ�s � cette zone
	if (iNbJointObject>0)
	{	file.write((char*) pNumJointObject, iNbJointObject*sizeof(int));
	}
	return true;
}
bool MapZone::SaveHeader(FILE& file)
{	fwrite((char*) &iType, sizeof(iType), 1, &file);
	fwrite((char*) &iMapZoneID, sizeof(iMapZoneID), 1, &file);
	fwrite((char*) szMapName, MAX_CHAR_ZONE*sizeof(char), 1, &file);
	fwrite((char*) &iNbJointObject, sizeof(iNbJointObject), 1, &file);
	// Sauvegarde la liste des num�ros des objets associ�s � cette zone
	if (iNbJointObject>0)
	{	fwrite((char*) pNumJointObject, iNbJointObject*sizeof(int), 1, &file);
	}
	return true;
}

//----------------------------------------------------------------------------//

//=== Charge les param�tres d'en-t�te ========================================//
bool MapZone::LoadHeader(ifstream& file)
{	// On ne lit pas son type, car il est lu juste avant d'instancier l'objet
	file.read((char*) &iMapZoneID, sizeof(iMapZoneID));
	file.read((char*) szMapName, MAX_CHAR_ZONE*sizeof(char));
	file.read((char*) &iNbJointObject, sizeof(iNbJointObject));
	// Lit la liste des num�ros des objets associ�s � cette zone
	if (iNbJointObject>0)
	{	pNumJointObject = new int [iNbJointObject];
		file.read((char*) pNumJointObject, iNbJointObject*sizeof(int));
	}
	return true;
}
bool MapZone::LoadHeader(TFileVirtual& file)
{	// On ne lit pas son type, car il est lu juste avant d'instancier l'objet
	file.read((char*) &iMapZoneID, sizeof(iMapZoneID));
	file.read((char*) szMapName, MAX_CHAR_ZONE*sizeof(char));
	file.read((char*) &iNbJointObject, sizeof(iNbJointObject));
	// Lit la liste des num�ros des objets associ�s � cette zone
	if (iNbJointObject>0)
	{	pNumJointObject = new int [iNbJointObject];
		file.read((char*) pNumJointObject, iNbJointObject*sizeof(int));
	}
	return true;
}
bool MapZone::LoadHeader(FILE& file)
{	// On ne lit pas son type, car il est lu juste avant d'instancier l'objet
	fread((char*) &iMapZoneID, sizeof(iMapZoneID), 1, &file);
	fread((char*) szMapName, MAX_CHAR_ZONE*sizeof(char), 1, &file);
	fread((char*) &iNbJointObject, sizeof(iNbJointObject), 1, &file);
	// Lit la liste des num�ros des objets associ�s � cette zone
	if (iNbJointObject>0)
	{	pNumJointObject = new int [iNbJointObject];
		fread((char*) pNumJointObject, iNbJointObject*sizeof(int), 1, &file);
	}
	return true;
}
//----------------------------------------------------------------------------//

//=== Attribue un nom � la Zone ==============================================//
bool MapZone::SetName(char* name)
{   if (strlen(name) >= MAX_CHAR_ZONE) return false;
	strcpy(szMapName, name);
    return true;
}
//----------------------------------------------------------------------------//

// Ajoute un objet au tableau
bool MapZone::AddObject2Zone(int objID)
{	// V�rifie si l'objet n'existe d�j� pas
	for (int i=0 ; i < iNbJointObject ; i++)
    { if (pNumJointObject[i] == objID) return false;
    }
    // Sinon, on peut l'ajouter �a la suite
    int* newTab = new int[iNbJointObject+1];
    memcpy(newTab, pNumJointObject, iNbJointObject);
	newTab[iNbJointObject] = objID;
    delete [] pNumJointObject;
    pNumJointObject = newTab;
   	iNbJointObject++;
    return true;
}
//----------------------------------------------------------------------------//

//=== Supprime cet objet du tableau ==========================================//
// Rmq : pas tr�s propre car on ne d�salloue pas toute la m�moire lib�r�e
//		 mais c'est plus efficace comme cel�
bool MapZone::DelObject2Zone(int objID)
{	// V�rifie si l'objet existe bien
	int index = -1;
	for (int i=0 ; i < iNbJointObject ; i++)
    {	if (pNumJointObject[i] == objID) index = i;
    }
    if (index == -1) return false;	// c'est qu'on l'a pas trouv�
    // Reste plus qu'� le supprimer
	if (iNbJointObject == 1)
    {	// a) Cas particulier du tableau � 1 seule zone
    	delete [] pNumJointObject;
    	pNumJointObject = NULL;
    } else
    {	// b) on recopie le derbier � l'endroit de celui que l'on veut supprimer
    	pNumJointObject[index] = pNumJointObject[iNbJointObject-1];
    }
    iNbJointObject--;
    return true;
}
//----------------------------------------------------------------------------//

/////////////////////////////// Fin de MAPZONE /////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe MAPZONERECT                                                         //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
MapZoneRect::MapZoneRect()
{	iType = MAPZONE_TYPE_RECT;
	dWidth = 0; dHeight = 0;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
MapZoneRect::~MapZoneRect()
{	//Supprime la liste des objets associ�s � cette zone
	// delete [] pNumJointObject; => parent
}
//----------------------------------------------------------------------------//

//=== Attribue les coordon�es de la zone rectangulaire =======================//
void MapZoneRect::SetZone(double x, double y, double w, double h)
{	dDepX = x;	dDepY = y;
	dWidth = w;	dHeight = h;
}
//----------------------------------------------------------------------------//

//=== Renvoi les coordonn�es du pt (DepX, DepY) ==============================//
void MapZoneRect::GetOrigine(double &x, double &y)
{	x = dDepX;	y = dDepY; 	
}
//----------------------------------------------------------------------------//

//=== Vrai si le point est dans la zone ======================================//
bool MapZoneRect::isPointInZone(double x, double y)
{	 return    ((dWidth>0) ? ((x >= dDepX) && (x <= dDepX + dWidth)) : ((x >= dDepX + dWidth) && (x <= dDepX)))
		    && ((dHeight>0)? ((y >= dDepY) && (y <= dDepY + dHeight)): ((y >= dDepY + dHeight) && (y <= dDepY)));
	// Bug detecte le 30/09/2000 => ne tient pas compte des dWidth et dHeight negatifs
	// return ((x >= dDepX) && (x <= dDepX + dWidth) && (y >= dDepY) && (y <= dDepY + dHeight));
}
//----------------------------------------------------------------------------//

//=== Sauvegarde ces informations dans un fichier ============================//
bool MapZoneRect::SaveIntoFile(ofstream& file)
{	MapZone::SaveHeader(file);
	file.write((char*) &dDepX, sizeof(dDepX));
	file.write((char*) &dDepY, sizeof(dDepY));
	file.write((char*) &dWidth, sizeof(dWidth));
	file.write((char*) &dHeight, sizeof(dHeight));
	return true;
}
bool MapZoneRect::SaveIntoFile(FILE& file)
{	MapZone::SaveHeader(file);
	fwrite(&dDepX, sizeof(dDepX), 1, &file);
	fwrite(&dDepY, sizeof(dDepY), 1, &file);
	fwrite(&dWidth, sizeof(dWidth), 1, &file);
	fwrite(&dHeight, sizeof(dHeight), 1, &file);
	return true;
}
//----------------------------------------------------------------------------//

//=== Charge ses informations � partir d'un fichier ==========================//
bool MapZoneRect::LoadFromFile(ifstream& file)
{   MapZone::LoadHeader(file);
	file.read((char*) &dDepX, sizeof(dDepX));
	file.read((char*) &dDepY, sizeof(dDepY));
	file.read((char*) &dWidth, sizeof(dWidth));
	file.read((char*) &dHeight, sizeof(dHeight));
	return true;
}
bool MapZoneRect::LoadFromFile(TFileVirtual& file)
{	MapZone::LoadHeader(file);
	file.read((char*) &dDepX, sizeof(dDepX));
	file.read((char*) &dDepY, sizeof(dDepY));
	file.read((char*) &dWidth, sizeof(dWidth));
	file.read((char*) &dHeight, sizeof(dHeight));
	return true;
}
bool MapZoneRect::LoadFromFile(FILE& file)
{	MapZone::LoadHeader(file);
	fread((char*) &dDepX, sizeof(dDepX), 1, &file);
	fread((char*) &dDepY, sizeof(dDepY), 1, &file);
	fread((char*) &dWidth, sizeof(dWidth), 1, &file);
	fread((char*) &dHeight, sizeof(dHeight), 1, &file);
	return true;
}
//----------------------------------------------------------------------------//

///////////////////////////// Fin de MAPZONERECT ///////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Classe MAPZONECIRCLE                                                       //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
MapZoneCircle::MapZoneCircle()
{	iType = MAPZONE_TYPE_CIRCLE;
	dRadius = -1;		// PErmet de savoir si le cercle a �t� initialis� ou non
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
MapZoneCircle::~MapZoneCircle()
{
}
//----------------------------------------------------------------------------//

//=== Attribue les coordon�es de la zone rectangulaire =======================//
void MapZoneCircle::SetZone(double x, double y, double r)
{	dCenterX = x;	dCenterY = y;
	dRadius = r;
	dSqrRadius = r*r;
}
//----------------------------------------------------------------------------//

//=== Attribue le centre du cercle ===========================================//
void MapZoneCircle::SetCenter(double &x, double &y)
{	dCenterX = x;	dCenterY = y;	
}
//----------------------------------------------------------------------------//

//=== Attribuele  rayon du cercle ============================================//
void MapZoneCircle::SetRadius(double &r)
{	dRadius = r;	dSqrRadius = r*r;
}
//----------------------------------------------------------------------------//

//=== Renvoi les coordonn�es du centre du cercle =============================//
void MapZoneCircle::GetCenter(double &x, double &y)
{	x = dCenterX; y = dCenterY;
}
//----------------------------------------------------------------------------//

//=== Vrai si le point est dans la zone
bool MapZoneCircle::isPointInZone(double x, double y)
{	return ((x - dCenterX)*(x - dCenterX) + (y - dCenterY)*(y - dCenterY)) < dSqrRadius;
}
//----------------------------------------------------------------------------//

//=== Sauvegarde ces informations dans un fichier ============================//
bool MapZoneCircle::SaveIntoFile(ofstream& file)
{	MapZone::SaveHeader(file);
	file.write((char*) &dCenterX, sizeof(dCenterX));
	file.write((char*) &dCenterY, sizeof(dCenterY));
	file.write((char*) &dRadius, sizeof(dRadius));
	return true;
}
bool MapZoneCircle::SaveIntoFile(FILE& file)
{	MapZone::SaveHeader(file);
	fwrite(&dCenterX, sizeof(dCenterX), 1, &file);
	fwrite(&dCenterY, sizeof(dCenterY), 1, &file);
	fwrite(&dRadius, sizeof(dRadius), 1, &file);
	return true;
}
//----------------------------------------------------------------------------//

//=== Charge ses informations � partir d'un fichier ==========================//
bool MapZoneCircle::LoadFromFile(ifstream& file)
{	MapZone::LoadHeader(file);
	file.read((char*) &dCenterX, sizeof(dCenterX));
	file.read((char*) &dCenterY, sizeof(dCenterY));
	file.read((char*) &dRadius, sizeof(dRadius));
	dSqrRadius = dRadius*dRadius;
	return true;
}
bool MapZoneCircle::LoadFromFile(TFileVirtual& file)
{	MapZone::LoadHeader(file);
	file.read((char*) &dCenterX, sizeof(dCenterX));
	file.read((char*) &dCenterY, sizeof(dCenterY));
	file.read((char*) &dRadius, sizeof(dRadius));
	dSqrRadius = dRadius*dRadius;
	return true;
}
bool MapZoneCircle::LoadFromFile(FILE& file)
{	MapZone::LoadHeader(file);
	fread((char*) &dCenterX, sizeof(dCenterX), 1, &file);
	fread((char*) &dCenterY, sizeof(dCenterY), 1, &file);
	fread((char*) &dRadius, sizeof(dRadius), 1, &file);
	dSqrRadius = dRadius*dRadius;
	return true;
}
//----------------------------------------------------------------------------//

//////////////////////////// Fin de MAPZONECIRCLE //////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Classe MAPZONELIST                                                         //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur ===========================================================//
MapZoneList::MapZoneList()
{
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
MapZoneList::~MapZoneList()
{	// Supprime toutes les Zones de la liste et vide le dico
	for (iter = mMapZones.begin(); iter != mMapZones.end(); iter++)
    {	delete (*iter).second;
    }
	mMapZones.clear();
}
//----------------------------------------------------------------------------//

//=== Rajoute une zone � la liste ============================================//
void MapZoneList::AddZone(MapZone* zone)
{	mMapZones.insert(std::make_pair(zone->GetID(), zone));
}
//----------------------------------------------------------------------------//

//=== Charge toutes les zones contenues dans un fichier ======================//
bool MapZoneList::LoadZonesFromFile(ifstream& file)
{	int nbZones = 0;
	int type = 0;
	MapZone* zone = NULL;

	// 1) R�cup�re le Nb de zones � charger
	file.read((char*) &nbZones, sizeof(nbZones));
	// 2) Puis, pour chaque zone 
	for (int i=0 ; i < nbZones ; i++)
	{	// - on r�cup�re son type
		file.read((char*) &type, sizeof(type));
		// - on alloue l'instance corespondante
		switch (type)
		{	case (MAPZONE_TYPE_RECT):
				zone = new MapZoneRect();
				break;
			case (MAPZONE_TYPE_CIRCLE):
				zone = new MapZoneCircle();
				break;
			default: return false;
		}
		// - et l'on charge les donn�es
		if (!zone->LoadFromFile(file)) return false;
		// - et enfin, on l'ajoute � la liste
		AddZone(zone);
	}
	return true;
}
bool MapZoneList::LoadZonesFromFile(TFileVirtual& file)
{	int nbZones = 0;
	int type = 0;
	MapZone* zone = NULL;

	// 1) R�cup�re le Nb de zones � charger
	file.read((char*) &nbZones, sizeof(nbZones));
	// 2) Puis, pour chaque zone 
	for (int i=0 ; i < nbZones ; i++)
	{	// - on r�cup�re son type
		file.read((char*) &type, sizeof(type));
		// - on alloue l'instance corespondante
		switch (type)
		{	case (MAPZONE_TYPE_RECT):
				zone = new MapZoneRect();
				break;
			case (MAPZONE_TYPE_CIRCLE):
				zone = new MapZoneCircle();
				break;
			default: return false;
		}
		// - et l'on charge les donn�es
		if (!zone->LoadFromFile(file)) return false;
		// - et enfin, on l'ajoute � la liste
		AddZone(zone);

	}
	return true;
}
bool MapZoneList::LoadZonesFromFile(FILE& file)
{	int nbZones = 0;
	int type = 0;
	MapZone* zone = NULL;

	// 1) R�cup�re le Nb de zones � charger
	fread((char*) &nbZones, sizeof(nbZones), 1, &file);
	// 2) Puis, pour chaque zone 
	for (int i=0 ; i < nbZones ; i++)
	{	// - on r�cup�re son type
		fread((char*) &type, sizeof(type), 1, &file);
		// - on alloue l'instance corespondante
		switch (type)
		{	case (MAPZONE_TYPE_RECT):
				zone = new MapZoneRect();
				break;
			case (MAPZONE_TYPE_CIRCLE):
				zone = new MapZoneCircle();
				break;
			default: return false;
		}
		// - et l'on charge les donn�es
		if (!zone->LoadFromFile(file)) return false;
		// - et enfin, on l'ajoute � la liste
		AddZone(zone);
	}
	return true;
}
//----------------------------------------------------------------------------//

//=== Sauve la liste des zones dans un fichier ===============================//
bool MapZoneList::SaveZonesToFile(ofstream& file)
{	// 1) Sauvegarde le Nb de zones � sauver
	int nbZones = mMapZones.size();
	file.write((char*) &nbZones, sizeof(nbZones));
	// 2) Puis on sauve chaque zone pr�c�d� par leur type
	for (iter = mMapZones.begin(); iter != mMapZones.end(); iter++)
    {	(*iter).second->SaveIntoFile(file);
	}	
	return true;
}
bool MapZoneList::SaveZonesToFile(FILE& file)
{	// 1) Sauvegarde le Nb de zones � sauver
	int nbZones = mMapZones.size();
	fwrite(&nbZones, sizeof(nbZones), 1, &file);
	// 2) Puis on sauve chaque zone pr�c�d� par leur type
	for (iter = mMapZones.begin(); iter != mMapZones.end(); iter++)
    {	(*iter).second->SaveIntoFile(file);
	}
	return true;
}
//----------------------------------------------------------------------------//

//=== Renvoi la 1�re zone contenue ds la liste ===============================//
MapZone* MapZoneList::GetFirstZone()
{	iter = mMapZones.begin();
    if (iter == mMapZones.end()) NULL;
	return (*iter).second;
}
//----------------------------------------------------------------------------//

//=== Renvoi la zone suivant celle ayant ce n�ID =============================//
MapZone* MapZoneList::GetNextZone(int curID)
{	for (iter = mMapZones.begin(); iter != mMapZones.end(); iter++)
	{	if ((*iter).second->GetID() == curID)
    	{	iter++;
        	if (iter == mMapZones.end()) return NULL;
            return (*iter).second;
        }
    }
    return NULL;
}
//----------------------------------------------------------------------------//

//=== Renvoi la zone pr�c�dent celle ayant ce n�ID ===========================//
MapZone* MapZoneList::GetPrevZone(int curID)
{	for (iter = mMapZones.begin(); iter != mMapZones.end(); iter++)
	{	if ((*iter).second->GetID() == curID)
    	{   if (iter == mMapZones.begin()) return NULL;
        	iter--;
            return (*iter).second;
        }
    }
    return NULL;
}
//----------------------------------------------------------------------------//

//=== Renvoi la 1�re zone �tant sur le point (x,y) ===========================//
MapZone* MapZoneList::GetFirstZoneOn(double &x, double &y)
{	for (iter = mMapZones.begin(); iter != mMapZones.end(); iter++)
	{	if ((*iter).second->isPointInZone(x, y))
    	{	return (*iter).second;
        }
    }
	return NULL;
}
//----------------------------------------------------------------------------//

//=== Renvoi la zone suivante � celle n�curID se trouvant en (x,y) ===========//
MapZone* MapZoneList::GetNextZoneOn(double &x, double &y, int curID)
{   // Parcours du d�but de la liste jusqu'au curID
	for (iter = mMapZones.begin(); (iter != mMapZones.end()) && ((*iter).second->GetID() == curID); iter++);
	// On saute cette zone
    iter++;
	// Puis de l� o� l'on en est jusqu'� ce qu'on trouve une zone se trouvant sur ce point
	for (; iter != mMapZones.end(); iter++)
	{	if ((*iter).second->isPointInZone(x, y))
    	{	return (*iter).second;
        }
    }
	return NULL;
}
//----------------------------------------------------------------------------//

//=== Supprime cette zone de la liste ========================================//
void MapZoneList::DelZone(int zoneID)
{	// Efface la zone
	delete mMapZones[zoneID];
    mMapZones[zoneID] = NULL;
	// Puis supprime la zone de la liste
    mMapZones.erase(zoneID);
}
//----------------------------------------------------------------------------//

/////////////////////////////// Fin de MAPZONELIST /////////////////////////////


