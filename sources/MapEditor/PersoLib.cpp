////////////////////////////////////////////////////////////////////////////////
//   UNITE : D�finitions des Personnages 		"PersoLib.cpp"                //
//                                                                            //
//	 Begin 02/07/98															  //
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "ZeNain.h"
#include "MapLib.h"
#include "GestFile.h"
#include "DDLibAsm.h"
#include "DILib.h"
#include "WorldSimulation.h"
#include "GameDebug.h"
#include "AnimAll.h"
#include "TimeClock.h"			// Gestion de l'horloge du jeu
#include "ScnEvents.h"			// Constantes 
#include "FileVirtual.h"		// Fichiers virtuels (dans un .pak, en m�moire ou tout simplement sur le disk)
#ifdef CBUILDER
#include "DDLibEditor.h"
#endif
#include "PersoLib.h"			// Son Header


Personnage *Hero= NULL,*Ogre = NULL;

//#define PERSOLIB_DEBUG
eCollideMode COLLIDE_MODE = COLLIDE_ZONE;   // collisions utilisant la zone-perso au sol

#define MAX_DECAL_PIXELS  7					// Nb maximal de pixels que le perso ait droit de se d�caler tout seul lors des collisions
const double RACINEde2 = sqrt(2);

//--- Liste de tous les mod�les de personnages du jeu ---
Character* ListeCharacters[NbCharacters];   // Liste des personnages du jeu

// Constructeur --------------------------------------------------------------//
Character::Character(int characType, int status):
    CharacterType(characType), // Indique le type de mod�le qu'il repr�sente :  HERO, OGRE ...
	iCharacterStatus(status)
{   // Librairies d'animations ou de transitions non encore charg�es
    AnimSpriteLib = NULL;
    AnimTransLib = NULL;
    peNbAnim = peNbTrans = 0;    	    // aucune animation ou transitions

    // d�fini la librairie de sprites � utiliser d'apr�s son type de character
	// <=> STLPersonnage = STLModelePersos[CharacterType]
    switch (CharacterType)
    {   case (HERO1) : STLPersonnage = STLHero; break;
        case (OGRE1) : STLPersonnage = STLOgre; break;
		case (NAINRENEGAT1) : STLPersonnage = STLModelePersos[NAINRENEGAT1]; break;
		case (MOINE1) : STLPersonnage = STLModelePersos[CharacterType]; break;
				
        default : STLPersonnage = NULL;  // � exclure
    }

    // S'occupe de la gestion des sprites superposables
    nbMembres = cNbMembresDuCorpsHumains;          // par d�faut, on prends celui du corps humains
    ObjetFrame = new ClisteObjetFrame[nbMembres];  // Tableau de nbMembres d'un tableau d'ObjetFrame

    // Initialise la Librairie des frames superpos�es du personnage
    FrameSuperpList = new SpriteFrameList();
    // Initialise la Librairie des sprites � superposer sur le perso
    STLFrameSuperp = new SpriteTileLib();
}
//----------------------------------------------------------------------------//

// Charge les informations sur Transitions & Animations ----------------------//
void Character::LoadIna(const char *InaName,SpriteTilePersoLib **STLPerso)
{   //  appelle la fonction LoadFileIna de "Animall.cpp"
    // Attention : R�pertoire NON complet    modifi� le 5/12/98
    char InaNameTresShort[ZENAIN_MAX_CHAR];
	strcpy(InaNameTresShort, InaName);	
    LoadFileIna(InaNameTresShort, &AnimSpriteLib, &AnimTransLib, &peNbAnim, &peNbTrans, STLPerso, this);
};
//----------------------------------------------------------------------------//

//=== Sauvegarde toutes les informations sur les sprites d'un personnage =====//
void Character::SaveToFileInfoSprites(FILE &fichier)
{  // Remplissage de l'en-t�te du bloc du fichier
   // Version du fichier
//   fichier << CGeneralSpriteInfos::Version;
    fwrite(&CGeneralSpriteInfos::Version,sizeof(CGeneralSpriteInfos::Version), 1, &fichier);
   // Nombre de sprites
 //  fichier << STLPersonnage->NbSprites;
    fwrite(&STLPersonnage->NbSprites,sizeof(STLPersonnage->NbSprites), 1, &fichier);

    SpriteTilePerso *Sprite;
    // Parcours toute la liste des sprites du perso
    for (int num = 0; num < STLPersonnage->NbSprites ; num++)
      {  Sprite = (SpriteTilePerso*) STLPersonnage->SprIndex[num];      // se place sur le bon sprite
         Sprite->GSprInf->SaveToFile(fichier);
      }
}
//----------------------------------------------------------------------------//

//=== Charge toutes les informations sur les sprites d'un personnage =========//
void Character::LoadFromFileInfoSprites(FILE &fichier)
{  // Remplissage de l'en-t�te du bloc du fichier
   // Version du fichier
    float Version;
    fread(&Version,sizeof(Version), 1, &fichier);
    // ATTENTION : Avant le 05/03/99 je mettais directement cette valeur dans STLPersonnage->NbSprites.
    //             Or, lorsque l'on rajoute des frames au fichier *.ina -> on rajoute des sprites
    int NbMdlSprites = 0;
    fread(&NbMdlSprites,sizeof(NbMdlSprites), 1, &fichier);
    SpriteTilePerso *Sprite;
    // Parcours toute la liste des sprites du perso
    for (int num = 0; num < NbMdlSprites ; num++)
	{	Sprite = (SpriteTilePerso*) STLPersonnage->SprIndex[num];      // se place sur le bon sprite
		Sprite->GSprInf->LoadFromFile(fichier);
	}
}
void Character::LoadFromFileInfoSprites(TFileVirtual &fichier)
{  // Remplissage de l'en-t�te du bloc du fichier
   // Version du fichier
    float Version;
    fichier.read(&Version, sizeof(Version));
    // ATTENTION : Avant le 05/03/99 je mettais directement cette valeur dans STLPersonnage->NbSprites.
    //             Or, lorsque l'on rajoute des frames au fichier *.ina -> on rajoute des sprites
    int NbMdlSprites = 0;
	fichier.read(&NbMdlSprites, sizeof(NbMdlSprites));
    SpriteTilePerso *Sprite = NULL;
    // Parcours toute la liste des sprites du perso
    for (int num = 0; num < NbMdlSprites ; num++)
	{	Sprite = (SpriteTilePerso*) STLPersonnage->SprIndex[num];      // se place sur le bon sprite
		Sprite->GSprInf->LoadFromFile(fichier);
	}
}
//----------------------------------------------------------------------------//




//=== Load fichier MDL =======================================================//
// Charge tout le contenu du fichier .mdl contenant toutes les informations
// de chaque tiles des anims du persos ...
void Character::LoadMdl(const char* MdlName)
{ 
  #ifndef NEW_FILE_MANAGER		
  FILE *f;
  // Ouvre le fichier binaire en lecture
  if ((f = fopen(MdlName,"rb"))==NULL)
           MessageBoxA(NULL,"Impossible de lire le fichier",MdlName,1);
  LoadFromFileInfoSprites(*f);
  fclose(f);
  #else
  TFileVirtual *file = NULL;
  // Ouvre le fichier binaire en lecture
  file = fopenFV((char*) MdlName);
  if (file == NULL) MessageBoxA(NULL,"Impossible de lire le fichier",MdlName,1);
  LoadFromFileInfoSprites(*file);
  fcloseFV(file);		
  #endif // NEW_FILE_MANAGER	
}
//----------------------------------------------------------------------------//
//=== Charge les bmp reli� � cet objet =======================================//
void Character::AllocateObjetBmp(int NumObjet)
{  // Recherche pour chaque membres du personnage dans la liste des bmp si l'image
   // est charg� ou non et la charge dans ce cas
   // Pour chaque membre du perso
   for (int m=0; m<nbMembres; m++)
   { // Pour chaque frame correspondant � l'un de ces membres
     for (uint i=0; i < ObjetFrame[m].TabObjetFrame.size() ; i++)
     { // Si cette frame correspond � l'objet
       if (ObjetFrame[m].TabObjetFrame[i].numIDTypeObjet == NumObjet)
       { int numFrame = ObjetFrame[m].TabObjetFrame[i].numFrameAnimSuperposee;
         // On cherche la bonne frame
         SpriteFrame* frame = FrameSuperpList->SF_List;
         if ((frame!=NULL) && (frame->IDFrame!=numFrame)) frame = frame->SF_Suiv;
         #ifdef PERSOLIB_DEBUG
         if (frame==NULL)
         {  MessageBoxA(NULL,"Frame non trouv�e !","Personnage::AllocateObjetBmp",1);
            Application->Terminate();
         }
         #endif
         // Regarde si la frame n'est pas d�j� charg�e
         if (!frame->BmpAlwaysIntoMemory)
         { if ((!frame->BmpLoadIntoMemory) && (frame->Semaphore==0))
           {  // On doit charger la frame en m�moire � partir du bmp sur DD
              frame->ReserveMemoryAndLoadBmp(&DS->pDD);
           }
           frame->Semaphore++;     // Un autre user utilise cette frame
         }
       }
     }
   }
}
//----------------------------------------------------------------------------//

//=== D�salloue la m�moire occup�e par les bmp repr�sentant cet objet ========//
void Character::DesallocateObjetBmp(int NumObjet)
{  // Recherche pour chaque membres du personnage dans la liste des bmp si l'image
   // est charg� ou non et la d�salloue s'il il faut
   // Pour chaque membre du perso
   for (int m=0; m<nbMembres; m++)
   { // Pour chaque frame correspondant � l'un de ces membres
     for (uint i=0; i < ObjetFrame[m].TabObjetFrame.size() ; i++)
     { // Si cette frame correspond � l'objet
       if (ObjetFrame[m].TabObjetFrame[i].numIDTypeObjet == NumObjet)
       { int numFrame = ObjetFrame[m].TabObjetFrame[i].numFrameAnimSuperposee;
         // On cherche la bonne frame
         SpriteFrame* frame = FrameSuperpList->SF_List;
         if ((frame!=NULL) && (frame->IDFrame!=numFrame)) frame = frame->SF_Suiv;
         #ifdef PERSOLIB_DEBUG
         if (frame==NULL)
         {  MessageBoxA(NULL,"Frame non trouv�e !","Personnage::DesallocateObjetBmp",1);
            Application->Terminate();
         }
         #endif
         // Regarde si la frame n'est pas d�j� charg�e
         if (!frame->BmpAlwaysIntoMemory)
         { if ((frame->BmpLoadIntoMemory) && (frame->Semaphore==1)) // Si c'est le seul � utiliser cette frame
           {  // On doit charger la frame en m�moire � partir du bmp sur DD
              frame->DesallocateMemory(&DS->pDD);
           }
           frame->Semaphore--;     // Un user de moins utilise cette frame
         }
       }
     }
   }
}
//----------------------------------------------------------------------------//
///////////////////////// FIN DE LA CLASSE CHARACTER ///////////////////////////

//=== Chgt de mode vid�o en plein cours du jeu ===============================//
void  ChangeListeCharactersVideoMode()
{	for (int i=0 ; i < NbCharacters ; i++)
	{	ListeCharacters[i]->FrameSuperpList->ChangeSpriteFrameVideoMode(&DS->pDD);
	}
}
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
// Fonctions Objets : PERSONNAGE											  //
////////////////////////////////////////////////////////////////////////////////

