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
 *  Isabel Serván Martínez,                                                *
 *  José Miguel Fernández Fernández      2.6   Manual              Spanish *
 *  José Miguel Fernández Fernández      2.7.1 Manual              Spanish *
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

/* Portion copyright (c) 2007, Philippe Valadon */


/* multiple.c
/*

/* Original File Header:
 */
/**********************************************************************
  Copyright (c) 1995 UC Regents, ModularCHEM Consortium

  multiple.c
  Multiple Molecule Support for Rasmol
  
  Author:      Gary Grossman (garyg@cory.EECS.Berkeley.EDU)
  Last Update: November 11, 1995
 **********************************************************************/

#include "rasmol.h"

#ifdef IBMPC
#include <windows.h>
#include <malloc.h>
#endif
#ifdef APPLEMAC
#ifdef __CONDITIONALMACROS__
#include <Printing.h>
#else
#include <PrintTraps.h>
#endif
#include <Types.h>
#include <QDOffscreen.h>
#endif
#ifndef sun386
#include <stdlib.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <math.h>

#if !defined(IBMPC) && !defined(VMS) && !defined(APPLEMAC)
#include <pwd.h>
#endif

#include <stdarg.h>

#define MULTIPLE_C

#include "command.h"
#include "cmndline.h"
#include "tokens.h"
#include "molecule.h"
#include "abstree.h"
#include "transfor.h"
#include "render.h"
#include "graphics.h"
#include "pixutils.h"
#include "repres.h"
#include "multiple.h"
#include "outfile.h"
#include "script.h"
#include "vector.h"
#include "brotate.h"
#include "langsel.h"


#define IsZero(x) ( ((x)<0.000000000000001)&&((x)>-0.000000000000001) )

static char *frindex(char *s, char c)
{
    register char *t = s;
    
    while (*t)
        t++;
	while (t != s) {
		if (*t == c)
            return t;
        t--;
    }
    return NULL;
}



#define MSG_LINES 4

/**********************************************************************
  Each molecule has an "instance" of these variables 

  To switch between molecules, the current variables are swapped
  out, and the new molecule's variables are swapped in.

  PV[11/30/00]
  The static set of instances has been replaced by a linked list of
  molecules. 
 **********************************************************************/
  
static int VarSize;

