/***************************************************************************
 *                            RasMol 2.7.2.1.1                             *
 *                                                                         *
 *                                 RasMol                                  *
 *                 Molecular Graphics Visualisation Tool                   *
 *                            26 January 2004                              *
 *                                                                         *
 *                   Based on RasMol 2.6 by Roger Sayle                    *
 * Biomolecular Structures Group, Glaxo Wellcome Research & Development,   *
 *                      Stevenage, Hertfordshire, UK                       *
 *         Version 2.6, August 1995, Version 2.6.4, December 1998          *
 *                   Copyright (C) Roger Sayle 1992-1999                   *
 *                                                                         *
 *                          and Based on Mods by                           *
 *Author             Version, Date             Copyright                   *
 *Arne Mueller       RasMol 2.6x1   May 98     (C) Arne Mueller 1998       *
 *Gary Grossman and  RasMol 2.5-ucb Nov 95     (C) UC Regents/ModularCHEM  *
 *Marco Molinaro     RasMol 2.6-ucb Nov 96         Consortium 1995, 1996   *
 *                                                                         *
 *Philippe Valadon   RasTop 1.3     Aug 00     (C) Philippe Valadon 2000   *
 *                                                                         *
 *Herbert J.         RasMol 2.7.0   Mar 99     (C) Herbert J. Bernstein    * 
 *Bernstein          RasMol 2.7.1   Jun 99         1998-2001               *
 *                   RasMol 2.7.1.1 Jan 01                                 *
 *                   RasMol 2.7.2   Aug 00                                 *
 *                   RasMol 2.7.2.1 Apr 01                                 *
 *                   RasMol 2.7.2.1.1 Jan 04                               *
 *                                                                         *
 *                    and Incorporating Translations by                    *
 *  Author                               Item                      Language*
 *  Isabel Serv�n Mart�nez,                                                *
 *  Jos� Miguel Fern�ndez Fern�ndez      2.6   Manual              Spanish *
 *  Jos� Miguel Fern�ndez Fern�ndez      2.7.1 Manual              Spanish *
 *  Fernando Gabriel Ranea               2.7.1 menus and messages  Spanish *
 *  Jean-Pierre Demailly                 2.7.1 menus and messages  French  *
 *  Giuseppe Martini, Giovanni Paolella, 2.7.1 menus and messages          *
 *  A. Davassi, M. Masullo, C. Liotto    2.7.1 help file           Italian *
 *                                                                         *
 *                             This Release by                             *
 * Herbert J. Bernstein, Bernstein + Sons, P.O. Box 177, Bellport, NY, USA *
 *                       yaya@bernstein-plus-sons.com                      *
 *               Copyright(C) Herbert J. Bernstein 1998-2001               *
 *                                                                         *
 * Please read the file NOTICE for important notices which apply to this   *
 * package. If you are not going to make changes to RasMol, you are not    *
 * only permitted to freely make copies and distribute them, you are       *
 * encouraged to do so, provided you do the following:                     *
 *   * 1. Either include the complete documentation, especially the file   *
 *     NOTICE, with what you distribute or provide a clear indication      *
 *     where people can get a copy of the documentation; and               *
 *   * 2. Please give credit where credit is due citing the version and    *
 *     original authors properly; and                                      *
 *   * 3. Please do not give anyone the impression that the original       *
 *     authors are providing a warranty of any kind.                       *
 *                                                                         *
 * If you would like to use major pieces of RasMol in some other program,  *
 * make modifications to RasMol, or in some other way make what a lawyer   *
 * would call a "derived work", you are not only permitted to do so, you   *
 * are encouraged to do so. In addition to the things we discussed above,  *
 * please do the following:                                                *
 *   * 4. Please explain in your documentation how what you did differs    *
 *     from this version of RasMol; and                                    *
 *   * 5. Please make your modified source code available.                 *
 *                                                                         *
 * This version of RasMol is not in the public domain, but it is given     *
 * freely to the community in the hopes of advancing science. If you make  *
 * changes, please make them in a responsible manner, and please offer us  *
 * the opportunity to include those changes in future versions of RasMol.  *
 ***************************************************************************/

/* molecule.c
 */

#include "rasmol.h"

#ifdef IBMPC
#include <windows.h>
#include <malloc.h>
#endif
#ifdef APPLEMAC
#include <Types.h>
#endif
#ifndef sun386
#include <stdlib.h>
#endif

#include <string.h>
#if defined(IBMPC) || defined(VMS) || defined(APPLEMAC)
#include "string_case.h"
#else
#include <strings.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <math.h>

#define MOLECULE
#include "molecule.h"
#include "cmndline.h"
#include "command.h"
#include "abstree.h"
#include "transfor.h"
#include "render.h"
#include "graphics.h"
#include "vector.h"
#include "brotate.h"
#include "langsel.h"

#define HBondPool   32
#define BondPool    32
#define AtomPool    32

#define NoLadder     0x00
#define ParaLadder   0x01
#define AntiLadder   0x02

#define Cos70Deg     0.34202014332567

#define MaxHBondDist   ((Long)300*300)
#define MaxBondDist    ((Long)475*475)
#define MinBondDist    ((Long)100*100)
#define AbsMaxBondDist 600

#if defined(APPLEMAC) || defined(RASTOPWIN)
#define AllocSize   256
typedef struct _AllocRef {
        struct _AllocRef *next;
        void *data[AllocSize];
        int count;
        } AllocRef;
static AllocRef *AllocList;
static int AllocAdd = 0;
static int AllocRem = 0;
#endif


static Molecule __far *FreeMolecule;
static Chain __far *FreeChain;
static Group __far *FreeGroup;
static RAtom __far *FreeAtom;


static Group __far *Cache;
static RAtom __far *ACache;
static IntCoord __far *IntPrev;
static HBond __far * __far *CurHBond;
static size_t MemSize;

/* Macros for commonly used loops */
#define ForEachAtom  for(chain=Database->clist;chain;chain=chain->cnext) \
                     for(group=chain->glist;group;group=group->gnext)    \
                     for(aptr=group->alist;aptr;aptr=aptr->anext)
#define ForEachBond  for(bptr=Database->blist;bptr;bptr=bptr->bnext)


/*=======================*/
/*  Function Prototypes  */
/*=======================*/

RAtom __far *FindCysSulphur(  Group __far* );
static IntCoord __far* GetInternalCoord( int );
Bond __far *ProcessBond( RAtom __far*, RAtom __far*, int );
static void CreateHydrogenBond( RAtom __far*, RAtom __far*,
                                RAtom __far*, RAtom __far*, int, int );
static int CalculateBondEnergy( Group __far* );
static void CalcProteinHBonds( Chain __far* );
static void CalcNucleicHBonds( Chain __far* );
static int IsHBonded( RAtom __far*, RAtom __far*, HBond __far* );
static void TestLadder( Chain __far* );


#ifdef APPLEMAC
/* External RasMac Function Declaration! */
void SetFileInfo( char*, OSType, OSType, short );
#endif


void DescribeMolecule( void )
{
    char buffer[80];

    InvalidateCmndLine();

    if( *Info.moleculename )
    {   WriteString(MsgStrs(MolMolNam));
        WriteString(Info.moleculename);
        WriteChar('\n');
    }

    if( *Info.classification )
    {   WriteString(MsgStrs(MolClass));
        WriteString(Info.classification);
        WriteChar('\n');
    }

    if( Database && (MainGroupCount>1) )
    {   WriteString(MsgStrs(MolSecStr));
        if( Info.structsource == SourceNone )
        {   WriteString(MsgStrs(MolNoAss));
        } else if( Info.structsource == SourcePDB )
        {   WriteString(MsgStrs(MolPDB));
        } else WriteString(MsgStrs(MolCalcul));
    }


    if( *Info.identcode )
    {   WriteString(MsgStrs(MolDatCod));
        WriteString(Info.identcode);
        WriteChar('\n');
    }

    if( *Info.technique )
    {   WriteString(MsgStrs(MolExpTech));
        WriteString(Info.technique);
        WriteChar('\n');
    }

    if( Info.chaincount > 1 )
    {   WriteString(MsgStrs(MolChainNb));
        sprintf(buffer,"%d\n",Info.chaincount);
        WriteString(buffer);
    }
   
    WriteString(MsgStrs(MolGrpNb));
    sprintf(buffer,"%d",MainGroupCount);
    WriteString(buffer);
    if( HetaAtomCount )
    {   sprintf(buffer," (%d)\n",HetaGroupCount);
        WriteString(buffer);
    } else WriteChar('\n');

    WriteString(MsgStrs(MolAtomNb));
    sprintf(buffer,"%ld",(long)MainAtomCount);
    WriteString(buffer);
    if( HetaAtomCount )
    {   sprintf(buffer," (%d)\n",HetaAtomCount);
        WriteString(buffer);
    } else WriteChar('\n');

    if( Info.bondcount )
    {   WriteString(MsgStrs(MolBondNb));
        sprintf(buffer,"%ld\n",(long)Info.bondcount);
        WriteString(buffer);
    }

    if( Info.ssbondcount != -1 )
    {   WriteString(MsgStrs(MolNumBr));
        sprintf(buffer,"%d\n",Info.ssbondcount);
        WriteString(buffer);
    }

    if( Info.hbondcount != -1 )
    {   WriteString(MsgStrs(MolNumHB));
        sprintf(buffer,"%d\n",Info.hbondcount);
        WriteString(buffer);
    }

    if( Info.helixcount != -1 )
    {   WriteString(MsgStrs(MolNumHel));
        sprintf(buffer,"%d\n",Info.helixcount);
        WriteString(buffer);

        WriteString(MsgStrs(MolNumStr));
        sprintf(buffer,"%d\n",Info.laddercount);
        WriteString(buffer);

        WriteString(MsgStrs(MolNumTurn));
        sprintf(buffer,"%d\n",Info.turncount);
        WriteString(buffer);
    }
}



/*==================================*/
/* Group & Chain Handling Functions */
/*==================================*/

void CreateChain( int ident )
{
    register Chain __far *prev;

    if( !CurMolecule )
    {   if( !(CurMolecule = FreeMolecule) )
        {   MemSize += sizeof(Molecule);
            CurMolecule = (Molecule __far *)_fmalloc(sizeof(Molecule));
            if( !CurMolecule ) RasMolFatalExit(MsgStrs(RfeDatabase));
            RegisterAlloc( CurMolecule );
        } else FreeMolecule = (void __far*)0;

        CurChain = (void __far*)0;
        CurMolecule->slist = (void __far*)0;
        CurMolecule->hlist = (void __far*)0;
        CurMolecule->blist = (void __far*)0;
        CurMolecule->clist = (void __far*)0;
        Database = CurMolecule;
    }

    /* Handle chain breaks! */
    prev = CurChain;
    if( !prev )
    {   prev = CurMolecule->clist;
        if( prev )
            while( prev->cnext )
                prev = prev->cnext;
    }

    if( !(CurChain = FreeChain) )
    {   MemSize += sizeof(Chain);
        CurChain = (Chain __far *)_fmalloc(sizeof(Chain));
        if( !CurChain ) RasMolFatalExit(MsgStrs(RfeDatabase));
        RegisterAlloc( CurChain );
    } else FreeChain = FreeChain->cnext;

    if( prev )
    {   prev->cnext = CurChain;
    } else CurMolecule->clist = CurChain;
    CurChain->cnext = (void __far*)0;
     
    CurChain->ident = ident;
    CurChain->model = NMRModel;
    CurChain->glist = (void __far*)0;
    CurChain->blist = (void __far*)0;
    CurGroup = (void __far*)0;
    Info.chaincount++;

    Cache = (Group __far*)0;
}


