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

/* Portion copyright (c) 2007, Philippe Valadon */


/* render.c
 */
#include "rasmol.h"

#ifdef IBMPC
#include <malloc.h>
#endif
#ifdef MSWIN
#include <windows.h>
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

#define RENDER
#include "molecule.h"
#include "graphics.h"
#include "pixutils.h"
#include "render.h"
#include "repres.h"
#include "abstree.h"
#include "transfor.h"
#include "command.h"
#include "cmndline.h"
#include "cif_fract.h"
#include "multiple.h"
#include "vector.h"
#include "brotate.h"
#include "langsel.h"

/* Avoid PowerPC Errors! */
#ifdef INFINITY
#undef INFINITY
#endif

#define PoolSize       16
#define ApproxZero     1.0E-3
#define INFINITY       200000
#define FUDGEFACTOR    1000

#ifdef INVERT
#define InvertY(y) (y)
#define ProperY(y) (-y)
#else
#define InvertY(y) (-(y))
#define ProperY(y) (y)
#endif

#define LightXComp       LightX
#define LightYComp       LightY
#define LightZComp       (LightZ*0.85)


/* Macros for commonly used loops */
#define ForEachAtom  for(chain=Database->clist;chain;chain=chain->cnext) \
                     for(group=chain->glist;group;group=group->gnext)    \
                     for(aptr=group->alist;aptr;aptr=aptr->anext)
#define ForEachBond  for(bptr=Database->blist;bptr;bptr=bptr->bnext)
#define ForEachBack  for(chain=Database->clist;chain;chain=chain->cnext) \
                     for(bptr=chain->blist;bptr;bptr=bptr->bnext)


typedef struct { double h,l; } Interval;

  
#if !defined(IBMPC) && !defined(APPLEMAC)
Card ColConstTable[MAXRAD];
#endif

static RAtom __far * __far *YBucket;
static RAtom __far * __far *IBuffer;

static int DBClear;

static RAtom __far *SBuffer;
static RAtom __far *Exclude;
static double ShadowI, ShadowJ, ShadowK;
static int  ShadowX, ShadowY, ShadowZ;
static int deltax, deltay, deltaz;
static int xcord, ycord, zcord;
static int xflag, yflag, zflag;
static int xhash, yhash, zhash;
static int RayCount;

static Item __far *FreeItem;
static double VoxRatio;
static int VoxelCount,InVoxCount;
static int VoxelsDone;

/* Identified Atom Info */
static AtomRef PickHist[4];
static Long IdentDist;
static int IdentFound;
static int IdentDepth;



/*=======================*/
/*  Function Prototypes  */
/*=======================*/

static void SqrInterval( Interval __far* );
static void VoxelInsert( RAtom __far*, int );
static int AtomInter( RAtom __far* );
static void TestAtomProximity( RAtom __far *, int, int );
static void DisplayHBonds( HBond __far *, int );



static void FatalRenderError( char *ptr )
{
    char buffer[80];

    sprintf(buffer,"Renderer Error: Unable to allocate %s!",ptr);
    RasMolFatalExit(buffer);
}


unsigned int isqrt( Card val )
{
#ifndef sun386
    register int i,result;
    register Card temp;
    register Card rem;

    i = 16;
    while( !(val&((Card)3<<30)) && i )
    {   val <<= 2;
        i--;
    }

    if( i )
    {   rem = (val>>30)-1;
        val <<= 2;
        result = 1;
        i--;

        while( i )
        {   rem = (rem<<2) | (val>>30);
            result <<= 1;
            val <<= 2;

            temp = result<<1;
            if( rem > temp )
            {   rem -= temp|1;
                result |= 1;
            }
            i--;
        }
        return( result );
    } else return( 0 );
#else
    return( (int)sqrt((double)val) );
#endif
}



/*=============================*/
/*  ClearBuffers Subroutines!  */
/*=============================*/
 
#ifdef MSWIN
/* Windows NT vs Microsoft C vs Borland Turbo C */

#if !defined RASTOPWIN
static void ClearMemory(  char __huge *ptr, long count )
{
#ifndef _WIN32
#ifdef __TURBOC__
    register long left;
    register int off;

    off = OFFSETOF(ptr);
    if( off )
    {   left = 65536 - off;
        if( count < left )
        {   _fmemset(ptr,0,(size_t)count);
            return;
        } else
        {   _fmemset(ptr,0,(size_t)left);
            count -= left;
            ptr += left;
        }
    }

    while( count > 65535 )
    {   _fmemset(ptr,0,(size_t)65535);
        count -= 65536;
        ptr += 65535;
        *ptr++ = '\0';
    }

    if( count )
        _fmemset(ptr,0,(size_t)count);
#else /* Microsoft C/C++ */
    while( count > 65534 )
    {   _fmemset(ptr,0,(size_t)65534);
        count -= 65534;
    ptr += 65534;
    }
    if( count )
        _fmemset(ptr,0,(size_t)count);
#endif
#else  /* Windows NT  */
    memset(ptr,0,(size_t)count);
#endif /* Windows NT */
}


void ClearBuffers( void )
{
    register Pixel __huge *ptr;

    if( !FBClear )
    {   FBClear = True;
        ptr = (Pixel __huge*)GlobalLock(FBufHandle);
        ClearMemory((char __huge*)ptr,(Long)XRange*YRange*sizeof(Pixel));
        GlobalUnlock(FBufHandle);
    }

    if( !DBClear )
    {   DBClear = True;
        ptr = (Pixel __huge*)GlobalLock(DBufHandle);
        ClearMemory((char __huge*)ptr,(Long)XRange*YRange*sizeof(short));
        GlobalUnlock(DBufHandle);
    }
}

#else    /* RASTOPWIN - Microsoft Visual C++ */

void ClearBuffers( void )
{
    register Pixel __huge *ptr;
#if !defined EIGHTBIT
    register Pixel __huge *end;
    register Long fill;
#endif

    if( !FBClear )
    {   FBClear = True;
        ptr = (Pixel __huge*)GlobalLock(FBufHandle);
#ifdef EIGHTBIT
        memset((char __huge*)ptr,0,(size_t)XRange*YRange*sizeof(Pixel));
#else /*THIRTYTWOBIT*/
        fill = Lut[BackCol];
        end = (Long*)(ptr+(Long)XRange*YRange);
        do { *ptr++ = fill; 
        } while( ptr<end );
#endif
        GlobalUnlock(FBufHandle);
    }

    if( !DBClear )
    {   DBClear = True;
        ptr = (Pixel __huge*)GlobalLock(DBufHandle);
        memset((char __huge*)ptr,0,(size_t)XRange*YRange*sizeof(short));
        GlobalUnlock(DBufHandle);
    }
}
#endif
/* End MSWIN */

#else    
#if defined(VMS) || defined(__sgi)        /* memset */
void ClearBuffers( void )
{
#ifndef EIGHTBIT
    register Long *ptr;
    register Long *end;
    register Long fill;

    if( !FBClear )
    {   FBClear = True;
        fill = Lut[BackCol];
#ifdef SIXTEENBIT
        fill |= fill<16;
#endif
        ptr = (Long*)FBuffer;
        end = (Long*)(FBuffer+(Long)XRange*YRange);
        do { *ptr++ = fill; *ptr++ = fill;
             *ptr++ = fill; *ptr++ = fill;
        } while( ptr<end );
    }
#else

    if( !FBClear )
    {   FBClear = True;
        memset(FBuffer,Lut[BackCol],(Long)XRange*YRange*sizeof(Pixel));
    }
#endif

    if( !DBClear )
    {   DBClear = True;
        memset(DBuffer,0,(Long)XRange*YRange*sizeof(short));
    }
}

#else /* !memset */
void ClearBuffers( void )
{
    register Long *ptr;
    register Long *end;
    register Long fill;

    if( !FBClear )
    {   FBClear = True;
    fill = Lut[BackCol];
#ifdef EIGHTBIT
    fill |= fill<<8;
    fill |= fill<<16;
#endif
#ifdef SIXTEENBIT
        fill |= fill<<16;
#endif
    ptr = (Long*)FBuffer;
    end = (Long*)(FBuffer+(Long)XRange*YRange);
    do { *ptr++ = fill; *ptr++ = fill;
         *ptr++ = fill; *ptr++ = fill;
    } while( ptr<end );
    }

    if( !DBClear )
    {   DBClear = True;
    ptr = (Long*)DBuffer;
    end = (Long*)(DBuffer+(Long)XRange*YRange);
    do { *ptr++ = 0; *ptr++ = 0;
         *ptr++ = 0; *ptr++ = 0;
    } while( ptr<end );
    }
}
#endif /* !memset    */
#endif /* UNIX & VMS */


void ReAllocBuffers( void )
{
    register RAtom __far * __far *iptr;
    register int index,len;
    register Long temp;

    temp = (Long)XRange*YRange*sizeof(short)+32;
#ifdef MSWIN
    if( DBufHandle ) GlobalFree(DBufHandle);
    DBufHandle = GlobalAlloc(GMEM_MOVEABLE,temp);
    if( !DBufHandle ) FatalRenderError("depth buffer");
#else
    if( DBuffer ) _ffree( DBuffer );
    DBuffer = (short*)_fmalloc( temp );
    if( !DBuffer ) FatalRenderError("depth buffer");
#endif
    DBClear=False;

    if( YBucket && (BuckY<YRange) )
    {   _ffree(YBucket); 
        YBucket=(void __far*)0; 
    }

    if( !YBucket )
    {   len = (YRange+4)*sizeof(RAtom __far*);
        YBucket = (RAtom __far* __far*)_fmalloc(len);
        if( !YBucket ) FatalRenderError("Y buckets");
        BuckY = YRange;
    }

    if( IBuffer && (ItemX<XRange) )
    {   _ffree(IBuffer); 
        IBuffer=(void __far*)0; 
    }

    if( !IBuffer )
    {   len = (XRange+4)*sizeof(RAtom __far*);
        IBuffer = (RAtom __far* __far*)_fmalloc(len);
        if( !IBuffer ) FatalRenderError("item buffer");
        len = XRange>>2;  iptr = IBuffer;
        for( index=0; index<=len; index++ )
        {   *iptr++ = (void __far*)0;  *iptr++ = (void __far*)0;
            *iptr++ = (void __far*)0;  *iptr++ = (void __far*)0;
        }
        ItemX = XRange;
    }
}


void ReSizeScreen( void )
{

#ifdef MSWIN
    if( !FBufHandle || (FBufX!=XRange) || (FBufY!=YRange) )
#else /* UNIX */
    if( !FBuffer || (FBufX!=XRange) || (FBufY!=YRange) )
#endif
    {   if( !CreateImage() )
            FatalRenderError("frame buffer");

        BucketFlag = False;
        FBufX=XRange;  FBufY=YRange;  FBClear = False;
        ReAllocBuffers();
        ClearBuffers();

#ifdef THIRTYTWOBIT
        if( BackR||BackG||BackB||DisplayMode )
            FBClear = False;
#endif

        /* Make sure to redraw if new size */
        RefreshState = True;
    }
}


