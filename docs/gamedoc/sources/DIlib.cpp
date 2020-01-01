////////////////////////////////////////////////////////////////////////////////
//  Unité : "DIlib.cpp" : librairie de fonctions associées à DirectInput      //
//	Copyright (C) 1998 Antoine Rey  antoine.rey@insa-lyon.fr				  //
//    					elric@i-france.com									  //
//	Version	     :  0.18													  //
//	Création     :  8/07/98													  //
//  Last Upgrade :  16/04/99												  //
////////////////////////////////////////////////////////////////////////////////

#include "ZeNain.h"			// Fichier commun à tout le projet
#include "DILib.h"			// Header de la classe

TDirectInput *DI = NULL;
// états des TOUCHES  enfoncées ou relâchées  (enfoncées == TRUE)
BOOL	Key_UP, Key_DOWN, Key_LEFT, Key_RIGHT,			// 4 flèches
		// pavé numérique
		Key_npE, Key_npSE, Key_npS, Key_npSO, Key_npO, Key_npNO, Key_npN, Key_npNE,
        // Espace, Contrôle et Alternative
		Key_SPACE, Key_LSHIFT, Key_LCTRL,
        // Escape
        Key_ESCAPE,
        // Touches caractères
        Key_K,
        // Chiffres
        Key_1, Key_2, Key_3, 
		// Ponctuation & autres
		Key_BACKSLASH, 
        // 6+1 touches utilisés par le GammaControl
        Key_INSERT, Key_DELETE, Key_PAGEUP, Key_PAGEDOWN, Key_END, Key_HOME, Key_BACKSPACE;

// états plus affinés des TOUCHES rien=0; appuyée=1; enfoncée=2; relâchée=3;
BYTE Key_ENTER = 0, Key_PLUS =0, Key_MOINS =0, Key_F1, Key_F2, Key_F3, Key_F4, Key_F5, Key_F6, Key_F7,
     Key_F8, Key_F12, Key_LALT = 0, Key_PAUSE = 0;


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// FONCTIONS TDIRECINPUT 													  //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------- Variables & Constantes
//=== Utilisés pour l'énumération des joys ===
const int NB_MAX_JOYSTICK = 22;  // Rien que ca pour se faire un pure Fifa à 22 !
static LPDIRECTINPUTDEVICE lpJoyDevices[NB_MAX_JOYSTICK];
static int nbJoyFind = 0;
int CALLBACK EnumJoysticks(LPCDIDEVICEINSTANCE pInst, LPVOID lpvContext);  // Voir fonctions annexes

// Constructeur --------------------------------------------------------------//
TDirectInput::TDirectInput(HWND Hand)
{
    // Initialise les variables à zéro
	pDI = NULL;
    pDIKeyBoard = NULL;
    bKeyBoard = false;
    bKeyboardActif = false;
    memset(Keys,0,256);
    pDIJoypad = NULL;
    bJoypad = false;
    bJoypadActif = false;

    // Crée l'objet DirectInput manageant le tout
    HRESULT hr;
	#ifdef VISUALC
	hr = DirectInputCreate(AfxGetInstanceHandle(), DIRECTINPUT_VERSION, &pDI, NULL);
	#elif BORLAND
	hr = DirectInputCreate(HInstance, DIRECTINPUT_VERSION, &pDI, NULL);
	#endif
    if FAILED(hr) TraceErrorDI(hr,99);

    // Met en place le Handle
    DIHandle = Hand;
};
//----------------------------------------------------------------------------//

