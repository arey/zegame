////////////////////////////////////////////////////////////////////////////////
// IDMap.h : permet de g�rer la gestion de tous les IDENTIFICATEURS du jeu,   //
//			   utilis�s dans les divers �diteurs.							  //
////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#ifndef IDMapH
#define IDMapH
//---------------------------------------------------------------------------
#include <map>              // librairie STL de gestion des biblioth�ques
//#include <string>			// librairie STL de gestion des cha�nes de caract�res

//using namespace std;		// utilis� pour valider l'utilisation de la STL
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
    // ajoute une paire <string,int> au dictionnaire en �crasant la valeur d�j� existante
    void EcraseID(AnsiString cle, int attribut);
    // ajout d'une paire � partir d'une clef, en trouvant l'attribut
    void AjoutClef(AnsiString cle);
    // supprime la paire <string,int> du dictionnaire
    void SupprID(AnsiString cle);

    // Cr�ation d'une paire � partir d'un n� et d'une cle de base (2)
    bool CreateBasedNumID(AnsiString cleDefaut, int attribut);

    // Cr�ation d'une paire � partir eulement d'un nom par d�faut (3)
    void CreateBasedID(AnsiString cleDefaut);

    // Modifie le nom d'un ID par un autre en conservant son num�ro
    // => renvoi faux si le nom de l'ID � remplacer n'existe pas
    //	           ou si le nouveau nom est d�j� utilis�	 
    bool ChangeClef(AnsiString src, AnsiString dest);

    // v�rifie si la cle "string" existe dans le dictionnaire
    bool IsCle(AnsiString cle);
    // v�rifie si l'�l�ment "int" existe dans le dictionnaire
    bool IsElement(int element);
    // v�rifie que le couple existe
    bool IsCouple(AnsiString &cle, int element);
    // Renvoie le plus grand attribut du dictinnaire
    int MaxAttribut();
    // Renvoie le n� correspondant � l'ID
    int NumID(AnsiString &cle);
    // Renvoie la string-cle correspondant au n�
    AnsiString CleID(int element);

  private:
	typedef std::map<AnsiString ,int, less<AnsiString> > DicoID;	// alias DicoID
	DicoID MapID;										    // Dictionnaire
    typedef DicoID::iterator Iterat;						// it�rateur sur le dico
};

} // fin du namespace std
#endif
