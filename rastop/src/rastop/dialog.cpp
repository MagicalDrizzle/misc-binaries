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
// dialog.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include <ctype.h>
#include <math.h>
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
// CDialogSetValue dialog

CDialogSetValue::CDialogSetValue(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSetValue::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSetValue)
	m_ValueTex = _T("");
	m_ValueNumber = 0;
	m_TexComment = _T("");
	m_TexValue = _T("");
	//}}AFX_DATA_INIT
	m_ValueType = Type_Ident;
}

void CDialogSetValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSetValue)
	DDX_Text(pDX, IDC_TEX_COMMENTS, m_TexComment);
	DDX_Text(pDX, IDC_TEX_VALUE, m_TexValue);
	//}}AFX_DATA_MAP
	if( m_ValueType == Type_Dots )
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
		DDV_MinMaxInt(pDX, m_ValueNumber, 1, 1000);
	} else if( m_ValueType == Type_DotSize )
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
		DDV_MinMaxInt(pDX, m_ValueNumber, 1, 20);
	} else if( m_ValueType == Type_Ribbons	)
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
		DDV_MinMaxInt(pDX, m_ValueNumber, 0, 1000);
	} else if( (m_ValueType == Type_HBonds)  ||
			   (m_ValueType == Type_SSBonds) ||
			   (m_ValueType == Type_Bonds)   )
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
		DDV_MinMaxInt(pDX, m_ValueNumber, 0, 250);
	} else if( m_ValueType == Type_Cartoons )
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
		DDV_MinMaxInt(pDX, m_ValueNumber, 1, 500);
	} else if( m_ValueType == Type_Trace )
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
		DDV_MinMaxInt(pDX, m_ValueNumber, 1, 250);
	} else if( m_ValueType == Type_Strands )
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);	
		DDV_MinMaxInt(pDX, m_ValueNumber, 1, 9);
	} else if( (m_ValueType == Type_Radius) || 
			   (m_ValueType == Type_Sphere)	) 
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
		DDV_MinMaxInt(pDX, m_ValueNumber, 0, 750);
	} else if(m_ValueType == Type_Renumber ||
			 (m_ValueType == Type_Within)	)
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
	} else if (m_ValueType == Type_CisAngle)
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
		DDV_MinMaxInt(pDX, m_ValueNumber, 0, 180);
	} else if (m_ValueType == Type_Stereo)
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
		DDV_MinMaxInt(pDX, m_ValueNumber, -180, 180);
	} else if (m_ValueType == Type_MolSize)
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueNumber);
		DDV_MinMaxInt(pDX, m_ValueNumber, 1500, 250000);
	} else
	{	DDX_Text(pDX, IDC_EDIT_VALUE, m_ValueTex);
	}
}

BEGIN_MESSAGE_MAP(CDialogSetValue, CDialog)
	//{{AFX_MSG_MAP(CDialogSetValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDialogSetValue::OnInitDialog() 
{	CDialog::OnInitDialog();
	CWnd* pWnd = GetDlgItem(IDC_EDIT_VALUE); 
	GotoDlgCtrl(pWnd);
	return false;
}

void CDialogSetValue::OnOK() 
{	
	int result;
	CDialog::OnOK();
	switch(m_ValueType)
	{	case(Type_Strands)	:	result = IsType_Strands();	break;
		case(Type_Ident)	:	result = IsType_Ident();	break;
		case(Type_Dots)		:
		case(Type_Radius)	:
		case(Type_Sphere)	:
		case(Type_Bonds)	:
		case(Type_HBonds)	:
		case(Type_SSBonds)	:
		case(Type_Ribbons)	:
		case(Type_Trace)	:
		case(Type_Cartoons)	:
		case(Type_Renumber)	:
		case(Type_Within)	:
		case(Type_Stereo)	:
		case(Type_MolSize)	:
		case(Type_CisAngle)	:	result = ConvertToTex();	break;
		default				:   CDialog::OnOK();			return;
	}

	if( !result )
	{	m_ValueTex.Empty();
		m_ValueNumber = 0;
	}
}

BOOL CDialogSetValue::IsType_Ident()
{	m_ValueTex.TrimLeft( );
	m_ValueTex.TrimRight( );
	UpdateData(false);

	for( int i=0;i<m_ValueTex.GetLength();i++ )
	{	int c = (int)m_ValueTex.GetAt(i);
		if( ((isalnum(c))||((c)=='_')||((c)=='$')) )	//IsIdentChar(c)
			continue;
		WriteString(MsgStrs(RTPErrChar));
		return false;
	}
	return true;
}

BOOL CDialogSetValue::ConvertToTex()
{	m_ValueTex.Format("%d", m_ValueNumber);
	return true;
}

int CDialogSetValue::IsType_Strands()
{	if(	m_ValueNumber == 1 ||
		m_ValueNumber == 2 ||
		m_ValueNumber == 3 ||
		m_ValueNumber == 4 ||
		m_ValueNumber == 5 ||
		m_ValueNumber == 9 	)
	{	ConvertToTex();
		return true;
	} else 
	{	WriteString(MsgStrs(RTPErrStrd));
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDialogSaveFile dialog

CDialogSaveFile::CDialogSaveFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSaveFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSaveFile)
	m_Format = 0;
	//}}AFX_DATA_INIT
}

void CDialogSaveFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSaveFile)
	DDX_Radio(pDX, IDC_SAVE_PDB, m_Format);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogSaveFile, CDialog)
	//{{AFX_MSG_MAP(CDialogSaveFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDialogScript dialog

CDialogScript::CDialogScript(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogScript::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogScript)
	m_Format = 0;
	//}}AFX_DATA_INIT
}

