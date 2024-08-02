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
// mainfrm.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "RasTop.h"
#include "rasdoc.h"
#include "rasview.h"
#include "mainfrm.h"
#include "dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// Send character stream to VS Debug screen

void SendforDebug( char* ptr) {
#ifdef _DEBUG
    TRACE0(ptr);
    TRACE0("\n");
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Reset user-defined set Box

void ResetUserSetBox( void )
{	GETFRAME
	pFrame->m_wndSelectBar.ResetBox(true);
}


/////////////////////////////////////////////////////////////////////////////
//Writing routines to infoline

void WriteChar(int ch)
{	
	static char obuffer[4096];
    static char *och = obuffer;
	static int i = 0;
    static int on = 0;

    static CString Templine = "";
    static char buffer[256];
    static char *pch = buffer;
    static int j = 0;

    GETFRAME

	if( pFrame->OutputLock==1 )
	{	
        // End of Transmission
        if( ch == EOF )
        {   *och = '\0';
            pFrame->m_szCommandText += obuffer;
            och = obuffer;
            i = 0;
            on = 0;
            pFrame->OutputLock = 0;
            InvalidateCmndLine();
            pFrame->m_wndCmdWindow.EcrisDansEcho(pFrame->m_szCommandText);

        // Initialize
        } else if( ch == '\0' )
        {   return;
        } else if( i==0 && !on )
        {   pFrame->m_szCommandText = "";
            och = obuffer;
            *och++ = ch;
            i++;
            on = 1;

        // Record
        } else
        {   if( ch=='\n' )
            {   *och++ = '\r';
                i++;
            }
            *och++ = ch;
            i++;
            if( i >= 4093)  // One extra in case \r
            {   *och = '\0';
                pFrame->m_szCommandText += obuffer;
                och = obuffer;
                i = 0;
            }
        }
        return;
   	}


    if (ch>=0 && ch <=255)  // No filter to local characters
        if ( ch<32 || ch>126 )
	        if( ch!=0x07 && ch!=0x00 && ch!=0x0D && ch!=0x0A && ch!=0x08 )
		        return;

    switch( ch )
    {	case(0x00): // Flush
                    if( CommandActive && j )
                    {   *pch = '\0';
                        Templine = buffer;
                        pFrame->m_wndCmdWindow.EcrisDansEcho(Templine);
                        pch = buffer;
                        j = 0;
                    }
                    break;

        case(0x07): MessageBeep(0);
		 	        break;
                
        case(0x0D): break;
        case(0x0A):	*pch++ = '\r'; 
                    *pch++ = ch;
			        *pch = '\0';
			        Templine = buffer;
                    if( !CommandActive && !pFrame->OutputLock )
			            pFrame->m_wndCommandBar.OnAddInfoline(Templine);
                    // Christian DUQUÉ - Juin 2003 - Fenêtre d'écho
			        pFrame->m_wndCmdWindow.EcrisDansEcho(Templine);

                    pch = buffer;
                    j = 0;
			        break;  

        default:	*pch++ = ch;
                    j++;
                    if( j==79 )
                    {   *pch++ = '\r';
                        *pch++ = '\n';
                        *pch = '\0';
                        Templine = buffer;
                        if( !CommandActive && !pFrame->OutputLock )
			                pFrame->m_wndCommandBar.OnAddInfoline(Templine);
			            pFrame->m_wndCmdWindow.EcrisDansEcho(Templine);
                        pch = buffer;
                        j = 0;
                    }
    } 
} 

void WriteString( char *ptr )
{	while( *ptr )
		WriteChar( *ptr++ );
    WriteChar('\0');    // End transmission
}



/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND( )
	ON_UPDATE_COMMAND_UI(ID_COMMANDVIEW, OnUpdateViewCommandBar)
	ON_COMMAND(ID_COMMANDVIEW, OnViewCommandBar)
	ON_MESSAGE(WM_COLOR, OnChangeColor)
	ON_COMMAND(ID_SELECTION_NEW, OnSelectionNew)
	ON_COMMAND(ID_SELECTION_ADD, OnSelectionAdd)
	ON_COMMAND(ID_SELECTION_REMOVE, OnSelectionRemove)
	ON_COMMAND(ID_SELECTION_EXTRACT, OnSelectionExtract)
	ON_COMMAND(ID_SELECTION_EXCLUDE, OnSelectionExclude)
	ON_UPDATE_COMMAND_UI(ID_SELECTION_NEW, OnUpdateSelectionNew)
	ON_UPDATE_COMMAND_UI(ID_SELECTION_ADD, OnUpdateSelectionAdd)
	ON_UPDATE_COMMAND_UI(ID_SELECTION_REMOVE, OnUpdateSelectionRemove)
	ON_UPDATE_COMMAND_UI(ID_SELECTION_EXTRACT, OnUpdateSelectionExtract)
	ON_UPDATE_COMMAND_UI(ID_SELECTION_EXCLUDE, OnUpdateSelectionExclude)
	ON_COMMAND(ID_VIEW_SELECTBAR, OnViewSelectbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELECTBAR, OnUpdateViewSelectbar)
	ON_COMMAND(ID_VIEW_SCRIPTBAR, OnViewScriptbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCRIPTBAR, OnUpdateViewScriptbar)
	ON_COMMAND(ID_VIEW_COLORBAR, OnViewColorbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLORBAR, OnUpdateViewColorbar)
	ON_COMMAND(ID_VIEW_COMMANDLINE, OnViewCommandLine)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMMANDLINE, OnUpdateViewCommandLine)
	ON_COMMAND(ID_COMMANDLINE_ECHO, OnEchoCommandLine)
	ON_UPDATE_COMMAND_UI(ID_COMMANDLINE_ECHO, OnUpdateEchoCommandLine)
	ON_COMMAND(ID_REPRESENTATION, OnRepresentation)
	ON_UPDATE_COMMAND_UI(ID_REPRESENTATION, OnUpdateRepresentation)
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)
	ON_COMMAND(ID_FILE_PREFERENCES, OnFilePreferences)
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_VIEW_MENU, OnViewMenus)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewMainbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewMainbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MENU, OnUpdateViewMenu)
	ON_WM_HELPINFO()
	ON_COMMAND(ID_HELP_RASMOL, OnHelpRasTop)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CHAIN, OnUpdateIndicatorChain)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GROUP, OnUpdateIndicatorGroup)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ATOM, OnUpdateIndicatorAtom)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COOR_NAME, OnUpdateIndicatorCoor)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_XCOOR, OnUpdateIndicatorXCoor)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_YCOOR, OnUpdateIndicatorYCoor)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZCOOR, OnUpdateIndicatorZCoor)
	ON_COMMAND(ID_PICK_ATOM, OnPickAtom)
	ON_UPDATE_COMMAND_UI(ID_PICK_ATOM, OnUpdatePickAtom)
	ON_COMMAND(ID_PICK_GROUP, OnPickGroup)
	ON_UPDATE_COMMAND_UI(ID_PICK_GROUP, OnUpdatePickGroup)
	ON_COMMAND(ID_PICK_CHAIN, OnPickChain)
	ON_UPDATE_COMMAND_UI(ID_PICK_CHAIN, OnUpdatePickChain)
	ON_COMMAND(ID_PICK_MOLSET, OnPickMolset)
	ON_UPDATE_COMMAND_UI(ID_PICK_MOLSET, OnUpdatePickMolset)
	ON_COMMAND(ID_PICK_CENTRE, OnPickCentre)
	ON_UPDATE_COMMAND_UI(ID_PICK_CENTRE, OnUpdatePickCentre)
	ON_COMMAND(ID_PICK_BOND, OnPickBond)
	ON_UPDATE_COMMAND_UI(ID_PICK_BOND, OnUpdatePickBond)
	ON_COMMAND(ID_PICK_ROTBOND, OnPickRotbond)
	ON_UPDATE_COMMAND_UI(ID_PICK_ROTBOND, OnUpdatePickRotbond)
	ON_COMMAND(ID_PICK_MONITOR, OnPickMonitor)
	ON_UPDATE_COMMAND_UI(ID_PICK_MONITOR, OnUpdatePickMonitor)
	ON_COMMAND(ID_PICK_LABEL, OnPickLabel)
	ON_UPDATE_COMMAND_UI(ID_PICK_LABEL, OnUpdatePickLabel)
	ON_COMMAND(ID_PICK_DISTANCE, OnPickDistance)
	ON_UPDATE_COMMAND_UI(ID_PICK_DISTANCE, OnUpdatePickDistance)
	ON_COMMAND(ID_PICK_ANGLE, OnPickAngle)
	ON_UPDATE_COMMAND_UI(ID_PICK_ANGLE, OnUpdatePickAngle)
	ON_COMMAND(ID_PICK_TORSION, OnPickTorsion)
	ON_UPDATE_COMMAND_UI(ID_PICK_TORSION, OnUpdatePickTorsion)
	ON_COMMAND(ID_PICK_LIGHT, OnPickLight)
	ON_UPDATE_COMMAND_UI(ID_PICK_LIGHT, OnUpdatePickLight)
	ON_COMMAND(ID_COORD_CHANGE, OnChangeCoordinates)
	ON_UPDATE_COMMAND_UI(ID_COORD_CHANGE, OnUpdateChangeCoordinates)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_UPDATERASVIEW, UpdateAllViews)
	ON_COMMAND_RANGE(ID_COLOR, ID_COLOR+MAX_COLORS, OnCommandColor)

