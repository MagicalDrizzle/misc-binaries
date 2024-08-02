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


/* repres.c
 */
#include "rasmol.h"

#ifdef IBMPC
#include <windows.h>
#include <malloc.h>
#endif
#ifdef APPLEMAC
#include <Types.h>
#include <Errors.h>
#ifdef __CONDITIONALMACROS__
#include <Printing.h>
#else
#include <PrintTraps.h>
#endif
#endif
#ifndef sun386
#include <stdlib.h>
#endif

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>

#define REPRES
#include "molecule.h"
#include "graphics.h"
#include "pixutils.h"
#include "repres.h"
#include "render.h"
#include "command.h"
#include "cmndline.h"
#include "abstree.h"
#include "transfor.h"
#include "infile.h"
#include "vector.h"
#include "brotate.h"
#include "langsel.h"


#ifdef INVERT
#define InvertY(y) (y)
#else
#define InvertY(y) (-(y))
#endif


#define ForEachAtom  for(chain=Database->clist;chain;chain=chain->cnext) \
                     for(group=chain->glist;group;group=group->gnext)    \
                     for(aptr=group->alist;aptr;aptr=aptr->anext)
#define ForEachBond  for(bptr=Database->blist;bptr;bptr=bptr->bnext)
#define ForEachBack  for(chain=Database->clist;chain;chain=chain->cnext) \
                     for(bptr=chain->blist;bptr;bptr=bptr->bnext)


static Monitor *FreeMonit;
static Label *FreeLabel;

/* Richards - Connolly */
typedef struct { int dx, dy, dz; } DotVector;
typedef struct {
        DotVector __far *probe;
        DotVector __far *dots;
        int count;
    } ElemDotStruct;
 
static ElemDotStruct __far *ElemDots;
static RAtom __far *Exclude, *BExclude, *CExclude;
static double IMat[3][3];                   /* Rotate around AB */

typedef struct _DList {
        struct _DList __far *dnext;       /* Linked list      */
        RAtom __far *atom;                /* Doublet Atom Ptr */
    } DList;

static DList __far *BList = NULL;

typedef struct _TList {
        struct _TList __far *tnext;       /* Linked list      */
        RAtom __far *atom;                /* Triplet Atom Ptr */
    } TList;

static TList __far *CList = NULL;

typedef struct _ProbeDot {
        struct _ProbeDot __far *next;
        double dx;
        double dy;
        int col;
    } ProbeDot;

typedef struct _TetraD {
        struct _TetraD __far *next;
        Long ax;
        Long ay;
        Long az;
        int arad;
        int acol;
        Long bx;
        Long by;
        Long bz;
        int brad;
        int bcol;
        Long ddab;
        Long cx;
        Long cy;
        Long cz;
        int crad;
        int ccol;
        Long px;                         /* probe center              */
        Long py;
        Long pz;
        double prx;                        /* probe center in AB coor  */
        double pry;
        int count;
        double inc;
    } TetraD;

static TetraD __far *Tetrad = NULL;   


/*=======================*/
/*  Function Prototypes  */
/*=======================*/
  
void AddMonitors( RAtom __far*, RAtom __far*, int );
static void AddDot( Long, Long, Long, int );
static void CheckVDWDot( Long, Long, Long, int );
static int TestSolventProbe( Long, Long, Long );


static void FatalRepresError( char *ptr )
{
    char buffer[80];
 
    FormatMsg(buffer,RepErrMem,ptr);
    RasMolFatalExit(buffer);
}
 

/*============================*/
/*  Label Handling Functions  */
/*============================*/

static void ResetLabels( void )
{
    register Label *ptr;
 
    while( LabelList )
    {   ptr = LabelList;
        LabelList = ptr->next;
        ptr->next = FreeLabel;
        free(ptr->label);
        FreeLabel = ptr;
    }
}
 

void DeleteLabel( Label *label )
{
    register Label **ptr;
 
    if( label->refcount == 1 )
    {   ptr = &LabelList;
        while( *ptr != label )
        ptr = &(*ptr)->next;
 
        *ptr = label->next;
        label->next = FreeLabel;
        free(label->label);
        FreeLabel = label;
    } else label->refcount--;
}
 

int DeleteLabels( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register int result;
 
    if( !Database )
        return( True );
 
    result = True;
 
    ForEachAtom
        if( aptr->flag & SelectFlag )
        {   if( aptr->label )
            {   DeleteLabel( (Label*)aptr->label );
                aptr->label = (void*)0;
            }
            result = False;
        }
    return( result );
}


Label *CreateLabel( char *text, int len )
{
    register Label *ptr;
 
    /* Test for existing label */
    for( ptr=LabelList; ptr; ptr=ptr->next )
        if( !strcmp(ptr->label,text) )
            return( ptr );
 
    if( FreeLabel )
    {   ptr = FreeLabel;  FreeLabel = ptr->next;
    } else 
    {    if( !(ptr=(Label*)_fmalloc(sizeof(Label))) )
            FatalRepresError(MsgStrs(RepErrLab));
        RegisterAlloc( ptr );
    }
 
    ptr->label = (char*)malloc(len+1);
    if( !ptr->label ) FatalRepresError(MsgStrs(RepErrLab));
    strcpy(ptr->label,text);
 
    ptr->next = LabelList;
    ptr->refcount = 0;
    LabelList = ptr;
    return( ptr );
}
 
 
void DefineLabels( char *label )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register Label *ptr;
    register char *cptr;
    register int len;
 
 
    if( !Database ) return;
    if( DeleteLabels() )
        return;
 
    len = 0;
    for( cptr=label; *cptr; cptr++ )
        len++;
 
    /* Strip trailing spaces */
    while( len && cptr[-1]==' ' )
    {   cptr--;  len--;
        *cptr = '\0';
    }
 
    if( !len )
        return;
 
    ptr = CreateLabel(label,len);

    ForEachAtom
        if( aptr->flag & SelectFlag )
        {   aptr->label = ptr;
            ptr->refcount++;
        }
}
 
 
void DefaultLabels( int enable )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register Label *label1;
    register Label *label2;
 
    if( !Database )
        return;
 
    label1 = (Label*)0;
    label2 = (Label*)0;
 
    if( MainGroupCount > 1 )
    {   ForEachAtom
            if( IsAlphaCarbon(aptr->refno) || IsSugarPhosphate(aptr->refno) )
            {   if( aptr->flag & SelectFlag )
                {   if( aptr->label )
                    {   DeleteLabel( (Label*)aptr->label );
                        aptr->label = (Label*)0;
                    }

                    if( enable )
                    {   if( Info.chaincount > 1 )
                        {   if( isdigit(chain->ident) )
                            {   if( !label1 )
                                    label1 = CreateLabel("%n%r:%c",7);
                                aptr->label = label1;
                                label1->refcount++;
                            } else
                            {   if( !label2 )
                                    label2 = CreateLabel("%n%r%c",6);
                                aptr->label = label2;
                                label2->refcount++;
                            }
                        } else
                        {   if( !label1 )
                                label1 = CreateLabel("%n%r",4);
                            aptr->label = label1;
                            label1->refcount++;
                        }
                    }
                    ReDrawFlag |= RFRefresh;
                }
                break;
            }
 
    } else /* Small Molecule! */
        ForEachAtom
            if( (aptr->flag&SelectFlag) && (aptr->elemno!=6)
                                        && (aptr->elemno!=1) )
            {   if( aptr->label )
                {   DeleteLabel( (Label*)aptr->label );
                    aptr->label = (Label*)0;
                }

                if( enable )
                {   if( !label1 )
                        label1 = CreateLabel("%e",2);
                    aptr->label = label1;
                    label1->refcount++;
                }
                ReDrawFlag |= RFRefresh;
            }
}


void LabelTerminii( int enable )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register Label *label;
 
    if( !Database )
        return;

    for( chain=Database->clist; chain; chain=chain->cnext )
        if( chain->glist && chain->glist->gnext )
        {   group = chain->glist;
            if( IsProtein(group->refno) )
            {   /* N terminus */
                aptr = FindGroupAtom(group,1);
                if( aptr && (aptr->flag&SelectFlag) )
                {   if( aptr->label )
                    {   DeleteLabel( (Label*)aptr->label );
                        aptr->label = (Label*)0;
                    }
                    if( enable )
                    {   label = CreateLabel("N",1);
                        aptr->label = label;
                        label->refcount++;
                    }
                }

                while( group->gnext )
                    group = group->gnext;

                /* C terminus */
                aptr = FindGroupAtom(group,1);
                if( aptr && (aptr->flag&SelectFlag) )
                {   if( aptr->label )
                    {   DeleteLabel( (Label*)aptr->label );
                        aptr->label = (Label*)0;
                    }
                    if( enable )
                    {   label = CreateLabel("C",1);
                        aptr->label = label;
                        label->refcount++;
                    }
                }

            } else if( IsDNA(group->refno) )
            {   /* 5' terminus */
                aptr = FindGroupAtom(group,7);
                if( aptr && (aptr->flag&SelectFlag) )
                {   if( aptr->label )
                    {   DeleteLabel( (Label*)aptr->label );
                        aptr->label = (Label*)0;
                    }
                    if( enable )
                    {   label = CreateLabel("5'",2);
                        aptr->label = label;
                        label->refcount++;
                    }
                }

                while( group->gnext )
                    group = group->gnext;

                /* 3' terminus */
                aptr = FindGroupAtom(group,7);
                if( aptr && (aptr->flag&SelectFlag) )
                {   if( aptr->label )
                    {   DeleteLabel( (Label*)aptr->label );
                        aptr->label = (Label*)0;
                    }
                    if( enable )
                    {   label = CreateLabel("3'",2);
                        aptr->label = label;
                        label->refcount++;
                    }
                }
            }
        }
}


void DisplayLabels( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register Label *label;
    register int col,c,z;
 
    auto unsigned char buffer[256];
 
 
    if( !Database )
        return;

    if( !UseSlabPlane )
    {   z = (int)(Scale*LocalRadius) + ZOffset;
    } else z = SlabValue - 1;
 
    ForEachAtom
        if( aptr->label )
        {   /* Perform Label Slabbing! */
            if( !ZValid(aptr->z) )
                continue;
 
           /* Perform Label Depth Slabbing! */
           if( aptr->z<0 )
                continue;
  
            label = (Label*)aptr->label;
            FormatLabel(chain,group,aptr,label->label,buffer);
 
            if( !UseLabelCol )
            {   if( !UseDepthCue )
                {   int m;
                    m = (ColourMask>>1);
                    c = (m*(aptr->z+ImageRadius-ZOffset+ShiftS))/ImageSize;
                    if( c<0 ) c = 0; if( c>m ) c = m;
                    c += (LastSpecShade>>1);
                } else
                {   c = (LastSpecShade*(aptr->z+ImageRadius-ZOffset+ShiftS))
                        /ImageSize;
                }
                col = aptr->col + c;
            } else col = LabelCol;
 
            /* (aptr->z+2) + ((aptr->flag & SphereFlag)?aptr->irad:0); */
            DisplayRasString(aptr->x+4,aptr->y,z,buffer,col);
        }
}