static void PrepareYBucket( void )
{
    register RAtom __far * __far *temp;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register int scan;
    register int rad;

    temp = YBucket;
    for( scan=0; scan<BuckY; scan++ )
        *temp++ = (void __far*)0;

    if( UseClipping )
    {   ForEachAtom
        if( aptr->flag&SphereFlag )
        {   rad = aptr->irad;
            if( (aptr->x-rad>=XRange) || (aptr->x+rad<0) || (aptr->y+rad<0) )
                continue;
            if( (scan=aptr->y-rad) >= BuckY )
                continue;

            if( scan>0 )
            {   aptr->bucket = YBucket[scan];
                YBucket[scan] = aptr;
            } else
            {   aptr->bucket = *YBucket;
                *YBucket = aptr;
            }
        }
    } else
    ForEachAtom
        if( aptr->flag&SphereFlag )
        {   scan = aptr->y-aptr->irad;
            aptr->bucket = YBucket[scan];
            YBucket[scan] = aptr;
        }
    BucketFlag = True;
}


static void SqrInterval( Interval __far *ival )
{
    register double l,h;

    l = ival->l;
    h = ival->h;

    if( l>=0.0 )
    {   ival->l = l*l;
    ival->h = h*h;
    } else if( h<0.0 )
    {   ival->l = h*h;
    ival->h = l*l;
    } else
    {   ival->h = (-l>h)? l*l : h*h;
    ival->l = 0.0;
    }
}


static void VoxelInsert( RAtom __far *ptr,  int ref )
{
    register Item __far *datum;
    register int i;

    if( !FreeItem )
    {   datum = (Item __far*)_fmalloc( PoolSize*sizeof(Item) );
        if( !datum ) FatalRenderError("voxel item");
        for( i=1; i<PoolSize; i++ )
        {   datum->list = FreeItem;
            FreeItem = datum++;
        }
    } else
    {   datum = FreeItem;
        FreeItem = datum->list;
    }
    datum->data = ptr;
    InVoxCount++;

    if( !HashTable[ref] ) VoxelCount++;
    datum->list = (Item __far*)HashTable[ref];
    HashTable[ref] = (void __far*)datum;
}


void ResetVoxelData( void )
{
    register Item __far *datum;
    register int i;

    if( VoxelsDone )
    {   for( i=0; i<VOXSIZE; i++ )
        if( HashTable[i] )
        {   datum = (Item __far*)HashTable[i];
            while( datum->list ) datum = datum->list;
            datum->list = FreeItem;
            FreeItem = (Item __far*)HashTable[i];
            HashTable[i] = (void __far*)0;
        }
        VoxelsDone = False;
    } else for( i=0; i<VOXSIZE; i++ )
        HashTable[i] = (void __far*)0;
    VoxelsClean = True;
}


void CreateVoxelData( int flag )   
{
    static Interval ix, iy, iz;
    register int lvx, lvy, lvz;
    register int hvx, hvy, hvz;
    register Long mx, my, mz;
    register int px, py, pz;
    register int i, rad;
    register double radius2;

    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;

    ResetVoxelData();
    InVoxCount = VoxelCount = 0;
    VoxRatio = (double)SideLen/VOXORDER;
    IVoxRatio = 1.0/VoxRatio;
    VoxelsDone = True;

    ForEachAtom
    if( aptr->flag & flag )
    {   mx = aptr->xorg + aptr->fxorg + Offset;
        my = aptr->yorg + aptr->fyorg + Offset;
        mz = aptr->zorg + aptr->fzorg + Offset;
        if( flag != SphereFlag )
        {   if( SolventDots || !ProbeRadius )
            {   rad = ElemVDWRadius(aptr->elemno);
            } else rad = ProbeRadius;
        } else rad = aptr->radius;  
        radius2 = (Long)rad*rad;

        /*Low - High*/
        lvx = (int)((mx-rad)*IVoxRatio);  hvx = (int)((mx+rad)*IVoxRatio);
        lvy = (int)((my-rad)*IVoxRatio);  hvy = (int)((my+rad)*IVoxRatio);
        lvz = (int)((mz-rad)*IVoxRatio);  hvz = (int)((mz+rad)*IVoxRatio);

        for( px=lvx; px<=hvx; px++ )
        {   ix.l=px*VoxRatio-mx;
            ix.h=ix.l+VoxRatio;  
            SqrInterval(&ix);
            i = VOXORDER2*px + VOXORDER*lvy;
           
            for( py=lvy; py<=hvy; py++ )
            {   iy.l=py*VoxRatio-my;
                iy.h=iy.l+VoxRatio;
                SqrInterval(&iy);
                
                for( pz=lvz; pz<=hvz; pz++ )
                {   iz.l=pz*VoxRatio-mz; 
                    iz.h=iz.l+VoxRatio;
                    SqrInterval(&iz);
    
#ifdef ORIG
                    /* Test for surface voxels */
                    if( ((ix.l+iy.l+iz.l)<radius2) && 
                    ((ix.h+iy.h+iz.h)>radius2) )
                        VoxelInsert( aptr, i+pz );
#else
                    /* Test for contained voxels */
                    if( ix.l+iy.l+iz.l < radius2 )
                        VoxelInsert( aptr, i+pz );
#endif
                } /*pz*/
                i += VOXORDER;
            } /*py*/
        } /*px*/
    }
}


void ShadowTransform( void )
{
    ShadowI = (LightXComp*LightDot(RotX[0],-RotY[0],RotZ[0]))/LightLength;
    ShadowJ = (LightYComp*LightDot(RotX[0],-RotY[0],RotZ[0]))/LightLength;
    ShadowK = (LightZComp*LightDot(RotX[0],-RotY[0],RotZ[0]))/LightLength;

    if( ShadowI>ApproxZero )
    {   deltax =  (int)(FUDGEFACTOR/ShadowI); xhash =  VOXORDER2; xflag =  1;
    } else if( ShadowI<-ApproxZero )
    {   deltax = -(int)(FUDGEFACTOR/ShadowI); xhash = -VOXORDER2; xflag = -1;
    } else xflag = 0;

    if( ShadowJ>ApproxZero )
    {   deltay =  (int)(FUDGEFACTOR/ShadowJ); yhash =  VOXORDER; yflag =  1;
    } else if( ShadowJ<-ApproxZero )
    {   deltay = -(int)(FUDGEFACTOR/ShadowJ); yhash = -VOXORDER; yflag = -1;
    } else yflag = 0;

    if( ShadowK>ApproxZero )
    {   deltaz =  (int)(FUDGEFACTOR/ShadowK); zhash = zflag =  1;
    } else if( ShadowK<-ApproxZero )
    {   deltaz = -(int)(FUDGEFACTOR/ShadowK); zhash = zflag = -1;
    } else zflag = 0;
}


static int AtomInter( RAtom __far *ptr )
{
    register Long modv,radius2;
    register int vx, vy, vz;
    register double tca;

    if( ptr->mbox == RayCount )
        return False;
    ptr->mbox = RayCount;

    vx = (int)(ptr->xorg + ptr->fxorg) - ShadowX;
    vy = (int)(ptr->yorg + ptr->fyorg) - ShadowY;
    vz = (int)(ptr->zorg + ptr->fzorg) - ShadowZ;

    tca = vx*ShadowI + vy*ShadowJ + vz*ShadowK;
    if( tca<0.0 ) return False;
    
    radius2 = ptr->radius+10;  radius2 = radius2*radius2;
    modv = (Long)vx*vx + (Long)vy*vy + (Long)vz*vz - radius2;
    return( modv<tca*tca );
}


static int VoxelInter( Item __far *ptr )
{
    while( ptr )
    {   if( (ptr->data!=Exclude) && AtomInter(ptr->data) )
        {   SBuffer = ptr->data;
            return True;
        }
        ptr = ptr->list;
    }
    return False;
}


static int ShadowRay( void )
{
    register Item __far * __far *ident;
    register double ex, ey, ez;
    register Long dx, dy, dz;
    register int ref;
   
    RayCount++;
    if( SBuffer )
    {   if( (SBuffer!=Exclude) && AtomInter(SBuffer) )
            return True;
        SBuffer = (void __far*)0;
    }

    ex = IVoxRatio*(ShadowX+Offset);  xcord = (int)ex;
    ey = IVoxRatio*(ShadowY+Offset);  ycord = (int)ey;
    ez = IVoxRatio*(ShadowZ+Offset);  zcord = (int)ez;

    ref = VOXORDER2*xcord+VOXORDER*ycord+zcord;
    ident = (Item __far* __far*)(HashTable+ref);

    if( xflag == 1 ) 
    {   dx = (Long)(((xcord+1)-ex)*deltax);
    } else if( xflag == -1 )
    {   dx = (Long)((ex-xcord)*deltax); 
    } else dx = INFINITY;

    if( yflag == 1 ) 
    {   dy = (Long)(((ycord+1)-ey)*deltay);
    } else if( yflag == -1 )
    {   dy = (Long)((ey-ycord)*deltay); 
    } else dy = INFINITY;

    if( zflag == 1 ) 
    {   dz = (Long)(((zcord+1)-ez)*deltaz);
    } else if( zflag == -1 )
    {   dz = (Long)((ez-zcord)*deltaz); 
    } else dz = INFINITY;
    
    while( !VoxelInter(*ident) )
    {   if( (dx<=dy) && (dx<=dz) )
        {   xcord += xflag;
            if( (xcord<0) || (xcord>=VOXORDER) )
                return False;
            ident += xhash; 
            dx += deltax;
        } else if( dy <= dz  ) /*(dy<=dx)*/
        {   ycord += yflag;
            if( (ycord<0) || (ycord>=VOXORDER) )
                return False;
            ident += yhash;
            dy += deltay;
        } else /* (dz<=dx) && (dz<=dy) */
        {   zcord += zflag;
            if( (zcord<0) || (zcord>=VOXORDER) )
                return False;
            ident += zhash; 
            dz += deltaz;
        }
    }
    return True;
}


#define UpdateScanAcross \
    if( depth>*dptr )   \
    {   *dptr = depth;  \
        iptr[dx] = ptr; \
    } dptr++; dx++;


