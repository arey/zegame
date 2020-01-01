////////////////////////////////////////////////////////////////////////////////
//  Unit�  "DSLib.cpp" : Librarie regroupant diverses fonctions de gestion	  //
//							  	 sonore et musicale sous DirectSound.		  //				
//                                                                            //  
//  Copyright (C) 1998 Antoine Rey    antoine.rey@insa-lyon.fr                //
//                                    elric@i-france.com                      //
//					                                                          //
//  Version      : beta 0.01                                                  //
//  Creation     : 10/04/1999                                                 //
//  Last upgrade : 10/04/1999                                                 //
////////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------- include
#include "ZeNain.h"
#include "winuser.h"		// Lib de gestion de messages Win32
#include "mmsystem.h" 		// Windows Multimedia API's
#include "wave.h"			// Librairie microsoft maitrisant le format .wav
#include "INILib.h"			// Lib de gestion des fichiers .ini
#include "FileVirtual.h"	// Fichiers virtuels
#include "SoundUtil.h"		// Librairie maison de lecture de .wav
#include "DSWave.h"			// Classe gerant des fichiers .wav
#include "DSLib.h"			// Header


// ------------------------------------------------------------------- variables
TDirectSound	*DSGame = NULL;  // Seul objet DirectSound du jeu
TDSMuzikLib	*DSMuzikLib = NULL;  // Seul instance du jeu de la classe TDSMuzikLib


////////////////////////////////////////////////////////////////////////////////
//	Classe TDIRECTSOUND														  //
////////////////////////////////////////////////////////////////////////////////

//=== Constructeur par d�faut ================================================//
TDirectSound::TDirectSound()
{	pDS = NULL;
	pPrimaryBuffer = NULL;
	bPause = true;
}
//----------------------------------------------------------------------------//

//=== Destructeur ============================================================//
TDirectSound::~TDirectSound()
{	bPause = true;
	if (pPrimaryBuffer != NULL) pPrimaryBuffer->Release();
	if (pDS != NULL) pDS->Release(); 
}
//----------------------------------------------------------------------------//