// Active les différents Devices ---------------------------------------------//
void TDirectInput::CreateDevice(BOOL bClavier,BOOL bSouris,BOOL bJoy)
{
    HRESULT hr;

    // si on veut le CLAVIER et si ce n'est déjà fait :
    if ((bClavier) && !(bKeyBoard))
    {
        // création du keyboard device
        hr = pDI->CreateDevice(GUID_SysKeyboard, &pDIKeyBoard, NULL);
        if FAILED(hr) TraceErrorDI(hr,99);

        // set Keyboard data format
        pDIKeyBoard->SetDataFormat(&c_dfDIKeyboard);
        if FAILED(hr) TraceErrorDI(hr,99);

        // set Cooperative Level
        pDIKeyBoard->SetCooperativeLevel(DIHandle, /*DISCL_EXCLUSIVE |*/ DISCL_FOREGROUND);
        if FAILED(hr) TraceErrorDI(hr,99);

        // Donne accés au clavier
        pDIKeyBoard->Acquire();
        if FAILED(hr) TraceErrorDI(hr,99);

        bKeyboardActif = true;
        bKeyBoard = TRUE;	// accés au clavier
    }

    // si on veut le JOYPAD et si ce n'est déjà fait :
    if ((bJoy) && !(bJoypad))
    { // Recherche des joysticks connectés à l'ordi
      nbJoyFind = 0;   // Nb de joys trouvés égal à 0
      pDI->EnumDevices(DIDEVTYPE_JOYSTICK, EnumJoysticks, pDI, DIEDFL_ATTACHEDONLY);
      if (nbJoyFind!=0)  // Aucun joystick n'a été trouvé
      { int joyChoose = 0; // On prends le 1er joystick trouvé
        hr = lpJoyDevices[joyChoose]->QueryInterface(IID_IDirectInputDevice2, (LPVOID*) &pDIJoypad);
        if FAILED(hr) TraceErrorDI(hr,99);
        lpJoyDevices[joyChoose]->Release();

        // On choisit d'utiliser un "simple joystick" et non pas un truc à la ultra sidefreestyle force-feedback pro turbo +
        pDIJoypad->SetDataFormat(&c_dfDIJoystick);
        // Rmq : en DISCL_FORGROUND -> ca plante ! ???
        pDIJoypad->SetCooperativeLevel(DIHandle, DISCL_EXCLUSIVE | DISCL_BACKGROUND);

        // Calibrage des axes du joy
        DIPROPRANGE diprg;  // Axe des X de (-1000, +1000)
        diprg.diph.dwSize       = sizeof(diprg);
        diprg.diph.dwHeaderSize = sizeof(diprg.diph);
        diprg.diph.dwObj        = DIJOFS_X;
        diprg.diph.dwHow        = DIPH_BYOFFSET;
        diprg.lMin              = -1000;
        diprg.lMax              = +1000;
        hr = pDIJoypad->SetProperty(DIPROP_RANGE, &diprg.diph);
        if FAILED(hr) TraceErrorDI(hr,99);
	diprg.diph.dwObj = DIJOFS_Y;  // De même pour l'axe des Y
        hr = pDIJoypad->SetProperty(DIPROP_RANGE, &diprg.diph);
        if FAILED(hr) TraceErrorDI(hr,99);

        // Calibrage des dead-zone du joy
        // Dead zone sur les X de 20%
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize       = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwObj        = DIJOFS_X;
	dipdw.diph.dwHow        = DIPH_BYOFFSET;
	dipdw.dwData            = 2000;
	hr = pDIJoypad->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
        if FAILED(hr) TraceErrorDI(hr,99);
        // *** Macro **** SetDIDwordProperty(pDIJoypad, DIPROP_DEADZONE, DIJOFS_Y, DIPH_BYOFFSET, 2000);
        dipdw.diph.dwObj = DIJOFS_Y;              // De même pour les Y
      	hr = pDIJoypad->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
        if FAILED(hr) TraceErrorDI(hr,99);

        // On peut utiliser le joy dès maintenant
        hr = pDIJoypad->Acquire();
        if FAILED(hr) TraceErrorDI(hr,99);

        bJoypadActif = true;
        bJoypad = true;
      }
    }
};
//----------------------------------------------------------------------------//