/*==============================*/
/*  Monitor Handling Functions  */
/*==============================*/

/* Restric to selected atoms */
void RestrictMonitors( void ) {
    register Monitor *ptr, *temp;

    if( !MonitList )
        return;

    ptr = MonitList;
    MonitList = (Monitor __far*)0;

    while(ptr) {
        temp = ptr;
        ptr = ptr->next;
        if( (temp->src->flag & SelectFlag) && (temp->dst->flag & SelectFlag) ) {
            temp->next = MonitList;
            MonitList = temp;
        } else {
            temp->next = FreeMonit;
            FreeMonit = temp;
            if( temp->col )
                Shade[Colour2Shade(temp->col)].refcount--;
        }        
    }

}
 
void DeleteMonitors( void )
{
    register Monitor *ptr;
 
    while( MonitList )
    {   ptr = MonitList;
        if( ptr->col )
            Shade[Colour2Shade(ptr->col)].refcount--;
 
        MonitList = ptr->next;
        ptr->next = FreeMonit;
        FreeMonit = ptr;
    }
}
 
void DeleteTempMonitors( void )
{
    register Monitor *ptr, *prev;

    while( MonitList && (MonitList->monmode != PickMonit) )
    {   ptr = MonitList;
        if( ptr->col )
            Shade[Colour2Shade(ptr->col)].refcount--;
        MonitList = ptr->next;
        ptr->next = FreeMonit;
        FreeMonit = ptr;
    }

    if( !MonitList ) return;

    prev = MonitList;
    ptr = MonitList->next;
    while( ptr )
    {   if( ptr->monmode != PickMonit )
        {   if( ptr->col )
                Shade[Colour2Shade(ptr->col)].refcount--; 

            prev->next = ptr->next;
            ptr->next = FreeMonit;
            FreeMonit = ptr;
            ptr = prev->next;
            continue;
        }
        prev = prev->next;
        ptr = ptr->next;
    }
}
 
/* Add a monitor w/a given number */
void AddMonitors2( RAtom __far *src, RAtom __far *dst,
      RAtom __far *mid1, RAtom __far *mid2,
     unsigned short dist, unsigned char units, int monmode )
{
    register Monitor **prev;
    register Monitor *ptr;
 
    /* Delete an already existing monitor! */
    for( prev = &MonitList;  *prev; prev = &ptr->next )
    {   ptr = *prev;
        if( ((ptr->src==src) && (ptr->dst==dst)
             &&(ptr->mid1==mid1) && (ptr->mid2==mid2)) ||
            ((ptr->src==dst) && (ptr->dst==src)
             &&(ptr->mid1==mid2) && (ptr->mid2==mid1)) )
        {   if( ptr->col )
                Shade[Colour2Shade(ptr->col)].refcount--;
 
             *prev = ptr->next;
             ptr->next = FreeMonit;
             FreeMonit = ptr;
             return;
         }
    }
 
    /* Create a new monitor! */
    if( FreeMonit )
    {   ptr = FreeMonit;  FreeMonit = ptr->next;
    } else 
    {   if( !(ptr=(Monitor*)malloc(sizeof(Monitor))) )
            FatalRepresError(MsgStrs(RepErrMon));
        RegisterAlloc( ptr );
    }
 
    ptr->dist = dist;
 
    ptr->src = src;
    ptr->dst = dst;
    ptr->mid1 = mid1;
    ptr->mid2 = mid2;
    ptr->monmode = monmode;
    ptr->col = 0;
    ptr->units = units;
 
    ptr->next = MonitList;
    MonitList = ptr;
}


void AddMonitors( RAtom __far *src, RAtom __far *dst, int monmode )
{
    register Long dx, dy, dz;
    register Long dist;
    unsigned short temp;
 
    dx = src->xorg - dst->xorg + src->fxorg - dst->fxorg;
    dy = src->yorg - dst->yorg + src->fyorg - dst->fyorg;
    dz = src->zorg - dst->zorg + src->fzorg - dst->fzorg;

    /* ptr->dist = 100.0*CalcDistance(src,dst) */
    dist = isqrt( dx*dx + dy*dy + dz*dz );
    temp = (unsigned short)((dist<<1)/5);

    AddMonitors2(src, dst, 
      (RAtom __far *)NULL, (RAtom __far *)NULL, temp, 127, monmode);

}


void CreateMonitor( Long src, Long dst )
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
        WriteString(MsgStrs(RepDupAtom));
        return;
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
        {   sprintf(value," %d",dst);
        } else if( dptr )
        {   sprintf(value," %d",src);
        } else sprintf(value,"s %d and %d",src,dst);
        FormatMsg(buffer,RepAtomSer,value);
        WriteString(buffer);
 
    } else AddMonitors( sptr, dptr, PickMonit );
}
 
 
void DisplayMonitors( void )
{
    register RAtom __far *s;
    register RAtom __far *d;
    register RAtom __far *m1;
    register RAtom __far *m2;
    register Monitor *ptr;
    register int x,y,z;
    register int sc,dc,c;
    register int col;
    register Long dx, dy, dz;
    register Long ldist;
 
    register unsigned char *cptr;
    register int dist;
    unsigned char buffer[11];
 
    if( !Database )
        return;
 
    if( !UseSlabPlane )
    {   z = (int)(Scale*LocalRadius) + ZOffset;
    } else z = SlabValue-1;
    buffer[10] = '\0';
    buffer[6] = '.';
 
    for( ptr=MonitList; ptr; ptr=ptr->next )
    {   s = ptr->src;
        d = ptr->dst;
        m1 = ptr->mid1;
        m2 = ptr->mid2;

        buffer[9] = ptr->units;
 
        if( !ptr->col )
        {   sc = s->col;
            dc = d->col;
        } else sc = dc = ptr->col;
 
        ClipDashVector(s->x,s->y,s->z,d->x,d->y,d->z,sc,dc,' ');
 
        if( DrawMonitDistance )
            if( ZValid( (s->z+d->z)/2 ) && (s->z+d->z)/2>0 )
            {   x = (s->x+d->x)/2;
                y = (s->y+d->y)/2;
 
                if( !UseLabelCol )
                {   if( !UseDepthCue )
                    {   int m;
                        m = (ColourMask>>1);
                        c = (m*((s->z+d->z)/2+ImageRadius-ZOffset+ShiftS))
                            /ImageSize;
                        if( c<0 ) c = 0; if( c>m ) c = m;
                        c += (LastSpecShade>>1);
                    } else
                    {   c = (LastSpecShade*((s->z+d->z)/2+ImageRadius-ZOffset+ShiftS))
                            /ImageSize;
                    }
                    col = sc + c;
                } else col = LabelCol;

                if ( BondSelected )
                {   if( (ptr->monmode == PickDist)&&(s->visited ^ d->visited) ) 
                    {    dx = s->xorg - d->xorg + s->fxorg - d->fxorg;
                        dy = s->yorg - d->yorg + s->fyorg - d->fyorg;
                        dz = s->zorg - d->zorg + s->fzorg - d->fzorg;
                        ldist = (int)isqrt( dx*dx + dy*dy + dz*dz );
                        ptr->dist = (int)((ldist<<1)/5);
                    } else if( (ptr->monmode == PickAngle)
                           && (( s->visited ^ d->visited ) ||
                               ( s->visited ^ m1->visited ) ||
                               ( m1->visited ^ d->visited )) )
                    {    ptr->dist = (int)(100*CalcAngle(s,m1,d));
                    } else if ((ptr->monmode == PickTorsn)
                           && (( s->visited ^ d->visited ) ||
                              ( s->visited ^ m1->visited ) ||
                              ( m1->visited ^ m2->visited ) ||
                              ( m2->visited ^ d->visited )) ) 
                    {    ptr->dist = (int)(100*CalcTorsion(s,m1,m2,d));
                    }
                }

                dist = ptr->dist;
                if ( dist < 0 ) dist = -dist;
                buffer[8] = (dist%10)+'0';  dist /= 10;
                buffer[7] = (dist%10)+'0';
                cptr = &buffer[5];

                if( dist > 9 )
                {   do {
                       dist /= 10;
                       *cptr-- = (dist%10)+'0';
                    } while( dist > 9 );
                    cptr++;
                } else *cptr = '0';
                if ( ptr->dist < 0 ) *(--cptr)='-';

                DisplayRasString(x+4,y,z,cptr,col);
            }
    }
}
 
 
/*=========================*/
/*  Dot Surface Functions  */
/*=========================*/

void DeleteDotSurface( void )
{
    register DotStruct __far *ptr;
    register int shade;
    register int i;
 
    while( DotPtr )
    {   for( i=0; i<DotPtr->count; i++ )
        {   shade = Colour2Shade(DotPtr->col[i]);
            Shade[shade].refcount--;
        }
 
        ptr = DotPtr->next;
        _ffree( DotPtr );
        DotPtr = ptr;
    }
}
 
 
static void AddDot( Long x, Long y, Long z, int col )
{
    register DotStruct __far *ptr;
    register int i, shade;
 
    if( !DotPtr || (DotPtr->count==DotMax) )
    {   ptr = (DotStruct __far*)_fmalloc(sizeof(DotStruct));
        if( !ptr ) FatalRepresError(MsgStrs(RepErrDSur));
        ptr->next = DotPtr;
        ptr->count = 0;
        DotPtr = ptr;
    } else ptr = DotPtr;
 
    shade = Colour2Shade(col);
    Shade[shade].refcount++;
 
    i = ptr->count++;
    ptr->col[i] = col;
    ptr->xpos[i] = x;
    ptr->ypos[i] = y;
    ptr->zpos[i] = z;
}
 
 
static void CheckVDWDot(  Long x,  Long y,  Long z, int col )
{
    register Item __far *item;
    register RAtom __far *aptr;
    register int ix,iy,iz;
    register int dx,dy,dz;
    register Long dist;
    register Long rad;
    register int i;
 
    ix = (int)((x+Offset)*IVoxRatio);
    iy = (int)((y+Offset)*IVoxRatio);
    iz = (int)((z+Offset)*IVoxRatio);
 
    i = VOXORDER2*ix + VOXORDER*iy + iz;
    for( item=HashTable[i]; item; item=item->list )
        if( item->data != Exclude )
        {   aptr = item->data;
            if( !ProbeRadius )
            {   rad = ElemVDWRadius(aptr->elemno);
            } else rad = ProbeRadius;
            rad = rad*rad;
 
            /* Optimized Test! */
            dx = (int)(aptr->xorg + aptr->fxorg  - x);
            if( (dist=(Long)dx*dx) < rad )
            {   dy = (int)(aptr->yorg + aptr->fyorg - y);
                if( (dist+=(Long)dy*dy) < rad )
                {   dz = (int)(aptr->zorg + aptr->fzorg - z);
                    if( (dist+=(Long)dz*dz) < rad )
                        return;
                }
            }
        }
    AddDot( x, y, z, col );
}