struct {
    void   *data;
    int     size;
    int     offset;
} VarList[] = {

    /* MOffset must be the first one - See GetMOffset() */
    MOffset,            sizeof (MOffset),        0,

    /* abstree.h */
    &SymbolTable,       sizeof (SymbolTable),    0,
    
   /* graphics.h */
    
    DialValue,          sizeof (DialValue),      0,

    /* molecule.h */
    &Info,              sizeof (Info),           0,
    &MainGroupCount,    sizeof (MainGroupCount), 0,
    &HetaGroupCount,    sizeof (HetaGroupCount), 0,
    &MainAtomCount,     sizeof (MainAtomCount),  0,
    &HetaAtomCount,     sizeof (HetaAtomCount),  0,
    &CisBondCutOff,     sizeof (CisBondCutOff),  0,
    &MinX,              sizeof (MinX),           0,
    &MinY,              sizeof (MinY),           0,
    &MinZ,              sizeof (MinZ),           0,
    &MaxX,              sizeof (MaxX),           0,
    &MaxY,              sizeof (MaxY),           0,
    &MaxZ,              sizeof (MaxZ),           0,
    &HMinMaxFlag,       sizeof (HMinMaxFlag),    0,
    &MMinMaxFlag,       sizeof (MMinMaxFlag),    0,
    &MinMainTemp,       sizeof (MinMainTemp),    0,
    &MaxMainTemp,       sizeof (MaxMainTemp),    0,
    &MinHetaTemp,       sizeof (MinHetaTemp),    0,
    &MaxHetaTemp,       sizeof (MaxHetaTemp),    0,
    &MinMainRes,        sizeof (MinMainRes),     0,
    &MaxMainRes,        sizeof (MaxMainRes),     0,
    &MinHetaRes,        sizeof (MinHetaRes),     0,
    &MaxHetaRes,        sizeof (MaxHetaRes),     0,
    &MinAltl,           sizeof (MinAltl),        0,
    &MaxAltl,           sizeof (MaxAltl),        0,
    &CurMolecule,       sizeof (CurMolecule),    0,
    &CurChain,          sizeof (CurChain),       0,
    &CurGroup,          sizeof (CurGroup),       0,
    &CurAtom,           sizeof (CurAtom),        0,
    &Database,          sizeof (Database),       0,
    UserMask,           sizeof (UserMask),       0,
    &MinBondDist,       sizeof (MinBondDist),    0,
    &MaxBondDist,       sizeof (MaxBondDist),    0,
    &ElemNo,            sizeof (ElemNo),         0,
    &ElemDesc,          sizeof (ElemDesc),       0,
    &ResNo,             sizeof (ResNo),          0,
    &HasHydrogen,       sizeof (HasHydrogen),    0,
    &MaskCount,         sizeof (MaskCount),      0,
    &NMRModel,          sizeof (NMRModel),       0,
    &NullBonds,         sizeof (NullBonds),      0,
    &MarkAtoms,         sizeof (MarkAtoms),      0,
    &HBondChainsFlag,   sizeof (HBondChainsFlag),0,

    /* render.h */
    &VoxelsClean,       sizeof (VoxelsClean),    0,
    &DrawBoundBox,      sizeof (DrawBoundBox),   0,
    &DrawAxes,          sizeof (DrawAxes),       0,
    &DrawDoubleBonds,   sizeof (DrawDoubleBonds),0,
    &DrawUnitCell,      sizeof (DrawUnitCell),   0,
    &DrawArea,          sizeof (DrawArea),       0,
    &SliceValue,        sizeof (SliceValue),     0,
    &SlabValue,         sizeof (SlabValue),      0,

    /* repres.h */
    &DotPtr,            sizeof (DotPtr),         0,
    &MonitList,         sizeof (MonitList),      0,
    &LabelList,         sizeof (LabelList),      0,

    /* command.h */
    &SelectCount,       sizeof (SelectCount),    0,
    &CalcBondsFlag,     sizeof (CalcBondsFlag),  0,

    /* transfor.h */
    &LastRX,            sizeof (LastRX),         0,
    &LastRY,            sizeof (LastRY),         0,
    &LastRZ,            sizeof (LastRZ),         0,
    &LastTX,            sizeof (LastTX),         0,
    &LastTY,            sizeof (LastTY),         0,
    &LastTZ,            sizeof (LastTZ),         0,
    &CenX,              sizeof (CenX),           0,
    &CenY,              sizeof (CenY),           0,
    &CenZ,              sizeof (CenZ),           0,
    &RCenx,             sizeof (RCenx),          0,
    &RCeny,             sizeof (RCeny),          0,
    &RCenz,             sizeof (RCenz),          0,
    &OrigCX,            sizeof (OrigCX),         0,
    &OrigCY,            sizeof (OrigCY),         0,
    &OrigCZ,            sizeof (OrigCZ),         0,
    RotX,               sizeof (RotX),           0,
    RotY,               sizeof (RotY),           0,
    RotZ,               sizeof (RotZ),           0,
    MatX,               sizeof (MatX),           0,
    MatY,               sizeof (MatY),           0,
    MatZ,               sizeof (MatZ),           0,
    InvX,               sizeof (InvX),           0,
    InvY,               sizeof (InvY),           0,
    InvZ,               sizeof (InvZ),           0,
    
    &XOffset,           sizeof (XOffset),        0,
    &YOffset,           sizeof (YOffset),        0,
    &ZOffset,           sizeof (ZOffset),        0,
    &SideLen,           sizeof (SideLen),        0,
    &Offset,            sizeof (Offset),         0,
    
    /* transfor.h */
    &UseScreenClip,     sizeof (UseScreenClip),  0,
    &DrawAtoms,         sizeof (DrawAtoms),      0,
    &DrawStars,         sizeof (DrawStars),      0,
    &DrawBonds,         sizeof (DrawBonds),      0,
    &DrawRibbon,        sizeof (DrawRibbon),     0,
    &LocalRadius,       sizeof (LocalRadius),    0,
    &MaxAtomRadius,     sizeof (MaxAtomRadius),  0,
    &MaxBondRadius,     sizeof (MaxBondRadius),  0,
    &ZoneBoth,          sizeof (ZoneBoth),       0,

    /* brotate.h */
    &BRotValue,         sizeof (BRotValue),      0,
    &BLastRot,          sizeof (BLastRot),       0,
    &BAxis,             sizeof (BAxis),          0,
    &BSrcAtom,          sizeof (BSrcAtom),       0,
    &BDstAtom,          sizeof (BDstAtom),       0,
    &BondSelected,      sizeof (BondSelected),   0,
    &RotBondList,       sizeof (RotBondList),    0,
    &BondOrderList,     sizeof (BondOrderList),  0,
    
    NULL, 0, 0
    };

#if defined(_powerc) || !defined(APPLEMAC)
#define my_memcpy memcpy
#else
void my_memcpy( char *t, char *s, int n )
{
    while   (--n >= 0)
    *t++ = *s++;
}
#endif

void GetMOffset( MolSet __far *mol, void *poffset )
{   if( mol==CurMolSet )
        my_memcpy(poffset,&MOffset,VarList[0].size);
    else
        my_memcpy(poffset,mol->data/*+ VarList[0].offset*/,VarList[0].size);
}

void SetMOffset( MolSet __far *mol, void *poffset )
{   if( mol==CurMolSet )
        my_memcpy(&MOffset,poffset,VarList[0].size);
    else
        my_memcpy(mol->data/*+ VarList[0].offset*/,poffset,VarList[0].size);
}

