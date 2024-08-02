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
// ColorBar.h : header file


#if !defined(AFX_ColorBar_H_INCLUDED_)
#define AFX_ColorBar_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#define MAX_COLORS				42
#define ID_COLOR				35000

// To hold the colours and their names
typedef struct {
    COLORREF crColour;
    TCHAR    *szName;
} ColourTable;

/////////////////////////////////////////////////////////////////////////////
// CColorBar window

class CColorBar : public CToolBar
{
// Construction
public:
	CColorBar();
	BOOL Init(CWnd* pParentWnd);

// Attributes
public:

// protected attributes
protected:
    static ColourTable m_crColours[MAX_COLORS];
	CComboBox m_ColorItem;
	CToolTipCtrl m_tooltip;
	HBITMAP PixTool;
	BITMAPINFO *BitToolInfo;
	HGLOBAL TBufHandle;
// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CColorBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorBar();
	void OnCommandHandler(UINT nID);

protected:
	int CColorBar::MakePalette( void );

	//{{AFX_MSG(CColorBar)z
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_ColorBar_H_INCLUDED_)
