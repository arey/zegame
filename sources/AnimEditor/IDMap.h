////////////////////////////////////////////////////////////////////////////////
// IDMap.h : permet de gérer la gestion de tous les IDENTIFICATEURS du jeu,   //
//			   utilisés dans les divers éditeurs.							  //
////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#ifndef IDMapH
#define IDMapH
//---------------------------------------------------------------------------
#include <map>              // librairie STL de gestion des bibliothèques
//#include <string>			// librairie STL de gestion des chaînes de caractères

//using namespace std;		// utilisé pour valider l'utilisation de la STL
namespace std {

class IDMap
{
  public:
    // Lit un fichier *.id et place son contenu dans le dictionnaire des ID
  	void LireFileID(AnsiString  IDfichier);
    // Sauve le contenu du dictionnaire dans un fichier *.id
  	void SauveFileID(AnsiString  IDfichier);
    // ajoute une paire <string,int> au dictionnaire si la clef n'existe pas
    void AjoutID(AnsiString cle, int attribut);
    // ajoute une paire <string,int> au dictionnaire en écrasant la valeur déjà existante
    void EcraseID(AnsiString cle, int attribut);
    // ajout d'une paire à partir d'une clef, en trouvant l'attribut
    void AjoutClef(AnsiString cle);
    // supprime la paire <string,int> du dictionnaire
    void SupprID(AnsiString cle);

    // Création d'une paire à partir d'un n° et d'une cle de base (2)
    bool CreateBasedNumID(AnsiString cleDefaut, int attribut);

    // Création d'une paire à partir eulement d'un nom par défaut (3)
    void CreateBasedID(AnsiString cleDefaut);

    // Modifie le nom d'un ID par un autre en conservant son numéro
    // => renvoi faux si le nom de l'ID à remplacer n'existe pas
    //	           ou si le nouveau nom est déjà utilisé	 
    bool ChangeClef(AnsiString src, AnsiString dest);

    // vérifie si la cle "string" existe dans le dictionnaire
    bool IsCle(AnsiString cle);
    // vérifie si l'élément "int" existe dans le dictionnaire
    bool IsElement(int element);
    // vérifie que le couple existe
    bool IsCouple(AnsiString &cle, int element);
    // Renvoie le plus grand attribut du dictinnaire
    int MaxAttribut();
    // Renvoie le n° correspondant à l'ID
    int NumID(AnsiString &cle);
    // Renvoie la string-cle correspondant au n°
    AnsiString CleID(int element);

  private:
	typedef std::map<AnsiString ,int, less<AnsiString> > DicoID;	// alias DicoID
	DicoID MapID;										    // Dictionnaire
    typedef DicoID::iterator Iterat;						// itérateur sur le dico
};

} // fin du namespace std
#endif