static void FreeDList( void )
{   register DList __far *list,*temp;

    list = BList;
    while( list )
    {   temp = list;
        list = list->dnext;
        _ffree(temp);
    }
    BList = NULL;
}


static void ResetDList( void )
{   register DList __far *list;

    list = BList;
    while( list )
    {   list->atom = NULL;
        list = list->dnext;
    }
}


static int CheckDList( RAtom __far *atom )
{
    register DList __far *list,*temp;

    list = BList;
    while( list )
    {   if( list->atom == atom )
        {   return True;
        } else if( !list->atom )
        {   list->atom = atom;
            return False;
        }
        list = list->dnext;
    }
    temp = (DList __far*)_fmalloc(sizeof(DList));
    if( !temp ) FatalRepresError(MsgStrs(RepErrConB));
    temp->atom = atom;
    temp->dnext = BList;
    BList = temp;
    return False;
}


static void FreeTList( void )
{   register TList __far *list,*temp;

    list = CList;
    while( list )
    {   temp = list;
        list = list->tnext;
        _ffree(temp);
    }
    CList = NULL;
}


static void ResetTList( void )
{   register TList __far *list;

    list = CList;
    while( list )
    {   list->atom = NULL;
        list = list->tnext;
    }
}


static int CheckTList( RAtom __far *atom )
{
    register TList __far *list,*temp;

    list = CList;
    while( list )
    {   if( list->atom == atom )
        {   return True;
        } else if( !list->atom )
        {   list->atom = atom;
            return False;
        }
        list = list->tnext;
    }
    temp = (TList __far*)_fmalloc(sizeof(TList));
    if( !temp ) FatalRepresError(MsgStrs(RepErrConC));
    temp->atom = atom;
    temp->tnext = CList;
    CList = temp;
    return False;
}


static void CreateTetraD( void )
{   
    Tetrad = (TetraD __far*)_fmalloc(sizeof(TetraD));
    if( !Tetrad ) FatalRepresError(MsgStrs(RepErrConT));
    Tetrad->next = NULL;
    Tetrad->ax = Tetrad->ay = Tetrad->az = 0;
    Tetrad->bx = Tetrad->by = Tetrad->bz = 0;
    Tetrad->cx = Tetrad->cy = Tetrad->cz = 0;
    Tetrad->arad = Tetrad->brad = Tetrad->crad= 0;
    Tetrad->acol = Tetrad->bcol = Tetrad->ccol;
    Tetrad->ddab = 0;
    Tetrad->px = Tetrad->py = Tetrad->pz = 0;
    Tetrad->prx = 0.0; Tetrad->pry = 0.0;   

    Tetrad->count = (int)(((Long)DotDensity*ProbeRadius*ProbeRadius)/((Long)250*250));
    Tetrad->count = (int)sqrt(PI*Tetrad->count);
    Tetrad->inc = 2*PI/(Tetrad->count);
}
 
 
static void InitElemDots( void )
{
    register int i,size;
 
    size = MAXELEMNO*sizeof(ElemDotStruct);
    ElemDots = (ElemDotStruct __far*)_fmalloc(size);
    if( !ElemDots ) FatalRepresError(MsgStrs(RepErrTabl));
 
    for( i=0; i<MAXELEMNO; i++ )
        ElemDots[i].count = 0;
}
 
 
static void ResetElemDots( void )
{
    register int i;
     
    for( i=0; i<MAXELEMNO; i++ )
    {   if( ElemDots[i].count )
            _ffree( ElemDots[i].dots );
        ElemDots[i].count = 0;
    }
}
 
 
static void AddElemDots( int elem, int density )
{
    register DotVector __far *ptr;
    register DotVector __far *probe;
    register double x, y, z, p, q, xy;
    register int equat,vert,horz;
    register int rad,count;
    register int i,j,k;
    register int temp=0;
 
 
    if( ( SolventDots && elem ) || !ProbeRadius  )
    {   rad = ElemVDWRadius(elem);
    } else rad = ProbeRadius;
 
    count = (int)(((Long)density*rad*rad)/((Long)250*250));
    ptr = (DotVector __far*)_fmalloc(count*sizeof(DotVector));
    if( !ptr ) FatalRepresError(MsgStrs(RepErrVect));
 
    if( SolventDots )
    {   probe = (DotVector __far*)_fmalloc(count*sizeof(DotVector));
        if( !probe ) FatalRepresError(MsgStrs(RepErrPrb));
        RegisterAlloc( probe );
        temp = rad + ProbeRadius;
    } else probe = NULL;
 
    equat = (int)sqrt(PI*count);
    if( !(vert=equat>>1) )
        vert = 1;
 
    i = 0;
    for( j=0; (i<count) && (j<vert); j++ )
    {   p = (PI*j)/(double)vert;
        z = cos(p);  xy = sin(p);
        horz = (int)(equat*xy);
        if( !horz ) horz = 1;
 
        for( k=0; (i<count) && (k<horz); k++ )
        {   q = (2.0*PI*k)/(double)horz;
            x = xy*sin(q);
            y = xy*cos(q);
 
            ptr[i].dx = (int)(rad*x);
            ptr[i].dy = (int)(rad*y);
            ptr[i].dz = (int)(rad*z);
            if( probe )
            {   probe[i].dx = (int)(temp*x);
                probe[i].dy = (int)(temp*y);
                probe[i].dz = (int)(temp*z);
            }
            i++;
        }
    }
    ElemDots[elem].probe = probe;
    ElemDots[elem].dots = ptr;
    ElemDots[elem].count = i;
}
 
 
static void FreeElemDots( void )
{
    register int i;
 
    for( i=0; i<MAXELEMNO; i++ )
        if( ElemDots[i].count )
            _ffree( ElemDots[i].dots );
    _ffree( ElemDots );
}


static int TestSolventProbe( Long x, Long y, Long z )
{
    register Item __far *item;
    register RAtom __far *aptr;
    register int lx,ly,lz;
    register int ux,uy,uz;
    register int dx,dy,dz;
    register int ix,iy,iz;
    register Long dist;
    register Long rad;
    register int i;
 
    dist = Offset-ProbeRadius;
    lx = (int)((x+dist)*IVoxRatio);
    if( lx >= VOXORDER ) return( True );
    ly = (int)((y+dist)*IVoxRatio);
    if( ly >= VOXORDER ) return( True );
    lz = (int)((z+dist)*IVoxRatio);
    if( lz >= VOXORDER ) return( True );
 
    dist = Offset+ProbeRadius;
    ux = (int)((x+dist)*IVoxRatio);
    if( ux < 0 ) return( True );
    uy = (int)((y+dist)*IVoxRatio);
    if( uy < 0 ) return( True );
    uz = (int)((z+dist)*IVoxRatio);
    if( uz < 0 ) return( True );
 
    if( lx < 0 ) lx = 0;  if( ux >= VOXORDER ) ux = VOXORDER-1;
    if( ly < 0 ) ly = 0;  if( uy >= VOXORDER ) uy = VOXORDER-1;
    if( lz < 0 ) lz = 0;  if( uz >= VOXORDER ) uz = VOXORDER-1;
 
    for( ix=lx; ix<=ux; ix++ )
       for( iy=ly; iy<=uy; iy++ )
          for( iz=lz; iz<=uz; iz++ )
          {   i = VOXORDER2*ix + VOXORDER*iy + iz;
              for( item=HashTable[i]; item; item=item->list )
                  if( (item->data != Exclude) && (item->data != BExclude) 
                       && (item->data != CExclude))
                  {   
                      aptr = item->data;
                      rad = ElemVDWRadius(aptr->elemno)+ProbeRadius;
                      /* - rad compensates for rounding errors */
                      rad = (rad)*(rad) - rad;

                      /* Optimized Test! */
                      dx = (int)(aptr->xorg + aptr->fxorg - x);
                      if( (dist=(Long)dx*dx) < rad )
                      {   dy = (int)(aptr->yorg + aptr->fyorg - y);
                          if( (dist+=(Long)dy*dy) < rad )
                          {   dz = (int)(aptr->zorg + aptr->fzorg - z);
                              dist+=(Long)dz*dz;

                              if( !BExclude )
                              {   /* Make sure to select surface atoms */
                                  if( (Long)(1.02*dist) < rad )
                                      return False;
                              } else
                                  if( dist < rad)
                                      return False;
                          }
                      }
                  }
          }
    return True;
}