// Constructeur --------------------------------------------------------------//
Personnage::Personnage(int PersoType):
	GameObject(START_ID_ANY_PERSOS),
    ModelePersoType(PersoType)     // Type de personnage : HERO, OGRE, NAIN	
{
    peEtat = peFutEtat = peOldEtat = ARRET1;
    peCollideEtat = NOSTATE;
    peCollideDirection = -1;	// Aucune collision initialement
    ArmeEnMain = NOWEAPON;
    peSpeed = peFutSpeed = peOldSpeed = pedVit = 0;
    peAccelerate = true;
    peSpeedTrans = false;
    peAllCollisions = true;     // Toutes les collisions sont activ�es
    peNumSprite = 0;
	peSprNumLib = 0;
    peDirection = peFutDirection = or_S;
    PersoNivX = 5;
    PersoNivY = 4.5;
//    PersoNivX = 30;
//    PersoNivY = 34;
    PersoNivXY2UV();
    PersoNivXYtoPeXY();
//    peX = 100; peY = 300;
    //--- Attribue un mod�le/squelette � ce nouveau perso ---
    ModelePerso = ListeCharacters[ModelePersoType];
	
	uLastActionUse = 0;
	uTimeIntActionUse = 1200;	// 1.2 ms
	uLastActionHit = 0;
	uTimeIntActionHit = 800;	// 0.8 ms

	peAnimSpeed = 1.0;			// Vitesse d'animation par d�faut
	peNumSpriteInterm = 0;

}
//----------------------------------------------------------------------------//

//=== Positionne le perso sur le niveau ======================================//
void Personnage::SetPosition(double x, double y)
{  PersoNivX = x;
   PersoNivY = y;
   PersoNivXY2UV();
   PersoNivXYtoPeXY();
}
//----------------------------------------------------------------------------//

// Correspondance Clavier / Etats --------------------------------------------//
void Personnage::Keyb2FutEtat(void)
{
    // ** Nouvelles positions demand�es
    if ((Key_K) && (peFutEtat != MEURT1) && (peEtat != MEURT1) && (peFutEtat != MORT1) && (peEtat != MORT1))
     { peFutEtat = MEURT1; peFutDirection = peDirection; }
    else
    if (Key_SPACE)	// Se prend un coup ds la tet�
     { peFutEtat = PRENDCOUP1; peFutDirection= peDirection;

       //*** [ACF] [TEST] [DEBUG] ***
       // Si il est devant un objet du d�cors qui est un vase, on le casse
       if (FrontObject.occupee)
       {  // SANS L'UTILISATION DES SCRIPTS
		  /*if (FrontObject.decors->Element != NULL)
          if (FrontObject.decors->Element->numAnimTileDecorsID == 1) // BOCAL1
          {	if ((FrontObject.decors->peEtat==ARRET1) && (FrontObject.decors->peFutEtat==ARRET1)
          		&& (FrontObject.decors->peFutDirection==FrontObject.decors->peDirection))
       		{ if ((rand() % 2) == 1) FrontObject.decors->peFutEtat=BRISE1;
         	  else FrontObject.decors->peFutEtat=BRISE2;
       		}
          }*/
		  
		  // V�rifie si le personnage peut r�it�rer son action 'utiliser'
		  // -> l'intervalle de temps entre ces 2 m�mes actions doit d�passer son tps de r�ponse
		  unsigned int curentTime = GameHistory->GetTime();
		  if ((curentTime - uLastActionUse) > uTimeIntActionUse)
		  {	  
 			  uLastActionUse = curentTime;
			  CreateEvent( EVENTS_STATUS_DIRECT, EVENTS_TYPE_JOUEUR, 
 						   SCN_TRIGGER_PLAYER_UTILISER, FrontObject.decors);
		  }
       }
     }
    else
    if (Key_LCTRL)	// Donne un coup
    {  switch (ArmeEnMain)
       {	case NOWEAPON	: peFutEtat = DONNECOUP1; break;
			case EPEE1		: peFutEtat = COUPEPEE1; break;
			case PETOIRE1	: peFutEtat = TIRER1; break;
       }
       peFutDirection = peDirection;
    }
    else
    if (Key_LALT == EVENTKEY_APPUYE)  // Change d'arme
    {  ArmeEnMain++;
       if (ArmeEnMain >= NbWeapons) ArmeEnMain = 0;
    }
    else
        //  Aucune touche de direction appuy�e, ni entrain de tourner, ni dans un �tat d'attente ou d'arr�t
        //  => on s'arr�te
    if (    (!Key_K) && (!Key_SPACE) && (!Key_LCTRL) && (!Key_LALT) 
         && (!Key_UP) && (!Key_DOWN) && (!Key_RIGHT) && (!Key_LEFT)
     	 && (!Key_npE) && (!Key_npSE) && (!Key_npS) && (!Key_npSO)
         && (!Key_npO) && (!Key_npNO) && (!Key_npN) && (!Key_npNE)
         //&& (peFutDirection==peDirection)
         && ((peFutEtat!=TOURNER1) || (peFutEtat!=TOURNER2) || (peFutEtat!=DEMITOUR1))
         && (peEtat!=ARRET1) && (peEtat!=STAND1) && (peEtat!=MORT1)
         && (peEtat!=DEMITOUR1))
      { peFutEtat = ARRET1; peFutDirection= peDirection;
      }
    else
	
	// On le fait sauter sans trop d'�lan
	if ( (Key_BACKSLASH) && 
	 	 ( (peEtat==ARRET1) || (peEtat==MARCHE1) || (peEtat==STAND1))
		 )
		{	peFutEtat = SAUTE1;
		} 
	else
	// PRIORITE aux touches fl�ch�es
	// Si une des touches de direction est appuy�e :
    if ((Key_UP) || (Key_DOWN) || (Key_RIGHT) || (Key_LEFT))
      {
        if ((Key_UP) && (!Key_DOWN) && (!Key_RIGHT) && (!Key_LEFT)) peFutDirection = or_N
        else if ((Key_UP) && (!Key_DOWN) && (Key_RIGHT) && (!Key_LEFT)) peFutDirection = or_NE
        else if ((Key_UP) && (!Key_DOWN) && (!Key_RIGHT) && (Key_LEFT)) peFutDirection = or_NO

        else if ((!Key_UP) && (Key_DOWN) && (!Key_RIGHT) && (!Key_LEFT)) peFutDirection = or_S
        else if ((!Key_UP) && (Key_DOWN) && (Key_RIGHT) && (!Key_LEFT)) peFutDirection = or_SE
        else if ((!Key_UP) && (Key_DOWN) && (!Key_RIGHT) && (Key_LEFT)) peFutDirection = or_SO

        else if ((!Key_UP) && (!Key_DOWN) && (Key_RIGHT) && (!Key_LEFT)) peFutDirection = or_E
        else if ((!Key_UP) && (!Key_DOWN) && (!Key_RIGHT) && (Key_LEFT)) peFutDirection = or_O;

        //  Si le perso MARCHE ou le perso est � l'ARRET et ds m�me DIRECTION
        //  et la touche Shift n'est pas appuy�e => le perso va MARCHER
        if ( (!Key_LSHIFT) &&
              ((peEtat==MARCHE1) || ((peEtat==ARRET1) && (peFutDirection==peDirection))) )
   	     					peFutEtat = MARCHE1;
        else
        //    Si le perso MARCHE et la touche shift appuy�e
        // ou Si le perso COURT et la touche shift est appuy�e
        // ou Si le perso est � l'ARRET et la touche shif est appuy�e        
        // => le perso va COURIR
        if ( (Key_LSHIFT) &&
              (     (peEtat==MARCHE1)
                 || ((peEtat==ARRET1) && (peFutDirection==peDirection))
                 || (peEtat==COURSE1)
              )
            ) peFutEtat = COURSE1;
        else  
        // si il n'est pas en train d'attendre, il demande � s'ARRETER
		if ((peFutEtat!=STAND1) && (peEtat!=DEMITOUR1)) peFutEtat= ARRET1; 
     }
   else if (   (Key_npE) || (Key_npSE) || (Key_npS) || (Key_npSO)
            || (Key_npO) || (Key_npNO) || (Key_npN) || (Key_npNE) )
     {
        if (   (Key_npN) && (!Key_npNE) && (!Key_npE) && (!Key_npSE)  // vers le Nord
            && (!Key_npS) && (!Key_npSO) && (!Key_npO) && (!Key_npNO) ) peFutDirection = or_N
        else if
         (   (   (Key_npN) && (!Key_npNE) && (!Key_npE) && (!Key_npSE) // vers le Nord-Ouest
              && (!Key_npS) && (!Key_npSO) && (Key_npO) && (!Key_npNO) )
          || (   (!Key_npNE) && (!Key_npS) && (!Key_npSO) && (!Key_npE)
              && (Key_npNO) && (!Key_npSE) )
          )  peFutDirection = or_NO
        else if
         (  (!Key_npN) && (!Key_npNE) && (!Key_npE) && (!Key_npSE)  // vers l'Ouest
            && (!Key_npS) && (!Key_npSO) && (Key_npO) && (!Key_npNO) ) peFutDirection = or_O
        else if
         (   (   (!Key_npN) && (!Key_npNE) && (!Key_npE) && (!Key_npSE) // vers le Sud-Ouest
              && (Key_npS) && (!Key_npSO) && (Key_npO) && (!Key_npNO) )
          || (   (!Key_npNE) && (!Key_npN) && (Key_npSO) && (!Key_npE)
              && (!Key_npNO) && (!Key_npSE) )
          )  peFutDirection = or_SO
        else if
         (  (!Key_npN) && (!Key_npNE) && (!Key_npE) && (!Key_npSE)     // vers le Sud
            && (Key_npS) && (!Key_npSO) && (!Key_npO) && (!Key_npNO) ) peFutDirection = or_S
        else if
         (   (   (!Key_npN) && (!Key_npNE) && (Key_npE) && (!Key_npSE) // vers le Sud-Est
              && (Key_npS) && (!Key_npSO) && (!Key_npO) && (!Key_npNO) )
          || (   (!Key_npNE) && (!Key_npN) && (!Key_npSO) && (!Key_npO)
              && (!Key_npNO) && (Key_npSE) )
          )  peFutDirection = or_SE
        else if
         (  (!Key_npN) && (!Key_npNE) && (Key_npE) && (!Key_npSE)     // vers l'Est
            && (!Key_npS) && (!Key_npSO) && (!Key_npO) && (!Key_npNO) ) peFutDirection = or_E
        else if
         (   (   (Key_npN) && (!Key_npNE) && (Key_npE) && (!Key_npSE) // vers le Nord-Est
              && (!Key_npS) && (!Key_npSO) && (!Key_npO) && (!Key_npNO) )
          || (   (Key_npNE) && (!Key_npS) && (!Key_npSO) && (!Key_npO)
              && (!Key_npNO) && (!Key_npSE) )
          )  peFutDirection = or_NE;

       /*

        if ((Key_UP) && (!Key_DOWN) && (Key_RIGHT) && (!Key_LEFT)) peFutDirection = or_NE;
        if ((Key_UP) && (!Key_DOWN) && (!Key_RIGHT) && (Key_LEFT)) peFutDirection = or_NO;

        if ((!Key_UP) && (Key_DOWN) && (!Key_RIGHT) && (!Key_LEFT)) peFutDirection = or_S;
        if ((!Key_UP) && (Key_DOWN) && (Key_RIGHT) && (!Key_LEFT)) peFutDirection = or_SE;
        if ((!Key_UP) && (Key_DOWN) && (!Key_RIGHT) && (Key_LEFT)) peFutDirection = or_SO;

        if ((!Key_UP) && (!Key_DOWN) && (Key_RIGHT) && (!Key_LEFT)) peFutDirection = or_E;
        if ((!Key_UP) && (!Key_DOWN) && (!Key_RIGHT) && (Key_LEFT)) peFutDirection = or_O;
     */
        //	Si le perso MARCHE ou le perso est � l'ARRET et ds m�me DIRECTION
        //	=> le perso va MARCHER
/*        if ((peEtat==MARCHE1)
             || ((peEtat==ARRET1) && (peFutDirection==peDirection)) )
   							peFutEtat = MARCHE1;
        else peFutEtat= ARRET1;  // sinon il demande � s'ARRETER*/
     };
          //=== Mouvements automatiques ====
     // Si le perso est � l'�tat ARRET1, on peut le faire attendre
     if ((peEtat==ARRET1) && (peFutEtat==ARRET1))
     {  if ((rand() % 100) == 0) peFutEtat=STAND1;
     }

}
//----------------------------------------------------------------------------//