void CDialogScript::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogScript)
	DDX_Radio(pDX, IDC_SAVE_SCRIPT, m_Format);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogScript, CDialog)
	//{{AFX_MSG_MAP(CDialogScript)
	ON_BN_CLICKED(IDC_SAVE_SCRIPT, OnClickScript)
	ON_BN_CLICKED(IDC_SAVE_MOLSCRIPT, OnClickMolscript)
	ON_BN_CLICKED(IDC_SAVE_KINEMAGE, OnSaveKinemage)
	ON_BN_CLICKED(IDC_MAGEDETAIL, OnClickMagedetail)
	ON_BN_CLICKED(IDC_SAVE_POVRAY, OnClickPovray)
	ON_BN_CLICKED(IDC_SAVE_VMRL, OnClickVmrl)
	ON_BN_CLICKED(IDC_SAVE_PHIPSI, OnSavePhipsi)
	ON_BN_CLICKED(IDC_SAVE_RDF, OnSaveRdf)
	ON_BN_CLICKED(IDC_SAVE_RPP, OnSaveRpp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDialogScript::OnClickScript()     {	UpdateData(true);   }
void CDialogScript::OnClickMolscript()  {	UpdateData(true);   }
void CDialogScript::OnSaveKinemage()    {	UpdateData(true);   }
void CDialogScript::OnClickMagedetail() {	UpdateData(true);   }
void CDialogScript::OnClickPovray()     {	UpdateData(true);   }
void CDialogScript::OnClickVmrl()       {	UpdateData(true);   }
void CDialogScript::OnSavePhipsi()      {	UpdateData(true);   }
void CDialogScript::OnSaveRdf()         {	UpdateData(true);   }
void CDialogScript::OnSaveRpp()         {	UpdateData(true);   }


/////////////////////////////////////////////////////////////////////////////
// CDialogImage dialog

CDialogImage::CDialogImage(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogImage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogImage)
	m_Format = 0;
	m_CartoonFlag = FALSE;
	m_SizeX = XRange;
	m_SizeY = YRange;
	//}}AFX_DATA_INIT
}

void CDialogImage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogImage)
	DDX_Radio(pDX, IDC_IMAGE_BMP, m_Format);
	DDX_Check(pDX, IDC_IMAGE_ENABLEVECTPS, m_CartoonFlag);
	DDX_Text(pDX, IDC_IMAGE_SIZEX, m_SizeX);
	DDV_MinMaxInt(pDX, m_SizeX, 16, 20000);
	DDX_Text(pDX, IDC_IMAGE_SIZEY, m_SizeY);
	DDV_MinMaxInt(pDX, m_SizeY, 16, 20000);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogImage, CDialog)
	//{{AFX_MSG_MAP(CDialogImage)
	ON_BN_CLICKED(IDC_IMAGE_ENABLEVECTPS, OnImageEnablevectps)
	ON_BN_CLICKED(IDC_IMAGE_BMP, OnImageBmp)
	ON_BN_CLICKED(IDC_IMAGE_GIF, OnImageGif)
	ON_BN_CLICKED(IDC_IMAGE_EPSF, OnImageEpsf)
	ON_BN_CLICKED(IDC_IMAGE_MONOPS, OnImageMonops)
	ON_BN_CLICKED(IDC_IMAGE_PPM, OnImagePpm)
	ON_BN_CLICKED(IDC_IMAGE_PPM2, OnImagePpm2)
	ON_BN_CLICKED(IDC_IMAGE_PICT, OnImagePict)
	ON_BN_CLICKED(IDC_IMAGE_IRIS, OnImageIris)
	ON_BN_CLICKED(IDC_IMAGE_SUNRLE, OnImageSunrle)
	ON_BN_CLICKED(IDC_IMAGE_SUN, OnImageSun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDialogImage::UpdateVectors()
{	UpdateData(true);
	if( m_CartoonFlag )
	{	m_CartoonFlag = false;
		UpdateData(false);
	}
}

void CDialogImage::OnImageEnablevectps() 
{	UpdateData(true);
	if( m_CartoonFlag )
	{	m_Format = 4;
		UpdateData(false);
	}
}

void CDialogImage::OnImageBmp() 		{	UpdateVectors();	}
void CDialogImage::OnImageGif()  		{	UpdateVectors();	}
void CDialogImage::OnImageEpsf()  		{	UpdateVectors();	}
void CDialogImage::OnImageMonops()  	{	UpdateVectors();	}
void CDialogImage::OnImagePpm()  		{	UpdateVectors();	}
void CDialogImage::OnImagePpm2()  		{	UpdateVectors();	}
void CDialogImage::OnImagePict()  		{	UpdateVectors();	}
void CDialogImage::OnImageIris()  		{	UpdateVectors();	}
void CDialogImage::OnImageSunrle()  	{	UpdateVectors();	}
void CDialogImage::OnImageSun()  		{	UpdateVectors();	}

/////////////////////////////////////////////////////////////////////////////
// CEditData

CEditData::CEditData()
{	VirtKeyCtrl = false;
}

CEditData::~CEditData()	{}

BEGIN_MESSAGE_MAP(CEditData, CRichEditCtrl)
	//{{AFX_MSG_MAP(CEditData)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditData::InitDisplay()
{	
	/*Create Font*/
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = -12;
	logFont.lfWeight = FW_NORMAL;
	logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
	_ASSERT( LF_FACESIZE > lstrlen( _T("Courrier New") ) );
	lstrcpy( logFont.lfFaceName, _T("Courrier New") );
	SeqFont.CreateFontIndirect( &logFont);
	SetFont( &SeqFont );

	SetBackgroundColor(false, RGB(190,215,240));
}

void CEditData::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( VirtKeyCtrl && nChar == 0x056 )	//Ctrl+V
	{	if( OpenClipboard() )
		{	if( IsClipboardFormatAvailable(CF_TEXT) )
			{	long Start, End, Size, Selection;
				long len = 0;

				HANDLE data = GetClipboardData(CF_TEXT);
				char *ptr = (char*)GlobalLock(data);
				GlobalUnlock(data);	

				while ( *ptr++ )
					len++;
				
				Size =  GetTextLength( );
				GetSel( Start, End );
				if( Start==0 && End==-1 )
					Selection = Size;
				else
					Selection = End - Start;

				Size =  Size - Selection + len + 5000;
				if( Size > GetLimitText( ) )
					LimitText(Size);
			} else
				WriteString(MsgStrs(RTPNoClip));

			CloseClipboard();
		}
	} 
	if( nChar == 0x011 )	//Ctrl 
		VirtKeyCtrl = true;

	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CEditData::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	if( nChar == 0x011 )	//Ctrl  
		VirtKeyCtrl = false;
	
	CRichEditCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CEditData::SaveData(CString & str, CString & name )
{	char filename[128];
	char ofilters[128];
	char* src;
	char* dst;
	dst = ofilters;
    dst = RegisterFormat(dst,"All Files (*.*)","*.*");
    *dst = '\0';
	
	dst = filename;
	src = (LPTSTR) name.LockBuffer();
	while( *src )
		*dst++ = *src++;
    *dst = '\0';
	name.UnlockBuffer();

	CFileDialog fdlg( false, 
					  NULL, 
					  (LPCSTR) name, 
					  OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT,
					  NULL,  NULL );

    fdlg.m_ofn.lStructSize=sizeof(OPENFILENAME);
    fdlg.m_ofn.hwndOwner = NULL;
    fdlg.m_ofn.lpstrFilter = ofilters;
    fdlg.m_ofn.lpstrCustomFilter = NULL;
    fdlg.m_ofn.nFilterIndex = 1;
    fdlg.m_ofn.lpstrDefExt = NULL;
    fdlg.m_ofn.nMaxFile = 128;
    fdlg.m_ofn.lpstrFileTitle = filename;
    fdlg.m_ofn.lpstrInitialDir = NULL;
    fdlg.m_ofn.lpstrTitle = "Select Directory and Filename";
    fdlg.m_ofn.Flags |= OFN_PATHMUSTEXIST;

	if( fdlg.DoModal() != IDOK )
		return;

	CFile file;
	CFileException fe;
	if( !file.Open(fdlg.GetPathName(), 
					CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite,
					&fe) )
	{	WriteString(MsgStrs(RTPNoFile));
		return;
	}

	BeginWait();
	char *ptr = str.LockBuffer();
	try
	{	while( *ptr )
			file.Write(ptr++, 1);
		file.Close();
	}
	catch(CException* e)
	{	file.Abort();
		WriteString(MsgStrs(RTPSaveData));
        e->Delete();
	}
	str.UnlockBuffer();

	EndWait();
}

/////////////////////////////////////////////////////////////////////////////
// CDialogEditMolecularFile dialog

CDialogEditMolecularFile::CDialogEditMolecularFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogEditMolecularFile::IDD, pParent)
{	//{{AFX_DATA_INIT(CDialogEditMolecularFile)
	m_MoleculeName = _T("");
	m_FileFormat = _T("Text");
	//}}AFX_DATA_INIT
}

