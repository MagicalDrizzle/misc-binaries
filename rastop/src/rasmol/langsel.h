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

#define MaxStrFlag     289  // Please, adjust!


enum {

/**********
 * RasMol *
 **********/

/* Command Errors */
        ErrSyntax    ,    
        ErrBigNum    ,    
        ErrBadOpt    ,    
        ErrParam     ,     
        ErrFilNam    ,    
        ErrBadLoad   ,   
        ErrNotNum    ,    
        ErrNotSep    ,    
        ErrNotBrac   ,   
        ErrNoCol     ,     
        ErrColour    ,    
        ErrBadArg    ,    
        ErrBadExpr   ,   
        ErrParen     ,     
        ErrScript    ,    
        ErrFunc      ,      
        ErrSetName   ,   
        ErrBadSet    ,    
        ErrInScrpt   ,   
        ErrOutScrpt  ,  
        ErrBadMolDB  ,  
        ErrNoBond    ,    
        ErrBlocSel   ,   

  /* File IO */
        StrLoadFile  ,
        StrErrFile   ,   
        StrCFmt      ,      
        StrDcmp      ,      
        StrErrNoFile , 
        StrErrSave   ,   
        StrSavWorld  ,  
        StrErrNoName , 
        StrNoMolSave , 
        StrErrSavRSM , 
        StrSavRSMOK  ,  
        StrErrSav    ,    
        StrErrWorld  ,  
        StrFailLoad  ,  
        StrDFile     ,     

  /* Script */
        StrScrDone   ,   
        StrSLong     ,     
        StrTooManyErr,
        StrScrStop   ,   
        StrSFile     ,     
        StrNoScrSave , 
        StrInterScr  ,  
        StrUnLight   ,   

        StrSMem      ,      

        StrCTerm     ,     
        StrCLong     ,     
        StrFNum      ,      
        StrCent      ,      
        StrCClip     ,     
        StrPClip     ,     
        StrNoClip    ,    

        StrNPrint    ,    
        StrNoPrint   ,   

        StrUnrec     ,     
        StrIgnore    ,    
        StrRCLong    ,    
        StrILong     ,     
        StrMolNam    ,    
        StrClass     ,     
        StrSecSt     ,     
        StrNoAsmt    ,    
        StrPDBRec    ,    
        StrCalc      ,      
        StrDBCode    ,    
        StrExpTec    ,    
        StrNumChn    ,    
        StrNumGrp    ,    
        StrNumAtm    ,    
        StrNumBnd    ,    
        StrNumBrg    ,    
        StrNumHbd    ,    
        StrNumHel    ,    
        StrNumStrnd  ,  
        StrNumTrn    ,    
        StrMalloc    ,    
        StrXSRes     ,     
        StrXSAtyp    ,    

  /* Object Description */
        StrObjModel  ,  
        StrObjGroup  ,  
        StrObjChain  ,  
        StrNoGroupSel,
        StrCurSel    ,    
        StrDscbSeq   ,   
        StrSeqModel  ,
        StrSeqChain  ,
        StrDesSym    ,
        StrUCell     ,     
        StrSGroup    ,    
        StrSymm      ,      
        StrMolSel    , 
        
        StrWrdlWrdl  ,
        StrWrdlDegr  ,
        StrWrdlPos   ,
        StrWrldRot   ,
        StrWrldZoom  ,
        StrWrdlScal  ,
        StrWrdlDDph  ,
        StrWrdlCDhp  ,
        StrWrldSlb   ,
        StrWrldDpth  ,
        StrWrldMol   ,
        StrWrldInd   ,
        StrWrldMol2  ,
        StrWrldAsbPar,
        StrWrldPos2  ,
        StrWrldXYZ   ,
        StrWrldRot2  ,
        StrWrldInd2  ,
        StrWrldSPar  ,
        StrWrldWPar  ,
           
        StrShowMol   , 
        StrCenWOri   ,
        StrCenMOri   ,
        StrCenCen    ,
        StrAbsVal    ,
        StrScreenVal ,
        StrRotateX   ,
        StrRotateY   ,
        StrRotateZ   ,
        StrRotBond   ,
        StrTransX    ,
        StrTransY    ,
        StrTransZ    ,
        StrZoom      ,
        StrWDepthDef ,
        StrWDepthCur ,
        StrWrongShow ,

        StrPickLabel , 

  /* RasMol Fatal Exit */
        RfeNoMem     ,     
        RfeNoCurMol  ,  
        RfeMemLang   ,   
        RfeSavDoc    ,    
        RfeHROpn     ,     
        RfeCmdMem    , 
        RfeDatabase  ,
        RfeNewRes    ,   
        RfeNewAtoms  , 

  /* brotate */
        RotDupSer    ,  
        RotAtSer     ,
        RotGraph     ,     
        RotNoRot     ,     
        RotNoSel     ,     
        RotSelOk     ,     

