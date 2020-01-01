////////////////////////////////////////////////////////////////////////////////
// IDMap.h : permet de g�rer la gestion de tous les IDENTIFICATEURS du jeu,   //
//	  		 utilis�s dans les divers �diteurs.								  //
//                                                                            //
// Version : 1.2															  //			
//  - 22/07/99 adaptation de la librairie pour Visual C++					  //
//	- 21/12/99 encodage et lecture � partir des fichiers virtuels			  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
#ifndef IDMapH
#define IDMapH

//---------------------------------------------------------------------- include
#ifndef MICROSOFT
#include <vcl.h>  			// pour l'utilisation des AnsiString
#endif
#include <map>              // librairie STL de gestion des biblioth�ques

//using namespace std;		// utilis� pour valider l'utilisation de la STL
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
    // ajoute une paire <string,int> au dictionnaire en �crasant la valeur d�j� existante
    void EcraseID(CString cle, int attribut);
    // ajout d'une paire � partir d'une clef, en trouvant l'attribut
    void AjoutClef(CString cle);
    // supprime la paire <string,int> du dictionnaire
    void SupprID(CString cle);

    // Cr�ation d'une paire � partir d'un n� et d'une cle de base (2)
    bool CreateBasedNumID(CString cleDefaut, int attribut);

    // Cr�ation d'une paire � partir eulement d'un nom par d�faut (3)
    void CreateBasedID(CString cleDefaut);

    // Modifie le nom d'un ID par un autre en conservant son num�ro
    // => renvoi faux si le nom de l'ID � remplacer n'existe pas
    //	           ou si le nouveau nom est d�j� utilis�
    bool ChangeClef(CString src, CString dest);

    // v�rifie si la cle "string" existe dans le dictionnaire
    bool IsCle(CString cle);
    // v�rifie si l'�l�ment "int" existe dans le dictionnaire
    bool IsElement(int element);
    // v�rifie que le couple existe
    bool IsCouple(CString &cle, int element);
    // Renvoie le plus grand attribut du dictionnaire
    int MaxAttribut();
    // Renvoie le n� correspondant � l'ID
    int NumID(CString &cle);
    // Renvoie la string-cle correspondant au n�
    CString CleID(int element);
	
	IDMap();			// Constructeur par d�faut
	virtual ~IDMap();	// Destructeur

  public:
    typedef std::map<CString ,int, less<CString> > DicoID;	// alias DicoID
    DicoID MapID;										    // Dictionnaire
    typedef DicoID::iterator Iterat;						// it�rateur sur le dico
	Iterat MapIDIter;
};

} // fin du namespace std
#endif