// Destructeur ---------------------------------------------------------------//
TDirectInput::~TDirectInput(void)
{
    if (pDI!=NULL)
     {
        if (pDIKeyBoard!=NULL)
        { if (bKeyboardActif) pDIKeyBoard->Unacquire();
          pDIKeyBoard->Release();
          pDIKeyBoard = NULL;
        }
        if (pDIJoypad!=NULL)
        {  if (bJoypadActif) pDIJoypad->Unacquire();
	   pDIJoypad->Release();
           pDIJoypad = NULL;
        }
        pDI->Release();
        pDI = NULL;
     };
};
//----------------------------------------------------------------------------//

// Recoit les données et les analyse -----------------------------------------//
void TDirectInput::AnalyseKeys()
{  HRESULT hr;
   if ((bKeyBoard) && (bKeyboardActif)) // s'occupe du CLAVIER
   {
      // Récupère l'état des touches
      hr = pDIKeyBoard->GetDeviceState(256,(LPVOID) &Keys);
      if (hr==DIERR_INPUTLOST) pDIKeyBoard->Acquire();      // clavier perdu

              // Analyse les touches (renvoie TRUE si enfoncée)
      Key_UP     = KEYDOWN(DIK_UP,Keys);			// 4 flèches
      Key_DOWN   = KEYDOWN(DIK_DOWN,Keys);
      Key_LEFT   = KEYDOWN(DIK_LEFT,Keys);
      Key_RIGHT  = KEYDOWN(DIK_RIGHT,Keys);
      // Pavé numérique  => 8 directions
      Key_npE	   = KEYDOWN(DIK_NUMPAD6,Keys);
      Key_npSE   = KEYDOWN(DIK_NUMPAD3,Keys);
      Key_npS	   = KEYDOWN(DIK_NUMPAD2,Keys);
      Key_npSO   = KEYDOWN(DIK_NUMPAD1,Keys);
      Key_npO	   = KEYDOWN(DIK_NUMPAD4,Keys);
      Key_npNO   = KEYDOWN(DIK_NUMPAD7,Keys);
      Key_npN	   = KEYDOWN(DIK_NUMPAD8,Keys);
      Key_npNE   = KEYDOWN(DIK_NUMPAD9,Keys);
      // touches de contrôles du perso
      Key_LCTRL  = KEYDOWN(DIK_LCONTROL,Keys);
      Key_LSHIFT = KEYDOWN(DIK_LSHIFT,Keys);
      Key_SPACE  = KEYDOWN(DIK_SPACE,Keys);
      // Touche Escape
      Key_ESCAPE = KEYDOWN(DIK_ESCAPE,Keys);
      // Touches caractères
      Key_K      = KEYDOWN(DIK_K,Keys);
      // Touches des chiffres
      Key_1      = KEYDOWN(DIK_1,Keys);
      Key_2      = KEYDOWN(DIK_2,Keys);
	  Key_3      = KEYDOWN(DIK_3,Keys);
      // Touches servant au contrôle Gamma
      Key_INSERT    = KEYDOWN(DIK_INSERT,Keys);
      Key_DELETE    = KEYDOWN(DIK_DELETE,Keys);
      Key_PAGEUP    = KEYDOWN(DIK_PRIOR,Keys);
      Key_PAGEDOWN  = KEYDOWN(DIK_NEXT,Keys);
      Key_END       = KEYDOWN(DIK_END,Keys);
      Key_HOME      = KEYDOWN(DIK_HOME,Keys);
      Key_BACKSPACE = KEYDOWN(DIK_BACK,Keys);
	  // Touches ponctuations et signes
	  Key_BACKSLASH = KEYDOWN(DIK_Z ,Keys);

      // touches "spéciales"
      // Touches pour lesquelles ont ne veut savoir que si elles viennent d'être appuyée
      Key_ENTER  = TouchesPlus(Key_ENTER,DIK_RETURN);
      Key_PLUS   = TouchesPlus(Key_PLUS,DIK_ADD);
      Key_MOINS  = TouchesPlus(Key_MOINS,DIK_SUBTRACT);
      Key_F1     = TouchesPlus(Key_F1,DIK_F1);
      Key_F2     = TouchesPlus(Key_F2,DIK_F2);
      Key_F3     = TouchesPlus(Key_F3,DIK_F3);
      Key_F4     = TouchesPlus(Key_F4,DIK_F4);
      Key_F5     = TouchesPlus(Key_F5,DIK_F5);
      Key_F6     = TouchesPlus(Key_F6,DIK_F6);
      Key_F7     = TouchesPlus(Key_F7,DIK_F7);
      Key_F8     = TouchesPlus(Key_F8,DIK_F8);
      Key_F12    = TouchesPlus(Key_F8,DIK_F12);      
      Key_LALT   = TouchesPlus(Key_LALT,DIK_LALT);
	  Key_PAUSE  = TouchesPlus(Key_PAUSE, DIK_P);	// DIK_PAUSE pris en charge qu'à partir de DirectX 7
   }

   if ((bJoypad) && (bJoypadActif)) // On s'occupe ensuite du Joystick
   {  DIJOYSTATE joystate;
      // Comme certain drivers ne préviennent pas DirectInput de l'état du périph, on scrute nous-même son état
      hr = pDIJoypad->Poll();
      if (hr != DI_OK)
      {  // Si ca échoue, on essaye de réacquérir le joysytick
         bJoypadActif = false;
         if (hr == DIERR_INPUTLOST) ReActive();
      }
      else
      {  // Puis on récupère les données du joystick
         hr = pDIJoypad->GetDeviceState(sizeof(joystate), &joystate);
         if (hr != DI_OK)
         {  // Si ca échoue, on essaye de réacquérir le joysytick
            bJoypadActif = false;
            if (hr == DIERR_INPUTLOST) ReActive();
         }
         else  // Si le Joystick est toujours actif, on continue ...
         {  // Récupère les paramètres d'état du joy : 3 axes et 32 boutons
             axeX = joystate.lX;
             axeY = joystate.lY;
             axeZ = joystate.lZ;
             for (int i=0 ; i < 32 ; i++) joyButtons[i] = joystate.rgbButtons[i];

             // Pour les touches du clavier qui ne sont pas enfoncées,
             // on regarde si leur équivalent sur le joypad ne sont pas non plus enfoncées
             // - d'abord pour les 4 boutons
             if (!Key_LCTRL)  Key_LCTRL  = KEYDOWN(0, DI->joyButtons);   // Left Ctrl <=> Bouton 0
             if (!Key_SPACE)  Key_SPACE  = KEYDOWN(1, DI->joyButtons);   // Espace <=> Bouton 1
             if (!Key_LSHIFT) Key_LSHIFT = KEYDOWN(2, DI->joyButtons);   // Left Shift <=> Bouton 2
             if (Key_LALT == EVENTKEY_RELACHE)                           // Left ALt <=> bouton 3
             { Key_LALT   = JoyBoutonsPlus(Key_LALT, 3);
             }

             // - puis les touches de direction
             if (!Key_UP)    if (axeY < 0) Key_UP = true;
             if (!Key_DOWN)  if (axeY > 0) Key_DOWN = true;
             if (!Key_LEFT)  if (axeX < 0) Key_LEFT = true;
             if (!Key_RIGHT) if (axeX > 0) Key_RIGHT = true;
        }
      }
   }
};
//----------------------------------------------------------------------------//

