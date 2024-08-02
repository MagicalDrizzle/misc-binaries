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
// RasTop.h
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RASTOP_H_INCLUDED_)
#define AFX_RASTOP_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

#define VERSION_NUMBER 1

#define DisplayOff		0
#define DisplayOn		1

#define WM_UPDATERASVIEW	(WM_USER + 0)
#define WM_COLOR			(WM_USER + 100)

#define GETFRAME CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
#define GETVIEW  CRasView* pView = pFrame->GetActiveRasView();
#define GETDOC   CRasDoc* pDoc = GetDocument();

#if defined(STRICT)
#undef STRICT
#endif

/////////////////////////////////////////////////////////////////////////////
// CRasTopApp:

class CRasTopApp : public CWinApp
{
public:

	CRasTopApp();
	void RasTopExit();
// Overrides
	//{{AFX_VIRTUAL(CRasTopApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CRasTopApp)
	afx_msg void OnFileOpenRasDoc();
	afx_msg void OnAboutrastop();
	afx_msg void OnFileNewRasDoc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAboutRasTop dialog

class CAboutRasTop : public CDialog
{
public:
	CAboutRasTop(CWnd* pParent = NULL);   

// Dialog Data
	//{{AFX_DATA(CAboutRasTop)
	enum { IDD = IDD_ABOUTRASTOP };
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CAboutRasTop)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutRasTop)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // !defined(AFX_RASTOP_H_INCLUDED_)