void CreateGroup( int pool )
{
    register Group __far *ptr;
    register int i;

    if( !(ptr = FreeGroup) )
    {   MemSize += pool*sizeof(Group);
        ptr = (Group __far *)_fmalloc( pool*sizeof(Group) );
        if( !ptr ) RasMolFatalExit(MsgStrs(RfeDatabase));
        RegisterAlloc( ptr );
        for( i=1; i<pool; i++ )
        {   ptr->gnext = FreeGroup;
            FreeGroup = ptr++;
        } 
    } else FreeGroup = ptr->gnext;
    
    if( CurGroup )
    {   ptr->gnext = CurGroup->gnext;
        CurGroup->gnext = ptr;
    } else 
    {   ptr->gnext = CurChain->glist;
        CurChain->glist = ptr;
    }
    CurGroup = ptr;

    CurAtom = (void __far*)0;
    ptr->alist = (void __far*)0;
    ptr->serno = -9999;
    ptr->sserno = -9999;
    ptr->insert = ' ';
    ptr->sinsert = ' ';
    ptr->struc = 0;
    ptr->flag = 0;
    ptr->col1 = 0;
    ptr->col2 = 0;
    ptr->model = NMRModel;
}


int FindResNo( char *ptr )
{
    register int ch1,ch2,ch3;
    register int refno;

    ch1 = ToUpper(ptr[0]);
    ch2 = ToUpper(ptr[1]);
    ch3 = ToUpper(ptr[2]);

    switch( ch1 )
    {   case(' '): if( ch2 == ' ' )
                   {   switch( ch3 )
                       {   case('A'):  return( 24 );
                           case('C'):  return( 25 );
                           case('G'):  return( 26 );
                           case('T'):  return( 27 );
                           case('U'):  return( 28 );
                           case('I'):  return( 30 );
                       }
                   } else if( ch2 == '+' )
                   {   if( ch3 == 'U' )
                           return( 29 );
                   } else if( ch2 == 'Y' )
                   {   if( ch3 == 'G' )
                           return( 39 );
                   }
                   break;

        case('0'): if( ch2 == 'M' )
                   {   if( ch3 == 'C' )
                       {   return( 33 );
                       } else if( ch3 == 'G' )
                           return( 38 );
                   }
                   break;

        case('1'): if( ch2 == 'M' )
                   {   if( ch3 == 'A' )
                       {   return( 31 );
                       } else if( ch3 == 'G' )
                           return( 34 );
                   }
                   break;

        case('2'): if( ch2 == 'M' )
                   {   if( ch3 == 'G' )
                           return( 35 );
                   }
                   break;

        case('5'): if( ch2 == 'M' )
                   {   if( ch3 == 'C' )
                       {   return( 32 );
                       } else if( ch3 == 'U' )
                           return( 41 );
                   }
                   break;

        case('7'): if( ch2 == 'M' )
                   {   if( ch3 == 'G' )
                           return( 37 );
                   }
                   break;

        case('A'): if( ch2 == 'L' )
                   {   if( ch3 == 'A' )
                           return(  0 );
                   } else if( ch2 == 'S' )
                   {   if( ch3 == 'P' )
                       {   return(  7 );
                       } else if( ch3 == 'N' )
                       {   return(  9 );
                       } else if( ch3 == 'X' )
                           return( 20 );
                   } else if( ch2 == 'R' )
                   {   if( ch3 == 'G' )
                           return( 12 );
                   } else if( ch2 == 'C' )
                   {   if( ch3 == 'E' )
                           return( 44 );
                   } else if( ch2 == 'D' )
                   {   if( ch3 == 'E' )
                           return( 24 );    /* "ADE" -> "  A" */
                   }
                   break;

        case('C'): if( ch2 == 'Y' )
                   {   if( ch3 == 'S' )
                       {   return( 17 );
                       } else if( ch3 == 'H' )
                       {   return( 17 );    /* "CYH" -> "CYS" */
                       } else if( ch3 == 'T' )
                           return( 25 );    /* "CYT" -> "  C" */
                   } else if( ch2 == 'O' )
                   {   if( ch3 == 'A' )
                           return( 51 );
                   } else if( ch2 == 'P' )
                   {   if( ch3 == 'R' )
                           return( 11 );    /* "CPR" -> "PRO" */
                   } else if( ch2 == 'S' )
                   {   if( ch3 == 'H' )
                       {   return( 17 );    /* "CSH" -> "CYS" */
                       } else if( ch3 == 'M' )
                           return( 17 );    /* "CSM" -> "CYS" */
                   }
                   break;

        case('D'): if( ch2 == 'O' )
                   {   if( ch3 == 'D' )
                           return( 47 );
                   } else if( ch2 == '2' )
                   {   if( ch3 == 'O' )
                           return( 47 );    /* "D2O" -> "DOD" */
                   }
                   break;

        case('F'): if( ch2 == 'O' )
                   {   if( ch3 == 'R' )
                           return( 45 );
                   }
                   break;

        case('G'): if( ch2 == 'L' )
                   {   if( ch3 == 'Y' )
                       {   return(  1 );
                       } else if( ch3 == 'U' )
                       {   return( 10 );
                       } else if( ch3 == 'N' )
                       {   return( 14 );
                       } else if( ch3 == 'X' )
                           return( 21 );
                   } else if( ch2 == 'U' )
                   {   if( ch3 == 'A' )
                           return( 26 );    /* "GUA" -> "  G" */
                   }
                   break;

        case('H'): if( ch2 == 'I' )
                   {   if( ch3 == 'S' )
                           return( 16 );
                   } else if( ch2 == 'O' )
                   {   if( ch3 == 'H' )
                           return( 46 );
                   } else if( ch2 == 'Y' )
                   {   if( ch3 == 'P' )
                           return( 23 );
                   } else if( ch2 == '2' )
                   {   if( ch3 == 'O' )
                       {   return( 46 );    /* "H20" -> "HOH" */
                       } else if( ch3 == 'U' )
                           return( 40 );
                   }
                   break;

        case('I'): if( ch2 == 'L' )
                   {   if( ch3 == 'E' )
                           return(  8 );
                   }
                   break;

        case('L'): if( ch2 == 'E' )
                   {   if( ch3 == 'U' )
                           return(  2 );
                   } else if( ch2 == 'Y' )
                   {   if( ch3 == 'S' )
                           return(  6 );
                   }
                   break;

        case('M'): if( ch2 == 'E' )
                   {   if( ch3 == 'T' )
                           return( 18 );
                   } else if( ch2 == '2' )
                   {   if( ch3 == 'G' )
                           return( 36 );
                   }
                   break;

        case('N'): if( ch2 == 'A' )
                   {   if( ch3 == 'D' )
                       {   return( 50 );
                       } else if( ch3 == 'P' )
                           return( 52 );
                   } else if( ch2 == 'D' )
                   {   if( ch3 == 'P' )
                           return( 53 );
                   }
                   break;

        case('P'): if( ch2 == 'R' )
                   {   if( ch3 == 'O' )
                           return( 11 );
                   } else if( ch2 == 'H' )
                   {   if( ch3 == 'E' )
                           return( 13 );
                   } else if( ch2 == 'C' )
                   {   if( ch3 == 'A' )
                           return( 22 );
                   } else if( ch2 == 'O' )
                   {   if( ch3 == '4' )
                           return( 49 );
                   } else if( ch2 == 'S' )
                   {   if( ch3 == 'U' )
                           return( 42 );
                   }
                   break;

        case('S'): if( ch2 == 'E' )
                   {   if( ch3 == 'R' )
                           return(  3 );
                   } else if( ch2 == 'O' )
                   {   if( ch3 == '4' )
                       {   return( 48 );
                       } else if( ch3 == 'L' )
                           return( 46 );    /* "SOL" -> "HOH" */
                   } else if( ch2 == 'U' )
                   {   if( ch3 == 'L' )
                           return( 48 );    /* "SUL" -> "SO4" */
                   }
                   break;

        case('T'): if( ch2 == 'H' )
                   {   if( ch3 == 'R' )
                       {   return(  5 );
                       } else if( ch3 == 'Y' )
                           return( 27 );    /* "THY" -> "  T" */
                   } else if( ch2 == 'Y' )
                   {   if( ch3 == 'R' )
                           return( 15 );
                   } else if( ch2 == 'R' )
                   {   if( ch3 == 'P' )
                       {   return( 19 );
                       } else if( ch3 == 'Y' )
             return( 19 );    /* "TRY" -> "TRP" */
                   } else if( ch2 == 'I' )
                   {   if( ch3 == 'P' )
                           return( 46 );    /* "TIP" -> "HOH" */
                   }
                   break;

        case('U'): if( ch2 == 'N' )
                   {   if( ch3 == 'K' )
                           return( 43 );
                   } else if( ch2 == 'R' )
                   {   if( ch3 == 'A' )
                       {   return( 28 );    /* "URA" -> "  U" */
                       } else if( ch3 == 'I' )
                           return( 28 );    /* "URI" -> "  U" */
                   }
                   break;

        case('V'): if( ch2 == 'A' )
                   {   if( ch3 == 'L' )
                           return(  4 );
                   }
                   break;

        case('W'): if( ch2 == 'A' )
                   {   if( ch3 == 'T' )
                           return( 46 );    /* "WAT" -> "HOH" */
                   }
                   break;
    }

    for( refno=MINRES; refno<ResNo; refno++ )
        if( !strncasecmp(Residue[refno],ptr,3) )
            return refno;

    if( ResNo++ == MAXRES )
        RasMolFatalExit(MsgStrs(RfeNewRes));
    Residue[refno][0] = ch1;
    Residue[refno][1] = ch2;
    Residue[refno][2] = ch3;
    return refno;
}


void ProcessGroup( int heta )
{
    register int serno;

    serno = CurGroup->serno;
    if( IsSolvent(CurGroup->refno) )
        heta = True;

    if( heta )
    {   HetaGroupCount++;
        if( HMinMaxFlag )
        {   if( serno > MaxHetaRes )
            {   MaxHetaRes = serno;
            } else if( serno < MinHetaRes )
                MinHetaRes = serno;
        } else MinHetaRes = MaxHetaRes = serno;
    } else
    {   MainGroupCount++;
        if( MMinMaxFlag )
        {   if( serno > MaxMainRes )
            {   MaxMainRes = serno;
            } else if( serno < MinMainRes )
                MinMainRes = serno;
        } else MinMainRes = MaxMainRes = serno;
    }
    if (CurGroup->model && !(MaxModel)){
      MaxModel = MinModel = CurGroup->model;
    }
    if (CurGroup->model > MaxModel) MaxModel = CurGroup->model;
    if (CurGroup->model < MinModel) MinModel = CurGroup->model;
}


void CreateMolGroup( void )
{
    CreateChain( ' ' );
    CreateGroup( 1 );

    CurGroup->refno = FindResNo( "MOL" );
    CurGroup->serno = 1;
        
    MinMainRes = MaxMainRes = 1;
    MinHetaRes = MaxHetaRes = 0;
    MainGroupCount = 1;
}


void CreateNextMolGroup( void )
{
    if( CurGroup->alist )
    {   CreateChain(' ');
        CreateGroup(1);
        MainGroupCount++;
        CurGroup->refno = FindResNo( "MOL" );
        CurGroup->serno = MainGroupCount;
        MaxMainRes++;
    }
}