END_MESSAGE_MAP()

static UINT indicators[] =
{	/*ID_SEPARATOR,*/
	ID_INDICATOR_CHAIN_NAME,
	ID_INDICATOR_CHAIN,
	ID_INDICATOR_GROUP_NAME,
	ID_INDICATOR_GROUP,
	ID_INDICATOR_ATOM_NAME,
	ID_INDICATOR_ATOM,
    ID_INDICATOR_XCOOR,
    ID_INDICATOR_YCOOR,
    ID_INDICATOR_ZCOOR,
    ID_INDICATOR_COOR_NAME,
	ID_INDICATOR_NULL
};

CMainFrame::CMainFrame()
{	
	m_WndRestoreInit = 1;
	m_SelectString = "";
	m_szCommandText = "";
    OutputLock = 0;
}

CMainFrame::~CMainFrame()	{}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{	cs.cx = 750;
	cs.cy = 550;
	return CMDIFrameWnd::PreCreateWindow(cs);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{	CString s;

	if( CMDIFrameWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	EnableDocking(CBRS_ALIGN_TOP);

	//Toolbar
	if( !m_wndMainBar.Create(this)||!m_wndMainBar.LoadToolBar(IDR_MAINFRAME) )
	{	TRACE0("Failed to create toolbar\n");
		return -1; 
	}
	m_wndMainBar.SetBarStyle(m_wndMainBar.GetBarStyle()|
		CBRS_TOOLTIPS|CBRS_FLYBY |CBRS_GRIPPER);
	m_wndMainBar.ModifyStyle(0,TBSTYLE_FLAT);
    s.LoadString(IDS_LOCAL_MAINWND);
	m_wndMainBar.SetWindowText(_T(s));
	m_wndMainBar.EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_wndMainBar,AFX_IDW_DOCKBAR_TOP);

	//SelectBar
	if( !m_wndSelectBar.Init(this) )
	{	TRACE0("Failed to create selectbar\n");
		return -1; 
	}
    s.LoadString(IDS_LOCAL_SELWND);
	m_wndSelectBar.SetWindowText(_T(s));
	m_wndSelectBar.EnableDocking(CBRS_ALIGN_TOP );
	DockControlBar(&m_wndSelectBar,AFX_IDW_DOCKBAR_TOP);

    //m_wndScriptBar
	if( !m_wndScriptBar.Init(this) )
	{	TRACE0("Failed to create selectbar\n");
		return -1; 
	}
    s.LoadString(IDS_LOCAL_SCRWND);
	m_wndScriptBar.SetWindowText(_T(s));
	m_wndScriptBar.EnableDocking(CBRS_ALIGN_TOP );
	DockControlBar(&m_wndScriptBar,AFX_IDW_DOCKBAR_TOP);

	//Status bar
	if( !m_wndStatusBar.Create(this, WS_CHILD | CBRS_BOTTOM) ||
		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)) )
	{	TRACE0("Failed to create status bar\n");
		return -1;
	}

	int cx, panex; UINT st, id;
	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_CHAIN_NAME);
	if( panex!=-1 )
		m_wndStatusBar.SetPaneStyle( panex, SBPS_NOBORDERS);

	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_CHAIN);
	if( panex!=-1 )
	{	m_wndStatusBar.GetPaneInfo( panex, id, st, cx );
		m_wndStatusBar.SetPaneInfo( panex, id, st, 140 );
	}
	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_GROUP_NAME);
	if( panex!=-1 )
		m_wndStatusBar.SetPaneStyle( panex, SBPS_NOBORDERS);

	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_GROUP);
	if( panex!=-1 )
	{	m_wndStatusBar.GetPaneInfo( panex, id, st, cx );
		m_wndStatusBar.SetPaneInfo( panex, id, st, 100 );
	}
	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_ATOM_NAME);
	if( panex!=-1 )
		m_wndStatusBar.SetPaneStyle( panex, SBPS_NOBORDERS);

	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_ATOM);
	if( panex!=-1 )
	{	m_wndStatusBar.GetPaneInfo( panex, id, st, cx );
		m_wndStatusBar.SetPaneInfo( panex, id, st, 120 );
	}

	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_XCOOR);
	if( panex!=-1 )
	{	m_wndStatusBar.GetPaneInfo( panex, id, st, cx );
		m_wndStatusBar.SetPaneInfo( panex, id, st, 55 );
	}

	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_YCOOR);
	if( panex!=-1 )
	{	m_wndStatusBar.GetPaneInfo( panex, id, st, cx );
		m_wndStatusBar.SetPaneInfo( panex, id, st, 55 );
	}

	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_ZCOOR);
	if( panex!=-1 )
	{	m_wndStatusBar.GetPaneInfo( panex, id, st, cx );
		m_wndStatusBar.SetPaneInfo( panex, id, st, 55 );
	}

	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_COOR_NAME);
	if( panex!=-1 )
	{	m_wndStatusBar.SetPaneStyle( panex, SBPS_NOBORDERS);
        m_wndStatusBar.GetPaneInfo( panex, id, st, cx );
		m_wndStatusBar.SetPaneInfo( panex, id, st, 80 );
	}

	panex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_NULL);
	if( panex!=-1 )
	{	m_wndStatusBar.GetPaneInfo( panex, id, st, cx );
		m_wndStatusBar.SetPaneInfo( panex, id, SBPS_DISABLED|SBPS_NOBORDERS,10);
	}

	//Color bar
	if( !m_wndColorBar.Init(this) )
	{	TRACE0("Failed to create color toolbar\n");
		return -1; 
	}

	CPoint point(550,150);
	FloatControlBar(&m_wndColorBar,point,CBRS_ALIGN_TOP );

    
	/* Christian DUQUÉ - Juin 2003 - Fenêtre d'écho */
	if( !m_wndCmdWindow.Create(IDD_DIALOG_CMDWND, this) )
	{	TRACE0("Impossible de creer la fenetre d'echo - commandes\n");
		return -1; 
	}
	

	// Command bar
	long sizex = 800;

	CClientDC dc(GetDesktopWindow());
	sizex = dc.GetDeviceCaps(HORZRES);

	UINT IDPannel;
	if( sizex >= 1024 )
		IDPannel = IDD_COMMANDVIEW_BIG;
	else
		IDPannel = IDD_COMMANDVIEW;

	if( !m_wndCommandBar.Create(this, IDPannel,
		CBRS_BOTTOM|CBRS_TOOLTIPS|CBRS_FLYBY, IDPannel) )
	{	TRACE0("Failed to create pannel\n");
		return -1; 
	}

	/* Create Font */
	long lgpix = 120;
	lgpix = dc.GetDeviceCaps(LOGPIXELSX);

	int ls = 1;
	if( lgpix > 120 )
	    ls = 3;

    CDC pDisplay;
	if( !pDisplay.CreateCompatibleDC(NULL) )
		return -1;

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfWeight = FW_NORMAL;
	logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	lstrcpy( logFont.lfFaceName, _T("Arial") );

	int height = -MulDiv(7+ls, pDisplay.GetDeviceCaps(LOGPIXELSY), 72);
	logFont.lfHeight = height;
	if( !m_wndCommandBar.m_FontPannel.CreateFontIndirect(&logFont) )
		return -1;

	height = -MulDiv(5+ls, pDisplay.GetDeviceCaps(LOGPIXELSY), 72);
	logFont.lfHeight = height;
	if( !m_wndCommandBar.m_FontBuddy.CreateFontIndirect(&logFont) )
		return -1;

	if(!m_wndCommandBar.OnInitDialogBar() )
	{	TRACE0("Failed to init CommandBar\n");
		return -1; 
	}

	return 0;
}

