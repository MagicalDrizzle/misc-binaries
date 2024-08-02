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
// ColorBar.cpp

#include "stdafx.h"
#include "rastop.h"
#include "colorbar.h"
#include "rasdoc.h"

#define ITEMBOX_INDEX	0
#define ITEMBOX_WIDTH	114
#define ITEMBOX_HEIGHT  240

/////////////////////////////////////////////////////////////////////////////
//Couleurs

ColourTable CColorBar::m_crColours[] = 
{
    { RGB( 255,  255,  255),    _T("No Color")          },
    { RGB( 255,  255,  255),    _T("White")             },
    { RGB( 192,  192,  192),    _T("Light Grey")        },
    { RGB( 150,  150,  150),    _T("Grey - 40%")        },
    { RGB( 100,  100,  100),    _T("Dark Grey")         },
    { RGB(   0,    0,    0),    _T("Black")             },

    { RGB(   0,   51,  102),    _T("Dark Teal")         },
    { RGB(   0,    0,  128),    _T("Dark Blue")         },
    { RGB(  51,   51,  153),    _T("Indigo")            },

    { RGB( 102,  102,  153),    _T("Blue-Grey")         },
    { RGB( 128,    0,  128),    _T("Violet")            },   
    { RGB( 128,    0,    0),    _T("Dark Red")          },

    { RGB(   0,    0,  255),    _T("Blue")              },
    { RGB(   0,  204,  255),    _T("Sky Blue")          },
    { RGB( 153,  204,  255),    _T("Pale Blue")         },

    { RGB( 204,  153,  255),    _T("Lavender")          },
    { RGB( 153,   51,  102),    _T("Plum")              },
    { RGB( 153,   51,    0),    _T("Brown")             },

    { RGB(  51,  102,  255),    _T("Light Blue")        },
    { RGB(   0,  255,  255),    _T("Turquoise")         },
    { RGB( 204,  255,  255),    _T("Light Turquoise")   },

    { RGB( 230,  230,  250),    _T("Pale Violet")       },
    { RGB( 255,    0,  255),    _T("Pink")              },
    { RGB( 255,    0,    0),    _T("Red")               },

    { RGB(   0,  128,  128),    _T("Teal")              },
    { RGB(  51,  204,  204),    _T("Aqua")              },
    { RGB( 204,  255,  204),    _T("Light Green ")      },

    { RGB( 255,  255,  153),    _T("Light Yellow")      },
    { RGB( 255,  153,  204),    _T("Rose")              },
    { RGB( 255,  102,    0),    _T("Orange")            },

    { RGB(   0,  128 ,   0),    _T("Green")             },
    { RGB(  51,  153,  102),    _T("Sea Green")         },
    { RGB(   0,  255,    0),    _T("Bright Green")      },

    { RGB( 255,  255,    0),    _T("Yellow")            }, 
    { RGB( 255,  204,  153),    _T("Tan")               },
    { RGB( 255,   153,   0),    _T("Light Orange")      },

    { RGB(   0,   51,    0),    _T("Dark Green")        },
    { RGB(  51,   51,    0),    _T("Olive Green")       },
    { RGB( 128,  128,    0),    _T("Dark Yellow")       },

    { RGB( 153,  204,    0),    _T("Lime")              },
    { RGB( 255,  204,    0),    _T("Gold")              },
    { RGB( 255,  255,  255),    _T("More Colors...")    },

};


/////////////////////////////////////////////////////////////////////////////
//Items and color ID
typedef struct {
    TCHAR	*ItemName;
    UINT	nID;
} ItemTable;

#define MAXITEM		13
static ItemTable ItemList[MAXITEM] = 
{	
	{ _T("atoms"),			ID_COLOR_ATOM				},
	{ _T("bonds"),			ID_COLOR_BONDS				},
	{ _T("H bonds"),		ID_COLOR_HBONDS				},
	{ _T("SS bonds"),		ID_COLOR_SSBONDS			},
	{ _T("ribbons"),		ID_RIBBONS_COLOR			},
	{ _T("central strands"),ID_RIBBONS_COLOR_INSIDE		},
	{ _T("outer strand"),	ID_RIBBONS_COLOR_OUTSIDE	},
	{ _T("dots"),			ID_COLOR_DOTS				},
	{ _T("labels"),			ID_COLOR_LABELS				},
	{ _T("monitors"),		ID_MONITOR_COLOUR			},
	{ _T("world axes"),		ID_COLOR_AXE				},
	{ _T("axes & boxes"),	ID_COLOR_BOX				},
	{ _T("background"),		ID_COLOR_BACKGROUND			},
};

