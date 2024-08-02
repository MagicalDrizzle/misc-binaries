 /**************************************************************************
 *                                                                         *
 *                                 RasTop                                  *
 /***************************************************************************
 *                                 RasMol                                  *
 *                                                                         *
 *                 Molecular Graphics Visualisation Tool                   *
 *                                                                         *
 * Please read the file RASLIC for the license terms which apply to this   *
 * file (Rasmol license).                                                  *
 ***************************************************************************/

/* Authors:
 * Philippe Valadon, copyright (c) 2007
 */



/////////////////////////////////////////////////////////////////////////////
// rasdoc.h
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RASDOC_H_INCLUDED_)
#define AFX_RASDOC_H_INCLUDED_

#include <afxtempl.h>
#include "datadisplay.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

char *RegisterFormat( char *buffer, char *desc, char *ext );

class CRasDoc : public CDocument
{
protected:
	CRasDoc();
	DECLARE_DYNCREATE(CRasDoc)

// Attributes
public:

// Operations
public:


// Overrides
	//{{AFX_VIRTUAL(CRasDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRasDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	int m_DocCopyNum;
	CData m_DataDisplay;
	int ViewActivationFlag;
	HBITMAP m_PixMap;

	static CRasDoc* OpenRSMFile(CString& PathName);
	void FixRange();
	void ManageDisplay(BOOL ActivationFlag);
    int GetMaxCBFactor();

protected:
	static CRasDoc* GetEmptyDoc();
	void CheckActivationState();
	int CountDuplicates();
	int ManageOpening(CString& PathName);
	int GetSaveAsFilename( CString &Filename, CString &NewFilename );

protected:
	//{{AFX_MSG(CRasDoc)
	afx_msg void OnSaveMolecularfile();
	afx_msg void OnUpdateSaveMolecularfile(CCmdUI* pCmdUI);
	afx_msg void OnSaveImage();
	afx_msg void OnUpdateSaveImage(CCmdUI* pCmdUI);
	afx_msg void OnSaveScript();
	afx_msg void OnUpdateSaveScript(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadRasmolscript();
	afx_msg void OnLoadMolecularfile();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileMoleculeAs();
	afx_msg void OnUpdateFileMoleculeAs(CCmdUI* pCmdUI);
	afx_msg void OnFileAdd();
	afx_msg void OnUpdateFileAdd(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_RASDOC_H_INCLUDED_)