// Transitions FUTUR ETAT -> ETAT PRESENT ------------------------------------//
// � partir des Directions Futures & Pr�sentes du perso, regarde les transitions
// possibles donn�es par le *.ina et les effectuent si possible
void Personnage::Fut2PresEtat(void)
{   peChangeAnim = false;

    // Regarde si l'on peut Faire le changement de direction && d'Etat
    if ((peFutDirection!=peDirection) // on doit Tourner ...
         && ((peFutEtat!=TOURNER1) && (peFutEtat!=TOURNER2)) )  // si c'est pas d�j� fait !
    {   int Compar = peFutDirection - peDirection;
            // Peut-�tre un demi-tour
        if ((abs(Compar)==4) && (peEtat!=ARRET1))
        { peFutEtat=DEMITOUR1;
          peChangeAnim = true;
        }
        else // Choix du sens 1 ou 2
        if (Compar>0)
        { if (Compar<=3) peFutEtat=TOURNER1;   // sens Positif
          else peFutEtat=TOURNER2;             // sens N�gatif
          peChangeAnim = true;
        }
        else
        { if (Compar>=-3) peFutEtat=TOURNER2;
          else peFutEtat=TOURNER1;
          peChangeAnim = true;
        }
    } else
    { if ((peFutEtat==TOURNER1) || (peFutEtat==TOURNER2) || (peFutEtat==DEMITOUR1))
      {  peFutEtat = ARRET1;
      }
    }

    BOOL sort = FALSE;
    // scrute toutes les transitions possibles
    for ( int i=0 ; (i < ModelePerso->peNbTrans) && (sort==FALSE); i++)
    {  // CAS PARTICULIERS du perso entrain de TOURNER ou DEMITOUR }
       if (    ((peFutEtat==TOURNER1) || (peFutEtat==TOURNER2) || (peFutEtat==DEMITOUR1) )
            && (ModelePerso->AnimTransLib[i]->NumActFut==peFutEtat)     // si les actions pr�sentes et futures
            && (ModelePerso->AnimTransLib[i]->NumActCour==peEtat)       // correspondent bien � ce que l'on souhaite
            && (   (ModelePerso->AnimTransLib[i]->ImageA==peNumSprite)  // et si l'on est � la bonne image
                || (ModelePerso->AnimTransLib[i]->ImageA== -1) )        // ou cas o� ImageA = "n"
           ) // fin du if
       {
           switch (peFutEtat)
            { case TOURNER1 :
                   peDirection++;
                   if (peDirection > 7) peDirection = 0;
                   peCollideEtat = NOSTATE;
                   break;
               case TOURNER2 :
                   peDirection--;
                   if (peDirection < 0) peDirection = 7;
                   peCollideEtat = NOSTATE;
                   break;
               case DEMITOUR1 :	// en plein demi-tour
                   if (peDirection!=peFutDirection)                          // si la direction a encore chang�
                   { peOldEtat = peEtat;                                     // conserve l'ancien �tat
                     peEtat = ModelePerso->AnimTransLib[i]->NumActInterm;    // �tat voulu
                     if ((peOldEtat==MARCHE1) || (peOldEtat==COURSE1)) peFutEtat = peOldEtat;
                     peCollideEtat = NOSTATE;
                     if (ModelePerso->AnimTransLib[i]->ImageB!=-1)
                         peNumSprite = ModelePerso->AnimTransLib[i]->ImageB; //  sprite voulu
                     peDirection = peFutDirection;  	                     //  direction voulue
                   };
                   break;
            }  // switch
            sort = TRUE;
       }  // if

       // TOUT DROIT
       if (peFutEtat!=peEtat)  // Si l'�tat voulu n'est plus ds le m�me �tat que pr�c�demment
       {       
           if ( (ModelePerso->AnimTransLib[i]->NumActFut==peFutEtat)
           && ( ModelePerso->AnimTransLib[i]->NumActCour==peEtat)
           && (    (ModelePerso->AnimTransLib[i]->ImageA==peNumSprite)
                || (ModelePerso->AnimTransLib[i]->ImageA== -1)   // cas o� ImageA = "n"
              ) )
           {
              // Si il vient d'y avoir collision
              // et que le joueur essaye de refaire la m�me action ayant provoqu�e la collision
              if ((peCollideEtat!=NOSTATE) &&(peCollideEtat==peFutEtat))
              {  // On ne fait rien !
              }
              else  // Sinon, on le fait avancer
              {  peOldEtat = peEtat;                       // conserve l'ancien �tat
                 peEtat = ModelePerso->AnimTransLib[i]->NumActInterm;
                 peNumSprite = ModelePerso->AnimTransLib[i]->ImageB;
                 sort = TRUE;
                 peChangeAnim = true;

                 if ((peEtat!=ARRET1) && (peEtat!=STOPNET1))
                 { peCollideEtat = NOSTATE;
                 }
              }
           }
       }
       else
       // CHGT AUTONOME DE TRANSITION NumActFut==NumActCour   (ex: MEURT1->MORT1)
       if (    (ModelePerso->AnimTransLib[i]->NumActCour==peEtat)
            &&	(ModelePerso->AnimTransLib[i]->NumActFut==ModelePerso->AnimTransLib[i]->NumActCour)
            &&  (    (ModelePerso->AnimTransLib[i]->ImageA==peNumSprite)
                 || (ModelePerso->AnimTransLib[i]->ImageA==-1) ))
          { peOldEtat = peEtat;                       // conserve l'ancien �tat
            peEtat = ModelePerso->AnimTransLib[i]->NumActInterm;
            peFutEtat = peEtat;
            peNumSprite = ModelePerso->AnimTransLib[i]->ImageB;
            sort = TRUE;
            peChangeAnim = true;
          }

     } // for
};
//----------------------------------------------------------------------------//


// BOUGE LE PERSO ------------------------------------------------------------//
// Remarque : Attention, � cause de la restructuration de persolib.cpp, ce qui
//            se trouve ds cette m�thode est report� dans Personnage::Update()
//            et ne sert que pour les PNJ et non plus au h�ros.
void Personnage::peBouge()
{
    // Transfert des informations de l'action courante vers le perso
    ResultAction();

    // Calcul le d�placement et affecte les nouvelles coordonn�es au perso
    if (ModelePerso->iCharacterStatus!=Character::CHARACTER_STATUE)
	{	DeplacementRealist();
	}
    //Deplacement();

    //** New : on dessine le perso � part, comme �� on peut calculer ses coordonn�es qd on veut
    // Puis dessine le perso avec le scrolling
    // peFastDraw(DS->pDDSBack);

    // Si l'on arrive � la fin de l'animation et rien n'a chang�,
    //   => on revient � son d�but
    if (++peNumSprite>=peNbSprite) peNumSprite=0;
};
//----------------------------------------------------------------------------//

//== Affiche sur <pScreen> le perso ==========================================//
void Personnage::peFastDraw(DDSurface *pScreen)
{
    if (DS->HardwareClipping)
    { // GravX(peSprNumLib,STLPersonnage)
      SpriteDraw(peX-ViewMap->ScreenX-GravSolX(),peY-ViewMap->ScreenY-GravSolY(),peSprNumLib,pScreen,ModelePerso->STLPersonnage);
    } else
    {   //peSprNumLib = 0;
        if (SHADOW_PERSO) SpriteDrawShadowClip(peX-ViewMap->ScreenX-GravSolX(),peY- ViewMap->ScreenY-GravSolY(),peSprNumLib,pScreen,ModelePerso->STLPersonnage);
        else SpriteDrawFast(peX-ViewMap->ScreenX-GravSolX(),peY- ViewMap->ScreenY-GravSolY(),peSprNumLib,pScreen,ModelePerso->STLPersonnage);

        //==== On regarde si il n'y a pas un sprite superposable � afficher ===
        // R�cup�re le num�ro du type d'objet que le perso a en main
        int numIDTypeObjSprite = ArmeEnMain;

        SpriteTilePerso *SpritePers = (SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib];
        // Regarde dans le PersoSprite courant si ce sprite superposable n'a pas d�j� �t� supperpos� par ailleurs
        // [BUG] [A CORRIGER] [PREVENTION] ->Milia2000
		if (ModelePerso->iCharacterStatus != Character::CHARACTER_STATUE)
		if (SpritePers->GSprInf->ListAnimSuperp.Existe(numIDTypeObjSprite))
        {  // R�cup�re le bon num�ro de sprite � afficher
           int num = SpritePers->GSprInf->ListAnimSuperp.GetIDS(numIDTypeObjSprite);
           SpriteTile *SpriteSuperp = ModelePerso->STLFrameSuperp->SprIndex[num];
           int posX, posY;
           SpritePers->GSprInf->ListAnimSuperp.GetCoord(numIDTypeObjSprite, posX, posY);
           int numIDSprSuperp = SpritePers->GSprInf->ListAnimSuperp.GetIDS(numIDTypeObjSprite);
           int pX = peX-ViewMap->ScreenX-GravSolX() + SpritePers->GravX + posX - SpriteSuperp->GravX;
           int pY = peY- ViewMap->ScreenY-GravSolY() + SpritePers->GravY + posY - SpriteSuperp->GravY;
           SpriteDrawShadowClip(pX,pY,numIDSprSuperp,pScreen,ModelePerso->STLFrameSuperp);
        }
    }
};
//----------------------------------------------------------------------------//

