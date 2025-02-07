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

/* transfor.h
 */

#define GroupAttr       0x00
#define ChainAttr       0x01
#define TempAttr        0x02
#define ChargeAttr      0x03
#define AltAttr         0x04
#define ModelAttr       0x05

#define MaskColourFlag  0x01
#define MaskRadiusFlag  0x02
#define MaskBothFlag    0x03

#define RibColInside    0x01
#define RibColOutside   0x02
#define RibColBoth      0x03

#ifdef EIGHTBIT
#define ColourDepth       16
#define ColourMask        15
#define AltlDepth         4
#ifdef APPLEMAC
#define LastShade         14
#else
#define LastShade         15
#endif
#else
#ifdef HICOLOR
#define ColourDepth       64
#define ColourMask        63
#define LastShade         63
#else
#define ColourDepth       32
#define ColourMask        31
#define LastShade         31
#endif
#define AltlDepth         8
#endif


#ifdef __esv
/* Evans & Sutherland Gamma! */
#define DefaultAmbient    0.05
#else
#define DefaultAmbient    0.4
#endif

typedef struct { 
        Long refcount;
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } ShadeDesc;

typedef struct {
        short col;
        short shade;
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } ShadeRef;


#ifdef IBMPC
#define BackCol    0
#define AxeCol     1
#define LabelCol   2
#define FirstCol   3
#endif

#ifdef APPLEMAC
#define BackCol    1
#define AxeCol     2
#define LabelCol   3
#define FirstCol   4
#endif

#if !defined(IBMPC) && !defined(APPLEMAC)
#define BackCol    5
#define AxeCol     6
#define LabelCol   7
#define FirstCol   8
#endif

#define Colour2Shade(x)  ((int)((x)-FirstCol)/ColourDepth)
#define Shade2Colour(x)  ((x)*ColourDepth+FirstCol)

/* Dot colours */
#define DotColNone    0
#define DotColAttrib  1
#define DotColPot     2


#ifdef TRANSFORM
ShadeDesc Shade[LastShade];
double RotX[3],RotY[3],RotZ[3];
double MOffset[3];
double MatX[3],MatY[3],MatZ[3];
double InvX[3],InvY[3],InvZ[3];
Long OrigCX,OrigCY,OrigCZ;
Long CenX, CenY, CenZ, ShiftS;
double RCenx, RCeny, RCenz;
double LastTX, LastTY, LastTZ;
double LastRX,LastRY,LastRZ;
double WLastTX, WLastTY, WLastTZ;
double WLastRX, WLastRY, WLastRZ;
double WTransValue[3];     
double WRotValue[3];
double WTransValueBack[3];
double WRotValueBack[3];
double WAxes[3][3];
double Zoom;
double Wo[3];

int FakeSpecular,SpecPower;
int ShadePower;
int BackR,BackG,BackB;
int DotR,DotG,DotB;
int LabR,LabG,LabB;
int AxeR,AxeG,AxeB;
int BoxCol;
int UseLabelCol;
int UseBackFade;
double Ambient;
int UseDotCol;
int DotCol;
int LightX, LightY, LightZ;
int LastSpecShade;

double Scale,MaxZoom;
double DScale,IScale;
Long SideLen,Offset;
Card WorldRadius,WorldSize,LocalRadius;
int XOffset,YOffset,ZOffset;
int UseScreenClip;
int ZoomRange;

int Hydrogens,HetaGroups;
int DrawAtoms,MaxAtomRadius;
int DrawBonds,MaxBondRadius;
int DrawStars;
int DrawRibbon;
int ZoneBoth;
int ModelInclude;

int ScaleCount;
ShadeRef ScaleRef[LastShade];
int AltlColours[AltlDepth];

#else
extern ShadeDesc Shade[LastShade];
extern double RotX[3],RotY[3],RotZ[3];
extern double MOffset[3];
extern double MatX[3],MatY[3],MatZ[3];
extern double InvX[3],InvY[3],InvZ[3];
extern Long OrigCX, OrigCY, OrigCZ;
extern Long CenX, CenY, CenZ, ShiftS;
extern double RCenx, RCeny, RCenz;
extern double LastTX, LastTY, LastTZ;
extern double LastRX,LastRY,LastRZ;
extern double WLastTX, WLastTY, WLastTZ;
extern double WLastRX, WLastRY, WLastRZ;
extern double WTransValue[3];     
extern double WRotValue[3];
extern double WTransValueBack[3];
extern double WRotValueBack[3];
extern double WAxes[3][3];
extern double Zoom;
extern double Wo[3];

