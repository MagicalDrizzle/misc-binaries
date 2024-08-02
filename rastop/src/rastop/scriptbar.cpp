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
// scriptbar.cpp 
/////////////////////////////////////////////////////////////////////////////

#define SCOMBOBOX_INDEX	3
#define SCOMBOBOX_WIDTH	200
#define SCOMBOBOX_HEIGHT 340

#include "stdafx.h"
#include "RasTop.h"
#include "rasdoc.h"
#include "rasview.h"
#include "mainfrm.h"
#include "scriptbar.h"

static UINT BASED_CODE ScriptButtons[] =
{	ID_REPRESENTATION,
    ID_VIEW_COMMANDLINE,
    ID_COMMANDLINE_ECHO,
		ID_SEPARATOR,
		ID_SEPARATOR,
//		ID_SEPARATOR,
};

CScriptBar::CScriptBar()	{}

CScriptBar::~CScriptBar()	{}

BOOL CScriptBar::Init(CWnd* pParentWnd)
{
	if( !Create(pParentWnd,WS_CHILD|CBRS_TOP & ~WS_VISIBLE, IDW_SCRIPTBAR) )
		return false;	
	SetBarStyle(GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_GRIPPER);
	ModifyStyle(0,TBSTYLE_FLAT);

	if( !LoadBitmap(IDR_SCRIPTBAR) )
		return false;
	if( !SetButtons(ScriptButtons, sizeof(ScriptButtons)/sizeof(UINT)) )
		return false;

	CRect rect(-SCOMBOBOX_WIDTH, -SCOMBOBOX_HEIGHT, 0, 0);
	if( !m_ScriptBox.Initialize( this, rect, IDC_SCRIPTBOX, 0) )
		return false;

	return true;
}

BEGIN_MESSAGE_MAP(CScriptBar, CToolBar)
	//{{AFX_MSG_MAP(CScriptBar)
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CScriptBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{	CToolBar::OnWindowPosChanged(lpwndpos);
    if( IsFloating( ) ) {
        CString s;
        s.LoadString(IDS_LOCAL_SCRWND);
		SetWindowText(_T(s));
    }
}



/////////////////////////////////////////////////////////////////////////////
// CScriptBox

CScriptBox::CScriptBox()	{}

CScriptBox::~CScriptBox()	{}

BEGIN_MESSAGE_MAP(CScriptBox, CComboBox)
	//{{AFX_MSG_MAP(CScriptBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CScriptBox::AddFile(char *file) {
    char *end;
    end= strrchr(file,'.');
    if (!end)
        return;
    if (!strcmp(end,".scr"))
        InsertString(-1,file);
}

BOOL CScriptBox::Initialize(CToolBar* pParentWnd, const RECT& rect,UINT nID, int Index)
{
	if( !Create(WS_CHILD | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL |
	            CBS_HASSTRINGS &~WS_VISIBLE, rect, pParentWnd, nID) )
		return false;

	//Create font
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
	SendMessage(WM_SETFONT, (WPARAM)hFont);

	pParentWnd->SetButtonInfo(SCOMBOBOX_INDEX + Index, nID, TBBS_SEPARATOR, 
							  SCOMBOBOX_WIDTH);

	if( m_hWnd!=NULL )
	{	CRect rect;
		pParentWnd->GetItemRect(SCOMBOBOX_INDEX + Index, rect);
		SetWindowPos(NULL, rect.left, rect.top, 0, 0, 
			SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS);
		ShowWindow(SW_SHOW);
	}

    /* Remake of GetRasMolPath() which is not initialized yet! */
    char Path[1024];
    int i,d;
    LPTSTR* name = NULL;
    *Path = '\0';
    SearchPath(NULL,"RasTop.exe","exe",1023,Path,name);
    i = d = 0;
    while (Path[i])
        if (Path[i++]==DirChar)
            d = i;
    Path[d] = '\0';

    if (*Path != '\0') {
        char *ptr;
        ptr = Path + d;
        strcpy(ptr,"scripts\\*.*");

	    //Fill box with script in RasTop/scritps    
        HANDLE hsearch;
        WIN32_FIND_DATA findFile;
        hsearch = FindFirstFile(Path, &findFile);
        if (hsearch != INVALID_HANDLE_VALUE) {
            AddFile(findFile.cFileName);
            while (FindNextFile(hsearch, &findFile)) {
                AddFile(findFile.cFileName);
            }
            FindClose(hsearch);
        }
    }
    SetCurSel(0);
	return true;
}


void CScriptBox::OnSelchange() 
{	GETFRAME
    GETVIEW
	int nSel = GetCurSel( );

    CString CurSelText;
    GetLBText( nSel, CurSelText );
    if (CurSelText.IsEmpty())
        return;

    CString script = RasMolPath;
    script = "script " + script + "scripts\\" + CurSelText;

    if (pView) {
        pView->ExecuteCommand( DisplayOn, script);
    }
}