/* ScanLine for Shadows! */
static void ScanLine( void )
{
    static RAtom __far *list;
    register RAtom __far *ptr;
    register RAtom __far * __far *iptr;
    register RAtom __far * __far *prev;
    register short __huge *dbase;
    register short __huge *dptr;
    register Pixel __huge *fptr;
    register Byte __far *tptr;

    register int pos,depth,inten;
    register int lastx,wide,scan;
    register int dx,dy,dz;

    fptr = FBuffer;
    dbase = DBuffer;
    list = (void __far*)0;  

    wide = XRange>>2;  iptr = IBuffer;
    for( pos=0; pos<=wide; pos++ )
    {   *iptr++ = (void __far*)0;  *iptr++ = (void __far*)0;
        *iptr++ = (void __far*)0;  *iptr++ = (void __far*)0;
    }

    for( scan=0; scan<YRange; scan++ )
    {   for( ptr = YBucket[scan]; ptr; ptr = ptr->bucket )
        {    ptr->next = list; list = ptr;
        }

        prev = &list;
        for( ptr=list; ptr; ptr=ptr->next )
        {   dy = scan - ptr->y;
            wide = LookUp[ptr->irad][AbsFun(dy)];
            lastx = (XRange-1)-ptr->x;
            if( wide<lastx ) lastx=wide;
            dx = - MinFun(wide,ptr->x);
    
            iptr = IBuffer+ptr->x;
            tptr = LookUp[wide];

            dptr = dbase+ptr->x+dx;
            while( dx<=lastx )
            {   depth = tptr[AbsFun(dx)]+ptr->z;
                UpdateScanAcross;
            }
    
            /* Remove completed atoms */
            if( dy == ptr->irad )
            {   *prev = ptr->next;
            } else prev = &ptr->next;
        } /*ptr*/
    
    
        /* Process visible scanline */
        prev = (RAtom __far* __far*)IBuffer;
        SBuffer = (void __far*)0;
        dptr = dbase; 
    
        for( pos=0; pos<XRange; pos++ )
        {   if( *prev )
            {   ptr = *prev;
                dz = *dptr-ptr->z;

                inten = (int)LightDot(pos-ptr->x,scan-ptr->y,dz);
                if( inten>0 )
                {   inten = (int)( (inten*ColConst[ptr->irad])>>ColBits);
                    dx = pos-XOffset;
                    dy = scan-YOffset;
                    dz = *dptr-ZOffset;
    
                    ShadowX = (int)(dx*InvX[0]+dy*InvX[1]+dz*InvX[2]);
                    ShadowY = (int)(dx*InvY[0]+dy*InvY[1]+dz*InvY[2]);
                    ShadowZ = (int)(dx*InvZ[0]+dy*InvZ[1]+dz*InvZ[2]);
        
                    Exclude = ptr;
                    if( ShadowRay() )
                    {   *fptr = Lut[ptr->col+(inten>>2)];
                    } else 
                        *fptr = Lut[ptr->col+inten];
                } else 
                    *fptr = Lut[ptr->col];
                *prev = (void __far*)0;
            }
            dptr++; fptr++; prev++;
        }
        dbase = dptr;
    } /*scan*/
}



static void DisplaySpaceFill( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;

    if( UseShadow )
    {   if( !BucketFlag )
            PrepareYBucket();
           ScanLine();
    } else if( UseClipping )
    {   ForEachAtom
        if( aptr->flag&SphereFlag )
            ClipSphere(aptr->x,aptr->y,aptr->z,aptr->irad,aptr->col);
    } else 
    ForEachAtom
        if( aptr->flag&SphereFlag )
            DrawSphere(aptr->x,aptr->y,aptr->z,aptr->irad,aptr->col);
}

static void DisplayStars( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;

    if( UseClipping )
    {   ForEachAtom
        if( aptr->flag&StarFlag )
            ClipStar(aptr->x,aptr->y,aptr->z,aptr->radius,aptr->col);
    } else 
    ForEachAtom
        if( aptr->flag&StarFlag )
            DrawStar(aptr->x,aptr->y,aptr->z,aptr->radius,aptr->col);
}


static void DisplayWireframe( void )
{
    register Bond __far *bptr;
    register RAtom __far *s;
    register RAtom __far *d;
    register int sc,dc;

    if( UseClipping )
    {   ForEachBond
       if( bptr->flag & DrawBondFlag )
           {   s = bptr->srcatom; d = bptr->dstatom;
           if( !bptr->col ) 
           {   sc = s->col;  dc = d->col;
           } else sc = dc = bptr->col;

           if( bptr->flag&WireFlag )
           {   ClipTwinVector(s->x,s->y,s->z,d->x,d->y,d->z,sc,dc,bptr->altl);
           } else if( bptr->flag&CylinderFlag )
           {   if( bptr->irad>0 )
               {  ClipCylinder(s->x,s->y,s->z,d->x,d->y,d->z,
                               sc,dc,bptr->irad, bptr->altl,bptr->iarad,bptr->cap);
                } else ClipTwinLine(s->x,s->y,s->z,d->x,d->y,d->z,
                                   sc+ColourMask,dc+ColourMask,bptr->altl);
           } else /* bptr->flag & DashFlag */
               ClipDashVector(s->x,s->y,s->z,d->x,d->y,d->z,sc,dc,bptr->altl);
       }
    } else
    ForEachBond
       if( bptr->flag & DrawBondFlag )
           {   s = bptr->srcatom; d = bptr->dstatom;
               if( !bptr->col )
               {   sc = s->col;  dc = d->col;
               } else sc = dc = bptr->col;

               if( bptr->flag&WireFlag )
               {    ClipTwinVector(s->x,s->y,s->z,d->x,d->y,d->z,sc,dc,
                        bptr->altl);
           } else if( bptr->flag&CylinderFlag )
           {   if( bptr->irad>0 )
               {  DrawCylinder(s->x,s->y,s->z,d->x,d->y,d->z,
                               sc,dc,bptr->irad,bptr->altl,bptr->iarad,bptr->cap);
               } else DrawTwinLine(s->x,s->y,s->z,d->x,d->y,d->z,
                   sc+ColourMask,dc+ColourMask,bptr->altl);
           } else ClipDashVector(s->x,s->y,s->z,d->x,d->y,d->z,sc,dc,
                        bptr->altl);
           }
}


/* Used by DisplayDoubleBonds! */
static void DisplayCylinder( int x1, int y1, int z1, 
                             int x2, int y2, int z2, 
                             int c1, int c2, int rad,
                             char altl, int arad, short cap)
{
    if( UseClipping )
    {   if( rad == 0 )
        {   ClipTwinLine(x1,y1,z1,x2,y2,z2,c1+ColourMask,c2+ColourMask,altl);
        } else ClipCylinder(x1,y1,z1,x2,y2,z2,c1,c2,rad,altl,arad,cap);
    } else
    {   if( rad == 0 )
        {   DrawTwinLine(x1,y1,z1,x2,y2,z2,c1+ColourMask,c2+ColourMask,altl);
        } else DrawCylinder(x1,y1,z1,x2,y2,z2,c1,c2,rad,altl,arad,cap);
    }
}


static void DisplayDoubleBonds( void )
{
    register RAtom __far *s;
    register RAtom __far *d;
    register Bond __far *bptr;
    register int dx,dy,ix,iy;
    register int ax,ay,sc,dc;
    register int k,flag;

    ForEachBond
        if( bptr->flag & DrawBondFlag )
        {   s = bptr->srcatom; d = bptr->dstatom;
            if( !bptr->col ) 
            {   sc = s->col;  dc = d->col;
            } else sc = dc = bptr->col;

            flag = (bptr->flag&CylinderFlag) && (bptr->irad>4);
            if( !(bptr->flag & DoubBondFlag) || flag )
            {   if( bptr->flag&WireFlag )
                {   ClipTwinVector(s->x,s->y,s->z,d->x,d->y,d->z,sc,dc, 
                     bptr->altl);
                } else if( bptr->flag&CylinderFlag )
                {   DisplayCylinder(s->x,s->y,s->z,d->x,d->y,d->z,
                                sc,dc,bptr->irad,bptr->altl,bptr->iarad,bptr->cap);
                } else /* bptr->flag & DashFlag */
                    ClipDashVector(s->x,s->y,s->z,d->x,d->y,d->z,sc,dc,
                      bptr->altl);
            } 

            if( (bptr->flag & (DoubBondFlag|TripBondFlag)) && !flag )
            {   if( s->x > d->x )
                {   ax = s->x - d->x;  dx = 1;
                } else
                {   ax = d->x - s->x;  dx = 0;
                }

                if( s->y > d->y )
                {   ay = s->y - d->y;  dy = 1;
                } else 
                {   ay = d->y - s->y;  dy = 0;
                }

                /* Determine Bond Separation */
                if( (bptr->flag&CylinderFlag) && bptr->irad )
                {   if( bptr->flag & DoubBondFlag )
                    {   k = (3*bptr->irad+1)>>1;
                    } else k = 3*bptr->irad;
                } else k = (bptr->flag&TripBondFlag)?3:2;

                if( ax > (ay<<1) )
                {   ix = 0;  iy = k;
                } else if( ay > (ax<<1) )
                {   iy = 0;  ix = k;
                } else /* diagonal */
                {   k = (3*k)>>2;
                    if( dx == dy )
                    {   ix = k;  iy = -k;
                    } else
                    {   ix = k;  iy = k;
                    }
                }

                if( bptr->flag&WireFlag )
                {   ClipTwinVector(s->x+ix,s->y+iy,s->z,
                                   d->x+ix,d->y+iy,d->z,sc,dc,bptr->altl);
                    ClipTwinVector(s->x-ix,s->y-iy,s->z,
                                   d->x-ix,d->y-iy,d->z,sc,dc,bptr->altl);
                } else if( bptr->flag&CylinderFlag )
                {   DisplayCylinder(s->x+ix,s->y+iy,s->z,d->x+ix,d->y+iy,d->z,
                                sc,dc,bptr->irad,bptr->altl,bptr->iarad,bptr->cap);
                    DisplayCylinder(s->x-ix,s->y-iy,s->z,d->x-ix,d->y-iy,d->z,
                                sc,dc,bptr->irad,bptr->altl,bptr->iarad,bptr->cap);
                } else /* bptr->flag & DashFlag */
                {  ClipDashVector(s->x+ix,s->y+iy,s->z,
                                  d->x+ix,d->y+iy,d->z,sc,dc,bptr->altl);
                   ClipDashVector(s->x+ix,s->y+iy,s->z,
                                  d->x+ix,d->y+iy,d->z,sc,dc,bptr->altl);
                }
            }
        }
}


static void CreateFlatCap( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register Bond __far *bptr;

    if( !Database ) 
        return;

    /* Reset */
    ForEachAtom
        aptr->visited = False;

    ForEachBond
        {   bptr->srcatom->visited += 1;
            bptr->dstatom->visited += 1;
        }

    /* cap = 1  src is flat cap
       cap = 2  des is flat cap
       cap = 3  src and des are both flat cap */
    ForEachBond
        {   if( bptr->srcatom->visited == 1 )
                bptr->cap = 1;
            else
                bptr->cap = 0; 
            if( bptr->dstatom->visited == 1 )
                bptr->cap += 2;
        }

    CapClean = True;
}

static void DisplayBackbone( void )
{
    register Chain __far *chain;
    register Bond __far *bptr;
    register RAtom __far *s;
    register RAtom __far *d;
    register int sc,dc;

    ForEachBack
       if( bptr->flag&DrawBondFlag )
       {   s = bptr->srcatom; d = bptr->dstatom;
           if( !bptr->col ) 
           {   sc = s->col;  dc = d->col;
           } else sc = dc = bptr->col;

           if( bptr->flag&CylinderFlag )
           {   if( bptr->irad>0 )
               { ClipCylinder(s->x,s->y,s->z,d->x,d->y,d->z,
                              sc,dc,bptr->irad,bptr->altl,bptr->iarad,False);
               } else ClipTwinLine(s->x,s->y,s->z,d->x,d->y,d->z,
                                   sc+ColourMask,dc+ColourMask,bptr->altl);
           } else if( bptr->flag & WireFlag )
           {      ClipTwinVector(s->x,s->y,s->z,d->x,d->y,d->z,sc,dc,bptr->altl);
           } else ClipDashVector(s->x,s->y,s->z,d->x,d->y,d->z,sc,dc,bptr->altl);
       }
}


