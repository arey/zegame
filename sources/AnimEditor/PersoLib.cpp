////////////////////////////////////////////////////////////////////////////////
//   UNITE : Définitions des Personnages 		"PersoLib.cpp"                //
//                                                                            //
//	 Begin 02/07/98															  //
////////////////////////////////////////////////////////////////////////////////

#include "MapLib.h"
#include "PersoLib.h"
#include "GestFile.h"
//#include "DirectLib.h"
//#include "DDLibAsm.h"
#include "DILib.h"
#include <math.h>
#include "WorldSimulation.h"

//#define PERSOLIB_DEBUG 
eCollideMode COLLIDE_MODE = COLLIDE_ZONE;   // collisions utilisant la zone-perso au sol

//--- Liste de tous les modèles de personnages du jeu ---
Character* ListeCharacters[NbCharacters];   // Liste des personnages du jeu

// Constructeur --------------------------------------------------------------//
Character::Character(int characType):
    CharacterType(characType) // Indique le type de modèle qu'il représente :  HERO, OGRE ...
{   // Librairies d'animations ou de transitions non encore chargées
    AnimSpriteLib = NULL;
    AnimTransLib = NULL;
    peNbAnim = peNbTrans = 0;    	    // aucune animation ou transitions

    // défini la librairie de sprites à utiliser d'aprés son type de character
	// <=> STLPersonnage = STLModelePersos[CharacterType]
    switch (CharacterType)
    {   case (HERO1) : STLPersonnage = STLHero; break;
        case (OGRE1) : STLPersonnage = STLOgre; break;
		case (NAINRENEGAT1) : STLPersonnage = STLModelePersos[NAINRENEGAT1]; break;
        default :
             // à exclure (sauf lors de la creation de nouveau model de perso)
	         STLPersonnage = NULL;
    }
    // S'occupe de la gestion des sprites superposables
    nbMembres = cNbMembresDuCorpsHumains;          // par défaut, on prends celui du corps humains
    ObjetFrame = new ClisteObjetFrame[nbMembres];  // Tableau de nbMembres d'un tableau d'ObjetFrame

    // Initialise la Librairie des frames superposées du personnage
    FrameSuperpList = new SpriteFrameList();
    // Initialise la Librairie des sprites à superposer sur le perso
    STLFrameSuperp = new SpriteTileLib();
}
//----------------------------------------------------------------------------//

// Charge les informations sur Transitions & Animations ----------------------//
void Character::LoadIna(const char *InaName,SpriteTilePersoLib **STLPerso)
{   //  appelle la fonction LoadFileIna de "Animall.cpp"
    // Attention : Répertoire NON complet    modifié le 5/12/98
    AnsiString InaNameTresShort = InaName;
    LoadFileIna(InaNameTresShort.c_str(),&AnimSpriteLib,&AnimTransLib,&peNbAnim,&peNbTrans,STLPerso,this);
};
//----------------------------------------------------------------------------//

