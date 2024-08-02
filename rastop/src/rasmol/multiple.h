/* Original File Header:
 */
/**********************************************************************
  Copyright (c) 1995 UC Regents, ModularCHEM Consortium

  multiple.c
  Multiple Molecule Support for Rasmol
  
  Author:      Gary Grossman (garyg@cory.EECS.Berkeley.EDU)
  Last Update: November 11, 1995
 **********************************************************************/

/* multiple.h
 */

/* Original file header: */
/**********************************************************************
  Copyright (c) 1995 UC Regents, ModularCHEM Consortium

  multiple.h
  Multiple Molecule Support for Rasmol
  
  Author:      Gary Grossman (garyg@cory.EECS.Berkeley.EDU)
  Last Update: November 9, 1995
 **********************************************************************/

/* Coordinate Flag */
#define MolCoor     0
#define WorldCoor   1
#define AbsCoor     2
#define ScreenCoor  3

typedef struct _MolSet {
        struct _MolSet __far *next;       /* Linked list of Molecules  */
        char __far *data;                 /* Variable Storage          */
        char title[64];                   /* Molecule Title            */
        int copynum;                      /* Copy Number               */
        int format;                       /* DataFileFormat            */
        char __far *molfile;              /* Molecular File            */
        int visible;                      /* Visibility Flag           */
    } MolSet;


#ifdef MULTIPLE_C

MolSet __far *MoleculeList;
MolSet __far *CurMolSet;
MolSet __far *CurRenderSet;
MolSet __far *CenterSet;
char RSMPathName[1024];
char SpaceName[64];
int WorldScriptFormat;                    /* RSMFormat | DataFormat */
                                          /* RSMFormat only if RSM opened
                                             first or saved as RSM */
int FileModified;
MolSet __far *QMolSet;
double QXCoor;
double QYCoor;
double QZCoor;
int SysCoor;


#else

extern MolSet __far *MoleculeList;
extern MolSet __far *CurMolSet;
extern MolSet __far *CurRenderSet;
extern MolSet __far *CenterSet;
extern char RSMPathName[1024];
extern char SpaceName[64];
extern int WorldScriptFormat;
extern int FileModified;
extern MolSet __far *QMolSet;
extern double QXCoor;
extern double QYCoor;
extern double QZCoor;
extern int SysCoor;


#endif

void GetMOffset( MolSet __far *, void * );
void SetMOffset( MolSet __far *, void * );
void CreateNewMolSet( void );
void RecordMolFile( void );
void ChangeMolFile( char * );
void SetDirectory( void );
void SetTitle( void );
void ZapMolecule( void );
void ZapDatabase( int );
void RestoreMoleculeData( MolSet __far * );
void StoreMoleculeData( MolSet __far * );
void SwitchMolecule( MolSet __far * );
void SelectMolecule( MolSet __far *, char*, int );
int CentreTransform( MolSet __far *, int, int, int, int );
void ReRadius( void );
int IsWorldActive( void );
int GetMolsetIndex( MolSet __far * );
void GetCurrentCoord( int, MolSet __far *, RAtom __far *, double *, double *, double * );
double GetClipCoord( int, MolSet __far *, int);
double getWorldRadius( void );
void InitialiseMultiple( void );