BOOL CDialogEditMolecularFile::OnInitDialog() 
{	GETFRAME
	GETVIEW
	if( pView==NULL )
		return false;
	CRasDoc* pDoc = pView->GetDocument();

	if( !CurMolSet || !CurMolSet->molfile )
		return false;

	CDialog::OnInitDialog();
	
	/*Create RichEditCtrl*/
	RECT rec;
	GetClientRect( &rec );
	rec.top = rec.top + 38;
	rec.bottom = rec.bottom - 3;
	rec.left = rec.left + 3;
	m_MolecularData.Create( WS_VISIBLE | WS_CHILD | ES_MULTILINE |
						 ES_WANTRETURN | WS_HSCROLL | WS_VSCROLL | 
						 ES_AUTOHSCROLL | ES_AUTOVSCROLL,
						 rec, this, IDC_MOLECULARDATA );
	m_MolecularData.InitDisplay();


	//Display the molecular file
	char *ptr = CurMolSet->molfile;
	int len = 0;
	while( *ptr++ ) len++;
	len += 5000;	//Extrabuffer for current writting
	m_MolecularData.LimitText( (long) len);
	m_MolecularData.SetWindowText( (LPCTSTR) CurMolSet->molfile );

	//Display title
	m_MoleculeName = CurMolSet->title;

	//Display format
	switch( CurMolSet->format )
	{	case(FormatPDB)	:		m_FileFormat = "PDB";		break;
		case(FormatAlchemy):	m_FileFormat = "Alchemy";	break;
		case(FormatMol2):		m_FileFormat = "Mol2";		break;
		case(FormatMDL)	:		m_FileFormat = "MDL";		break;
		case(FormatXYZ)	:		m_FileFormat = "XYZ";		break;
		case(FormatCharmm):		m_FileFormat = "CHARM";		break;
		case(FormatMOPAC):		m_FileFormat = "MOPAC";		break;
		case(FormatCIF):		m_FileFormat = "CIF";		break;
		case(FormatNMRPDB):		m_FileFormat = "MNR PDB";	break;
        case(FormatMacroMod):	m_FileFormat = "MacroMod";	break;
        case(FormatBiosym):		m_FileFormat = "Biosym";	break;
        case(FormatSHELX):		m_FileFormat = "SHELX";		break;
        case(FormatFDAT):		m_FileFormat = "FDAT";		break;
		default:				m_FileFormat = "Text";
	}

	//Data initialized here
	UpdateData(false);
	//Flag for modification of molecular file
	m_MolecularData.SetModify(false);

	return true;
}

void CDialogEditMolecularFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogEditMolecularFile)
	DDX_Text(pDX, IDC_MOLECULE_NAME, m_MoleculeName);
	DDV_MaxChars(pDX, m_MoleculeName, 256);
	DDX_CBString(pDX, IDC_MOLECULE_FORMAT, m_FileFormat);
	//}}AFX_DATA_MAP
}

void CDialogEditMolecularFile::OnOK() 
{	CDialog::OnOK();	}