//=== Sauvegarde toutes les informations sur les sprites d'un personnage =====//
void Character::SaveToFileInfoSprites(FILE &fichier)
{  // Remplissage de l'en-tête du bloc du fichier
   // Version du fichier
//   fichier << CGeneralSpriteInfos::Version;
 //   fwrite(&CGeneralSpriteInfos::Version,sizeof(CGeneralSpriteInfos::Version), 1, &fichier);
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
{  // Remplissage de l'en-tête du bloc du fichier
   // Version du fichier
/*    float Version;
    fread(&Version,sizeof(Version), 1, &fichier);*/
    // ATTENTION : Avant le 05/03/99 je mettais directement cette valeur dans STLPersonnage->NbSprites.
    //             Or, lorsque l'on rajoute des frames au fichier *.ina -> on rajoute des sprites;
    int NbMdlSprites = 0;
    fread(&NbMdlSprites,sizeof(NbMdlSprites), 1, &fichier);

    SpriteTilePerso *Sprite;
    // Parcours toute la liste des sprites du perso
    for (int num = 0; num < NbMdlSprites ; num++)
      {  Sprite = (SpriteTilePerso*) STLPersonnage->SprIndex[num];      // se place sur le bon sprite
         Sprite->GSprInf->LoadFromFile(fichier);
      }
}
//----------------------------------------------------------------------------//

//=== Save a MDL file ========================================================//
bool Character::SaveMdl(const char* MdlName)
{   FILE *f;
	if ((f = fopen(MdlName,"wb"))==NULL)
    {	MessageBoxA(NULL,"Impossible de créer le fichier : ",MdlName,1);
    	return false;
    }

    // Sauve les infos de bases
    // a) N° de version du mdl
    fwrite(&CGeneralSpriteInfos::Version,sizeof(CGeneralSpriteInfos::Version), 1, f);
    // b) Informations des anims
    SaveToFileInfoSprites(*f);

    fclose(f);	// Et referme le fichier
}
//----------------------------------------------------------------------------//

//=== Load fichier MDL =======================================================//
// Charge tout le contenu du fichier .mdl contenant toutes les informations
// de chaque tiles des anims du persos ...
void Character::LoadMdl(const char* MdlName)
{
   // On charge les données que l'on a peut-être
   FILE *f;
   // Ouvre le fichier binaire en lecture
   if ((f = fopen(MdlName,"rb"))==NULL)
   { // Si l'ouverture du fichier à échouée -> on suppose que le fichier n'a pas encore été créé
     MessageBoxA(NULL,"Impossible de lire le fichier : Création d'un nouveau fichier vide",MdlName,1);
     SaveMdl(MdlName);
   } else
   { // On charge le fichier
     float version;
     fread(&version,sizeof(version), 1, f);
     if (version < CGeneralSpriteInfos::Version)
     {  AnsiString v = "Ancienne version : ";  v += version;
         v += "/ version courante : "; v += CGeneralSpriteInfos::Version;
        MessageBoxA(NULL,"Version de la structure sAnimInfos trop ancienne",v.c_str(),1);
        return;
     }
	 LoadFromFileInfoSprites(*f);
     fclose(f);
  }
}
//----------------------------------------------------------------------------//
//=== Charge les bmp relié à cet objet =======================================//
void Character::AllocateObjetBmp(int NumObjet)
{  // Recherche pour chaque membres du personnage dans la liste des bmp si l'image
   // est chargé ou non et la charge dans ce cas
   // Pour chaque membre du perso
   for (int m=0; m<nbMembres; m++)
   { // Pour chaque frame correspondant à l'un de ces membres
     for (int i=0; i < ObjetFrame[m].TabObjetFrame.size() ; i++)
     { // Si cette frame correspond à l'objet
       if (ObjetFrame[m].TabObjetFrame[i].numIDTypeObjet == NumObjet)
       { int numFrame = ObjetFrame[m].TabObjetFrame[i].numFrameAnimSuperposee;
         // On cherche la bonne frame
         SpriteFrame* frame = FrameSuperpList->SF_List;
         if ((frame!=NULL) && (frame->IDFrame!=numFrame)) frame = frame->SF_Suiv;
         #ifdef PERSOLIB_DEBUG
         if (frame==NULL)
         {  MessageBoxA(NULL,"Frame non trouvée !","Personnage::AllocateObjetBmp",1);
            Application->Terminate();
         }   
         #endif
         // Regarde si la frame n'est pas déjà chargée
         if (!frame->BmpAlwaysIntoMemory)
         { if ((!frame->BmpLoadIntoMemory) && (frame->Semaphore==0))
           {  // On doit charger la frame en mémoire à partir du bmp sur DD
              frame->ReserveMemoryAndLoadBmp(&DS->pDD);
           }
           frame->Semaphore++;     // Un autre user utilise cette frame
         }
       }
     }
   }
}
//----------------------------------------------------------------------------//

//=== Désalloue la mémoire occupée par les bmp représentant cet objet ========//
void Character::DesallocateObjetBmp(int NumObjet)
{  // Recherche pour chaque membres du personnage dans la liste des bmp si l'image
   // est chargé ou non et la désalloue s'il il faut
   // Pour chaque membre du perso
   for (int m=0; m<nbMembres; m++)
   { // Pour chaque frame correspondant à l'un de ces membres
     for (int i=0; i < ObjetFrame[m].TabObjetFrame.size() ; i++)
     { // Si cette frame correspond à l'objet
       if (ObjetFrame[m].TabObjetFrame[i].numIDTypeObjet == NumObjet)
       { int numFrame = ObjetFrame[m].TabObjetFrame[i].numFrameAnimSuperposee;
         // On cherche la bonne frame
         SpriteFrame* frame = FrameSuperpList->SF_List;
         if ((frame!=NULL) && (frame->IDFrame!=numFrame)) frame = frame->SF_Suiv;
         #ifdef PERSOLIB_DEBUG
         if (frame==NULL)
         {  MessageBoxA(NULL,"Frame non trouvée !","Personnage::DesallocateObjetBmp",1);
            Application->Terminate();
         }   
         #endif   
         // Regarde si la frame n'est pas déjà chargée
         if (!frame->BmpAlwaysIntoMemory)
         { if ((frame->BmpLoadIntoMemory) && (frame->Semaphore==1)) // Si c'est le seul à utiliser cette frame  
           {  // On doit charger la frame en mémoire à partir du bmp sur DD
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


////////////////////////////////////////////////////////////////////////////////
// Fonctions Objets : PERSONNAGE											  //
////////////////////////////////////////////////////////////////////////////////

// Constructeur --------------------------------------------------------------//
Personnage::Personnage(int PersoType):
    ModelePersoType(PersoType)     // Type de personnage : HERO, OGRE, NAIN
{
    peEtat = peFutEtat = peOldEtat = ARRET1;
    peCollideEtat = NOSTATE;
    ArmeEnMain = NOWEAPON;
    peSpeed = peFutSpeed = peOldSpeed = pedVit = 0;
    peAccelerate = true;
    peSpeedTrans = false;
    peAllCollisions = true;     // Toutes les collisions sont activées
    peNumSprite = 0;
    peDirection = peFutDirection = or_S;
    PersoNivX = 5;
    PersoNivY = 4.5;
    PersoNivXY2UV();
    PersoNivXYtoPeXY();
//    peX = 100; peY = 300;
    //--- Attribue un modèle/squelette à ce nouveau perso ---
    ModelePerso = ListeCharacters[ModelePersoType];
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
    // ** Nouvelles positions demandées
    if (Key_K)
     { peFutEtat = MEURT1; peFutDirection = peDirection; }
    else
    if (Key_SPACE)	// Se prend un coup ds la tetê
     { peFutEtat = PRENDCOUP1; peFutDirection= peDirection;  }
    else
    if (Key_LCTRL)	// Donne un coup
    {  switch (ArmeEnMain)
       { case NOWEAPON : peFutEtat = DONNECOUP1; break;
         case EPEE1    : peFutEtat = COUPEPEE1; break;
       }
       peFutDirection = peDirection;
    }
    else
    if (Key_LALT == EVENTKEY_APPUYE)  // Change d'arme
    {  ArmeEnMain++;
       if (ArmeEnMain >= NbWeapons) ArmeEnMain = 0; 
    }
    else
        //  Aucune touche de direction appuyée, ni entrain de tourner
        //  => on s'arrête
    if ( (!Key_UP) && (!Key_DOWN) && (!Key_RIGHT) && (!Key_LEFT)
     	 && (!Key_npE) && (!Key_npSE) && (!Key_npS) && (!Key_npSO)
         && (!Key_npO) && (!Key_npNO) && (!Key_npN) && (!Key_npNE)
         && ( (peFutEtat!=TOURNER1) || (peFutEtat!=TOURNER2) || (peFutEtat!=DEMITOUR1))  )
      { peFutEtat = ARRET1; peFutDirection= peDirection; }
    else
	// PRIORITE aux touches fléchées
        // Si une des touches de direction est appuyée :
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

        //  Si le perso MARCHE ou le perso est à l'ARRET et ds même DIRECTION
        //  et la touche Shift n'est pas appuyée => le perso va MARCHER
        if ( (!Key_LSHIFT) &&
              ((peEtat==MARCHE1) || ((peEtat==ARRET1) && (peFutDirection==peDirection))) )
   	     					peFutEtat = MARCHE1;
        else
        //    Si le perso MARCHE et la touche shift appuyée
        // ou Si le perso COURT et la touche shift est appuyée
        // ou Si le perso est à l'ARRET et la touche shif est appuyée        
        // => le perso va COURIR
        if ( (Key_LSHIFT) &&
              (     (peEtat==MARCHE1)
                 || ((peEtat==ARRET1) && (peFutDirection==peDirection))
                 || (peEtat==COURSE1)
              )
            ) peFutEtat = COURSE1;
        else  // si il n'est pas en train d'attendre, il demande à s'ARRETER
        if (peFutEtat!=STAND1) peFutEtat= ARRET1; 
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
        //	Si le perso MARCHE ou le perso est à l'ARRET et ds même DIRECTION
        //	=> le perso va MARCHER
/*        if ((peEtat==MARCHE1)
             || ((peEtat==ARRET1) && (peFutDirection==peDirection)) )
   							peFutEtat = MARCHE1;
        else peFutEtat= ARRET1;  // sinon il demande à s'ARRETER*/
     };
          //=== Mouvements automatiques ====
     // Si le perso est à l'état ARRET1, on peut le faire attendre
     if ((peEtat==ARRET1) && (peFutEtat==ARRET1))
     {  if (random(100) == 0) peFutEtat=STAND1;
     }

}
//----------------------------------------------------------------------------//

//=== Met à jour le perso, qu'il soit joueur ou non joueur ===================//
void Personnage::Update()
{ // Sert pour l'instant pour les PNJ
}
//----------------------------------------------------------------------------//


// Transitions FUTUR ETAT -> ETAT PRESENT ------------------------------------//
// à partir des Directions Futures & Présentes du perso, regarde les transitions
// possibles données par le *.ina et les effectuent si possible
void Personnage::Fut2PresEtat(void)
{   // Regarde si l'on peut Faire le changement de direction && d'Etat
    if ((peFutDirection!=peDirection) // on doit Tourner ...
         && ((peFutEtat!=TOURNER1) || (peFutEtat!=TOURNER2)) )  // si c'est pas déjà fait !
       {    int Compar = peFutDirection - peDirection;
       		// Peut-être un demi-tour
            if ((abs(Compar)==4) && (peEtat!=ARRET1)) peFutEtat=DEMITOUR1;
       		// Choix du sens 1 ou 2
            else
            if (Compar>0)
               { if (Compar<=3) peFutEtat=TOURNER1;   // sens Positif
               	 else peFutEtat=TOURNER2;             // sens Négatif
               }
            else
               { if (Compar>=-3) peFutEtat=TOURNER2;
                 else peFutEtat=TOURNER1;
               }
       };

    BOOL sort = FALSE;

   // Si l'état voulu n'est plus le même que précédemment
  if (peFutEtat!=peEtat) // PAS FORCEMMENT  => ex: MEURT1-MORT1 à l'AIR DE MARCHER
       // scrute toutes les transitions possibles
       for ( int i=0 ; (i < ModelePerso->peNbTrans) && (sort==FALSE); i++)
         {  // CAS PARTICULIERS du perso entrain de TOURNER ou DEMITOUR }
  	    if (    ((peFutEtat==TOURNER1) || (peFutEtat==TOURNER2) || (peFutEtat==DEMITOUR1) )
                 && (ModelePerso->AnimTransLib[i]->NumActFut==peFutEtat)     // si les actions présentes et futures
                 && (ModelePerso->AnimTransLib[i]->NumActCour==peEtat)       // correspondent bien à ce que l'on souhaite
                 && (   (ModelePerso->AnimTransLib[i]->ImageA==peNumSprite)  // et si l'on est à la bonne image
                     || (ModelePerso->AnimTransLib[i]->ImageA== -1) )        // ou cas où ImageA = "n"
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
                        if (peDirection!=peFutDirection)  // si la directon a encore changé
                        { peOldEtat = peEtat;                       // conserve l'ancien état
                          peEtat = ModelePerso->AnimTransLib[i]->NumActInterm;   // état voulu
                          peCollideEtat = NOSTATE;
                          if (ModelePerso->AnimTransLib[i]->ImageB!=-1)
                              peNumSprite = ModelePerso->AnimTransLib[i]->ImageB; //  sprite voulu
                          peDirection = peFutDirection;  	     //  direction voulue
                        };
                    	break;
                 }
                sort = TRUE;
                //  CAS où le perso va MAINTENANT TOUT DROIT
 /*               if (peDirection==peFutDirection)
                 {
                 	peEtat = AnimTransLib[i]->NumActInterm;		// état voulu
	                if (AnimTransLib[i]->ImageB!=-1)
                      peNumSprite = AnimTransLib[i]->ImageB;    // sprite voulu
                 } */
            };

            // TOUT DROIT
         	if ( (ModelePerso->AnimTransLib[i]->NumActFut==peFutEtat)
                && ( ModelePerso->AnimTransLib[i]->NumActCour==peEtat)
                && (    (ModelePerso->AnimTransLib[i]->ImageA==peNumSprite)
                     || (ModelePerso->AnimTransLib[i]->ImageA== -1)   // cas où ImageA = "n"
                   ) )
              {
                   // Si il vient d'y avoir collision
                   // et que le joueur essaye de refaire la même action ayant provoquée la collision
                   if ((peCollideEtat!=NOSTATE) &&(peCollideEtat==peFutEtat))
                   {  // On ne fait rien !

                   }
                   else  // Sinon, on le fait avancer
                   {  peOldEtat = peEtat;                       // conserve l'ancien état
              	      peEtat = ModelePerso->AnimTransLib[i]->NumActInterm;
                      peNumSprite = ModelePerso->AnimTransLib[i]->ImageB;
                      sort = TRUE;

                      if ((peEtat!=ARRET1) && (peEtat!=STOPNET1))
                      { peCollideEtat = NOSTATE;
                      }
                   }
                
                /*if ((peEtat!=ARRET1) && (peEtat!=STOPNET1) && (peCollideEtat!=NOSTATE))
                  {  peCollideEtat = NOSTATE;
                  }
                else*/
      /*          {  peOldEtat = peEtat;                       // conserve l'ancien état
              	   peEtat = AnimTransLib[i]->NumActInterm;

                   peNumSprite = AnimTransLib[i]->ImageB;
                   sort = TRUE;
                }*/
              }
            else
            // CHGT AUTONOME DE TRANSITION NumActFut==NumActCour   (ex: MEURT1->MORT1)
            if (    (ModelePerso->AnimTransLib[i]->NumActCour==peEtat)
            	 &&	(ModelePerso->AnimTransLib[i]->NumActFut==ModelePerso->AnimTransLib[i]->NumActCour)
            	 &&  (    (ModelePerso->AnimTransLib[i]->ImageA==peNumSprite)
                      || (ModelePerso->AnimTransLib[i]->ImageA==-1) ))
               { peOldEtat = peEtat;                       // conserve l'ancien état
                 peEtat = ModelePerso->AnimTransLib[i]->NumActInterm;
                 peNumSprite = ModelePerso->AnimTransLib[i]->ImageB;
                 sort = TRUE;
               };
        }
};
//----------------------------------------------------------------------------//


// BOUGE LE PERSO ------------------------------------------------------------//
void Personnage::peBouge()
{
    // Transfert des informations de l'action courante vers le perso
    ResultAction();

    // Calcul le déplacement et affecte les nouvelles coordonnées au perso
    //DeplacementRealist();
 //   Deplacement();

 //** New : on dessine le perso à part, comme çà on peut calculer ses coordonnées qd on veut
 // Puis dessine le perso avec le scrolling
 //   peFastDraw(DS->pDDSBack);

    // Si l'on arrive à la fin de l'animation et rien n'a changé,
    //   => on revient à son début
    if (++peNumSprite>=peNbSprite) peNumSprite=0;
};
//----------------------------------------------------------------------------//

//== Affiche sur <pScreen> le perso ==========================================//
void Personnage::peFastDraw(DDSurface *pScreen)
{
    if (DS->HardwareClipping)
       // GravX(peSprNumLib,STLPersonnage)
      SpriteDraw(peX-ViewMap->ScreenX-GravSolX(),peY-ViewMap->ScreenY-GravSolY(),peSprNumLib,pScreen,ModelePerso->STLPersonnage);
    else
      { //peSprNumLib = 0;
        /*if (SHADOW_PERSO) SpriteDrawShadowClip(peX-ViewMap->ScreenX-GravSolX(),peY- ViewMap->ScreenY-GravSolY(),peSprNumLib,pScreen,ModelePerso->STLPersonnage);
        else*/ SpriteDrawFast(peX-ViewMap->ScreenX-GravSolX(),peY- ViewMap->ScreenY-GravSolY(),peSprNumLib,pScreen,ModelePerso->STLPersonnage);

        //==== On regarde si il n'y a pas un sprite superposable à afficher ===
        // Récupère le numéro du type d'objet que le perso a en main
        int numIDTypeObjSprite = ArmeEnMain;

        SpriteTilePerso *SpritePers = (SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib];
        // Regarde dans le PersoSprite courant si ce sprite superposable n'a pas déjà été supperposé par ailleurs
        if (SpritePers->GSprInf->ListAnimSuperp.Existe(numIDTypeObjSprite))
        {  // Récupère le bon numéro de sprite à afficher
           int num = SpritePers->GSprInf->ListAnimSuperp.GetIDS(numIDTypeObjSprite);
           SpriteTile *SpriteSuperp = ModelePerso->STLFrameSuperp->SprIndex[num];
           int posX, posY;
           SpritePers->GSprInf->ListAnimSuperp.GetCoord(numIDTypeObjSprite, posX, posY);
           int numIDSprSuperp = SpritePers->GSprInf->ListAnimSuperp.GetIDS(numIDTypeObjSprite);
           int pX = peX-ViewMap->ScreenX-GravSolX() + SpritePers->GravX + posX - SpriteSuperp->GravX;
           int pY = peY- ViewMap->ScreenY-GravSolY() + SpritePers->GravY + posY - SpriteSuperp->GravY;
           //SpriteDrawShadowClip(pX,pY,numIDSprSuperp,pScreen,ModelePerso->STLFrameSuperp);
        }
      }
};
//----------------------------------------------------------------------------//

// N° SPRITE DS LIBRAIRIE ----------------------------------------------------//
void Personnage::ResultAction(void)
{
    // Trouve le n° de l' ACTION COURANTE
    int anm = -1,  i = 0;
    while ( (i < ModelePerso->peNbAnim) && (anm==-1) )
      {  if (ModelePerso->AnimSpriteLib[i]->Numero==peEtat) anm = i;
      	 i++;  };

    // renvoie les informations de l'ACTION
    peNbSprite=ModelePerso->AnimSpriteLib[anm]->NbSprites;
//********* Y4AVAIS UN BUG ICI jusqu'au 7/04/99 ********* //
    // Si l'animation a changée, il se peut que l'on doit revenir au départ
    if (peNumSprite>=peNbSprite) peNumSprite=0;

    // renvoie le n° du sprite correspondant
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

    }
};
//----------------------------------------------------------------------------//

// DEPLACEMENT ---------------------------------------------------------------//
//   => calcul des nouvelles coordonnées du perso à partir de divers facteurs
// pour l'instant BIDON : au FEELING
void Personnage::Deplacement(void)
{   // **** ATTENTION : vieille procédure à 2 sous *****
    int peXOld = peX, peYOld = peY; // sauvegarde des coordonées du perso avant déplacement

    int avX = 4,avY = 4, avXY =3; // Données bidons
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

    // Calcule ses coordonées sur la Map
    //  => +30 pour avoir les coordonnées des pieds
    ViewMap->TransXYtoUV(peX/*-GravSolX()*/ , peY/*-GravSolY()*/, &peU, &peV);

    // Regarde si il ne marche pas sur un MUR
    if ((peU>=0) && (peV>=0) && (peU<MapNiveau->NivWidth) && (peV<MapNiveau->NivHeight))
    if (!MapNiveau->PrincEtage->LayerDecors->Carte[peU][peV].isAccessible())
      { peX = peXOld; peY = peYOld; };

    // fait suivre la caméra pour que le perso soit tjs au milieu
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


//=== Va chercher les coordonnées du point représentant le centre de gravité ===
//    du personnage projeté au sol
int Personnage::GravSolX()
{   return (((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->GravSol.x);
}
int Personnage::GravSolY()
{   return (((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->GravSol.y);
}
//----------------------------------------------------------------------------//

//== Outils de convertion des divers coordonnées du perso ====================//
// Convertion des PersoNiv(X,Y) en PersoNiv(U,V)
void Personnage::PersoNivXY2UV()
{    PersoNivU = PersoNivX / WorldModel.SQR_LENGTH;
     PersoNivV = PersoNivY / WorldModel.SQR_LENGTH;
}
//=== Convertion des PersoNiv(U,V) en PersoNiv(X,Y) ==========================//
void Personnage::PersoNivUV2XV()
{     PersoNivX = PersoNivU * WorldModel.SQR_LENGTH;
      PersoNivY = PersoNivV * WorldModel.SQR_LENGTH;
}
//----------------------------------------------------------------------------//

//=== Convertion des PersoNiv(X,Y) en pe(X,Y)=================================//
void Personnage::PersoNivXYtoPeXY()
{    PersoNivXY2UV();
     peX = ( PersoNivU - PersoNivV ) * ISO_WIDTH_HALF;
     peY = ( PersoNivU + PersoNivV ) * ISO_HEIGHT_HALF;
}
//----------------------------------------------------------------------------//

//=== Effectue les réglages de transitions entre 2 anims =====================//
void Personnage::ChangementAnim(void)
{   // Vérifie si le perso ne vient pas tout juste de changer d'anim
    if (peEtat!=peOldEtat)
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
       {  // Transition immédiate
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
       peOldEtat = peEtat;    // Changement effectué
    }
}
//----------------------------------------------------------------------------//

//=== Deplacement "réaliste" du personnage ===================================//
//=> tiens compte des coordonnées du niveau
void Personnage::DeplacementRealist(void)
{   double persoNivXOld = PersoNivX, // sauvegarde des coordonées du perso avant déplacement
           persoNivYOld = PersoNivY;

    // 0) Vérifie si le perso ne vient pas tout juste de changer d'anim
    ChangementAnim();

    // 1) Calcul des petits déplacements du personnage d'aprés :
    //  - le type de personnage : Homme, loup, tortue ...
    //  - le nombre de frame composant une anim de marche : 12, 16 ...
    //  - l'état du personnage : fatigué, blessé, dopé ...
    //  - le type de marche : discrète, course, lent, rapide, pressé ...

    // a) longeur réelle à parcourir entre 2 frames d'anim
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

    // b) déplacements horizontaux, verticaux & diagonaux
    double av_HV   = longueur;
    double av_Diag = longueur / sqrt(2);

    // c) Modification des coordonnées du perso sur la Map du niveau
    if (   (peEtat == MARCHE1) || (peEtat == COURSE1)
        || ((peEtat == ARRET1) && (peSpeedTrans))
        || (peEtat == DEMITOUR1) )
    { // Convertion Direction Ecran -> Direction Carte
      eMapDirection peDirectionMap = (eMapDirection) peDirection;
      switch (peDirectionMap)   // Direction à l'écran
        {   // Deplacement en diagonal sur la Carte
            case Map_SO : PersoNivX-=av_Diag; PersoNivY+=av_Diag; break;
            case Map_SE : PersoNivX+=av_Diag; PersoNivY+=av_Diag; break;
            case Map_NE : PersoNivX+=av_Diag; PersoNivY-=av_Diag; break;
            case Map_NO : PersoNivX-=av_Diag; PersoNivY-=av_Diag; break;
            // Déplacements horizontaux
            case Map_E : PersoNivX+=av_HV; break;
            case Map_O : PersoNivX-=av_HV; break;
            // Déplacements verticaux
            case Map_S : PersoNivY+=av_HV; break;
            case Map_N : PersoNivY-=av_HV; break;
        };
    }

    // Regarde si les coordonnées de ses pieds n'est pas sur un mur
    PersoNivXY2UV();
  /*  if (COLLIDE_MODE==COLLIDE_PIXEL)
    { if ( (PersoNivU>=0) && (PersoNivV>=0)
           && (PersoNivU<MapNiveau->NivWidth) && (PersoNivV<MapNiveau->NivHeight))
          if (!MapNiveau->PrincEtage->LayerDecors->Carte[(int) PersoNivU][(int) PersoNivV].Accessible)
          {  PersoNivX = persoNivXOld;
             PersoNivY = persoNivYOld;
             PersoNivXY2UV();
          //   peEtat = ARRET1;
          }
    }*/

    // Calcul des coordonnées dans la vue 3D ISO
    PersoNivXYtoPeXY();
    // Calcule ses coordonnées sur la Map formees de trapezes
    ViewMap->TransXYtoUV(peX, peY , &peU, &peV);



    // fait suivre la caméra pour que le perso soit tjs au milieu
    // pour le moment
/*    ViewMap->OldScreenX = ViewMap->ScreenX;	// sauvegarde les anciennes valeures pour le scrolling du sol
    ViewMap->OldScreenY = ViewMap->ScreenY;
    ViewMap->OldScreenU = ViewMap->ScreenU;
    ViewMap->OldScreenU = ViewMap->ScreenV;*/

    if (ModelePersoType==HERO1)
    { ViewMap->ScreenX = peX - ViewMap->ScreenViewX/2;
      ViewMap->ScreenY = peY - ViewMap->ScreenViewY/2;
      ViewMap->PosCoinUV();
    }

    //*** Si c'est le personnage joueur, on teste les collisions ***
    if (peAllCollisions)
    {
      //=== Autre test de collisions ===
      if (COLLIDE_MODE==COLLIDE_ZONE)
      { // 0) Au départ, on suppose qu'il n'y a pas de collisions
        bool Collision = false;

        // 1) Regarde tout d'abord si les pieds du persos ne sont pas sur un décors infrancissable
        // => Si les pieds du perso sont sur la carte
        if (    (PersoNivU>=0) && (PersoNivV>=0)
             && (PersoNivU<MapNiveau->NivWidth) && (PersoNivV<MapNiveau->NivHeight))
        { // => On regarde si le centre des pieds du perso se trouve sur la carte
          if (!MapNiveau->PrincEtage->LayerDecors->Carte[(int) PersoNivU][(int) PersoNivV].isAccessible())
          {  Collision = true;
          }
        }

        // 2) Si l'on n'a pas de collisions a ce niveau là, on pousse le test
        if (!Collision)
        {  Collision = DetectColis_Decors();
           //if (Collision) Totale = true;       // Rmq : on peut enlever le "if (collision)"
        }

        // 3) Si l'on a rencontré des collisions dans une de ces 2 phases, on doit revenir
        //    à la position précédente du perso et l'arrêter NET
        if (Collision)
        {  // On reprends ses anciennes coordonnées
           double dNivX = PersoNivX - persoNivXOld;
           double dNivY = PersoNivY - persoNivYOld;
           PersoNivX = persoNivXOld;
           PersoNivY = persoNivYOld;
         //  if (Totale)
           { PersoNivX-=dNivX;
             PersoNivY-=dNivY;
           }

           // On change les ETATS du perso
           peCollideEtat = peEtat; // Conserve l'état dans lequel on a détecté une collision totale
           peFutEtat = STOPNET1;   // On stop net le perso
           peOldSpeed = peSpeed;   // On sauvegarde l'ancienne vitesse actuelle du perso
           peSpeed   = 0;
           peFutSpeed =0;
           peSpeedTrans = false;
           // Change la frame d'animation à afficher
           Fut2PresEtat();
           ResultAction();

           PersoNivXY2UV();
           // Calcul des coordonnées dans la vue 3D ISO
           PersoNivXYtoPeXY();
           // Calcule ses coordonnées sur la Map formees de trapezes
           ViewMap->TransXYtoUV(peX, peY , &peU, &peV);
           if (ModelePersoType==HERO1)
           { ViewMap->ScreenX = peX - ViewMap->ScreenViewX/2;
             ViewMap->ScreenY = peY - ViewMap->ScreenViewY/2;
             ViewMap->PosCoinUV();
           }
        }
      }
      else
      {
         #ifdef PERSOLIB_DEBUG
         DS->pDDSBack->Lock2();
         if (CharacterType==HERO1) DS->pDDSBack->PutPixel(peX-ViewMap->ScreenX,peY-ViewMap->ScreenY,0xF01);
         DS->pDDSBack->UnLock();
         #endif
      }
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
    pu = floor( ( rx + ry ) / ISO_DENOMINATEUR );
    pv = floor( ( ry - rx ) / ISO_DENOMINATEUR );
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


   if (difX>ISO_WIDTH_HALF) Application->Terminate();
   if (difX<-ISO_WIDTH_HALF) Application->Terminate();
   if (difY<0) Application->Terminate();
   if (difY>ISO_HEIGHT) Application->Terminate();

    if (strictGauche)
    { if (difX >=0) horiz = DROITE;   // point à DROITE du losange
      else          horiz = GAUCHE;   // point à GAUCHE du losange
    } else
    { if (difX >0)  horiz = DROITE;   // point à DROITE du losange
      else          horiz = GAUCHE;   // point à GAUCHE du losange
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

//--- Structure représentant les coordonnées d'un point (U,V) sur la map ---
struct tPoint
{ int u, v;
  void SetPoint(int i_u, int i_v) { u = i_u; v = i_v; };
  bool Equal(int &u2,int &v2) { return ((u==u2) && (v==v2)); };
};
//--- Tableau  de points à 2 colonnes et N lignes ---
const unsigned int MAX_LIGNES  = 50,
                   NB_COLONNES = 2;
tPoint Tab [MAX_LIGNES][NB_COLONNES];


//=== Collisions du perso avec le décors =====================================//
bool Personnage::DetectColis_Decors()
{  // Récupère les coordonnées du rectangle de la zone au sol du perso
   int pt_x1 = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->ZoneSol.x1,
       pt_y1 = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->ZoneSol.y1,
       pt_x2 = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->ZoneSol.x2,
       pt_y2 = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->ZoneSol.y2;
   // Et les coordonnées du centre des pieds ddu perso
   int gravX = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->GravSol.x,
       gravY = ((SpriteTilePerso*) ModelePerso->STLPersonnage->SprIndex[peSprNumLib])->GSprInf->GravSol.y;
   // Convertit les coordonnées du rectangle dans le repère de l'écran
   int pX1 = peX - (gravX - pt_x1),
       pY1 = peY - (gravY - pt_y1),
       pX2 = peX - (gravX - pt_x2),
       pY2 = peY - (gravY - pt_y2);

   //*** Debug ***
   #ifdef PERSOLIB_DEBUG
   DS->pDDSBack->Lock2();
   if (CharacterType==HERO1)
   { DS->pDDSBack->PutPixel(pX1-ViewMap->ScreenX,pY1-ViewMap->ScreenY,0xF01);
     DS->pDDSBack->PutPixel(pX2-ViewMap->ScreenX,pY1-ViewMap->ScreenY,0xF01);
     DS->pDDSBack->PutPixel(pX1-ViewMap->ScreenX,pY2-ViewMap->ScreenY,0xF01);
     DS->pDDSBack->PutPixel(pX2-ViewMap->ScreenX,pY2-ViewMap->ScreenY,0xF01);
   }
   DS->pDDSBack->UnLock();
   #endif

   // Convertit ces coordonnées de type (X,Y) en (U,V) et récupère le coin
   // du losange dans lequel ils se trouvent
   int U1, V1, U2, V2, U3, V3, U4, V4;
   eVertical   vert1,  vert2,  vert3,  vert4;
   eHorizontal horiz1, horiz2, horiz3, horiz4;

   // Point en HAUT à GAUCHE (U1,V1)
   ConvertXYCtoUVC(pX1, pY1, U1, V1, vert1, horiz1, false, false);
   // Point en HAUT à DROITE (U2,V2)
   ConvertXYCtoUVC(pX2, pY1, U2, V2, vert2, horiz2, true, false);
   // Point en BAS à GAUCHE (U3,V3)
   ConvertXYCtoUVC(pX1, pY2, U3, V3, vert3, horiz3, false, true);
   // Point en BAS à DROITE (U4,V4)
   ConvertXYCtoUVC(pX2, pY2, U4, V4, vert4, horiz4, true, true);

   //=== Début de l'algo ===
   // Index du tableau
   int indexG=0, indexD=0;
   const int cGAUCHE = 0,
             cDROITE = 1;

   //--- I] Remplissage des 1ères lignes du tableau ---
   // 0) Si les 2 points du haut (U1,V1) et (U2,V2) sont sur la même CASE
   if ((U1==U2) && (V1==V2))
   {  Tab[indexG][cGAUCHE].SetPoint(U1,V1);
      Tab[indexD][cDROITE].SetPoint(U2,V2);
      indexG = indexD = 0;
   }
   else
   { // 1) Si les 2 points du haut (U1,V1) et (U2,V2) sont sur la même LIGNE
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
           indexG = indexD = 0;   // index sur la 1ère ligne du tableau
        }
     }
     // 2) Sinon un des 2 Points est plus haut que l'autre
     else
     {  // 2.a)  Si c'est le point en HAUT à DROITE qui est le plus haut
        if ((U1+V1)>(U2+V2))
        {  Tab[indexG][cGAUCHE].SetPoint(U1,V1-1);
           Tab[indexD][cDROITE].SetPoint(U2,V2);
           Tab[indexG+1][cGAUCHE].SetPoint(U1,V1);
           indexG = 1; indexD = 0;
        }
        else  // 2.b) Sinon c'est le point en HAUT à GAUCHE qui est le plus haut
        {  Tab[indexG][cGAUCHE].SetPoint(U1,V1);
           Tab[indexD][cDROITE].SetPoint(U2-1,V2);
           Tab[indexD+1][cDROITE].SetPoint(U2,V2);
           indexG = 0; indexD = 1;
        }
     }
   }

   //--- II] Remplissage des 2 colonnes du tableau ---
   for (int col = 0; col < NB_COLONNES; col++)  // Pour chacune des 2 colonnes faire
   {  // 1) Détermine l'index à utiliser dépendant des colonnes et le point d'arrêt
      int index, termU, termV;
      eHorizontal  horiz;
      switch (col)
      {  case cGAUCHE : index = indexG; termU = U3; termV = V3; horiz = horiz1; break;
         case cDROITE : index = indexD; termU = U4; termV = V4; horiz = horiz2; break;
      }
      // 2) Détermine le sens de descente du tableau
      eHorizontal direct;
      direct = horiz;     // Si le coin est à Gauche alors on va à Gauche, sinon on va à Droite

      // 3) Tant que l'on est pas arrivé en bas de la colonne -> on descend
      int tempU, tempV;
      while (!Tab[index][col].Equal(termU,termV))
      {  // 3.a) On récupère les coordonnées de l'ancienne case
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

   //--- III] Remplissage de la dernière ligne ---
   int indexMax;
   // 0) => si les 2 points (U3,V3) et (U4,V4) ne sont pas confondu alors
   if (!((Tab[indexG][cGAUCHE].u==Tab[indexD][cDROITE].u) && (Tab[indexG][cGAUCHE].v==Tab[indexD][cDROITE].v)))
   // <=> !(U3==U4 && V3==V4)
   {  //  1) Si ces 2 points sont sur la même ligne
      if ((U3+V3)==(U4+V4))
      {  // 1.a) Si ils sont en HAUT, c'est fini
         if (vert3==HAUT) { indexMax = indexG; }
         else
         { // 1.b) Sinon, ils sont en BAS et faut complète la dernière ligne
           Tab[indexG+1][cGAUCHE].SetPoint(U3+1,V3);
           Tab[indexD+1][cDROITE].SetPoint(U4,V4+1);
           indexMax = indexG+1;
         }
      }
      else // 2) Si ils ne sont pas sur la même ligne
      {  // 2.a) Si c'est le point en bas à droite (U4,V4) qui est le plus haut
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

   // IV]  Affichage des résultats => Debuggage
   int cU, cV, cUMax, cVMax;

   // 1) Pour toutes les lignes remplies du tableau
   for (int index=0; index <= indexMax ; index++)
   {  // 2) On scrute toutes les cases de la ligne de gauche à droite
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
            if (CharacterType==HERO1) DS->BackISOTrapeze(lX-ViewMap->ScreenX,lY-ViewMap->ScreenY, 0x0F1,0);
            #endif
         }

         // Regarde si cette case se trouve sur un mur 
         if ( (cU>=0) && (cV>=0) && (cU<MapNiveau->NivWidth) && (cV<MapNiveau->NivHeight))
           if (  // Si c'est sur un losange du décors Infranchissable
                 (!MapNiveau->PrincEtage->LayerDecors->Carte[(int) cU][(int) cV].isAccessible())
                 // Et si il faut des collisions totales
                &&(MapNiveau->PrincEtage->LayerDecors->Carte[(int) cU][(int) cV].TrapCollide == TOTALE) )
           { return true;
           }

         if ((cU==cUMax) && (cV==cVMax)) finalLigne = true;
         cV--; cU++;   // Avance d'une case sur la droite
      }
   }

   return false;       // Tout c'est bien passé ! aucune collisions rencontrées ...
}
//----------------------------------------------------------------------------//


//=== Classe ClisteObjetFrame ================================================//
//== Gère une liste d'objets de couple numObjet-numFrame                    ==//

//=== recherche du numéro de la frame d'aprés un type d'objet renvoi ===
//  - 0 si pas trouvé dans la liste
//  - ou numFramAnimlSuperposee.
int ClisteObjetFrame::Search(int numTypObj)
{  for (int i=0; i < TabObjetFrame.size(); i++)
   {  if (TabObjetFrame[i].numIDTypeObjet == numTypObj)
      { return TabObjetFrame[i].numFrameAnimSuperposee;
      }
   }
   return 0;
};
//----------------------------------------------------------------------------//