static void DrawProbe( void )
{
    register DotVector __far *ptr;
    register int i,count,col;
    register Long pa,pb,pc;
    register Long tx,ty,tz;
    register double vax,vay,vaz;
    register double vbx,vby,vbz;
    register double vcx,vcy,vcz;
    register double ka,kb,kr,ks,pd;
    register Long mx,my,mz;
    register double sa,sb,sc,sm;
    register double da,db,dc;

    if( !ProbeRadius )
        return;

/* P centre of probe, D probe dots, ABC atom triplet         */
/* M is the projection of D on the ABC plane along PD line   */
/* kr equal |PM|/|PD|, only projections inside ABC are drawn */

    /* ABC Plane */
    pa  = Tetrad->ay*(Tetrad->bz-Tetrad->cz);
    pa += Tetrad->by*(Tetrad->cz-Tetrad->az);
    pa += Tetrad->cy*(Tetrad->az-Tetrad->bz);

    pb  = Tetrad->az*(Tetrad->bx-Tetrad->cx);
    pb += Tetrad->bz*(Tetrad->cx-Tetrad->ax);
    pb += Tetrad->cz*(Tetrad->ax-Tetrad->bx);

    pc  = Tetrad->ax*(Tetrad->by-Tetrad->cy);
    pc += Tetrad->bx*(Tetrad->cy-Tetrad->ay);
    pc += Tetrad->cx*(Tetrad->ay-Tetrad->by);

    pd  = (double)Tetrad->ax*(double)(Tetrad->by*Tetrad->cz-Tetrad->cy*Tetrad->bz);
    pd += (double)Tetrad->bx*(double)(Tetrad->cy*Tetrad->az-Tetrad->ay*Tetrad->cz);
    pd += (double)Tetrad->cx*(double)(Tetrad->ay*Tetrad->bz-Tetrad->by*Tetrad->az);

    kb = pd-(double)Tetrad->px*pa-(double)Tetrad->py*pb-(double)Tetrad->pz*pc;  

    /* cos AC - Vector orthogonal to AB */
    tx = Tetrad->bx-Tetrad->ax;
    ty = Tetrad->by-Tetrad->ay;
    tz = Tetrad->bz-Tetrad->az;
    vax = -(double)pc*ty + (double)pb*tz;
    vay = - (double)pa*tz + (double)pc*tx;
    vaz = (double)pa*ty - (double)pb*tx;
    sa =  vax*(Tetrad->cx-Tetrad->ax);
    sa += vay*(Tetrad->cy-Tetrad->ay);
    sa += vaz*(Tetrad->cz-Tetrad->az);

    /* cos BC - Vector orthogonal to BA */
    tx = Tetrad->cx-Tetrad->bx;
    ty = Tetrad->cy-Tetrad->by;
    tz = Tetrad->cz-Tetrad->bz;
    vbx = -(double)pc*ty + (double)pb*tz;
    vby = - (double)pa*tz + (double)pc*tx;
    vbz = (double)pa*ty - (double)pb*tx;
    sb =  vbx*(Tetrad->ax-Tetrad->bx);
    sb += vby*(Tetrad->ay-Tetrad->by);
    sb += vbz*(Tetrad->az-Tetrad->bz);

    /* cos CA - Vector orthogonal to CB */
    tx = Tetrad->ax-Tetrad->cx;
    ty = Tetrad->ay-Tetrad->cy;
    tz = Tetrad->az-Tetrad->cz;
    vcx = -(double)pc*ty + (double)pb*tz;
    vcy = - (double)pa*tz + (double)pc*tx;
    vcz = (double)pa*ty - (double)pb*tx;
    sc =  vcx*(Tetrad->bx-Tetrad->cx);
    sc += vcy*(Tetrad->by-Tetrad->cy);
    sc += vcz*(Tetrad->bz-Tetrad->cz);


    /* Probe generated for elem = 0 */
    if( !ElemDots[0].count )
        AddElemDots(0,DotDensity);

    ptr = ElemDots[0].dots;
    count = ElemDots[0].count;
    ks = 2.5;
    if( (Tetrad->arad/ProbeRadius+1.0)>ks )
        ks = Tetrad->arad/ProbeRadius+1.0;
    if( (Tetrad->brad/ProbeRadius+1.0)>ks )
        ks = Tetrad->brad/ProbeRadius+1.0;
    if( (Tetrad->crad/ProbeRadius+1.0)>ks )
        ks = Tetrad->crad/ProbeRadius+1.0;

    for( i=0; i<count; i++ )
    {   ka = (double)ptr[i].dx*pa + (double)ptr[i].dy*pb + (double)ptr[i].dz*pc;
        kr = kb/ka;
        if( (kr<=0.0) || (kr>ks) )
            continue;
         
        mx = Tetrad->px + (Long)(kr*ptr[i].dx);
        my = Tetrad->py + (Long)(kr*ptr[i].dy);
        mz = Tetrad->pz + (Long)(kr*ptr[i].dz);

        /* AB */
        sm =  vax*(mx-Tetrad->ax);
        sm += vay*(my-Tetrad->ay);
        sm += vaz*(mz-Tetrad->az);
        if( (sm>0.0) && (sa<0.0) )
            continue;
        if( (sm<0.0) && (sa>0.0) )
            continue;

        /* BC */
        sm =  vbx*(mx-Tetrad->bx);
        sm += vby*(my-Tetrad->by);
        sm += vbz*(mz-Tetrad->bz);
        if( (sm>0) && (sb<0) ) continue;
        if( (sm<0) && (sb>0) ) continue;

        /* CA */
        sm =  vcx*(mx-Tetrad->cx);
        sm += vcy*(my-Tetrad->cy);
        sm += vcz*(mz-Tetrad->cz);
        if( (sm>0.0) && (sc<0.0) ) continue;
        if( (sm<0.0) && (sc>0.0) ) continue;

        da  = (Tetrad->px+ptr[i].dx-Tetrad->ax)*(Tetrad->px+ptr[i].dx-Tetrad->ax);
        da += (Tetrad->py+ptr[i].dy-Tetrad->ay)*(Tetrad->py+ptr[i].dy-Tetrad->ay);
        da += (Tetrad->pz+ptr[i].dz-Tetrad->az)*(Tetrad->pz+ptr[i].dz-Tetrad->az);
        db  = (Tetrad->px+ptr[i].dx-Tetrad->bx)*(Tetrad->px+ptr[i].dx-Tetrad->bx);
        db += (Tetrad->py+ptr[i].dy-Tetrad->by)*(Tetrad->py+ptr[i].dy-Tetrad->by);
        db += (Tetrad->pz+ptr[i].dz-Tetrad->bz)*(Tetrad->pz+ptr[i].dz-Tetrad->bz);
        dc  = (Tetrad->px+ptr[i].dx-Tetrad->cx)*(Tetrad->px+ptr[i].dx-Tetrad->cx);
        dc += (Tetrad->py+ptr[i].dy-Tetrad->cy)*(Tetrad->py+ptr[i].dy-Tetrad->cy);
        dc += (Tetrad->pz+ptr[i].dz-Tetrad->cz)*(Tetrad->pz+ptr[i].dz-Tetrad->cz);

        if( da<db )
            if( da<dc )
                col = Tetrad->acol;
            else
                col = Tetrad->ccol;
        else
            if( db<dc )
                col = Tetrad->bcol;
            else
                col = Tetrad->ccol;

        AddDot(Tetrad->px+ptr[i].dx,
               Tetrad->py+ptr[i].dy,
               Tetrad->pz+ptr[i].dz,col);
    }
}


static void TransformPoint(double *x, double *y, double *z )
{
    register double a,b;
     a = IMat[0][0]**x+IMat[0][1]**y+IMat[0][2]**z;
     b = IMat[1][0]**x+IMat[1][1]**y+IMat[1][2]**z;
    *z = IMat[2][0]**x+IMat[2][1]**y+IMat[2][2]**z;
    *x = a;
    *y = b;
}   


static void PointBack(Long *x, Long *y, Long *z )
{
    register Long a,b;
     a = (Long)(IMat[0][0]**x+IMat[1][0]**y+IMat[2][0]**z);
     b = (Long)(IMat[0][1]**x+IMat[1][1]**y+IMat[2][1]**z);
    *z = (Long)(IMat[0][2]**x+IMat[1][2]**y+IMat[2][2]**z);
    *x = a;
    *y = b;
}   


static void RPointBack(double *x, double *y, double *z )
{
    register double a,b;
     a = IMat[0][0]**x+IMat[1][0]**y+IMat[2][0]**z;
     b = IMat[0][1]**x+IMat[1][1]**y+IMat[2][1]**z;
    *z = IMat[0][2]**x+IMat[1][2]**y+IMat[2][2]**z;
    *x = a;
    *y = b;
}   
 

static int ToreTransform( void )
{
    register double sinb,cosb,sing,cosg;
    register double nor,znor,ratio;
    register Long x,y,z;
    register Long arad,brad;
    register Long dis,ix;
    register int i,j,tcount,col;
    register double tinc;
    double rx,ry,tx,ty,tz;
    Long a,b,c;

    x = Tetrad->bx - Tetrad->ax;
    y = Tetrad->by - Tetrad->ay;
    z = Tetrad->bz - Tetrad->az;
   
    dis = Tetrad->ddab;
    if( dis==0 )
        return False;
    nor = sqrt(dis);
    znor = sqrt(x*x+y*y);
    cosb = znor/nor;
    sinb = z/nor;
    if( (x==0) && (y==0) )
    {   cosg = 1.0;
        sing = 0.0;
    } else
    {   cosg = x/znor;
        sing = y/znor;
    }

    IMat[0][0] = cosg*cosb;  
    IMat[1][0] = -sing;      
    IMat[2][0] = -cosg*sinb; 
    IMat[0][1] = sing*cosb;  
    IMat[1][1] = cosg;       
    IMat[2][1] = -sinb*sing; 
    IMat[0][2] = sinb;       
    IMat[1][2] = 0.0;        
    IMat[2][2] = cosb;       

    arad = Tetrad->arad;
    brad = Tetrad->brad;

    /* Contained atoms */
    if( arad>brad )
    {   if( arad>=(nor+brad) )
            return False;
    } else if( brad>=(nor+arad) )
            return False;

    /* P coordinates */
    ratio = (arad+ProbeRadius)*(arad+ProbeRadius);
    ratio -= (brad+ProbeRadius)*(brad+ProbeRadius);
    ratio  = 0.5 + ratio/(2*dis);
    Tetrad->prx = ratio*nor;
    Tetrad->pry = ratio*ratio*dis;
    Tetrad->pry = (arad+ProbeRadius)*(arad+ProbeRadius) - Tetrad->pry;
    Tetrad->pry  = sqrt(Tetrad->pry);

    /* Generate circle around P */
    for( i=(Tetrad->count/2 + 1); i<Tetrad->count; i++ )
    {   rx = ProbeRadius*cos(Tetrad->inc*(i+0.5));
        ry = ProbeRadius*sin(Tetrad->inc*(i+0.5));
        ix = (Long)(Tetrad->prx - Tetrad->pry*rx/ry);
        if( (ix<0) || (ix>nor) )
            continue;
        if( (Tetrad->prx+rx)<(nor/2) )
            col = Tetrad->acol;
        else
            col = Tetrad->bcol;

        /* Generate Tores */
        tcount = (int)(((Long)DotDensity*((Long)Tetrad->pry+ry)
                *((Long)Tetrad->pry+ry))/((Long)250*250));
        tcount = (int)sqrt(PI*tcount);
        tinc = 2*PI/tcount;

        for( j=0; j<tcount; j++)
        {   register double tcos,tsin;

            tcos = cos(tinc*j);
            tsin = sin(tinc*j);
            tx = Tetrad->prx;
            ty = Tetrad->pry*tcos;
            tz = Tetrad->pry*tsin;
            RPointBack(&tx,&ty,&tz);
            tx += Tetrad->ax;
            ty += Tetrad->ay;
            tz += Tetrad->az;
            if( !TestSolventProbe((Long)tx,(Long)ty,(Long)tz) )
                continue;    
            a = (Long)(Tetrad->prx+rx);
            b = (Long)((Tetrad->pry+ry)*tcos);
            c = (Long)((Tetrad->pry+ry)*tsin);
            PointBack(&a,&b,&c);
            a += Tetrad->ax;
            b += Tetrad->ay;
            c += Tetrad->az;
            AddDot(a,b,c,col);
        }
    }
    return True;
}


