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


/* Original header for this routine:
 */
/**********************************************************************
  Copyright (c) 1995 UC Regents, ModularCHEM Consortium

  wbrotate.c
  World Rotate/Bond Rotate
  
  Author:      Gary Grossman (garyg@cory.EECS.Berkeley.EDU)
  Last Update: November 14, 1995
 **********************************************************************/

#include "rasmol.h"

#ifdef IBMPC
#include <windows.h>
#endif
#ifdef APPLEMAC
#ifdef __CONDITIONALMACROS__
#include <Printing.h>
#else
#include <PrintTraps.h>
#endif
#include <Types.h>
#endif
#include <stdio.h>
#include <math.h>

#define BROTATE
#include "molecule.h"
#include "abstree.h"
#include "cmndline.h"
#include "transfor.h"
#include "command.h"
#include "render.h"
#include "repres.h"
#include "graphics.h"
#include "multiple.h"
#include "vector.h"
#include "brotate.h"
#include "langsel.h"

/* Macros for commonly used loops */
#define ForEachAtom  for(chain=Database->clist;chain;chain=chain->cnext) \
             for(group=chain->glist;group;group=group->gnext)    \
             for(ptr=group->alist;ptr;ptr=ptr->anext)
#define ForEachBond  for(bptr=Database->blist;bptr;bptr=bptr->bnext) 
#define ForEachHBond for(hptr=Database->hlist;hptr;hptr=hptr->hnext) 
#define ForEachSBond for(sptr=Database->slist;sptr;sptr=sptr->hnext) 
#define ForEachBack  for(chain=Database->clist;chain;chain=chain->cnext) \
             for(bptr=chain->blist;bptr;bptr=bptr->bnext)


static void ResetVisited( void )
{
    /* Construct a graph of the current molecule */
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;
            
    ForEachAtom
        ptr->visited = 0;
}

static void ResetCoord( void )
{
   /* Reset the bond rotation coordinate shifts */
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    
    if( !Database )
        return;
            
    ForEachAtom
    {   ptr->fxorg = ptr->fyorg = ptr->fzorg = 0;
        ptr->x = ptr->y = ptr->z = 0;
    }
}

static void UpdateCoord( void )
{
   /* Reset the bond rotation coordinate shifts */
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    
    if( !Database )
        return;
            
    ForEachAtom
    {   ptr->fxorg += ptr->x;
        ptr->fyorg += ptr->y;
        ptr->fzorg += ptr->z;
        ptr->x = ptr->y = ptr->z = 0;
    }
}


static int ConstructGraph( void )
{
    /* Construct a graph of the current molecule */
    register Chain __far *chain;
    register Group __far *group;
    register Bond __far *bptr;
    register RAtom __far *ptr;
    
    if( !Database )
        return False;
            
    /* Initialize graph */
    ForEachAtom 
    {   ptr->visited = 0;
        ptr->nbonds = 0;
    }
    
    /* Go through each edge (bond), insert edge in our graph */
    ForEachBond 
    {   if (bptr->srcatom->nbonds == MaxBonds) return False;
        if (bptr->dstatom->nbonds == MaxBonds) return False;
        bptr->srcatom->bonds[bptr->srcatom->nbonds++] = bptr->dstatom;
        bptr->dstatom->bonds[bptr->dstatom->nbonds++] = bptr->srcatom;
    }
    
    return True;
}



static int visits;

int BondRotatable( RAtom __far *atom )
{
    int i;
    
    if( atom == NULL )
        return True;
    if( atom->visited )
        return True;
    if( atom == BSrcAtom )
        return False;

    atom->visited = True;
    
    for( i=0; i<atom->nbonds; i++ )
        if( atom == BDstAtom && atom->bonds[i] == BSrcAtom )
            continue;
        else if( !BondRotatable(atom->bonds[i]) )
            return False;

    return True;
}


