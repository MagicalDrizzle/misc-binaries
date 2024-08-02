 /***************************************************************************
 *                                 RasMol                                  *
 *                                                                         *
 *                 Molecular Graphics Visualisation Tool                   *
 *                                                                         *
 * Please read the file RASLIC for the license terms which apply to this   *
 * file (Rasmol license).                                                  *
 ***************************************************************************/

/* Authors:
 * Philippe Valadon, copyright (c) 2000-2007
 */




/////////////////////////////////////////////////////////////////////////////
// dialog.h
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_DIALOG_H)
#define AFX_DIALOG_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



/////////////////////////////////////////////////////////////////////////////
// CDialogSetValue dialog

#define Type_Ident		1
#define Type_Ribbons	2
#define Type_Strands	3
#define Type_Bonds		4
#define Type_Dots		5
#define Type_Radius		6
#define Type_Sphere		7
#define Type_HBonds		8
#define Type_SSBonds	9
#define Type_Trace		10
#define Type_Cartoons	11
#define Type_Renumber	12
#define Type_Within		13
#define Type_CisAngle	14
#define Type_DotSize	15
#define Type_Stereo     16
#define Type_MolSize    17

class CDialogSetValue : public CDialog
{
// Construction
public:
	int m_ValueType;
	CDialogSetValue(CWnd* pParent = NULL); 

// Dialog Data
	//{{AFX_DATA(CDialogSetValue)
	enum { IDD = IDD_DIALOG_SETVALUE };
	int	m_ValueNumber;
	CString m_ValueTex;
	CString	m_TexComment;
	CString	m_TexValue;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDialogSetValue)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	int IsType_Strands();
	BOOL IsType_Ident();
	BOOL ConvertToTex();

	//{{AFX_MSG(CDialogSetValue)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDialogSaveFile dialog

class CDialogSaveFile : public CDialog
{
// Construction
public:
	CDialogSaveFile(CWnd* pParent = NULL);  

// Dialog Data
	//{{AFX_DATA(CDialogSaveFile)
	enum { IDD = IDD_SAVE_MOLECULARFILE };
	int		m_Format;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDialogSaveFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDialogSaveFile)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDialogScript dialog

class CDialogScript : public CDialog
{
// Construction
public:
	CDialogScript(CWnd* pParent = NULL);   

// Dialog Data
	//{{AFX_DATA(CDialogScript)
	enum { IDD = IDD_SAVE_SCRIPT };
	int		m_Format;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CDialogScript)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDialogScript)
	afx_msg void OnClickMolscript();
	afx_msg void OnClickScript();
	afx_msg void OnSaveKinemage();
	afx_msg void OnClickMagedetail();
	afx_msg void OnClickPovray();
	afx_msg void OnClickVmrl();
	afx_msg void OnSavePhipsi();
	afx_msg void OnSaveRdf();
	afx_msg void OnSaveRpp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDialogImage dialog

class CDialogImage : public CDialog
{
// Construction
public:
	CDialogImage(CWnd* pParent = NULL); 

// Dialog Data
	//{{AFX_DATA(CDialogImage)
	enum { IDD = IDD_SAVE_IMAGE };
	int		m_Format;
	BOOL	m_CartoonFlag;
	int		m_SizeX;
	int		m_SizeY;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDialogImage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);  
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateVectors();

	//{{AFX_MSG(CDialogImage)
	afx_msg void OnImageEnablevectps();
	afx_msg void OnImageBmp();
	afx_msg void OnImageGif();
	afx_msg void OnImageEpsf();
	afx_msg void OnImageMonops();
	afx_msg void OnImagePpm();
	afx_msg void OnImagePpm2();
	afx_msg void OnImagePict();
	afx_msg void OnImageIris();
	afx_msg void OnImageSunrle();
	afx_msg void OnImageSun();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CEditData