void CMainFrame::OnSysCommand( UINT nID, LPARAM lParam )
{	
	CMDIFrameWnd::OnSysCommand( nID, lParam );

	static BOOL IconeFlag = false;

	//Allow decent restoring if initially maximized
    if( m_WndRestoreInit && WindowMaximized && ((nID & 0xFFF0) == SC_RESTORE) && !IconeFlag )
    {   int x = ::GetSystemMetrics(SM_CXMAXIMIZED);
	    int y = ::GetSystemMetrics(SM_CYMAXIMIZED);
        int a = x/7;
        int b = y/7;
	    x = 2*x/3;
	    y = 2*y/3;
	    m_WndRestoreInit = 0;
		MoveWindow(a,b,x,y,TRUE );
    }

	if( (nID & 0xFFF0) == SC_MINIMIZE )
    {   IconeFlag = TRUE;
    } else
        IconeFlag = FALSE;

	//Stop spinmode on screensaver
	if( (nID & 0xFFF0) == SC_SCREENSAVE || (nID & 0xFFF0) == SC_MINIMIZE ) 
		m_wndCommandBar.ResetSpinMode();

    // Record window placement
    if( ((nID & 0xFFF0) == SC_MAXIMIZE ) ||
        ((nID & 0xFFF0) == SC_MINIMIZE ) ||
        ((nID & 0xFFF0) == SC_RESTORE )  ||
        ((nID & 0xFFF0) == SC_MOVE )     ||
        ((nID & 0xFFF0) == SC_SIZE)  )
    {   RecordRasMolWindow();
        PrefsModified = true;
        m_WndRestoreInit = 0;
        
    }

}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
    POINT p;
	p.x = ::GetSystemMetrics(SM_CXMAXIMIZED);
	p.y = ::GetSystemMetrics(SM_CYMAXIMIZED);
	lpMMI->ptMaxSize = p;
	p.x = - ::GetSystemMetrics(SM_CXSIZEFRAME);
	p.y = - ::GetSystemMetrics(SM_CYSIZEFRAME);
    lpMMI->ptMaxPosition = p;
	
	CMDIFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::OnClose() 
{	
    if( ClipBoardSize > 1000000 )
    {   CString s;
        s.LoadString(IDS_LOCAL_CLIP);
        if( AfxMessageBox(_T(s), MB_YESNO) == IDYES )
        {   ::OpenClipboard(NULL);
            ::EmptyClipboard();
            ::CloseClipboard();
        }
    }

    RecordColorWindow();

	CMDIFrameWnd::OnClose();
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
//Bars

/* ToolBar */
void CMainFrame::OnViewMainbar() 
{	BOOL bView = (m_wndMainBar.GetStyle() & WS_VISIBLE);
	ShowControlBar(&m_wndMainBar, !bView, false);
    WindowMain  = !bView;
    PrefsModified = true;
	if( !bView )
		DisableMenu = false;
}

void CMainFrame::OnUpdateViewMainbar(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( (m_wndMainBar.GetStyle() & WS_VISIBLE) != 0 );	}


/* SelectBar */
void CMainFrame::OnViewSelectbar() 
{	BOOL bView = (m_wndSelectBar.GetStyle() & WS_VISIBLE);
	ShowControlBar(&m_wndSelectBar, !bView, false);
    WindowSelection  = !bView;
    PrefsModified = true;
	if( !bView )
		DisableMenu = false;
}

void CMainFrame::OnUpdateViewSelectbar(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( (m_wndSelectBar.GetStyle() & WS_VISIBLE) != 0 );	}


/* ScriptBar */
void CMainFrame::OnViewScriptbar() 
{	BOOL bView = (m_wndScriptBar.GetStyle() & WS_VISIBLE);
	ShowControlBar(&m_wndScriptBar, !bView, false);
    WindowScripting  = !bView;
    PrefsModified = true;
	if( !bView )
		DisableMenu = false;
}

void CMainFrame::OnUpdateViewScriptbar(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( (m_wndScriptBar.GetStyle() & WS_VISIBLE) != 0 );	}

/* ColorBar */
void CMainFrame::OnViewColorbar() 
{	BOOL bView = (m_wndColorBar.GetStyle() & WS_VISIBLE);
	ShowControlBar(&m_wndColorBar, !bView, false);
    WindowColour  = !bView;
    PrefsModified = true;
	if( !bView )
		DisableMenu = false;
}

void CMainFrame::OnUpdateViewColorbar(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( (m_wndColorBar.GetStyle() & WS_VISIBLE) != 0 );	}


/* Command Line */
void CMainFrame::OnViewCommandLine() 
{	BOOL bView = (m_wndCmdWindow.GetStyle() & WS_VISIBLE);
    ShowCommandLine(!bView);
    WindowCommandLine  = !bView;
    PrefsModified = true;
	if( !bView )
		DisableMenu = false;
}

void CMainFrame::OnUpdateViewCommandLine(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( (m_wndCmdWindow.GetStyle() & WS_VISIBLE) != 0 );	}

void CMainFrame::OnEchoCommandLine() {
    GUItoCmdEcho = GUItoCmdEcho==0;

}

void CMainFrame::OnUpdateEchoCommandLine(CCmdUI* pCmdUI)
{	pCmdUI->SetCheck(GUItoCmdEcho!=0);	}

/* CommandBar */
void CMainFrame::OnViewCommandBar() 
{	BOOL bView = (m_wndCommandBar.GetStyle() & WS_VISIBLE);
	ShowControlBar(&m_wndCommandBar, !bView, false);
    WindowCommandPanel  = !bView;
    PrefsModified = true;
	if( !bView )
		DisableMenu = false;
}

void CMainFrame::ActivateCommandBar()
{	BOOL bView = (m_wndCommandBar.GetStyle() & WS_VISIBLE);
    if( !bView )
    {   ShowControlBar(&m_wndCommandBar, true, false);	
        WindowCommandPanel  = true;
        PrefsModified = true;
	    DisableMenu = false;
    }
}

void CMainFrame::DesactivateCommandBar()
{	BOOL bView = (m_wndCommandBar.GetStyle() & WS_VISIBLE);
	if( bView )
    {   ShowControlBar(&m_wndCommandBar, false, false);
        WindowCommandPanel  = false;
        PrefsModified = true;
    }
}

void CMainFrame::OnUpdateViewCommandBar(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( (m_wndCommandBar.GetStyle() & WS_VISIBLE) != 0 );	}


/* StatusBar */
void CMainFrame::OnViewStatusBar() 
{	BOOL bView = (m_wndStatusBar.GetStyle() & WS_VISIBLE);
	ShowControlBar(&m_wndStatusBar, !bView, false);
    WindowStatus  = !bView;
    PrefsModified = true;
	if( !bView )
		DisableMenu = false;
}

void CMainFrame::OnUpdateViewStatusBar(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( (m_wndStatusBar.GetStyle() & WS_VISIBLE) != 0 );	}



void CMainFrame::OnViewMenus() 
{	static BOOL MainBarFlag = true;
	static BOOL SelectBarFlag = true;
	static BOOL CommandBarFlag = true;
	static BOOL StatusBarFlag = true;
    static BOOL ColorBarFlag = true;
    static BOOL CommandLineFlag = true;
    static BOOL ScriptBarFlag = true;

	//DisableMenu always False at beginning
	DisableMenu = !DisableMenu;

	if( DisableMenu )
	{	//memorize status
		MainBarFlag = m_wndMainBar.GetStyle() & WS_VISIBLE;
		SelectBarFlag = m_wndSelectBar.GetStyle() & WS_VISIBLE;
		CommandBarFlag = m_wndCommandBar.GetStyle() & WS_VISIBLE;
		StatusBarFlag = m_wndStatusBar.GetStyle() & WS_VISIBLE;
		ColorBarFlag = m_wndColorBar.GetStyle() & WS_VISIBLE;
        CommandLineFlag = m_wndCmdWindow.GetStyle() & WS_VISIBLE;
        ScriptBarFlag = m_wndScriptBar.GetStyle() & WS_VISIBLE;

		if( MainBarFlag )		ShowControlBar(&m_wndMainBar, false, false);
		if( SelectBarFlag )		ShowControlBar(&m_wndSelectBar, false, false);
		if( CommandBarFlag )	ShowControlBar(&m_wndCommandBar, false, false);
		if( StatusBarFlag )		ShowControlBar(&m_wndStatusBar, false, false);
		if( ColorBarFlag )		ShowControlBar(&m_wndColorBar, false, false);
        if( CommandLineFlag )   m_wndCmdWindow.ShowWindow(SW_HIDE);
		if( ScriptBarFlag )		ShowControlBar(&m_wndScriptBar, false, false);
	} else
	{	if( MainBarFlag )		ShowControlBar(&m_wndMainBar, true, false);
		if( SelectBarFlag )		ShowControlBar(&m_wndSelectBar, true, false);
		if( CommandBarFlag && GetActiveRasView() )
								ShowControlBar(&m_wndCommandBar, true, false);
		if( StatusBarFlag )		ShowControlBar(&m_wndStatusBar, true, false);
		if( ColorBarFlag )		ShowControlBar(&m_wndColorBar, true, false);
        if( CommandLineFlag )   m_wndCmdWindow.ShowWindow(SW_SHOWNORMAL);
		if( ScriptBarFlag )		ShowControlBar(&m_wndScriptBar, true, false);
	}

	RecalcLayout();
}

void CMainFrame::EnableMenus( int flag )
{	if( DisableMenu==flag )
		OnViewMenus();
}

void CMainFrame::OnUpdateViewMenu(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( DisableMenu );	}


/*=======================*/
/* Status Bar Indicators */
/*=======================*/

void CMainFrame::OnUpdateIndicatorChain(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();

    CString strChain,s;
	strChain = "";

	if( !Database || !QChain || !QMolSet || !QAtom )
	{	pCmdUI->SetText( strChain ); 
		return;
	}

	if( !QMolSet->visible && !DisplayMode )
		return;

	if( QMolSet->copynum!=1 )
		strChain.Format("%s #%d ",QMolSet->title,QMolSet->copynum);
	else
		strChain.Format("%s ",QMolSet->title);

	if( QChain->ident!=' ' )
    {	s.LoadString(IDS_LOCAL_CHAIN);
        strChain += s;
		strChain += QChain->ident;
	}

    CString strModel;
	strModel = "";
	if( QAtom->model )
		strModel.Format(IDS_LOCAL_MODEL,QAtom->model);
    pCmdUI->SetText( strChain+strModel ); 
}

void CMainFrame::OnUpdateIndicatorGroup(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();

    CString strGroup;
	strGroup = "";

	if( !Database || !QGroup )
	{	pCmdUI->SetText( strGroup ); 
		return;
	}

	if( QMolSet && !QMolSet->visible && !DisplayMode )
		return;

	if (!(QGroup->serno == -9999))
	{	char *str = Residue[QGroup->refno];
		if( str[0]!=' ' )  strGroup += str[0];
        strGroup += str[1];
		strGroup += str[2];
	
		CString strSerno;
		strSerno.IsEmpty();
        strSerno.Format( " %d",QGroup->serno);
        if( !(QGroup->insert == ' ') && !(QGroup->insert=='\0') )
			strSerno += QGroup->insert;
		pCmdUI->SetText( strGroup+strSerno ); 
	}
}

void CMainFrame::OnUpdateIndicatorAtom(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();

    CString strAtom;
	strAtom = "";

	if( !Database || !QAtom )
	{	pCmdUI->SetText( strAtom ); 
		return;
	}

	if( QMolSet && !QMolSet->visible && !DisplayMode )
		return;
	
	char *str = ElemDesc[QAtom->refno];

    int i = 0;
    while( str[i]!='\0' )
    {   strAtom += str[i]; 
        i++;
    }

    if( !(QAtom->altl == ' '))
    {   strAtom += ';';
        strAtom += QAtom->altl;
	}
    CString strSerno;
	strSerno = "";
    strSerno.Format( " %d", QAtom->serno ); 
    pCmdUI->SetText( strAtom+strSerno ); 
}

void CMainFrame::OnUpdateIndicatorXCoor(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();

    CString strXcoor;
	strXcoor = "";
   
	if( !Database || !QAtom )
	{	pCmdUI->SetText( strXcoor ); 
		return;
	}

	if( QMolSet && !QMolSet->visible && !DisplayMode )
		return;
    strXcoor.Format( "x %.3f", QXCoor );
    pCmdUI->SetText( strXcoor ); 
}

void CMainFrame::OnUpdateIndicatorYCoor(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();

    CString strYcoor;
	strYcoor = "";
   
	if( !Database || !QAtom )
	{	pCmdUI->SetText( strYcoor ); 
		return;
	}

	if( QMolSet && !QMolSet->visible && !DisplayMode )
		return;
    strYcoor.Format( "y %.3f", QYCoor );
    pCmdUI->SetText( strYcoor ); 
}

void CMainFrame::OnUpdateIndicatorZCoor(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();

    CString strZcoor;
	strZcoor = "";
   
	if( !Database || !QAtom )
	{	pCmdUI->SetText( strZcoor ); 
		return;
	}

	if( QMolSet && !QMolSet->visible && !DisplayMode )
		return;
    strZcoor.Format( "z %.3f", QZCoor );
    pCmdUI->SetText( strZcoor ); 
}

void CMainFrame::OnUpdateIndicatorCoor(CCmdUI *pCmdUI)
{    pCmdUI->Enable();

    CString strCoor;
    if( SysCoor == MolCoor )
        strCoor.LoadString(IDS_LOCAL_COMOL);
    else if( SysCoor == WorldCoor )
        strCoor.LoadString(IDS_LOCAL_COWRLD);
    else if( SysCoor == ScreenCoor )
        strCoor.LoadString(IDS_LOCAL_COSCREEN);
    else if( SysCoor == AbsCoor )
        strCoor.LoadString(IDS_LOCAL_COABS);
    else
        strCoor = " ";
    pCmdUI->SetText( strCoor ); 
}

/*==================*/
/* Picking Handlers */
/*==================*/

void CMainFrame::OnPickAtom() 
{	if( PickMode==PickAtom )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickAtom);
}