static int GetMoleculeVariable ( MolSet __far *mol, void *field, void *data )
{
    register int i;

    if( mol==CurMolSet )
        StoreMoleculeData(CurMolSet);

    /* Find field */
    for (i = 0; VarList[i].data; i++)
        if( VarList[i].data == field )
        {   my_memcpy (data, mol->data + VarList[i].offset,
                   VarList[i].size);
            return True;
        }
    return False;
}


static int SetMoleculeVariable( MolSet __far *mol, void *field, void *data )
{
    register int i;
    register MolSet __far *mptr;

    /* Find field */
    for( i=0; VarList[i].data; i++ )
        if( VarList[i].data == field ) 
        {   if( !mol ) 
            {   for (mptr = MoleculeList; mptr; mptr=mptr->next)
                    my_memcpy (mptr->data + VarList[i].offset,
                        data, VarList[i].size);
            } else
            {    my_memcpy (mol->data + VarList[i].offset,
                    data, VarList[i].size);
            }
            return True;
        }
    return False;
}

void StoreMoleculeData( MolSet __far *mol )
{    
    register int i;
    register char *data;

    if( !mol )
        return;
    data = mol->data;
    for( i=0; VarList[i].data; i++ )
    {   my_memcpy( data, VarList[i].data, VarList[i].size );
        data += VarList[i].size;
    }
}


void RestoreMoleculeData( MolSet __far *mol )
{    
    register int i;
    register double temp;
    register char *data;
    
    if( !mol )
        return;
    
    /* Horrible hack to keep scaling factor same */
    temp = DialValue[3];

    data = mol->data;  
    for( i=0; VarList[i].data; i++ ) 
    {    my_memcpy( VarList[i].data, data, VarList[i].size );
        data += VarList[i].size;
    }
    
    DialValue[3] = temp;
}


static void DestroyCurMolSet( void )
{      
    register MolSet __far *mptr;
    char __far *cptr;

    if( !MoleculeList )
        return;

    if( !CurMolSet )
        RasMolFatalExit (MsgStrs(RfeNoCurMol));

    mptr = MoleculeList;
    if( mptr==CurMolSet )
    {    MoleculeList = MoleculeList->next;
        mptr = MoleculeList;
    } else
    {    while( mptr->next!=CurMolSet )
            mptr = mptr->next;
        mptr->next = CurMolSet->next;
    }

    /* Clear current MolSet */
    cptr = CurMolSet->data;
    _ffree( cptr );
    cptr = CurMolSet->molfile;
    if( cptr )
        _ffree( cptr );
    _ffree( CurMolSet );

    if( CenterSet==CurMolSet )
        CenterSet =  (void __far*)0;
    CurMolSet = (void __far*)0;    

    SwitchMolecule(mptr);
    if( !CenterSet && mptr )
        CentreTransform(mptr,0,0,0,False);
}


/* Partial Zap */
static void ZapCurMolSet( void )
{   
    register int i;

    /* Save zoom */
    for( i=0; i<3; i++ )
        DialValue[i] = 0.0;
    for( i=4; i<7; i++ )
       DialValue[i] = 0.0;
    LastRX = LastRY = LastRZ = 0.0;
    LastTX = LastTY = LastTZ = 0.0;
    
    DestroyDatabase();
    ResetSymbolTable();
    ResetMolSet();
    ResetBondsSel();
    ResetBondOrder();

    ZoneBoth = True;
    HetaGroups = True;    
    Hydrogens = True;

    ResetUserSetBox( );
    PickCount = 0;
    ReDrawFlag |= RFRefresh;
}


static void ZapRasMol( void )
{   
    register int i;

    for( i=0; i<9; i++ )
        DialValue[i] = 0.0;

    DestroyDatabase();
    ResetSymbolTable();
    ResetTransform();
    ResetRenderer();
    ResetRepres();
    ResetBondsSel();
    ResetBondOrder();

    ZoneBoth = True;
    HetaGroups = True;    
    Hydrogens = True;

#ifndef IBMPC
    FBClear = False;
#endif

    ResetUserSetBox( );
    PickCount = 0;
    ReDrawFlag = RFRefresh;
}


void ZapMolecule( void )
{
    ZapCurMolSet();
    DestroyCurMolSet();
    ReRadius();

    FileModified = True;

    if( Interactive )
       UpdateScrollBars();

    AdviseUpdate(AdvName);
    AdviseUpdate(AdvClass);
    AdviseUpdate(AdvIdent);
}


void ZapDatabase( int full )
{    
    register MolSet __far *mptr;
    char __far *vptr,*fptr;

    if( full )
    {   mptr = MoleculeList;
        while( mptr )
        {   SwitchMolecule(mptr);
            ZapRasMol();
            mptr = mptr->next;
        }
        SwitchMolecule(MoleculeList);
    }

    while( MoleculeList )
    {    mptr = MoleculeList;
        vptr = mptr->data;
        fptr = mptr->molfile;
        MoleculeList = MoleculeList->next;
        _ffree( vptr );
        if( fptr )
            _ffree( fptr );
        _ffree( mptr );
    }

    CurMolSet = (void __far*)0;    
    CenterSet = (void __far*)0;
    MoleculeList = (void __far*)0;    
    *RSMPathName = '\0';
    *SpaceName = '\0';
    FileModified = True;

    ZapRasMol();
    ReRadius();

    BackR = BackG = BackB = 0;
    RefreshState = True;
    ResetColourMap();
    DefineColourMap();
    ClearBuffers();
    ClearImage();
    ReDrawFlag = 0;

    if( Interactive )
       UpdateScrollBars();

    AdviseUpdate(AdvName);
    AdviseUpdate(AdvClass);
    AdviseUpdate(AdvIdent);
}