void CDialogEditMolecularFile::OnSave() 
{	

    if( !m_MolecularData ) return;

	GETFRAME
	GETVIEW
	if( pView==NULL ) 
	{	WriteString(MsgStrs(RTPErrDialog));
		CDialog::OnCancel( );
		return;
	}

	//UpdateData
	OnOK();

	//molecular file
	CRasDoc* pDoc = pView->GetDocument();	
	CString sbuf = "";
	
	if( m_MolecularData.GetModify() )
	{	//GetWindowText does not work for very big files!
		char * ptr;
		int tlen = m_MolecularData.GetWindowTextLength( )
			+ 2*m_MolecularData.GetLineCount( );	//return charaters
		TRY
		{	ptr = (LPTSTR) sbuf.GetBuffer( tlen );
		}
		CATCH ( CMemoryException, eData )
		{	WriteString(MsgStrs(RTPErrMem));
			return;
		}
		END_CATCH

		char MolBuf[128];
		char *pMB;
	
		for( int l=0; l<m_MolecularData.GetLineCount( ); l++)
		{	pMB = MolBuf;
			int max = m_MolecularData.GetLine( l, pMB, 127 );
			for( int j = 0; j<max; j++)
				*ptr++ = *pMB++;
		}
		while(1)	//Adjust extra LF and CR
		{	ptr--;
			if( *ptr != 0x0A && *ptr != 0x0D )
				break;
		}
		*++ptr = 0x0D;
		*++ptr = 0x0A;
		*++ptr = '\0';

		sbuf.ReleaseBuffer( );

		/* Search for != with initial molfile */
		BOOL Modified = false;
		ptr =  (LPTSTR) sbuf.LockBuffer();
		char *cmp = CurMolSet->molfile;
		while( *cmp )
		{	if( *cmp!=*ptr )
			{	Modified = true;
				break;
			}
			ptr++;
			cmp++;
		}
		if( *ptr ) Modified = true;
		sbuf.ReleaseBuffer( );

		if( Modified )
		{	ptr =  (LPTSTR) sbuf.LockBuffer();
			ChangeMolFile(ptr);
            FileModified = true;
			sbuf.ReleaseBuffer( );
		}
	}

	//Title
	m_MoleculeName.TrimLeft();
	m_MoleculeName.TrimRight();
	CString temp = CurMolSet->title;
	if( m_MoleculeName!=temp ) 
	{	strcpy(CurMolSet->title,(LPCTSTR) m_MoleculeName);
		SetTitle();
	}

	//format
	int format;
	if( m_FileFormat=="Alchemy" )		format = FormatAlchemy;
	else if( m_FileFormat=="Mol2" )		format = FormatMol2;
	else if( m_FileFormat=="MDL" )		format = FormatMDL;
	else if( m_FileFormat=="XYZ" )		format = FormatXYZ;
	else if( m_FileFormat=="CHARM" )	format = FormatCharmm;
	else if( m_FileFormat=="MOPAC" )	format = FormatMOPAC;
	else if( m_FileFormat=="CIF" )		format = FormatCIF;
	else if( m_FileFormat=="MNR PDB" )	format = FormatNMRPDB;
	else if( m_FileFormat=="MacroMod" )	format = FormatMacroMod;
	else if( m_FileFormat=="Biosym" )	format = FormatBiosym;
	else if( m_FileFormat=="SHELX" )	format = FormatSHELX;
	else if( m_FileFormat=="CIF" )		format = FormatCIF;
	else if( m_FileFormat=="FDAT" )		format = FormatFDAT;
	else								format = FormatPDB;
	
	CurMolSet->format = format;

	pDoc->SetModifiedFlag(true);
	
}

void CDialogEditMolecularFile::OnProcess() 
{	GETFRAME
	GETVIEW
	if( pView==NULL ) 
	{	WriteString(MsgStrs(RTPErrDialog));
		CDialog::OnCancel( );
		return;
	}

	OnSave();
	CRasDoc* pDoc = pView->GetDocument();	
	/* need internal processing TO-DO */
	pView->RefreshView();	
}

void CDialogEditMolecularFile::OnExport() 
{
    if( !m_MolecularData ) return;

    GETFRAME
	GETVIEW
	if( pView==NULL ) 
	{	WriteString(MsgStrs(RTPErrDialog));
		CDialog::OnCancel( );
		return;
	}

	CString name = CurMolSet->title;
	name.Left( 16 );
	int p = name.Find('.');
	if( p!=-1 ) name = name.Left( p );

	UpdateData(true);

	if( m_FileFormat=="MNR PDB"			)	name += ".pdb";
	else if( m_FileFormat=="PDB"		)	name += ".pdb";
	else if( m_FileFormat=="MDL"		)	name += ".mol";
	else if( m_FileFormat=="CIF"		)	name += ".cif";
	else if( m_FileFormat=="XYZ"		)	name += ".xyz";
	else if( m_FileFormat=="Alchemy"	)	name += ".mol";
	else if( m_FileFormat=="MOPAC"		)	name += ".mop";
	else if( m_FileFormat=="CHARM"		)	name += ".chm";
	else if( m_FileFormat=="Mol2"		)	name += ".mol";
	else if( m_FileFormat=="Biosym"		)	name += ".txt";
	else if( m_FileFormat=="MacroMod"	)	name += ".txt";
	else if( m_FileFormat=="SHELX"		)	name += ".txt";
	else if( m_FileFormat=="FDAT"		)	name += ".txt";

	if( m_MolecularData.GetModify() )
	{	//GetWindowText does not work for very big files!
		CString sbuf = "";
		char * pBuf;
		int tlen = m_MolecularData.GetWindowTextLength( )
			+ 2*m_MolecularData.GetLineCount( );	//return charaters
		TRY
		{	pBuf = (LPTSTR) sbuf.GetBuffer( tlen );
		}
		CATCH ( CMemoryException, eData)
		{	WriteString(MsgStrs(RTPErrMem));
			return;
		}
		END_CATCH

		char MolBuf[128];
		char *pMB;
	
		for( int l=0; l<m_MolecularData.GetLineCount( ); l++)
		{	pMB = MolBuf;
			int max = m_MolecularData.GetLine( l, pMB, 127 );
			for( int j = 0; j<max; j++)
				*pBuf++ = *pMB++;
		}
		while(1)	//Adjust extra LF and CR
		{	pBuf--;
			if( *pBuf != 0x0A && *pBuf != 0x0D )
				break;
		}
		*++pBuf = 0x0D;
		*++pBuf = 0x0A;
		*++pBuf = '\0';

		sbuf.ReleaseBuffer( );
		m_MolecularData.SaveData(sbuf,name);
	} else
	{	CString temp = CurMolSet->molfile;
		m_MolecularData.SaveData( temp, name);
	}
}