// N� SPRITE DS LIBRAIRIE ----------------------------------------------------//
void Personnage::ResultAction(void)
{
    // Trouve le n� de l' ACTION COURANTE
    int anm = -1,  i = 0;
    while ( (i < ModelePerso->peNbAnim) && (anm==-1) )
      {  if (ModelePerso->AnimSpriteLib[i]->Numero==peEtat) anm = i;
      	 i++;  };

	//******** Encore un BUG inexpliqu� *******
	if (anm==-1) return;

    // renvoie les informations de l'ACTION
    peNbSprite=ModelePerso->AnimSpriteLib[anm]->NbSprites;
    //********* Y'AVAIS UN BUG ICI jusqu'au 7/04/99 *********//
    // Si l'animation a chang�e, il se peut que l'on doit revenir au d�part
    if (peNumSprite>=peNbSprite) peNumSprite=0;

    // renvoie le n� du sprite correspondant
    switch (ModelePerso->AnimSpriteLib[anm]->Type)
       {	// 8 directions
    		case 1 : peSprNumLib = peNumSprite
           			+ peNbSprite * peDirection
					+ ModelePerso->AnimSpriteLib[anm]->FirstSprite;
					break;
			// 1 direction
     		case 3 : peSprNumLib = peNumSprite + ModelePerso->AnimSpriteLib[anm]->FirstSprite;
					break;
			// 8 directions
	   		case 2 :  peSprNumLib = ModelePerso->AnimSpriteLib[anm]->SprRepertoire[peDirection][peNumSprite];
           			break;
			// 1 direction
	   		case 4 :  peSprNumLib = ModelePerso->AnimSpriteLib[anm]->SprRepertoire[0][peNumSprite];
	       			break;
            default : peSprNumLib = 0;  // au pire des cas

       };

	// Pr�vient son adversaire qu'il l'a touch�
	if (    ((peEtat == DONNECOUP1) || (peEtat == COUPEPEE1) || (peEtat == TIRER1))
		 && (peNumSprite == 10) )	// [ACF]
	// Si il y'a un objet ou un perso devant lui
	if (FrontObject.decors != NULL)
	{
		  // V�rifie si le personnage peut r�it�rer son action 'frapper'
		  // -> l'intervalle de temps entre ces 2 m�mes actions doit d�passer son tps de r�ponse
		  unsigned int curentTime = GameHistory->GetTime();
		  if ((curentTime - uLastActionHit) > uTimeIntActionHit)
		  {	  
 			  uLastActionHit = curentTime;
			  CreateEvent( EVENTS_STATUS_DIRECT, EVENTS_TYPE_JOUEUR, 
 						   SCN_TRIGGER_PLAYER_FRAPPER, FrontObject.decors);
		  }
	}
}
//----------------------------------------------------------------------------//

// DEPLACEMENT ---------------------------------------------------------------//
//   => calcul des nouvelles coordonn�es du perso � partir de divers facteurs
// pour l'instant BIDON : au FEELING
void Personnage::Deplacement(void)
{   // **** ATTENTION : vieille proc�dure � 2 sous *****
    int peXOld = peX, peYOld = peY; // sauvegarde des coordon�es du perso avant d�placement

    int avX = 4,avY = 4, avXY =3; // Donn�es bidons
    if (peEtat == MARCHE1)
      switch (peDirection)
        {
            case 0 : peX-=avX; break;
            case 1 : peX-=avXY; peY+=avXY; break;
            case 2 : peY+=avY;  break;
            case 3 : peY+=avXY; peX+=avXY; break;
            case 4 : peX+=avX; break;
            case 5 : peX+=avXY; peY-=avXY;   break;
            case 6 : peY-=avY; break;
            case 7 : peX-=avXY; peY-=avXY;   break;
        };

    // Calcule ses coordon�es sur la Map
    //  => +30 pour avoir les coordonn�es des pieds
    ViewMap->TransXYtoUV(peX/*-GravSolX()*/ , peY/*-GravSolY()*/, &peU, &peV);

    // Regarde si il ne marche pas sur un MUR
    if ((peU>=0) && (peV>=0) && (peU<MapNiveau->NivWidth) && (peV<MapNiveau->NivHeight))
    if (!MapNiveau->PrincEtage->LayerDecors->Carte[peU][peV].isAccessible())
      { peX = peXOld; peY = peYOld; };

    // fait suivre la cam�ra pour que le perso soit tjs au milieu
    // pour le moment
    ViewMap->OldScreenX = ViewMap->ScreenX;	// sauvegarde les anciennes valeures pour le scrolling du sol
    ViewMap->OldScreenY = ViewMap->ScreenY;
    ViewMap->OldScreenU = ViewMap->ScreenU;
    ViewMap->OldScreenU = ViewMap->ScreenV;

    ViewMap->ScreenX = peX - ViewMap->ScreenViewX/2;
    ViewMap->ScreenY = peY - ViewMap->ScreenViewY/2;
    ViewMap->PosCoinUV();

//	ViewMap->ScreenX -=0;
//	ViewMap->ScreenY -=0;
};
//----------------------------------------------------------------------------//