void CreateNewMolSet( void )
{
    register MolSet __far *mptr, *list;
    char __far *var;

    /* Reset Center */
    CentreTransform(CurMolSet,0,0,0,False);

    /* Save current */
    StoreMoleculeData(CurMolSet);

    /* Zero all allocated memories */
    Database = (void __far*)0;
    SymbolTable = (void __far*)0;
    DotPtr = (DotStruct __far*)0;
    MonitList = (Monitor __far*)0;
    LabelList = (void*)0;
    RotBondList = (BondRot __far *)0;
    BondOrderList = (BondOrder __far *)0;

    /* Clear rasmol */
    if( MoleculeList )
        ZapCurMolSet();
    else
        ZapRasMol();

    /* Create new */
    mptr = (MolSet __far*)_fmalloc(sizeof(MolSet));
    if( !mptr )
        RasMolFatalExit (MsgStrs(RfeNoMem));

    mptr->next = (void __far*)0;
    mptr->title[0] = 0;
    mptr->copynum = 0;
    mptr->format = 0;
    mptr->molfile = (char __far*)0;
    mptr->visible = True;

    var = (char __far*)_fmalloc(VarSize);
    if( !var )
        RasMolFatalExit (MsgStrs(RfeNoMem));
    mptr->data = var;

    if( !MoleculeList )
    {    MoleculeList = mptr;
    } else
    {    list = MoleculeList;
        while( list->next )
            list = list->next;
        list->next = mptr;
    }

    /* Save in case and Update */
    StoreMoleculeData( mptr );
    CurMolSet = mptr;

    /* Set center */
    CentreTransform(CurMolSet,0,0,0,False);
}


void ChangeMolFile( char *newfile )
{   char __far *ptr;
    register int len;

    if( !*newfile ) return;

    ptr = CurMolSet->molfile;
    if( ptr )
        _ffree( ptr );

    ptr = newfile;
    len = 0;
    while( *ptr++ ) len++;
    len+=1;

    ptr = (char __far*)_fmalloc(len);
    if( !ptr )
        RasMolFatalExit (MsgStrs(RfeNoMem));
    CurMolSet->molfile = ptr;

    while( *newfile ) *ptr++ = *newfile++;
    *ptr = '\0';
}