// Réactive les périphériques ------------------------------------------------//
void TDirectInput::ReActive()
{  ReActive(true, true, true);
};
//----------------------------------------------------------------------------//

//=== Réactive le périph. précisé ============================================//
void TDirectInput::ReActive(bool bClavier, bool bSouris, bool bJoy)
{  HRESULT hr;
   if ((bClavier) && (bKeyBoard) && (!bKeyboardActif))
   { hr =  pDIKeyBoard->Acquire();
     if  (hr==DI_OK) bKeyboardActif = true;
   }
   if ((bJoy) && (bJoypad) && (!bJoypadActif))
   { hr = pDIJoypad->Acquire();
     if  (hr==DI_OK) bJoypadActif = true;
   }
}
//----------------------------------------------------------------------------//

// Réactive les périphériques ------------------------------------------------//
void TDirectInput::UnActive()
{  UnActive(true, true, true);
}
//----------------------------------------------------------------------------//

//=== Désactive le périph. précisé ===========================================//
void TDirectInput::UnActive(bool bClavier, bool bSouris, bool bJoy)
{  HRESULT hr;
   if ((bClavier) && (bKeyBoard) && (bKeyboardActif))
   { hr =  pDIKeyBoard->Unacquire();
     if (hr==DI_OK) bKeyboardActif = false;
   }
   if ((bJoy) && (bJoypad) && (bJoypadActif))
   { hr = pDIJoypad->Unacquire();
     if (hr==DI_OK) bJoypadActif = false;
   }
}
//----------------------------------------------------------------------------//