void CMainFrame::OnUpdatePickAtom(CCmdUI* pCmdUI) 
{	if( Database==NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickAtom );
	}
}

void CMainFrame::OnPickGroup() 
{	if( PickMode==PickGroup )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickGroup);
}

void CMainFrame::OnUpdatePickGroup(CCmdUI* pCmdUI) 
{	if( Database==NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickGroup );
	}
}

void CMainFrame::OnPickChain() 
{	if( PickMode==PickChain )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickChain);
}

void CMainFrame::OnUpdatePickChain(CCmdUI* pCmdUI) 
{	if( Database==NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickChain );
	}
}

void CMainFrame::OnPickMolset() 
{	if( PickMode==PickMolSet )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickMolSet);
}

void CMainFrame::OnUpdatePickMolset(CCmdUI* pCmdUI) 
{	if( Database==NULL || (MoleculeList && (MoleculeList->next==NULL)) )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickMolSet );
	}
}

void CMainFrame::OnPickCentre() 
{	if( PickMode==PickCentr )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickCentr);
}

void CMainFrame::OnUpdatePickCentre(CCmdUI* pCmdUI) 
{	if( Database==NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickCentr );
	}
}

void CMainFrame::OnPickBond() 
{	if( PickMode==PickBond )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickBond);
}