//=== Va chercher les coordonn�es du point repr�sentant le centre de gravit� ===
//    du personnage projet� au sol
int Personnage::GravSolX()
{   return (((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->GravSol.x);
}
int Personnage::GravSolY()
{   return (((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->GravSol.y);
}
//----------------------------------------------------------------------------//

//--- Outils de convertion des divers coordonn�es du perso -------------------//
//--- et fonctions supl�mentaires de convertion ------------------------------//
//=== Convertion des PersoNiv(X,Y) en PersoNiv(U,V) ==========================//
void Personnage::PersoNivXY2UV()
{    PersoNivU = PersoNivX / WorldModel.SQR_LENGTH;
     PersoNivV = PersoNivY / WorldModel.SQR_LENGTH;
	 peU = (int) PersoNivU;
	 peV = (int) PersoNivV;
}
void ConvNivXY2UV(double X, double Y, double &U, double &V)
{	 U = X / WorldModel.SQR_LENGTH;
     V = Y / WorldModel.SQR_LENGTH;
}
//----------------------------------------------------------------------------//

//=== Convertion des PersoNiv(U,V) en PersoNiv(X,Y) ==========================//
void Personnage::PersoNivUV2XY()
{     PersoNivX = PersoNivU * WorldModel.SQR_LENGTH;
      PersoNivY = PersoNivV * WorldModel.SQR_LENGTH;
}
void ConvNivUV2XY(double U, double V, double &X, double &Y)
{	 X = U * WorldModel.SQR_LENGTH;
     Y = V * WorldModel.SQR_LENGTH;
}
//----------------------------------------------------------------------------//

//=== Convertion des PersoNiv(X,Y) en pe(X,Y)=================================//
void Personnage::PersoNivXYtoPeXY()
{    PersoNivXY2UV();
     peX = (int) (( PersoNivU - PersoNivV ) * ISO_WIDTH_HALF);
     peY = (int) (( PersoNivU + PersoNivV ) * ISO_HEIGHT_HALF);
}
void ConvNivUVtoPeXY(double U, double V, int &pex, int &pey)
{	 pex = (int) (( U - V ) * ISO_WIDTH_HALF);
     pey = (int) (( U + V ) * ISO_HEIGHT_HALF);
}
//----------------------------------------------------------------------------//


//=== Convertion de pe(X,Y) en PersoNiv(U,V) =================================//
void ConvNivPeXYtoUV(int pex, int pey, double &U, double &V)
{	 double rx = pex* ISO_HEIGHT_HALF;
     double ry = pey * ISO_WIDTH_HALF;
     U = ( rx + ry ) / ISO_DENOMINATEUR;
     V = ( ry - rx ) / ISO_DENOMINATEUR;
}
//----------------------------------------------------------------------------//

//=== Convertion de pe(X,Y) en PersoNiv(X,Y) =================================//
void Personnage::PersoPeXYtoNivXY()
{    double rx = peX* ISO_HEIGHT_HALF;
     double ry = peY * ISO_WIDTH_HALF;
     PersoNivU = floor( ( rx + ry ) / ISO_DENOMINATEUR );
     PersoNivV = floor( ( ry - rx ) / ISO_DENOMINATEUR );
     PersoNivUV2XY();
}
//----------------------------------------------------------------------------//

//--- Convertions Totales ----------------------------------------------------//
//=== Convertion de PersoNiv(X,Y) en PersoNiv(U,V) et pe(X,Y) ================//
void ConvNivXYtoAll(double X, double Y, double &U, double &V, int &pex, int &pey)
{	ConvNivXY2UV(X, Y, U, V);
	ConvNivUVtoPeXY(U, V, pex, pey);
}
//=== Convertion de PersoNiv(U,V) en PersoNiv(X,Y) et pe(X,Y) ================//
void ConvNivUVtoAll(double U, double V, double &X, double &Y, int &pex, int &pey)
{	ConvNivUV2XY(U, V, X, Y);
	ConvNivUVtoPeXY(U, V, pex, pey);
}
//=== Convertion de pe (X,Y) en PersoNiv(U,V) et PersoNiv(X,Y) ===============//
void ConvNivUVtoAll(int pex, int pey, double &U, double &V, double &X, double &Y)
{	ConvNivPeXYtoUV(pex, pey, U, V);
	ConvNivUV2XY(U, V, X, Y);
}
//----------------------------------------------------------------------------//

//=== Effectue les r�glages de transitions entre 2 anims =====================//
void Personnage::ChangementAnim(void)
{   // V�rifie si le perso ne vient pas tout juste de changer d'anim
    if (peChangeAnim)  // if (peEtat!=peOldEtat)
    {  // a) Transition MARCHE->COURSE
       if ((peEtat==COURSE1) && (peOldEtat==MARCHE1))
       {  // Transition sur 2 pas
          peFutSpeed = WorldModel.PAS_HOMME_COURSE;
          pedVit = (peFutSpeed - WorldModel.PAS_HOMME_MARCHE)*2 / (peNbSprite*2);
          peOldSpeed = peSpeed;
          peAccelerate = true;
          peSpeedTrans = true;
       }
       else // d) Transition COURSE->MARCHE
       if ((peEtat==MARCHE1) && (peOldEtat==COURSE1))
       {  // Transition en 2 pas
          peFutSpeed = WorldModel.PAS_HOMME_MARCHE;
          pedVit = (peFutSpeed - WorldModel.PAS_HOMME_COURSE) / peNbSprite;
          peOldSpeed = peSpeed;
          peAccelerate = false;
          peSpeedTrans = true;
       }
       else // c) Tansition ARRET->MARCHE
       if ((peEtat==MARCHE1) && (peOldEtat==ARRET1))
       {  // Tansition sur 1 pas
          peFutSpeed = WorldModel.PAS_HOMME_MARCHE;
          pedVit = (peFutSpeed - 0) * 2 / peNbSprite;
          peOldSpeed = peSpeed;
          peAccelerate = true;
          peSpeedTrans = true;
       }
       else // d) Transition MARCHE->ARRET  !!!!!  pas prise en compte !!!!!!
       if ((peEtat==ARRET1) && (peOldEtat==MARCHE1))
       {  // Transition imm�diate
          peFutSpeed = 0;
          pedVit = -peSpeed;
          peOldSpeed = peSpeed;
          peAccelerate = false;
          peSpeedTrans = true;
       }
       else // e) Transition ARRET->COURSE
       if ((peEtat==COURSE1) && (peOldEtat==ARRET1))
       {  // Transition sur 3 pas
          peFutSpeed = WorldModel.PAS_HOMME_COURSE;
          pedVit = (peFutSpeed - 0)*2 / (peNbSprite*3);
          peOldSpeed = peSpeed;
          peAccelerate = true;
          peSpeedTrans = true;
       }
       else // f) Transition COURSE->ARRET
       if ((peEtat==ARRET1) && (peOldEtat==COURSE1))
       {  // Transition sur 2 pas
          peFutSpeed = 0;
          pedVit = (0 - WorldModel.PAS_HOMME_COURSE)*2 / (peNbSprite*2);
          peOldSpeed = peSpeed;
          peAccelerate = false;
          peSpeedTrans = true;
       }
       else // g) Transition COURSE->DEMITOUR1
       if ((peEtat==DEMITOUR1) && (peOldEtat==COURSE1))
       {  // Transition sur 2 pas
          peFutSpeed = 0;
          pedVit = (0 - WorldModel.PAS_HOMME_COURSE)*2 / (peNbSprite*2);
          peOldSpeed = peSpeed;
          peAccelerate = false;
          peSpeedTrans = true;
       }
       else // h) Transition MARCHE->DEMITOUR1
       if ((peEtat==DEMITOUR1) && (peOldEtat==MARCHE1))
       {  // Transition sur 1 retournement
          peFutSpeed = 0;
          pedVit = (0 - peSpeed) / (peNbSprite);
          peOldSpeed = peSpeed;
          peAccelerate = false;
          peSpeedTrans = true;
       }
       else // i) Transition DEMITOUR1->AMRCHE
       if ((peEtat==MARCHE1) && (peOldEtat==DEMITOUR1))
       {  // Tansition sur 1 pas
          peFutSpeed = WorldModel.PAS_HOMME_MARCHE;
          pedVit = (peFutSpeed - 0) * 2 / peNbSprite;
          peOldSpeed = peSpeed;
          peAccelerate = true;
          peSpeedTrans = true;
       }
       //peOldEtat = peEtat;    // Changement effectu�
    }
}
//----------------------------------------------------------------------------//

//=== Deplacement "r�aliste" du personnage ===================================//
//=> tiens compte des coordonn�es du niveau
void Personnage::DeplacementRealist(void)
{   // Coordonn�es sur lesquelles on va travailler
	double newPersoNivX = PersoNivX,
    	   newPersoNivY = PersoNivY,
           newPersoNivU = PersoNivU,
    	   newPersoNivV = PersoNivV;
   	int newPeX = peX, newPeY = peY,
        newPeU = peU, newPeV = peV;

    bool AvancePerso = true;		// Permet de faire avancer le perso si il n'y a pas eu de collisions


    // 0) V�rifie si le perso ne vient pas tout juste de changer d'anim
    ChangementAnim();

    // 1) Calcul des petits d�placements du personnage d'apr�s :
    //  - le type de personnage : Homme, loup, tortue ...
    //  - le nombre de frame composant une anim de marche : 12, 16 ...
    //  - l'�tat du personnage : fatigu�, bless�, dop� ...
    //  - le type de marche : discr�te, course, lent, rapide, press� ...

    // a) longeur r�elle � parcourir entre 2 frames d'anim
    // Calcule de la vitesse actuelle du perso
    if (peSpeedTrans)
    { peSpeed+=pedVit;
      if ((peAccelerate) && (peSpeed>=peFutSpeed))
      { peSpeed = peFutSpeed;
        pedVit = 0;
        peSpeedTrans = false;
      }
      else if ((!peAccelerate) && (peSpeed<=peFutSpeed))
      { peSpeed = peFutSpeed;
        pedVit = 0;
        peSpeedTrans = false;
      }
    }

    //peSpeed = WorldModel.PAS_HOMME_MARCHE;
    double longueur = peSpeed * 2 / peNbSprite;

    // b) d�placements horizontaux, verticaux & diagonaux
    double av_HV   = longueur;
    double av_Diag = longueur / sqrt(2);

    // c) Modification des coordonn�es du perso sur la Map du niveau
    if (   (peEtat == MARCHE1) || (peEtat == COURSE1)
        || ((peEtat == ARRET1) && (peSpeedTrans))
        || (peEtat == DEMITOUR1) )
    { // Convertion Direction Ecran -> Direction Carte
      eMapDirection peDirectionMap = (eMapDirection) peDirection;
      switch (peDirectionMap)   // Direction � l'�cran
        {   // Deplacement en diagonal sur la Carte
            case Map_SO : newPersoNivX-=av_Diag; newPersoNivY+=av_Diag; break;
            case Map_SE : newPersoNivX+=av_Diag; newPersoNivY+=av_Diag; break;
            case Map_NE : newPersoNivX+=av_Diag; newPersoNivY-=av_Diag; break;
            case Map_NO : newPersoNivX-=av_Diag; newPersoNivY-=av_Diag; break;
            // D�placements horizontaux
            case Map_E : newPersoNivX+=av_HV; break;
            case Map_O : newPersoNivX-=av_HV; break;
            // D�placements verticaux
            case Map_S : newPersoNivY+=av_HV; break;
            case Map_N : newPersoNivY-=av_HV; break;
        };
    }
    // Convertion des cordonn�es (newPersoNivX, newPersoNivY) dans la vue du jeu et dans la vue 3D ISO
    ConvNivXYtoAll(newPersoNivX, newPersoNivY, newPersoNivU, newPersoNivV, newPeX, newPeY),
    // Calcule ses coordonn�es sur la Map formees de trapezes
    newPeU = (int) newPersoNivU;  newPeV = (int) newPersoNivV;

    // Regarde si les coordonn�es de ses pieds n'est pas sur un mur
/*    if (COLLIDE_MODE==COLLIDE_PIXEL)
    { if ( (PersoNivU>=0) && (PersoNivV>=0)
           && (PersoNivU<MapNiveau->NivWidth) && (PersoNivV<MapNiveau->NivHeight))
          if (!MapNiveau->PrincEtage->LayerDecors->Carte[(int) PersoNivU][(int) PersoNivV].isAccessible())
          {  PersoNivX = persoNivXOld;
             PersoNivY = persoNivYOld;
             PersoNivXY2UV();
          //   peEtat = ARRET1;
          }
    }       */


    //*** Si c'est le personnage joueur, on teste les collisions ***
    if (peAllCollisions)
    {
      //=== Autre test de collisions ===
      if (COLLIDE_MODE==COLLIDE_ZONE)
      { // 0) Au d�part, on suppose qu'il n'y a pas de collisions
        bool Collision = false;

        // 1) Si l'on a d�tect� une collision la fois d'avant et que le perso va tjs dans la m�me direction -> collision
        if ( (!Collision) && (peDirection == peCollideDirection))
        {	if ((peEtat==MARCHE1) || (peEtat==ARRET1) || (peEtat==COURSE1))
        	{	Collision = true;
	        }
        }

        // 2) TEST COLLISIONS avec des Objets circulaires ***
        if (!Collision)
        if ((peEtat==MARCHE1) || (peEtat==ARRET1))
        {  bool realCollide = DetectColis_DecorsCircle(newPersoNivX, newPersoNivY, newPersoNivU, newPersoNivV, newPeX, newPeY, newPeU, newPeV);
           Collision = realCollide;
        }

         // 1) Regarde tout d'abord si les pieds du persos ne sont pas sur un d�cors infrancissable
        // => Si les pieds du perso sont sur la carte
        if (!Collision)
        if (    (newPersoNivU>=0) && (newPersoNivV>=0)
             && (newPersoNivU<MapNiveau->NivWidth) && (newPersoNivV<MapNiveau->NivHeight))
        { // => On regarde si le centre des pieds du perso se trouve sur la carte
          TCase *Case = &MapNiveau->PrincEtage->LayerDecors->Carte[newPeU][newPeV];
          if (!Case->isAccessible())
          {	// *** TEST COLLISIONS ***
           	Collision = true;

            // Si c'est un objet ayant une zone de collision circulaire, on n'en tient pas compte
            if ((Case->ptrObjet!=NULL) && (Case->ptrObjet->DecorsAnime) && (Case->ptrObjet->SecondTile!=NULL) && (Case->ptrObjet->SecondTile->CircleCollision)) Collision = false;
            else if ((Case->ptrObjet!=NULL) && (!Case->ptrObjet->DecorsAnime) && (Case->ptrObjet->DecTileObj!=NULL) && (Case->ptrObjet->DecTileObj->CircleCollision)) Collision = false; 

          }
        }

        // 3) Si l'on n'a pas de collisions a ce niveau l�, on pousse le test
        if (!Collision)
        { // *** TEST COLLISIONS ***
           Collision = DetectColis_Decors(newPersoNivX, newPersoNivY, newPersoNivU, newPersoNivV, newPeX, newPeY, newPeU, newPeV);
           //if (Collision) Totale = true;       // Rmq : on peut enlever le "if (collision)"
        }

        // 4) Si l'on a rencontr� des collisions dans une de ces 2 phases, on doit revenir
        //    � la position pr�c�dente du perso et l'arr�ter NET
        if (Collision)
        {  // On NE change PAS les coordonn�es du perso : on ne tient pas compte des nouvelles
           // Mais on change les ETATS du perso
           peCollideEtat = peEtat; // Conserve l'�tat dans lequel on a d�tect� une collision totale
           peCollideDirection = peDirection;
           peFutEtat = STOPNET1;   // On stop net le perso
           peOldSpeed = peSpeed;   // On sauvegarde l'ancienne vitesse actuelle du perso
           peSpeed   = 0;
           peFutSpeed =0;
           peSpeedTrans = false;
           // Change la frame d'animation � afficher
           Fut2PresEtat();
           ResultAction();
           // On ne doit pas faire avancer le personnage
           AvancePerso = false;
        }
        else
        {  peCollideDirection = -1;	// Aucune Collision de d�tect�e
        }	
      }
      else
      {


         #ifdef PERSOLIB_DEBUG
         DS->pDDSBack->Lock2();
         if (ModelePersoType==HERO1) DS->pDDSBack->PutPixel(peX-ViewMap->ScreenX,peY-ViewMap->ScreenY,0xF01);
         DS->pDDSBack->UnLock();
         #endif
      }
    }

    // R�tablit les nouvelles coordonn�es du perso si il n'y a pas eu de collisions
    if (AvancePerso)
    {    SetCoords(newPersoNivX, newPersoNivY, newPersoNivU, newPersoNivV, newPeX, newPeY, newPeU, newPeV);
    }

    // fait suivre la cam�ra pour que le perso soit tjs au milieu
    // pour le moment
    if (ModelePersoType==HERO1)
    { ViewMap->ScreenX = peX - ViewMap->ScreenViewX/2;
      ViewMap->ScreenY = peY - ViewMap->ScreenViewY/2;
      ViewMap->PosCoinUV();
    }
}
//----------------------------------------------------------------------------//

//=== Calcul les coords (U,V) des points (px,py) =============================//
//=== et retourne le coin du losange dans lequel il se trouve ================//
void Personnage::ConvertXYCtoUVC(int px, int py,int &pu, int &pv, eVertical &vert, eHorizontal &horiz,
                                 bool strictGauche, bool strictBas)
{   // Calcul pu et pv
    double rx = px* ISO_HEIGHT_HALF;
    double ry = py * ISO_WIDTH_HALF;
    pu = int( ( rx + ry ) / ISO_DENOMINATEUR );
    pv = int( ( ry - rx ) / ISO_DENOMINATEUR );
    // puis calcul la difference
    int tempX = ( pu - pv ) * ISO_WIDTH_HALF;
    int tempY = ( pu + pv ) * ISO_HEIGHT_HALF;
    int difX = px - tempX;
    int difY = py - tempY;

/*   int lX, lY;
   ViewMap->TransXYtoUV(px,py,&pu,&pv);
   ViewMap->TransUVtoXY(pu,pv,&lX,&lY);
   int difX = px - lX;
   int difY = py - lY; */


	if (   (difX>ISO_WIDTH_HALF) ||  (difX<-ISO_WIDTH_HALF) 
	    || (difY<0) || (difY>ISO_HEIGHT) )
	{
		#ifdef CBUILDER
		Application->Terminate();
		#else
		_asm int 3;
		#endif
	}

    if (strictGauche)
    { if (difX >=0) horiz = DROITE;   // point � DROITE du losange
      else          horiz = GAUCHE;   // point � GAUCHE du losange
    } else
    { if (difX >0)  horiz = DROITE;   // point � DROITE du losange
      else          horiz = GAUCHE;   // point � GAUCHE du losange
    }
    if (strictBas)
    { if (difY > ISO_HEIGHT_HALF) vert = BAS;   // point en BAS du losange
      else                        vert = HAUT;  // point en HAUT du losange
    } else
    { if (difY >=ISO_HEIGHT_HALF) vert = BAS;   // point en BAS du losange
      else                        vert = HAUT;  // point en HAUT du losange
    }
}
//----------------------------------------------------------------------------//

//--- Structure repr�sentant les coordonn�es d'un point (U,V) sur la map ---
struct tPoint
{ int u, v;
  void SetPoint(int i_u, int i_v) { u = i_u; v = i_v; };
  bool Equal(int &u2,int &v2) { return ((u==u2) && (v==v2)); };
};
//--- Tableau  de points � 2 colonnes et N lignes ---
const unsigned int MAX_LIGNES  = 50,
                   NB_COLONNES = 2;
tPoint Tab [MAX_LIGNES][NB_COLONNES];


//=== Collisions du perso avec le d�cors =====================================//
bool Personnage::DetectColis_Decors(double &newPersoNivX, double &newPersoNivY, double &newPersoNivU, double &newPersoNivV, int &newPeX, int &newPeY, int &newPeU, int &newPeV)
{  // R�cup�re les coordonn�es du rectangle de la zone au sol du perso
   int pt_x1 = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->ZoneSol.x1,
       pt_y1 = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->ZoneSol.y1,
       pt_x2 = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->ZoneSol.x2,
       pt_y2 = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->ZoneSol.y2;
   // Et les coordonn�es du centre des pieds ddu perso
   int gravX = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->GravSol.x,
       gravY = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->GravSol.y;
   // Convertit les coordonn�es du rectangle dans le rep�re de l'�cran
   int pX1 = newPeX - (gravX - pt_x1),
       pY1 = newPeY - (gravY - pt_y1),
       pX2 = newPeX - (gravX - pt_x2),
       pY2 = newPeY - (gravY - pt_y2);

   //*** Debug ***
   #ifdef PERSOLIB_DEBUG
   DS->pDDSBack->Lock2();
   if (ModelePersoType==HERO1)
   { DS->pDDSBack->PutPixel(pX1-ViewMap->ScreenX,pY1-ViewMap->ScreenY,0xF01);
     DS->pDDSBack->PutPixel(pX2-ViewMap->ScreenX,pY1-ViewMap->ScreenY,0xF01);
     DS->pDDSBack->PutPixel(pX1-ViewMap->ScreenX,pY2-ViewMap->ScreenY,0xF01);
     DS->pDDSBack->PutPixel(pX2-ViewMap->ScreenX,pY2-ViewMap->ScreenY,0xF01);
   }
   DS->pDDSBack->UnLock();
   #endif

   // Convertit ces coordonn�es de type (X,Y) en (U,V) et r�cup�re le coin
   // du losange dans lequel ils se trouvent
   int U1, V1, U2, V2, U3, V3, U4, V4;
   eVertical   vert1,  vert2,  vert3,  vert4;
   eHorizontal horiz1, horiz2, horiz3, horiz4;

   // Point en HAUT � GAUCHE (U1,V1)
   ConvertXYCtoUVC(pX1, pY1, U1, V1, vert1, horiz1, false, false);
   // Point en HAUT � DROITE (U2,V2)
   ConvertXYCtoUVC(pX2, pY1, U2, V2, vert2, horiz2, true, false);
   // Point en BAS � GAUCHE (U3,V3)
   ConvertXYCtoUVC(pX1, pY2, U3, V3, vert3, horiz3, false, true);
   // Point en BAS � DROITE (U4,V4)
   ConvertXYCtoUVC(pX2, pY2, U4, V4, vert4, horiz4, true, true);

   //=== D�but de l'algo ===
   // Index du tableau
   int indexG=0, indexD=0;
   const int cGAUCHE = 0,
             cDROITE = 1;

   //--- I] Remplissage des 1�res lignes du tableau ---
   // 0) Si les 2 points du haut (U1,V1) et (U2,V2) sont sur la m�me CASE
   if ((U1==U2) && (V1==V2))
   {  Tab[indexG][cGAUCHE].SetPoint(U1,V1);
      Tab[indexD][cDROITE].SetPoint(U2,V2);
      indexG = indexD = 0;
   }
   else
   { // 1) Si les 2 points du haut (U1,V1) et (U2,V2) sont sur la m�me LIGNE
     if ((U1+V1)==(U2+V2))
     {  // 1.a) Si ils sont en HAUT
        if (vert1==HAUT)
        {  Tab[indexG][cGAUCHE].SetPoint(U1,V1-1);
           Tab[indexD][cDROITE].SetPoint(U2-1,V2);
           Tab[indexG+1][cGAUCHE].SetPoint(U1,V1);
           Tab[indexD+1][cDROITE].SetPoint(U2,V2);
           indexG = indexD = 1;  // index sur la 2nde ligne du tableau
        }
        else  // 1.b) Sinon ils sont en BAS
        {  Tab[indexG][cGAUCHE].SetPoint(U1,V1);
           Tab[indexD][cDROITE].SetPoint(U2,V2);
           indexG = indexD = 0;   // index sur la 1�re ligne du tableau
        }
     }
     // 2) Sinon un des 2 Points est plus haut que l'autre
     else
     {  // 2.a)  Si c'est le point en HAUT � DROITE qui est le plus haut
        if ((U1+V1)>(U2+V2))
        {  Tab[indexG][cGAUCHE].SetPoint(U1,V1-1);
           Tab[indexD][cDROITE].SetPoint(U2,V2);
           Tab[indexG+1][cGAUCHE].SetPoint(U1,V1);
           indexG = 1; indexD = 0;
        }
        else  // 2.b) Sinon c'est le point en HAUT � GAUCHE qui est le plus haut
        {  Tab[indexG][cGAUCHE].SetPoint(U1,V1);
           Tab[indexD][cDROITE].SetPoint(U2-1,V2);
           Tab[indexD+1][cDROITE].SetPoint(U2,V2);
           indexG = 0; indexD = 1;
        }
     }
   }

   //--- II] Remplissage des 2 colonnes du tableau ---
   for (int col = 0; col < NB_COLONNES; col++)  // Pour chacune des 2 colonnes faire
   {  // 1) D�termine l'index � utiliser d�pendant des colonnes et le point d'arr�t
      int index = 0, termU = 0, termV = 0;
      eHorizontal  horiz = horiz1;
      switch (col)
      {  case cGAUCHE : index = indexG; termU = U3; termV = V3; horiz = horiz1; break;
         case cDROITE : index = indexD; termU = U4; termV = V4; horiz = horiz2; break;
      }
      // 2) D�termine le sens de descente du tableau
      eHorizontal direct;
      direct = horiz;     // Si le coin est � Gauche alors on va � Gauche, sinon on va � Droite

      // 3) Tant que l'on est pas arriv� en bas de la colonne -> on descend
      int tempU, tempV;
      while (!Tab[index][col].Equal(termU,termV))
      {  // 3.a) On r�cup�re les coordonn�es de l'ancienne case
         tempU = Tab[index][col].u;
         tempV = Tab[index][col].v;
         index++;     // On descend d'une ligne

         //*** Debuggage ***
     //    if (index>=MAX_LIGNES) return;

         // 3.b) On choppe la new case
         switch (direct)
         {  case GAUCHE : Tab[index][col].SetPoint(tempU,tempV+1); direct = DROITE; break;
            case DROITE : Tab[index][col].SetPoint(tempU+1,tempV); direct = GAUCHE; break;
         }
      }

      // 4) On remet les index gauche & droite au bon niveau
      switch (col)
      {  case cGAUCHE : indexG = index; break;
         case cDROITE : indexD = index; break;
      }
      // Normalement, en principe, on a ici :
      //   -  Tab[indexG,cGAUCHE] = (U3,V3)
      //   -  Tab[indexD,cDROITE] = (U4,V4)
   }

   //--- III] Remplissage de la derni�re ligne ---
   int indexMax;
   // 0) => si les 2 points (U3,V3) et (U4,V4) ne sont pas confondu alors
   if (!((Tab[indexG][cGAUCHE].u==Tab[indexD][cDROITE].u) && (Tab[indexG][cGAUCHE].v==Tab[indexD][cDROITE].v)))
   // <=> !(U3==U4 && V3==V4)
   {  //  1) Si ces 2 points sont sur la m�me ligne
      if ((U3+V3)==(U4+V4))
      {  // 1.a) Si ils sont en HAUT, c'est fini
         if (vert3==HAUT) { indexMax = indexG; }
         else
         { // 1.b) Sinon, ils sont en BAS et faut compl�te la derni�re ligne
           Tab[indexG+1][cGAUCHE].SetPoint(U3+1,V3);
           Tab[indexD+1][cDROITE].SetPoint(U4,V4+1);
           indexMax = indexG+1;
         }
      }
      else // 2) Si ils ne sont pas sur la m�me ligne
      {  // 2.a) Si c'est le point en bas � droite (U4,V4) qui est le plus haut
         if (indexG>indexD)
         {  Tab[indexD+1][cDROITE].SetPoint(U4,V4+1);
            indexMax = indexG;
         }
         else // 2.b) c'est le point (U3,V3) qui est le plus haut des 2
         {  Tab[indexG+1][cGAUCHE].SetPoint(U3+1,V3);
            indexMax = indexD;
         }
      }
   }
   else
   {  indexMax = indexG;
   }

   // IV]  Affichage des r�sultats => Debuggage
   int cU, cV, cUMax, cVMax;

   // 1) Pour toutes les lignes remplies du tableau
   for (int index=0; index <= indexMax ; index++)
   {  // 2) On scrute toutes les cases de la ligne de gauche � droite
      cU = Tab[index][cGAUCHE].u;
      cV = Tab[index][cGAUCHE].v;
      cUMax = Tab[index][cDROITE].u;
      cVMax = Tab[index][cDROITE].v;
     //*** Debuggage *** if ((cU+cV)!=(cUMax+cVMax)) return false;
      bool finalLigne = false;
      while (!finalLigne)
      {  // Pour l'instant, affiche les losanges
         int lX, lY;
         if ((cU>0) && (cV>0))
         {  ViewMap->TransUVtoXY(cU,cV,&lX,&lY);
            #ifdef PERSOLIB_DEBUG
            if (ModelePersoType==HERO1) DS->BackISOTrapeze(lX-ViewMap->ScreenX,lY-ViewMap->ScreenY, 0x0F1,0);
            #endif
         }

         // Regarde si cette case se trouve sur un mur
         if ( (cU>=0) && (cV>=0) && (cU<MapNiveau->NivWidth) && (cV<MapNiveau->NivHeight))
           if (  // Si c'est sur un losange du d�cors Infranchissable
                 (!MapNiveau->PrincEtage->LayerDecors->Carte[(int) cU][(int) cV].isAccessible())
                 // Et si il faut des collisions totales
                &&(MapNiveau->PrincEtage->LayerDecors->Carte[(int) cU][(int) cV].TrapCollide == TOTALE) )
           { return true;
           }

         if ((cU==cUMax) && (cV==cVMax)) finalLigne = true;
         cV--; cU++;   // Avance d'une case sur la droite
      }
   }

   return false;       // Tout c'est bien pass� ! aucune collisions rencontr�es ...
}
//----------------------------------------------------------------------------//

//=== Met � jour tous les param�tres du personnage joueur (H�ros) ============//
void Personnage::Update()
{  // R�cup�re ce que souhaite faire le joueur : �tat du clavier et du joypad
   Keyb2FutEtat();

   // Transforme ses d�sirs en r�alit� (du moins essaye ...)
   Fut2PresEtat();

   // Le fait avancer <=> peBouge();
   // Transfert des informations de l'action courante vers le perso
   ResultAction();

   // Calcul le d�placement et affecte les nouvelles coordonn�es au perso
   DeplacementRealist();   //Deplacement();

   // It�ration de l'animation courante
   // Si l'on arrive � la fin de l'animation et rien n'a chang� => on revient � son d�but
   if (++peNumSprite>=peNbSprite) peNumSprite=0;

   // Regarde si il n'est pas en face d'un objet du d�cors
   if (!DetectObjet_Rayon(2, FrontObject))
   {	FrontObject.occupee = false;	// Aucun objet du d�cors devant le perso 
		FrontObject.decors = NULL;
   }
}
//----------------------------------------------------------------------------//

//##ModelId=38A4889F0208
struct CircleDecors
{	DecorsObjet*  objet;	 //  ptr vers un d�cors intervenant dans les collisions circulaires
	//##ModelId=38A4889F020C
	TileDecors*	  tile;		 //  ptr vers le tile sur lequel le d�cors s'appuye
	int cercleX, cercleY;	 //  coordonn�es du centre du cercle sur la carte
	//##ModelId=38A4889F0209
    int rayon;				 //  rayon du cercle

	//##ModelId=38A4889F0249
    CircleDecors::CircleDecors(DecorsObjet* obj, TileDecors* til, int x, int y, int r):		// Constructeur
    	objet(obj),tile(til),cercleX(x),cercleY(y),rayon(r) {};
};

//=== Changement de rep�re : ISO -> Cart�sien ================================//
void TransUVtoXY(int U, int V, int *X, int *Y)
{   // Coordonn�es de l'angle sup�rieur du trap�ze
    *X = ( U - V ) * ISO_WIDTH_HALF;
    *Y = ( U + V ) * ISO_HEIGHT_HALF;
}
//----------------------------------------------------------------------------//

bool isInCircle(int cX, int cY, int rayon, int pX, int pY)
{  int dx = pX - cX;
   int dy = pY - cY;
   return (sqrt(dx*dx + dy*dy) <= rayon);
}

int nbPixel(int cercleX, int cercleY, int rayon, int X1, int Y1, int X2, int Y2, int direction, int &dX, int &dY)
{   int compteur = 0;
	dX = dY = 0;

 	switch (direction)
    { case 0 :  // Ouest    ->  droite verticale
      case 4 :  // Est
			if (Y2>Y1)
            {  for (int y=Y1 ; y <= Y2 ; y++, dY++)
               {  if (isInCircle(cercleX, cercleY, rayon, X1, y)) compteur++;
                  else return compteur;
               }
            } else
            {  for (int y=Y2 ; y >= Y2 ; y--, dY--)
               {  if (isInCircle(cercleX, cercleY, rayon, X1, y)) compteur++;
                  else return compteur;
               }
            }
            break;
      case 2 :  // Sud    ->  droite horizontale
      case 6 :  // Nord
			if (X2>X1)
            {  for (int x=X1 ; x <= X2 ; x++, dX++)
               {  if (isInCircle(cercleX, cercleY, rayon, x, Y1)) compteur++;
                  else return compteur;
               }
            } else
            {  for (int x=X1 ; x >= X2 ; x--, dX--)
               {  if (isInCircle(cercleX, cercleY, rayon, x, Y1)) compteur++;
                  else return compteur;
               }
            }
            break;
      case 1 :  // Sud-Ouest   -> diagonale � partie de haut-gauche
      case 5 :  // Nord-Est
      		if (X2>X1)
            {  for (int x=X1, y=Y1 ; (x<=X2) && (y<=Y2) ; x++, y++, dX++, dY++)
               {  if (isInCircle(cercleX, cercleY, rayon, x, y)) compteur++;
                  else return compteur;
               }
            } else
            {  for (int x=X1, y=Y1 ; (x>=X2) && (y>=Y2) ; x--, y--, dX--, dY--)
               {  if (isInCircle(cercleX, cercleY, rayon, x, y)) compteur++;
                  else return compteur;
               }
            }
      		break;
      case 3 :  // Sud-Est   -> diagonale � partie de haut-droite
      case 7 :  // Nord-Ouest
      		if (X2<X1)
            {  for (int x=X1, y=Y1 ; (x>=X2) && (y<=Y2) ; x--, y++, dX--, dY++)
               {  if (isInCircle(cercleX, cercleY, rayon, x, y)) compteur++;
                  else return compteur;
               }
            } else
            {  for (int x=X1, y=Y1 ; (x<=X2) && (y>=Y2) ; x++, y--, dX++, dY--)
               {  if (isInCircle(cercleX, cercleY, rayon, x, y)) compteur++;
                  else return compteur;
               }
            }
      		break;
    }
    return -1;
}

// �change 2 integers  (utilis� par l'algo de lignes)
void swap(int &a, int&b)
{   int Temp = a;
    a = b;
    b = Temp;
}

//=== Detecte les collisions avec les petits d�cors circulaires ==============//
bool Personnage::DetectColis_DecorsCircle(double &newPersoNivX, double &newPersoNivY, double &newPersoNivU, double &newPersoNivV, int &newPeX, int &newPeY, int &newPeU, int &newPeV)
{    const int BlocPerimetre = 4;          // S'int�resse � une zone rectangulaire form�e de blocs autour du personnage
     std::vector<CircleDecors> DecorsList; // Liste des d�cors se trouvant autour du perso
     TMap * MapDecors = MapNiveau->PrincEtage->LayerDecors;    // Carte des d�cors sur laquelle le perso se trouve [A MODIFIER]

     // 1) R�cup�ration des objets du d�cors pr�sents � c�t� du perso et servant lors des tests de collisions circulaires
     int posU, posV;
     DecorsObjet* decors;
     for (int u = -BlocPerimetre ; u <= +BlocPerimetre ; u++)
     { for (int v = -BlocPerimetre ; v <= +BlocPerimetre ; v++)
       { // Position d'un bloc autour du perso
         posU = u + newPeU;
         posV = v + newPeV;
         // Si cette case est dans le niveau
         if ((posU>= 0) && (posV>=0) && (posU<MapDecors->MapWidth) && (posU<MapDecors->MapHeight))
         {  decors = MapDecors->Carte[posU][posV].ptrObjet;
            if (decors!=NULL)  // Si cettte case est occup�e par un Objet du d�cors
            { // Si cet objet tient compte des collisions circulaires
              if (decors->DecorsAnime)  // Objets anim�s
              { if (decors->SecondTile->CircleCollision)  // Un objet susceptible d'intervenir ds les collisions circulaires trouv�
                {  int cX, cY;
      			   TransUVtoXY(decors->PosU, decors->PosV, &cX, &cY);
                   cX += decors->SecondTile->CircleCenterX - decors->SecondTile->DecorsCornerX - decors->SecondTile->TrapPrincX;
                   cY += decors->SecondTile->CircleCenterY - decors->SecondTile->DecorsCornerY - decors->SecondTile->TrapPrincY;
                   DecorsList.push_back(CircleDecors(decors, decors->SecondTile, cX, cY, decors->SecondTile->CircleRadius));
                }
              } else  // D�cors statique
              { if (decors->DecTileObj->CircleCollision)
                {  int cX, cY;
      			   TransUVtoXY(decors->PosU, decors->PosV, &cX, &cY);
                   cX += decors->DecTileObj->CircleCenterX - decors->DecTileObj->DecorsCornerX - decors->DecTileObj->TrapPrincX;
                   cY += decors->DecTileObj->CircleCenterY - decors->DecTileObj->DecorsCornerY - decors->DecTileObj->TrapPrincY;
                   DecorsList.push_back(CircleDecors(decors, decors->DecTileObj, cX, cY, decors->DecTileObj->CircleRadius));
                }
              }
       		}
         }
       }
     }
     if (DecorsList.size()==0) return false;         // Si on n'a pas trouv� d'objets, pas la peine de continuer

     // 2) Elimination des objets situ�s derri�res le perso


     // 3) On r�cup�re les 2 points de la droite situ�e devant le perso
     int x1=0, y1=0, x2=0, y2=0;
     ZoneOrientRect &zonePied = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->ZonePied;
     int points[4][2];
     int pointX1=0, pointY1=0, pointX2=0, pointY2=0;

     // R�orientation des points dans le rectangle
 	 if ((peDirection==7) || (peDirection==5) || (peDirection==3) || (peDirection==1))	  // Rectangles Diagonaux
     { points[0][0] = zonePied.x1; points[0][1] = zonePied.y1;
	   points[1][0] = zonePied.x2; points[1][1] = zonePied.y2;
	   points[2][0] = zonePied.x3; points[2][1] = zonePied.y3;
       points[3][0] = zonePied.x4; points[3][1] = zonePied.y4;

       // On arrange ces points de tel mani�re � ce que le 1er pt soit en haut et que les suivant suivent le sens des aiguilles d'une montre
       for (int i=0 ; i < 4 ; i++)	// on fait monter le point le plus haut en 0 et descendre le pt le plus bas en 3
       { if (points[i][1] < points[0][1]) { swap(points[i][1],points[0][1]); swap(points[i][0],points[0][0]); }
         if (points[i][1] > points[2][1]) { swap(points[i][1],points[2][1]); swap(points[i][0],points[2][0]); }
       }
       // on range les 2 points du milieu
       if (points[1][0]<points[3][0]) { swap(points[1][0],points[3][0]); swap(points[1][1],points[3][1]); }
     } else	     // Rectangles droits
     { pointX1 = zonePied.x1; pointY1 = zonePied.y1; pointX2 = zonePied.x3; pointY2 = zonePied.y3;
       if (pointX1>pointX2) { swap(pointX1,pointX2); swap(pointY1,pointY2); };
     }

     // -> On les r�cup�re suivant son orientation dans la vue du jeu
     switch (peDirection)
     {  case 7 : // Nord-Ouest
              x1 = points[3][0]; y1 = points[3][1]; x2 = points[0][0]; y2 = points[0][1];  break;
        case 1 :  // Sud-Ouest
              x1 = points[2][0]; y1 = points[2][1]; x2 = points[3][0]; y2 = points[3][1];  break;
        case 3 :  // Sud-Est
              x1 = points[1][0]; y1 = points[1][1]; x2 = points[2][0]; y2 = points[2][1];  break;
        case 5 :  // Nord-Est
              x1 = points[0][0]; y1 = points[0][1]; x2 = points[1][0]; y2 = points[1][1];  break;
        case 4 :  // Est
              x1 = pointX2; y1 = pointY1;  x2 = pointX2; y2 = pointY2;  break;
       	case 6 :  // Nord
       		  x1 = pointX1; y1 = pointY1;  x2 = pointX2; y2 = pointY1;  break;
       	case 2 :  // Sud
       		  x1 = pointX1; y1 = pointY2;  x2 = pointX2; y2 = pointY2;  break;
       	case 0 :  // Ouest
              x1 = pointX1; y1 = pointY1;  x2 = pointX1; y2 = pointY2;  break;
     }
     // Et les coordonn�es du centre des pieds ddu perso
     int gravX = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->GravSol.x,
         gravY = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->GravSol.y;

     x1 += newPeX - gravX;
     y1 += newPeY - gravY;
     x2 += newPeX - gravX;
     y2 += newPeY - gravY;

	 #ifdef PERSOLIB_DEBUG
     // Convertit les coordonn�es du rectangle dans le rep�re de l'�cran
     int pX1 = x1 - ViewMap->ScreenX,
         pY1 = y1 - ViewMap->ScreenY,
         pX2 = x2 - ViewMap->ScreenX,
         pY2 = y2 - ViewMap->ScreenY;

	 //  TRACEDBG("x1 : %d / y1 : %d  / x2 : %d / y2 : %d / Orientation : %d\n", x1, y1, x2, y2, peDirection);
	 DS->pDDSBack->Line(pX1, pY1, pX2, pY2, (0<<11) & (0<<5) & (15) );
	 #endif

     // 4) Pour chacun des cercles, on va d�terminer si un des points de cette droite est � l'int�rieur
     bool pt1, pt2;
     for (uint i=0 ; i < DecorsList.size() ; i++)
     {	// Regarde si l'un des 2 points est inscrit dans le cercle
        int cercleX = DecorsList[i].cercleX;
        int cercleY = DecorsList[i].cercleY;
        int rayon = DecorsList[i].rayon;
     	pt1 = isInCircle(cercleX, cercleY, rayon, x1, y1);
        pt2 = isInCircle(cercleX, cercleY, rayon, x2, y2);
        //TRACEDBG("i : %d  /  pt1 : %b  /pt 2 : %b  / cercleX : %d / cercleY : %d\n", i, pt1, pt2,DecorsList[i].cercleX,DecorsList[i].cercleY);

        // Si l'un des 2 points est � l'int�rieur du cercle, on regarde de combien de pixels on doit d�caler le perso pour qu'il ne soit plus dedans
        if ( (pt1 && !pt2)  || (!pt1 && pt2) )
        {  int beginX, beginY, endX, endY;
           if (pt1) { beginX = x1; beginY = y1; endX = x2; endY = y2; }
           else     { beginX = x2; beginY = y2; endX = x1; endY = y1; }

           int DX, DY;
           int result = nbPixel(cercleX, cercleY, rayon, beginX, beginY, endX, endY, peDirection, DX, DY);

           int DXY = DX+DY;
           if ((DX!=0) && (DY!=0)) DXY = (int) ((double) DXY / RACINEde2);
           // On d�cale le perso et on change ses coordonn�es
           if ((DXY > 0) && (DXY <= MAX_DECAL_PIXELS))
           {  newPeX+=DX;
              newPeY+=DY;
              // -> En partant des coordonn�es exprim�es en pixels, on arrive aux coords du monde
			  ConvNivPeXYtoUV(newPeX, newPeY, newPersoNivU, newPersoNivV);
              ConvNivUV2XY(newPersoNivU, newPersoNivV, newPersoNivX, newPersoNivY);
              newPeU = (int) newPersoNivU;  newPeV = (int) newPersoNivV;

                 //TRACEDBG("result : %d   dX : %d  dY : %d  dXY : %d\n",result, DX, DY, DXY);
           } else
           // Le nb de pixels afin de d�caler le perso est trop important et l'on doit le stopper net
           if (result > MAX_DECAL_PIXELS)
           {  return true;
           }
        } else
        if ( pt1 && pt2 )	// Les 2 points sont dans le cercle -> Vrai collision ind�tournable
        {  return true;
        }
     }
     return false;
}
//----------------------------------------------------------------------------//


void Personnage::SetCoords(const double &X,const double &Y, const double &U, const double &V, int pex, int pey, int peu, int pev)
{	PersoNivX = X;	PersoNivY = Y;
    PersoNivU = U;	PersoNivV = V;
    peX = pex; peY = pey;
    peU = peu; peV = pev;	
}


//=== V�rifie si il y'a des obstacles
bool Personnage::DetectColis_Possibility(double posNivX, double posNivY, int direction)
{	return false;
}

//== D�tecte si un objet du d�cors est pr�sent devant lui � un certains rayon //
bool Personnage::DetectObjet_Rayon(int rayon, CaseOccupee& BlocObjet)
{
    TMap * MapDecors = MapNiveau->PrincEtage->LayerDecors;    // Carte des d�cors sur laquelle le perso se trouve [A MODIFIER]

    // 1) D�termination du nb de cases � explorer d'apr�s le rayon de recherche
    int NbCases = rayon*3;
    CaseOccupee* cases = new CaseOccupee[NbCases];

	// 2) Convertion de la direction "Ecran" vers la direction "Carte"
    eMapDirection peDirectionMap = (eMapDirection) peDirection;

    // 3) A partir de la direction du perso, on recherche si les cases qu'il va falloir scruter
    int DepU, DepV,  // pt de d�part � gauche devant le perso
    	IncU=0, IncV=0,  difU=0, difV=0;
    bool diag = false;
    switch (peDirectionMap)   // Direction � l'�cran
    {	// Deplacement en diagonal sur la carte r�elle du monde
        case Map_SO : difU = 0; difV = 1; IncU =-1;  IncV = 1; diag = true; break;
        case Map_SE : difU = 1; difV = 0; IncU = 1;  IncV = 1; diag = true; break;
        case Map_NE : difU = 0; difV =-1; IncU = 1;  IncV =-1; diag = true; break;
        case Map_NO : difU =-1; difV = 0; IncU =-1;  IncV =- 1; diag = true; break;
        // D�placements horizontaux
        case Map_E : IncU = rayon;  IncV = 3; difU = 1; difV =-1; break;
        case Map_O : IncU =-rayon;  IncV =-3; difU =-1; difV = 1; break;
        // D�placements verticaux
        case Map_S : IncV = rayon;  IncU =-3; difU = 1; difV =  1; break;
        case Map_N : IncV =-rayon;  IncU = 3; difU =-1; difV = -1; break;
    }
	DepU = peU + difU;
	DepV = peV + difV;

    // 4) On d�termine (d'apr�s un petit algo) si les cases qui nous int�ressent sont occup�es ou non
    int cU = DepU;
  	int cV = DepV;
    int ind = 0;
    TCase *bloc;
    DecorsObjet* dec;
    //   - Cas d'une orientation en diagonale
	if (diag)
    {   for (int j=0 ; j < 3 ; j++)
    	{
        	for (int i=0 ; i < rayon ; i++)
    		{
                if ((cU>=0) && (cV>=0) && (cU<MapDecors->MapWidth) && (cV<MapDecors->MapHeight))
                {
                	bloc = &MapDecors->Carte[cU][cV];
                	dec = bloc->ptrObjet; 
                	if (dec!=NULL)
                	{	// On a trouv� un bloc sur lequel se trouve un objet du d�cors
                        cases[ind].occupee = true;
                        cases[ind].posU = cU;
                        cases[ind].posV = cV;
                        cases[ind].decors = dec;
                        cases[ind].decID = dec->NumObjet;
                        // Calcul de la distance
                        float difu = (float) (cU + 0.5 - PersoNivU);
                        float difv = (float) (cV + 0.5 - PersoNivV);
                        cases[ind].dist = difu*difu + difv*difv;
                        //TRACEDBG("Occupe en (%d, %d) par n�%d a une dist de %f\n", cU, cV, dec->NumObjet, cases[ind].dist);
                    }
                }
        		cU += IncU;
            	cV += IncV;
                ind++;
        	}
            if (j==0)
            {	DepU += (difU==0) ?  IncU : 0;
            	DepV += (difV==0) ?  IncV : 0;
            } else  // (j==1)
            {	DepU += (difV==0) ? -IncU : 0;
            	DepV += (difU==0) ? -IncV : 0;
            }
            cU = DepU;
  			cV = DepV;
        }
    }
    else
    //   - Cas d'une orientation horizontale ou verticale
    {   int iU = (IncV >0) ? 1 : -1;
        int iV = (IncU >0) ? 1 : -1;
    	for (int i=0 ; i < abs(IncU) ; i++)
        {  	for (int j=0 ; j < abs(IncV) ; j++)
        	{   if ((cU>=0) && (cV>=0) && (cU<MapDecors->MapWidth) && (cV<MapDecors->MapHeight))
                {
                	bloc = &MapDecors->Carte[cU][cV];
                	dec = bloc->ptrObjet;
                	if (dec!=NULL)
                	{	// On a trouv� un bloc sur lequel se trouve un objet du d�cors
                        cases[ind].occupee = true;
                        cases[ind].posU = cU;
                        cases[ind].posV = cV;
                        cases[ind].decors = dec;
                        cases[ind].decID = dec->NumObjet;
                        // Calcul de la distance
                        float difu = (float) (cU + 0.5 - PersoNivU);
                        float difv = (float) (cV + 0.5 - PersoNivV);
                        cases[ind].dist = difu*difu + difv*difv;
                    }
                }
                cV += iV;
                ind++;
            }
            cU += iU;
        }
    }

    // 5) On recherche le d�cors le plus proche du perso
    int indice = -1;
    float distmin = -1;
    for (int i=0; i < NbCases ; i++)
    {	if (cases[i].occupee)
    	{	if ((distmin==-1) || ( cases[i].dist < distmin))
        	{	indice = i;
            	distmin = (float) cases[i].dist;
            }
        }
    }

    // 6) Si l'on a trouv� un d�cors, on renvoie ses coordonn�es
    if (indice != -1)
    {   BlocObjet = cases[indice];
    	BlocObjet.dist = sqrt(BlocObjet.dist);	//
        return true;
    }

	return false;	// aucun d�cors � proximit�
}


//=== Classe ClisteObjetFrame ================================================//
//== G�re une liste d'objets de couple numObjet-numFrame                    ==//

//=== recherche du num�ro de la frame d'apr�s un type d'objet renvoi ===
//  - 0 si pas trouv� dans la liste
//  - ou numFramAnimlSuperposee.
int ClisteObjetFrame::Search(int numTypObj)
{  for (uint i=0; i < TabObjetFrame.size(); i++)
   {  if (TabObjetFrame[i].numIDTypeObjet == numTypObj)
      { return TabObjetFrame[i].numFrameAnimSuperposee;
      }
   }
   return 0;
};
//----------------------------------------------------------------------------//