  /* infile */
        FilInvCrys   ,
        FilInvScal   ,
        FilInvZmat   ,
        FilCIFa      ,   
        FilCIFb      ,   
        FilCIFMat    , 
        FilCIFCart   ,
        FilStrCon    , 
        FilGeoBon    , 
        FilNoFil     ,  

  /* molecule */
        MolMolNam    ,   
        MolClass     ,     
        MolSecStr    ,   
        MolNoAss     ,     
        MolPDB       ,         
        MolCalcul    ,   
        MolDatCod    ,   
        MolExpTech   ,  
        MolChainNb   ,   
        MolGrpNb     ,     
        MolAtomNb    ,   
        MolBondNb    ,   
        MolNumBr     ,    
        MolNumHB     ,     
        MolNumHel    ,   
        MolNumStr    ,   
        MolNumTurn   ,  
        MolNumBds    ,   
        MolSerNb     ,    
        MolErrAtSer  , 
        MolPrevRot   ,  
        MolNextRot   ,  
        MolNoRotBnd  , 
        MolNoBndRem  , 
        MolPlease    ,   

  /* multiple */
        MulDataNF    ,    
        MulMisEnd    ,    
        MulSeeHelp   ,   
        MulErrNoNam  ,  
        MulErrNoInd  ,  
        MulNoTrans   ,   

  /* output */
        OutNoFile    ,    
        Out8bitGIF   ,   
        Out8bitBMP   ,   
        OutPSObj     ,     
        OutNoMemPS   ,   
        OutNoMem     ,     

  /* prefer */
        PrfNotFound  ,  
        PrfResetDef  ,  
        PrfLoaded    ,    
        PrfCreate    ,    

  /* render */
        RenNoBdSel   ,   
        RenMol2      ,
        RenAtom      ,      
        RenHetero    ,    
        RenGroup     ,     
        RenChain     ,     
        RenModel     ,  
        RenWorld     ,     
        RenMol       ,       
        RenSel        ,    
        RenScreen    , 
        RenCoord     ,  
        RenAbout     ,  
        RenRot       ,    
        RenRot2      ,   
        RenWOri      ,   
        RenNoCenter  ,
        RenAtom2     ,  
        RenAtomDup   ,
        RenDist      ,      
        RenAngle     ,     
        RenTorsion   ,   

  /* repress */
        RepErrMem    ,  
        RepErrLab    ,  
        RepErrMon    ,  
        RepErrDSur   , 
        RepErrConB   , 
        RepErrConC   , 
        RepErrConT   , 
        RepErrTabl   , 
        RepErrVect   , 
        RepErrPrb    ,  

        RepDupAtom   ,   
        RepAtomSer   ,   
        RepAllDot    ,  
        RepOneDot    ,    
        RepNoDot     ,     

  /* script */           
        ScrNoFile    ,    

  /* transfor */
        TrnNoAtSel   ,   
        TrnOneAtSel  ,  
        TrnManAtSel  ,
        TrnNoShade   ,   
        TrnNoPot     ,     
        TrnNoCol     ,  
        
  /* Help */
        HlpHFil      ,      
        HlpHTop      ,   
        HlpHNone     ,     
        HlpHTMLHelp  ,  


  /* langsel */
        LANNoLoc     ,
        LANNoFile    , 


/*************
 * RasTopWin *
 *************/

  /* datadisplay */
        RTPErrScrData,

  /* dialog */
        RTPErrChar   ,   
        RTPErrStrd   ,   
        RTPNoClip    ,    
        RTPNoFile    ,    
        RTPSaveData  ,    
        RTPErrDialog , 
        RTPErrMem    ,    
        RTPErrPath   ,   

  /* mainframe */
        RTPWCoor     ,     
        RTPSCoor     ,     
        RTPACoor     ,     
        RTPMCoor     ,     
        RTPStopScr   ,   

  /* rasgraphics */
        RTPZeroAt    ,    
        RTPOneAt     ,  
        RTPManyAt    ,
        RTPPosClip   ,   
        RTPNoMemSel  ,  
        RTPNoDataClip,
        RTPNoComdClip,
        RTPUnkCmd    ,    
        RTPNoSelClip , 
        RTPOneSelClip,
        RTPManSelClip,
        RTPPastePos  ,  
        RTPScrPaste  ,  
        RTPSelBig    ,

  /* rasdoc - rasview */
        RTPImgBig    ,    
        RTPNoWind    ,    
        RTPErrDat    ,    
        RTPNoSave    , 
        RTPRsmLoad   ,   
        RTPScrLoad   , 
        RTPScrBox    ,
        RTPScrInter  ,  
        RTPScrDone   ,   
        RTPErrImg    ,    
        RTPZoomFac   ,   
        RTPScrLong   , 
        RTPSelMol    ,
        RTPSelDir    ,
        
};

typedef struct {
    char *    msg;
    char * msgtag;
    char * engmsg;
} langstr;



#ifdef LANGSEL


#else


#endif


void InitialiseLangSel( void );
void ExitLangSel( void );
char * MsgStrs( int );
void FormatMsg( char *, int, char * );
void SwitchLang( char * );
