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
// mainfrm.h 
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H_INCLUDED_)
#define AFX_MAINFRM_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define		SELECTION_EMPTY		0
#define		SELECTION_NEW		1
#define		SELECTION_ADD		2
#define		SELECTION_REMOVE	3
#define		SELECTION_EXTRACT	4
#define		SELECTION_EXCLUDE	5

#include "pannel.h"
#include "selectbar.h"
#include "colorbar.h"
#include "cmdwindow.h"
#include "scriptbar.h"


/////////////////////////////////////////////////////////////////////////////
// CMainFrame mainframe

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:	
	CToolBar	m_wndMainBar;
	CStatusBar	m_wndStatusBar;
	int			m_WndRestoreInit;
	CPannelBar  m_wndCommandBar;
	CSelectBar	m_wndSelectBar;
	CScriptBar	m_wndScriptBar;
	CColorBar	m_wndColorBar;
	CString		m_SelectString;
	COLORREF	m_crColour;
    int         OutputLock;

    // This string is to capture output for "show" command dialog
	CString		m_szCommandText;
	// Christian DUQUÉ - Juin 2003 - Fenêtre d'écho
	CmdWindow    m_wndCmdWindow;


// Operations
public:
	virtual ~CMainFrame();
	void SetSelection( int nMode, CString& sExpression);

	// Overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void ActivateCommandBar();
	void DesactivateCommandBar();
	CRasView* GetActiveRasView();
	CRect GetScreenClientCoor();
	void EnableMenus( int flag );

protected:

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand( UINT nID, LPARAM lParam );	
	afx_msg void OnUpdateViewCommandBar(CCmdUI* pCmdUI);
	afx_msg void OnViewCommandBar();
	afx_msg LRESULT OnChangeColor(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelectionNew();
	afx_msg void OnSelectionAdd();
	afx_msg void OnSelectionRemove();
	afx_msg void OnSelectionExtract();
	afx_msg void OnSelectionExclude();
	afx_msg void OnUpdateSelectionNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectionAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectionRemove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectionExtract(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectionExclude(CCmdUI* pCmdUI);
	afx_msg void OnViewSelectbar();
	afx_msg void OnUpdateViewSelectbar(CCmdUI* pCmdUI);
	afx_msg void OnViewScriptbar();
	afx_msg void OnUpdateViewScriptbar(CCmdUI* pCmdUI);
	afx_msg void OnViewColorbar();
	afx_msg void OnUpdateViewColorbar(CCmdUI* pCmdUI);
	afx_msg void OnViewCommandLine();
	afx_msg void OnUpdateViewCommandLine(CCmdUI* pCmdUI);
	afx_msg void OnEchoCommandLine();
	afx_msg void OnUpdateEchoCommandLine(CCmdUI* pCmdUI);
	afx_msg void OnRepresentation();
	afx_msg void OnUpdateRepresentation(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateViewStatusBar(CCmdUI* pCmdUI);
	afx_msg void OnFilePreferences();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnViewMenus();
	afx_msg void OnViewMainbar();
	afx_msg void OnUpdateViewMainbar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelpRasTop();
	afx_msg void OnUpdateIndicatorChain(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorGroup(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorAtom(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorCoor(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorXCoor(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorYCoor(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorZCoor(CCmdUI *pCmdUI);
	afx_msg void OnPickAtom();
	afx_msg void OnUpdatePickAtom(CCmdUI* pCmdUI);
	afx_msg void OnPickGroup();
	afx_msg void OnUpdatePickGroup(CCmdUI* pCmdUI);
	afx_msg void OnPickChain();
	afx_msg void OnUpdatePickChain(CCmdUI* pCmdUI);
	afx_msg void OnPickMolset();
	afx_msg void OnUpdatePickMolset(CCmdUI* pCmdUI);
	afx_msg void OnPickCentre();
	afx_msg void OnUpdatePickCentre(CCmdUI* pCmdUI);
	afx_msg void OnPickBond();
	afx_msg void OnUpdatePickBond(CCmdUI* pCmdUI);
	afx_msg void OnPickRotbond();
	afx_msg void OnUpdatePickRotbond(CCmdUI* pCmdUI);
	afx_msg void OnPickMonitor();
	afx_msg void OnUpdatePickMonitor(CCmdUI* pCmdUI);
	afx_msg void OnPickLabel();
	afx_msg void OnUpdatePickLabel(CCmdUI* pCmdUI);
	afx_msg void OnPickDistance();
	afx_msg void OnUpdatePickDistance(CCmdUI* pCmdUI);
	afx_msg void OnPickAngle();
	afx_msg void OnUpdatePickAngle(CCmdUI* pCmdUI);
	afx_msg void OnPickTorsion();
	afx_msg void OnUpdatePickTorsion(CCmdUI* pCmdUI);
	afx_msg void OnPickLight();
	afx_msg void OnUpdatePickLight(CCmdUI* pCmdUI);
	afx_msg void OnChangeCoordinates();
	afx_msg void OnUpdateChangeCoordinates(CCmdUI* pCmdUI);
	//}}AFX_MSG

protected:
	afx_msg LRESULT UpdateAllViews(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCommandColor(UINT nID);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_MAINFRM_H_INCLUDED_)
