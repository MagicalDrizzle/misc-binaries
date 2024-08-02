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
 * Christian Duqué, copyright (c) 2003 (RasTop 2.0.2-VF June 2003)
 */

//////////////////////////////////////////////////////////////////////
// cmdwindow.h
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CmdWindow_H__INCLUDED_)
	#define AFX_CmdWindow_H__INCLUDED_
	#if _MSC_VER >= 1000
		#pragma once
	#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// CRasMolEdit window

class CRasMolEdit : public CEdit
{
// Construction
public:
	CRasMolEdit();

// Attributes
public:
    CString TextBuffer;
protected:

// Operations
public:
    void ProcessCommand( int before);

protected:
    void SetCaretToEnd();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRasMolEdit)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRasMolEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRasMolEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
    afx_msg LRESULT OnSetText(WPARAM, LPARAM); 


	DECLARE_MESSAGE_MAP()
};



	/////////////////////////////////////////////////////////////////////////////
	// CmdWindow

	class CmdWindow : public CDialog
	{
		DECLARE_DYNAMIC(CmdWindow)
		public:
			CmdWindow();
			virtual ~CmdWindow();

		// Attributes
		public:
			CFont m_FontEchoText;

		protected:
			CRasMolEdit m_EchoText;
			CBrush m_brush;

		// Operations

		public:
			BOOL Create(UINT nIDTemplate, CWnd* pParentWnd);
			void EcrisDansEcho(CString& strText);
            void ChangeTxtColor( COLORREF newtxtcolor );
            void ChangeBkgColor( COLORREF newbkgcolor );
            COLORREF getTxtColor( void );
            COLORREF getBkgColor( void );

		protected:
        //{{AFX_VIRTUAL(CmdWindow)
	    //}}AFX_VIRTUAL
		protected:
		//{{AFX_MSG(CmdWindow)
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnMove(int x, int y);
	    afx_msg void OnClose();

	    //}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};


#endif // !defined(AFX_CmdWindow_H__INCLUDED_)

/////////////////////////////////////////////////////////////////////////////

