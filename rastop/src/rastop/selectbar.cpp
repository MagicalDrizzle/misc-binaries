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


///////////////////////////////////////////////////////////////////////////////
//SelectBar

#define COMBOBOX_INDEX	11
#define COMBOBOX_WIDTH	110
#define COMBOBOX_HEIGHT 340

#include "stdafx.h"
#include "RasTop.h"
#include "rasdoc.h"
#include "rasview.h"
#include "mainfrm.h"
#include "selectbar.h"

static UINT BASED_CODE SelectButtons[] =
{	ID_COORD_CHANGE,
    ID_DISPLAY_WORLDAXES,
		ID_SEPARATOR,
    ID_VIEW_COLORBAR,
		ID_SEPARATOR,
	ID_SELECT_SHOW,
    ID_SELECT_ALL,
	ID_SELECT_PREVIOUS,
	ID_SELECT_INVERSE,
	ID_SELECT_EXPRESSION,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
	ID_SELECTION_NEW,
	ID_SELECTION_ADD,
	ID_SELECTION_REMOVE,
	ID_SELECTION_EXTRACT,
	ID_SELECTION_EXCLUDE,
};

CSelectBar::CSelectBar()	{}

CSelectBar::~CSelectBar()	{}

BOOL CSelectBar::Init(CWnd* pParentWnd)
{
	if( !Create(pParentWnd,WS_CHILD|CBRS_TOP & ~WS_VISIBLE, IDW_SELECTBAR) )
		return false;	
	SetBarStyle(GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_GRIPPER);
	ModifyStyle(0,TBSTYLE_FLAT);

	if( !LoadBitmap(IDR_SELECTIONBAR) )
		return false;
	if( !SetButtons(SelectButtons, sizeof(SelectButtons)/sizeof(UINT)) )
		return false;

	CRect rect(-COMBOBOX_WIDTH, -COMBOBOX_HEIGHT, 0, 0);
	if( !m_ElementBox.Init( this, rect, IDC_ELEMENTBOX, 0, ElementList) )
		return false;
	if( !m_PropertyBox.Init( this, rect, IDC_PROPERTYBOX, 1, PropertyList) )
		return false;
	if( !m_UserSetBox.Init( this, rect, IDC_USERBOX, 2, UserList) )
		return false;
    // make box smaller
    rect.bottom = rect.top + 22;
	if( !m_Selected.Init( this, rect, IDC_USERSELECTION, 3) )
		return false;

	return true;
}

BEGIN_MESSAGE_MAP(CSelectBar, CToolBar)
	//{{AFX_MSG_MAP(CSelectBar)
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CSelectBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{	CToolBar::OnWindowPosChanged(lpwndpos);
    if( IsFloating( ) ) {
        CString s;
        s.LoadString(IDS_LOCAL_SELWND);
		SetWindowText(_T(s));
    }
}

void CSelectBar::AddKeyword(CString & Key)
{	if( m_UserSetBox.FindString(-1, Key)==CB_ERR )
		m_UserSetBox.InsertString(-1, Key);
}

void CSelectBar::ResetBox(BOOL Status)
{	int i,j;

	m_UserSetBox.ResetContent( );
	i = 0;
	while( true )
	{	if( *(UserList[i].set) == '\0' )
			break;
		m_UserSetBox.InsertString( -1, UserList[i].set  );
		i++;
	}

	// Add defined atom sets
	if( Status && Database )
	{	SymEntry  **prev, *ptr;
		SymEntry __far *cur;
		SymEntry __far *temp;
		CString szTemp;
	
		// Add sets to box
		cur = SymbolTable;
		if( cur )
		{	while( cur->lft )
				cur = cur->lft;	

			while( cur )
			{   szTemp = cur->ident;
				szTemp.MakeLower();
				AddKeyword(szTemp);
				prev = &SymbolTable;
				temp = *prev;
				while( temp->rgt )
					temp = temp->rgt;	
				while( *prev )
				{   ptr = *prev;
					i = strcmp(temp->ident,ptr->ident);
				    j = strcmp(cur->ident,ptr->ident);
					if( i>0&&j<0 ) temp = ptr;
				    prev = (j<0)? &(ptr->lft) : &(ptr->rgt);
				}
				if( cur==temp )
					cur = NULL;
				else
					cur = temp;
			}
		}
	}

	m_UserSetBox.SetCurSel( 0 );
    CString temp = "";
    m_Selected.ChangeSelection(temp);
}