static void DisplayHBonds( HBond __far *list, int mode )
{
    register HBond __far *ptr;
    register RAtom __far *s;
    register RAtom __far *d;
    register int sc,dc;

    for( ptr=list; ptr; ptr=ptr->hnext )
        if( ptr->flag & DrawBondFlag )
        {   if( mode )
            {   s = ptr->srcCA; d = ptr->dstCA;
                if( !s || !d ) continue;
            } else
            {   d = ptr->src;
                s = ptr->dst;
            }

            if( !ptr->col )
            {   sc = s->col;  dc = d->col;
            } else sc = dc = ptr->col;
            if( ptr->flag & CylinderFlag )
            {   if( ptr->irad>0 )
            {   ClipCylinder(s->x,s->y,s->z,d->x,d->y,d->z,
                                sc,dc,ptr->irad,ptr->altl,ptr->iarad,False);
            } else ClipTwinLine(s->x,s->y,s->z,d->x,d->y,d->z,
                                sc+ColourMask,dc+ColourMask,ptr->altl);
            } else ClipDashVector(s->x,s->y,s->z,d->x,d->y,d->z,sc,dc,ptr->altl);
        }
}


static void DisplayWorldAxes( void )
{   
    register double dxx,dxy,dxz;
    register double dyx,dyy,dyz;
    register double dzx,dzy,dzz;
    register double xo,yo,zo;
    register int x, y, z,max;

    /* Draw once */
    if( CurMolSet != MoleculeList )
        return;

    /* Transpose in screen coordinates */
    xo = Wo[0] + SRange;
    yo = Wo[1] + HRange;
    zo = Wo[2] + DRange;

    /* Use size from first MolSet */
    max = MaxFun(MaxX,MaxY);
    max = MaxFun(max,MaxZ);

    dxx = max*Scale*WAxes[0][0];
    dxy = max*Scale*WAxes[1][0];
    dxz = max*Scale*WAxes[2][0];

    dyx = max*Scale*WAxes[0][1];
    dyy = max*Scale*WAxes[1][1];
    dyz = max*Scale*WAxes[2][1];

    dzx = max*Scale*WAxes[0][2];
    dzy = max*Scale*WAxes[1][2];
    dzz = max*Scale*WAxes[2][2];
            
    x = (int)(xo + dxx); y = (int)(yo + dxy); z = (int)(zo + dxz);
    if( ZValid(z) ) DisplayRasString(x+2,y,z,(unsigned char *)"X",AxeCol);
    ClipTwinLine((int)(xo-dxx),(int)(yo-dxy),(int)(zo-dxz),x,y,z,AxeCol,AxeCol,' ');
    
    x = (int)(xo + dyx); y = (int)(yo + dyy); z = (int)(zo + dyz);
    if( ZValid(z) ) 
        DisplayRasString((int)(xo-dyx)+2,(int)(yo-dyy),(int)(zo-dyz),
        (unsigned char *)"Y",AxeCol);
    ClipTwinLine((int)(xo-dyx),(int)(yo-dyy),(int)(zo-dyz),x,y,z,AxeCol,AxeCol,' ');
        
    x = (int)(xo - dzx); y = (int)(yo - dzy); z = (int)(zo - dzz);
    if( ZValid(z) ) DisplayRasString(x+2,y,z,(unsigned char *)"Z",AxeCol);
    ClipTwinLine((int)(xo+dzx),(int)(yo+dzy),(int)(zo+dzz),x,y,z,AxeCol,AxeCol,' ');
}


