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



////////////////////////////////////////////////////////////////////////
// rasdoc.cpp 
////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RasTop.h"
#include <limits.h>
#include <direct.h>
#include <stdlib.h>

#include "dialog.h"
class CRasView;	//Forward declaration
#include "mainfrm.h"
#include "rasdoc.h"
#include "rasview.h"
#include "dialog.h"

static char snamebuf[1024];
static char fnamebuf[1024];

#define STOPTIMER		GETFRAME \
						pFrame->m_wndCommandBar.KillTimerPannel();
#define STARTTIMER		pFrame->m_wndCommandBar.SetTimerPannel();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRasDoc

IMPLEMENT_DYNCREATE(CRasDoc, CDocument)

BEGIN_MESSAGE_MAP(CRasDoc, CDocument)
	//{{AFX_MSG_MAP(CRasDoc)
	ON_COMMAND(ID_SAVE_MOLECULARFILE, OnSaveMolecularfile)
	ON_UPDATE_COMMAND_UI(ID_SAVE_MOLECULARFILE, OnUpdateSaveMolecularfile)
	ON_COMMAND(ID_SAVE_IMAGE, OnSaveImage)
	ON_UPDATE_COMMAND_UI(ID_SAVE_IMAGE, OnUpdateSaveImage)
	ON_COMMAND(ID_SAVE_SCRIPT, OnSaveScript)
	ON_UPDATE_COMMAND_UI(ID_SAVE_SCRIPT, OnUpdateSaveScript)
	ON_COMMAND(ID_FILE_LOAD_RASMOLSCRIPT, OnFileLoadRasmolscript)
	ON_COMMAND(ID_FILE_LOAD_MOLECULARFILE, OnLoadMolecularfile)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_COMMAND(ID_FILE_MOLECULE_AS, OnFileMoleculeAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_MOLECULE_AS, OnUpdateFileMoleculeAs)
	ON_COMMAND(ID_FILE_ADD, OnFileAdd)
	ON_UPDATE_COMMAND_UI(ID_FILE_ADD, OnUpdateFileAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRasDoc construction/destruction

CRasDoc::CRasDoc()
{
	ViewActivationFlag = 2;
	m_PixMap = NULL;
	m_DocCopyNum = 1;
}


CRasDoc::~CRasDoc()	{}


/////////////////////////////////////////////////////////////////////////////
// Switching documents

// Manage the switch of RasMol between documents
void CRasDoc::ManageDisplay(BOOL ActivationFlag)
{	
	GETFRAME
	
	/* Activation of the view */
	if( ActivationFlag )	
	{	if( ViewActivationFlag != 1 )
			ViewActivationFlag = 1;
		else
			return;

		CheckActivationState();
		TRACE1( "TRACE\tView Activation\t%s\n", GetTitle() );

		if( m_PixMap )
		{	DeleteObject(m_PixMap);
			m_PixMap = NULL;
		}

		m_DataDisplay.RegenerateData();
		ResetUserSetBox( );
		if ( Database )
			ReDrawFlag |= RFRefresh | RFColour | RFInitial;

		GETVIEW
		if( pView!=NULL )
			pView->RefreshView();

	/* Deactivation of the view */
	} else		
	{	// Only one deactivation
		if( ViewActivationFlag != 0 )
			ViewActivationFlag = 0;
		else
			return;

		TRACE1( "TRACE\tView Deactivation\t%s\n", GetTitle() );

		if( Database )
		{	FixRange();
			HDC hDC = ::GetDC(NULL);
			if( PixMap )
			{	LPSTR Buffer = (LPSTR)::GlobalLock((HGLOBAL) FBufHandle); 
	
				m_PixMap = CreateDIBitmap( hDC, (BITMAPINFOHEADER __far *)BitInfo, 
				           CBM_INIT, Buffer, BitInfo, DIB_RGB_COLORS);
				if( m_PixMap==NULL )
					RasMolFatalExit (MsgStrs(RfeNoMem));
				GlobalUnlock(Buffer);
			}
			::ReleaseDC(NULL,hDC);
		}

		m_DataDisplay.SaveData();
		pFrame->m_wndCommandBar.ResetSpinMode();
		ZapDatabase(False);
	}
}

void CRasDoc::CheckActivationState()
{	/* Ensure that all Rasmol data are deactivated before activating
	   the current one */

	CRasDoc* pDoc;
	CDocTemplate* pDocTemp;
	pDocTemp = GetDocTemplate( );
	POSITION posdoc = pDocTemp->GetFirstDocPosition();
	while( posdoc != 0 )
	{	pDoc = (CRasDoc *)pDocTemp->GetNextDoc(posdoc);
		if( pDoc->ViewActivationFlag && (pDoc != this) )
			pDoc->ManageDisplay(false);
	}
}

void CRasDoc::FixRange()
{	/* Initialize Range according to saved XRange and YRange.
	   Done systematically before zapping the database.	*/

	XRange = m_DataDisplay.Data_XRange;
	YRange = m_DataDisplay.Data_YRange;
	Range =  MinFun(XRange,YRange);
	ZRange = Range;
	HRange = YRange>>1;
	WRange = XRange>>1;
	if( UseStereo )
	    SRange = WRange>>1;
	else
		SRange = WRange;
}

/////////////////////////////////////////////////////////////////////////////
// CRasDoc diagnostics

#ifdef _DEBUG
void CRasDoc::AssertValid() const
{	CDocument::AssertValid();	}

void CRasDoc::Dump(CDumpContext& dc) const
{	CDocument::Dump(dc);		}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

/* ========= */
/* Documents */
/* ========= */

BOOL CRasDoc::OnNewDocument()
{	if( !CDocument::OnNewDocument() )
		return false;
	GETFRAME
	pFrame->ActivateCommandBar();
	pFrame->m_wndCommandBar.UpdatePannel();
	return true;
}

CRasDoc* CRasDoc::GetEmptyDoc()
{	
	GETFRAME

	// Check if active document is empty; if not make sure all are deactivated
	CRasFrame *pChild = (CRasFrame *) pFrame->GetActiveFrame();
	if( !pChild )
		return NULL;
	CRasDoc *pDoc = (CRasDoc *)pChild->GetActiveDocument( );
	if( pDoc )
	{	if( !MoleculeList && (pDoc->m_DataDisplay.Data_MoleculeList==NULL)  )
		{	return pDoc;
		} else
		{	pDoc->CheckActivationState();
			pDoc->ManageDisplay(false);
		}
	}
	
	// Check if any empty document
	POSITION tpos = AfxGetApp( )->GetFirstDocTemplatePosition( );
	CDocTemplate* pDocTemp = AfxGetApp( )->GetNextDocTemplate(tpos);
	POSITION posdoc = pDocTemp->GetFirstDocPosition( );
	int countdoc = 0;
	while( posdoc!=NULL )
	{	pDoc = (CRasDoc *)pDocTemp->GetNextDoc(posdoc);
		if( pDoc->m_DataDisplay.Data_MoleculeList!=NULL )
			pDoc = NULL;
		else
			break;
	}
	if( pDoc )
	{	// Activate the unique view		
		POSITION posview = pDoc->GetFirstViewPosition( );
		CRasView *pView = (CRasView *)pDoc->GetNextView( posview );
		ASSERT( posview == 0 && pView != NULL );
		CMDIChildWnd *pChild = (CMDIChildWnd *)pView->GetParentFrame( );
		pChild->MDIActivate();
		return pDoc;
	}

	// At last, create a new document
	((CRasTopApp*) AfxGetApp( ))->OnFileNewRasDoc();
	pChild = (CRasFrame *) pFrame->GetActiveFrame();
	pDoc = (CRasDoc *)pChild->GetActiveDocument( );		
	if( pDoc==NULL || MoleculeList) 
	{	InvalidateCmndLine();
        WriteString(MsgStrs(RTPNoWind));
		return NULL;
	}
	return pDoc;
}


void CRasDoc::OnCloseDocument() 
{	
	ManageDisplay(true);
	if( m_PixMap )
		DeleteObject(m_PixMap);
	CloseDisplay();
		
	if( MoleculeList )
	{	FixRange();
		ZapDatabase(True);

		// Free all rasmol memory allocations
		int mem = 0;
		CDocTemplate* pDocTemp = GetDocTemplate( );
		POSITION posdoc = pDocTemp->GetFirstDocPosition();
		while( posdoc )
		{	CRasDoc* pDoc = (CRasDoc *)pDocTemp->GetNextDoc(posdoc);
			if( pDoc->m_DataDisplay.Data_MoleculeList!=NULL )
				mem = 1;
		}
		if( !mem )
		{	ResetVoxelData();
			PurgeDatabase();
			UpdateRender(2);
			UpdateAbstree();
			UpdateRepres();
		}
	}
	
	SetPickMode(PickIdent);

	CDocument::OnCloseDocument();
}


/* ======== */
/* Openings */
/* ======== */

CRasDoc* CRasDoc::OpenRSMFile(CString& PathName)
{
	CRasDoc* pDoc = CRasDoc::GetEmptyDoc();
	if( pDoc == NULL )
		return NULL;
	if( !pDoc->ManageOpening(PathName) )
	{	InvalidateCmndLine();
        WriteString(MsgStrs(RTPErrDat));
		return NULL;
	} else {
        int n = PathName.ReverseFind('\\');
        CString name;
        if (n==-1)
            name = PathName;
        else
            name = PathName.Right(PathName.GetLength()-n-1);
        char buffer[80];
        InvalidateCmndLine();
        char *ptr = name.LockBuffer( );
        FormatMsg(buffer,RTPRsmLoad,ptr);
        name.UnlockBuffer( );
        WriteString(buffer);        
    }
	return( pDoc );
}


int CRasDoc::CountDuplicates( void )
{	
	CDocTemplate* pDocTemp = GetDocTemplate( );
	POSITION posdoc = pDocTemp->GetFirstDocPosition( );
	int countdoc = 0;
	int max = 0;
	while( posdoc!=NULL )
	{	CRasDoc* pOtherDoc = (CRasDoc *)pDocTemp->GetNextDoc(posdoc);
		ASSERT(pOtherDoc->IsKindOf(RUNTIME_CLASS(CRasDoc)));
		if( pOtherDoc == this )
		{	countdoc++;
			continue;
		}
		if( !strcmp(SpaceName,pOtherDoc->m_DataDisplay.Data_SpaceName) )
		{	countdoc++;
			if( pOtherDoc->m_DocCopyNum > max )
				max = pOtherDoc->m_DocCopyNum;
		}
	}
	ASSERT(countdoc>=1);
	if( max >= countdoc )
		countdoc = max + 1;
	m_DocCopyNum = countdoc;
	return(countdoc);
}

int CRasDoc::ManageOpening(CString& PathName)
{	GETFRAME
	GETVIEW
	if( pView==NULL )
		return false;

    /* Get Extension */
    char path_buffer[_MAX_PATH];
    char ext[_MAX_EXT];
	strcpy(path_buffer,PathName);
    _splitpath(path_buffer,NULL,NULL,NULL,ext);


	/*=== Opening file according to extension ===*/
    CString Command;
	BeginWait();
    if( !strcmp(ext,".pdb") || !strcmp(ext,".cif") )
	    Command = "load " + PathName + "\n";
    else
	    Command = "script " + PathName + "\n";
	pView->ExecuteCommand(DisplayOff,Command);
	EndWait();

	if( Database==NULL )
		return false;
	
	if( CurMolSet && (WorldScriptFormat==RSMFormat) )
	{	
		/* Compatibility RasTop 1.3 Format */
		if( !*(CurMolSet->title) )
		{	strcpy(CurMolSet->title,SpaceName);
			SetCanvasTitle( );
		}

		FileModified = False;
	} else
		FileModified = True;

    // Add to recent FileList
	if( CountDuplicates()==1 )
		AfxGetApp()->AddToRecentFileList(PathName);

	// Tric to keep the pathname while not changing the title on screen
	SetPathName(( LPCTSTR)PathName, false);
	SetCanvasTitle( );

	// Force background drawing in 32 bits
	FBClear = false;
	pView->RefreshView();
	return true;
}

void CRasDoc::OnFileAdd() 
{	

	char BASED_CODE Filter[] = 
		 "RSM Scripts or Files (*.rsm;*.pdb;*.cif)|*.rsm; *.pdb; *.cif|All Files (*.*)|*.*||";

	CFileDialog fdlg(true, ".rsm", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | 
		 OFN_OVERWRITEPROMPT , Filter, NULL);

	if( fdlg.DoModal()!=IDOK )
		return;
	
	GETFRAME
	GETVIEW
	if( pView==NULL )
		return;
	
	BeginWait();
	CString Command = "add " + fdlg.GetPathName() + "\n";
	pView->ExecuteCommand(DisplayOff,Command);
	EndWait();
}

void CRasDoc::OnUpdateFileAdd(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( true );		}


/* =============== */
/* Saving Routines */
/* =============== */

int CRasDoc::GetSaveAsFilename( CString &Filename, CString &NewFilename )
{
	char BASED_CODE Filter[] = "All Files (*.*)|*.*||";
 
	Filename.LockBuffer();
	CFileDialog fdlg(false, NULL, (LPCSTR) Filename, OFN_HIDEREADONLY | 
		OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT , Filter, NULL);
	Filename.UnlockBuffer();

	if( fdlg.DoModal()!=IDOK )
		return false;
	NewFilename = fdlg.GetPathName();
	return true;
}

void CRasDoc::OnFileSave() 
{	
	if( FileModified==False )
	{	InvalidateCmndLine();
        WriteString(MsgStrs(RTPNoSave));
		return;
	} else if( WorldScriptFormat!=RSMFormat )
	{	OnFileSaveAs();
		return;
	} else if( m_DocCopyNum>1 ) 
	{	OnFileSaveAs();
		return;
    }
    
	GETFRAME
	GETVIEW
	if( pView==NULL )
	{	RasMolFatalExit(MsgStrs(RfeSavDoc));
		return;
	}
	pView->ExecuteCommand(DisplayOff,"save\n");

}

void CRasDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( CurMolSet!=NULL);	}