/*=========================*/
/* Atom Handling Functions */
/*=========================*/

RAtom __far *CreateAtom( void )
{
    register RAtom __far *ptr;
    register int i;

    if( !(ptr = FreeAtom) )
    {   MemSize += AtomPool*sizeof(RAtom);
        ptr = (RAtom __far *)_fmalloc( AtomPool*sizeof(RAtom) );
        if( !ptr ) RasMolFatalExit(MsgStrs(RfeDatabase));
        RegisterAlloc( ptr );
        for( i=1; i<AtomPool; i++ )
        {   ptr->anext = FreeAtom;
            FreeAtom = ptr++;
        } 
    } else FreeAtom = ptr->anext;

    if( CurAtom )
    {   ptr->anext = CurAtom->anext;
        CurAtom->anext = ptr;
    } else 
    {   ptr->anext = CurGroup->alist;
        CurGroup->alist = ptr;
    }
    CurAtom = ptr;

    SelectCount++;
    ptr->flag = SelectFlag | NonBondFlag;
    ptr->flag2 = 0;
    ptr->label = (void*)0;
    ptr->radius = 375;
    ptr->altl = ' ';
    ptr->mbox = 0;
    ptr->col = 0;
    ptr->model = 0;
    ptr->surf = 0;
    ptr->xtrl = 0;
    ptr->ytrl = 0;
    ptr->ztrl = 0;
    ptr->fxorg = 0;
    ptr->fyorg = 0;
    ptr->fzorg = 0;
    return ptr;
}


void ProcessAtom( RAtom __far *ptr )
{
    int altc;

    ptr->elemno = GetElemNumber(CurGroup,ptr);
    if( ptr->elemno == 1 )
    {   ptr->flag |= HydrogenFlag;
        HasHydrogen = True;
    }

    if( !IsSolvent(CurGroup->refno) )
    {   if( !(ptr->flag&(HydrogenFlag|HeteroFlag)) )
            ptr->flag |= NormAtomFlag;
    } else ptr->flag |= HeteroFlag;

#ifdef INVERT
    ptr->yorg = -ptr->yorg;
    ptr->ytrl = -ptr->ytrl;
#endif

    ptr->fxorg = 0;
    ptr->fyorg = 0;
    ptr->fzorg = 0;

    if( HMinMaxFlag || MMinMaxFlag )
    {   if( ptr->xorg < MinX ) 
        {   MinX = ptr->xorg;
        } else if( ptr->xorg > MaxX ) 
            MaxX = ptr->xorg;

        if( ptr->yorg < MinY ) 
        {   MinY = ptr->yorg;
        } else if( ptr->yorg > MaxY ) 
            MaxY = ptr->yorg;

        if( ptr->zorg < MinZ ) 
        {   MinZ = ptr->zorg;
        } else if( ptr->zorg > MaxZ ) 
            MaxZ = ptr->zorg;
    } else 
    {   MinX = MaxX = ptr->xorg;
        MinY = MaxY = ptr->yorg;
        MinZ = MaxZ = ptr->zorg;
    }
            
    if( ptr->flag & HeteroFlag )
    {   if( HMinMaxFlag )
        {   if( ptr->temp < MinHetaTemp ) 
            {   MinHetaTemp = ptr->temp;
            } else if( ptr->temp > MaxHetaTemp ) 
                MaxHetaTemp = ptr->temp;
        } else MinHetaTemp = MaxHetaTemp = ptr->temp;
        HMinMaxFlag = True;
        HetaAtomCount++;
    } else
    {   if( MMinMaxFlag )
        {   if( ptr->temp < MinMainTemp ) 
            {   MinMainTemp = ptr->temp;
            } else if( ptr->temp > MaxMainTemp ) 
                MaxMainTemp = ptr->temp;
        } else MinMainTemp = MaxMainTemp = ptr->temp;
        MMinMaxFlag = True;
        MainAtomCount++;
    }

    if ( ptr->altl != '\0' && ptr->altl != ' ')
    {
        altc = (((int)ptr->altl)&(AltlDepth-1))+1;
        if (MaxAltl < altc) MaxAltl = altc;
    }
}


RAtom __far *FindGroupAtom( Group __far *group, int n )
{
    register RAtom __far *ptr;

    for( ptr=group->alist; ptr; ptr=ptr->anext )
        if( ptr->refno == n ) return( ptr );
    return( (RAtom __far*)0 );
}


int NewAtomType( char *ptr )
{
    register int refno;
    register int i;

    for( refno=0; refno<ElemNo; refno++ )
        if( !strncasecmp(ElemDesc[refno],ptr,12) )
            return refno;

    if( ElemNo++ == MAXELEM )
        RasMolFatalExit(MsgStrs(RfeNewAtoms));

    for( i=0; i<12; i++ )
        ElemDesc[refno][i] = '\0';
    for( i=0; ptr[i]&&i<12; i++)
        ElemDesc[refno][i] = ptr[i];
    return refno;
}


int SimpleAtomType( char *type )
{
    char name[5];

    name[4] = '\0';

    name[2] = name[3] = ' ';
    if( type[1] && (type[1]!=' ') )
    {   name[0] = ToUpper(type[0]);
        name[1] = ToUpper(type[1]);
    } else
    {   name[1] = ToUpper(type[0]);
        name[0] = ' ';
    }
    return NewAtomType(name);
}


int ComplexAtomType( char *ptr )
{
    static char name[5];
    register int i;

    name[4] = '\0';

    if( isdigit(ptr[1]) )
    {   /* IDATM PDB files! */
        name[0] = ' ';
        name[1] = ToUpper(ptr[0]);
        name[2] = ToUpper(ptr[1]);
        name[3] = ToUpper(ptr[2]);
    } else if( ptr[1] == ' ' )
    {   /* Corina PDB files! */
        name[0] = ' ';
        name[1] = ToUpper(ptr[0]);
        name[2] = ToUpper(ptr[2]);
        name[3] = ToUpper(ptr[3]);
    } else for( i=0; i<4; i++ )
        name[i] = ToUpper(ptr[i]);

    /* Handle Unconventional Naming */
    if( IsProtein(CurGroup->refno) )
    {   if( name[0]=='H' )
        {   name[0]=' ';
            name[1]='H';
        }
    } else if( IsNucleo(CurGroup->refno) )
    {   if( name[3]=='\'' )
            name[3] = '*';

        if( (name[1]=='O') && (name[2]=='P') )
        {   if( !strncmp(name," OP1",4) ||
                !strncmp(name,"1OP ",4) )
                return( 8 );
            if( !strncmp(name," OP2",4) ||
                !strncmp(name,"2OP ",4) )
                return( 9 );
        }
    }
    return NewAtomType(name);
}



/*===============================*/
/* Z-Matrix Conversion Functions */
/*===============================*/

void InitInternalCoords( void )
{
    IntList = (IntCoord __far*)0;
    IntPrev = (IntCoord __far*)0;
}


IntCoord __far* AllocInternalCoord( void )
{
    register IntCoord __far *ptr;

    ptr = (IntCoord __far*)_fmalloc(sizeof(IntCoord));
    if( !ptr ) RasMolFatalExit(MsgStrs(RfeDatabase));
    ptr->inext = (IntCoord __far*)0;

    if( IntPrev )
    {   IntPrev->inext = ptr;
    } else IntList = ptr;
    IntPrev = ptr;
    return( ptr );
}


static IntCoord __far* GetInternalCoord( int index )
{
    register IntCoord __far *ptr;

    ptr = IntList;
    while( (index>1) && ptr->inext )
    {   ptr = ptr->inext;
        index--;
    }
    return ptr;
}


void FreeInternalCoords( void )
{
    register IntCoord __far *ptr;

    while( IntList )
    {    ptr = IntList;
         IntList = ptr->inext;
         _ffree( ptr );
    }
}


int ConvertInternal2Cartesian( void )
{
    register IntCoord __far *ptr;
    register IntCoord __far *na;
    register IntCoord __far *nb;
    register IntCoord __far *nc;
    register double cosph,sinph,costh,sinth,coskh,sinkh;
    register double cosa,sina,xcosd,xsind;
    register double dist,angle,dihed;

    register double xpd,ypd,zpd,xqd,yqd,zqd;
    register double xa,ya,za,xb,yb,zb;
    register double rbc,xyb,yza,temp;
    register double xpa,ypa,zqa;
    register double xd,yd,zd;
    register int flag;

    /* Atom #1 */
    ptr = IntList;
    ptr->dist  = 0.0;
    ptr->angle = 0.0;
    ptr->dihed = 0.0;

    ptr = ptr->inext;
    if( !ptr ) return( True );

    /* Atom #2 */
    ptr->angle = 0.0;
    ptr->dihed = 0.0;

    ptr = ptr->inext;
    if( !ptr ) return( True );

    /* Atom #3 */
    dist = ptr->dist;
    angle = Deg2Rad*ptr->angle;
    cosa = cos(angle);
    sina = sin(angle);
    if( ptr->na == 1 )
    {   na = IntList;
        ptr->dist = na->dist + cosa*dist;
    } else /* ptr->na == 2 */
    {   na = IntList->inext;
        ptr->dist = na->dist - cosa*dist;
    }
    ptr->angle = sina*dist;
    ptr->dihed = 0.0;

    while( ptr->inext )
    {   ptr = ptr->inext;
        dist = ptr->dist;
        angle = Deg2Rad*ptr->angle;
        dihed = Deg2Rad*ptr->dihed;

        /* Optimise this access?? */
        na = GetInternalCoord(ptr->na);
        nb = GetInternalCoord(ptr->nb);
        nc = GetInternalCoord(ptr->nc);

        xb = nb->dist  - na->dist;
        yb = nb->angle - na->angle;
        zb = nb->dihed - na->dihed;

        rbc = xb*xb + yb*yb + zb*zb;
        if( rbc < 0.0001 )
            return( False );
        rbc= 1.0/sqrt(rbc);

        cosa = cos(angle);
        sina = sin(angle);


        if( fabs(cosa) >= 0.999999 )
        {   /* Colinear */
            temp = dist*rbc*cosa;
            ptr->dist  = na->dist  + temp*xb;
            ptr->angle = na->angle + temp*yb;
            ptr->dihed = na->dihed + temp*zb;
        } else
        {   xa = nc->dist  - na->dist;
            ya = nc->angle - na->angle;
            za = nc->dihed - na->dihed;

            xsind = -sin(dihed);
            xcosd = cos(dihed);

            xd = dist*cosa;
            yd = dist*sina*xcosd;
            zd = dist*sina*xsind;

            xyb = sqrt(xb*xb + yb*yb);
            if( xyb < 0.1 )
            {   /* Rotate about y-axis! */
                temp = za; za = -xa; xa = temp;
                temp = zb; zb = -xb; xb = temp;
                xyb = sqrt(xb*xb + yb*yb);
                flag = True;
            } else flag = False;

            costh = xb/xyb;
            sinth = yb/xyb;
            xpa = costh*xa + sinth*ya;
            ypa = costh*ya - sinth*xa;

            sinph = zb*rbc;
            cosph = sqrt(1.0 - sinph*sinph);
            zqa = cosph*za  - sinph*xpa;

            yza = sqrt(ypa*ypa + zqa*zqa);

            if( yza > 1.0E-10 )
            {   coskh = ypa/yza;
                sinkh = zqa/yza;

                ypd = coskh*yd - sinkh*zd;
                zpd = coskh*zd + sinkh*yd;
            } else
            {   /* coskh = 1.0; */
                /* sinkh = 0.0; */
                ypd = yd;
                zpd = zd;
            }

            xpd = cosph*xd  - sinph*zpd;
            zqd = cosph*zpd + sinph*xd;
            xqd = costh*xpd - sinth*ypd;
            yqd = costh*ypd + sinth*xpd;

            if( flag )
            {   /* Rotate about y-axis! */
                ptr->dist  = na->dist  - zqd;
                ptr->angle = na->angle + yqd;
                ptr->dihed = na->dihed + xqd;
            } else
            {   ptr->dist  = na->dist  + xqd;
                ptr->angle = na->angle + yqd;
                ptr->dihed = na->dihed + zqd;
            }
        }
    }
    return True;
}