static void DisplayBoxes( void )
{
    register double lena, lenb, lenc;
    register double tmpx, tmpy, tmpz;
    register double cosa, cosb, cosg;
    register double temp, sing;

    register int xorg, yorg, zorg;
    register int dxx,dxy,dxz;
    register int dyx,dyy,dyz;
    register int dzx,dzy,dzz;
    register int x, y, z;
    register int col;

    if( UseDepthCue )
        col = BoxCol;
    else
        col = BoxCol + ColourMask;

    if( DrawAxes  || DrawBoundBox )
    {   
        dxx = (int)(MaxX*MatX[0]);
        dxy = (int)(MaxX*MatY[0]);
        dxz = (int)(MaxX*MatZ[0]);

        dyx = (int)(MaxY*MatX[1]);
        dyy = (int)(MaxY*MatY[1]);
        dyz = (int)(MaxY*MatZ[1]);

        dzx = (int)(MaxZ*MatX[2]);
        dzy = (int)(MaxZ*MatY[2]);
        dzz = (int)(MaxZ*MatZ[2]);
            
        if( DrawAxes )
        {   /* Line (MinX,0,0) to (MaxX,0,0) */
            x = XOffset+dxx;  y = YOffset+dxy;  z = ZOffset+dxz;
            if( ZValid(z) && (z>0) )
                DisplayRasString(x+2,y,z,(unsigned char *)"X",col);
            if( UseDepthCue )
                ClipTwinVector(XOffset-dxx,YOffset-dxy,ZOffset-dxz,x,y,z,col,col,' ');
            else
                ClipTwinLine(XOffset-dxx,YOffset-dxy,ZOffset-dxz,x,y,z,col,col,' ');
    
            /* Line (0,MinY,0) to (0,MaxY,0) */
            x = XOffset+dyx;  y = YOffset+dyy;  z = ZOffset+dyz;
            if( ZValid(z) && (z>0) )
                DisplayRasString(XOffset-dyx+2,YOffset-dyy,ZOffset-dyz,
                    (unsigned char *)"Y",col);
            if( UseDepthCue )
                ClipTwinVector(XOffset-dyx,YOffset-dyy,ZOffset-dyz,x,y,z,col,col,' ');
            else
                ClipTwinLine(XOffset-dyx,YOffset-dyy,ZOffset-dyz,x,y,z,col,col,' ');

            /* Line (0,0,MinZ) to (0,0,MaxZ) */
            x = XOffset-dzx;  y = YOffset-dzy;  z = ZOffset-dzz;
            if( ZValid(z) && (z>0) ) DisplayRasString(x+2,y,z,
              (unsigned char *)"Z",col);
            if( UseDepthCue )
                ClipTwinVector(XOffset+dzx,YOffset+dzy,ZOffset+dzz,x,y,z,col,col,' ');
            else
                ClipTwinLine(XOffset+dzx,YOffset+dzy,ZOffset+dzz,x,y,z,col,col,' '); 
        }

        if( DrawBoundBox )
        {   /* Line (MinX,MinY,MinZ) to (MaxX,MinY,MinZ) */
            x=XOffset-dyx-dzx;  y=YOffset-dyy-dzy;  z=ZOffset-dyz-dzz;
            if( UseDepthCue )
                ClipDashVector(x-dxx,y-dxy,z-dxz,x+dxx,y+dxy,z+dxz,col,col,' ');
            else
                ClipDashLine(x-dxx,y-dxy,z-dxz,x+dxx,y+dxy,z+dxz,col,col,' ');
    
            /* Line (MaxX,MinY,MinZ) to (MaxX,MaxY,MinZ) */
            x=XOffset+dxx-dzx;  y=YOffset+dxy-dzy;  z=ZOffset+dxz-dzz;
            if( UseDepthCue )
                ClipDashVector(x-dyx,y-dyy,z-dyz,x+dyx,y+dyy,z+dyz,col,col,' ');
            else
                ClipDashLine(x-dyx,y-dyy,z-dyz,x+dyx,y+dyy,z+dyz,col,col,' ');
    
            /* Line (MaxX,MaxY,MinZ) to (MinX,MaxY,MinZ) */
            x=XOffset+dyx-dzx;  y=YOffset+dyy-dzy;  z=ZOffset+dyz-dzz;
            if( UseDepthCue )
                ClipDashVector(x+dxx,y+dxy,z+dxz,x-dxx,y-dxy,z-dxz,col,col,' ');
            else
                ClipDashLine(x+dxx,y+dxy,z+dxz,x-dxx,y-dxy,z-dxz,col,col,' ');
    
            /* Line (MinX,MaxY,MinZ) to (MinX,MinY,MinZ) */
            x=XOffset-dxx-dzx;  y=YOffset-dxy-dzy;  z=ZOffset-dxz-dzz;
            if( UseDepthCue )
                ClipDashVector(x+dyx,y+dyy,z+dyz,x-dyx,y-dyy,z-dyz,col,col,' ');
            else
                ClipDashLine(x+dyx,y+dyy,z+dyz,x-dyx,y-dyy,z-dyz,col,col,' ');
    
            /* Line (MinX,MinY,MinZ) to (MinX,MinY,MaxZ) */
            x=XOffset-dxx-dyx;  y=YOffset-dxy-dyy;  z=ZOffset-dxz-dyz;
            if( UseDepthCue )
                ClipDashVector(x-dzx,y-dzy,z-dzz,x+dzx,y+dzy,z+dzz,col,col,' ');
            else
                ClipDashLine(x-dzx,y-dzy,z-dzz,x+dzx,y+dzy,z+dzz,col,col,' ');
    
            /* Line (MaxX,MinY,MinZ) to (MaxX,MinY,MaxZ) */
            x=XOffset+dxx-dyx;  y=YOffset+dxy-dyy;  z=ZOffset+dxz-dyz;
            if( UseDepthCue )
                ClipDashVector(x-dzx,y-dzy,z-dzz,x+dzx,y+dzy,z+dzz,col,col,' ');
            else
                ClipDashLine(x-dzx,y-dzy,z-dzz,x+dzx,y+dzy,z+dzz,col,col,' ');
    
            /* Line (MaxX,MaxY,MinZ) to (MaxX,MaxY,MaxZ) */
            x=XOffset+dxx+dyx;  y=YOffset+dxy+dyy;  z=ZOffset+dxz+dyz;
            if( UseDepthCue )
                ClipDashVector(x-dzx,y-dzy,z-dzz,x+dzx,y+dzy,z+dzz,col,col,' ');
            else
                ClipDashLine(x-dzx,y-dzy,z-dzz,x+dzx,y+dzy,z+dzz,col,col,' ');
    
            /* Line (MinX,MaxY,MinZ) to (MinX,MaxY,MaxZ) */
            x=XOffset-dxx+dyx;  y=YOffset-dxy+dyy;  z=ZOffset-dxz+dyz;
            if( UseDepthCue )
                ClipDashVector(x-dzx,y-dzy,z-dzz,x+dzx,y+dzy,z+dzz,col,col,' ');
            else
                ClipDashLine(x-dzx,y-dzy,z-dzz,x+dzx,y+dzy,z+dzz,col,col,' ');
   
            /* Line (MinX,MinY,MaxZ) to (MaxX,MinY,MaxZ) */
            x=XOffset-dyx+dzx;  y=YOffset-dyy+dzy;  z=ZOffset-dyz+dzz;
            if( UseDepthCue )
                ClipDashVector(x-dxx,y-dxy,z-dxz,x+dxx,y+dxy,z+dxz,col,col,' ');
            else
                ClipDashLine(x-dxx,y-dxy,z-dxz,x+dxx,y+dxy,z+dxz,col,col,' ');

            /* Line (MaxX,MinY,MaxZ) to (MaxX,MaxY,MaxZ) */
            x=XOffset+dxx+dzx;  y=YOffset+dxy+dzy;  z=ZOffset+dxz+dzz;
            if( UseDepthCue )
                ClipDashVector(x-dyx,y-dyy,z-dyz,x+dyx,y+dyy,z+dyz,col,col,' ');
            else
                ClipDashLine(x-dyx,y-dyy,z-dyz,x+dyx,y+dyy,z+dyz,col,col,' ');
    
            /* Line (MaxX,MaxY,MaxZ) to (MinX,MaxY,MaxZ) */
            x=XOffset+dyx+dzx;  y=YOffset+dyy+dzy;  z=ZOffset+dyz+dzz;
            if( UseDepthCue )
                ClipDashVector(x+dxx,y+dxy,z+dxz,x-dxx,y-dxy,z-dxz,col,col,' ');
            else
                ClipDashLine(x+dxx,y+dxy,z+dxz,x-dxx,y-dxy,z-dxz,col,col,' ');
    
            /* Line (MinX,MaxY,MaxZ) to (MinX,MinY,MaxZ) */
            x=XOffset-dxx+dzx;  y=YOffset-dxy+dzy;  z=ZOffset-dxz+dzz;
            if( UseDepthCue )
                ClipDashVector(x+dyx,y+dyy,z+dyz,x-dyx,y-dyy,z-dyz,col,col,' ');
            else
                ClipDashLine(x+dyx,y+dyy,z+dyz,x-dyx,y-dyy,z-dyz,col,col,' ');
        }
    }

    if( DrawUnitCell && *Info.spacegroup )
    {   /* Calculate Unit Cell! */
      if (det(Info.matf2o) < 1.1 )
      {
        lena = 250.0*Info.cella;
        lenb = 250.0*Info.cellb;
        lenc = 250.0*Info.cellc;

        cosa = cos(Info.cellalpha);
        cosb = cos(Info.cellbeta);
        cosg = cos(Info.cellgamma);  
        sing = sin(Info.cellgamma);

        temp = cosa*cosa + cosb*cosb + cosg*cosg - 2.0*cosa*cosb*cosg;
        tmpx = cosb; 
        tmpy = (cosa - cosb*cosg)/sing;
        tmpz = -sqrt(1.0-temp)/sing;

        dxx = (int)(lena*MatX[0]);
        dxy = (int)(lena*MatY[0]);
        dxz = (int)(lena*MatZ[0]);

        dyx = (int)(lenb*(cosg*MatX[0] + ProperY(sing*MatX[1])));
        dyy = (int)(lenb*(cosg*MatY[0] + ProperY(sing*MatY[1])));
        dyz = (int)(lenb*(cosg*MatZ[0] + ProperY(sing*MatZ[1])));

        dzx = (int)(lenc*(tmpx*MatX[0] + ProperY(tmpy*MatX[1]) + tmpz*MatX[2]));
        dzy = (int)(lenc*(tmpx*MatY[0] + ProperY(tmpy*MatY[1]) + tmpz*MatY[2]));
        dzz = (int)(lenc*(tmpx*MatZ[0] + ProperY(tmpy*MatZ[1]) + tmpz*MatZ[2]));

        xorg = XOffset - (int)(OrigCX*MatX[0]+OrigCY*MatX[1]+OrigCZ*MatX[2]);
        yorg = YOffset - (int)(OrigCX*MatY[0]+OrigCY*MatY[1]+OrigCZ*MatY[2]);
        zorg = ZOffset - (int)(OrigCX*MatZ[0]+OrigCY*MatZ[1]+OrigCZ*MatZ[2]);

      } else {

        double cellmat[3][3];
        int ii;

        for (ii = 0; ii < 3; ii++) {
          cellmat[0][ii] = MatX[0]*Info.matf2o[0][ii] + 
            ProperY(MatX[1]*Info.matf2o[1][ii]) - MatX[2]*Info.matf2o[2][ii];
          cellmat[1][ii] = MatY[0]*Info.matf2o[0][ii] + 
            ProperY(MatY[1]*Info.matf2o[1][ii]) - MatY[2]*Info.matf2o[2][ii];
          cellmat[2][ii] = MatZ[0]*Info.matf2o[0][ii] + 
            ProperY(MatZ[1]*Info.matf2o[1][ii]) - MatZ[2]*Info.matf2o[2][ii];
        }


        xorg = XOffset - (int)(OrigCX*MatX[0]+OrigCY*MatX[1]+OrigCZ*MatX[2])
          + (int)((250.*Info.vecf2o[0])*MatX[0]+
            ProperY(250.*Info.vecf2o[1])*MatX[1]-
            (250.*Info.vecf2o[2])*MatX[2]);
        yorg = YOffset - (int)(OrigCX*MatY[0]+OrigCY*MatY[1]+OrigCZ*MatY[2])
          + (int)((250.*Info.vecf2o[0])*MatY[0]+
            ProperY(250.*Info.vecf2o[1])*MatY[1]-
            (250.*Info.vecf2o[2])*MatY[2]);
        zorg = ZOffset - (int)(OrigCX*MatZ[0]+OrigCY*MatZ[1]+OrigCZ*MatZ[2])
          + (int)((250.*Info.vecf2o[0])*MatZ[0]+
            ProperY(250.*Info.vecf2o[1])*MatZ[1]-
            (250.*Info.vecf2o[2])*MatZ[2]);

        dxx = (int)(250.*cellmat[0][0]);
        dxy = (int)(250.*cellmat[1][0]);
        dxz = (int)(250.*cellmat[2][0]);

        dyx = (int)(250.*cellmat[0][1]);
        dyy = (int)(250.*cellmat[1][1]);
        dyz = (int)(250.*cellmat[2][1]);

        dzx = (int)(250.*cellmat[0][2]);
        dzy = (int)(250.*cellmat[1][2]);
        dzz = (int)(250.*cellmat[2][2]);

        
    }

        /* Draw Unit Cell! */
        x = xorg;
        y = yorg;
        z = zorg;
        if( UseDepthCue )
        {   ClipDashVector(x,y,z,x+dxx,y+dxy,z+dxz,col,col,' ');
            ClipDashVector(x,y,z,x+dyx,y+dyy,z+dyz,col,col,' ');
            ClipDashVector(x,y,z,x+dzx,y+dzy,z+dzz,col,col,' ');
        } else
        {   ClipDashLine(x,y,z,x+dxx,y+dxy,z+dxz,col,col,' ');
            ClipDashLine(x,y,z,x+dyx,y+dyy,z+dyz,col,col,' ');
            ClipDashLine(x,y,z,x+dzx,y+dzy,z+dzz,col,col,' ');
        }
    
        x = xorg + dxx + dyx;
        y = yorg + dxy + dyy;
        z = zorg + dxz + dyz;
        if( UseDepthCue )
        {   ClipDashVector(x,y,z,x-dxx,y-dxy,z-dxz,col,col,' ');
            ClipDashVector(x,y,z,x-dyx,y-dyy,z-dyz,col,col,' ');
            ClipDashVector(x,y,z,x+dzx,y+dzy,z+dzz,col,col,' ');
        } else
        {   ClipDashLine(x,y,z,x-dxx,y-dxy,z-dxz,col,col,' ');
            ClipDashLine(x,y,z,x-dyx,y-dyy,z-dyz,col,col,' ');
            ClipDashLine(x,y,z,x+dzx,y+dzy,z+dzz,col,col,' ');
        }

        x = xorg + dxx + dzx;
        y = yorg + dxy + dzy;
        z = zorg + dxz + dyz;
        if( UseDepthCue )
        {   ClipDashVector(x,y,z,x-dxx,y-dxy,z-dxz,col,col,' ');
            ClipDashVector(x,y,z,x+dyx,y+dyy,z+dyz,col,col,' ');
            ClipDashVector(x,y,z,x-dzx,y-dzy,z-dzz,col,col,' ');
        } else
        {   ClipDashLine(x,y,z,x-dxx,y-dxy,z-dxz,col,col,' ');
            ClipDashLine(x,y,z,x+dyx,y+dyy,z+dyz,col,col,' ');
            ClipDashLine(x,y,z,x-dzx,y-dzy,z-dzz,col,col,' ');
        }

        x = xorg + dyx + dzx;
        y = yorg + dyy + dzy;
        z = zorg + dyz + dzz;
        if( UseDepthCue )
        {   ClipDashVector(x,y,z,x+dxx,y+dxy,z+dxz,col,col,' ');
            ClipDashVector(x,y,z,x-dyx,y-dyy,z-dyz,col,col,' ');
            ClipDashVector(x,y,z,x-dzx,y-dzy,z-dzz,col,col,' ');
        } else
        {   ClipDashLine(x,y,z,x+dxx,y+dxy,z+dxz,col,col,' ');
            ClipDashLine(x,y,z,x-dyx,y-dyy,z-dyz,col,col,' ');
            ClipDashLine(x,y,z,x-dzx,y-dzy,z-dzz,col,col,' ');
        }
    }
}


static void DisplayArea( void )
{    register int z;

    if( !UseSlabPlane )
    {   z = ImageRadius + ZOffset;
    } else z = SlabValue - 1;

    ClipDashLine(AreaX1,AreaY1,z,AreaX1,AreaY2,z,AxeCol,AxeCol,' ');
    ClipDashLine(AreaX1,AreaY2,z,AreaX2,AreaY2,z,AxeCol,AxeCol,' ');
    ClipDashLine(AreaX1,AreaY1,z,AreaX2,AreaY1,z,AxeCol,AxeCol,' ');
    ClipDashLine(AreaX2,AreaY1,z,AreaX2,AreaY2,z,AxeCol,AxeCol,' ');

}