void CRasDoc::OnFileSaveAs() 
{	CString prompt, name, newname;
	
	STOPTIMER
	GETVIEW
	if( pView==NULL )
	{	RasMolFatalExit(MsgStrs(RfeSavDoc));
		STARTTIMER
		return;
	}

	if( m_DocCopyNum>1 )
		name.Format("%s copy%d",SpaceName,m_DocCopyNum);
	else
		name = SpaceName;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
	name += ".rsm";
	newname.Empty();
	
	if( !GetSaveAsFilename(name, newname) )
	{	STARTTIMER
		return;
	}				
	if( !newname.IsEmpty() )
	{	AfxGetApp()->AddToRecentFileList(newname);
	    newname = "save " + newname;
		newname += "\n";
		pView->ExecuteCommand(DisplayOff,newname);
    	CountDuplicates();
        SetCanvasTitle( );
    }
	STARTTIMER
}

void CRasDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( MoleculeList!=NULL );		}

BOOL CRasDoc::SaveModified() 
{	CString prompt, name, newname;
	
	GETFRAME
	GETVIEW
	if( pView==NULL )
	{	RasMolFatalExit(MsgStrs(RfeSavDoc));
		return true;
	}

	/* Called when closing a document - Bypass MFCs */

	/* Empty document */
	if( !MoleculeList || !Database )
	{	return true;
	} else 

	/* Ongoing Script */
	if( FileDepth > -1 )
	{	return true;
	} else 

    /* Probably from a previous script  */
    if( CurMolSet && !CurMolSet->molfile )
	{	return true;
	} else 

	/* Unmodified Document, not a duplicat */
	if( !FileModified && (m_DocCopyNum==1) )
	{	return true;
	} else 

	/* Modified RSM , no duplicat */
	if( (WorldScriptFormat==RSMFormat) && (m_DocCopyNum==1) )
	{	STOPTIMER
		AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, SpaceName);
		switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
		{
			case IDCANCEL:
				STARTTIMER
				return false;
	
			case IDYES:
				pView->ExecuteCommand(DisplayOff,"save\n");
				break;

			case IDNO:
				break;

			default:
				ASSERT(FALSE);
				break;
		}
		STARTTIMER
		return true;
	} else

	/* Duplicats and data files */
	if( (m_DocCopyNum>1) || (WorldScriptFormat==DataFormat))
	{	
		STOPTIMER
		if( m_DocCopyNum>1 )
			name.Format("%s copy%d",SpaceName,m_DocCopyNum);
		else
			name = SpaceName;
		AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
		switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
		{
			case IDCANCEL:
				STARTTIMER
				return false;
	
			case IDYES:
				name += ".rsm";
				newname.Empty();
				if( !GetSaveAsFilename(name, newname) )
				{	STARTTIMER
					return false;
				}				
				if( !newname.IsEmpty() )
				{	AfxGetApp()->AddToRecentFileList(newname);
				    newname = "save " + newname;
					newname += "\n";
					pView->ExecuteCommand(DisplayOff,newname);
				}
				break;

			case IDNO:
				break;

			default:
				ASSERT(FALSE);
				break;
		}
		STARTTIMER
		return true;
	} 

	return true;
}