/////////////////////////////////////////////////////////////////////////////
// CColorBar


BEGIN_MESSAGE_MAP(CColorBar, CToolBar)
	//{{AFX_MSG_MAP(CColorBar)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_LBUTTONUP()
	ON_WM_NCLBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CColorBar::CColorBar()	{}

CColorBar::~CColorBar()	
{   ::DeleteObject(PixTool);
	free(BitToolInfo);
	GlobalFree(TBufHandle);
}

BOOL CColorBar::Init(CWnd* pParentWnd)
{
    int i = 0;
	DWORD dwStyle = WS_CHILD | CBRS_FLOAT_MULTI | CBRS_SIZE_FIXED |
		CBRS_TOOLTIPS | CBRS_FLYBY;

	if( !Create(pParentWnd, dwStyle, IDW_COLORBAR) )
		return false;

	ModifyStyle(0, TBSTYLE_FLAT);

	//Set the buttons
	if( !SetButtons(NULL,MAX_COLORS+2) )
		return false;

	//Set info first line
	SetButtonInfo(ITEMBOX_INDEX /*=0*/, ID_SEPARATOR, TBBS_SEPARATOR | TBBS_WRAPPED, 114 );
	SetButtonInfo(1, ID_HIDDEN_SEPARATOR, TBBS_SEPARATOR | TBBS_WRAPPED, 9 );

	//Set info for color buttons
	for(i=0;i<MAX_COLORS;i++ )
	{	SetButtonInfo(i+2,ID_COLOR+i,TBBS_BUTTON,i);
		UINT nStyle = GetButtonStyle(i+2);
		if( (i+2)%6==1 )
			nStyle |= TBBS_WRAPPED;
		else
			nStyle &= ~TBBS_WRAPPED;
    	SetButtonStyle(i+2,nStyle);
	}

    //Load bitmap
	if( !MakePalette() )
		return false;
    BOOL b = SetBitmap(PixTool);

	//Create combobox for item list
	CRect rect(-ITEMBOX_WIDTH, -ITEMBOX_HEIGHT, 0, 0);
	if( !m_ColorItem.Create(WS_CHILD | CBS_DROPDOWNLIST |
		CBS_AUTOHSCROLL | WS_VSCROLL | CBS_HASSTRINGS, rect, this,
		IDC_ITEMBOX) )
		return false;
	
	//Create font
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if( hFont==NULL )
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
  	m_ColorItem.SendMessage(WM_SETFONT, (WPARAM)hFont);
  	m_ColorItem.SendMessage(CB_SETITEMHEIGHT,-1,17);

	//Set info and show the box
	SetButtonInfo(ITEMBOX_INDEX, IDC_ITEMBOX, TBBS_SEPARATOR, ITEMBOX_WIDTH);
	if( m_ColorItem.m_hWnd!=NULL )
	{	CRect rect;
		GetItemRect(ITEMBOX_INDEX, rect);
		m_ColorItem.SetWindowPos(NULL, rect.left, rect.top, 0, 0, 
			SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS);
		m_ColorItem.ShowWindow(SW_SHOW);
	}

	//Fill the box with items
	for(i=0;i<MAXITEM;i++ )
		m_ColorItem.InsertString( -1, ItemList[i].ItemName);
	m_ColorItem.SetCurSel( 0 );

	//Set size
	SIZE sizeb, sizei;
	sizeb.cx = 19;
	sizeb.cy = 18; 
	sizei.cx = 12;
	sizei.cy = 12;	
	SetSizes(sizeb,sizei);

	//Tooltips
	m_tooltip.Create(this);
	m_tooltip.Activate(true);
	m_tooltip.SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE);
	m_tooltip.SetDelayTime( (UINT) 1000 );

	for( i=0;i<MAX_COLORS;i++ )
	{	GetItemRect( i+2, &rect );
		m_tooltip.AddTool(this,m_crColours[i].szName, &rect,i+1);
	}
	GetToolBarCtrl( ).SetToolTips(&m_tooltip);


    CString s;
    s.LoadString(IDS_LOCAL_COLWND);
	SetWindowText(_T(s));
	EnableDocking(0);

	return true;
}