int CreateBondAxis( Long src, Long dst )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register RAtom __far *sptr;
    register RAtom __far *dptr;
    register int done;
    char buffer[80];
    char value[20];
 
    if( src == dst )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(RotDupSer));
        return False;
    }
 
    done = False;
    sptr = (RAtom __far*)0;
    dptr = (RAtom __far*)0;
 
    for( chain=Database->clist; chain && !done; chain=chain->cnext )
        for( group=chain->glist; group && !done; group=group->gnext )
            for( aptr=group->alist; aptr; aptr=aptr->anext )
            {   if( aptr->serno == src )
                {   sptr = aptr;
                    if( dptr )
                    {   done = True;
                        break;
                    }
                } else if( aptr->serno == dst )
                {   dptr = aptr;
                    if( sptr )
                    {   done = True;
                        break;
                    }
                }
            }
 
    if( !done )
    {   InvalidateCmndLine();
        if( sptr )
        {   sprintf(value," %ld",dst);
        } else if( dptr )
        {   sprintf(value," %ld",src);
        } else sprintf(value,"s %ld and %ld",src,dst);
        FormatMsg(buffer,RotAtSer,value);
        WriteString(buffer);
        return False;
    }
    
    return (SetBondAxis( sptr, dptr ));
}


int SetBondAxis( RAtom __far *src, RAtom __far *dst )
{
    int i;
    register BondRot __far *brptr;

    BSrcAtom = src;
    BDstAtom = dst;
    SubtractAtoms(BDstAtom, BSrcAtom, BAxis);
    NormalizeVector(BAxis);
    
    if( !ConstructGraph() )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(RotGraph));
        return False;
    }
    
    /* Ensure that this is a bond-rotatable part */
    if( !BondRotatable(BDstAtom))
    {   for( i=0; i<BSrcAtom->nbonds; i++ )
        {   if( BSrcAtom->bonds[i] == BDstAtom )
            {   InvalidateCmndLine();
                WriteString(MsgStrs(RotNoRot));
                return False;
            }
        }  
        InvalidateCmndLine();
        WriteString(MsgStrs(RotNoSel));
        return False;
    }
    
    if( FileDepth == -1 )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(RotSelOk));
    }
    brptr = BondSelected;
    BondSelected = (BondRot __far *)_fmalloc(sizeof(BondRot));
    if( !BondSelected )
        RasMolFatalExit(MsgStrs(RfeNoMem));
    if( brptr )
    {   brptr->brnext = BondSelected;
    } else
    {   RotBondList = BondSelected;
    }
    BondSelected->brnext = (BondRot __far *)NULL;
    BondSelected->BSrcAtom = BSrcAtom;
    BondSelected->BDstAtom = BDstAtom;
    BondSelected->BRotValue = 0;
    BLastRot = -999999.;

    if( DisplayMode )
        ReDrawFlag |= RFRefresh;
    return True;
}


void Traverse( RAtom __far *atom, double matrix[4][4] )
{
    int i;
    double x, y, z;
    
    if( atom == NULL )
        return;
    
    if( atom->visited )
        return;
    
    atom->visited = 1;
    
    x = (double)(atom->xorg-BSrcAtom->xorg+atom->fxorg-BSrcAtom->fxorg)
        +(double)((atom->xtrl-BSrcAtom->xtrl)/40.);
    y = (double)(atom->yorg-BSrcAtom->yorg+atom->fyorg-BSrcAtom->fyorg)
        +(double)((atom->ytrl-BSrcAtom->ytrl)/40.);
    z = (double)(atom->zorg-BSrcAtom->zorg+atom->fzorg-BSrcAtom->fzorg)
        +(double)((atom->ytrl-BSrcAtom->ytrl)/40.);
    
    atom->x = (Long)(x*matrix[0][0]+y*matrix[0][1]+z*matrix[0][2]
                  +BSrcAtom->xorg-atom->xorg+BSrcAtom->fxorg-atom->fxorg);
    atom->y = (Long)(x*matrix[1][0]+y*matrix[1][1]+z*matrix[1][2]
                  +BSrcAtom->yorg-atom->yorg+BSrcAtom->fyorg-atom->fyorg);
    atom->z = (Long)(x*matrix[2][0]+y*matrix[2][1]+z*matrix[2][2]
                  +BSrcAtom->zorg-atom->zorg+BSrcAtom->fzorg-atom->fzorg);
    
    for (i=0; i<atom->nbonds; i++)
    Traverse(atom->bonds[i], matrix);
    
    visits++;
}