void CRasDoc::OnFileMoleculeAs() 
{	CString prompt, name, newname;
	
	STOPTIMER
	GETVIEW
	if( pView==NULL )
	{	RasMolFatalExit(MsgStrs(RfeSavDoc));
		STARTTIMER
		return;
	}

	if( CurMolSet->copynum>1 )
		name.Format("%s copy%d",CurMolSet->title,CurMolSet->copynum);
	else
		name = CurMolSet->title;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
	name += ".rsm";
	newname.Empty();
	
	if( !GetSaveAsFilename(name, newname) )
	{	STARTTIMER
		return;
	}				
	if( !newname.IsEmpty() )
	{	newname = "save molecule " + newname;
		newname += "\n";
		pView->ExecuteCommand(DisplayOff,newname);
	}
	STARTTIMER
}

void CRasDoc::OnUpdateFileMoleculeAs(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( (MoleculeList!=NULL) && MoleculeList->next );		}


/* =================== */
/* Mol. File & Scripts */
/* =================== */


CString LoadInputFile(int format )
{	static char ifilters[512];
	register char *ext;
    register int num;
    CString Command;
    Command.IsEmpty();

    char* dst;
    *fnamebuf = '\0';

    switch( format )
    {   case(FormatPDB):      ext = "PDB";  num = 1;  break;
		case(FormatAlchemy):  ext = "MOL";  num = 2;  break;
		case(FormatMol2):     ext = "MOL";  num = 3;  break;
		case(FormatMDL):      ext = "MOL";  num = 4;  break;
		case(FormatXYZ):      ext = "XYZ";  num = 5;  break;
		case(FormatCharmm):   ext = "CHM";  num = 6;  break;
        case(FormatMOPAC):    ext = "MOP";  num = 7;  break;
		case(FormatCIF):      ext = "CIF";  num = 8;  break;
    }

   /* filter for file opening */
    dst = ifilters;      
    dst = RegisterFormat(dst,"Brookhaven Databank (*.pdb;*.ent)","*.pdb;*.ent");
    dst = RegisterFormat(dst,"Alchemy File Format (*.alc;*.mol)","*.alc;*.mol");
    dst = RegisterFormat(dst,"Sybyl MOL2 Format (*.syb;*.mol)","*.syb;*.mol");
    dst = RegisterFormat(dst,"MDL Mol File Format (*.mdl;*.mol)","*.mdl;*.mol");
    dst = RegisterFormat(dst,"MSC (XMol) XYZ Format (*.xyz)","*.xyz");
    dst = RegisterFormat(dst,"CHARMm File Format (*.chm)","*.chm");
    dst = RegisterFormat(dst,"MOPAC File Format (*.mop)","*.mop");
    dst = RegisterFormat(dst,"CIF File Format  (*.cif)","*.cif");
    dst = RegisterFormat(dst,"All Files (*.*)","*.*");
    *dst = '\0';

	CFileDialog fdlg(true,NULL,NULL,OFN_HIDEREADONLY | 
	OFN_OVERWRITEPROMPT, NULL, NULL);

    fdlg.m_ofn.lStructSize=sizeof(OPENFILENAME);
    fdlg.m_ofn.hwndOwner = NULL;
    fdlg.m_ofn.lpstrFilter = ifilters;
    fdlg.m_ofn.lpstrCustomFilter = NULL;
    fdlg.m_ofn.nFilterIndex = num;
    fdlg.m_ofn.nMaxFile = 128;
    fdlg.m_ofn.lpstrFileTitle = NULL;
    fdlg.m_ofn.lpstrInitialDir = NULL;
    fdlg.m_ofn.lpstrTitle = MsgStrs(RTPSelMol);
    fdlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
    fdlg.m_ofn.lpstrFile = fnamebuf;

    fdlg.m_ofn.lpstrDefExt = ext;

    RefreshState = False;
	if (fdlg.DoModal()!=IDOK)
    {   RefreshState = True;
    	return Command;
    }
    RefreshState = True;

	ScriptFileFormat = DataFormat;
	CString cext = fdlg.GetFileExt();
	Command = fnamebuf;
	if( (cext == "pdb")||(cext == "ent") )
	{	Command = "load pdb " + Command + "\n";
	} else if( cext == "alc" )
	{	Command = "load alchemy " + Command + "\n";
	} else if( cext == "syb" )
	{	Command = "load mol2 " + Command + "\n";
	} else if( cext == "mdl" )
	{	Command = "load mdl " + Command + "\n";
	} else if( cext == "xyz" )
	{	Command = "load xyz " + Command + "\n";
	} else if( cext == "chm" )
	{	Command = "load charmm " + Command + "\n"; 
	} else if( cext == "mop" )
	{	Command = "load mopac " + Command + "\n"; 
	} else if( cext == "cif" )
	{	Command = "load cif " + Command + "\n"; 
	} else 
	{	switch( fdlg.m_ofn.nFilterIndex )
		{   case(1): Command = "load pdb " + Command + "\n";		break;
		    case(2): Command = "load alchemy " + Command + "\n";	break;
		    case(3): Command = "load mol2 " + Command + "\n";	break;
		    case(4): Command = "load mdl " + Command + "\n";		break;
		    case(5): Command = "load xyz " + Command + "\n";		break;
		    case(6): Command = "load charmm " + Command + "\n";	break;
		    case(7): Command = "load mopac " + Command + "\n";	break;
		    case(8): Command = "load cif " + Command + "\n";		break;
			default: Command = "load pdb " + Command + "\n";    
		}
	}
	return Command;
}