void RecordMolFile( void )
{   register int start,end,n,r,temp;
    register int ch,len;
    FILE *fp;
    char buffer[18];
    char *ptr;

/* This function is not multiplatform yet! */
#if !defined(MSWIN)
    return;
#endif

    if( !CurMolSet )
        return;

    if( !((FileDepth==1) && (ScriptFileFormat == RSMFormat)) ) 
        if( (FileDepth>0) || (ScriptFileFormat == ScriptFormat) )
            return;

    fp = fopen(DataFileName,"rb");

    if( !fp )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(MulDataNF));
        return;
    }

    BeginWait();

    start = end = 0;
    if( ScriptFileFormat == DataFormat )
    {   while( (ch=getc(fp))!=EOF )
            end++;
    } else if( ScriptFileFormat == RSMFormat )
    {   do {
            len = 0;
            temp = end;

            ch = getc(fp);
            end++;

            while( isspace(ch) )
            {   ch =  getc(fp);
                end++;
            }

            while( (ch!='\n') && (ch!='\r') &&  (ch!=EOF) )
            {   if( len<18 )
                   buffer[len++] = ch;
                ch = getc(fp);
                end++;
            }
            buffer[len] = '\0';

            if( ch == '\r' )
            {   ch = getc(fp);
                end++;
                if( ch != '\n' )
                {   ungetc(ch,fp);
                    end--;
                }
            }

            if( (len==18) && (buffer[0]=='#') )
                if( !strncmp(buffer,"#!rasmol -rsm file",18) )
                {   end = temp;
                    break;
                }
        } while( ch!=EOF );

        if( ch==EOF )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(MulDataNF));
            fclose( fp );
            EndWait();
            return;
        }

    } else if( ScriptFileFormat == InlineFormat )
    {   do {
            len = 0;
            temp = start;

            ch = getc(fp);
            start++;

            while( isspace(ch) )
            {   ch = getc(fp);
                start++;
            }

            while( (ch!='\n') && (ch!='\r') &&  (ch!=EOF) )
            {   if( len<6 )
                   buffer[len++] = ToUpper(ch);
                ch = getc(fp);
                start++;
            }
            buffer[len] = '\0';

            if( ch == '\r' )
            {   ch = getc(fp);
                start++;
                if( ch != '\n' )
                {   ungetc(ch,fp);
                    start--;
                }
            }

            if( buffer[0]=='#' )
            {    if( !strncmp(buffer,"EXIT",5) || !strncmp(buffer,"QUIT",4) )
                {   break;
                } else if(!strncmp(buffer,"HEADER",6)||!strncmp(buffer,"DATA_",5))
                {   start = temp;
                    break;
                }
            }
        } while( ch!=EOF );

        if( ch==EOF )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(MulMisEnd));
            WriteString(MsgStrs(MulSeeHelp));
            fclose( fp );
            EndWait();
            return;
        } else
        {   end = start;
            while( (ch=getc(fp))!=EOF )
                end++;
        }
    } else
    {   InvalidateCmndLine();
        WriteString(MsgStrs(MulDataNF));
        fclose( fp );
        EndWait();
        return;
    }

    fseek(fp,start,SEEK_SET);

    /* Remove Start Trailing */    
    do {
        ch = getc(fp);
        start++;
        if( (ch != '\r') && (ch != '\n') && (!isspace(ch)) )
        {   ungetc(ch,fp);
            start--;
            break;
        }
    } while( ch != EOF );
    
    len = end - start;
    if( len<=0 )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(MulDataNF));
        fclose( fp );
        EndWait();
        return;
    }

    ptr = (char __far*)_fmalloc(len+1);
    if( !ptr )
        RasMolFatalExit (MsgStrs(RfeNoMem));
    CurMolSet->molfile = ptr;

    while( len-- )
    {   ch = getc(fp);
        if( ch == '\r' )
        {   while( len )
            {   ch = getc(fp);
                if( ch != '\r' )
                {   ungetc(ch,fp);
                    ch = '\r';
                    break;
                }
                len--;
            }
        }
        *ptr++ = ch;
    }
    *ptr = '\0';

    /* Remove End Trailing */
    while( ptr-- )
    {    if( (*ptr != '\r') && (*ptr != '\n') )
           break;
        else
           *ptr = '\0';
    }

    /* Fix absence of end of line in some files (for editing) */
    n = r = 0;
    ptr = CurMolSet->molfile;
    while( *ptr )
    {   if( *ptr == '\n' ) n++;
        else if( *ptr == '\r' ) r++;
        ptr++;
    }
    if( n==0 && r!=0 )
    {    ptr = CurMolSet->molfile;
        while( *ptr ) if( *ptr++ == '\r' ) *(ptr - 1) = '\n';
    }

    fclose( fp );
    EndWait();

}


/* Set with the first file loaded and after saving */
void SetDirectory( )
{    
    register char *s;

#if defined DirChar

    /* Copy Directory */
    strcpy(RSMPathName, DataFileName);
    s = frindex(RSMPathName, DirChar);
    if( s==NULL )
    {   *RSMPathName = '\0';
    } else
    {   s++;
        *s = '\0';
    }

    /* Copy File Name */
    s = frindex(DataFileName, DirChar);
    if( s!=NULL )
        s++;
    else
        s = DataFileName;

    SpaceName[0] = 0;
    SpaceName[62] = 0;
    SpaceName[63] = 0;
    if( s!=NULL )
        strncpy(SpaceName,s, 62);

    /* Remove Extension up to 4 ch. - May truncate names with '.' */
    s = strrchr(SpaceName, (int)'.');
    if( s )
    {   int len;
        len = strlen(SpaceName);
        len -= s - SpaceName;
        if( len<=5 )
            SpaceName[s - SpaceName] = '\0';
    }
#endif
    return;
}


void SetTitle( void )
{
    register char *s;
    char buffer[64];
    register MolSet __far *mptr;
    register int max,count,result;

    buffer[0] = 0;
    buffer[62] = 0;
    buffer[63] = 0;

    /* Define Title if not existing yet */
    if( !*CurMolSet->title )
    {   
        if( *Info.identcode ) 
        {   strncpy(buffer,Info.identcode,62);
        } else 
        {    if( *Info.filename )
            {    /* Remove directory */
#ifdef DirChar
                s = frindex(Info.filename, DirChar);
                if( s==NULL )
                    s = Info.filename;
                else
                    s++;
#else
                s = Info.filename
#endif
                strncpy(buffer,s, 62);

                /* Remove extension */
                s = strrchr(buffer, (int)'.');
                if( s ) 
                    buffer[s - buffer] = '\0';
            }
        }

        strncpy(CurMolSet->title,buffer,64);
    } else
        strncpy(buffer,CurMolSet->title,62);

    /* Duplicat Titles */
    CurMolSet->copynum = 0;
    mptr = MoleculeList;
    count = max = 0;
    while( mptr )
    {   result = strcmp(mptr->title,buffer);
        if( !result )
        {   count++;
            if( mptr->copynum > max )
                max = mptr->copynum;
        }
        mptr = mptr->next;
    }
    if( max>count )
        count = max + 1;
    CurMolSet->copynum = count;

    SetCanvasTitle( );
}