extern int FakeSpecular,SpecPower;
extern int ShadePower;
extern int BackR,BackG,BackB;
extern int DotR,DotG,DotB;
extern int LabR,LabG,LabB;
extern int AxeR,AxeG,AxeB;
extern int BoxCol;
extern int UseLabelCol;
extern int UseBackFade;
extern double Ambient;
extern int UseDotCol;
extern int DotCol;
extern int LightX, LightY, LightZ;
extern int LastSpecShade;

extern double Scale,MaxZoom;
extern double DScale,IScale;
extern Long SideLen,Offset;
extern Card WorldRadius,WorldSize,LocalRadius;
extern int XOffset,YOffset,ZOffset;
extern int UseScreenClip;
extern int ZoomRange;

extern int Hydrogens,HetaGroups;
extern int DrawAtoms,MaxAtomRadius;
extern int DrawBonds,MaxBondRadius;
extern int DrawStars;
extern int DrawRibbon;
extern int ZoneBoth;
extern int ModelInclude;

extern int ScaleCount;
extern ShadeRef ScaleRef[LastShade];
extern int AltlColours[AltlDepth];

#endif

/* These define light source position */
#define LightDot(x,y,z)  ((LightX*(x))+InvertY(LightY*(y))+(LightZ*(z)))
#define ORIGINAL
#define LightLength 1024


void SetRadiusValue( int, int  );
void SetRadiusTemperature( int );
void SetVanWaalRadius( int, int );
void DisableSpacefill( void );
void SetHBondStatus( int, int, int, int );
void SetRibbonStatus( int, int, int );
void SetRibbonCartoons( void );
void SetTraceTemperature( void );
void EnableWireframe( int, int, int );
void EnableBackbone( int, int, int );
void DisableWireframe( void );
void DisableBackbone( void );

void SetSurfaceStatus( int, int, int );
void DisableSurface( int );


void DisplaySelectCount( void );
void SelectZoneExpr( Expr* );
void RestrictZoneExpr( Expr* );
void RestrictZone( int );
void SelectArea( int, int, int, int, int, int );
void SelectZone( int );
void SelectAtom( int, RAtom __far *, Group __far * );
void SelectGroup( int, Group __far * );
void SelectChain( int, Chain __far * );
void ResetPrevSelection( void );
void StorePrevSelection( void );
void SelectPrevSelection( int );

void ChangeLight( int, int, int, int );
int DefineShade( int, int, int );
void DefineColourMap();
void ResetColourMap();
void SetLutEntry( int, int, int, int );
int IsCPKColour( RAtom __far * );
int IsVDWRadius( RAtom __far * );

void ColourBackNone();
void ColourBondNone();
void ColourHBondType();
void ColourHBondNone( int );
void ColourRibbonNone( int );
void ColourMonitNone();
void ColourBackAttrib( int, int, int );
void ColourBondAttrib( int, int, int );
void ColourHBondAttrib( int, int, int, int );
void ColourRibbonAttrib( int, int, int, int );
void ColourMonitAttrib( int, int, int );
void ColourDotsAttrib( int, int, int );
void ColourBoxAttrib( int, int, int );
void ColourDotsPotential();
void MonoColourAttrib( int, int, int );
void ScaleColourAttrib( int );
void CPKColourAttrib();
void AminoColourAttrib();
void ShapelyColourAttrib();
void StructColourAttrib();
void UserMaskAttrib( int );

void DefaultRepresentation();

void DetermineClipping( void );
void InitialiseTransform( void );
void SetWorldSize( Card );
int GetWorldSize( void );
void InitialTransform( void );
void PrepareBondRotate( void );
void ReviseInvMatrix( void );
void PrepareWorldZoom( int );
void PrepareWorldRotate( int );
void PrepareMoleculeRotate( void );
void PrepareWorldTranslate( void );
void PrepareMoleculeTranslate( void );
void RMat2RV( double *, double *, double *, double[3], double[3], double[3] );
void ApplyTransform( void );
void DrawWorld( void );
void SetWorldBack( void );
void ResetWorldAxes( int );
void ResetMolSet( void );
void ResetTransform( void );