/*=========================*/
/* Bond Handling Functions */
/*=========================*/

Bond __far *ProcessBond( RAtom __far *src, RAtom __far *dst, int flag )
{
    register Bond __far *ptr;
    register int i;

    if( flag & (DoubBondFlag|TripBondFlag) )
        DrawDoubleBonds = True;

    if( !(ptr = FreeBond) )
    {   MemSize += BondPool*sizeof(Bond);
        ptr = (Bond __far *)_fmalloc( BondPool*sizeof(Bond) );
        if( !ptr ) RasMolFatalExit(MsgStrs(RfeDatabase));
        RegisterAlloc( ptr );
        for( i=1; i<BondPool; i++ )
        {   ptr->bnext = FreeBond;
            FreeBond = ptr++;
        } 
    } else FreeBond = ptr->bnext;

    ptr->flag = flag | SelectFlag;
    ptr->srcatom = src;
    ptr->dstatom = dst;
    ptr->radius = 0;
    ptr->irad = 0;
    ptr->aradius = 0;
    ptr->iarad = 0;
    ptr->col = 0;
    ptr->altl = '\0';
    ptr->cap = False;
    if (src && src->altl != '\0' && src->altl != ' ') ptr->altl = src->altl;
    if (dst && dst->altl != '\0' && dst->altl != ' ') ptr->altl = dst->altl;

    return ptr;
}

static void CreateHydrogenBond( RAtom __far *srcCA, RAtom __far *dstCA,
                                RAtom __far *src, RAtom __far *dst,
                                int energy, int offset )
{
    register HBond __far *ptr;
    register int i,flag;

    if( !(ptr = FreeHBond) )
    {   MemSize += HBondPool*sizeof(HBond);
        ptr = (HBond __far *)_fmalloc( HBondPool*sizeof(HBond) );
        if( !ptr ) RasMolFatalExit(MsgStrs(RfeDatabase));
        RegisterAlloc( ptr );
        for( i=1; i<HBondPool; i++ )
        {   ptr->hnext = FreeHBond;
            FreeHBond = ptr++;
        } 
    } else FreeHBond = ptr->hnext;

    if( (offset>=-128) && (offset<127) )
    {   ptr->offset = (Char)offset;
    } else ptr->offset = 0;

    flag = ZoneBoth? src->flag&dst->flag : src->flag|dst->flag;
    ptr->flag = flag & SelectFlag;

    ptr->src = src;
    ptr->dst = dst;
    ptr->srcCA = srcCA;
    ptr->dstCA = dstCA;
    ptr->energy = energy;
    ptr->col = 0;
    ptr->altl = '\0';
    if (src && src->altl != '\0' && src->altl != ' ') ptr->altl = src->altl;
    if (dst && dst->altl != '\0' && dst->altl != ' ') ptr->altl = dst->altl;

    *CurHBond = ptr;
    ptr->hnext = (void __far*)0;
    CurHBond = &ptr->hnext;
    Info.hbondcount++;
}


static RAtom __far *LocateAtom( Long serno, int flag )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;

    if( Cache )
    {   for( aptr=ACache; aptr; aptr=aptr->anext )
            if( aptr->serno == serno )
            {   ACache = aptr;
                return( aptr );
            }

        for( aptr=Cache->alist; aptr != ACache; aptr=aptr->anext )
            if( aptr->serno == serno )
            {   ACache = aptr;
                return( aptr );
            }

        if( Cache->gnext )
            for( aptr=Cache->gnext->alist; aptr; aptr=aptr->anext )
                if( aptr->serno == serno )
                {   if( flag )
                    {   Cache = Cache->gnext;
                        ACache = aptr;
                    }
                    return( aptr );
                }
    }

    if( CurChain )
    {   for( group=CurChain->glist; group; group=group->gnext )
            for( aptr=group->alist; aptr; aptr=aptr->anext )
                if( aptr->serno == serno )
                {   Cache = group;
                    ACache = aptr;
                    return( aptr );
                }
    }

    for( chain=Database->clist; chain; chain=chain->cnext )
        if( chain != CurChain )
            for( group=chain->glist; group; group=group->gnext )
                for( aptr=group->alist; aptr; aptr=aptr->anext )
                    if( aptr->serno == serno )
                    {   Cache = group;
                        ACache = aptr;
                        return( aptr );
                    }

    return (RAtom __far*)0;
}


int CreateBond( Long src, Long dst, int flag )
{
    register RAtom __far *sptr;
    register RAtom __far *dptr;
    register Bond __far *bptr;

    if( src == dst )
        return False;

    sptr = LocateAtom(src,False);  if( !sptr ) return False;
    dptr = LocateAtom(dst,False);  if( !dptr ) return False;

    if( flag != HydrBondFlag )
    {   /* Reset Non-bonded flags! */
        sptr->flag &= ~NonBondFlag;
        dptr->flag &= ~NonBondFlag;

        bptr = ProcessBond( sptr, dptr, flag );
        bptr->bnext = CurMolecule->blist;
        CurMolecule->blist = bptr;
        Info.bondcount++;
        CapClean = False;

    } else /* Hydrogen Bond! */
    {   if( Info.hbondcount < 0 ) 
        {   CurHBond = &CurMolecule->hlist;
            Info.hbondcount = 0;
        }
        CreateHydrogenBond( NULL, NULL, sptr, dptr, 0, 0 );
    }
    return True;
}


static int RemoveBond(  Long nsrc, Long ndst )
{  
    int found = False;
    register RAtom __far *src;
    register RAtom __far *dst;
    register Bond __far *bptr;
    register Bond __far *pbptr;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register int done;
    char buffer[80];
    char value[20];
 
    if( nsrc == ndst )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(MolSerNb));
        return False;
    }

    if( !Database )
        return False;
 
    done = False;
    src = (RAtom __far*)NULL;
    dst = (RAtom __far*)NULL;
 
    for( chain=Database->clist; chain && !done; chain=chain->cnext )
        for( group=chain->glist; group && !done; group=group->gnext )
            for( aptr=group->alist; aptr; aptr=aptr->anext )
            {   if( aptr->serno == nsrc )
                {   src = aptr;
                    if( dst )
                    {   done = True;
                        break;
                    }
                } else if( aptr->serno == ndst )
                {   dst = aptr;
                    if( src )
                    {   done = True;
                        break;
                    }
                }
            }


    if( !done )
    {   InvalidateCmndLine();
        if( src )
        {   sprintf(value," %d",ndst);
        } else if( dst )
        {   sprintf(value," %d",nsrc);
        } else sprintf(value,"s %d and %d",nsrc,ndst);
        FormatMsg(buffer,MolErrAtSer,value);
        WriteString(buffer);
    } else 
    {
    pbptr = NULL;

    ForEachBond
    {   if( ( bptr->srcatom == src && bptr->dstatom == dst ) ||
            ( bptr->dstatom == src && bptr->srcatom == dst ) )
        {   found = True;
            if( pbptr ) 
            {   pbptr->bnext = bptr->bnext;
            } else
            {   Database->blist = bptr->bnext;
            }
            Info.bondcount--;
            bptr->bnext = FreeBond;
            FreeBond = bptr->bnext;
        } else 
            pbptr = bptr;
    }
  
    /* Remove the bond from the list of selected bonds */
    if( RotBondList )
    {   BondRot __far *brptr=RotBondList;
        BondRot __far *pbrptr=NULL;   

        while (brptr)
        {   if( (src == brptr->BSrcAtom && dst == brptr->BDstAtom) ||
                (src == brptr->BDstAtom && dst == brptr->BSrcAtom) )
            {   if (pbrptr)
                {   pbrptr->brnext = brptr->brnext;
                    if( BondSelected == brptr )
                    {   BondSelected = brptr->brnext;
                        if(!BondSelected)
                        {   BondSelected = pbrptr;
                            WriteString(MsgStrs(MolPrevRot));
                        } else
                        {   WriteString(MsgStrs(MolNextRot));
                        }
                    }
                    _ffree(brptr);
                    brptr = pbrptr->brnext;
                } else
                {   if( BondSelected == brptr )
                    {   BondSelected = brptr->brnext;
                        if(!BondSelected)
                        {   WriteString(MsgStrs(MolNoRotBnd));
                        } else
                        {   WriteString(MsgStrs(MolNextRot));
                        }
                    }
                    RotBondList = brptr->brnext;
                    _ffree(brptr);
                    brptr = RotBondList;
                }
                BLastRot = -999999.;
            } else
            {   brptr = brptr->brnext;
            }
        }
    }

    FindCisBonds();
    CapClean = False;
    return found;
    }

    return False;
}


/* Change bond order in the 'sens' direction; *
 * Fix bond order to order                    *
 * Remove Bond if both null.                  *
 * Record order Change                        */