void SwitchMolecule( MolSet __far *mol )
{
    if( (mol==CurMolSet) || !mol )
        return;
    StoreMoleculeData(CurMolSet);
    CurMolSet = mol;
    RestoreMoleculeData(CurMolSet);
}


void SelectMolecule( MolSet __far *set, char *name, int index )
{
    register MolSet __far *ptr;
    register int i;

    ptr = set;
    i = 0;

    if( !ptr )
    {    if( name )
        {   ptr = MoleculeList;
            if( !index ) index++;
            while( ptr )
            {    if( !strcmp(ptr->title,name) )
                    if( ptr->copynum==index )
                        break;
                ptr = ptr->next;
            }
            if( !ptr )
            {   InvalidateCmndLine();
                WriteString(MsgStrs(MulErrNoNam));
            }
        } else /* index */
        {   ptr = MoleculeList;
            while( ptr )
            {    if( i==index ) break;
                ptr = ptr->next;
                i++;
            }
            if( !ptr )
            {   InvalidateCmndLine();
                WriteString(MsgStrs(MulErrNoInd));
            }
        }
    }

    if( ptr && ptr!=CurMolSet)
    {   MolSet __far *mtemp;
        Long x,y,z;

        mtemp = CurMolSet;
        x = CenX; y = CenY; z = CenZ;

        SwitchMolecule(ptr);
        if( RotMode==RotAll )
        {   if( mtemp==CenterSet )
                CentreTransform(CenterSet,x,y,z,False);
        } else
            CentreTransform(CurMolSet,x,y,z,False);

        SetCanvasTitle( );
        ResetUserSetBox( );
    }

    PickCount = 0;

    if( DisplayMode )
       ReDrawFlag |= RFRefresh | RFColour;
}


static void TranslateCenter( int xo, int yo, int zo, int origin )
{    
    if( !MoleculeList )
        return;

    CenX = xo;
    CenY = yo;
    CenZ = zo;

    RCenx = xo*MatX[0]+yo*MatX[1]+zo*MatX[2];
    RCeny = xo*MatY[0]+yo*MatY[1]+zo*MatY[2];
    RCenz = xo*MatZ[0]+yo*MatZ[1]+zo*MatZ[2];

    if( origin )
    {    /* Zero MOffset */
        DialValue[DialTX] = LastTX-WTransValue[0] + WLastTX - MOffset[0]/Range;
        DialValue[DialTY] = LastTY-WTransValue[1] + WLastTY - MOffset[1]/Range;
        DialValue[DialTZ] = LastTZ-WTransValue[2] + WLastTZ - MOffset[2]/Range;

        /* Send to world origin */
        DialValue[DialTX] += Wo[0]/Range;
        DialValue[DialTY] += Wo[1]/Range;
        DialValue[DialTZ] += Wo[2]/Range;

        /* Translate new center */
        DialValue[DialTX] -= RCenx/Range;
        DialValue[DialTY] -= RCeny/Range;
        DialValue[DialTZ] -= RCenz/Range;

        ReDrawFlag |= RFTrans;
        PrepareMoleculeTranslate();
    } else
        ReDrawFlag |= RFRefresh;
}

int CentreTransform( MolSet __far *ptr, int xo, int yo, int zo, int origin )
{   register MolSet __far *mcur;

    if( !MoleculeList || !CurMolSet )
        return False;

    /* Protect world construction */
    if( origin && (RotMode==RotAll) )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(MulNoTrans)); 
        return False;
    }

    mcur = CurMolSet;

    /* RotMol */
    if( RotMode==RotMol )
    {   if( !ptr )
        {   SwitchMolecule(CenterSet);
            if( CenterSet ) TranslateCenter(0,0,0,False);
            SwitchMolecule(mcur);
            CenterSet = ptr;
        } else if( ptr==CurMolSet )
        {   if( CenterSet!=CurMolSet )
            {   SwitchMolecule(CenterSet);
                if( CenterSet ) TranslateCenter(0,0,0,False);
                SwitchMolecule(mcur);
                TranslateCenter(xo,yo,zo,origin);
                CenterSet = CurMolSet;
            } else
            {   TranslateCenter(xo,yo,zo,origin);
                CenterSet = CurMolSet;
            }
        } else
            return False;
    } else 
    /* RotAll */
    if( ptr==CenterSet )
    {   SwitchMolecule(ptr);
        if( ptr ) TranslateCenter(xo,yo,zo,origin);
        SwitchMolecule(mcur);
    } else
    {   SwitchMolecule(CenterSet);
        if( CenterSet ) TranslateCenter(0,0,0,False);
        SwitchMolecule(ptr);
        CenterSet = ptr;
        if( ptr ) TranslateCenter(xo,yo,zo,origin);
        SwitchMolecule(mcur);
    }
    return True;
}


void ReRadius( void )
{
    register MolSet __far *mptr, *mcur;
    register Card size;
    
    size = 1;

    if( MoleculeList )
    {   mcur = CurMolSet;
        mptr = MoleculeList;
        while( mptr )
        {   SwitchMolecule(mptr);
            if( LocalRadius > size )
                size = LocalRadius;
            mptr = mptr->next;
        }
        SwitchMolecule(mcur);
        SetWorldSize(size<<1);
    } else
    {   WorldRadius = 1;
        WorldSize = 1;
    }
}