class CEditData : public CRichEditCtrl
{
// Construction
public:
	CEditData();

// Attributes
public:
protected:
	BOOL VirtKeyCtrl;
	CFont SeqFont;
// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CEditData)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void SaveData( CString& str, CString& name );
	void InitDisplay();
	virtual ~CEditData();

protected:
	//{{AFX_MSG(CEditData)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CDialogEditMolecularFile dialog

class CDialogEditMolecularFile : public CDialog
{
// Construction
public:
	CDialogEditMolecularFile(CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CDialogEditMolecularFile)
	enum { IDD = IDD_MOLECULAR_FILE };
	CEditData	m_MolecularData;
	CString	m_MoleculeName;
	CString	m_FileFormat;
	//}}AFX_DATA

protected:

// Overrides
	//{{AFX_VIRTUAL(CDialogEditMolecularFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CDialogEditMolecularFile)
	virtual void OnOK();
	afx_msg void OnSave();
	afx_msg void OnProcess();
	afx_msg void OnExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CColorPicker

class CColorPicker : public CComboBox
{
// Construction
public:
	CColorPicker();
    virtual ~CColorPicker();

// Attributes
private:
	bool Initialized;
public:
	static COLORREF RasMolColors[];

// Operations
private:
	void Init();
public:
	COLORREF getSelectedColor();
    void setColor ( COLORREF col );
	//{{AFX_VIRTUAL(CComboColorPicker)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	//{{AFX_MSG(CColorPicker)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CDialogPreference dialog

class CDialogPreference : public CDialog
{
// Construction

public:
	CDialogPreference(CWnd* pParent = NULL);
    CColorPicker m_cmndlinetextbox;
    CColorPicker m_cmndlinebkgbox;

// Dialog Data
	//{{AFX_DATA(CDialogPreference)
	enum { IDD = IDD_PREFERENCES };
	int		m_ZoomEdit;
	BOOL	m_SequenceFalse;
	BOOL	m_SequenceTrue;
	CString	m_HelpSource;
	CString	m_DefaultRepres;
	BOOL	m_coord_mol;
	BOOL	m_coord_world;
	BOOL	m_coord_abs;
	BOOL	m_coord_screen;
	//}}AFX_DATA

public:
// Overrides
	//{{AFX_VIRTUAL(CDialogPreference)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDialogPreference)
	afx_msg void OnMoleculeSeqFalse();
	afx_msg void OnMoleculeSeqTrue();
	afx_msg void OnBrowseHelp();
	afx_msg void OnBrowseDefaultRepres();
	afx_msg void OnCoordAbs();
	afx_msg void OnCoordMol();
	afx_msg void OnCoordWorld();
	afx_msg void OnCoordScreen();
	afx_msg void OnSelchangePrefsCmdcol();
	afx_msg void OnSelchangePrefsCmdcolbkg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDialogLabel dialog

class CDialogLabel : public CDialog
{
// Construction
public:
	CDialogLabel(CWnd* pParent = NULL); 

// Dialog Data
	//{{AFX_DATA(CDialogLabel)
	enum { IDD = IDD_LABEL_SETTINGS };
	int		m_nFontSize;
	CString	m_sLabelFormat;
	int		m_FontStroke;
	int		m_FontFS;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CDialogLabel)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CDialogLabel)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDialogSelect dialog

class CDialogSelect : public CDialog
{
// Construction
public:
	CDialogSelect(CWnd* pParent = NULL);  

	int m_nCommand;
// Dialog Data
	//{{AFX_DATA(CDialogSelect)
	enum { IDD = IDD_SELECT_EXPRESSION };
	CEdit	m_EditSelection;
	CString	m_sExpr;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDialogSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolBar    m_DlgToolBar;

	//{{AFX_MSG(CDialogSelect)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewSelection();
	afx_msg void OnAddSelection();
	afx_msg void OnRemoveSelection();
	afx_msg void OnExtractSelection();
	afx_msg void OnExcludeSelection();
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDialogCommand dialog

class CDialogCommand : public CDialog
{
// Construction
public:
	CDialogCommand(CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CDialogCommand)
	enum { IDD = IDD_DIALOG_COMMAND };
	CEdit	m_EditCommand;
	CString	m_Command;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDialogCommand)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDialogCommand)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDialogRasMol dialog

#define DLG_INFO	1
#define DLG_SEQ		2
#define DLG_PHIPSI	3
#define DLG_RPP		4
#define DLG_COOR	5
#define DLG_CHN		6
#define DLG_GRP		7
#define DLG_ATM		8
#define DLG_SYM		9
#define DLG_WORLD   10

class CDialogRasMol : public CDialog
{
// Construction
public:
	CDialogRasMol(CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CDialogRasMol)
	enum { IDD = IDD_DIALOG_SHOW };
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDialogRasMol)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_ShowType;

protected:
	CEditData m_Commandline;

	//{{AFX_MSG(CDialogRasMol)
	virtual BOOL OnInitDialog();
	afx_msg void OnSave();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDialogChangeLightSource dialog

class CDialogChangeLightSource : public CDialog
{
// Construction
public:
	CDialogChangeLightSource(CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CDialogChangeLightSource)
	enum { IDD = IDD_DIALOG_LIGHTSOURCE };
	CSpinButtonCtrl	m_lightx;
	CSpinButtonCtrl	m_lighty;
	CSpinButtonCtrl	m_lightz;
	int		m_lix;
	int		m_liy;
	int		m_liz;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CDialogChangeLightSource)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	void ExecuteCommand();

	//{{AFX_MSG(CDialogChangeLightSource)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLightDefault();
	afx_msg void OnApplyNow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDialogLoadPrefs dialog

class CDialogLoadPrefs : public CDialog
{
// Construction
public:
	CDialogLoadPrefs(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogLoadPrefs)
	enum { IDD = IDD_LOADPREFS };
	CString	m_PrefsPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogLoadPrefs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogLoadPrefs)
	afx_msg void OnBrowseLoadprefs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DIALOG_H)