int ChangeBondOrder( Long src, Long dst, int sens, int order )
{
    register RAtom __far *sptr;
    register RAtom __far *dptr;
    register Bond __far *bptr;

    if( src == dst )
        return False;

    sptr = LocateAtom(src,True);  if( !sptr ) return False;
    dptr = LocateAtom(dst,False); if( !dptr ) return False;

    bptr = (void __far*)0;
    ForEachBond
        if( ((bptr->srcatom==sptr)&&(bptr->dstatom==dptr)) || 
            ((bptr->srcatom==dptr)&&(bptr->dstatom==sptr)) )
            break;

    if( !bptr )
    {   if( sens==-1 )
        {   return False;
        } else if( sens==1 )
        {   if( !CreateBond( src, dst, NormBondFlag ) )
                return False;
            RecordBondOrder(CurMolecule->blist->srcatom,
                            CurMolecule->blist->dstatom,0,0);
            RecordBondOrder(CurMolecule->blist->srcatom,
                            CurMolecule->blist->dstatom,1,0);
            /* Ensure Drawing */
            bptr = (void __far*)0;
            ForEachBond
                if( ((bptr->srcatom==sptr)&&(bptr->dstatom==dptr)) || 
                    ((bptr->srcatom==dptr)&&(bptr->dstatom==sptr)) )
                    break;
            if( bptr )
                if( (bptr->flag&0xF)==SelectFlag )
                    bptr->flag |= DashFlag;
        } else if( sens==0 )
        {   if( order==1 )
            {   if( !CreateBond( src, dst, NormBondFlag ) )
                    return False;
                RecordBondOrder(CurMolecule->blist->srcatom,
                                CurMolecule->blist->dstatom,0,0);
                RecordBondOrder(CurMolecule->blist->srcatom,
                                CurMolecule->blist->dstatom,1,0);
            } else if( order==2 )
            {   if( !CreateBond( src, dst, DoubBondFlag ) )
                    return False;
                RecordBondOrder(CurMolecule->blist->srcatom,
                                CurMolecule->blist->dstatom,0,0);
                RecordBondOrder(CurMolecule->blist->srcatom,
                                CurMolecule->blist->dstatom,2,0);
            } else if( order==3 )
            {   if( !CreateBond( src, dst, TripBondFlag ) )
                    return False;
                RecordBondOrder(CurMolecule->blist->srcatom,
                                CurMolecule->blist->dstatom,0,0);
                RecordBondOrder(CurMolecule->blist->srcatom,
                                CurMolecule->blist->dstatom,3,0);
            } else if( !order )
            {   WriteString(MsgStrs(MolNoBndRem));
                return False;
            } else
                return False;
        }    
    } else /* Bond exist */
    {   if( sens==-1 )
        {   if( bptr->flag & NormBondFlag )
            {   /* Remove bond */
                if( !RemoveBond(src, dst) )
                    return False;
                RecordBondOrder(bptr->srcatom,bptr->dstatom,0,1);
            } else if( bptr->flag & DoubBondFlag )
            {   /* Convert Double to Single */
                bptr->flag &= ~(DoubBondFlag);
                bptr->flag |= NormBondFlag;
                RecordBondOrder(bptr->srcatom,bptr->dstatom,1,2);
            } else if( bptr->flag & TripBondFlag )
            {   /* Convert Triple to Double */
                DrawDoubleBonds = True;
                bptr->flag &= ~(TripBondFlag);
                bptr->flag |= DoubBondFlag;
                RecordBondOrder(bptr->srcatom,bptr->dstatom,2,3);
            } else
                return False;
        } else if( sens==1 ) 
        {   if( bptr->flag & NormBondFlag )
            {   /* Convert Single to Double */
                DrawDoubleBonds = True;
                bptr->flag &= ~(NormBondFlag);
                bptr->flag |= DoubBondFlag;
                RecordBondOrder(bptr->srcatom,bptr->dstatom,2,1);
            } else if( bptr->flag & DoubBondFlag )
            {   /* Convert Double to Triple */
                DrawDoubleBonds = True;
                bptr->flag &= ~(DoubBondFlag);
                bptr->flag |= TripBondFlag;
                RecordBondOrder(bptr->srcatom,bptr->dstatom,3,2);
            } else 
                return False;
        } else /* sens==0 */
        {   
            if( (bptr->flag&NormBondFlag) && (order==1) ) 
                return False;
            else if( (bptr->flag&DoubBondFlag) && (order==2) )
                return False;
            else if( (bptr->flag&TripBondFlag) && (order==3) )
                return False;

            if( bptr->flag&NormBondFlag )
                RecordBondOrder(bptr->srcatom,bptr->dstatom,1,1);
            else if( bptr->flag&DoubBondFlag )
                RecordBondOrder(bptr->srcatom,bptr->dstatom,2,2);
            else if( bptr->flag&TripBondFlag )
                RecordBondOrder(bptr->srcatom,bptr->dstatom,3,3);

            if( !order )
            {   if( !RemoveBond(src,dst) )
                    return False;
                RecordBondOrder(bptr->srcatom,bptr->dstatom,0,0);
                return True;
            }

            bptr->flag &= ~(NormBondFlag);
            bptr->flag &= ~(DoubBondFlag);
            bptr->flag &= ~(TripBondFlag);

            if( order==1 )
            {   /* Make Simple */
                bptr->flag |= NormBondFlag;
                RecordBondOrder(bptr->srcatom,bptr->dstatom,1,1);
            } else if( order==2 )
            {   /* Make Double */
                DrawDoubleBonds = True;
                bptr->flag |= DoubBondFlag;
                RecordBondOrder(bptr->srcatom,bptr->dstatom,2,2);
            } else if( order==3 )
            {   /* Make Triple */
                DrawDoubleBonds = True;
                bptr->flag |= TripBondFlag;
                RecordBondOrder(bptr->srcatom,bptr->dstatom,3,3);
            } else
                return False;
        }
    }
    return True;
}



int CreateNewBond (Long src, Long dst )
{
    register Bond __far *bptr;
    register Long bs,bd;

    ForEachBond
    {   bs = bptr->srcatom->serno;
        bd = bptr->dstatom->serno;

        if( ((bs==src)&&(bd==dst)) || ((bs==dst)&&(bd==src)) )
                   return False;
    }
    CreateBond( src, dst, NormBondFlag );
    return True;
}


static void TestBonded(  RAtom __far *sptr,  RAtom __far *dptr, int flag )
{
    register Bond __far *bptr;
    register Long dx, dy, dz;
    register Long max, dist;

    if ( !(sptr->model == dptr->model) ) return;
    if ( !(sptr->altl == ' ' || sptr->altl == '\0') &&
         !(dptr->altl == ' ' || dptr->altl == '\0') &&
         !(sptr->altl == dptr->altl) ) return;
    if( flag )
    {    /* Sum of covalent radii with 0.56A tolerance */
         dist = Element[sptr->elemno].covalrad + 
                Element[dptr->elemno].covalrad + 140;
         max = dist*dist;  
    } else 
    {    /* Fast Bio-Macromolecule Bonding Calculation */
         if( (sptr->flag|dptr->flag) & HydrogenFlag )
         {      max = MaxHBondDist;
         } else max = MaxBondDist;
    }
    
    dx = sptr->xorg-dptr->xorg 
         + sptr->fxorg-dptr->fxorg;   if( (dist=dx*dx)>max ) return;
    dy = sptr->yorg-dptr->yorg
         + sptr->fyorg-dptr->fyorg;   if( (dist+=dy*dy)>max ) return;
    dz = sptr->zorg-dptr->zorg
         + sptr->fzorg-dptr->fzorg;   if( (dist+=dz*dz)>max ) return;

    if( dist > MinBondDist )
    {   /* Reset Non-bonded flags! */
        sptr->flag &= ~NonBondFlag;
        dptr->flag &= ~NonBondFlag;

        bptr = ProcessBond(sptr,dptr,NormBondFlag);
        bptr->bnext = CurMolecule->blist;
        CurMolecule->blist = bptr;
        Info.bondcount++;
    }
}


static void ReclaimHBonds( HBond __far *ptr )
{
    register HBond __far *temp;

    if( ptr )
    {   temp = ptr;
        while( temp->hnext )
            temp=temp->hnext;
        temp->hnext = FreeHBond;
        FreeHBond = ptr;
    }
}


/* Reset all H-bonds and SS-Bonds */
void ReclaimAllBonds( void )
{
    if( !Database ) return;
    ReclaimHBonds( CurMolecule->slist );
    CurMolecule->slist = (void __far*)0;
    Info.ssbondcount = -1;

    ReclaimHBonds( CurMolecule->hlist );
    CurMolecule->hlist = (void __far*)0;
    CurHBond = &CurMolecule->hlist;
    Info.hbondcount = -1;
}

static void ReclaimBonds( Bond __far *ptr )
{
    register Bond __far *temp;

    if( ptr )
    {   temp = ptr;
        while( temp->bnext )
            temp=temp->bnext;
        temp->bnext = FreeBond;
        FreeBond = ptr;
    }
}


static Bond __far *ExtractBonds( Bond __far *ptr )
{
    register RAtom __far *src;
    register RAtom __far *dst;
    register Long dx, dy, dz;
    register Long max, dist;
    register Bond __far *result;
    register Bond __far *temp;

    result = (Bond __far*)0;

    while( ptr )
    {   temp = ptr;
        src = ptr->srcatom;
        dst = ptr->dstatom;

        ptr = temp->bnext;
        dist = Element[src->elemno].covalrad + 
                Element[dst->elemno].covalrad + 140;

        max = dist*dist;  
        dx = src->xorg-dst->xorg
             + src->fxorg-dst->fxorg; dist = dx*dx;
        if (!(dist > max)) {
          dy = src->yorg-dst->yorg
               + src->fyorg-dst->fyorg; dist += dy*dy;
        }
        if (!(dist > max)) {
          dz = src->zorg-dst->zorg
               + src->fzorg-dst->fzorg; dist += dz*dz;
        }

        if( (temp->flag & NormBondFlag) &&  !(dist > max) )
        {   temp->bnext = FreeBond;
            FreeBond = temp;
        } else /* Long or Double or Triple! */
        {   temp->bnext = result;
            result = temp;
        }
    }
    return result;
}


static void InsertBonds( Bond __far **list,  Bond __far *orig )
{
    register RAtom __far *src;
    register RAtom __far *dst;
    register Bond __far *temp;
    register Bond __far *ptr;

    while( orig )
    {   ptr = orig;
        orig = ptr->bnext;
        src = ptr->srcatom;
        dst = ptr->dstatom;
        for( temp = *list; temp; temp=temp->bnext )
            if( ((temp->srcatom==src)&&(temp->dstatom==dst)) ||
                ((temp->srcatom==dst)&&(temp->dstatom==src)) )
                break;

        if( temp )
        {   temp->flag = ptr->flag;
            ptr->bnext = FreeBond;
            FreeBond = ptr;
        } else
        {   ptr->bnext = *list;
            *list = ptr;
        }
    }
}


void CreateMoleculeBonds( int info, int flag, int force )
{
    register int i, x, y, z;
    register Long tx, ty, tz;
    register Long mx, my, mz; 
    register Long dx, dy, dz;
    register int lx, ly, lz, ux, uy, uz;
    register RAtom __far *aptr, __far *dptr;
    register Chain __far *chain;
    register Group __far *group;
    register Bond __far *list;
    char buffer[40];

    if( !Database ) 
        return;

    dx = (MaxX-MinX)+1;
    dy = (MaxY-MinY)+1;
    dz = (MaxZ-MinZ)+1;

    /* Save Explicit Long, Double and Triple Bonds on File Load */
    list = (Bond __far*)0;
    if (!force)
      list = ExtractBonds( CurMolecule->blist );
    CurMolecule->blist = (Bond __far*)0;
    Info.bondcount = 0;

    ResetVoxelData();

    for( chain=Database->clist; chain; chain=chain->cnext )
    {   for( group=chain->glist; group; group=group->gnext )
            for( aptr=group->alist; aptr; aptr=aptr->anext )
            {   /* Initially non-bonded! */
                aptr->flag |= NonBondFlag;

                mx = aptr->xorg + aptr->fxorg - MinX;
                my = aptr->yorg + aptr->fyorg - MinY;
                mz = aptr->zorg + aptr->fzorg - MinZ;

                tx = mx-AbsMaxBondDist;  
                ty = my-AbsMaxBondDist;  
                tz = mz-AbsMaxBondDist;  

                lx = (tx>0)? (int)((VOXORDER*tx)/dx) : 0;
                ly = (ty>0)? (int)((VOXORDER*ty)/dy) : 0;
                lz = (tz>0)? (int)((VOXORDER*tz)/dz) : 0;

                tx = mx+AbsMaxBondDist;  
                ty = my+AbsMaxBondDist;  
                tz = mz+AbsMaxBondDist;

                ux = (tx<dx)? (int)((VOXORDER*tx)/dx) : VOXORDER-1;
                uy = (ty<dy)? (int)((VOXORDER*ty)/dy) : VOXORDER-1;
                uz = (tz<dz)? (int)((VOXORDER*tz)/dz) : VOXORDER-1;

                for( x=lx; x<=ux; x++ )
                {   i = VOXORDER2*x + VOXORDER*ly;
                    for( y=ly; y<=uy; y++ )
                    {   for( z=lz; z<=uz; z++ )
                        {   dptr = (RAtom __far*)HashTable[i+z];
                            while( dptr )
                            {   TestBonded(aptr,dptr,flag);
                                dptr = dptr->next;
                            }
                        }
                        i += VOXORDER;
                    }
                }
                
                x = (int)((VOXORDER*mx)/dx);
            y = (int)((VOXORDER*my)/dy);
                z = (int)((VOXORDER*mz)/dz);

                i = VOXORDER2*x + VOXORDER*y + z;
                aptr->next = (RAtom __far*)HashTable[i];
                HashTable[i] = (void __far*)aptr;
            }
        VoxelsClean = False;
    }

    /* Replace Long, Double & Triple Bonds! */
    InsertBonds(&CurMolecule->blist,list);

    if( info )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(MolNumBds));
        sprintf(buffer,"%ld\n",(long)Info.bondcount);
        WriteString(buffer);
    }
}



