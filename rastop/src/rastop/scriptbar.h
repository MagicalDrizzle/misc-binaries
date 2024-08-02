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
// scriptbar.h 
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTBAR_H_)
#define AFX_SCRIPTBAR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// CScriptBox combo

class CScriptBox : public CComboBox
{
	//{{AFX_VIRTUAL(CScriptBox)
	//}}AFX_VIRTUAL

public:
	CScriptBox();
	virtual ~CScriptBox();
	BOOL Initialize(CToolBar* pParentWnd, const RECT& rect, UINT nID, int Index);

private:
    void CScriptBox::AddFile(char *file);

protected:

	SetCommand* SetList;

	//{{AFX_MSG(CSelectBox)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CScriptBar toolbar

class CScriptBar : public CToolBar
{
// Construction
public:
	CScriptBar();
	BOOL Init(CWnd* pParentWnd);

protected:

// Attributes
public:

protected:
	CScriptBox m_ScriptBox;


// Operations

public:

// Overrides
	//{{AFX_VIRTUAL(CSelectBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScriptBar();

protected:

	//{{AFX_MSG(CSelectBar)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SCRIPTBAR_H_)