static void TestAtomTriplet( void )
{
    double cx,cy,cz;
    register Long crad;
    register double alpha,root,la,lb,pr;
    Long px,py,pz;
    double rx,ry,rz;

    cx = Tetrad->cx - Tetrad->ax;
    cy = Tetrad->cy - Tetrad->ay;
    cz = Tetrad->cz - Tetrad->az;
    crad = Tetrad->crad;
    
    TransformPoint(&cx,&cy,&cz);

    la = cy*cy+cz*cz;
    pr = Tetrad->pry*Tetrad->pry;
    /* Aligned atoms */
    if( la==0.0)
        return;
    alpha = (crad+ProbeRadius)*(crad+ProbeRadius)
            -(cx-Tetrad->prx)*(cx-Tetrad->prx)-pr-la;
    if( cz!=0.0 )
    {   lb = alpha*cy;
        root = 4*la*pr-alpha*alpha;
        /* Probe contacts C inside A or B */
        if( root<0 )
            return;
        root = fabs(cz)*sqrt(root);

        rx = Tetrad->prx;
        ry = (-lb + root )/(2*la);
        rz = (-alpha/2 - cy*ry)/cz;
        RPointBack(&rx,&ry,&rz);
        /* Rare errors due to rounding at this point, but still some! */
        /* Would be better to do everything in double */
        if( rx>0 ) rx += 0.5; else rx -= 0.5;
        if( ry>0 ) ry += 0.5; else ry -= 0.5;
        if( rz>0 ) rz += 0.5; else rz -= 0.5;
        px = (Long)(rx) + Tetrad->ax;
        py = (Long)(ry) + Tetrad->ay;
        pz = (Long)(rz) + Tetrad->az;
        Tetrad->px = px;
        Tetrad->py = py;
        Tetrad->pz = pz;
        if( TestSolventProbe(px,py,pz) )
            DrawProbe();

        if( root==0.0 )
            return;

        rx = Tetrad->prx;
        ry = (-lb - root )/(2*la);
        rz = (-alpha/2 - cy*ry)/cz;
        RPointBack(&rx,&ry,&rz);
        if( rx>0 ) rx += 0.5; else rx -= 0.5;
        if( ry>0 ) ry += 0.5; else ry -= 0.5;
        if( rz>0 ) rz += 0.5; else rz -= 0.5;
        px = (Long)(rx) + Tetrad->ax;
        py = (Long)(ry) + Tetrad->ay;
        pz = (Long)(rz) + Tetrad->az;
        Tetrad->px = px;
        Tetrad->py = py;
        Tetrad->pz = pz;
        if( TestSolventProbe(px,py,pz) )
            DrawProbe();
    } else
    {   ry = -alpha/(2*cy);
        root = pr-ry*ry;
        if( root<0 )
            return;
        root = sqrt(root);

        rz = root;
        rx = Tetrad->prx;
        RPointBack(&rx,&ry,&rz);
        if( rx>0 ) rx += 0.5; else rx -= 0.5;
        if( ry>0 ) ry += 0.5; else ry -= 0.5;
        if( rz>0 ) rz += 0.5; else rz -= 0.5;
        px = (Long)(rx) + Tetrad->ax;
        py = (Long)(ry) + Tetrad->ay;
        pz = (Long)(rz) + Tetrad->az;
        Tetrad->px = px;
        Tetrad->py = py;
        Tetrad->pz = pz;
        if( TestSolventProbe(px,py,pz) )
            DrawProbe();

        if( root==0.0 )
            return;

        ry = -alpha/(2*cy);
        rz = -root;
        rx = Tetrad->prx;
        RPointBack(&rx,&ry,&rz);
        if( rx>0 ) rx += 0.5; else rx -= 0.5;
        if( ry>0 ) ry += 0.5; else ry -= 0.5;
        if( rz>0 ) rz += 0.5; else rz -= 0.5;
        px = (Long)(rx) + Tetrad->ax;
        py = (Long)(ry) + Tetrad->ay;
        pz = (Long)(rz) + Tetrad->az;
        Tetrad->px = px;
        Tetrad->py = py;
        Tetrad->pz = pz;
        if( TestSolventProbe(px,py,pz) )
            DrawProbe();
    }
}


static void IdentifyTripletAtoms( void )
{
    register Item __far *item;
    register RAtom __far *cptr;
    register Long x,y,z;
    register int lx,ly,lz;
    register int ux,uy,uz;
    register int dx,dy,dz;
    register int ix,iy,iz;
    register Long dist;
    register Long rad,arad,brad,crad;
    register int i;
    register int radius;
 
    x = Tetrad->ax;
    y = Tetrad->ay;
    z = Tetrad->az;
    arad = Tetrad->arad;
    brad = Tetrad->brad;
    radius = arad+2*ProbeRadius;

    dist = Offset-radius;
    lx = (int)((x+dist)*IVoxRatio);
    if( lx >= VOXORDER ) return;
    ly = (int)((y+dist)*IVoxRatio);
    if( ly >= VOXORDER ) return;
    lz = (int)((z+dist)*IVoxRatio);
    if( lz >= VOXORDER ) return;
 
    dist = Offset+radius;
    ux = (int)((x+dist)*IVoxRatio);
    if( ux < 0 ) return;
    uy = (int)((y+dist)*IVoxRatio);
    if( uy < 0 ) return;
    uz = (int)((z+dist)*IVoxRatio);
    if( uz < 0 ) return;
 
    if( lx < 0 ) lx = 0;  if( ux >= VOXORDER ) ux = VOXORDER-1;
    if( ly < 0 ) ly = 0;  if( uy >= VOXORDER ) uy = VOXORDER-1;
    if( lz < 0 ) lz = 0;  if( uz >= VOXORDER ) uz = VOXORDER-1;
 
   for( ix=lx; ix<=ux; ix++ )
       for( iy=ly; iy<=uy; iy++ )
          for( iz=lz; iz<=uz; iz++ )
          {   i = VOXORDER2*ix + VOXORDER*iy + iz;
              for( item=HashTable[i]; item; item=item->list )
                  if( (item->data != Exclude) && 
                      (item->data != BExclude) &&
                      (item->data->visited > BExclude->visited) )
                  {   cptr = item->data;
                      crad = ElemVDWRadius(cptr->elemno);
                      rad = arad + crad + 2*ProbeRadius;
                      rad = rad*rad;
                      x = cptr->xorg + cptr->fxorg;
                      y = cptr->yorg + cptr->fyorg;
                      z = cptr->zorg + cptr->fzorg;
 
                      /* Optimized Test! */
                      dx = (int)(x - Tetrad->ax);
                      if( (dist=(Long)dx*dx) < rad )
                      {   dy = (int)(y - Tetrad->ay);
                          if( (dist+=(Long)dy*dy) < rad )
                          {   dz = (int)(z- Tetrad->az);
                              if( (dist+=(Long)dz*dz) < rad )
                              {   
                                  /* Test second pair */
                                  rad = brad + crad + 2*ProbeRadius;;
                                  rad = rad*rad;
                                  dx = (int)(x - Tetrad->bx);
                                  if( (dist=(Long)dx*dx) < rad )
                                  {   dy = (int)(y - Tetrad->by);
                                      if( (dist+=(Long)dy*dy) < rad )
                                      {   dz = (int)(z - Tetrad->bz);
                                          if( (dist+=(Long)dz*dz) < rad )
                                          {   if( !CheckTList(cptr) ) 
                                              {   CExclude = cptr;  
                                                  Tetrad->cx = x;
                                                  Tetrad->cy = y;
                                                  Tetrad->cz = z;
                                                  Tetrad->crad = crad;
                                                  Tetrad->ccol = cptr->col;
                                                  TestAtomTriplet();
                                              }
                                          }
                                      }
                                  }
                              }
                          }
                      }
                  }
          }
}


static void IdentifyCloseAtoms( RAtom __far *aptr )
{
    register Item __far *item;
    register RAtom __far *bptr;
    register Long x,y,z;
    register int lx,ly,lz;
    register int ux,uy,uz;
    register int dx,dy,dz;
    register int ix,iy,iz;
    register Long dist;
    register Long rad,arad,brad;
    register int i;
    register int radius;

    Exclude = aptr;
    BExclude = NULL;
    CExclude = NULL;
    x = aptr->xorg + aptr->fxorg;
    y = aptr->yorg + aptr->fyorg;
    z = aptr->zorg + aptr->fzorg;
    arad = ElemVDWRadius(aptr->elemno);
    Tetrad->ax = x;
    Tetrad->ay = y;
    Tetrad->az = z;
    Tetrad->arad = arad;
    Tetrad->acol = aptr->col;
    radius = arad + 2*ProbeRadius;

    dist = Offset-radius;
    lx = (int)((x+dist)*IVoxRatio);
    if( lx >= VOXORDER ) return;
    ly = (int)((y+dist)*IVoxRatio);
    if( ly >= VOXORDER ) return;
    lz = (int)((z+dist)*IVoxRatio);
    if( lz >= VOXORDER ) return;
 
    dist = Offset+radius;
    ux = (int)((x+dist)*IVoxRatio);
    if( ux < 0 ) return;
    uy = (int)((y+dist)*IVoxRatio);
    if( uy < 0 ) return;
    uz = (int)((z+dist)*IVoxRatio);
    if( uz < 0 ) return;
 
    if( lx < 0 ) lx = 0;  if( ux >= VOXORDER ) ux = VOXORDER-1;
    if( ly < 0 ) ly = 0;  if( uy >= VOXORDER ) uy = VOXORDER-1;
    if( lz < 0 ) lz = 0;  if( uz >= VOXORDER ) uz = VOXORDER-1;
 
    for( ix=lx; ix<=ux; ix++ )
        for( iy=ly; iy<=uy; iy++ )
            for( iz=lz; iz<=uz; iz++ )
            {   i = VOXORDER2*ix + VOXORDER*iy + iz;
                for( item=HashTable[i]; item; item=item->list )
                    if( (item->data != Exclude) && 
                        (item->data->visited > aptr->visited) )
                    {   bptr = item->data;
                        brad = ElemVDWRadius(bptr->elemno);
                        rad = arad + brad + 2*ProbeRadius;;
                        rad = rad*rad;
 
                        /* Optimized Test! */
                        dx = (int)(bptr->xorg + bptr->fxorg - x);
                        if( (dist=(Long)dx*dx) < rad )
                        {   dy = (int)(bptr->yorg + bptr->fyorg - y);
                            if( (dist+=(Long)dy*dy) < rad )
                            {   dz = (int)(bptr->zorg + bptr->fzorg - z);
                                if( (dist+=(Long)dz*dz) < rad )
                                {   if( !CheckDList(bptr) )
                                    {   BExclude = bptr;
                                        Tetrad->bx = bptr->xorg + bptr->fxorg;
                                        Tetrad->by = bptr->yorg + bptr->fyorg;
                                        Tetrad->bz = bptr->zorg + bptr->fzorg;
                                        Tetrad->brad = brad;
                                        Tetrad->bcol = bptr->col;
                                        Tetrad->ddab = dist;
                                        if( ToreTransform() )
                                            IdentifyTripletAtoms();
                                        CExclude = NULL;
                                        ResetTList();
                                    }
                                }
                            }
                        }
                    }
            }
}
 

/*=================================*/
/*  Richards & Connolly Surface    */
/*  Roger Sayle (C) 1998           */
/*  Philippe Valadon (C) 2001      */
/*=================================*/