void CMainFrame::OnUpdatePickBond(CCmdUI* pCmdUI) 
{	if( Database == NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickBond );
	}
}

void CMainFrame::OnPickRotbond() 
{	if( PickMode==PickRBond )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickRBond);
}

void CMainFrame::OnUpdatePickRotbond(CCmdUI* pCmdUI) 
{	if( Database == NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickRBond );
	}
}

void CMainFrame::OnPickMonitor() 
{	if( PickMode==PickMonit )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickMonit);
}

void CMainFrame::OnUpdatePickMonitor(CCmdUI* pCmdUI) 
{	if( Database == NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickMonit );
	}
}

void CMainFrame::OnPickLabel() 
{	if( PickMode==PickLabel )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickLabel);
}

void CMainFrame::OnUpdatePickLabel(CCmdUI* pCmdUI) 
{	if( Database==NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickLabel );
	}
}

void CMainFrame::OnPickDistance() 
{	if( PickMode==PickDist )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickDist);
}

void CMainFrame::OnUpdatePickDistance(CCmdUI* pCmdUI) 
{	if( Database==NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickDist );
	}
}

void CMainFrame::OnPickAngle() 
{	if( PickMode==PickAngle )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickAngle);
}

void CMainFrame::OnUpdatePickAngle(CCmdUI* pCmdUI) 
{	if( Database==NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickAngle );
	}
}