int IsWorldActive( void )
{   
    if( !Database || !MoleculeList )
        return False;

    if( !CenterSet )
        return True;

    if( MoleculeList->next )
        return True;

    if( !IsZero(Wo[0]) || !IsZero(Wo[1]) )
        return True;

    if( !IsZero(WTransValue[0])||!IsZero(WTransValue[1])||!IsZero(WTransValue[2]) )
        return True;

    if( !IsZero(WTransValueBack[0])||!IsZero(WTransValueBack[1])||!IsZero(WTransValueBack[2]) )
        return True;

    if( !IsZero(WRotValue[0]) || !IsZero(WRotValue[1]) || !IsZero(WRotValue[2]) )
        return True;

    if(!IsZero(WRotValueBack[0])||!IsZero(WRotValueBack[1])||!IsZero(WRotValueBack[2]))
        return True;

    return False;
}


int GetMolsetIndex( MolSet __far * testset)
{   MolSet *mptr;
    int count;

    if( !MoleculeList ) return 0;

    mptr = MoleculeList;
    count = 1;
    while( mptr )
    {   if( mptr == testset ) break;
        mptr = mptr->next;
        count++;  
    }

    return count;
}


static void GetWorldCoord( MolSet __far *mptr, RAtom __far *aptr,
                   double *x, double *y, double *z )
{   
    register MolSet __far *mcur;
    register int i,j;
    double w[3];
    double A[4][4],B[4][4],R[4][4];

    mcur = CurMolSet;
    SwitchMolecule(mptr);

    if( aptr )
    {   w[0] = (double)(aptr->xorg + aptr->fxorg);
        w[1] = (double)(aptr->yorg + aptr->fyorg);
        w[2] = (double)(aptr->zorg + aptr->fzorg);
    } else
    {   w[0] = 0.0;
        w[1] = 0.0;
        w[2] = 0.0;
    }

    *x = w[0]*MatX[0] + w[1]*MatX[1] + w[2]*MatX[2];
    *y = w[0]*MatY[0] + w[1]*MatY[1] + w[2]*MatY[2];
    *z = w[0]*MatZ[0] + w[1]*MatZ[1] + w[2]*MatZ[2];

    w[0] = (*x + MOffset[0] - Wo[0])/(Scale*250.0);
    w[1] = (*y + MOffset[1] - Wo[1])/(Scale*250.0);
    w[2] = (*z + MOffset[2] - Wo[2])/(Scale*250.0);

    IdentityMatrix(A);
    for( i=0; i<3; i++ )
        A[i][3] = w[i];
    A[3][3] = 0.0;

    ZeroMatrix(R);
    /* Invert Normed Matrix */
    for( i=0; i<3; i++ )
        for( j=0; j<3; j++ )
            R[i][j] = WAxes[j][i];

    MultMatrix(R,A,B);        

    *x = B[0][3];
    *y = B[1][3];
    *z = B[2][3];
#ifdef INVERT
    *y = -*y;
#endif
    *z = -*z;

    SwitchMolecule(mcur);
}


static void GetMolCoord( MolSet __far *mptr, RAtom __far *aptr,
                  double *x, double *y, double *z )
{
    register MolSet __far *mcur;

    mcur = CurMolSet;
    SwitchMolecule(mptr);

    if( aptr )
    {   *x = (double)(aptr->xorg + aptr->fxorg + OrigCX)/250.0
            +(double)(aptr->xtrl)/10000.0;
        *y = (double)(aptr->yorg + aptr->fyorg + OrigCY)/250.0
            +(double)(aptr->ytrl)/10000.0;
        *z = (double)(aptr->zorg + aptr->fzorg + OrigCZ)/250.0
            -(double)(aptr->ztrl)/10000.0;
    } else
    {   *x = *y = *z = 0.0;
    }

#ifdef INVERT
    *y = -*y;
#endif
    *z = -*z;

    SwitchMolecule(mcur);
}


static void GetScreenCoord( MolSet __far *mptr, RAtom __far *aptr,
                  double *x, double *y, double *z )
{
    register MolSet __far *mcur;
    register double dx,dy,dz;

    mcur = CurMolSet;
    SwitchMolecule(mptr);

    if( aptr )
    {   dx = (double)(aptr->xorg + aptr->fxorg)*MatX[0]+
             (double)(aptr->yorg + aptr->fyorg)*MatX[1]+
             (double)(aptr->zorg + aptr->fzorg)*MatX[2];
        dy = (double)(aptr->xorg + aptr->fxorg)*MatY[0]+
             (double)(aptr->yorg + aptr->fyorg)*MatY[1]+
             (double)(aptr->zorg + aptr->fzorg)*MatY[2];
        dz = (double)(aptr->xorg + aptr->fxorg)*MatZ[0]+
             (double)(aptr->yorg + aptr->fyorg)*MatZ[1]+
             (double)(aptr->zorg + aptr->fzorg)*MatZ[2];
    } else
    {   dx = dy = dz = 0.0;
    }

    *x = (MOffset[0] + dx)/(Scale*250.0);
    *y = (MOffset[1] + dy)/(Scale*250.0);
    *z = (MOffset[2] + dz - Wo[2])/(Scale*250.0);
#ifdef INVERT
    *y = -*y;
#endif
    *z = -*z;

    SwitchMolecule(mcur);
}