//=== Initialise DirectSound =================================================//
bool TDirectSound::InitDSound(HWND hWnd)
{	HRESULT hr;
	DSBUFFERDESC desc;

	hr = DirectSoundCreate(NULL, &pDS, NULL);							// Cr��e l'objet COM DirectSound
	if FAILED(hr) return false;
	
	// [DEBUG] hr = pDS->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE);	// Met en place le mode de coop�ration avec windows : les applications en arri�re-plan sont muettes
    hr = pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if FAILED(hr) return false;

	
	// Cr�e le buffer sonore principal
	ZeroMemory(&desc, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	// [DEBUG] desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	hr = pDS->CreateSoundBuffer(&desc, &pPrimaryBuffer, NULL);
	if FAILED(hr)	return false;

	
	// D�fini par d�faut le format du buffer principal :
	//	44100Hz, 16bits, st�r�o
	WAVEFORMATEX wfx;
	memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
	wfx.wFormatTag = WAVE_FORMAT_PCM;		// Etiquette de format
	wfx.nChannels = 2;						// Nb de canaux
	wfx.nSamplesPerSec = 44100;				// Taux d'�chantillonage
	wfx.wBitsPerSample = 16;				// Nb de bits / �chantillon
	wfx.nBlockAlign = (unsigned short) (wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign; 
	hr = pPrimaryBuffer->SetFormat(&wfx); 
	if FAILED(hr) return false;

	bPause = false;
	return true;		// Initialisation completed
}

//=== Fait une ch'tite pause =================================================//
void TDirectSound::SetPause(bool p)
{	bPause = p;
}
//----------------------------------------------------------------------------//

//=== Renvoie vrai si l'on est en pause ======================================//
bool TDirectSound::IsPaused()
{	return bPause;
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
//	Classe TDSSoundFile 													  //
////////////////////////////////////////////////////////////////////////////////

//===	Constructeur par d�faut ================================================//
TDSSoundFile::TDSSoundFile()
{	hSoundFV = NULL;
    pSoundNotify = NULL;
    pSoundBuffer = NULL;
	soundName[0] = '\0';
	soundPathName[0] = '\0';
	bPlaying = false;
}
//----------------------------------------------------------------------------//


//=== Destructeur ============================================================//
TDSSoundFile::~TDSSoundFile()
{	// ferme le fichier .wav ouvert
/*	#ifdef NEW_FILE_MANAGER
	WaveCloseReadFileFV(hSoundFV, &pWaveFormat);
	#else
	WaveCloseReadFile(&hWaveFile, &pWaveFormat);			
	#endif*/
	// disable le gestionnaire d'�v�nements 
	if (pSoundNotify) pSoundNotify->Release();	
	// ferme le sound buffer
	if (pSoundBuffer) pSoundBuffer->Release();
}
//----------------------------------------------------------------------------//


//=== Ventile les evenements windows concernant le don =======================//
void TDSSoundFile::DSWinMain()
{	
	// Supension de l'�x�cution du programme jusqu'� ce qu'il se produise :
	//  - soit l'un des �v�nements  de hEvents est d�clench�,
	//  - soit un message est plac� dans la file
	DWORD dwEvt = MsgWaitForMultipleObjects(
						NUMEVENTS,				// Nb d'�v�nements
						hEvent,					// Emplacement des handles
						FALSE,					// Ne les attends pas tous
						0,						// Dur�e d'attente
						QS_ALLINPUT);			// Chaque message est un �v�nement

	//   => renvoie ds 'dwEvt' un indice du tableau des �v�nements en sachant
	//   que WAIT_OBJET_0 d�signe le 1er �l�ment
	dwEvt -= WAIT_OBJECT_0;

	// Si l'�v�nement a �t� d�fini par le tampon de capture sonore, on doit le traiter
	if ( dwEvt < NUMEVENTS)
	{	StreamToBuffer(dwEvt);		// Copie des donn�es wav vers le buffer audio
	} 
}
//----------------------------------------------------------------------------//


//=== Vrai si l'on est en-train de jouer le .wav =============================//
bool TDSSoundFile::isPlaying()
{	return bPlaying;
}
//----------------------------------------------------------------------------//

//=== Revient au d�but de la musique =========================================//
void TDSSoundFile::Beginning()
{	// [A REVOIR] Pas au point dut tout -> Milia2000
	// => on rouvre le fichier et on perd donc du temps
	Open(soundPathName, soundName);
}
//----------------------------------------------------------------------------//

//=== Fait une p'tite pause ==================================================//
void TDSSoundFile::SetPause(bool pause)
{	if (pause)
	{	pSoundBuffer->Stop();
	} else
	{	Play();
	}
}
//----------------------------------------------------------------------------//

//=== Joue le .wav en boucle ou non ==========================================//
void TDSSoundFile::Play()
{	bPlaying = true;
	pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
}
//----------------------------------------------------------------------------//

//=== Ouvre un fichier wav et se pr�pare � le lire ===========================//
bool TDSSoundFile::Open(char* soundPathName, char* name)
{	if (name!=NULL)
	{	strcpy(soundName, name);
	} else
	{	soundName[0]='\0';
	}
	if (soundPathName!=NULL)
	{	strcpy(this->soundPathName, soundPathName);
	} else
	{	this->soundPathName[0]='\0';
	}

	    
    if (pSoundNotify != NULL)
    {   pSoundNotify->Release();
        pSoundNotify = NULL;
    } 
    if (pSoundBuffer != NULL)
    {   pSoundBuffer->Release();
        pSoundBuffer = NULL;
    }

	return true;
}
//----------------------------------------------------------------------------//

//=== Cree un DirectSound buffer de @length octets ===========================//
// Et prepare les call back functions
HRESULT TDSSoundFile::createSoundBuffer(int length)
{	// On cr�e un buffer secondaire au m�me format que le fichier .wav que l'on veut lire
	// -> c'est un streaming buffer qui �volue en permance
	memset(&desc, 0, sizeof(DSBUFFERDESC)); 	
	desc.dwSize = sizeof(DSBUFFERDESC);     
	desc.dwFlags = 
            DSBCAPS_GETCURRENTPOSITION2   // Toujours une bonne id�e
            | DSBCAPS_GLOBALFOCUS         // Permet le background playing
            | DSBCAPS_CTRLPOSITIONNOTIFY; // Needed for notification 
	// La taille  du buffer est arbitraire mais doit �tre au moins de 2 secondes pour que l'on puisse �crire des donn�es dedans pdt qu'il est lu
	desc.dwBufferBytes = length;  
	desc.lpwfxFormat = pWaveFormat;
	HRESULT hr = DSGame->pDS->CreateSoundBuffer(&desc, &pSoundBuffer, NULL);
	if FAILED(hr) return hr;

		// Etant donn� que l'on a affaire � un streaming-buffer, on va devoir �tre averti lorsque la 
	// position courante du player audio aura atteint certaines positions : ce sera temps de
	// remplir le flux audio avec les prochaines donn�es audio. 
	// Ici, on va �tre averti au d�but et au milieu du buffer; sinon, on sera averti � intervalle 
	// r�gulier du buffer.
	
	
	// Cr�ation des NUMEVENTS �v�nements ds 'hEvent'
	for (int i = 0; i < NUMEVENTS; i++)
	{	hEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == hEvent[i]) return -1;
	}

	// Associe une certaine position du buffer � un handler
	int offset = 0;
	for (i = 0; i < NUMEVENTS; i++)
	{	EventPos[i].dwOffset = offset;
		EventPos[i].hEventNotify = hEvent[i];
		offset += desc.dwBufferBytes / NUMEVENTS;
    }

	// Interface le buffer secondaire avec le gestionnaire d'�v�nement
	hr = IDirectSoundBuffer_QueryInterface(pSoundBuffer, IID_IDirectSoundNotify, (VOID **)&pSoundNotify);
    if FAILED(hr) return hr; 
 
    // Lui confie les �v�nements souhait�s	
	hr = IDirectSoundNotify_SetNotificationPositions(pSoundNotify, NUMEVENTS, EventPos);
    if FAILED(hr)
	{	IDirectSoundNotify_Release(pSoundNotify);
        return hr;
    }
	return 0;
}
//----------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
//	Classe TDSMUZIKLIB														  //
////////////////////////////////////////////////////////////////////////////////

extern AnsiString CheminApplication;
extern const char cPathSounds[];
extern const char cIniZeGame[];

//=== Constructeur par d�faut ================================================//
TDSMuzikLib::TDSMuzikLib(bool musik)
{	int		nbMuzik;
	char	Zikpath[512]; 
	char	ZikpathName[512];
	char	ZikName[128];
	char	ZikNumber[8];
	TDSSoundFile* pSoundFile = NULL;

	iCurrentMuzik = -1;
	bMusikGame = musik;
	
	if (!bMusikGame) return;

	// R�cup�re les fichiers wav d'ambiance musicale
	TINILib	ini((char*) cIniZeGame);
	nbMuzik = ini.GetIntProfile("Audio", "NbMuzik", 0);
	if (nbMuzik>0)
	{	strcpy(Zikpath, LPCSTR(CheminApplication));
		for (int i=1 ; i <= nbMuzik ; i++)
		{	// R�cup�re le nom du .wav suivant
			strcpy(ZikName, "MuzikFile");
			itoa(i, ZikNumber, 10);
			strcat(ZikName, ZikNumber);		
			ini.GetStringProfile("Audio", ZikName, ZikName);
			#ifndef NEW_FILE_MANAGER
			strcpy(ZikpathName, Zikpath);
			strcat(ZikpathName, cPathSounds);
			#else
			strcpy(ZikpathName, cPathSounds);
			#endif // NEW_FILE_MANAGER
			strcat(ZikpathName, ZikName);
			// Pr�pare le fichier � �tre jou� et l'ajoute � la liste des musiques
			pSoundFile = AllocateSoundFile(ZikName);
			if (pSoundFile != NULL)
			{	pSoundFile->Open(ZikpathName, ZikName);
				vSoundMuzik.push_back(pSoundFile);
			}
		}

		// Puis on joue la 1�re Musique
		iCurrentMuzik = 0;
		vSoundMuzik[iCurrentMuzik]->Play();
	}
}
//----------------------------------------------------------------------------//


//=== Destructeur ============================================================//
TDSMuzikLib::~TDSMuzikLib()
{	for (int i=0; i < vSoundMuzik.size() ; i++)
	{	delete vSoundMuzik[i];
	}
	vSoundMuzik.clear();
}
//----------------------------------------------------------------------------//

//=== Renvoi une instance d'un fichier sonore suivant son extension ==========//
// Gere : .wav, 
TDSSoundFile* TDSMuzikLib::AllocateSoundFile(char* filename)
{	char extension[10];
	int i =0, j=0;
	bool parseext = false;
	// Recupere l'extension du fichier sonore
	while ((filename[i]!='\0') && (j<10))
	{	if (parseext)
		{	extension[j++] = filename[i];
		}
		else if (filename[i]=='.') parseext = true;
		i++;
	}
	if ((j<1) || (j>=10)) return NULL;
	extension[j] = '\0';

	if (strcmp(extension, "wav")==0) return new TDSWave();
	else if (strcmp(extension, "WAV")==0) return new TDSWave();
	
	return NULL;
}

//=== S'occupe de mettre � jour le SoundBuffer ===============================//
void TDSMuzikLib::MuzikManage()
{	// on sort si on est pas en train de jouer les musiques d'ambiance
	if (!bMusikGame) return;

	if (iCurrentMuzik != -1)
	{	// Fichier wav en cours
		if (vSoundMuzik[iCurrentMuzik]->isPlaying())
		{	vSoundMuzik[iCurrentMuzik]->DSWinMain();
		} else
		// Si il est fini, on joue le suivant
		{	iCurrentMuzik++;
			if (iCurrentMuzik >= vSoundMuzik.size()) iCurrentMuzik = 0;
			vSoundMuzik[iCurrentMuzik]->Beginning();
			vSoundMuzik[iCurrentMuzik]->Play();
		}
	}	
}
//----------------------------------------------------------------------------//

//=== Stoppe toutes les musiques =============================================//
void TDSMuzikLib::SetPause(bool pause)
{	if (DSGame == NULL) return;
	// V�rifie si l'on n'est pas d�j� ds l'�tat souhait�
	if (DSGame->IsPaused() == pause) return;

	if (iCurrentMuzik != -1)
	{	DSGame->SetPause(pause);
		vSoundMuzik[iCurrentMuzik]->SetPause(pause);		
	}
}
//----------------------------------------------------------------------------//

/////////////////////// Fin de la classe TDSMuzikLib ///////////////////////////



///////////////////////////////////////////////////////////////////////////////
//							Fonctions EXTERNES								 //
///////////////////////////////////////////////////////////////////////////////


//=== Initialize tout ce qui se rapporte au son =============================//
void InitGame_DirectSound(HWND hWnd)
{	char sMusik[16];
	TINILib	ini((char*) cIniZeGame);
	ini.GetStringProfile("Audio", "Muzik", sMusik);
	bool bMusik = (strcmp(sMusik, "Yes") == 0) ;
	if (bMusik)
	{	DSGame = new TDirectSound();
		DSGame->InitDSound(hWnd);
	}
	DSMuzikLib = new TDSMuzikLib(bMusik);	
}
//---------------------------------------------------------------------------//

//=== Initialize tout ce qui se rapporte au son =============================//
void CloseGame_DirectSound()
{	delete DSMuzikLib; DSMuzikLib = NULL;
	delete DSGame; DSGame = NULL;
}
//---------------------------------------------------------------------------//