void CRasDoc::OnLoadMolecularfile() 
{	GETFRAME
	GETVIEW
	if( !pView )
		return;

	BOOL newdoc = (MoleculeList == NULL);

    CString Command = LoadInputFile(FormatPDB);
	BeginWait( );
    pView->ExecuteCommand(DisplayOn,Command);
	EndWait( );
	if( newdoc )
		if( CountDuplicates()>1 )
			SetCanvasTitle( );
	FileModified = true;
}

void CRasDoc::OnFileLoadRasmolscript() 
{	char ifilters[128];
    char* dst;
	*fnamebuf = '\0';

	GETFRAME
	GETVIEW
	if( pView==NULL )
		return;

    InvalidateCmndLine();
	WriteString(MsgStrs(RTPScrLoad));

	dst = ifilters; 
	dst = RegisterFormat(dst,"RasMol Script Files (*.scr)","*.scr");
	dst = RegisterFormat(dst,"Top Script Files (*.top)","*.top");
	dst = RegisterFormat(dst,"All Files (*.*)","*.*");
	*dst = '\0';


	CFileDialog fdlg(true, NULL, NULL, 
					  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL,  NULL);

	fdlg.m_ofn.lStructSize=sizeof(OPENFILENAME);
	fdlg.m_ofn.hwndOwner = NULL;
	fdlg.m_ofn.lpstrFilter = ifilters;
	fdlg.m_ofn.lpstrCustomFilter = NULL;
	fdlg.m_ofn.nFilterIndex = 1;
	fdlg.m_ofn.nMaxFile = 128;
	fdlg.m_ofn.lpstrFileTitle = NULL;
	fdlg.m_ofn.lpstrInitialDir = NULL;
	fdlg.m_ofn.lpstrTitle = MsgStrs(RTPScrBox);
	fdlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
	fdlg.m_ofn.lpstrFile = fnamebuf;
	fdlg.m_ofn.lpstrDefExt = "scr";
	
	if( fdlg.DoModal()!=IDOK )
	{	InvalidateCmndLine();
        WriteString(MsgStrs(RTPScrInter));
		return;
	}

	CString Command = fdlg.GetPathName();
	Command ="script " + Command + "\n";
	pView->ExecuteCommand(DisplayOn,Command);
	
	if( !IsPaused )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(RTPScrDone));
    }
}