/*=================================*/
/*  Disulphide bridging functions  */
/*=================================*/


RAtom __far *FindCysSulphur( Group __far *group )
{
    register RAtom __far *ptr;
    register char *elem;

    for( ptr=group->alist; ptr; ptr=ptr->anext )
    {   elem = ElemDesc[ptr->refno];
        if( (elem[1]=='S') && (elem[0]==' ')  )
            return( ptr );
    }
    return (RAtom __far*)0;
}


void TestDisulphideBridge( Group __far *group1, Group __far *group2,
                                  RAtom __far *cys1 )
{
    register HBond __far *ptr;
    register RAtom __far *cys2;
    register int dx, dy, dz;
    register Long max,dist;

    if( !(cys2=FindCysSulphur(group2)) )
        return;

    if ((group1->model) && (group2->model) &&
      (group1->model != group2->model)) return;

    max = (Long)750*750;
    dx = (int)(cys1->xorg-cys2->xorg
           + cys1->fxorg-cys2->fxorg);   if( (dist=(Long)dx*dx)>max ) return;
    dy = (int)(cys1->yorg-cys2->yorg
           + cys1->fyorg-cys2->fyorg);   if( (dist+=(Long)dy*dy)>max ) return;
    dz = (int)(cys1->zorg-cys2->zorg
           + cys1->fzorg-cys2->fzorg);   if( (dist+=(Long)dz*dz)>max ) return;

    if( !(ptr = FreeHBond) )
    {   MemSize += sizeof(HBond);
        ptr = (HBond __far*)_fmalloc(sizeof(HBond));
        if( !ptr ) RasMolFatalExit(MsgStrs(RfeDatabase));
        RegisterAlloc( ptr );
    } else FreeHBond = ptr->hnext;

    ptr->dst = cys1;
    if( !(ptr->dstCA=FindGroupAtom(group1,1)) )
        ptr->dstCA = cys1;

    ptr->src = cys2;
    if( !(ptr->srcCA=FindGroupAtom(group2,1)) )
        ptr->srcCA = cys2;

    ptr->offset = 0;
    ptr->energy = 0;
    ptr->flag = 0;
    ptr->col = 0;

    ptr->hnext = CurMolecule->slist;
    CurMolecule->slist = ptr;

    group1->flag |= CystineFlag;
    group2->flag |= CystineFlag;
    Info.ssbondcount++;
}


void FindDisulphideBridges( void )
{
    register Chain __far *chn1;
    register Chain __far *chn2;
    register Group __far *group1;
    register Group __far *group2;
    register RAtom __far *cys;
    char buffer[40];

    if( !Database ) return;
    ReclaimHBonds( CurMolecule->slist );
    CurMolecule->slist = (void __far*)0;
    Info.ssbondcount = 0;

    for(chn1=Database->clist;chn1;chn1=chn1->cnext)
        for(group1=chn1->glist;group1;group1=group1->gnext)
            if( IsCysteine(group1->refno) )
            {   cys = FindCysSulphur(group1);
                if( cys )
                {   for(group2=group1->gnext;group2;group2=group2->gnext)
                        if( IsCysteine(group2->refno) )
                            TestDisulphideBridge(group1,group2,cys);

                    for(chn2=chn1->cnext;chn2;chn2=chn2->cnext)
                        for(group2=chn2->glist;group2;group2=group2->gnext)
                            if( IsCysteine(group2->refno) )
                                TestDisulphideBridge(group1,group2,cys);
            }
            }

    if( FileDepth == -1 )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(MolNumBr));
        sprintf(buffer,"%d\n",Info.ssbondcount);
        WriteString(buffer);
    }
}



void FindCisBonds( void )
{
  register Chain __far *chain;
  register Group __far *group;
  register Group __far *prev;

  if( !Database ) return;
  Info.cisbondcount = 0; /* Number of Bonds not used at the moment ... */
 
  /*zeroed CisBonFlag*/
  for(chain=Database->clist;chain;chain=chain->cnext)
    for(group=chain->glist;group;group=group->gnext)      
        group->flag &= (~CisBondFlag);
 
  for(chain=Database->clist;chain;chain=chain->cnext)
  {    prev = (Group __far *)0;
    for(group=chain->glist;group;group=group->gnext)      
    {    if( prev && IsAmino(prev->refno) && IsAmino(group->refno))
        {
        /* if( fabs(- (CalcOmegaAngle(prev, group) - 180.0)) < CisBondCutOff ){ */
            if( fabs(CalcOmegaAngle(prev, group)) < CisBondCutOff )
            {    group->flag |= CisBondFlag;
                prev->flag |= CisBondFlag;
                Info.cisbondcount++;  
            } else if( group->flag&CisBondFlag && CisBondFlag&prev->flag )      
             {   group->flag &= (~CisBondFlag);
                prev->flag  &= (~CisBondFlag);
            }
        }
        prev = group;
    }
  }   
}


/*================================================*/
/*  Kabsch & Sander DSSP Structure Determination  */
/*================================================*/

/* Coupling constant for Electrostatic Energy   */
/* QConst = -332 * 0.42 * 0.2 * 1000.0 [*250.0] */
#define QConst (-6972000.0)
#define MaxHDist ((Long)2250*2250)
#define MinHDist ((Long)125*125)


/* Protein Donor Atom Coordinates */
static int hxorg,hyorg,hzorg;
static int nxorg,nyorg,nzorg;
static RAtom __far *best1CA;
static RAtom __far *best2CA;
static RAtom __far *best1;
static RAtom __far *best2;
static RAtom __far *optr;
static int res1,res2;
static int off1,off2;


static int CalculateBondEnergy( Group __far *group )
{
    register double dho,dhc;
    register double dnc,dno;

    register RAtom __far *cptr;
    register Long dx,dy,dz;
    register Long dist;
    register int result;

    if( !(cptr=FindGroupAtom(group,2)) )  return(0);
    if( !(optr=FindGroupAtom(group,3)) )  return(0);

    dx = hxorg - optr->xorg - optr->fxorg;  
    dy = hyorg - optr->yorg - optr->fyorg;  
    dz = hzorg - optr->zorg - optr->fzorg;
    dist = dx*dx+dy*dy+dz*dz;
    if( dist < MinHDist ) 
        return( -9900 );
    dho = sqrt((double)dist);

    dx = hxorg - cptr->xorg - cptr->fxorg;  
    dy = hyorg - cptr->yorg - cptr->fyorg;  
    dz = hzorg - cptr->zorg - cptr->fzorg;
    dist = dx*dx+dy*dy+dz*dz;
    if( dist < MinHDist ) 
        return( -9900 );
    dhc = sqrt((double)dist);

    dx = nxorg - optr->xorg - optr->fxorg;  
    dy = nyorg - optr->yorg - optr->fyorg;  
    dz = nzorg - optr->zorg - optr->fzorg;
    dist = dx*dx+dy*dy+dz*dz;
    if( dist < MinHDist ) 
        return( -9900 );
    dnc = sqrt((double)dist);

    dx = nxorg - optr->xorg - optr->fxorg;  
    dy = nyorg - optr->yorg - optr->fyorg;  
    dz = nzorg - optr->zorg - optr->fzorg;
    dist = dx*dx+dy*dy+dz*dz;
    if( dist < MinHDist ) 
        return( -9900 );
    dno = sqrt((double)dist);

    result = (int)(QConst/dho - QConst/dhc + QConst/dnc - QConst/dno);

    if( result<-9900 ) 
    {   return -9900;
    } else if( result>-500 ) 
        return 0;
    return result;
}


static void CalcProteinHBonds( Chain __far *chn1 )
{
    register int energy, offset;
    register Chain __far *chn2;
    register Group __far *group1;
    register Group __far *group2;
    register RAtom __far *ca1;
    register RAtom __far *ca2;
    register RAtom __far *pc1;
    register RAtom __far *po1;
    register RAtom __far *n1;
    register int pos1,pos2;
    register int dx,dy,dz;
    register double dco;
    register Long dist;

    pos1 = 0;
    pc1 = po1 = (void __far*)0;
    for(group1=chn1->glist;group1;group1=group1->gnext)
    {   pos1++;
        if( pc1 && po1 )
        {   dx = (int)(pc1->xorg - po1->xorg + pc1->fxorg - po1->fxorg );
            dy = (int)(pc1->yorg - po1->yorg + pc1->fyorg - po1->fyorg);
            dz = (int)(pc1->zorg - po1->zorg + pc1->fzorg - po1->fzorg);
        } else
        {   pc1 = FindGroupAtom(group1,2);
            po1 = FindGroupAtom(group1,3);
            continue;
        }

        pc1 = FindGroupAtom(group1,2);
        po1 = FindGroupAtom(group1,3);

        if( !IsAmino(group1->refno) || IsProline(group1->refno) )
            continue;

        if( !(ca1=FindGroupAtom(group1,1)) ) continue;
        if( !(n1=FindGroupAtom(group1,0)) )  continue;

        dist = (Long)dx*dx + (Long)dy*dy + (Long)dz*dz;
        dco = sqrt( (double)dist )/250.0;

        nxorg = (int)(n1->xorg + n1->fxorg);   hxorg = nxorg + (int)(dx/dco);
        nyorg = (int)(n1->yorg + n1->fyorg);   hyorg = nyorg + (int)(dy/dco);
        nzorg = (int)(n1->zorg + n1->fzorg);   hzorg = nzorg + (int)(dz/dco);
        res1 = res2 = 0;

        if( HBondChainsFlag )
        {   /* Hydrogen bond between chains! */
            chn2=Database->clist;
        } else chn2 = chn1;

        
        do {
            /* Only consider non-empty peptide chains! */
            if( !chn2->glist || !IsProtein(chn2->glist->refno) )
            {   chn2 = chn2->cnext;
                continue;
            }

            pos2 = 0;
            for(group2=chn2->glist;group2;group2=group2->gnext)
            {   pos2++;
                if( (group2==group1) || (group2->gnext==group1) )
                    continue;

                if( !IsAmino(group2->refno) ) 
                    continue;
                if( !(ca2=FindGroupAtom(group2,1)) ) 
                    continue;

                dx = (int)(ca1->xorg-ca2->xorg + ca1->fxorg-ca2->fxorg);
                if( (dist=(Long)dx*dx) > MaxHDist )
                    continue;

                dy = (int)(ca1->yorg-ca2->yorg + ca1->fyorg-ca2->fyorg);
                if( (dist+=(Long)dy*dy) > MaxHDist )
                    continue;

                dz = (int)(ca1->zorg-ca2->zorg + ca1->fzorg-ca2->fzorg);
                if( (dist+=(Long)dz*dz) > MaxHDist )
                    continue;

                energy = CalculateBondEnergy(group2);
                if( energy )
                {   if( chn1 == chn2 )
                    {   offset = pos1 - pos2;
                    } else offset = 0;

                    if( energy<res1 )
                    {   best2CA = best1CA;  best1CA = ca2;
                        best2 = best1;      best1 = optr;
                        res2 = res1;        res1 = energy;
                        off2 = off1;        off1 = offset;
                    } else if( energy<res2 )
                    {   best2CA = ca2;
                        best2 = optr;
                        res2 = energy;
                        off2 = offset;
                    }
                }
            }  /* group2 */
            chn2 = chn2->cnext;
        } while( HBondChainsFlag && chn2 );

        if( res1 ) 
        {   if( res2 ) 
                CreateHydrogenBond(ca1,best2CA,n1,best2,res2,off2);
            CreateHydrogenBond(ca1,best1CA,n1,best1,res1,off1);
        }
    }
}