static void DisplaySelected( void )
{
    register RAtom __far *s, __far *d;
    register Chain __far *chain;
    register Group __far *group;
    register Bond __far *bptr;
    register RAtom __far *aptr;
    register int irad,sc,dc;
    register int col;

    irad = (int)(Scale*20);
        
    if( CurRenderSet==CurMolSet )
    {   if( irad>0 )
        {   ForEachBond
            {   if( BondSelected &&
                    ( (bptr->srcatom == BondSelected->BSrcAtom && 
                         bptr->dstatom == BondSelected->BDstAtom) ||
                        (bptr->srcatom == BondSelected->BDstAtom &&
                         bptr->dstatom == BondSelected->BSrcAtom) ) )
                {   s = bptr->srcatom;  
                    sc = Shade2Colour(4);
                    d = bptr->dstatom;  
                    dc = Shade2Colour(4);
                    if( CurMolSet->visible )
                        ClipCylinder(s->x,s->y,s->z,d->x,d->y,d->z,
                          sc,dc,irad,bptr->altl,4*irad/5,bptr->cap);
                    else
                        ClipDashLine(s->x,s->y,s->z,d->x,d->y,d->z,
                            sc+ColourMask,dc+ColourMask,bptr->altl);
                } else
                {   s = bptr->srcatom;  
                    col = (s->flag&SelectFlag)? 1 : 0;
                    sc = Shade2Colour(col);

                    d = bptr->dstatom;  
                    col = (d->flag&SelectFlag)? 1 : 0;
                    dc = Shade2Colour(col);
                    if( CurMolSet->visible )
                        ClipCylinder(s->x,s->y,s->z,d->x,d->y,d->z,
                          sc,dc,irad,bptr->altl,4*irad/5,bptr->cap);
                    else
                        ClipDashLine(s->x,s->y,s->z,d->x,d->y,d->z,
                            sc+ColourMask,dc+ColourMask,bptr->altl);
                }
            }
        } else
        {   ForEachBond
            {   if( BondSelected &&
                    ( (bptr->srcatom == BondSelected->BSrcAtom && 
                         bptr->dstatom == BondSelected->BDstAtom) ||
                        (bptr->srcatom == BondSelected->BDstAtom &&
                         bptr->dstatom == BondSelected->BSrcAtom) ) )
                {   s = bptr->srcatom;  
                    sc = Shade2Colour(4);
                    d = bptr->dstatom;  
                    dc = Shade2Colour(4);
                    if( CurMolSet->visible )
                        ClipTwinLine(s->x,s->y,s->z,d->x,d->y,d->z,
                            sc+ColourMask,dc+ColourMask,bptr->altl);
                    else
                        ClipDashLine(s->x,s->y,s->z,d->x,d->y,d->z,
                            sc+ColourMask,dc+ColourMask,bptr->altl);
                } else
                {   s = bptr->srcatom;  
                    col = (s->flag&SelectFlag)? 1 : 0;
                    sc = Shade2Colour(col);

                    d = bptr->dstatom;  
                    col = (d->flag&SelectFlag)? 1 : 0;
                    dc = Shade2Colour(col);
                    if( CurMolSet->visible )
                        ClipTwinLine(s->x,s->y,s->z,d->x,d->y,d->z,
                            sc+ColourMask,dc+ColourMask,bptr->altl);
                    else
                        ClipDashLine(s->x,s->y,s->z,d->x,d->y,d->z,
                            sc+ColourMask,dc+ColourMask,bptr->altl);
                }
            }
        }


        irad = (int)(Scale*50);
        ForEachAtom
        if( aptr->flag&NonBondFlag )
        {   col = Shade2Colour( (aptr->flag&SelectFlag)? 1 : 0 );
            ClipSphere(aptr->x,aptr->y,aptr->z,irad,col);
        }

    } else
    {   sc = dc = Shade2Colour(2);
        if( irad>0 )
        {   ForEachBond
            {   s = bptr->srcatom;
                d = bptr->dstatom;
                if( CurMolSet->visible )
                    ClipCylinder(s->x,s->y,s->z,d->x,d->y,d->z,
                      sc,dc,irad,bptr->altl,4*irad/5,bptr->cap);
                else
                    ClipDashLine(s->x,s->y,s->z,d->x,d->y,d->z,
                        sc+ColourMask,dc+ColourMask,bptr->altl);
            } 
        } else 
            ForEachBond
            {   s = bptr->srcatom;
                d = bptr->dstatom;
                if( CurMolSet->visible )
                    ClipTwinLine(s->x,s->y,s->z,d->x,d->y,d->z,
                        sc+ColourMask,dc+ColourMask,bptr->altl);
                else
                    ClipDashLine(s->x,s->y,s->z,d->x,d->y,d->z,
                        sc+ColourMask,dc+ColourMask,bptr->altl);
            }

        irad = (int)(Scale*50);
        ForEachAtom
            if( aptr->flag&NonBondFlag )
                ClipSphere(aptr->x,aptr->y,aptr->z,irad,sc);
    }

    /* Rotation Center */
    if( CurMolSet==CenterSet )
    {   col = Shade2Colour(3);
        irad = 200;
        /*ClipStar(XOffset+(int)RCenx,YOffset+(int)RCeny,ZOffset+(int)RCenz,irad,col);*/
        ClipStar((int)(MOffset[0]+RCenx) + SRange,
                 (int)(MOffset[1]+RCeny) + HRange,
                 (int)(MOffset[2]+RCenz + DRange),irad,col);
    } else if( !CenterSet )
    {   if( (RotMode==RotAll) && (CurMolSet==MoleculeList) )
        {   col = Shade2Colour(3);
            irad = 200;
            ClipStar((int)(Wo[0]) + SRange,
                     (int)(Wo[1]) + HRange,
                     (int)(Wo[2] + DRange),irad,col);
        } else if( (RotMode==RotMol) && (CurMolSet==CurRenderSet) )
        {   col = Shade2Colour(3);
            irad = 200;
            ClipStar((int)(MOffset[0]+RCenx) + SRange,
                     (int)(MOffset[1]+RCeny) + HRange,
                     (int)(MOffset[2]+RCenz + DRange),irad,col);
        }
    }
}


static void RenderFrame( void )
{
    register Chain __far *chain;
 

    if( !DisplayMode && CurMolSet->visible )
    {   if( DrawAtoms ) 
            DisplaySpaceFill();
        if( DrawStars )
            DisplayStars();

        if( !UseSlabPlane || (SlabMode != SlabSection) )
        {   if( DrawBonds ) 
                {   if( DrawDoubleBonds )
                    {   DisplayDoubleBonds();
                    } else DisplayWireframe();
                }

            if( DrawRibbon )
            for( chain=Database->clist; chain; chain=chain->cnext )
                if( chain->glist )
                            DisplayRibbon( chain );
    
            if( DotPtr ) DisplaySurface();
            if( LabelList ) DisplayLabels();
            if( MonitList ) DisplayMonitors();
            DisplayHBonds( Database->slist, SSBondMode );
            DisplayHBonds( Database->hlist, HBondMode );
            DisplayBackbone();
        }

        if( DrawArea)
            DisplayArea();
        DisplayBoxes();

    } else if( DisplayMode )
    {   DisplaySelected();
        if( DrawArea)
            DisplayArea();
        DisplayBoxes();
    }
    
    if( DrawWorldAxes )
        DisplayWorldAxes();
}


void DrawFrame( void )
{
    register double temp;

    if( !Database ) 
        return;

    if( !CapClean )
        CreateFlatCap();

    if( !DisplayMode )
    {   if( UseShadow && DrawAtoms )
            if( !VoxelsClean )
                CreateVoxelData( SphereFlag );
    }

    if( UseSlabPlane )
    {   SlabValue = (int)(WClip[0] + DRange);
        if( LightZComp >= 0 )
            SlabInten = (int)(ColourMask*LightZComp/LightLength);
        else
            SlabInten = 0;
        SliceValue = SlabValue+16;
        UseClipping = True;
    } else
        UseClipping = UseScreenClip;

#ifdef MSWIN
    /* Lock Buffers into Memory */
    FBuffer = (Pixel __huge*)GlobalLock(FBufHandle);
    DBuffer = (short __huge*)GlobalLock(DBufHandle);
#endif

    /* Common View Elements */
    View.yskip = XRange;
    View.ymax = YRange;

    if( UseStereo )
    {
        /* Inversion left-right starting rastop 2.0 */
        temp = - StereoAngle/180.0;

        /* Create 'Right' View structure */
        View.fbuf = FBuffer+WRange;
        View.dbuf = DBuffer+WRange;
        View.xmax = WRange;

        WRotValue[1] -= temp;
        ReDrawFlag |= RFRotateY;
        PrepareWorldRotate(0);
        ApplyTransform();
        RenderFrame();

        /* Create 'Left' View structure */
        View.fbuf = FBuffer;
        View.dbuf = DBuffer;
        View.xmax = WRange;

        WRotValue[1] += temp;
        PrepareWorldRotate(3);
        ApplyTransform();
        RenderFrame();

    } else /* Mono */
    {   /* Create 'Mono' View structure */
        View.fbuf = FBuffer;
        View.dbuf = DBuffer;
        View.xmax = XRange;
        RenderFrame();
    }

#ifdef MSWIN
    /* Unlock Buffers */
    GlobalUnlock(FBufHandle);
    GlobalUnlock(DBufHandle);
#endif
    DBClear = False;
    FBClear = False;
}


static void TestAtomProximity(  RAtom __far *ptr, int xpos, int ypos )
{
    register Long dist;
    register int dx,dy;

    if( !ptr) return;

    if( UseSlabPlane && (ptr->z>SlabValue) )
        return;

    if( UseDepthPlane && (ptr->z<0) )
        return;

    dx = ptr->x - xpos;
    dy = ptr->y - ypos;

    dist = (Long)dx*dx + (Long)dy*dy;

    if( IdentFound )
    {   if( dist==IdentDist )
        {   if( ptr->z<IdentDepth )
                return;
        } else if( dist>IdentDist ) 
            return;
    }

    IdentDepth = ptr->z;
    IdentFound = True;
    IdentDist = dist;
    QAtom = ptr;
}