void CMainFrame::OnPickTorsion() 
{	if( PickMode==PickTorsn )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickTorsn);
}

void CMainFrame::OnUpdatePickTorsion(CCmdUI* pCmdUI) 
{	if( Database == NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickTorsn );
	}
}

void CMainFrame::OnPickLight() 
{	if( PickMode==PickLight )
		::SetPickMode(PickIdent);
	else
		::SetPickMode(PickLight);
}

void CMainFrame::OnUpdatePickLight(CCmdUI* pCmdUI) 
{	if( Database == NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck( PickMode==PickLight );
	}
}

void CMainFrame::OnChangeCoordinates() 
{	if( Database==NULL )
		return;
	CRasView* pView = GetActiveRasView();
	if( pView==NULL )
		return;

    if( SysCoor == MolCoor )
    {	pView->ExecuteCommand( DisplayOff, "set coordinate world\n" );
        InvalidateCmndLine();
        WriteString(MsgStrs(RTPWCoor));
    } else if( SysCoor == WorldCoor )
    {	pView->ExecuteCommand( DisplayOff, "set coordinate screen\n" );
        InvalidateCmndLine();
        WriteString(MsgStrs(RTPSCoor));
    } else if( SysCoor == ScreenCoor )
    {	pView->ExecuteCommand( DisplayOff, "set coordinate absolute\n" );
        InvalidateCmndLine();
        WriteString(MsgStrs(RTPACoor));
    } else if( SysCoor == AbsCoor )
    {	pView->ExecuteCommand( DisplayOff, "set coordinate molecule\n" );
        InvalidateCmndLine();
        WriteString(MsgStrs(RTPMCoor));
    }

    m_wndCommandBar.UpdatePannel();
}

void CMainFrame::OnUpdateChangeCoordinates(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(true);
    pCmdUI->SetCheck(false);
}