void BondRotate( void )
{
    double matrix[4][4];
    BondRot __far *brptr;
    
    if (!BondSelected)
        return;

    if (BondSelected->BRotValue == BLastRot)
        return;

    visits = 0;
    ResetCoord();
    
    brptr = RotBondList;
    while (brptr)
    {   BSrcAtom = brptr->BSrcAtom;
        BDstAtom = brptr->BDstAtom;
        SubtractAtoms(BDstAtom, BSrcAtom, BAxis);
        NormalizeVector(BAxis);
        ResetVisited();
        RotateAxisMatrix(matrix, PI*(brptr->BRotValue),
             BAxis[0], BAxis[1], BAxis[2]);
        BSrcAtom->visited = 1;
        Traverse(BDstAtom, matrix);
        UpdateCoord();
        brptr = brptr->brnext;
    }
    BLastRot = BondSelected->BRotValue;
   
}


void ResetBonds( void )
{   
    ResetBondsSel();
    ResetCoord();

    /* Recall original bonds */
    if( BondOrderList)
    {   BondOrder __far *bcptr = BondOrderList;
        while( bcptr )
        {   ChangeBondOrder(bcptr->BSrcAtom->serno,
                            bcptr->BDstAtom->serno,
                            False,bcptr->order);
            bcptr = bcptr->next;
        }
    } 

    ResetBondOrder();

    FileModified = True;
    ReDrawFlag |= RFRotBond;
    PrepareBondRotate();
}


void ResetBondsSel( void )
{
   /* Remove all bonds from the list of selected bonds */
   if( RotBondList)
   {   BondRot __far *brptr = RotBondList;

       while( brptr )
       {   RotBondList = brptr->brnext;
           _ffree(brptr);
           brptr = RotBondList;
       }
       BLastRot = -999999.;
   } 
   RotBondList = (BondRot __far *)NULL;
   BondSelected = (BondRot __far *)NULL;
}


void RecordBondOrder(  RAtom __far *src, RAtom __far *dst, int neworder, int order )
{
    register BondOrder __far *optr;

    for( optr=BondOrderList; optr; optr=optr->next )
    {   if( (src==optr->BDstAtom&&dst==optr->BSrcAtom) ||
            (dst==optr->BDstAtom&&src==optr->BSrcAtom) )
        {   optr->neworder = neworder;
            return;
        }
    }
    
    optr = (BondOrder __far *)_fmalloc(sizeof(BondOrder));
    if( !optr )
        RasMolFatalExit(MsgStrs(RfeNoMem));
    optr->next = BondOrderList;
    optr->BSrcAtom = src;
    optr->BDstAtom = dst;
    /* Keep Record of Original Bonding Order */
    optr->order = order;
    optr->neworder = neworder;
    BondOrderList = optr;
}


void ResetBondOrder( void )
{
   /* Remove all bond order changes */
   if( BondOrderList)
   {   BondOrder __far *bcptr = BondOrderList;

       while( bcptr )
       {   BondOrderList = bcptr->next;
           _ffree(bcptr);
           bcptr = BondOrderList;
       }
   } 
   BondOrderList = (BondOrder __far *)NULL;
}


void InitialiseBRotate( void )
{
    RotBondList = BondSelected = (BondRot __far *)NULL;
    BondOrderList = (BondOrder __far *)NULL; 
}