BEGIN_MESSAGE_MAP(CDialogEditMolecularFile, CDialog)
	//{{AFX_MSG_MAP(CDialogEditMolecularFile)
	ON_BN_CLICKED(ID_SAVE, OnSave)
	ON_BN_CLICKED(ID_PROCESS, OnProcess)
	ON_BN_CLICKED(ID_EXPORT, OnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CColorPicker

COLORREF CColorPicker::RasMolColors[] = {
    RGB(   0,   0,   0 ),  /* Black      */
    RGB(   0,   0, 255 ),  /* Blue       */
    RGB( 175, 214, 255 ),  /* BlueTint   */
    RGB( 175, 117,  89 ),  /* Brown      */
    RGB(   0, 255, 255 ),  /* Cyan       */
    RGB( 255, 156,   0 ),  /* Gold       */
    RGB( 125, 125, 125 ),  /* Gray       */
    RGB(   0, 255,   0 ),  /* Green      */
    RGB(  46, 139,  87 ),  /* GreenBlue  */
    RGB( 152, 255, 179 ),  /* GreenTint  */
    RGB( 255,   0, 101 ),  /* HotPink    */
    RGB( 255,   0, 255 ),  /* Magenta    */
    RGB( 255, 165,   0 ),  /* Orange     */
    RGB( 255, 101, 117 ),  /* Pink       */
    RGB( 255, 171, 187 ),  /* PinkTint   */
    RGB( 160,  32, 240 ),  /* Purple     */
    RGB( 255,   0,   0 ),  /* Red        */
    RGB( 255,  69,   0 ),  /* RedOrange  */
    RGB(   0, 250, 109 ),  /* SeaGreen   */
    RGB(  58, 144, 255 ),  /* SkyBlue    */
    RGB( 238, 130, 238 ),  /* Violet     */
    RGB( 255, 255, 255 ),  /* White      */
    RGB( 255, 255,   0 ),  /* Yellow     */
    RGB( 246, 246, 117 )   /* YellowTint */
	};

CColorPicker::CColorPicker() {
	Initialized = false;
}
CColorPicker::~CColorPicker() {}

BEGIN_MESSAGE_MAP(CColorPicker, CComboBox)
	//{{AFX_MSG_MAP(CColorPicker)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CColorPicker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{   if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	Init();
	return false;
}

void CColorPicker::PreSubclassWindow() 
{   Init();
	CComboBox::PreSubclassWindow();
    SetCurSel( 0 );	
}

void CColorPicker::Init()
{   int index = -1;
    if (Initialized)
		return;
	for (int col = 0; col < sizeof(RasMolColors)/sizeof(COLORREF); col++) {
        int result = AddString("ok");
		if (result==CB_ERRSPACE)
			break;
        else
            SetItemData(col, RasMolColors[col]);
    }
	Initialized = true;
}

void CColorPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dcContext;
	CBrush bbrush;
	bbrush.CreateStockObject(BLACK_BRUSH);
	
	if (!dcContext.Attach(lpDrawItemStruct->hDC))
		return;

	/* Store */
	COLORREF TextColor = dcContext.GetTextColor();
	COLORREF BkColor = dcContext.GetBkColor();

	if (lpDrawItemStruct->itemAction & ODA_FOCUS) {
		dcContext.DrawFocusRect(&lpDrawItemStruct->rcItem);
	} else if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE) {
		if (lpDrawItemStruct->itemState & ODS_FOCUS)
			dcContext.DrawFocusRect(&lpDrawItemStruct->rcItem);
		else
			dcContext.ExtTextOut(0, 0, ETO_OPAQUE, &lpDrawItemStruct->rcItem, _T(""), 0, NULL);
	}

	if ((int)lpDrawItemStruct->itemID >= 0) {
		::InflateRect(&lpDrawItemStruct->rcItem, -2, -2);
		dcContext.FillSolidRect(&lpDrawItemStruct->rcItem, (COLORREF)lpDrawItemStruct->itemData);
		dcContext.FrameRect(&lpDrawItemStruct->rcItem, &bbrush);
	}

	/* Restore */
	dcContext.SetTextColor(TextColor);
	dcContext.SetBkColor(BkColor);
	dcContext.Detach();
}

COLORREF CColorPicker::getSelectedColor()
{   int n = GetCurSel();
    if (CB_ERR == n)
		return RGB(0, 0, 0);
	return RasMolColors[n];
}