/* ====== */
/* Export */
/* ====== */

/* Return Adjusted ClipBoardFactor */
int CRasDoc::GetMaxCBFactor( void )
{
    register double maxzoom, dialzoom;
    register int cbfactor;

    cbfactor = ClipBoardFactor;
    while( 1 )
    {   int done = 1;
        /* Check MaxZoom superior to one */
        maxzoom = 0.336*WorldSize/(Range*cbfactor);
        if( maxzoom < 1.0 )
            done = 0;
        /* Check DialValue[DialZoom] does not change */
        if( DialValue[3]>0.0 )
        {   dialzoom = DialValue[3];
            dialzoom *= MaxZoom/maxzoom;
            if( dialzoom>1.0 )
                done = 0;
        }
        if( !done )
            cbfactor -= 1;
        else
            break;
    }

    if( !cbfactor )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(RTPErrImg));
        return 0;
    }

    if( cbfactor!=ClipBoardFactor )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(RTPZoomFac));
    }

    return cbfactor;
}


void CRasDoc::OnSaveImage() 
{	
    STOPTIMER
	GETVIEW
	if( pView==NULL )
	{	STARTTIMER
		return;
	}
	CDialogImage dlg;
	dlg.m_SizeX = m_DataDisplay.Data_XRange;
	dlg.m_SizeY = m_DataDisplay.Data_YRange;
	if( dlg.DoModal()!=IDOK )
	{	STARTTIMER
		return;
	}

	CString ext;
	int num;
	switch( dlg.m_Format )
	{	case(0)	:	ext = "bmp";  num=1;	break;
		case(1)	:	ext = "gif";  num=2;	break;
		case(2)	:	ext = "ps";	  num=3;	break;
		case(3)	:	ext = "ps";	  num=4;	break;
		case(4)	:	ext = "ps";	  num=5;	break;
		case(5)	:	ext = "ppm";  num=6;	break;
		case(6)	:	ext = "ppm";  num=7;	break;
		case(7)	:	ext = "pict";  num=8;	break;
		case(8)	:	ext = "rgb";  num=9;	break;
		case(9)	:	ext = "ras";  num=10;	break;
		case(10):	ext = "ras";  num=11;	break;
	}

	char filename[128];
	CString fn = GetTitle();
	if( fn.Find('.')!= -1 )		fn = fn.Left(fn.Find('.'));
	if( fn.IsEmpty() )			fn = "notitle";
	if( fn.GetLength()>24 )		fn = fn.Left(24);
	if( !ext.IsEmpty() )		fn += "." + ext;
	fn.MakeLower( );
	strcpy(filename,fn.LockBuffer());
	fn.UnlockBuffer();

	char ofilters[512];
	char* dst;
	dst = ofilters;
    dst = RegisterFormat(dst,"Microsoft Bitmap","*.bmp");
    dst = RegisterFormat(dst,"CompuServe GIF","*.gif");
    dst = RegisterFormat(dst,"Colour PostScript","*.ps;*.eps");
    dst = RegisterFormat(dst,"Mono PostScript","*.ps;*.esp");
    dst = RegisterFormat(dst,"Vector PostScript","*.ps;*.eps");
    dst = RegisterFormat(dst,"Raw Portable Pixmap","*.ppm");
    dst = RegisterFormat(dst,"ASCII Portable Pixmap","*.ppm");
    dst = RegisterFormat(dst,"Apple Macintosh PICT","*.pict");
    dst = RegisterFormat(dst,"Silicon Graphics RGB","*.rgb");
    dst = RegisterFormat(dst,"RLE Sun Rasterfile","*.ras;*.im8");
    dst = RegisterFormat(dst,"Sun Rasterfile","*.ras");
    dst = RegisterFormat(dst,"All Files (*.*)","*.*");
    *dst = '\0';

	CFileDialog fdlg(false, NULL, (LPCSTR) fn, OFN_HIDEREADONLY | 
		OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT ,  NULL,  NULL);

    fdlg.m_ofn.lStructSize=sizeof(OPENFILENAME);
    fdlg.m_ofn.hwndOwner = NULL;
    fdlg.m_ofn.lpstrFilter = ofilters;
    fdlg.m_ofn.lpstrCustomFilter = NULL;
    fdlg.m_ofn.nFilterIndex = num;
    fdlg.m_ofn.lpstrDefExt = ext;
    fdlg.m_ofn.nMaxFile = 128;
    fdlg.m_ofn.lpstrFileTitle = filename;
    fdlg.m_ofn.lpstrInitialDir = NULL;
    fdlg.m_ofn.lpstrTitle = MsgStrs(RTPSelDir);
    fdlg.m_ofn.Flags |= OFN_PATHMUSTEXIST;

	if (fdlg.DoModal()!=IDOK)
	{	STARTTIMER
		return;
	}

	if( (XRange != dlg.m_SizeX) || (YRange != dlg.m_SizeY) ) 
    {   double maxzoom, dialzoom;
        int dx,xrange,yrange,range;

		xrange = dlg.m_SizeX;
		if( dx = xrange%4 )
			xrange += 4-dx;
		yrange = dlg.m_SizeY;
		range =  MinFun(xrange,yrange);
  
        /* Check MaxZoom superior to one */
        maxzoom = 0.336*WorldSize/range;
        if( maxzoom < 1.0 )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(RTPImgBig));
            return;
        }

        /* Check DialValue[DialZoom] does not change */
        if( DialValue[3]>0.0 )
        {   dialzoom = DialValue[3];
            dialzoom *= MaxZoom/maxzoom;
            if( dialzoom>1.0 )
            {   InvalidateCmndLine();
                WriteString(MsgStrs(RTPImgBig));
                return;
            }
        }
    }

	BeginWait();

	BOOL changedim = false;
	if( (XRange != dlg.m_SizeX) || (YRange != dlg.m_SizeY) ) 
	{	changedim = true;
		XRange = dlg.m_SizeX;
		int dx;
		if( dx = XRange%4 )
			XRange += 4-dx;
		YRange = dlg.m_SizeY;
		Range =  MinFun(XRange,YRange);
		ZRange = Range;
		HRange = YRange>>1;
		WRange = XRange>>1;
		ReDrawFlag |= RFReSize;
	    RefreshState = True;
		ClearImage();
		RefreshScreen();
	}

	CString sext = fdlg.GetFileExt();
	CString Command = fdlg.GetPathName();
    if( dlg.m_Format == (int)(fdlg.m_ofn.nFilterIndex -1) )
	{	switch( dlg.m_Format )
		{   case(0):    Command ="save bmp " + Command + "\n";      break;
    	    case(1):    Command ="save gif " + Command + "\n";      break;
		    case(2):    Command ="save eps " + Command + "\n";      break;
		    case(3):    Command ="save monops " + Command + "\n";   break;
		    case(4):    Command ="save vectps " + Command + "\n";   break;
		    case(5):    Command ="save ppm " + Command + "\n";      break;
		    case(6):    Command ="save ppm " + Command + "\n";      break;  //WritePPMFile(name,False): no token
		    case(7):    Command ="save pict " + Command + "\n";     break;
		    case(8):    Command ="save iris " + Command + "\n";     break;
		    case(9):    Command ="save sunrle " + Command + "\n";   break;
		    case(10):   Command ="save sun " + Command + "\n";      break;
		}
	} else if( sext == "bmp" )
	{	                Command ="save bmp " + Command + "\n";  
	} else if( sext == "gif" )
	{	                Command ="save gif " + Command + "\n";
	} else if( sext == "pic" )
	{	                Command ="save pict " + Command + "\n";   
	} else if( !((fdlg.m_ofn.nFilterIndex == 10) && sext.IsEmpty()) )
	{	switch(fdlg.m_ofn.nFilterIndex -1)
		{   case(0):    Command ="save bmp " + Command + "\n";      break;
		    case(1):    Command ="save gif " + Command + "\n";      break;
		    case(2):    Command ="save eps " + Command + "\n";      break;
		    case(3):    Command ="save monops " + Command + "\n";   break;
		    case(4):    Command ="save vectps " + Command + "\n";   break;
		    case(5):    Command ="save ppm " + Command + "\n";      break;
		    case(6):    Command ="save ppm " + Command + "\n";      break;  //WritePPMFile(name,False): no token
		    case(7):    Command ="save pict " + Command + "\n";     break;
		    case(8):    Command ="save iris " + Command + "\n";     break;
		    case(9):    Command ="save sunrle " + Command + "\n";   break;
		    case(10):   Command ="save sun " + Command + "\n";      break;
		}
	}

	pView->ExecuteCommand(DisplayOff,Command);

	if( changedim ) 
	{	FixRange();
		ReDrawFlag |= RFReSize;
        RefreshState = True;
		ClearImage();
		RefreshScreen();
	}
	EndWait();
	STARTTIMER
}