/////////////////////////////////////////////////////////////////////////////

/*Active RasView*/
CRasView* CMainFrame::GetActiveRasView()
{	CMDIChildWnd *pChild = (CMDIChildWnd *) GetActiveFrame();
	if ( pChild == NULL )
		return NULL;
	CRasView *pView = (CRasView *) pChild->GetActiveView();
	if( pView == NULL )
		return NULL;
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(CRasView)));
	return pView;
}

void CMainFrame::OnCommandColor(UINT nID)
{	m_wndColorBar.OnCommandHandler(nID);
}

LRESULT CMainFrame::OnChangeColor(WPARAM wParam, LPARAM lParam )
{	m_crColour = (COLORREF) wParam;
	return 0L;
}

/*Selections*/
void CMainFrame::SetSelection( int nMode, CString& sExpression)
{	if( Database==NULL )
		return;
	CRasView* pView = GetActiveRasView();
	if( pView==NULL )
		return;
	if( sExpression.IsEmpty() )
		return;

	switch(nMode)
	{ case(SELECTION_EMPTY)	:
	  case(SELECTION_NEW)	:	
		pView->ExecuteCommand( DisplayOn, "select (" + sExpression + ")\n" );
		break;
	  case(SELECTION_ADD)	:	
		pView->ExecuteCommand( DisplayOn, "select (" + sExpression + \
		") || selected\n" );
		break;
	  case(SELECTION_REMOVE):	
		pView->ExecuteCommand( DisplayOn, "select selected and not ("	+ \
		sExpression + ")\n" );
		break;
	  case(SELECTION_EXTRACT):	
		pView->ExecuteCommand( DisplayOn,"select (" + sExpression + \
		") && selected\n" );
		break;
	  case(SELECTION_EXCLUDE):		
		pView->ExecuteCommand( DisplayOn, 
							"select (" + sExpression + ") and not selected\n" );
		break;
	  default				:
		return;
	}
}

void CMainFrame::OnSelectionNew()
{	if( !Database ) return;
	if( m_SelectString.IsEmpty() )
		return;
	SetSelection( SELECTION_NEW, m_SelectString );
}

void CMainFrame::OnUpdateSelectionNew(CCmdUI* pCmdUI)
{	/*pCmdUI->Enable( Database != NULL );*/	}

void CMainFrame::OnSelectionAdd()
{	if( !Database ) return;
	if( m_SelectString.IsEmpty() )
		return;
	SetSelection( SELECTION_ADD, m_SelectString );
}

void CMainFrame::OnUpdateSelectionAdd(CCmdUI* pCmdUI)
{	/*pCmdUI->Enable( Database != NULL );*/	}

void CMainFrame::OnSelectionRemove()
{	if( !Database ) return;
	if( m_SelectString.IsEmpty() )
		return;
	SetSelection( SELECTION_REMOVE, m_SelectString );
}

void CMainFrame::OnUpdateSelectionRemove(CCmdUI* pCmdUI)
{	/*pCmdUI->Enable( Database != NULL );*/	}

void CMainFrame::OnSelectionExtract()
{	if( !Database ) return;
	if( m_SelectString.IsEmpty() )
		return;
	SetSelection( SELECTION_EXTRACT, m_SelectString );
}

void CMainFrame::OnUpdateSelectionExtract(CCmdUI* pCmdUI)
{	/*pCmdUI->Enable( Database!=NULL );*/	}

void CMainFrame::OnSelectionExclude()
{	if( !Database ) return;
	if( m_SelectString.IsEmpty() )
		return;
	SetSelection(SELECTION_EXCLUDE, m_SelectString);
}

void CMainFrame::OnUpdateSelectionExclude(CCmdUI* pCmdUI)
{	/*pCmdUI->Enable( Database!=NULL );*/	}


/*Preferences*/

void CMainFrame::OnFilePreferences() 
{	m_wndCommandBar.KillTimerPannel();
	CDialogPreference dlg;
	dlg.m_ZoomEdit = ClipBoardFactor;
	dlg.m_SequenceTrue = SeqFormat;
	dlg.m_SequenceFalse = !SeqFormat;
	dlg.m_coord_mol = (SysCoor==MolCoor);
	dlg.m_coord_world = (SysCoor==WorldCoor);
	dlg.m_coord_abs = (SysCoor==AbsCoor);
	dlg.m_coord_screen = (SysCoor==ScreenCoor);

	CString HelpSource = HelpHTMLSource;
	dlg.m_HelpSource = HelpSource;
	CString DefaultRepres = DefaultScriptName;
	dlg.m_DefaultRepres = DefaultRepres;

	if( dlg.DoModal()!=IDOK )
	{	m_wndCommandBar.SetTimerPannel();
		return;
	} 

	if( ClipBoardFactor != dlg.m_ZoomEdit )
	{	ClipBoardFactor = dlg.m_ZoomEdit;
		PrefsModified = true;
	}

	if( SeqFormat != dlg.m_SequenceTrue )
	{	SeqFormat = dlg.m_SequenceTrue;
		PrefsModified = true;
	}

    if(	dlg.m_coord_mol&(SysCoor!=MolCoor) )
    {   SysCoor = MolCoor;
        m_wndCommandBar.UpdatePannel();
		PrefsModified = true;
    } else if( dlg.m_coord_world&(SysCoor!=WorldCoor) )
    {   SysCoor = WorldCoor;
        m_wndCommandBar.UpdatePannel();
		PrefsModified = true;
    } else if( dlg.m_coord_screen&(SysCoor!=ScreenCoor) )
    {   SysCoor = ScreenCoor;
        m_wndCommandBar.UpdatePannel();
		PrefsModified = true;
    } else if( dlg.m_coord_abs&(SysCoor!=AbsCoor) )
    {   SysCoor = AbsCoor;
        m_wndCommandBar.UpdatePannel();
		PrefsModified = true;
    }

	if( !(DefaultRepres == dlg.m_DefaultRepres) )
	{	dlg.m_DefaultRepres.LockBuffer();
		strcpy(DefaultScriptName,dlg.m_DefaultRepres);
		dlg.m_DefaultRepres.UnlockBuffer();
		PrefsModified = true;
	}

	if( !(HelpSource == dlg.m_HelpSource) )
	{	dlg.m_HelpSource.LockBuffer();
		strcpy(HelpHTMLSource,dlg.m_HelpSource);
		dlg.m_HelpSource.UnlockBuffer();
		PrefsModified = true;
	}

	m_wndCommandBar.SetTimerPannel();
}