void IdentifyAtom( int xpos, int ypos )
{
    register int rad, wide, dpth;
    register int new, dx, dy, dz;
    register Chain __far *chain;
    register Group __far *group;
    register HBond __far *hptr;
    register RAtom  __far *aptr;
    register Bond __far *bptr;
    register MolSet __far *mptr;

    /* Reset Search */
    QMolSet = (void __far*)0;
    QChain = (void __far*)0;
    QGroup = (void __far*)0;
    QAtom = (void __far*)0;
    QXCoor = 0.0;
    QYCoor = 0.0;
    QZCoor = 0.0;
    IdentFound = False;

if(MoleculeList != CurMolSet)
{
	CurMolSet = CurMolSet;
}

    if( !MoleculeList )  
       return;

    if( UseStereo )
        if( xpos>WRange )
            return;

    StoreMoleculeData( CurMolSet );

    /* Check all molecules */
    for( mptr=MoleculeList; mptr; mptr=mptr->next )
    {   RestoreMoleculeData( mptr );

    /* Reset Search */
    QMolSet = (void __far*)0;
    QChain = (void __far*)0;
    QGroup = (void __far*)0;
    QAtom = (void __far*)0;
    QXCoor = 0.0;
    QYCoor = 0.0;
    QZCoor = 0.0;
    IdentFound = False;

    if( !DisplayMode )
    {   if( !UseSlabPlane || (SlabMode != SlabSection) )
        {   for( chain=Database->clist; chain; chain=chain->cnext )
                for( group=chain->glist; group; group=group->gnext )
                {   if( group->flag & DrawKnotFlag )
                    {   if( IsProtein(group->refno) )
                        {   aptr = FindGroupAtom(group,1);
                        } else
                            aptr = FindGroupAtom(group,7);
                        if( aptr )
                            TestAtomProximity(aptr,xpos,ypos);
                    }
                }

            /* Double tolerance for spline knots! */
            if( IdentFound ) IdentDist >>= 1;
    
            if( DrawBonds )
            ForEachBond
                if( bptr->flag&DrawBondFlag )
                {   TestAtomProximity(bptr->srcatom,xpos,ypos);
                    TestAtomProximity(bptr->dstatom,xpos,ypos);
                }
    
            ForEachBack
                if( bptr->flag&DrawBondFlag )
                {   TestAtomProximity(bptr->srcatom,xpos,ypos);
                    TestAtomProximity(bptr->dstatom,xpos,ypos);
                }
    
            for( hptr=Database->hlist; hptr; hptr=hptr->hnext )
            if( hptr->flag )
            {   if( HBondMode )
                {   TestAtomProximity(hptr->srcCA,xpos,ypos);
                    TestAtomProximity(hptr->dstCA,xpos,ypos);
                } else
                {   TestAtomProximity(hptr->src,xpos,ypos);
                    TestAtomProximity(hptr->dst,xpos,ypos);
                }
            }
    
            for( hptr=Database->slist; hptr; hptr=hptr->hnext )
            if( hptr->flag )
            {   if( HBondMode )
                {   TestAtomProximity(hptr->srcCA,xpos,ypos);
                    TestAtomProximity(hptr->dstCA,xpos,ypos);
                } else
                {   TestAtomProximity(hptr->src,xpos,ypos);
                    TestAtomProximity(hptr->dst,xpos,ypos);
                }
            }
    
        }

         ForEachAtom
        {   /* Identify bond! */
            if( aptr==QAtom )
            {   QChain = chain;
                QGroup = group;
            }

            if( aptr->flag & (SphereFlag | StarFlag) )
            {   dy = AbsFun(aptr->y-ypos);
                if( dy>aptr->irad ) continue;
                rad = LookUp[aptr->irad][dy];
                dx = AbsFun(aptr->x-xpos);
                if( dx>rad ) continue;
    
                new = False;
                dpth = aptr->z+LookUp[rad][dx];

                if( UseDepthPlane && ( (dpth<=0)||SlabValue<0 ) ) 
                    continue;

                if( UseSlabPlane && (aptr->z+rad>=SlabValue) )
                {   dz = SlabValue-aptr->z;
                    if( SlabMode && (dz >= -rad) )
                    {   wide = LookUp[aptr->irad][AbsFun(dz)];
                        if( (dy<=wide) && (dx<=(int)(LookUp[wide][dy])) )
                        {   if( SlabMode == SlabFinal )
                            {   dpth = SliceValue;
                                new = True;
                            } else if( SlabMode == SlabHollow )
                            {   dpth = aptr->z-LookUp[rad][dx];
                                if( dpth<0 ) continue;
                                new = !IdentFound || (dpth>IdentDepth);
                            } else if( SlabMode != SlabHalf )
                            {/* SlabClose, SlabSection */
                            dpth = dx*dx+dy*dy+dz*dz+SliceValue;
                            if( IdentFound )
                            {   new = (IdentDepth<SliceValue) || (dpth<IdentDepth);
                            } else new=True;
                            }
                            } else if( (dz>0) && (SlabMode!=SlabSection) )
                                new = !IdentFound || (dpth>IdentDepth);
                        }
                    } else if( !UseSlabPlane || (SlabMode != SlabSection) )
                        new = !IdentFound || IdentDist || (dpth>IdentDepth);
    
                    if( new )
                    {   IdentFound = True;
                        IdentDepth = dpth;
                        IdentDist = 0;
            
                        QChain = chain;
                        QGroup = group;
                        QAtom = aptr;
                    }
                } 
            }
    } else /* Display Mode */
    {   ForEachAtom
        {   TestAtomProximity(aptr,xpos,ypos);
            /* Identify bond! */
            if( aptr == QAtom )
            {   QChain = chain;
                QGroup = group;
            }
        }
    }

    if( !IdentFound || (IdentDist>=50) )
    {   /* Reset Pick Atom! */
        QChain = (void __far*)0;
        QGroup = (void __far*)0;
        QAtom = (void __far*)0;
        QXCoor = 0.0;
        QYCoor = 0.0;
        QZCoor = 0.0;
    } else break;
    }

    RestoreMoleculeData( CurMolSet );
    QMolSet = mptr;
    if( QMolSet )
        GetCurrentCoord(SysCoor,QMolSet,QAtom,&QXCoor,&QYCoor,&QZCoor);
    return;
}


void SetPickMode( int mode )
{
    PickCount = 0;
    if( PickMode==mode )
        return;

    if( (mode==PickAtom)||(mode==PickGroup)||(mode==PickChain) )
        StorePrevSelection();

    if( mode==PickRBond )
    {    if( !BondSelected )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(RenNoBdSel));
            return;
        }
    }

    /* Set Mode */
    PickMode = mode;

    if( (PickMode==PickNone)||(PickMode==PickIdent) )
       CursorMode = CURRasmol;
    else
       CursorMode = CURPicking;

    if ( PickMode == PickDist ||
         PickMode == PickAngle || 
         PickMode == PickTorsn ) 
    {   ReDrawFlag |= RFRefresh;
        DeleteTempMonitors();
    }

    if ( mode == PickDist ||
         mode == PickAngle ||
         mode == PickTorsn || 
         mode == PickMonit ) 
    {   DrawMonitDistance = True;
        ReDrawFlag |= RFRefresh;
    }

}


static void DescribeAtom( AtomRef *ptr, int flag )
{
    register char *str;
    register int i,ch;
    char buffer[40];

    str = Residue[ptr->grp->refno];
    for( i=0; i<3; i++ )
        if( str[i]!=' ' ) 
             WriteChar(str[i]);

    sprintf(buffer,"%d",ptr->grp->serno);
    WriteString(buffer);

    ch = ptr->chn->ident;
    if( ch != ' ' )
    {   if( isdigit(ch) )
            WriteChar(':');
        WriteChar(ch);
    }

    WriteChar('.');
    str = ElemDesc[ptr->atm->refno];
    for( i=0; str[i] && i<12; i++ )
        if( str[i]!=' ' ) 
             WriteChar(str[i]);

    if( flag )
    {   sprintf(buffer," (%ld)",ptr->atm->serno);
        WriteString(buffer);
    }
}


