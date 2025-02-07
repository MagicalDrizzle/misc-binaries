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

/* tokens.h
 */

/* Lexeme Tokens */
#define IdentTok       256
#define NumberTok      257
#define FloatTok       258
#define StringTok      259

/* Command Tokens */
#define AdviseTok      260
#define BackboneTok    261
#define CartoonTok     262
#define CentreTok      263
#define ClipboardTok   264
#define ColourTok      265
#define ConnectTok     266
#define DashTok        267
#define DepthTok       268
#define DefineTok      269
#define DelayTok       270
#define DisplayTok     271
#define EchoTok        272
#define ExitTok        273
#define HelpTok        274
#define LabelTok       275
#define LoadTok        276
#define LoopTok        277
#define MoleculeTok    278
#define MonitorTok     279
#define MoveTok        280
#define PrintTok       281
#define QuitTok        282
#define RefreshTok     283
#define RenumTok       284
#define ResetTok       285
#define ResizeTok      286
#define RestoreTok     287
#define RestrictTok    288
#define RotateTok      289
#define SaveTok        290
#define ScriptTok      291
#define SelectTok      292
#define SetTok         293
#define ShowTok        294
#define SlabTok        295
#define SourceTok      296
#define SpacefillTok   297
#define StarTok        298
#define StructureTok   299
#define SymmetryTok    300
#define TitleTok       301
#define TraceTok       302
#define TranslateTok   303
#define ViewTok        304
#define WaitTok        305
#define WireframeTok   306
#define WriteTok       307
#define ZapTok         308
#define ZoomTok        309

/* Predicate Tokens */
#define IsPredTok(x)   (((x)>=310) && ((x)<=349))
#define PredTokOrd(x)  ((x)-310)
#define PredTokChr(x)  ((x)+310)

#define AlphaTok       310
#define AminoTok       311
#define ATTok          312
#define BondedTok      313
#define CGTok          314
#define CystineTok     315
#define DNATok         316
#define HelixTok       317
#define HeteroTok      318
#define HydrogenTok    319
#define IonTok         320
#define LigandTok      321
#define MainChainTok   322
#define NucleicTok     323
#define ProteinTok     324
#define PurineTok      325
#define PyrimidineTok  326
#define RNATok         327
#define SelectedTok    328
#define SheetTok       329
#define SidechainTok   330
#define SolventTok     331
#define TurnTok        332
#define WaterTok       333

#define AcidicTok      334
#define AcyclicTok     335
#define AliphaticTok   336
#define AromaticTok    337
#define BasicTok       338
#define BuriedTok      339
#define ChargedTok     340
#define CisBondedTok   341
#define CyclicTok      342
#define HydrophobicTok 343
#define LargeTok       344
#define MediumTok      345
#define NeutralTok     346
#define PolarTok       347
#define SmallTok       348
#define SurfaceTok     349


/* Property Tokens */
#define IsPropTok(x)   (((x)>=350) && ((x)<=356))
#define TemperatureTok 350
#define RadiusTok      351
#define AtomNoTok      352
#define ElemNoTok      353
#define ModelTok       354
#define ResNoTok       355
#define AltlTok        356

/* File Format Tokens */
/* Warning! Tokens are related to Format values */
#define IsMoleculeToken(x)  (((x)>=360) && ((x)<=375))

#define PDBTok         360
#define MacroModelTok  361
#define GaussianTok    362
#define AlchemyTok     363
#define NMRPDBTok      364
#define CharmmTok      365
#define BiosymTok      366
#define MOPACTok       367
#define SHELXTok       368
#define Mol2Tok        369
#define FDATTok        370
#define MMDBTok        371
#define MDLTok         372
#define XYZTok         373
#define CIFTok         374
#define CEXTok         375

/* Raster Tokens */
#define IsImageToken(x) (((x)>=380) && ((x)<=398) || ((x) == PhiPsiTok) )
#define GIFTok         380
#define PPMTok         381
#define SUNTok         382
#define SUNRLETok      383
#define EPSFTok        384
#define PICTTok        385
#define IRISTok        386
#define BMPTok         387
#define MonoPSTok      388
#define JPEGTok        389
#define PNGTok         390
#define VectPSTok      391
#define KinemageTok    392
#define MolScriptTok   393
#define POVRayTok      394
#define VRMLTok        396
#define RamachanTok    397  /* ok, this isn't a real image format ... */
#define RamPrintTok    398