/* Default Representation */

void CMainFrame::OnRepresentation() {
    if(useDefaultScript)
        useDefaultScript = 0;
    else
        useDefaultScript = 1;

}

void CMainFrame::OnUpdateRepresentation(CCmdUI* pCmdUI)
{	pCmdUI->SetCheck(useDefaultScript!=0);	}


BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{	int nDoc = 0;
	CMDIChildWnd *pChild = (CMDIChildWnd *) GetActiveFrame();
	CRasDoc* pDoc = (CRasDoc *) pChild->GetActiveDocument( );
	if( pDoc != NULL )
	{	CDocTemplate* pDocTemp;
		pDocTemp = pDoc->GetDocTemplate( );
		POSITION posdoc = pDocTemp->GetFirstDocPosition( );
		while(posdoc != NULL)
		{	pDoc = (CRasDoc *)pDocTemp->GetNextDoc(posdoc);
			nDoc++;
		}
	}

	//0x0000ff00 = 65280 - allow document switch through the menu when pause
	BOOL b = 
		(int) LOWORD(wParam) >= 65280 && (int) LOWORD(wParam) <= (65280 + nDoc -1);
	//allow to switch command pannel if paused
	if( (int) LOWORD(wParam)==ID_COMMANDVIEW )
		b = true;
	//same for full screen
	if( (int) LOWORD(wParam)==ID_VIEW_MENU )
		b = true;

	BOOL c = IsPaused && !b && !( (int) HIWORD(wParam) );
	if( c )
	{	InvalidateCmndLine();
        WriteString(MsgStrs(RTPStopScr));
		return true;
	}
	
	return CMDIFrameWnd::OnCommand(wParam, lParam);
}

LRESULT CMainFrame::UpdateAllViews(WPARAM wParam, LPARAM lParam)
{
	CRasView* pPrevView = NULL;
	CRasView* pNextView = NULL;
	CRasDoc* pPrevDoc;
	CRasDoc* pNextDoc;

	CMDIChildWnd *pChild = (CMDIChildWnd *) GetActiveFrame();
	pPrevDoc = (CRasDoc *) pChild->GetActiveDocument( );
	CDocTemplate* pDocTemp;
	pDocTemp = pPrevDoc->GetDocTemplate( );
	POSITION posdoc = pDocTemp->GetFirstDocPosition( );

	//Activate all views successively
	//to cause redrawing of each database.
	while( posdoc!=NULL )
	{	//Save data from previous
		pPrevView = GetActiveRasView();
		ASSERT( pPrevView!=NULL );
		if( pPrevView!=NULL )
		{	pPrevDoc = (CRasDoc *)pPrevView->GetDocument();
			pPrevDoc->ManageDisplay(false);
		}

		//Get the doc and the unique view
		pNextDoc = (CRasDoc *)pDocTemp->GetNextDoc(posdoc);
		POSITION posview = pNextDoc->GetFirstViewPosition( );
		pNextView = (CRasView *)pNextDoc->GetNextView( posview );
		ASSERT( posview==0 && pNextView!=NULL );

		pChild = (CMDIChildWnd *)pNextView->GetParentFrame( );
		pChild->MDIActivate();

		//Pseudo Onsize
		CRect rec;
		pNextView->GetWindowRect( (LPRECT) rec );
		if( int dx = rec.Width( )%4 )
			rec.right += 4-dx;
		pNextDoc->m_DataDisplay.Data_XRange = rec.Width( );
		pNextDoc->m_DataDisplay.Data_YRange = rec.Height( );
		pNextDoc->FixRange();
		if( Database )
        	ReDrawFlag |= RFReSize;
		pNextView->RefreshView();	
	}

	return 0L;
}

//Get available client area in screen coordinates
CRect CMainFrame::GetScreenClientCoor()
{	CRect rRecFrame;
	CRect rect;

	GetClientRect( (LPRECT) rRecFrame );
	ClientToScreen( (LPRECT) rRecFrame );

	if( (m_wndMainBar.GetStyle() & WS_VISIBLE) != 0 &&
		 !m_wndMainBar.IsFloating( ) )
	{	m_wndMainBar.GetWindowRect(&rect);
		rRecFrame.top = rect.bottom ;
	}

	if( (m_wndSelectBar.GetStyle() & WS_VISIBLE) != 0 )
	{	m_wndSelectBar.GetWindowRect(&rect);
		if( rect.bottom > rRecFrame.top )
			rRecFrame.top = rect.bottom ;
	}

	if( (m_wndScriptBar.GetStyle() & WS_VISIBLE) != 0 )
	{	m_wndScriptBar.GetWindowRect(&rect);
		if( rect.bottom > rRecFrame.top )
			rRecFrame.top = rect.bottom ;
	}

	if( (m_wndCommandBar.GetStyle() & WS_VISIBLE) != 0 )
	{	m_wndCommandBar.GetWindowRect( &rect );
		rRecFrame.bottom = rect.top;
	} else if( (m_wndStatusBar.GetStyle() & WS_VISIBLE) != 0 )
	{	m_wndStatusBar.GetWindowRect( &rect );
		rRecFrame.bottom = rect.top;
	}

	return(rRecFrame);
}


/*======*/
/* Help */
/*======*/

void CMainFrame::OnHelpRasTop() 
{	StartHelpHTML();	}

/* Handle F1 key */
BOOL CMainFrame::OnHelpInfo(HELPINFO* pHelpInfo) 
{	OnHelpRasTop();
	return true;
}