int CColorBar::MakePalette( void )
{	long *buf,*ptr,*end;
	long fill;
	long 
	int size,i,j,k; 
    HDC hDC;

    hDC = ::GetDC(NULL);
        
    size = sizeof(BITMAPINFOHEADER);
    BitToolInfo = (BITMAPINFO *)malloc(size);
	if( !BitToolInfo )
		return NULL;

    BitToolInfo->bmiHeader.biSize = size;
    BitToolInfo->bmiHeader.biCompression = BI_RGB;
    BitToolInfo->bmiHeader.biXPelsPerMeter = 0;
    BitToolInfo->bmiHeader.biYPelsPerMeter = 0;
    BitToolInfo->bmiHeader.biClrImportant = 0;
    BitToolInfo->bmiHeader.biSizeImage = 0;
    BitToolInfo->bmiHeader.biBitCount = 32;
    BitToolInfo->bmiHeader.biPlanes = 1;
    BitToolInfo->bmiHeader.biClrUsed = 0;
    BitToolInfo->bmiHeader.biWidth = 12*MAX_COLORS;
    BitToolInfo->bmiHeader.biHeight = 12;

    size = (long)12*12*MAX_COLORS*sizeof(long);
    TBufHandle = GlobalAlloc(GHND,size);
    buf = (long*)GlobalLock(TBufHandle);

	ptr = buf;
	fill = (long)::GetSysColor(COLOR_3DSHADOW);
	end = (long*)(ptr +(long)12*12*MAX_COLORS);
	do { *ptr++ = fill; 
	} while( ptr<end );
	
	for( k=0;k<MAX_COLORS;k++ )
	{	fill = (long)m_crColours[k].crColour;
		fill = (long)( (GetRValue(m_crColours[k].crColour)<<16) |
		(GetGValue(m_crColours[k].crColour))<<8 ) |
		(GetBValue(m_crColours[k].crColour) );

		for( i=0;i<10;i++ )
		{	for( j=0;j<10;j++ )
			{	ptr = buf + (long)((i+1)*12*MAX_COLORS + k*12 + j+1);
				*ptr = fill;
			}
		}
	}

	//no color
	for( j=2;j<8;j++ )
	{	ptr = buf + (long)(5*12*MAX_COLORS + j + 1 );
		*ptr = 0;
	}

	//more colors
	for( j=2;j<9;j++ )
	{	ptr = buf + (long)(5*12*MAX_COLORS + 12*(MAX_COLORS-1) + j + 1 );
		*ptr = 0;
	}
	for( i=2;i<9;i++ )
	{	ptr = buf + (long)(i*12*MAX_COLORS + 12*(MAX_COLORS-1) + 5 + 1 );
		*ptr = 0;
	}
   
    PixTool = CreateDIBitmap( hDC, (BITMAPINFOHEADER*)BitToolInfo, 
		CBM_INIT, buf, BitToolInfo, DIB_RGB_COLORS);

    GlobalUnlock(TBufHandle);
    ::ReleaseDC(NULL,hDC);
	return true;
}

//Process WM_COMMAND message from parent window
void CColorBar::OnCommandHandler(UINT nID)
{	COLORREF crColour;

	if( nID==ID_COLOR )
	{	crColour = 0xFFFFFFFF;
	} else if( nID==ID_COLOR+MAX_COLORS-1 )
	{	//Perform a custom colour selection
		CColorDialog dlg(RGB(0,0,0), CC_FULLOPEN | CC_RGBINIT | CC_SHOWHELP, NULL);
		if ( dlg.DoModal()==IDOK )
        	crColour = dlg.GetColor();
        else
            return;

	} else if( nID>(ID_COLOR) && nID<(ID_COLOR+MAX_COLORS-1) )
	{	crColour = m_crColours[nID-ID_COLOR].crColour;
	} else
		return;

	GetTopLevelParent( )->SendMessage(WM_COLOR, (WPARAM)crColour); 
	GetTopLevelParent( )->PostMessage(WM_COMMAND, 
		(WPARAM)ItemList[m_ColorItem.GetCurSel( )].nID);

	return;
}



void CColorBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CToolBar::OnWindowPosChanging(lpwndpos);

    /* Closing Colour Window */
    if( lpwndpos->flags == 151   )
    {   WindowColour = FALSE;
        PrefsModified = TRUE;
    }
}