void CColorPicker::setColor( COLORREF col )
{   int i = 0;
    for (i=0; i < sizeof(RasMolColors)/sizeof(COLORREF); i++) {
        if (RasMolColors[i] == col)
        {   SetCurSel(i);	
            return;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// CDialogPreference dialog

CDialogPreference::CDialogPreference(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPreference::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogPreference)
	m_ZoomEdit = 0;
	m_SequenceFalse = !m_SequenceTrue;
	m_SequenceTrue = true;
	m_HelpSource = _T("");
	m_DefaultRepres = _T("");
	m_coord_mol = false;
	m_coord_world = false;
	m_coord_abs = false;
	m_coord_screen = false;
	//}}AFX_DATA_INIT

}

BOOL CDialogPreference::OnInitDialog() {
    CDialog::OnInitDialog();
    m_cmndlinetextbox.SubclassDlgItem(IDC_PREFS_CMDCOL,this);
    m_cmndlinebkgbox.SubclassDlgItem(IDC_PREFS_CMDCOLBKG,this);

    GETFRAME
    COLORREF color = pFrame->m_wndCmdWindow.getTxtColor();	
    m_cmndlinetextbox.setColor(color);
    color = pFrame->m_wndCmdWindow.getBkgColor();	
    m_cmndlinebkgbox.setColor(color);
    return true;
}


void CDialogPreference::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPreference)
	DDX_Text(pDX, IDC_ZOOMEDIT, m_ZoomEdit);
	DDV_MinMaxInt(pDX, m_ZoomEdit, 1, 10);
	DDX_Check(pDX, ID_MOLECULE_SEQ_FALSE, m_SequenceFalse);
	DDX_Check(pDX, ID_MOLECULE_SEQ_TRUE, m_SequenceTrue);
	DDX_Text(pDX, IDC_EDIT_HELPSOURCE, m_HelpSource);
	DDV_MaxChars(pDX, m_HelpSource, 245);
	DDX_Text(pDX, IDC_EDIT_DEFAULTREPRES, m_DefaultRepres);
	DDV_MaxChars(pDX, m_DefaultRepres, 240);
	DDX_Check(pDX, ID_COORD_MOL, m_coord_mol);
	DDX_Check(pDX, ID_COORD_WORLD, m_coord_world);
	DDX_Check(pDX, ID_COORD_ABS, m_coord_abs);
	DDX_Check(pDX, ID_COORD_SCREEN, m_coord_screen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogPreference, CDialog)
	//{{AFX_MSG_MAP(CDialogPreference)
	ON_BN_CLICKED(ID_MOLECULE_SEQ_FALSE, OnMoleculeSeqFalse)
	ON_BN_CLICKED(ID_MOLECULE_SEQ_TRUE, OnMoleculeSeqTrue)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowseHelp)
	ON_BN_CLICKED(IDC_BROWSE_DEFAULTREPRES, OnBrowseDefaultRepres)
	ON_BN_CLICKED(ID_COORD_ABS, OnCoordAbs)
	ON_BN_CLICKED(ID_COORD_MOL, OnCoordMol)
	ON_BN_CLICKED(ID_COORD_WORLD, OnCoordWorld)
	ON_BN_CLICKED(ID_COORD_SCREEN, OnCoordScreen)
	ON_CBN_SELCHANGE(IDC_PREFS_CMDCOL, OnSelchangePrefsCmdcol)
	ON_CBN_SELCHANGE(IDC_PREFS_CMDCOLBKG, OnSelchangePrefsCmdcolbkg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDialogPreference::OnSelchangePrefsCmdcol() 
{
    COLORREF newtextcolor = m_cmndlinetextbox.getSelectedColor();
    GETFRAME
    pFrame->m_wndCmdWindow.ChangeTxtColor(newtextcolor);
}
	

void CDialogPreference::OnSelchangePrefsCmdcolbkg() 
{
    COLORREF newbkgcolor = m_cmndlinebkgbox.getSelectedColor();	
    GETFRAME
    pFrame->m_wndCmdWindow.ChangeBkgColor(newbkgcolor);
}

void CDialogPreference::OnMoleculeSeqTrue() 
{	UpdateData( true);
	m_SequenceFalse = false;
	m_SequenceTrue = true;
	UpdateData( false);
}

void CDialogPreference::OnMoleculeSeqFalse() 
{	UpdateData(true);
	m_SequenceFalse = true;
	m_SequenceTrue = false;
	UpdateData(false);
}

void CDialogPreference::OnCoordAbs() 
{	UpdateData(true);
	m_coord_mol = false;
	m_coord_world = false;
	m_coord_abs = true;
	m_coord_screen = false;
	UpdateData(false);
}

void CDialogPreference::OnCoordMol() 
{	UpdateData(true);
	m_coord_mol = true;
	m_coord_world = false;
	m_coord_abs = false;
	m_coord_screen = false;
	UpdateData(false);
}

void CDialogPreference::OnCoordWorld() 
{	UpdateData(true);
	m_coord_mol = false;
	m_coord_world = true;
	m_coord_abs = false;
	m_coord_screen = false;
	UpdateData(false);
}


void CDialogPreference::OnCoordScreen() 
{	UpdateData(true);
	m_coord_mol = false;
	m_coord_world = false;
	m_coord_abs = false;
	m_coord_screen = true;
	UpdateData(false);
}

void CDialogPreference::OnBrowseHelp() 
{	GETFRAME

	CFileDialog help(true, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | 
	OFN_PATHMUSTEXIST,  NULL,  NULL);

	if ( help.DoModal()==IDOK )
	{	m_HelpSource = help.GetPathName();
		m_HelpSource = help.GetPathName();
		if( m_HelpSource.GetLength()>245 )
		{	WriteString(MsgStrs(RTPErrPath));
			m_HelpSource.Empty();
		}

		UpdateData(false);
	}
}

void CDialogPreference::OnBrowseDefaultRepres() 
{	GETFRAME

	CFileDialog script(true, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | 
	OFN_PATHMUSTEXIST,  NULL,  NULL);

	if ( script.DoModal()==IDOK )
	{	m_DefaultRepres = script.GetPathName();
		if( m_DefaultRepres.GetLength()>240 )
		{	WriteString(MsgStrs(RTPErrPath));
			m_DefaultRepres.Empty();
		}

		UpdateData(false);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDialogLabel dialog


CDialogLabel::CDialogLabel(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogLabel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogLabel)
	m_nFontSize = 8;
	m_sLabelFormat = _T("");
	m_FontStroke = 0;
	m_FontFS = 0;
	//}}AFX_DATA_INIT
}

BOOL CDialogLabel::OnInitDialog() 
{	CDialog::OnInitDialog();
	CWnd* pWnd = CWnd::GetDlgItem(IDC_LABEL_FORMAT);
	pWnd->SetFocus();
	return false;
}

void CDialogLabel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogLabel)
	DDX_Text(pDX, IDC_LABEL_FONTSIZE, m_nFontSize);
	DDV_MinMaxInt(pDX, m_nFontSize, 2, 48);
	DDX_Text(pDX, IDC_LABEL_FORMAT, m_sLabelFormat);
	DDV_MaxChars(pDX, m_sLabelFormat, 127);
	DDX_Text(pDX, IDC_LABEL_STROKE, m_FontStroke);
	DDV_MinMaxInt(pDX, m_FontStroke, 0, 8);
	DDX_Radio(pDX, IDC_LABEL_FS, m_FontFS);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogLabel, CDialog)
	//{{AFX_MSG_MAP(CDialogLabel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogSelect dialog

CDialogSelect::CDialogSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSelect)
	m_sExpr = _T("");
	m_nCommand = SELECTION_EMPTY;
	//}}AFX_DATA_INIT
}

void CDialogSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSelect)
	DDX_Control(pDX, IDC_EXPRESSION, m_EditSelection);	
	DDX_Text(pDX, IDC_EXPRESSION, m_sExpr);
	DDV_MaxChars(pDX, m_sExpr, 127);
	//}}AFX_DATA_MAP

	m_EditSelection.SetSel(0,-1,true);
}

BEGIN_MESSAGE_MAP(CDialogSelect, CDialog)
	//{{AFX_MSG_MAP(CDialogSelect)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_EXP_NEW, OnNewSelection)
	ON_COMMAND(ID_EXP_ADD, OnAddSelection)
	ON_COMMAND(ID_EXP_REMOVE, OnRemoveSelection)
	ON_COMMAND(ID_EXP_EXTRACT, OnExtractSelection)
	ON_COMMAND(ID_EXP_EXCLUDE, OnExcludeSelection)
	
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
END_MESSAGE_MAP()