static void CalculateConnollySurface( int density )
{
    register DotVector __far *probe;
    register DotVector __far *ptr;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register int i,count;
    register int elem;
    register short visit;

    BeginWait();

    /* Reset */
    ForEachAtom
        aptr->visited = False;
    visit = 0;

    /* Get Surface Atoms */           
    ForEachAtom
        if( aptr->surf&DotFlag )
        {   elem = aptr->elemno;
            /* Screen at maximum density */
            if( !ElemDots[elem].count )
                AddElemDots(elem,2000);
 
            Exclude = aptr;
            BExclude = NULL;
            CExclude = NULL;
            ptr = ElemDots[elem].dots;
            probe = ElemDots[elem].probe;
            count = ElemDots[elem].count;
            for( i=0; i<count; i++ )
                if( TestSolventProbe( aptr->xorg + aptr->fxorg + probe[i].dx,
                                      aptr->yorg + aptr->fyorg + probe[i].dy,
                                      aptr->zorg + aptr->fzorg + probe[i].dz ) )
                {   /* Ordered visit */
                    visit++;
                    aptr->visited = visit;
                    break;
                }            
        }
    ResetElemDots();


    /* Get Direct Dots */
    ForEachAtom
        if( aptr->visited )
        {   elem = aptr->elemno;
            if( !ElemDots[elem].count )
                AddElemDots(elem,density);
 
            Exclude = aptr;
            BExclude = NULL;
            CExclude = NULL;
            ptr = ElemDots[elem].dots;
            probe = ElemDots[elem].probe;
            count = ElemDots[elem].count;
            if( SolventDots )
            {   for( i=0; i<count; i++ )
                    if( TestSolventProbe( aptr->xorg + aptr->fxorg + probe[i].dx,
                                          aptr->yorg + aptr->fyorg + probe[i].dy,
                                          aptr->zorg + aptr->fzorg + probe[i].dz ) )
                        AddDot( aptr->xorg + aptr->fxorg + ptr[i].dx,
                                aptr->yorg + aptr->fyorg + ptr[i].dy,
                                aptr->zorg + aptr->fzorg + ptr[i].dz,
                                aptr->col );
            }
        }


    /* Get Indirect Dots */
    CreateTetraD();
    ForEachAtom
    {   ResetDList();
        ResetTList();
        if( aptr->visited )
            IdentifyCloseAtoms(aptr);
    }
    FreeDList();
    FreeTList();
    if( Tetrad )
        _ffree(Tetrad);

    FreeElemDots();
    EndWait();
}
 
void CalculateDotSurface( int density )
{
    register DotVector __far *probe;
    register DotVector __far *ptr;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register int i,count;
    register int elem;

    DotDensity = density;
 
    if( !Database )
        return;
 
    DeleteDotSurface();
    ResetVoxelData();
 
    InitElemDots();
    CreateVoxelData( AllAtomFlag );

    VoxelsClean = False;

    if( SolventDots )
    {   CalculateConnollySurface(density);
        return;
    }
 
    ForEachAtom
        if( aptr->surf & DotFlag )
        {   elem = aptr->elemno;
            if( !ElemDots[elem].count )
                AddElemDots(elem,density);
 
            Exclude = aptr;
            ptr = ElemDots[elem].dots;
            probe = ElemDots[elem].probe;
            count = ElemDots[elem].count;
            for( i=0; i<count; i++ )
                CheckVDWDot( aptr->xorg +  aptr->fxorg + ptr[i].dx,
                             aptr->yorg + aptr->fyorg + ptr[i].dy,
                             aptr->zorg + aptr->fzorg + ptr[i].dz,
                             aptr->col);
        }
 
    FreeElemDots();
}
 

static Long ReadValue( char *ptr, int len )
{
    register Long result;
    register char ch;
    register int neg;

    result = 0;
    neg = False;
    while( len-- )
    {   ch = *ptr++;
        if( isdigit(ch) )
        {   result = (10*result)+(ch-'0');
        } else if( ch=='-' )
           neg = True;
    }
    return( neg? -result : result );
}


void LoadDotsFile( FILE *fp, int info )
{
    register Long x,y,z;
    register Long count;
    register int shade;
    register int col;
    char buffer[256];

    DisableSurface(DotFlag);
    shade = DefineShade(255,255,255);
    col = Shade2Colour(shade);

    count = 0;
    while( FetchRecord(fp,buffer) )
    {   x = ReadValue(buffer+13,8);
        y = ReadValue(buffer+22,8);
        z = ReadValue(buffer+31,8);

        x =  x/4 - OrigCX;
        y =  y/4 - OrigCY;
        z = -z/4 - OrigCZ;
        AddDot(x,y,z,col);
        count++;
    }
    fclose(fp);

    if( count )
        ReDrawFlag |= RFRefresh;

    if( info )
    {   InvalidateCmndLine();

        if( count > 1 )
        {   sprintf(buffer,"%ld",count);
            WriteString(buffer);
            WriteString(MsgStrs(RepAllDot));
        } else if( count == 1 )
        {      WriteString(MsgStrs(RepOneDot));
        } else WriteString(MsgStrs(RepNoDot));
    }
}


void DisplaySurface( void )
{
    register DotStruct __far *ptr;
    register int xi,yi,zi;
    register double x,y,z;
    register int i,j,k;
 
    if( DotSize==1 )
    {    for( ptr=DotPtr; ptr; ptr=ptr->next )
            for( i=0; i<ptr->count; i++ )
            {   x = ptr->xpos[i];
                y = ptr->ypos[i];
                z = ptr->zpos[i];
    
                xi = (int)(x*MatX[0]+y*MatX[1]+z*MatX[2])+XOffset;
                if( XValid(xi) )
                {   yi = (int)(x*MatY[0]+y*MatY[1]+z*MatY[2])+YOffset;
                    if( YValid(yi) )
                    {   zi = (int)(x*MatZ[0]+y*MatZ[1]+z*MatZ[2])+ZOffset;
                        if( ZValid(zi) )
                        {   if( UseDotCol==DotColAttrib )
                                PlotDeepPoint(xi,yi,zi,DotCol);
                            else
                                PlotDeepPoint(xi,yi,zi,ptr->col[i]);
                        }
                    }
                }
            }
    } else
    {    for( ptr=DotPtr; ptr; ptr=ptr->next )
            for( i=0; i<ptr->count; i++ )
            {   x = ptr->xpos[i];
                y = ptr->ypos[i];
                z = ptr->zpos[i];
    
                xi = (int)(x*MatX[0]+y*MatX[1]+z*MatX[2])+XOffset;
                if( XValid(xi) && XValid(xi+DotSize) )
                {   yi = (int)(x*MatY[0]+y*MatY[1]+z*MatY[2])+YOffset;
                    if( YValid(yi) && YValid(yi+DotSize) )
                    {   zi = (int)(x*MatZ[0]+y*MatZ[1]+z*MatZ[2])+ZOffset;
                        if( ZValid(zi) )
                            for( j=0; j<DotSize; j++ )
                                for( k=0; k<DotSize; k++ )
                                    if( UseDotCol==DotColAttrib )
                                        PlotDeepPoint(xi+j,yi+k,zi,DotCol);
                                    else
                                        PlotDeepPoint(xi+j,yi+k,zi,ptr->col[i]);
                    }
                }
            }
    }
}


/*==============================*/
/*  Ribbon & Cartoon Functions  */
/*==============================*/
 
static void CalculateVInten( Knot *ptr )
{
    register double inten;
 
    if( !ptr->vsize )
        ptr->vsize = sqrt( ptr->vnx*ptr->vnx +
                           ptr->vny*ptr->vny +
                           ptr->vnz*ptr->vnz ) + 1;
 
#ifdef ORIGINAL
    inten = LightDot(ptr->vnx,ptr->vny,ptr->vnz);
    inten /= ptr->vsize*LightLength;
#else
    inten = (double)ptr->vnz/ptr->vsize;
#endif
 
    if( ptr->vnz < 0 ) inten = -inten;

    if( inten > 0.0 )
    {   ptr->vinten = (char)(ColourMask*inten);
    } else ptr->vinten = 0;
}
 

static void CalculateHInten( Knot *ptr )
{
    register double inten;
 
    /* The intensity of the sides of a protein cartoon
     * may be calculated using ptr->cx,cy,cz and this
     * should save interpolating ptr->hnx,hny,hnz!
     */
 
    if( !ptr->hsize )
        ptr->hsize = sqrt( ptr->hnx*ptr->hnx +
                           ptr->hny*ptr->hny +
                           ptr->hnz*ptr->hnz ) + 1;

#ifdef ORIGINAL
    inten = LightDot(ptr->hnx,ptr->hny,ptr->hnz);
    inten /= ptr->hsize*LightLength;
#else
    inten = ptr->hnz / ptr->hsize;
#endif

    if( ptr->hnz < 0 ) inten = -inten;

    if( inten > 0.0 )
    {   ptr->hinten = (char)(ColourMask*inten);
    } else ptr->hinten = 0;
}


/* Improve slightly the rendering of helix */
void AlignHelix( Knot *ptr )
{    double cost, sint, _cost;
    double x,y,z,a,b,c,ab,bc,ac;
    double size;

    /*theta = -0.2;*/
    cost = 0.98006657784124;
    sint = -0.19866933079506;
    _cost = 1 - cost;

    x = ptr->vnx;
    y = ptr->vny;
    z = ptr->vnz;

    size = sqrt(ptr->tx*ptr->tx + 
                 ptr->ty*ptr->ty +
                 ptr->tz*ptr->tz);
    size++;

    a = ptr->tx/size;
    b = ptr->ty/size;
    c = ptr->tz/size;

    ab = a*b;
    bc = b*c;
    ac = a*c;

    ptr->vnx =(x*a*a+y*ab+z*ac)*_cost+
              x*cost-y*c*sint+z*b*sint;
    ptr->vny =(x*ab+y*b*b+z*bc)*_cost+
              x*c*sint+y*cost-z*a*sint;
    ptr->vnz =(x*ac+y*bc+z*c*c)*_cost-
              x*b*sint+y*a*sint+z*cost;
}


void AlignNucleoHelix( Knot *ptr )
{   double cost, sint, _cost;
    double x,y,z,a,b,c,ab,bc,ac;
    double size;

    /*theta = 0.8;*/
    cost = 0.69670670934717;
    sint = 0.71735609089952;
    _cost = 1 - cost;

    x = ptr->vnx;
    y = ptr->vny;
    z = ptr->vnz;

    size = sqrt(ptr->tx*ptr->tx + 
                ptr->ty*ptr->ty +
                ptr->tz*ptr->tz);
    size++;

    a = ptr->tx/size;
    b = ptr->ty/size;
    c = ptr->tz/size;

    ab = a*b;
    bc = b*c;
    ac = a*c;

    ptr->vnx =(x*a*a+y*ab+z*ac)*_cost+
              x*cost-y*c*sint+z*b*sint;
    ptr->vny =(x*ab+y*b*b+z*bc)*_cost+
              x*c*sint+y*cost-z*a*sint;
    ptr->vnz =(x*ac+y*bc+z*c*c)*_cost-
              x*b*sint+y*a*sint+z*cost;
}