int PickAtoms( int shift, int xpos, int ypos )
{
    register AtomRef *ptr;
    register Label *label;
    register double temp;
    register char *str;
    register size_t len;

    char buffer[80];
    AtomRef ref;

    if( PickMode==PickNone )
        return False;

    /* No Picking for Hidden Molecule in Normal Mode */
    if( PickMode!=PickMolSet )
        if( !DisplayMode && !CurMolSet->visible )
            return False;

    IdentifyAtom(xpos,ypos);

    if( !QAtom )
        return False;

    if( PickMode==PickCentr && shift )
        SetPickMode(PickOrign);

    /*=========================================*/
    /* Coordinates Retrieval for all Molecules */
    /*=========================================*/
    if( PickMode == PickIdent || PickMode == PickCoord )
    {   InvalidateCmndLine();

        if( MoleculeList && MoleculeList->next )
        {   WriteString(MsgStrs(RenMol2));
            sprintf(buffer,"%s",QMolSet->title);
            WriteString(buffer);
            if( QMolSet->copynum!=1 )
                sprintf(buffer," #%d  ",QMolSet->copynum);
            else
                sprintf(buffer,"  ");
            WriteString(buffer);
        }

        WriteString(MsgStrs(RenAtom));
        str = ElemDesc[QAtom->refno];
        if( str[0]!=' ' )   WriteChar(str[0]);
        WriteChar(str[1]);  WriteChar(str[2]);
        {   int iii;
            for (iii = 3; str[iii] && iii < 12; iii++)
            {
                if( str[iii]!=' ' )   WriteChar(str[iii]);
            }
        }

        if( !(QAtom->altl == ' '))
        {   WriteChar(';');
            WriteChar(QAtom->altl);
        }

        sprintf(buffer," %ld  ",QAtom->serno);
        WriteString(buffer);

        if (!(QGroup->serno == -9999))
        {   str = Residue[QGroup->refno];
            if( QAtom->flag&HeteroFlag )
            {   WriteString(MsgStrs(RenHetero));
            } else WriteString(MsgStrs(RenGroup));

            if( str[0]!=' ' )  WriteChar(str[0]);
            WriteChar(str[1]); WriteChar(str[2]);

            sprintf(buffer," %d",QGroup->serno);
            WriteString(buffer);
            if( !(QGroup->insert == ' ') && !(QGroup->insert=='\0') )
                WriteChar(QGroup->insert);
        }

        if( QChain->ident!=' ' )
        {   WriteString(MsgStrs(RenChain));
            WriteChar(QChain->ident);
        }

        if( QAtom->model )
        {   WriteString(MsgStrs(RenModel));
            sprintf(buffer,"%d",QAtom->model);
            WriteString(buffer);
        }
        WriteChar('\n');

        if( PickMode == PickCoord || shift != 0 )
        {   if( SysCoor == WorldCoor )
                WriteString(MsgStrs(RenWorld));
            else if( SysCoor == MolCoor )
                WriteString(MsgStrs(RenMol));
            else if( SysCoor == ScreenCoor )
                WriteString(MsgStrs(RenScreen));
            WriteString(MsgStrs(RenCoord));
            sprintf(buffer, "%9.3f %9.3f %9.3f\n",QXCoor,QYCoor,QZCoor);
            WriteString(buffer);
        }
        return True;
    } else if( (QMolSet!=CurMolSet) && 
               !( (PickMode==PickMolSet) || 
                  (PickMode==PickCentr ) ||
                  (PickMode==PickOrign ) ) )
        return True;

    /*================*/
    /* Atom Selection */
    /*================*/
    if( PickMode==PickAtom )
    {   SelectAtom( shift, QAtom, QGroup );
        return True;

    /*=================*/
    /* Group Selection */
    /*=================*/
    } else if( PickMode == PickGroup )
    {   SelectGroup( shift, QGroup );
        return True;

    /*=================*/
    /* Chain Selection */
    /*=================*/
    } else if( PickMode == PickChain )
    {   SelectChain( shift, QChain );
        return True;

    /*====================*/
    /* Molecule Selection */
    /*====================*/
    } else if( PickMode == PickMolSet )
    {   if( QMolSet!=CurMolSet )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(RenMol));
            sprintf(buffer,"%s",QMolSet->title);
            WriteString(buffer);
            if( QMolSet->copynum!=1 )
            {   sprintf(buffer," #%d",QMolSet->copynum);
                WriteString(buffer);
            }
            WriteString(MsgStrs(RenSel));
            SelectMolecule(QMolSet,'\0',0);
        }
        return True;

    /*==========*/
    /* Labeling */
    /*==========*/
    } else if( PickMode == PickLabel )
    {   if( !QAtom->label )
        {   if( *LabelFormat!='\0' )
            {    len = strlen(LabelFormat);
                label = CreateLabel(LabelFormat,len);                    
            } else if( MainGroupCount > 1 )
            {   strcpy(buffer,"%n%r");
                str = buffer+4;
                if( Info.chaincount > 1 )
                {   if( isdigit(QChain->ident) )
                        *str++ = ':';
                    *str++ = '%';
                    *str++ = 'c';
                }
                strcpy(str,".%a");

                len = (str-buffer) + 3;
                label = CreateLabel(buffer,(int)len);
            } else label = CreateLabel("%e%i%A",6);

                QAtom->label = label;
                label->refcount++;
        } else
        {   DeleteLabel( (Label*)QAtom->label );
            QAtom->label = (void*)0;
        }
        ReDrawFlag |= RFRefresh;

    /*===========================*/
    /* Centre & Origin Selection */
    /*===========================*/
    } else if( (PickMode==PickCentr) || (PickMode==PickOrign) )
    {   
        if( CentreTransform(QMolSet, QAtom->xorg + QAtom->fxorg,
                QAtom->yorg + QAtom->fyorg,QAtom->zorg + QAtom->fzorg,
                PickMode==PickOrign) )
        {   
            ref.chn = QChain;
            ref.grp = QGroup;
            ref.atm = QAtom;

            InvalidateCmndLine();
            if( CenterSet )
            {   if( QMolSet!=CurMolSet )
                {   WriteString(MsgStrs(RenRot2));
                    sprintf(buffer,"%s ",QMolSet->title);
                    WriteString(buffer);
                    if( QMolSet->copynum!=1 )
                    {   sprintf(buffer,"#%d ",QMolSet->copynum);
                        WriteString(buffer);
                    }
                    WriteString(MsgStrs(RenAbout));
                } else
                    WriteString(MsgStrs(RenRot));
                DescribeAtom(&ref,True);
                WriteChar('\n');
            } else
            {   sprintf(buffer,MsgStrs(RenWOri));
                WriteString(buffer);
            }
        } else
        {   InvalidateCmndLine();
            WriteString(MsgStrs(RenNoCenter));
        }

    /*=================*/
    /* Monitor Drawing */
    /*=================*/
    } else if( PickMode == PickMonit )
    {   if( PickCount == 0 )
        {   PickHist[0].atm = QAtom;
            PickCount = 1;
        } else if( PickCount == 1 )
        {   if( !shift )
            {   if( PickHist[0].atm != QAtom )
                {   AddMonitors(PickHist[0].atm,QAtom,PickMonit);
                    ReDrawFlag |= RFRefresh;
                 }
                 PickCount = 2;
            } else PickHist[0].atm = QAtom;
        } else /* PickCount == 2 */
            if( !shift )
            {   PickHist[0].atm = QAtom;
                PickCount = 1;
            } else if( PickHist[0].atm != QAtom )   
            {   AddMonitors(PickHist[0].atm,QAtom,PickMonit);
                ReDrawFlag |= RFRefresh;
            }

    /*===============================*/
    /* Bond Modification & Selection */
    /*===============================*/
    } else if( PickMode == PickBond )
    {   if( PickCount == 2 )
            PickCount = 0;
        
        ptr = PickHist+PickCount;
        ptr->chn = QChain;
        ptr->grp = QGroup;
        ptr->atm = QAtom;
        PickCount++;
 
        InvalidateCmndLine();
        WriteString(MsgStrs(RenAtom2));
        WriteChar(PickCount+'0');
        WriteString(": ");
        DescribeAtom(ptr,True);
        WriteChar('\n');

        if( PickCount == 2 )
        {   if( PickHist[0].atm == QAtom )
            {   WriteString(MsgStrs(RenAtomDup));
                return False;
            }
            if( shift==-1 )
            ChangeBondOrder(PickHist[0].atm->serno, PickHist[1].atm->serno,-1,0);
            else if( shift==1 )        
            ChangeBondOrder(PickHist[0].atm->serno, PickHist[1].atm->serno,1,0);
            if( shift ) ReDrawFlag |= RFRefresh;
            SetBondAxis(PickHist[0].atm, PickHist[1].atm);
        }

                
    /*============================*/
    /* Distance, Angle or Torsion */
    /*============================*/
    } else if( (PickMode==PickDist)||(PickMode==PickAngle)||(PickMode==PickTorsn) )
    {   if( PickCount )
        {   if( shift )
            {   PickCount--;
            } else if( PickCount == PickMode )
                PickCount = 0;
        }

        ptr = PickHist+PickCount;
        ptr->chn = QChain;
        ptr->grp = QGroup;
        ptr->atm = QAtom;
        PickCount++;

        InvalidateCmndLine();
        WriteString(MsgStrs(RenAtom2));
        WriteChar(PickCount+'0');
        WriteString(": ");
        DescribeAtom(ptr,True);
        WriteChar('\n');

        if( PickCount == PickMode )
        {   
            if ( PickMode != PickMonit )
                DeleteTempMonitors();
        
            if( PickMode == PickDist )
            {   temp = (double)CalcDistance(PickHist[0].atm,
                                           PickHist[1].atm);

                WriteString(MsgStrs(RenDist));
                DescribeAtom(PickHist,False);
                WriteChar('-');
                DescribeAtom(PickHist+1,False);
                sprintf(buffer,": %.3f\n",temp);
                WriteString(buffer);
                AddMonitors(PickHist[0].atm, QAtom, PickDist);
                ReDrawFlag |= RFRefresh;

            } else if( PickMode == PickAngle )
            {   temp = (double)CalcAngle(PickHist[0].atm,
                                        PickHist[1].atm,
                                        PickHist[2].atm);

                WriteString(MsgStrs(RenAngle));
                DescribeAtom(PickHist,False);
                WriteChar('-');
                DescribeAtom(PickHist+1,False);
                WriteChar('-');
                DescribeAtom(PickHist+2,False);
                sprintf(buffer,": %.1f\n",temp);
                WriteString(buffer);

                AddMonitors2(PickHist[0].atm, PickHist[2].atm,
                  PickHist[1].atm, (RAtom __far *)NULL,
                  (unsigned short) (temp*100), 128, PickAngle);
                ReDrawFlag |= RFRefresh;

           } else /* PickMode == PickTorsn */
            {   temp = (double)CalcTorsion(PickHist[0].atm,
                                          PickHist[1].atm,
                                          PickHist[2].atm,
                                          PickHist[3].atm);

                WriteString(MsgStrs(RenTorsion));
                DescribeAtom(PickHist,False);
                WriteChar('-');
                DescribeAtom(PickHist+1,False);
                WriteChar('-');
                DescribeAtom(PickHist+2,False);
                WriteChar('-');
                DescribeAtom(PickHist+3,False);
                sprintf(buffer,": %.1f\n",temp);
                WriteString(buffer);

                AddMonitors2(PickHist[0].atm, PickHist[3].atm,
                  PickHist[1].atm, PickHist[2].atm,
                  (unsigned short) (temp*100), 128, PickTorsn);
                ReDrawFlag |= RFRefresh;
           }
        }
    }
    return True;
}


void SetStereoMode( int enable )
{
    ReDrawFlag |= RFRefresh | RFTransX;
    StereoView = ViewLeft;
    UseStereo = enable;
    DetermineClipping();
}


void ResetRenderer( void )
{
    if( !VoxelsClean )
        ResetVoxelData();

    DrawAtoms = False;  MaxAtomRadius = 0;
    DrawBonds = False;  MaxBondRadius = 0;
    DrawStars = False;
    DrawRibbon = False;

    SlabMode = SlabClose;
    UseSlabPlane = False;
    WClip[0] = -999999.;
    WClip[1] = -999999.;
    UseDepthPlane = False;
    UseLabelCol = False;
    UseShadow = False;

    UseDepthCue = False;

    SSBondMode = False;
    HBondMode = False;
    DisplayMode = 0;

    DrawDoubleBonds = False;
    DrawBoundBox = False;
    DrawUnitCell = False;
    DrawAxes = False;
    DrawWorldAxes = False;

    SetStereoMode(False);
    StereoAngle = 6.0;
    
    /* Initialise ColConst and set default lighting! */
    ChangeLight( 0, LightX, LightY, LightZ );

}


static void InitialiseTables( void )
{
    register Byte __far *ptr;
    register unsigned int i,rad;

    ptr = Array;
    LookUp[0] = ptr;  *ptr++ = 0;
    LookUp[1] = ptr;  *ptr++ = 1;  *ptr++ = 0;
    
    for( rad=2; rad<MAXRAD; rad++ )
    {   LookUp[rad] = ptr;
        *ptr++ = (Byte)rad; 
        for( i=1; i<rad; i++ )
            *ptr++ = (Byte)(Rint(sqrt(1-(double)i*i/(rad*rad))*rad));
        *ptr++ = 0;    
    }
}


void InitialiseRenderer( void )
{   register int rad,maxval;

    FBuffer = (void __huge*)0;  
    DBuffer = (void __huge*)0;
    IBuffer = (void __far*)0;   
    YBucket = (void __far*)0;

#if defined(MSWIN) || defined(APPLEMAC)
    FBufHandle = NULL;
    DBufHandle = NULL;
#endif

#if defined(IBMPC) || defined(APPLEMAC)
    /* Allocate tables on FAR heaps */ 
    Array = (Byte __far*)_fmalloc(MAXTABLE*sizeof(Byte));
    LookUp = (Byte __far* __far*)_fmalloc(MAXRAD*sizeof(Byte __far*));
    HashTable = (void __far* __far*)_fmalloc(VOXSIZE*sizeof(void __far*));
    ColConst = (Card __far*)_fmalloc(MAXRAD*sizeof(Card));
    
    if( !Array || !LookUp || !HashTable || !ColConst )
    FatalRenderError("tables");
#else
    ColConst = ColConstTable;
#endif

    InitialiseTables();

    /* Initialise ColConst! */
    /* Empirical formula to reproduce original rasmol lighting! */
    for( rad=0; rad<MAXRAD; rad++ )
    {   maxval = (int)( LightLength*(double)(rad + 0.5 + (double)rad/70) ) + 4;
        ColConst[rad] = ((Card)ColourDepth<<ColBits)/maxval;
    }

    FreeItem = (Item __far*)0;
    PickMode = PickIdent;
    DrawArea = False;
    AreaX1 = AreaX2 = AreaY1 = AreaY2 = 0;

    RotMode = RotAll;
    FastDraw = False;
    RefreshState = True;
    DepthCueMode = 2;
    VoxelsClean = False;
    VoxelsDone = False;
    BucketFlag = False;
    RayCount = 0;

    /*Default lighting values*/
    LightX = 1;
    LightY = 1;
    LightZ = 2;

    ResetRenderer();
    ReSizeScreen();
}

#if defined RASTOPWIN
void UpdateRender(int Level)
{
//    register Item __far *ptr;
    
    if( Level==1 )
    {    if(YBucket) free(YBucket);
        if(IBuffer) free(IBuffer);
        return;
    } else if( Level==2 )
    {   
//        ptr =  FreeItem;
//        while(ptr != (Item __far*)0) {
//            FreeItem = FreeItem->list;
//            _ffree(ptr);
//            ptr =  FreeItem;
//        }            
        FreeItem = (Item __far*)0;
    } 
}
#endif