/////////////////////////////////////////////////////////////////////////////
// CSelectBox

CSelectBox::CSelectBox()	{}

CSelectBox::~CSelectBox()	{}

BEGIN_MESSAGE_MAP(CSelectBox, CComboBox)
	//{{AFX_MSG_MAP(CSelectBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSelectBox::Init(CToolBar* pParentWnd, const RECT& rect, 
					   UINT nID, int Index, SetCommand *List)
{
	if( !Create(WS_CHILD | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL |
	            CBS_HASSTRINGS &~WS_VISIBLE, rect, pParentWnd, nID) )
		return false;

	SetList = List;

	//Create font
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
	SendMessage(WM_SETFONT, (WPARAM)hFont);

	pParentWnd->SetButtonInfo(COMBOBOX_INDEX + Index, nID, TBBS_SEPARATOR, 
							  COMBOBOX_WIDTH);

	if( m_hWnd!=NULL )
	{	CRect rect;
		pParentWnd->GetItemRect(COMBOBOX_INDEX + Index, rect);
		SetWindowPos(NULL, rect.left, rect.top, 0, 0, 
			SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS);
		ShowWindow(SW_SHOW);
	}

	//Fill the box with predefined sets
	int i = 0;
	while( true )
	{	if( *(List[i].set) == '\0' )
			break;
		InsertString( -1, List[i].set  );
		i++;
	}
	SetCurSel( 0 );

	return true;
}

void CSelectBox::OnSelchange() 
{	GETFRAME
	int nSel = GetCurSel( );

	// Need a tric
	if( !strcmp(SetList[0].command,"#set#") )
	{	if( nSel == CB_ERR || nSel == 0 )
		{	pFrame->m_SelectString = "";
            (pFrame->m_wndSelectBar).m_Selected.ChangeSelection(pFrame->m_SelectString);
			SetCurSel( 0 );
		} else
		{	CString CurSelText;
			GetLBText( nSel, CurSelText );
			pFrame->m_SelectString = CurSelText;
            (pFrame->m_wndSelectBar).m_Selected.ChangeSelection(CurSelText);
		}
	} else 
	{	if( nSel == CB_ERR || nSel == 0 || (*(SetList[nSel].command)=='\0') )
		{	pFrame->m_SelectString = "";
            (pFrame->m_wndSelectBar).m_Selected.ChangeSelection(pFrame->m_SelectString);
			SetCurSel( 0 );
		} else  
		{	CString CurSelText;
			CurSelText = SetList[nSel].command;
			//GetLBText( nSel, CurSelText );
			pFrame->m_SelectString = CurSelText;
			CurSelText = SetList[nSel].set;
            (pFrame->m_wndSelectBar).m_Selected.ChangeSelection(CurSelText);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSelected

CSelected::CSelected()	{}

CSelected::~CSelected()	{}

BEGIN_MESSAGE_MAP(CSelected, CStatic)
	//{{AFX_MSG_MAP(CSelectBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSelected::Init(CToolBar* pParentWnd, const RECT& rect, UINT nID, int Index)
{

	if( !Create( NULL, WS_CHILD | WS_BORDER & ~WS_VISIBLE, rect, pParentWnd, nID ) )
		return false;

	//Create font
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
	SendMessage(WM_SETFONT, (WPARAM)hFont);

	pParentWnd->SetButtonInfo(COMBOBOX_INDEX + Index, nID, TBBS_SEPARATOR, 
							  COMBOBOX_WIDTH);

	if( m_hWnd!=NULL )
	{	CRect rect;
		pParentWnd->GetItemRect(COMBOBOX_INDEX + Index, rect);
		SetWindowPos(NULL, rect.left, rect.top, 0, 0, 
			SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS);
		ShowWindow(SW_SHOW);
	}

	return true;
}

void CSelected::ChangeSelection( CString &Selection )
{   CString temp = Selection;
    temp = " " + temp;
    if( temp.GetLength()>10 )
    {   temp = temp.Left(10);
        temp += '.';
    }
    this->SetWindowText( (LPCTSTR) temp ); }