void CRasDoc::OnUpdateSaveImage(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( (int)Database );	}

void CRasDoc::OnSaveMolecularfile() 
{	
    STOPTIMER
	GETVIEW
	if( pView==NULL )
	{	STARTTIMER
		return;
	}
	CDialogSaveFile dlg;
	if( dlg.DoModal()!=IDOK )
	{	STARTTIMER
		return;
	}
	CString ext;
	int num;
	switch( dlg.m_Format )
	{	case(0)	:	ext = "pdb";	num = 1;	break;
		case(1)	:	ext = "mdl";	num = 2;	break;
		case(2)	:	ext = "xyz";	num = 3;	break;
		case(3)	:	ext = "cif";	num = 4;	break;
	}

	CString Filename = GetTitle();
	int d = Filename.Find('.');
	if(d != -1) Filename = Filename.Left(d);
	if(Filename.IsEmpty()) Filename = "notitle";
	if(Filename.GetLength()>24) Filename = Filename.Left(24);
	Filename += "." + ext;

	char ofilters[512];
	char* dst;
	dst = ofilters;
    dst = RegisterFormat(dst,"Brookhaven Databank (*.pdb;*.ent)","*.pdb;*.ent");
    dst = RegisterFormat(dst,"MDL Mol File Format (*.mdl;*.mol)","*.mdl;*.mol");
    dst = RegisterFormat(dst,"MSC (XMol) XYZ Format (*.xyz)","*.xyz");
    dst = RegisterFormat(dst,"Crystallographic Information File (*.cif)","*.cif");
    dst = RegisterFormat(dst,"All Files (*.*)","*.*");
    *dst = '\0';

	CFileDialog fdlg(false, NULL, (LPCSTR) Filename, OFN_HIDEREADONLY | 
		OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT ,  NULL,  NULL);

    fdlg.m_ofn.lStructSize=sizeof(OPENFILENAME);
    fdlg.m_ofn.hwndOwner = NULL;
    fdlg.m_ofn.lpstrFilter = ofilters;
    fdlg.m_ofn.lpstrCustomFilter = NULL;
    fdlg.m_ofn.nFilterIndex = num;
    fdlg.m_ofn.lpstrDefExt = ext;
    fdlg.m_ofn.nMaxFile = 128;
    fdlg.m_ofn.lpstrFileTitle = NULL;
    fdlg.m_ofn.lpstrInitialDir = NULL;
    fdlg.m_ofn.lpstrTitle = MsgStrs(RTPSelDir);
    fdlg.m_ofn.Flags |= OFN_PATHMUSTEXIST;

	if( fdlg.DoModal()!=IDOK )
	{	STARTTIMER
		return;
	}

	CString Command = fdlg.GetPathName();
	CString sext = fdlg.GetFileExt();
	if( sext == "pdb" )
		Command ="save pdb " + Command + "\n";
	else if( sext == "mdl" )
		Command ="save mdl " + Command + "\n";
	else if( sext == "xyz" )
		Command ="save xyz " + Command + "\n"; 
	else if( sext == "cif" )
		Command ="save cif " + Command + "\n";
	else
	{	switch(dlg.m_Format)
		{	case(0)	:    Command ="save pdb " + Command + "\n";      break;
			case(1)	:    Command ="save mdl " + Command + "\n";      break;
			case(2)	:    Command ="save xyz " + Command + "\n";      break;
			case(3)	:    Command ="save cif " + Command + "\n";      break;
		}
	}
	pView->ExecuteCommand(DisplayOff,Command);
	STARTTIMER
}

