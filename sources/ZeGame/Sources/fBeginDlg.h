////////////////////////////////////////////////////////////////////////////////
//  Unit�	 : fBeginDlg.h													  //
//	R�le	 : fiche de configuration lanc�e au d�marrage du jeu			  //	
//	Remarque : m�me r�le que l'unit� fBegin sous C++Builder					  //	
////////////////////////////////////////////////////////////////////////////////

#ifndef dBeginDlgH
#define dBeginDlgH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Classe fBeginDlg  :  bo�te de dialogue avec le joueur				   //
/////////////////////////////////////////////////////////////////////////////
class fBeginDlg : public CDialog
{
// Construction
public:

	fBeginDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(fBeginDlg)
	enum { IDD = IDD_ZEGAME_CONFIGFICHE };
	CButton	m_FullScreenBtn;
	CButton	m_CBPeriodeMode;
	CButton	m_CBDemoTournante;
	CButton	m_CBPnj;
	CEdit	m_LevelNameEdit;
	CComboBox	m_ResolutionCB;
	CButton	m_CBPlayOnline;
	CEdit	m_PlayerNameEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(fBeginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	bool LetsGo() { return bLetsGo; }
	bool LetsShow() { return bShowDialogBox; }
	static void setShow(bool show) { bShowDialogBox = show; }

// Implementation
protected:
	HICON m_hIcon;						// Handle sur l'ic�ne
	CString CheminApplication_deb;		// Chemin de l'ex�cutable
	bool bLetsGo;						// Vrai lorsque l'utilisateur a appuye sur le btn OK
	static bool bShowDialogBox;			// Indique si il faut afficher cette boite de dialogue au demarrage ou non
	int resolution;

	int GetResolution(char* res);		// Renvoi la r�solution 
	AnsiString randomName();			//Renvoi aleatoirement un nom de joueur
	
public:
	// Generated message map functions
	//{{AFX_MSG(fBeginDlg)
	afx_msg void OnLetsgo();
	afx_msg void OnButtonCharger();
	afx_msg void OnCheckPlayonline();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // dBeginDlgH
