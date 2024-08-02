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
// selectbar.h 
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELECTBAR_H_)
#define AFX_SELECTBAR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// Set and Commands
typedef struct {
    char	*set;
    char	*command;
} SetCommand;

static SetCommand ElementList[] = 
{	
	{ "elements"            ,""             },
	{ "--------------------",""             },
	{ "C"                   ,"*.c???"       }, /* elemno 12 */
	{ "H"                   ,"*.h???"       },
	{ "O"                   ,"*.o???"       },
	{ "N"                   ,"*.n???"       },
	{ "S"                   ,"*.s???"       },
	{ "P"                   ,"*.p???"       },
	{ "--------------------",""             },
	{ "HOH"                 ,"HOH"          },
	{ "DOD"                 ,"DOD"          },
	{ "PO4"                 ,"PO4"          },
	{ "SO4"                 ,"SO4"          },
	{ "--------------------",""             },
	{ "A"                   ,"A"            },
	{ "C"                   ,"C"            },
	{ "G"                   ,"G"            },
	{ "T"                   ,"T"            },
	{ "U"                   ,"U"            },
	{ "A+G - Pur"           ,"purine"       },
	{ "C+T - Pyr"           ,"pyrimidine"   },
	{ "A+T"                 ,"at"           },
	{ "C+G"                 ,"cg"           },
	{ "--------------------",""             },
	{ "A - ala"             ,"ALA"          },
	{ "N - asn"             ,"ASN"          },
	{ "D - asp"             ,"ASP"          },
	{ "R - arg"             ,"ARG"          },
	{ "C - cys"             ,"CYS"          },
	{ "Q - gln"             ,"GLN"          },
	{ "E - glu"             ,"GLU"          },
	{ "G - gly"             ,"GLY"          },
	{ "H - his"             ,"HIS"          },
	{ "I - ile"             ,"ILE"          },
	{ "L - leu"             ,"LEU"          },
	{ "K - lys"             ,"LYS"          },
	{ "M - met"             ,"MET"          },
	{ "F - phe"             ,"PHE"          },
	{ "P - pro"             ,"PRO"          },
	{ "S - ser"             ,"SER"          },
	{ "T - thr"             ,"THR"          },
	{ "W - trp"             ,"TRP"          },
	{ "Y - tyr"             ,"TYR"          },
	{ "V - val"             ,"VAL"          },
	{ ""                    ,""             }
};

static SetCommand PropertyList[] = 
{	
	{ "properties"          ,""             },
	{ "--------------------",""             },
	{ "all"                 ,"all"          },
	{ "protein"             ,"protein"      },
	{ "nucleic"             ,"nucleic"      },
	{ "dna"                 ,"dna"          },
	{ "rna"                 ,"rna"          },
	{ "ligand"              ,"ligand"       },
	{ "water"               ,"water"        },
	{ "solvent"             ,"solvent"      },
	{ "ion"                 ,"ion"          },
	{ "hydrogen"            ,"hydrogen"     },
	{ "hetero atom"         ,"hetero"       },
	{ "bonded"              ,"bonded"       },
	{ "--------------------",""             },
	{ "main chain"          ,"mainchain"    },
	{ "side chain"          ,"sidechain"    },
	{ "backbone"            ,"backbone"     },
	{ "helix"               ,"helix"        },
	{ "sheet"               ,"sheet"        },
	{ "turn"                ,"turn"         },
	{ "surface"             ,"surface"      },
	{ "buried"              ,"buried"       },
	{ "alpha"               ,"alpha"	    },
	{ "amino"               ,"amino"	    },
	{ "cis-bonded"          ,"cisbonded"    },
	{ "cystine"	            ,"cystine"      },
	{ "--------------------",""             },
	{ "charged"             ,"charged"      },
	{ "acidic"              ,"acidic"	    },
	{ "basic"               ,"basic"	    },
	{ "neutral"             ,"neutral"      },
	{ "polar"               ,"polar"	    },
	{ "hydrophobic"         ,"hydrophobic"  },
	{ "aliphatic"           ,"aliphatic"    },
	{ "aromatic"            ,"aromatic"	    },
	{ "small"               ,"small"	    },
	{ "medium"	            ,"medium"       },
	{ "large"	            ,"large"        },
	{ "acyclic"             ,"acyclic"      },
	{ "cyclic"	            ,"cyclic"       },
	{ ""                    ,""             }
};

static SetCommand UserList[] = 
{	
	{ "user sets"           ,"#set#"        },
	{ "--------------------",""             },
	{ ""                    ,""             }
};


/////////////////////////////////////////////////////////////////////////////
// CSelectBox combo

class CSelectBox : public CComboBox
{
	//{{AFX_VIRTUAL(CSelectBox)
	//}}AFX_VIRTUAL

public:
	CSelectBox();
	virtual ~CSelectBox();
	BOOL Init(CToolBar* pParentWnd, const RECT& rect, UINT nID, int Index, SetCommand *List);

protected:

	SetCommand* SetList;

	//{{AFX_MSG(CSelectBox)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};




/////////////////////////////////////////////////////////////////////////////
// CSelected box

class CSelected : public CStatic 
{
	//{{AFX_VIRTUAL(CSelected)
	//}}AFX_VIRTUAL

public:
	CSelected();
	virtual ~CSelected();
	BOOL Init(CToolBar* pParentWnd, const RECT& rect, UINT nID, int Index);
    void ChangeSelection( CString &Selection );

protected:
	//{{AFX_MSG(CSelectBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSelectBar toolbar

class CSelectBar : public CToolBar
{
// Construction
public:
	CSelectBar();
	BOOL Init(CWnd* pParentWnd);
	CSelected m_Selected;

protected:
	CSelectBox m_ElementBox;
	CSelectBox m_PropertyBox;
	CSelectBox m_UserSetBox;

// Attributes
public:

// Operations
	void SetBarPalette(BOOL bDraw);
	void AddKeyword( CString& Key );
	void ResetBox(BOOL Status);

public:

// Overrides
	//{{AFX_VIRTUAL(CSelectBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSelectBar();

protected:

	//{{AFX_MSG(CSelectBar)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SELECTBAR_H_)