static void CalcNucleicHBonds( Chain __far *chn1 )
{
    register Chain __far *chn2;
    register Group __far *group1;
    register Group __far *group2;
    register Group __far *best;
    register RAtom __far *ca1;
    register RAtom __far *ca2;
    register RAtom __far *n1;
    register Long max,dist;
    register int dx,dy,dz;
    register int refno;

    for(group1=chn1->glist;group1;group1=group1->gnext)
    {   if( !IsPurine(group1->refno) ) continue;
        /* Find N1 of Purine Group */
        n1 = FindGroupAtom(group1,21);
        if( !n1 ) continue;

        /* Maximum N1-N3 distance 5A */
        refno = NucleicCompl(group1->refno);
        max = (Long)1250*1250;
        best = (void __far*)0;

        for(chn2=Database->clist;chn2;chn2=chn2->cnext)
        {   /* Only consider non-empty nucleic acid chains! */
            if( (chn1==chn2) || !chn2->glist || 
                !IsDNA(chn2->glist->refno) )
                continue;

            for(group2=chn2->glist;group2;group2=group2->gnext)
                if( group2->refno == (Byte)refno )
                {   /* Find N3 of Pyramidine Group */
                    if( !(ca1=FindGroupAtom(group2,23)) )
                        continue;

                    dx = (int)(ca1->xorg - n1->xorg + ca1->fxorg - n1->fxorg);
                    if( (dist=(Long)dx*dx) >= max ) 
                        continue;

                    dy = (int)(ca1->yorg - n1->yorg + ca1->fyorg - n1->fyorg);
                    if( (dist+=(Long)dy*dy) >= max ) 
                        continue;

                    dz = (int)(ca1->zorg - n1->zorg + ca1->fzorg - n1->fzorg);
                    if( (dist+=(Long)dz*dz) >= max )
                        continue;

                    best1 = ca1;
                    best = group2;
                    max = dist;
                }
        }

        if( best )
        {   /* Find the sugar phosphorous atoms */
            ca1 = FindGroupAtom( group1, 7 );
            ca2 = FindGroupAtom( best, 7 );

            CreateHydrogenBond( ca1, ca2, n1, best1, 0, 0 );
            if( IsGuanine(group1->refno) )
            {   /* Guanine-Cytosine */
                ca1 = FindGroupAtom(group1,22);   /* G.N2 */
                ca2 = FindGroupAtom(best,26);     /* C.O2 */
                if( ca1 && ca2 )
                    CreateHydrogenBond( (void __far*)0, (void __far*)0,
                                        ca1, ca2, 0, 0 );

                ca1 = FindGroupAtom(group1,28);   /* G.O6 */
                ca2 = FindGroupAtom(best,24);     /* C.N4 */
                if( ca1 && ca2 )
                    CreateHydrogenBond( (void __far*)0, (void __far*)0,
                                        ca1, ca2, 0, 0 );

            } else /* Adenine-Thymine */
            {   ca1 = FindGroupAtom(group1,25);  /* A.N6 */
                ca2 = FindGroupAtom(best,27);    /* T.O4 */
                if( ca1 && ca2 )
                    CreateHydrogenBond( (void __far*)0, (void __far*)0,
                                        ca1, ca2, 0, 0 );
            }
        }
    }
}


void CalcHydrogenBonds( void )
{
    register Chain __far *chn1;
    char buffer[40];

    if( !Database ) return;
    ReclaimHBonds( CurMolecule->hlist );
    CurMolecule->hlist = (void __far*)0;
    CurHBond = &CurMolecule->hlist;
    Info.hbondcount = 0;

    if( MainAtomCount > 10000 )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(MolPlease));
    }

    for(chn1=Database->clist; chn1; chn1=chn1->cnext)
        if( chn1->glist )
        {   if( IsProtein(chn1->glist->refno) )
            {   CalcProteinHBonds(chn1);
            } else if( IsDNA(chn1->glist->refno) )
                CalcNucleicHBonds(chn1);
        }

    if( FileDepth == -1 )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(MolNumHB));
        sprintf(buffer,"%d\n",Info.hbondcount);
        WriteString(buffer);
    }
}


static int IsHBonded( RAtom __far *src, RAtom __far *dst, HBond __far *ptr )
{
    while( ptr && (ptr->srcCA==src) )
        if( ptr->dstCA == dst )
        {   return True;
        } else ptr=ptr->hnext;
    return False;
}


static void FindAlphaHelix( int pitch, int flag )
{
    register HBond __far *hbond;
    register Chain __far *chain;
    register Group __far *group;
    register Group __far *first;
    register Group __far *ptr;
    register RAtom __far *srcCA;
    register RAtom __far *dstCA;
    register int res,dist,prev;

    /* Protein chains only! */
    hbond = Database->hlist;
    for( chain=Database->clist; chain; chain=chain->cnext )
    {   first = chain->glist;
        if( first  && IsProtein(first->refno) )
        {   /* Scan Protein Chain for Helix */
            prev = False; dist = 0;
            for( group=chain->glist; hbond && group; group=group->gnext )
            {   if( IsAmino(group->refno) )
                {   srcCA = FindGroupAtom(group,1);
                    if( srcCA )
                    {   if( dist == pitch )
                        {   res = False;
                            dstCA = FindGroupAtom(first,1);
                            while( hbond && (hbond->srcCA==srcCA) )
                            {   if( hbond->dstCA==dstCA ) res = True;
                                hbond = hbond->hnext;
                            }

                            if( res )
                            {   if( prev )
                                {   if( !(first->struc&HelixFlag) ) 
                                    Info.helixcount++;

                                    ptr = first;
                                    do {
                                        ptr->struc |= flag;
                                        ptr = ptr->gnext;
                                    } while( ptr != group );
                                } else prev = True;
                            } else prev = False;
                        } else while( hbond && (hbond->srcCA==srcCA) )
                            hbond = hbond->hnext;
                    } else prev = False;
                } else prev = False;

                if( group->struc & HelixFlag )
                {   first = group; prev = False; dist = 1;
                } else if( dist == pitch )
                {   first = first->gnext;
                } else dist++;
            }

        } else if( first && IsNucleo(first->refno) )
            while( hbond && !IsAminoBackbone(hbond->src->refno) )
                hbond = hbond->hnext;
    }
}


static RAtom __far *cprevi, __far *ccurri, __far *cnexti;
static HBond __far *hcurri, __far *hnexti;
static Group __far *curri, __far *nexti;


static void TestLadder( Chain __far *chain )
{
    register RAtom __far *cprevj, __far *ccurrj, __far *cnextj;
    register HBond __far *hcurrj, __far *hnextj;
    register Group __far *currj, __far *nextj;
    register int count, result, found;

    /* Avoid Compiler Warnings! */
    cprevj = ccurrj = (RAtom __far*)0;
    hcurrj = (HBond __far*)0;
    currj  = (Group __far*)0;

    /* Already part of atleast one ladder */
    found = curri->flag & SheetFlag;
    nextj = nexti->gnext;

    hnextj = hnexti;
    while( hnextj && hnextj->srcCA==cnexti )
        hnextj = hnextj->hnext;

    while( chain )
    {   if( nextj )
            {
            if( IsProtein(chain->glist->refno) )
            {   count = 1;
                do {
                    cnextj = FindGroupAtom(nextj,1);
                    if( count == 3 )
                    {   if( IsHBonded(cnexti,ccurrj,hnexti) &&
                            IsHBonded(ccurrj,cprevi,hcurrj) )
                        {   result = ParaLadder;
                        } else if( IsHBonded(cnextj,ccurri,hnextj) &&
                                   IsHBonded(ccurri,cprevj,hcurri) )
                        {   result = ParaLadder;
                        } else if( IsHBonded(cnexti,cprevj,hnexti) &&
                                   IsHBonded(cnextj,cprevi,hnextj) )
                        {   result = AntiLadder;
                        } else if( IsHBonded(ccurrj,ccurri,hcurrj) &&
                                   IsHBonded(ccurri,ccurrj,hcurri) )
                        {   result = AntiLadder;
                        } else result = NoLadder;

                        if( result )
                        {   curri->struc |= SheetFlag;
                            currj->struc |= SheetFlag;
                            if( found ) return;
                            found = True;
                        }
                    } else count++;

            cprevj = ccurrj; ccurrj = cnextj; 
            currj = nextj;   hcurrj = hnextj;

            while( hnextj && hnextj->srcCA==cnextj )
            hnextj = hnextj->hnext;
                nextj = nextj->gnext;
        } while( nextj );

        } else if( IsNucleo(chain->glist->refno) )
        while( hnextj && !IsAminoBackbone(hnextj->src->refno) )
            hnextj = hnextj->hnext;
            }
        chain = chain->cnext;
    if( chain )
        nextj = chain->glist;
    }
}


static void FindBetaSheets( void )
{
    register Chain __far *chain;
    register int ladder;
    register int count;

    hnexti = Database->hlist;
    for( chain=Database->clist; chain; chain=chain->cnext )
    {   nexti = chain->glist;
    if( nexti )
    {   if( IsProtein(nexti->refno) )
        {   count = 1;
        ladder = False;
        do {
            cnexti = FindGroupAtom(nexti,1);

            if( count == 3 )
            {   TestLadder( chain );
            if( curri->struc & SheetFlag )
            {   if( !ladder )
                {   Info.laddercount++;
                ladder = True;
                }
            } else ladder = False;
            } else count++;

            cprevi = ccurri; ccurri = cnexti; 
            curri = nexti;   hcurri = hnexti;
            while( hnexti && hnexti->srcCA==cnexti )
            hnexti = hnexti->hnext;
                nexti = nexti->gnext;
        } while( nexti );

        } else if( IsNucleo(nexti->refno) )
        while( hnexti && !IsAminoBackbone(hnexti->src->refno) )
            hnexti = hnexti->hnext;
        }
    }
}