BOOL CDialogSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_DlgToolBar.Create(this);
	m_DlgToolBar.LoadToolBar(IDR_TOOLBAR_EXPRESSION);
	m_DlgToolBar.ShowWindow(SW_SHOW);

	m_DlgToolBar.SetBarStyle(CBRS_ALIGN_TOP|CBRS_BORDER_BOTTOM|CBRS_TOOLTIPS|CBRS_FLYBY);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	EnableToolTips(TRUE);

	GotoDlgCtrl(&m_EditSelection);
	return false;
}

BOOL CDialogSelect::OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID =pNMHDR->idFrom;
    if( pTTT->uFlags & TTF_IDISHWND )
        nID = ::GetDlgCtrlID((HWND)nID);
	if( nID )
	{
		pTTT->lpszText = MAKEINTRESOURCE(nID);
		pTTT->hinst = AfxGetResourceHandle();
		return(true);
	}
    return( false );
} 

void CDialogSelect::OnNewSelection()
{	m_nCommand = SELECTION_NEW;
	CDialog::OnOK();
}

void CDialogSelect::OnAddSelection()
{	m_nCommand = SELECTION_ADD;
	CDialog::OnOK();
}

void CDialogSelect::OnRemoveSelection()
{	m_nCommand = SELECTION_REMOVE;
	CDialog::OnOK();
}

void CDialogSelect::OnExtractSelection()
{	m_nCommand = SELECTION_EXTRACT;
	CDialog::OnOK();
}

void CDialogSelect::OnExcludeSelection()
{	m_nCommand = SELECTION_EXCLUDE;
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CDialogCommand dialog

CDialogCommand::CDialogCommand(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogCommand::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogCommand)
//	m_Command = _T("");
	//}}AFX_DATA_INIT
}

void CDialogCommand::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogCommand)
	DDX_Control(pDX, IDC_EDIT_VALUE, m_EditCommand);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_Command);
	DDV_MaxChars(pDX, m_Command, 16384);
	//}}AFX_DATA_MAP

	m_EditCommand.SetSel(0,-1,true);
}

BEGIN_MESSAGE_MAP(CDialogCommand, CDialog)
	//{{AFX_MSG_MAP(CDialogCommand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDialogCommand::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GotoDlgCtrl( &m_EditCommand );
	return false;
}



/////////////////////////////////////////////////////////////////////////////
// CDialogRasMol dialog
/////////////////////////////////////////////////////////////////////////////


CDialogRasMol::CDialogRasMol(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRasMol::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRasMol)
	//}}AFX_DATA_INIT
	m_ShowType = DLG_INFO;
}

void CDialogRasMol::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRasMol)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogRasMol, CDialog)
	//{{AFX_MSG_MAP(CDialogRasMol)
	ON_BN_CLICKED(ID_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDialogRasMol::OnInitDialog() 
{	CDialog::OnInitDialog();

	/*Execute show command*/
	GETFRAME
	GETVIEW
	if( pView==NULL )
		return true;
	pFrame->m_szCommandText.Empty();
    InvalidateCmndLine();
	pFrame->OutputLock = 1;

	switch( m_ShowType )
	{case( DLG_INFO ):	pView->ExecuteCommand(DisplayOff,"show information\n");
						break;
	 case( DLG_SEQ ):	pView->ExecuteCommand(DisplayOff,"show sequence\n");
						break;
	 case( DLG_PHIPSI ):pView->ExecuteCommand(DisplayOff,"show phipsi\n");
						break;
	 case( DLG_RPP ):	pView->ExecuteCommand(DisplayOff,"show RPP\n");
						break;
	 case( DLG_COOR ):	pView->ExecuteCommand(DisplayOff,"show selected coord\n");
						break;
	 case( DLG_CHN ):	pView->ExecuteCommand(DisplayOff,"show selected chain\n");
						break;
	 case( DLG_GRP ):	pView->ExecuteCommand(DisplayOff,"show selected group\n");
						break;
	 case( DLG_ATM ):	pView->ExecuteCommand(DisplayOff,"show selected atom\n");
						break;
	 case( DLG_SYM ):	pView->ExecuteCommand(DisplayOff,"show symmetry\n");
						break;
	 case( DLG_WORLD ):	pView->ExecuteCommand(DisplayOff,"show world\n");
						break;
	 default:			return true;
	}

	WriteChar(EOF);	//to close the data transfer

	/*Create RichEditCtrl*/
	RECT rec;
	GetClientRect( &rec );
	rec.top = rec.top + 38;
	rec.bottom = rec.bottom - 3;
	rec.left = rec.left + 3;
	m_Commandline.Create( WS_VISIBLE | WS_CHILD | ES_MULTILINE |
						  WS_HSCROLL | WS_VSCROLL |
						  ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_WANTRETURN,
						  rec, this, IDC_SHOWDATA );
	m_Commandline.InitDisplay();

	/*Display the text*/
	int len = pFrame->m_szCommandText.GetLength();
	len += 5000;	//extrabuffer for editing
 	m_Commandline.LimitText( (long) len);
	m_Commandline.SetWindowText( (LPCTSTR) pFrame->m_szCommandText );

    // Reset transmission
    pFrame->m_szCommandText = "";
    pFrame->OutputLock = 0;


	return true; 
}

void CDialogRasMol::OnOK() 
{	CDialog::OnOK();
	GETFRAME
	pFrame->m_szCommandText.Empty();
}

void CDialogRasMol::OnSave() 
{	
	GETFRAME
	GETVIEW
	if( pView == NULL )
		return;
	CRasDoc* pDoc = pView->GetDocument();

	CString name;
	if( *Info.identcode )
		name = Info.identcode;
	else
		name = pDoc->GetTitle();
	name.Left( 16 );
	int p = name.Find('.');
	if( p != -1 )
		name = name.Left( p );

	switch( m_ShowType )
	{	case( DLG_INFO ):	name += "_info.txt";
							break;
		case( DLG_SEQ ):	name += ".seq";
							break;
		case( DLG_PHIPSI ):	name += "_phipsi.txt";
							break;
		case( DLG_RPP ):	name += "_rmcd.txt";
							break;
		case( DLG_COOR ):	name += ".coor";
							break;
		case( DLG_CHN ):
		case( DLG_GRP ):
		case( DLG_ATM ):	name += "_select.txt";
							break;
		case( DLG_SYM ):	name += "_sym.txt";
							break;
		default:			name += "_info.txt";
	}

	CString sbuf;
	m_Commandline.GetWindowText( sbuf );
	m_Commandline.SaveData( sbuf , name );
}



/////////////////////////////////////////////////////////////////////////////
// CDialogChangeLightSource dialog
/////////////////////////////////////////////////////////////////////////////

CDialogChangeLightSource::CDialogChangeLightSource(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogChangeLightSource::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogChangeLightSource)
	m_lix = 1;
	m_liy = 1;
	m_liz = 2;
	//}}AFX_DATA_INIT
}