void DisplayRibbon( Chain  __far *chain )
{
    register Group __far *group;
    register RAtom __far *captr;
    register RAtom __far *o1ptr;
    register RAtom __far *o2ptr;
    register RAtom __far *next;
 
    register double prev,wide;
    register int col1,col2;
    register double bx,by,bz;
    register double dx,dy,dz;
    register int arrow;
    register double size;
    register int backside, frontside;
    register double lastwx, lastwy, lastwz;
 
    static Knot mid1, mid2, mid3;
    static Knot knot1, knot2;


    prev = False;
    backside = frontside = 0;
    lastwx = lastwy = lastwz = 0.0;
    group = chain->glist;
    if( IsProtein(group->refno) )
    {   captr = FindGroupAtom(group,1);
    } else captr = FindGroupAtom(group,7);
 
    while( group->gnext )
    {   if( IsProtein(group->gnext->refno) )
        {   next = FindGroupAtom(group->gnext,1);
            o1ptr = FindGroupAtom(group,3);
        } else /* Nucleic Acid */
        {   next = FindGroupAtom(group->gnext,7);
            o1ptr = FindGroupAtom(group->gnext,10);
        }
 
        /* When not to have a control point! */
        if( !next || !captr || !o1ptr || (next->flag&BreakFlag) ||
            !((group->flag|group->gnext->flag)&DrawKnotFlag) )
        {   group = group->gnext;
            captr = next;
            prev = False;
            backside |= 0x0010;
            continue;
        }
 
        knot2.tx = next->x - captr->x;
        knot2.ty = next->y - captr->y;
        knot2.tz = next->z - captr->z;
 
        if( IsProtein(group->refno) )
        {   bx = o1ptr->x - captr->x;
            by = o1ptr->y - captr->y;
            bz = o1ptr->z - captr->z;
 
        } else /* Nucleic Acid */
        {   o2ptr = FindGroupAtom(group,8);
            if( o2ptr && !FindGroupAtom(group,17) )
            {   /* Deoxyribonucleic Acid */
                bx = (o1ptr->x + o2ptr->x)/2 - captr->x;
                by = (o1ptr->y + o2ptr->y)/2 - captr->y;
                bz = (o1ptr->z + o2ptr->z)/2 - captr->z;
            } else /* Ribonucleic Acid */
            {   bx = o1ptr->x - captr->x;
                by = o1ptr->y - captr->y;
                bz = o1ptr->z - captr->z;
            }
        }
 
        knot2.px = (captr->x + next->x)/2;
        knot2.py = (captr->y + next->y)/2;
        knot2.pz = (captr->z + next->z)/2;
 
        /* c := a x b */
        knot2.vnx = knot2.ty*bz - knot2.tz*by;
        knot2.vny = knot2.tz*bx - knot2.tx*bz;
        knot2.vnz = knot2.tx*by - knot2.ty*bx;


        if( group->flag & CartoonFlag )
        {    if( !(group->gnext->flag & CartoonFlag) )
                frontside |= 0x0001;
        } else
            backside |= 0x0010;

        if( IsNucleo(group->refno)&IsNucleo(group->gnext->refno) )
            AlignNucleoHelix( &knot2 );

        if( (group->struc&group->gnext->struc) & HelixFlag )
        {   /* Compensate for narrowing of helices! */
            size = sqrt(knot2.vnx*knot2.vnx +
                         knot2.vny*knot2.vny +
                         knot2.vnz*knot2.vnz);
            knot2.vsize = size;
 
            if( size != 0.0 )
            {   /* 1.00 Angstrom Displacement */
                wide = 250*Scale;
#ifdef INVERT
                knot2.px += (wide*knot2.vnx)/size;
                knot2.py += (wide*knot2.vny)/size;
                knot2.pz += (wide*knot2.vnz)/size;
#else
                knot2.px -= (wide*knot2.vnx)/size;
                knot2.py -= (wide*knot2.vny)/size;
                knot2.pz -= (wide*knot2.vnz)/size;
#endif
            }
            
            AlignHelix( &knot2 );
        
        } else knot2.vsize = 0;
 
        if( !(group->flag&group->gnext->flag&TraceFlag) )
        {   /* d := c x a */
            dx = (knot2.vny*knot2.tz - knot2.vnz*knot2.ty)/96;
            dy = (knot2.vnz*knot2.tx - knot2.vnx*knot2.tz)/96;
            dz = (knot2.vnx*knot2.ty - knot2.vny*knot2.tx)/96;

            knot2.hsize = sqrt(dx*dx + dy*dy + dz*dz);

            /* Handle Carbonyl Oxygen Flip */
            if( prev && ((knot1.hnx*dx +
                          knot1.hny*dy +
                          knot1.hnz*dz)<0) )
            {   knot2.hnx = -dx;   knot2.vnx = -knot2.vnx;
                knot2.hny = -dy;   knot2.vny = -knot2.vny;
                knot2.hnz = -dz;   knot2.vnz = -knot2.vnz;
            } else
            {   knot2.hnx = dx;
                knot2.hny = dy;
                knot2.hnz = dz;
            }
 
            arrow = False;
            if( group->flag&CartoonFlag )
            {   if( DrawBetaArrows && (group->struc&SheetFlag) &&
                    !(group->gnext->struc&SheetFlag) )
                {   wide = (3*group->width)/2;
                    arrow = True;
                } else
                    wide = group->width;
            } else if( group->flag & WideKnotFlag )
            {   /* Average Ribbon Width */
                if( group->gnext->flag & WideKnotFlag )
                {   wide = (group->width+group->gnext->width)/2;
                } else if( group->gnext->flag & CartoonFlag )
                {   wide = group->gnext->width;
                } wide = group->width;
            } else 
                wide = group->gnext->width;
 
            /* Set Ribbon Width */
            wide = wide*Scale;
 
            if( knot2.hsize && !arrow )
            {   size = knot2.hsize;
                knot2.wx = (wide*knot2.hnx)/size;
                knot2.wy = (wide*knot2.hny)/size;
                knot2.wz = (wide*knot2.hnz)/size;
                knot2.wide = (short)wide;
            } else
            {   knot2.wide = 0;
                knot2.wx = 0;
                knot2.wy = 0;
                knot2.wz = 0;
            }
            
            if( group->flag & CartoonFlag )
                if( prev && (knot1.wide!=wide) && knot1.hsize )
                {   size = knot1.hsize;
                    lastwx = knot1.wx;
                    lastwy = knot1.wy;
                    lastwz = knot1.wz;
                    knot1.wx = ((Long)wide*knot1.hnx)/size;
                    knot1.wy = ((Long)wide*knot1.hny)/size;
                    knot1.wz = ((Long)wide*knot1.hnz)/size;
                    backside |= 0x0100;
                    if ( knot1.wide<wide )
                    {   lastwx = knot1.wx;
                        lastwy = knot1.wy;
                        lastwz = knot1.wz;
                    }

                }
 
            if( (group->flag|group->gnext->flag)&CartoonFlag )
            {   CalculateVInten( &knot2 );
                CalculateHInten( &knot2 );
 
                size = knot2.vsize;
                wide = CartoonHeight*Scale;
                knot2.dx = (wide*knot2.vnx)/size;
                knot2.dy = (wide*knot2.vny)/size;
                knot2.dz = (wide*knot2.vnz)/size;
            } else if( (group->flag|group->gnext->flag)&RibbonFlag )
                CalculateVInten( &knot2 );
        }
 
        if( !(col1 = group->col1) )
            col1 = captr->col;
 
        if( prev )
        {   /* Approximate spline segment with plane! */
            /* SolidRibbon( &knot1, &knot2, col1 );   */
 
            /* Calculate Hermite Spline Points */
            mid1.px = (54*knot1.px + (Long)9*knot1.tx +
                       10*knot2.px - 3*knot2.tx)/64;
            mid1.py = (54*knot1.py + 9*knot1.ty +
                       10*knot2.py - 3*knot2.ty)/64;
            mid1.pz = (54*knot1.pz + 9*knot1.tz +
                       10*knot2.pz - 3*knot2.tz)/64;
 
            mid2.px = (4*knot1.px + knot1.tx +
                       4*knot2.px - knot2.tx)/8;
            mid2.py = (4*knot1.py + knot1.ty +
                       4*knot2.py - knot2.ty)/8;
            mid2.pz = (4*knot1.pz + knot1.tz +
                       4*knot2.pz - knot2.tz)/8;
 
            mid3.px = (10*knot1.px + 3*knot1.tx +
                       54*knot2.px - 9*knot2.tx)/64;
            mid3.py = (10*knot1.py + 3*knot1.ty +
                       54*knot2.py - 9*knot2.ty)/64;
            mid3.pz = (10*knot1.pz + 3*knot1.tz +
                       54*knot2.pz - 9*knot2.tz)/64;
 
            if( group->flag & TraceFlag )
            {   wide = (int)(group->width*Scale);
                ClipCylinder( Rint(knot1.px), Rint(knot1.py), Rint(knot1.pz),
                              Rint(mid1.px), Rint(mid1.py), Rint(mid1.pz),
                              col1, col1, Rint(wide), ' ', Rint(wide), False);
                ClipCylinder( Rint(mid1.px), Rint(mid1.py), Rint(mid1.pz),
                              Rint(mid2.px), Rint(mid2.py), Rint(mid2.pz),
                              col1, col1, Rint(wide), ' ', Rint(wide), False);
                ClipCylinder( Rint(mid2.px), Rint(mid2.py), Rint(mid2.pz),
                              Rint(mid3.px), Rint(mid3.py), Rint(mid3.pz),
                              col1, col1, Rint(wide), ' ', Rint(wide), False);
                ClipCylinder( Rint(mid3.px), Rint(mid3.py), Rint(mid3.pz),
                              Rint(knot2.px), Rint(knot2.py), Rint(knot2.pz),
                              col1, col1, Rint(wide), ' ', Rint(wide), False);
            } else if( group->flag & DotsFlag )
            {   wide = group->width*Scale;
                ClipSphere(Rint(knot1.px),Rint(knot1.py),Rint(knot1.pz),Rint(wide),col1);
                ClipSphere(Rint(mid2.px), Rint(mid2.py), Rint(mid2.pz), Rint(wide),col1);
            } else
            {   /* Calculate Hermite Spline Widths */
                mid1.wx = (27*knot1.wx + 5*knot2.wx)/32;
                mid1.wy = (27*knot1.wy + 5*knot2.wy)/32;
                mid1.wz = (27*knot1.wz + 5*knot2.wz)/32;
 
                mid2.wx = (knot1.wx + knot2.wx)/2;
                mid2.wy = (knot1.wy + knot2.wy)/2;
                mid2.wz = (knot1.wz + knot2.wz)/2;
 
                mid3.wx = (5*knot1.wx + 27*knot2.wx)/32;
                mid3.wy = (5*knot1.wy + 27*knot2.wy)/32;
                mid3.wz = (5*knot1.wz + 27*knot2.wz)/32;
 
                /* Draw the Spline Segments */
                if( group->flag & (StrandFlag|DashStrandFlag) )
                {   if( !(col2 = group->col2) )
                        col2 = captr->col;
                    if( group->flag & StrandFlag )
                    {   StrandRibbon( &knot1, &mid1,  col1, col2 );
                        StrandRibbon( &mid1,  &mid2,  col1, col2 );
                        StrandRibbon( &mid2,  &mid3,  col1, col2 );
                        StrandRibbon( &mid3,  &knot2, col1, col2 );
                    } else /* group->flag & DashStrandFlag */
                    {   DashRibbon( &knot1, &mid1,  col1, col2 );
                        DashRibbon( &mid1,  &mid2,  col1, col2 );
                        DashRibbon( &mid2,  &mid3,  col1, col2 );
                        DashRibbon( &mid3,  &knot2, col1, col2 );
                    }
                } else /* Ribbon or Cartoon! */
                {   mid1.vsize = 0;
                    mid1.vnx = ((Long)27*knot1.vnx + 5*knot2.vnx)/32;
                    mid1.vny = ((Long)27*knot1.vny + 5*knot2.vny)/32;
                    mid1.vnz = ((Long)27*knot1.vnz + 5*knot2.vnz)/32;
                    CalculateVInten( &mid1 );
 
                    mid2.vsize = 0;
                    mid2.vnx = (knot1.vnx + knot2.vnx)/2;
                    mid2.vny = (knot1.vny + knot2.vny)/2;
                    mid2.vnz = (knot1.vnz + knot2.vnz)/2;
                    CalculateVInten( &mid2 );
 
                    mid3.vsize = 0;
                    mid3.vnx = ( 5*knot1.vnx + 27*knot2.vnx)/32;
                    mid3.vny = ( 5*knot1.vny + 27*knot2.vny)/32;
                    mid3.vnz = ( 5*knot1.vnz + 27*knot2.vnz)/32;
                    CalculateVInten( &mid3 );
 
                    if( group->flag & RibbonFlag )
                    {   if( group->struc & HelixFlag )
                        {   if( !(col2 = group->col2) )
                                col2 = captr->col;                          
                        } else col2 = col1;
                        
                        if( col1 != col2 )
                        {   SolidRibbon2( &knot1, &mid1,  col1, col2 );
                            SolidRibbon2( &mid1,  &mid2,  col1, col2 );
                            SolidRibbon2( &mid2,  &mid3,  col1, col2 );
                            SolidRibbon2( &mid3,  &knot2, col1, col2 );
                        } else
                        {   SolidRibbon( &knot1, &mid1,  col1 );
                            SolidRibbon( &mid1,  &mid2,  col1 );
                            SolidRibbon( &mid2,  &mid3,  col1 );
                            SolidRibbon( &mid3,  &knot2, col1 );
                        }
                    } else if( group->flag & CartoonFlag ) /* Cartoon! */
                    {   /* Calculate Spline Heights */
                        wide = CartoonHeight*Scale;
 
                        size = mid1.vsize;
                        mid1.dx = (wide*mid1.vnx)/size;
                        mid1.dy = (wide*mid1.vny)/size;
                        mid1.dz = (wide*mid1.vnz)/size;
 
                        size = mid2.vsize;
                        mid2.dx = (wide*mid2.vnx)/size;
                        mid2.dy = (wide*mid2.vny)/size;
                        mid2.dz = (wide*mid2.vnz)/size;
 
                        size = mid3.vsize;
                        mid3.dx = (wide*mid3.vnx)/size;
                        mid3.dy = (wide*mid3.vny)/size;
                        mid3.dz = (wide*mid3.vnz)/size;
 
                        /* Calculate Surface Intensity */
                        mid1.hsize = 0;
                        mid1.hnx = (27*knot1.hnx + 5*knot2.hnx)/32;
                        mid1.hny = (27*knot1.hny + 5*knot2.hny)/32;
                        mid1.hnz = (27*knot1.hnz + 5*knot2.hnz)/32;
                        CalculateHInten( &mid1 );
 
                        mid2.hsize = 0;
                        mid2.hnx = (knot1.hnx + knot2.hnx)/2;
                        mid2.hny = (knot1.hny + knot2.hny)/2;
                        mid2.hnz = (knot1.hnz + knot2.hnz)/2;
                        CalculateHInten( &mid2 );
 
                        mid3.hsize = 0;
                        mid3.hnx = ( 5*knot1.hnx + 27*knot2.hnx)/32;
                        mid3.hny = ( 5*knot1.hny + 27*knot2.hny)/32;
                        mid3.hnz = ( 5*knot1.hnz + 27*knot2.hnz)/32;
                        CalculateHInten( &mid3 );

                        if( arrow )
                            backside|= 0x1010;
                        
                        if( backside )
                        {   RectRibbon( &knot1, &mid1,  col1, backside,
                               lastwx, lastwy, lastwz );
                        } else
                            RectRibbon( &knot1, &mid1,  col1, 0, 0, 0, 0 );

                        if( arrow )
                        {   RectRibbon( &mid1,  &mid2,  col1, 0x1000, 0, 0, 0 );
                            RectRibbon( &mid2,  &mid3,  col1, 0x1000, 0, 0, 0 );
                            RectRibbon( &mid3,  &knot2, col1, 0x1000, 0, 0, 0 );
                        } else
                        {   RectRibbon( &mid1,  &mid2,  col1, 0, 0, 0, 0 );
                            RectRibbon( &mid2,  &mid3,  col1, 0, 0, 0, 0 );
                            RectRibbon( &mid3,  &knot2, col1, frontside, 0, 0, 0 );
                        }

                        backside = frontside = 0;
                    }
                }
            }
        } else if( group == chain->glist )
        {   knot1 = knot2;
            knot1.px = captr->x;
            knot1.py = captr->y;
            knot1.pz = captr->z;
 
            if( group->flag & RibbonFlag )
            {   SolidRibbon( &knot1, &knot2, col1 );    
            } else if( group->flag & CartoonFlag )
            {   RectRibbon( &knot1, &knot2, col1, 0x0010, 0, 0, 0 );
            } else if( group->flag & StrandFlag )
            {   if( !(col2 = group->col2) )
                    col2 = captr->col;
                StrandRibbon( &knot1,  &knot2, col1, col2 );
            } else if( group->flag & DashStrandFlag )
            {   if( !(col2 = group->col2) )
                    col2 = captr->col;
                DashRibbon( &knot1,  &knot2, col1, col2 );
            } else if( group->flag & TraceFlag )
            {   ClipCylinder( Rint(knot1.px), Rint(knot1.py), Rint(knot1.pz),
                              Rint(knot2.px), Rint(knot2.py), Rint(knot2.pz),
                              col1, col1, Rint(group->width*Scale),
                              ' ',Rint(group->width*Scale),False);
            } else if( group->flag & DotsFlag )
            {   wide = group->width*Scale;
                ClipSphere(Rint(knot1.px),Rint(knot1.py),Rint(knot1.pz),Rint(wide),col1);
            }
            prev = True;
        } else prev = True;
        group = group->gnext;
        captr = next;

        knot1 = knot2;
    }
 
    if( prev )
    {   if( !(col1 = group->col1) )
            col1 = captr->col;
 
        if( group->flag & CartoonFlag )
        {   /* Test for arrow head! */
            if( DrawBetaArrows && (group->struc&SheetFlag) )
            {   wide = (3*group->width)/2;
                knot2.px = captr->x + (knot2.tx/2);
                knot2.py = captr->y + (knot2.ty/2);
                knot2.pz = captr->z + (knot2.tz/2);
 
                arrow = True;
            } else
            {   wide = group->width;
                knot2.px = captr->x;
                knot2.py = captr->y;
                knot2.pz = captr->z;
                arrow = False;
            }
 
            wide = Scale*wide;
            if( (knot1.wide!=wide) && knot1.hsize )
            {   size = knot1.hsize;
                knot1.wx = (wide*knot1.hnx)/size;
                knot1.wy = (wide*knot1.hny)/size;
                knot1.wz = (wide*knot1.hnz)/size;
 
                if( !arrow )
                {   knot2.wx = knot1.wx;
                    knot2.wy = knot1.wy;
                    knot2.wz = knot1.wz;
                } else
                {   knot2.wx = 0;
                    knot2.wy = 0;
                    knot2.wz = 0;
                }
            } else if( arrow )
            {   knot2.wx = 0;
                knot2.wy = 0;
                knot2.wz = 0;
            }
 
            RectRibbon( &knot1, &knot2, col1, 0x0001, 0, 0, 0 );
        } else /* !Cartoon */
        {   knot2.px = captr->x;
            knot2.py = captr->y;
            knot2.pz = captr->z;
 
            if( group->flag & RibbonFlag )
            {   SolidRibbon( &knot1, &knot2, col1 );    
            } else if( group->flag & StrandFlag )
            {   if( !(col2 = group->col2) )
                    col2 = captr->col;
                StrandRibbon( &knot1,  &knot2, col1, col2 );
            } else if( group->flag & DashStrandFlag )
            {   if( !(col2 = group->col2) )
                    col2 = captr->col;
                DashRibbon( &knot1,  &knot2, col1, col2 );
            } else if( group->flag & TraceFlag )
            {   ClipCylinder( Rint(knot1.px), Rint(knot1.py), Rint(knot1.pz),
                              Rint(knot2.px), Rint(knot2.py), Rint(knot2.pz),
                              col1, col1, Rint(group->width*Scale),
                              ' ',Rint(group->width*Scale),False);
            } else if( group->flag & DotsFlag )
            {   wide = group->width*Scale;
                ClipSphere(Rint(knot1.px),Rint(knot1.py),Rint(knot1.pz),Rint(wide),col1);
                ClipSphere(Rint(knot2.px),Rint(knot2.py),Rint(knot2.pz),Rint(wide),col1);
            }   
        }
    }
}
 
 
void ResetRepres( void )
{
    DeleteDotSurface();
    DeleteMonitors();
    SolventDots = False;
    ProbeRadius = 0;

    ResetLabels();

    SurfaceChainsFlag = False;
    DotDensity = 50;
    DotSize = 1;
    DrawMonitDistance = True;
    DrawBetaArrows = True;
    CartoonHeight = 75;
}


void InitialiseRepres( void )
{
    *LabelFormat = '\0';

    DotPtr = (DotStruct __far*)0;
    MonitList = (Monitor __far*)0;
    LabelList = (void*)0;

    FreeMonit = (Monitor __far*)0;
    FreeLabel = (void*)0;

    ResetRepres();
}

#if defined RASTOPWIN
void UpdateRepres()
{    FreeLabel = (void*)0;
    FreeMonit = (Monitor __far*)0;
}
#endif