// Redonne le nouvel état d'une Touche + -------------------------------------//
BYTE TDirectInput::TouchesPlus(BYTE Touch,BYTE TouchConst)
{
     switch (Touch)
     {  case 0 : if KEYDOWN(TouchConst,Keys) Touch=1; break;
        case 1 : Touch = 2; break;
        case 2 : if (!KEYDOWN(TouchConst,Keys)) Touch=3; break;
        case 3 : Touch = 0; break;
     }
     return Touch;     
};
//----------------------------------------------------------------------------//

// Redonne le nouvel état d'un Bouton + --------------------------------------//
BYTE TDirectInput::JoyBoutonsPlus(BYTE Bouton, BYTE BoutonConst)
{
     switch (Bouton)
     {  case 0 : if KEYDOWN(BoutonConst, joyButtons) Bouton=1; break;
        case 1 : Bouton = 2; break;
        case 2 : if (!KEYDOWN(BoutonConst, joyButtons)) Bouton=3; break;
        case 3 : Bouton = 0; break;
     }
     return Bouton;
};
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
// FONCTIONS ANNEXES														  //
////////////////////////////////////////////////////////////////////////////////


// TRACERRORDI ---------------------------------------------------------------//
void TraceErrorDI(HRESULT hErr,int Gravite)    // Gestion des erreurs de DirectInput
{
    AnsiString Erreur;

    //if ((DI!=NULL) && (DI->pDI!=NULL)) DS->pDD->FlipToGDISurface(); // DS->pDD vient de "DDLib.cpp"
    switch (hErr)
    {
       	case DI_BUFFEROVERFLOW  : Erreur="The device buffer overflowed and some input was lost. This value is equal to the S_FALSE standard COM return value.";break;
        case DI_DOWNLOADSKIPPED : Erreur="The parameters of the effect were successfully updated, but the effect could not be downloaded because the associated device was not acquired in exclusive mode.";break;
        case DI_EFFECTRESTARTED : Erreur="The effect was stopped, the parameters were updated, and the effect was restarted.";break;
        case DI_POLLEDDEVICE	 : Erreur="The device is a polled device. As a result, device buffering will not collect any data and event notifications will not be signaled until the IDirectInputDevice2::Poll method is called.";break;
        case DI_TRUNCATED		 : Erreur="The parameters of the effect were successfully updated, but some of them were beyond the capabilities of the device and were truncated to the nearest supported value.";break;
        case DI_TRUNCATEDANDRESTARTED  : Erreur="Equal to DI_EFFECTRESTARTED | DI_TRUNCATED.";break;
        case DIERR_ACQUIRED	 : Erreur="The operation cannot be performed while the device is acquired.";break;
        case DIERR_ALREADYINITIALIZED  : Erreur="This object is already initialized";break;
        case DIERR_BADDRIVERVER : Erreur="The object could not be created due to an incompatible driver version or mismatched or incomplete driver components.";break;
        case DIERR_BETADIRECTINPUTVERSION : Erreur="The application was written for an unsupported prerelease version of DirectInput.";break;
        case DIERR_DEVICEFULL   : Erreur="The device is full.";break;
        case DIERR_DEVICENOTREG : Erreur="The device or device instance is not registered with DirectInput. This value is equal to the REGDB_E_CLASSNOTREG standard COM return value.";break;
        case DIERR_EFFECTPLAYING : Erreur="The parameters were updated in memory but were not downloaded to the device because the device does not support updating an effect while it is still playing.";break;
        case DIERR_HASEFFECTS   : Erreur="The device cannot be reinitialized because there are still effects attached to it.";break;
        case DIERR_GENERIC		 : Erreur="An undetermined error occurred inside the DirectInput subsystem. This value is equal to the E_FAIL standard COM return value.";break;
        case DIERR_HANDLEEXISTS : Erreur="The device already has an event notification associated with it. This value is equal to the E_ACCESSDENIED standard COM return value.";break;
        case DIERR_INCOMPLETEEFFECT  : Erreur="The effect could not be downloaded because essential information is missing. For example, no axes have been associated with the effect, or no type-specific information has been supplied.";break;
        case DIERR_INPUTLOST 	 : Erreur="Access to the input device has been lost. It must be reacquired.";break;
        case DIERR_INVALIDPARAM : Erreur="An invalid parameter was passed to the returning function, or the object was not in a state that permitted the function to be called. This value is equal to the E_INVALIDARG standard COM return value.";break;
        case DIERR_MOREDATA	 : Erreur="Not all the requested information fitted into the buffer.";break;
        case DIERR_NOAGGREGATION  : Erreur="This object does not support aggregation.";break;
        case DIERR_NOINTERFACE  : Erreur="The specified interface is not supported by the object. This value is equal to the E_NOINTERFACE standard COM return value.";break;
        case DIERR_NOTACQUIRED  : Erreur="The operation cannot be performed unless the device is acquired.";break;
        case DIERR_NOTBUFFERED  : Erreur="The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering.";break;
        case DIERR_NOTDOWNLOADED : Erreur="The effect is not downloaded.";break;
        case DIERR_NOTEXCLUSIVEACQUIRED  : Erreur="The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode.";break;
        case DIERR_NOTFOUND  	 : Erreur="The requested object does not exist.";break;
        case DIERR_NOTINITIALIZED  : Erreur="This object has not been initialized.";break;
        case DIERR_OLDDIRECTINPUTVERSION  : Erreur="The application requires a newer version of DirectInput.";break;
        case DIERR_OUTOFMEMORY  : Erreur="The DirectInput subsystem couldn't allocate sufficient memory to complete the call. This value is equal to the E_OUTOFMEMORY standard COM return value.";break;
        case DIERR_UNSUPPORTED  : Erreur="The function called is not supported at this time. This value is equal to the E_NOTIMPL standard COM return value.";break;
        case E_PENDING  		 : Erreur="Data is not yet available.";break;
     	default : Erreur="Erreur inconnue !"; break;
    }
    // appelle d'une fonction de "DDLibcpp"
	#ifdef VISUALC
	MessageError("Erreur DirectInput",(char*) LPCSTR(Erreur),Gravite);
	#elif BORLAND
	MessageError("Erreur DirectInput",Erreur.c_str(),Gravite);
	#endif
}
//----------------------------------------------------------------------------//


//=== Appelée afin d'énumérer les Joysticks présents =========================//
int CALLBACK EnumJoysticks(LPCDIDEVICEINSTANCE pInst, LPVOID lpvContext)
{   HRESULT hr;
    LPDIRECTINPUTDEVICE pDevice;
    LPDIRECTINPUT pdi = (LPDIRECTINPUT) lpvContext;

    // Crée une interface
    hr = pdi->CreateDevice( pInst->guidInstance, &pDevice, NULL );
    // Si ca échoue, le joystick doit probablement être mal branché ou défaillant
    if (FAILED(hr)) return DIENUM_STOP;
    lpJoyDevices[nbJoyFind++] = pDevice;  // Un joystick de plus de trouvé
    return DIENUM_CONTINUE;               // Et l'on regarde si y'en a pas un autre de branché, par hazard ?
}
//----------------------------------------------------------------------------//