void CDialogChangeLightSource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CDialogChangeLightSource)
	DDX_Text(pDX, IDC_LIGHTXBUDDY, m_lix);
	DDV_MinMaxInt(pDX, m_lix, -100, 100);
	DDX_Text(pDX, IDC_LIGHTYBUDDY, m_liy);
	DDV_MinMaxInt(pDX, m_liy, -100, 100);
	DDX_Text(pDX, IDC_LIGHTZBUDDY, m_liz);
	DDV_MinMaxInt(pDX, m_liz, -100, 100);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CDialogChangeLightSource, CDialog)
	//{{AFX_MSG_MAP(CDialogChangeLightSource)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_LIGHT_DEFAULT, OnLightDefault)
	ON_BN_CLICKED(ID_APPLY_NOW, OnApplyNow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void Norm100( int &dx, int &dy, int &dz, int sx, int sy, int sz )
{	double nor;

	nor = sqrt( (double)sx*sx + (double)sy*sy + (double)sz*sz );

	dx = (int)(100*(double)sx/nor);
	dy = (int)(100*(double)sy/nor);
	dz = (int)(100*(double)sz/nor);
}

void CDialogChangeLightSource::ExecuteCommand() 
{	GETFRAME
	GETVIEW
	if( pView==NULL )
		return;
	
	CString Command;
	Command.Format("set lightsource [%d,%d,%d]\n",m_lix,m_liy,-m_liz);
	pView->ExecuteCommand( DisplayOn, Command);
}

BOOL CDialogChangeLightSource::OnInitDialog() 
{
	m_lightx.SubclassDlgItem(IDC_LIGHTX, this);
	m_lightx.SetRange( -1, 1);
	
	m_lighty.SubclassDlgItem(IDC_LIGHTY, this);
	m_lighty.SetRange( -1, 1);
	
	m_lightz.SubclassDlgItem(IDC_LIGHTZ, this);
	m_lightz.SetRange( -1, 1);

	m_lightx.SetPos( 0 );
	m_lighty.SetPos( 0 );
	m_lightz.SetPos( 0 );

	Norm100( m_lix, m_liy, m_liz, LightX, LightY, LightZ);

	CDialog::OnInitDialog();
	return true;
}


void CDialogChangeLightSource::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	if( nSBCode==8 )
	{	int ip = ((int)nPos<0)? -1:1;
		int id = pScrollBar->GetDlgCtrlID();
		int tempx,tempy,tempz;
		double theta = (double)-ip*PI/20;

		if( id==IDC_LIGHTX )
		{	tempx = LightX;
			tempy = (int)( LightY*cos(theta) - LightZ*sin(theta) );
			tempz = (int)( LightY*sin(theta) + LightZ*cos(theta) );
		} else if( id==IDC_LIGHTY )
		{	tempx = (int)( LightX*cos(theta) - LightZ*sin(theta) );
			tempy = LightY;
			tempz = (int)( LightX*sin(theta) + LightZ*cos(theta) );
		} else if( id==IDC_LIGHTZ )
		{	tempx = (int)( LightX*cos(theta) - LightY*sin(theta) );
			tempy = (int)( LightX*sin(theta) + LightY*cos(theta) );
			tempz = LightZ;
		}

		Norm100( m_lix, m_liy, m_liz, tempx, tempy, tempz);
		ExecuteCommand();
		Norm100( m_lix, m_liy, m_liz, LightX, LightY, LightZ);
		UpdateData(false);
		m_lightx.SetPos( 0 );
		m_lighty.SetPos( 0 );
		m_lightz.SetPos( 0 );
	}
}


void CDialogChangeLightSource::OnLightDefault() 
{	GETFRAME
	GETVIEW
	if( pView==NULL )
		return;
	pView->ExecuteCommand( DisplayOn, "set headlight default\n");

	Norm100( m_lix, m_liy, m_liz, LightX, LightY, LightZ);
	UpdateData(false);
}


void CDialogChangeLightSource::OnApplyNow() 
{	UpdateData(true);
	Norm100( m_lix, m_liy, m_liz, m_lix, m_liy, m_liz);
	ExecuteCommand();
	Norm100( m_lix, m_liy, m_liz, LightX, LightY, LightZ);
	UpdateData(false);
}



/////////////////////////////////////////////////////////////////////////////
// CDialogLoadPrefs dialog
/////////////////////////////////////////////////////////////////////////////

CDialogLoadPrefs::CDialogLoadPrefs(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogLoadPrefs::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogLoadPrefs)
	m_PrefsPath = _T("");
	//}}AFX_DATA_INIT
}


void CDialogLoadPrefs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogLoadPrefs)
	DDX_Text(pDX, IDC_EDIT_LOADPREFS, m_PrefsPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogLoadPrefs, CDialog)
	//{{AFX_MSG_MAP(CDialogLoadPrefs)
	ON_BN_CLICKED(IDC_BROWSE_LOADPREFS, OnBrowseLoadprefs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDialogLoadPrefs::OnBrowseLoadprefs() 
{	GETFRAME

	CFileDialog script(true, NULL, m_PrefsPath, OFN_HIDEREADONLY | 
	OFN_PATHMUSTEXIST,  NULL,  NULL);

	if ( script.DoModal()==IDOK )
	{	m_PrefsPath = script.GetPathName();
		if( m_PrefsPath.GetLength()>240 )
		{	WriteString(MsgStrs(RTPErrPath));
			m_PrefsPath.Empty();
		}

		UpdateData(false);
	}
}