static void GetAbsCoord( MolSet __far *mptr, RAtom __far *aptr,
                  double *x, double *y, double *z )
{
    register MolSet __far *mcur;
    register double dx,dy,dz;

    mcur = CurMolSet;
    SwitchMolecule(mptr);

    if( aptr )
    {   dx = (double)(aptr->xorg + aptr->fxorg)*MatX[0]+
             (double)(aptr->yorg + aptr->fyorg)*MatX[1]+
             (double)(aptr->zorg + aptr->fzorg)*MatX[2];
        dy = (double)(aptr->xorg + aptr->fxorg)*MatY[0]+
             (double)(aptr->yorg + aptr->fyorg)*MatY[1]+
             (double)(aptr->zorg + aptr->fzorg)*MatY[2];
        dz = (double)(aptr->xorg + aptr->fxorg)*MatZ[0]+
             (double)(aptr->yorg + aptr->fyorg)*MatZ[1]+
             (double)(aptr->zorg + aptr->fzorg)*MatZ[2];
    } else
    {   dx = dy = dz = 0.0;
    }

    *x = (MOffset[0] + dx - Wo[0])/(Scale*250.0);
    *y = (MOffset[1] + dy - Wo[1])/(Scale*250.0);
    *z = (MOffset[2] + dz - Wo[2])/(Scale*250.0);
#ifdef INVERT
    *y = -*y;
#endif
    *z = -*z;

    SwitchMolecule(mcur);
}


void GetCurrentCoord( int sys, MolSet __far *mptr, RAtom __far *aptr, 
                     double *x, double *y, double *z )
{   if( !mptr )
    {   *x = 0.0;
        *y = 0.0;
        *z = 0.0;
        return;
    }

    if( sys == WorldCoor )
        GetWorldCoord(mptr,aptr,x,y,z);
    else if( sys == MolCoor )
        GetMolCoord(mptr,aptr,x,y,z);
    else  if( sys == ScreenCoor )
        GetScreenCoord(mptr,aptr,x,y,z);
    else  if( sys == AbsCoor )
        GetAbsCoord(mptr,aptr,x,y,z);
}


double GetClipCoord( int sys, MolSet __far *mptr, int clip)
{   
    register MolSet __far *mcur;
    register double pos;
    register int dial;

    if( clip )
        dial = DialBClip;
    else
        dial = DialSlab;

    if( !mptr ) 
        return 0.0;

    mcur = CurMolSet;
    SwitchMolecule(mptr);

    if( sys == MolCoor )
    {   pos = 50 + 50*DialValue[dial];
    } else if( sys == WorldCoor )
    {   if( WClip[clip] == -999999. ) pos = 0.0;
        else pos = -(WClip[clip]-Wo[2])/(Scale*250.0);
    } else if( sys == ScreenCoor )
    {   if( WClip[clip] == -999999. ) pos = 0.0;
        else pos = -(WClip[clip]-Wo[2])/(Scale*250.0);
    } else if( sys == AbsCoor )
    {   if( WClip[clip] == -999999. ) pos = 0.0;
        else pos = -(WClip[clip]-Wo[2])/(Scale*250.0);
    } else pos = 0.0;

    SwitchMolecule(mcur);
    return pos;
}

double getWorldRadius( void ) {
    register MolSet __far *mptr;
    register double r,l,m;
    
	r = 0;
    mptr = MoleculeList;
    while( mptr )
    {   SwitchMolecule(mptr);
        l = 0;
        m = MOffset[0]-Wo[0];
        l += m*m;
        m = MOffset[1]-Wo[1];
        l += m*m;
        m = MOffset[2]-Wo[2];
        l += m*m;
        l =  sqrt(l) + Scale*LocalRadius;
        if( l > r )
            r = l;
        mptr = mptr->next;
    }
    return r;
}


void InitialiseMultiple( void )
{    register int i;

    VarSize = 0;
    for( i = 0; VarList[i].data; i++ )
    {   VarList[i].offset = VarSize;
        VarSize += VarList[i].size;
    }

    MoleculeList = (void __far*)0;
    CurMolSet = (void __far*)0;    
    CurRenderSet = (void __far*)0;
    CenterSet = (void __far*)0;
    *RSMPathName = '\0';
    *SpaceName = '\0';
    WorldScriptFormat = DataFormat;
    FileModified = True;
    QMolSet = (void __far*)0;
    QXCoor = 0.0;
    QYCoor = 0.0;
    QZCoor = 0.0;
    SysCoor = WorldCoor;
}