void CRasDoc::OnUpdateSaveMolecularfile(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( (int)Database );	}

void CRasDoc::OnSaveScript() 
{	
    STOPTIMER
	GETVIEW
	if( pView==NULL )
	{	STARTTIMER
		return;
	}
	CDialogScript dlg;
	if( dlg.DoModal()!=IDOK )
	{	STARTTIMER
		return;
	}
	CString ext;
	int num;
	switch( dlg.m_Format )
	{	case(0)	:	ext = "scr";	num = 1;	break;
		case(1)	:	ext = "mol";	num = 2;	break;
		case(2)	:	ext = "kin";	num = 3;	break;
		case(3)	:	ext = "kin";	num = 4;	break;
		case(4)	:	ext = "pov";	num = 5;	break;
		case(5)	:	ext = "vrm";	num = 6;	break;
		case(6)	:	ext = "txt";	num = 7;	break;
		case(7)	:	ext = "txt";	num = 8;	break;
		case(8)	:	ext = "txt";	num = 9;	break;
	}
	
	CString Filename = GetTitle();
	int d = Filename.Find('.');
	if( d != -1 )
		Filename = Filename.Left(d);
	if( Filename.IsEmpty() )
		Filename = "notitle";
	if( Filename.GetLength()>24 )
		Filename = Filename.Left(24);
	Filename += "." + ext;

	char ofilters[512];
	char* dst;
	dst = ofilters;
    dst = RegisterFormat(dst,"Rasmol Script file (*.scr)","*.scr");
    dst = RegisterFormat(dst,"MolScript file (*.mol)","*.mol");
    dst = RegisterFormat(dst,"Kinemage file (*.kin)","*.kin");
    dst = RegisterFormat(dst,"Kinemage details (*.kin)","*.kin");
    dst = RegisterFormat(dst,"POVRay file (*.pov)","*.pov");
    dst = RegisterFormat(dst,"VMRL file (*.vmr)","*.vmr");
    dst = RegisterFormat(dst,"Phipsi (*.txt)","*.txt");
    dst = RegisterFormat(dst,"Ramachandran file (*.txt)","*.txt");
    dst = RegisterFormat(dst,"Ramachandran plot (*.txt)","*.txt");
    dst = RegisterFormat(dst,"All Files (*.*)","*.*");
    *dst = '\0';

	CFileDialog fdlg(false, NULL, (LPCSTR) Filename, OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | 
	OFN_OVERWRITEPROMPT ,  NULL,  NULL);

    fdlg.m_ofn.lStructSize=sizeof(OPENFILENAME);
    fdlg.m_ofn.hwndOwner = NULL;
    fdlg.m_ofn.lpstrFilter = ofilters;
    fdlg.m_ofn.lpstrCustomFilter = NULL;
    fdlg.m_ofn.nFilterIndex = num;
    fdlg.m_ofn.lpstrDefExt = ext;
    fdlg.m_ofn.nMaxFile = 128;
    fdlg.m_ofn.lpstrFileTitle = NULL;
    fdlg.m_ofn.lpstrInitialDir = NULL;
    fdlg.m_ofn.lpstrTitle = MsgStrs(RTPSelDir);
    fdlg.m_ofn.Flags |= OFN_PATHMUSTEXIST;

	if( fdlg.DoModal() != IDOK )
	{	STARTTIMER
		return;
	}
	CString Command = fdlg.GetPathName();
	switch( dlg.m_Format )
	{	
	    case(0)	:	Command ="save script " + Command + "\n";         break;
	    case(1)	:	Command ="save molscript " + Command + "\n";      break;
	    case(2)	:	Command ="save kinemage " + Command + "\n";       break;
	    case(3)	:	pView->ExecuteCommand(DisplayOff,"set kinemage on\n");
                    Command ="save kinemage " + Command + "\n";
                    pView->ExecuteCommand(DisplayOff,Command);
                    pView->ExecuteCommand(DisplayOff,"set kinemage off\n");
                    STARTTIMER
                    return;
	    case(4)	:	Command ="save povray " + Command + "\n";         break;
	    case(5)	:	Command ="save vrml " + Command + "\n";           break;
	    case(6)	:	Command ="save phipsi " + Command + "\n";         break;
	    case(7)	:	Command ="save RamachandranDataFile " + Command + "\n";   break;
	    case(8)	:	Command ="save RamachandranPrinterPlot " + Command + "\n";break;
	}
	pView->ExecuteCommand(DisplayOff,Command);
	STARTTIMER
}

void CRasDoc::OnUpdateSaveScript(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( (int)Database );	}


///////////////////////////////////////////////////////////////////////////////////

char *RegisterFormat( char *buffer, char *desc, char *ext )
{
    while( *buffer++ = *desc++ );
    while( *buffer++ = *ext++ );
    return( buffer );
}