/* Feature Tokens */
#define AtomTok        400
#define BondTok        401
#define DotsTok        402
#define HBondTok       403
#define RibbonTok      404
#define SSBondTok      405
#define Ribbon1Tok     406
#define Ribbon2Tok     407
#define UnBondTok      408

/* Expression Tokens */
#define TrueTok        410
#define FalseTok       411
#define AllTok         412
#define NoneTok        413
#define AndTok         414
#define OrTok          415
#define NotTok         416
#define WithinTok      417
#define XorTok         418

/* Colour Tokens */
/* Warning! Tokens are related to colour values */
#define IsColourToken(x) (((x)>=420) && ((x)<=443))
#define Token2Colour(x)  ((x)-420)

#define BlackTok       420
#define BlueTok        421
#define BlueTintTok    422
#define BrownTok       423
#define CyanTok        424
#define GoldTok        425
#define GrayTok        426
#define GreenTok       427
#define GreenBlueTok   428
#define GreenTintTok   429
#define HotPinkTok     430
#define MagentaTok     431
#define OrangeTok      432
#define PinkTok        433
#define PinkTintTok    434
#define PurpleTok      435
#define RedTok         436
#define RedOrangeTok   437
#define SeaGreenTok    438
#define SkyBlueTok     439
#define VioletTok      440
#define WhiteTok       441
#define YellowTok      442
#define YellowTintTok  443

#define CPKTok         444
#define ShapelyTok     445
#define ResidueTok     446
#define UserTok        447
#define GroupTok       448
#define ChainTok       449
#define TypeTok        450
#define PotentialTok   451
#define ChargeTok      452

/* Variable Tokens */
#define ShadePowerTok  459
#define AmbientTok     460
#define AxesTok        461
#define BackFadeTok    462
#define BackgroundTok  463
#define BondModeTok    464
#define BoundBoxTok    465
#define CisAngleTok    466
#define DepthCueTok    467
#define FontSizeTok    468
#define FontStrokeTok  469
#define HourGlassTok   470
#define MenusTok       471
#define MouseTok       472
#define PickingTok     473
#define ShadowTok      474
#define SlabModeTok    475
#define SpecularTok    476
#define SpecPowerTok   477
#define StrandsTok     478
#define TransparentTok 479
#define UnitCellTok    480

/* SlabMode Tokens */
#define RejectTok      481
#define HalfTok        482
#define HollowTok      483
#define SolidTok       484
#define SectionTok     485

/* MouseMode Tokens */
#define RasMolTok      486
#define InsightTok     487
#define QuantaTok      488
#define SybylTok       489

/* Information Tokens */
#define InfoTok        490
#define SequenceTok    491
#define VersionTok     492
#define PhiPsiTok      493

/* Display Mode Tokens */
#define NormalTok      495
#define StereoTok      496
#define MonoTok        497
#define HardwareTok    498

/* Axis Tokens */
#define XTok           500
#define YTok           501
#define ZTok           502

/* Picking Tokens */
#define IdentifyTok    505
#define CoordTok       506
#define DistanceTok    507
#define AngleTok       508
#define TorsionTok     509
#define OriginTok      510

/* Misc Tokens */
#define InLineTok      511
#define VDWTok         512
#define HeaderTok      513
#define CIFDataTok     514
#define FSTok          515
#define PSTok          EPSFTok

/* Clipboard Tokens */
#define ImageTok       516
#define PositionTok    517
#define CopyTok        518
#define PasteTok       519
#define EmptyTok       520

/* Other Misc. */
#define LightSourceTok 521
#define DefaultTok     522
#define PreviousTok    523
#define HTMLTok        524
#define PrefsTok       525
#define WorldTok       526
#define HideTok        527
#define AddTok         528
#define AbsoluteTok    529
#define SizeTok        530
#define ScreenTok      531
#define WorldDepthTok  532
#define ScaleTok       533


/* Bond Tokens */
#define SingleTok      600
#define DoubleTok      601
#define TripleTok      602


/* Language Tokens */
#define LangTok        620
#define EnglishTok     621
#define FrenchTok      622
#define GermanTok      623
#define ItalianTok     624
#define SpanishTok     625

/* GUI */
#define WindowTok      640
#define MainTok        641
#define CommandPanelTok 642
#define CommandLineTok  643
#define SelectionTok   644
#define MaximizedTok   645
#define StatusTok      646



int LookUpKeyword( char* );

