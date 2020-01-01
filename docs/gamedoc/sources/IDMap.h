////////////////////////////////////////////////////////////////////////////////
// IDMap.h : permet de gérer la gestion de tous les IDENTIFICATEURS du jeu,   //
//	  		 utilisés dans les divers éditeurs.								  //
//                                                                            //
// Version : 1.2															  //			
//  - 22/07/99 adaptation de la librairie pour Visual C++					  //
//	- 21/12/99 encodage et lecture à partir des fichiers virtuels			  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
#ifndef IDMapH
#define IDMapH

//---------------------------------------------------------------------- include
#ifndef MICROSOFT
#include <vcl.h>  			// pour l'utilisation des AnsiString
#endif
#include <map>              // librairie STL de gestion des bibliothèques

//using namespace std;		// utilisé pour valider l'utilisation de la STL
namespace std {

class IDMap
{
  public:
    // Lit un fichier *.id et place son contenu dans le dictionnaire des ID
    void LireFileID(CString  IDfichier);
    // Sauve le contenu du dictionnaire dans un fichier *.id
    void SauveFileID(CString  IDfichier);
    // ajoute une paire <string,int> au dictionnaire si la clef n'existe pas
    void AjoutID(CString cle, int attribut);
    // ajoute une paire <string,int> au dictionnaire en écrasant la valeur déjà existante
    void EcraseID(CString cle, int attribut);
    // ajout d'une paire à partir d'une clef, en trouvant l'attribut
    void AjoutClef(CString cle);
    // supprime la paire <string,int> du dictionnaire
    void SupprID(CString cle);

    // Création d'une paire à partir d'un n° et d'une cle de base (2)
    bool CreateBasedNumID(CString cleDefaut, int attribut);

    // Création d'une paire à partir eulement d'un nom par défaut (3)
    void CreateBasedID(CString cleDefaut);

    // Modifie le nom d'un ID par un autre en conservant son numéro
    // => renvoi faux si le nom de l'ID à remplacer n'existe pas
    //	           ou si le nouveau nom est déjà utilisé
    bool ChangeClef(CString src, CString dest);

    // vérifie si la cle "string" existe dans le dictionnaire
    bool IsCle(CString cle);
    // vérifie si l'élément "int" existe dans le dictionnaire
    bool IsElement(int element);
    // vérifie que le couple existe
    bool IsCouple(CString &cle, int element);
    // Renvoie le plus grand attribut du dictionnaire
    int MaxAttribut();
    // Renvoie le n° correspondant à l'ID
    int NumID(CString &cle);
    // Renvoie la string-cle correspondant au n°
    CString CleID(int element);
	
	IDMap();			// Constructeur par défaut
	virtual ~IDMap();	// Destructeur

  public:
    typedef std::map<CString ,int, less<CString> > DicoID;	// alias DicoID
    DicoID MapID;										    // Dictionnaire
    typedef DicoID::iterator Iterat;						// itérateur sur le dico
	Iterat MapIDIter;
};

} // fin du namespace std
#endif