static void FindTurnStructure( void )
{
    static RAtom __far *aptr[5];
    register Chain __far *chain;
    register Group __far *group;
    register Group __far *prev;
    register RAtom __far *ptr;
    register Long ux,uy,uz,mu;
    register Long vx,vy,vz,mv;
    register int i,found,len;
    register double CosKappa;
    /* Avoid compiler warnings */
    prev = (Group __far*)0;

    for( chain=Database->clist; chain; chain=chain->cnext )
    if( chain->glist && IsProtein(chain->glist->refno) )
    {   len = 0;  found = False;
        for( group=chain->glist; group; group=group->gnext )
        {    ptr = FindGroupAtom(group,1);
         if( ptr && (ptr->flag&BreakFlag) )
         {   found = False;
             len = 0;
         } else if( len==5 )
         {   for( i=0; i<4; i++ )
             aptr[i] = aptr[i+1];
             len = 4;
         } else if( len==2 )
             prev = group;

         aptr[len++] = ptr;
         if( len==5 ) 
         {   if( !(prev->struc&(HelixFlag|SheetFlag)) &&
             aptr[0] && aptr[2] && aptr[4] )
             {   ux = aptr[2]->xorg - aptr[0]->xorg
                             + aptr[2]->fxorg - aptr[0]->fxorg;
             uy = aptr[2]->yorg - aptr[0]->yorg
                             + aptr[2]->fyorg - aptr[0]->fyorg;
             uz = aptr[2]->zorg - aptr[0]->zorg
                             + aptr[2]->fzorg - aptr[0]->fzorg;

             vx = aptr[4]->xorg - aptr[2]->xorg
                             + aptr[4]->fxorg - aptr[2]->fxorg;
             vy = aptr[4]->yorg - aptr[2]->yorg
                             + aptr[4]->fyorg - aptr[2]->fyorg;
             vz = aptr[4]->zorg - aptr[2]->zorg
                             + aptr[4]->fzorg - aptr[2]->fzorg;
#ifdef INVERT
                         uy = -uy;
                         vy = -vy;
#endif
                         uz = -uz;
                         vz = -vz;

             mu = ux*ux + uy*uy + uz*uz;
             mv = vx*vx + vz*vz + vy*vy;
             if( mu && mv )
             {   CosKappa = (double)(ux*vx + uy*vy + uz*vz);
                 CosKappa /= sqrt( (double)mu*mv );
                 if( CosKappa<Cos70Deg )
                 {   if( !found )
                     Info.turncount++;
                 prev->struc |= TurnFlag;
                 }
             }
             }
             found = prev->struc&TurnFlag;
             prev = prev->gnext;
         } /* len==5 */
        }
    }
}


static void FindBetaTurns( void )
{
    static RAtom __far *aptr[4];
    register Chain __far *chain;
    register Group __far *group;
    register Group __far *prev;
    register Group __far *next;
    register RAtom __far *ptr;
    register Long dx,dy,dz;
    register int found,len;
    register int flag;

    for( chain=Database->clist; chain; chain=chain->cnext )
        if( chain->glist && IsProtein(chain->glist->refno) )
        {   prev = chain->glist;  
            len = 0;  found = False;
            for( next=chain->glist; next; next=next->gnext )
            {   ptr = FindGroupAtom(next,1);
                if( ptr && (ptr->flag&BreakFlag) )
                {   found = False;
                    prev = next;
                    len = 0;
                } else if( len==4 )
                {   aptr[0] = aptr[1];
                    aptr[1] = aptr[2];
                    aptr[2] = aptr[3];
                    aptr[3] = ptr;

                } else aptr[len++] = ptr;
                if( len==4 ) 
                {   flag = False;
                    if( aptr[0] && aptr[3] )
                    {   dx = aptr[3]->xorg - aptr[0]->xorg
                             + aptr[3]->fxorg - aptr[0]->fxorg;
                        dy = aptr[3]->yorg - aptr[0]->yorg
                             + aptr[3]->fyorg - aptr[0]->fyorg;
                        dz = aptr[3]->zorg - aptr[0]->zorg
                             + aptr[3]->fzorg - aptr[0]->fzorg;
                        if( dx*dx + dy*dy + dz*dz < (Long)1750*1750 )
                        {   group = prev;
                            while( group!=next->gnext )
                            {   if( !(group->struc&(HelixFlag|SheetFlag)) )
                                {   group->struc |= TurnFlag;
                                    flag = True;
                                }
                                group = group->gnext;
                            }
                            if( !found && flag ) 
                                Info.turncount++;
                        }
                    }
                    prev = prev->gnext;   
                    found = flag;
                } /* len==4 */
            }
        }
}


void DetermineStructure( int flag )
{
    register Chain __far *chain;
    register Group __far *group;
    char buffer[40];

    if( !Database )
    return;

    if( Info.hbondcount < 0 )
    CalcHydrogenBonds();

    if( Info.helixcount >= 0 )
    for( chain=Database->clist; chain; chain=chain->cnext )
        for( group=chain->glist; group; group=group->gnext )
        group->struc = 0;

    Info.structsource = SourceCalc;
    Info.laddercount = 0;
    Info.helixcount = 0;
    Info.turncount = 0;

    if( Info.hbondcount )
    {   FindAlphaHelix(4,Helix4Flag);
    FindBetaSheets();
    FindAlphaHelix(3,Helix3Flag);
    FindAlphaHelix(5,Helix5Flag);

        if( !flag )
    {   FindTurnStructure();
        } else FindBetaTurns();
    }

    if( FileDepth == -1 )
    {   InvalidateCmndLine();

        WriteString(MsgStrs(MolNumHel));
        sprintf(buffer,"%d\n",Info.helixcount);
        WriteString(buffer);
        WriteString(MsgStrs(MolNumStr));
        sprintf(buffer,"%d\n",Info.laddercount);
        WriteString(buffer);
        WriteString(MsgStrs(MolNumTurn));
        sprintf(buffer,"%d\n",Info.turncount);
        WriteString(buffer);
    }
}


void RenumberMolecule( int start )
{
    register Chain __far *chain;
    register Group __far *group;
    register int hinit, minit;
    register int resno;

    if( !Database )
    return;

    hinit = minit = False;
    for( chain=Database->clist; chain; chain=chain->cnext )
    {   resno = start;
    for( group=chain->glist; group; group=group->gnext )
    {   if( group->alist->flag & HeteroFlag )
        {   if( hinit )
        {   if( resno > MaxHetaRes )
            {   MaxHetaRes = resno;
            } else if( resno < MinHetaRes )
            MinHetaRes = resno;
        } else MinHetaRes = MaxHetaRes = resno;
        hinit = True;
        } else
        {   if( minit )
        {   if( resno > MaxMainRes )
            {   MaxMainRes = resno;
            } else if( resno < MinMainRes )
            MinMainRes = resno;
        } else MinMainRes = MaxMainRes = resno;
        minit = True;
        }
        group->serno = resno++;
    }
    }
}



/*===============================*/
/* Molecule Database Maintenance */
/*===============================*/

static void ReclaimAtoms( RAtom __far *ptr )
{
    register RAtom __far *temp;

    if( ptr )
    {   temp = ptr;
        while( temp->anext )
        temp=temp->anext;
    temp->anext = FreeAtom;
    FreeAtom = ptr;
    }
}


static void ResetDatabase( void )
{
    int i,j;

    Cache = (Group __far*)0;

    Database = CurMolecule = (void __far*)0;
    MainGroupCount = HetaGroupCount = 0;
    Info.chaincount = 0;
    Info.bondcount = 0;
    HetaAtomCount = 0;
    MainAtomCount = 0;  
    SelectCount = 0;

    Info.ssbondcount = -1;
    Info.hbondcount = -1;

    Info.structsource = SourceNone;
    Info.laddercount = -1;
    Info.helixcount = -1;
    Info.turncount = -1;

    CurGroup = (void __far*)0;
    CurChain = (void __far*)0;
    CurAtom = (void __far*)0;

    MinX = MinY = MinZ = 0;
    MaxX = MaxY = MaxZ = 0;

    MinMainTemp = MaxMainTemp = 0;
    MinHetaTemp = MaxHetaTemp = 0;
    MinMainRes = MaxMainRes = 0;
    MinHetaRes = MaxHetaRes = 0;
    MinAltl = MaxAltl = 0;

    MinModel = MaxModel = 0;

    *Info.moleculename = 0;
    *Info.classification = 0;
    *Info.spacegroup = 0;
    *Info.identcode = 0;
    *Info.filename = 0;
    *Info.technique = 0;
    *Info.date = 0;

    VoxelsClean = False;
    HMinMaxFlag = False;
    MMinMaxFlag = False;
    HasHydrogen = False;
    ElemNo = MINELEM;
    ResNo = MINRES;
    MaskCount = 0;
    NMRModel = 0;
    NullBonds = 0;
    CapClean = False;

    for ( i=0; i<3; i++ ) {
      Info.vecf2o[i] = Info.veco2f[i] = Info.cell[i] = 0.;
        Info.cell[i+3] = 90.;
      for ( j=0; j<3; j++) {
        Info.matf2o[i][j] = Info.mato2f[i][j] = ((i!=j)?0.:1.);
        }
    }

}


void DestroyDatabase( void )
{
    register void __far *temp;
    register Group __far *gptr;

    if( Database )
    {   ReclaimHBonds( Database->slist );
        ReclaimHBonds( Database->hlist );
        ReclaimBonds( Database->blist );

        while( Database->clist )
        {   ReclaimBonds(Database->clist->blist);
            gptr = Database->clist->glist;
            if( gptr )
            {   ReclaimAtoms(gptr->alist);
                while( gptr->gnext )
                {   gptr = gptr->gnext;
                    ReclaimAtoms(gptr->alist);
                }
                gptr->gnext = FreeGroup;
                FreeGroup = Database->clist->glist;
            }
            temp = Database->clist->cnext;
            Database->clist->cnext = FreeChain;
            FreeChain = Database->clist;
            Database->clist = temp;
        }

        FreeMolecule = Database;
        Database = (void __far*)0;
    }
    ResetDatabase();
}


/* Avoid System Memory Leaks at closure! */
void RegisterAlloc( void *data )
{   register AllocRef *ptr;
    
    if( !AllocList || (AllocList->count==AllocSize) )
    {   ptr = (AllocRef *)_fmalloc( sizeof(AllocRef) );
        if( !ptr ) 
            RasMolFatalExit(MsgStrs(RfeDatabase));
        ptr->next = AllocList;
        ptr->data[0] = data;
        ptr->count = 1;
        AllocList = ptr;
    } else AllocList->data[AllocList->count++] = data;
    AllocAdd ++;
}


void FreeAlloc( data )
    void *data;
{
    _ffree(data);
}


void PurgeDatabase( void )
{
    register AllocRef *ptr;
    register AllocRef *tmp;
    register int i;
    
    /* Avoid Memory Leaks at exit in debug mode */
    for( ptr=AllocList; ptr; ptr=tmp )
    {   for( i=0; i<ptr->count; i++ )
            if (ptr->data[i]) {
                _ffree( ptr->data[i] );
                AllocRem++;
            }
        tmp = ptr->next;
        _ffree( ptr );
    }

    AllocList = (void*)0;
    FreeMolecule = (void __far*)0;
    FreeChain = (void __far*)0;
    FreeGroup = (void __far*)0;
    FreeAtom = (void __far*)0;
    FreeBond = (void __far*)0;
    FreeHBond = (void __far*)0;
}


void InitialiseDatabase( void )
{
    FreeMolecule = (void __far*)0;
    FreeHBond = (void __far*)0;
    FreeChain = (void __far*)0;
    FreeGroup = (void __far*)0;
    FreeAtom = (void __far*)0;
    FreeBond = (void __far*)0;
    Info.cisbondcount = -1; /* to ititialize it has to be < 0 */   
    CisBondCutOff = CIS;

    AllocList = (void*)0;

    HBondChainsFlag = False;
    ResetDatabase();
}

