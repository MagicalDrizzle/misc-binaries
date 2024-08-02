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

#define LANGSEL

#include "rasmol.h"

#ifdef IBMPC
#include <windows.h>
#include <shellapi.h>
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
#if defined(IBMPC) || defined(VMS) || defined(APPLEMAC)
#include "string_case.h"
#else
#include <strings.h>
#endif
#include <ctype.h>
#include <stdio.h>

#if !defined(IBMPC) && !defined(VMS) && !defined(APPLEMAC)
#include <pwd.h>
#endif

#include "langsel.h"
#include "cmndline.h"
#include "help.h"
//#include <string.h>

static langstr langstrs[] = {

/**********
 * RasMol *
 **********/

  /* Command Errors */
  { (void __far*)0, "ErrSyntax",     "Invalid command syntax!\n",                                  },
  { (void __far*)0, "ErrBigNum",     "Parameter value too large!\n",                               },
  { (void __far*)0, "ErrBadOpt",     "Invalid parameter setting!\n",                               },
  { (void __far*)0, "ErrParam",      "Invalid parameter name!\n",                                  },
  { (void __far*)0, "ErrFilNam",     "Filename string expected!\n",                                },
  { (void __far*)0, "ErrBadLoad",    "Molecule database loaded!\n",                                },
  { (void __far*)0, "ErrNotNum",     "Integer value expected!\n",                                  },
  { (void __far*)0, "ErrNotSep",     "Comma separator missing!\n",                                 },
  { (void __far*)0, "ErrNotBrac",    "Close bracket ']' expected!\n",                              },
  { (void __far*)0, "ErrNoCol",      "No colour specified!\n",                                     },
  { (void __far*)0, "ErrColour",     "Unknown or incorrect colour!\n",                             },
  { (void __far*)0, "ErrBadArg",     "Invalid command argument!\n",                                },
  { (void __far*)0, "ErrBadExpr",    "Syntax error in expression!\n",                              },
  { (void __far*)0, "ErrParen",      "Close parenthesis ')' expected!\n",                          },
  { (void __far*)0, "ErrScript",     "Script command stack too deep!\n",                           },
  { (void __far*)0, "ErrFunc",       "Open parenthesis '(' expected!\n",                           }, 
  { (void __far*)0, "ErrSetName",    "Invalid or missing atom set name!\n",                        },
  { (void __far*)0, "ErrBadSet",     "Not enough memory to define set!\n",                         },
  { (void __far*)0, "ErrInScrpt",    "Command disabled in script file!\n",                         },
  { (void __far*)0, "ErrOutScrpt",   "Command invalid (valid only within a script)!\n",            },
  { (void __far*)0, "ErrBadMolDB",   "Molecule database not loaded!\n",                            },
  { (void __far*)0, "ErrNoBond",     "Bond for rotation not picked!\n",                            },
  { (void __far*)0, "ErrBlocSel",    "Invalid selection bloc!\n",                                  },

  /* File IO */
  { (void __far*)0, "StrLoadFile",   "Loading molecule ",                                          },
  { (void __far*)0, "StrErrFile",    "Error: File '$' not found!\n",                               },
  { (void __far*)0, "StrCFmt",       "Error: Unrecognised compression format!\n",                  },
  { (void __far*)0, "StrDcmp",       "Error: Unable to decompress file!\n",                        },
  { (void __far*)0, "StrErrNoFile",  "Error: Unable to create file!\n"                             },
  { (void __far*)0, "StrErrSave",    "Error: Failure in saving molecular data!\n"                  },
  { (void __far*)0, "StrSavWorld",   "Error: World saving interrupted!\n"                          },
  { (void __far*)0, "StrErrNoName",  "Error: file name missing!\n"                                 },
  { (void __far*)0, "StrNoMolSave",  "Error: Molecular data not recorded!\n"                       },
  { (void __far*)0, "StrErrSavRSM",  "Error: Unable to save RSM script!\n",                        },
  { (void __far*)0, "StrSavRSMOK",   "Saving successful!\n",                                       },
  { (void __far*)0, "StrErrSav",     "Error: Unable to save file!\n",                              },
  { (void __far*)0, "StrErrWorld",   "Error: Failure in opening world\n",                          },
  { (void __far*)0, "StrFailLoad",   "Failure in loading molecular file\n",                        },
  { (void __far*)0, "StrDFile",      "Cannot open dots file '$'\n",                                },

  /* Script */
  { (void __far*)0, "StrScrDone",    "Script executed!\n"                                          },
  { (void __far*)0, "StrSLong",      "Script command line too long!\n",                            },
  { (void __far*)0, "StrTooManyErr", "Error: Too many unrecognized commands!\n"                    },
  { (void __far*)0, "StrScrStop",    "Warning: Script interrupted!\n"                              },
  { (void __far*)0, "StrSFile",      "Cannot open script file '$'\n",                              },
  { (void __far*)0, "StrNoScrSave",  "Unable to save script data!\n"                               },
  { (void __far*)0, "StrInterScr",   "*** RasMol script interrupted! ***\n",                       },
  { (void __far*)0, "StrUnLight",    "Headlight undefined; default lighting used!\n",              },

  { (void __far*)0, "StrSMem",       "Command Error: Insufficient memory!\n",                      },

  { (void __far*)0, "StrCTerm",      "String constant unterminated!\n",                            },
  { (void __far*)0, "StrCLong",      "String constant too long!\n",                                },
  { (void __far*)0, "StrFNum",       "Invalid floating point number!\n",                           },
  { (void __far*)0, "StrCent",       "No Atoms to Centre!\n",                                      },
  { (void __far*)0, "StrCClip",      "Unable to copy to clipboard!\n",                             },
  { (void __far*)0, "StrPClip",      "Unable to paste from clipboard!\n",                          },
  { (void __far*)0, "StrNoClip",     "No clipboard operations if drawing is off!\n",               },

  { (void __far*)0, "StrNPrint",     "Warning: No suitable printer!\n",                            },
  { (void __far*)0, "StrNoPrint",    "Warning: Cannot print with drawing off!\n",                  },

  { (void __far*)0, "StrUnrec",      "Unrecognised command!\n",                                    },
  { (void __far*)0, "StrIgnore",     "Warning: Ignoring rest of command!\n",                       },
  { (void __far*)0, "StrRCLong",     "Warning: Remote command too long!\n",                        },
  { (void __far*)0, "StrILong",      "Identifier too long!\n",                                     },
  { (void __far*)0, "StrMolNam",     "Molecule name ......... ",                                   },
  { (void __far*)0, "StrClass",      "Classification ........ ",                                   },
  { (void __far*)0, "StrSecSt",      "Secondary Structure ... ",                                   },
  { (void __far*)0, "StrNoAsmt",     "No Assignment\n",                                            },
  { (void __far*)0, "StrPDBRec",     "PDB Data Records\n",                                         },
  { (void __far*)0, "StrCalc",       "Calculated\n",                                               },
  { (void __far*)0, "StrDBCode",     "Database Code ......... ",                                   },
  { (void __far*)0, "StrExpTec",     "Experiment Technique .. ",                                   },
  { (void __far*)0, "StrNumChn",     "Number of Chains ...... ",                                   },
  { (void __far*)0, "StrNumGrp",     "Number of Groups ...... ",                                   },
  { (void __far*)0, "StrNumAtm",     "Number of Atoms ....... ",                                   },
  { (void __far*)0, "StrNumBnd",     "Number of Bonds ....... ",                                   },
  { (void __far*)0, "StrNumBrg",     "Number of Bridges ..... ",                                   },
  { (void __far*)0, "StrNumHbd",     "Number of H-Bonds ..... ",                                   },
  { (void __far*)0, "StrNumHel",     "Number of Helices ..... ",                                   },
  { (void __far*)0, "StrNumStrnd",   "Number of Strands ..... ",                                   },
  { (void __far*)0, "StrNumTrn",     "Number of Turns ....... ",                                   },
  { (void __far*)0, "StrMalloc",     "Memory allocation failed",                                   },
  { (void __far*)0, "StrXSRes",      "Too many new residues",                                      },
  { (void __far*)0, "StrXSAtyp",     "Too many new atom types",                                    },

  /* Object Description */
  { (void __far*)0, "StrObjModel",   "Model: $\n"                                                  },
  { (void __far*)0, "StrObjGroup",   "  ($/$ atoms)\n"                                             },
  { (void __far*)0, "StrObjChain",   "  ($/$ groups)\n"                                            },
  { (void __far*)0, "StrNoGroupSel", "  no group completely selected\n"                            },
  { (void __far*)0, "StrCurSel",     "# Current selection\n"                                       },
  { (void __far*)0, "StrDscbSeq",    "# Sequence\n"                                                },
  { (void __far*)0, "StrSeqModel",   "Model: $  "                                                  },
  { (void __far*)0, "StrSeqChain",   "Chain $:\n"                                                  },
  { (void __far*)0, "StrDesSym",     "# Symmetry\n"                                                },
  { (void __far*)0, "StrUCell",      "Unit Cell",                                                  },
  { (void __far*)0, "StrSGroup",     "Space Group",                                                },
  { (void __far*)0, "StrSymm",       "No crystal symmetry data!\n",                                },
  { (void __far*)0, "StrMolSel",     "# Molecule currently selected: "                             },
  
  { (void __far*)0, "StrWrdlWrdl",   "# World\n"                                                   },
  { (void __far*)0, "StrWrdlDegr",   "Degrees\n"                                                   },
  { (void __far*)0, "StrWrdlPos",    "Position: "                                                  },
  { (void __far*)0, "StrWrldRot",    "Rotation: "                                                  },
  { (void __far*)0, "StrWrldZoom",   "Zoom:     "                                                  },
  { (void __far*)0, "StrWrdlScal",   "Scale:    "                                                  },
  { (void __far*)0, "StrWrdlDDph",   "Default worlddepth: "                                        },
  { (void __far*)0, "StrWrdlCDhp",   "Current worlddepth: "                                        },
  { (void __far*)0, "StrWrldSlb",    "Slab:     "                                                  },
  { (void __far*)0, "StrWrldDpth",   "Depth:    "                                                  },
  { (void __far*)0, "StrWrldMol",    "Molecules:\n"                                                },
  { (void __far*)0, "StrWrldInd",    "Index     Title\n"                                           },
  { (void __far*)0, "StrWrldMol2",   " molecules\n"                                                },
  { (void __far*)0, "StrWrldAsbPar", "# Absolute parameters\n"                                     },
  { (void __far*)0, "StrWrldPos2",   "Position  (Angstroms):\n"                                    },
  { (void __far*)0, "StrWrldXYZ",    "Index     x         y         z\n"                           },
  { (void __far*)0, "StrWrldRot2",   "Rotation (degrees):\n"                                       },
  { (void __far*)0, "StrWrldInd2",   "Index\n"                                                     },
  { (void __far*)0, "StrWrldSPar",   "# Screen parameters\n"                                       },
  { (void __far*)0, "StrWrldWPar",   "# World parameters\n"                                        },

  { (void __far*)0, "StrShowMol",    "# Molecule: "                                                },
  { (void __far*)0, "StrCenWOri",    "centre at world origin\n"                                    },
  { (void __far*)0, "StrCenMOri",    "centre at molecule origin\n"                                 },
  { (void __far*)0, "StrCenCen",     "centre at "                                                  },
  { (void __far*)0, "StrAbsVal",     "Values in absolute coordinates:\n"                           },
  { (void __far*)0, "StrScreenVal",  "Values in screen coordinates:\n"                             },
  { (void __far*)0, "StrRotateX",    "rotate x $\n"                                                },
  { (void __far*)0, "StrRotateY",    "rotate y $\n"                                                },
  { (void __far*)0, "StrRotateZ",    "rotate z $\n"                                                },
  { (void __far*)0, "StrRotBond",    "rotate bond $\n"                                             },
  { (void __far*)0, "StrTransX",     "translate x $\n"                                             },
  { (void __far*)0, "StrTransY",     "translate y $\n"                                             },
  { (void __far*)0, "StrTransZ",     "translate z $\n"                                             },
  { (void __far*)0, "StrZoom",       "zoom $\n"                                                    },
  { (void __far*)0, "StrWDepthDef",  "Default worlddepth: $ Angstroms or $ RU\n"                   },
  { (void __far*)0, "StrWDepthCur",  "Current worlddepth: $ Angstroms or $ RU\n"                   },
  { (void __far*)0, "StrWrongShow",  "Error argument on show command!\n"                           },

  { (void __far*)0, "StrPickLabel",  "Picking mode: clik on atoms to add labels!\n",               },

  /* RasMol Fatal Exit */
  { (void __far*)0, "RfeNoMem",      "Error: Insufficient memory!"                                 },
  { (void __far*)0, "RfeNoCurMol",   "Error: No Current Molecule Defined"                          },
  { (void __far*)0, "RfeMemLang",    "Error: Memory allocation failed in LangSel!"                 },
  { (void __far*)0, "RfeSavDoc",     "Error: Failure in saving documents!\n"                       },
  { (void __far*)0, "RfeHROpn",      "Command Error: Unable to reopen help file!\n",               },
  { (void __far*)0, "RfeCmdMem",     "Command Error: Insufficient memory!"                         },
  { (void __far*)0, "RfeDatabase",   "Database Error: Memory allocation failed!"                   },
  { (void __far*)0, "RfeNewRes",     "Database Error: Too many new residues!"                      },
  { (void __far*)0, "RfeNewAtoms",   "Database Error: Too many new atom types!"                    },

  /* brotate */
  { (void __far*)0, "RotDupSer",     "Error: Duplicate atom serial numbers!\n",                    },
  { (void __far*)0, "RotAtSer",      "Error: Atom serial number $ not found!\n"                    },
  { (void __far*)0, "RotGraph",      "Error: ConstructGraph failed\n"                              },
  { (void __far*)0, "RotNoRot",      "Warning: Cannot rotate this bond!\n"                         },
  { (void __far*)0, "RotNoSel",      "Error: There is no bond to select!\n"                        },
  { (void __far*)0, "RotSelOk",      "Bond selected\n"                                             },

  /* infile */
  { (void __far*)0, "FilInvCrys",    "Error: Invalid CRYST1\n"                                     },
  { (void __far*)0, "FilInvScal",    "Error: Invalid SCALE ignored\n"                              },
  { (void __far*)0, "FilInvZmat",    "Error: Invalid MOPAC z-matrix file!\n"                       },
  { (void __far*)0, "FilCIFa",       "Error: Missing or Incomplete CIF Cell\n"                     },
  { (void __far*)0, "FilCIFb",       "Error: Invalid CIF Cell\n"                                   },
  { (void __far*)0, "FilCIFMat",     "Error: Invalid CIF fractional transformation matrix\n"       },
  { (void __far*)0, "FilCIFCart",    "Error: Invalid CIF Cartesian transformation matrix\n"        },
  { (void __far*)0, "FilStrCon",     "Warning: Null bonds in STRUCT_CONN!\n"                       },
  { (void __far*)0, "FilGeoBon",     "Warning: Null bonds in GEOM_BOND!\n"                         },
  { (void __far*)0, "FilNoFil",      "Error: Unable to create file!\n"                             },

  /* molecule */
  { (void __far*)0, "MolMolNam",     "Molecule name ....... "                                      },
  { (void __far*)0, "MolClass",      "Classification ...... "                                      },
  { (void __far*)0, "MolSecStr",     "Secondary Structure . "                                      },
  { (void __far*)0, "MolNoAss",      "No Assignment\n"                                             },
  { (void __far*)0, "MolPDB",        "PDB Data Records\n"                                          },
  { (void __far*)0, "MolCalcul",     "Calculated\n"                                                },
  { (void __far*)0, "MolDatCod",     "Database Code ....... "                                      },
  { (void __far*)0, "MolExpTech",    "Experiment Technique. "                                      },
  { (void __far*)0, "MolChainNb",    "Number of Chains .... "                                      },
  { (void __far*)0, "MolGrpNb",      "Number of Groups .... "                                      },
  { (void __far*)0, "MolAtomNb",     "Number of Atoms ..... "                                      },
  { (void __far*)0, "MolBondNb",     "Number of Bonds ..... "                                      },
  { (void __far*)0, "MolNumBr",      "Number of Bridges ... "                                      },
  { (void __far*)0, "MolNumHB",      "Number of H-Bonds ... "                                      },
  { (void __far*)0, "MolNumHel",     "Number of Helices ... "                                      },
  { (void __far*)0, "MolNumStr",     "Number of Strands ... "                                      },
  { (void __far*)0, "MolNumTurn",    "Number of Turns ..... "                                      },
  { (void __far*)0, "MolNumBds",     "Number of Bonds ..... "                                      },
  { (void __far*)0, "MolSerNb",      "Error: Duplicate atom serial numbers!\n"                     },
  { (void __far*)0, "MolErrAtSer",   "Error: Atom serial number $ not found!\n"                    },
  { (void __far*)0, "MolPrevRot",    "Previous rotation bond selected.\n"                          },
  { (void __far*)0, "MolNextRot",    "Next rotation bond selected.\n"                              },
  { (void __far*)0, "MolNoRotBnd",   "No rotation bond selected.\n"                                },
  { (void __far*)0, "MolNoBndRem",   "Error: There is no bond to remove!\n"                        },
  { (void __far*)0, "MolPlease",     "Please wait...\n"                                             },

  /* multiple */
  { (void __far*)0, "MulDataNF",     "Error: Molecular data not found!\n"                          },
  { (void __far*)0, "MulMisEnd",     "Error: Missing end of script in inline data!\n"              },
  { (void __far*)0, "MulSeeHelp",    "       See help how to enter it manually.\n"                 },
  { (void __far*)0, "MulErrNoNam",   "Error: There is no molecule with this name!\n"               },
  { (void __far*)0, "MulErrNoInd",   "Error: There is no molecule at this index!\n"                },
  { (void __far*)0, "MulNoTrans",    "Warning: Cannot translate molecule if world is on!\n"        },

  /* output */
  { (void __far*)0, "OutNoFile",     "Output Error: Unable to create file `"                       },
  { (void __far*)0, "Out8bitGIF",    "Output Error: Only 8-bit GIF files supported!\n"             },
  { (void __far*)0, "Out8bitBMP",    "Output Error: Only 8-bit BMP files supported!\n"             },
  { (void __far*)0, "OutPSObj",      "Output Error: Too many PostScript objects!\n"                },
  { (void __far*)0, "OutNoMemPS",    "Not enough memory to create PostScript!\n"                   },
  { (void __far*)0, "OutNoMem",      "Output Error: Unable to allocate memory!\n"                  },

  /* prefer */
  { (void __far*)0, "PrfNotFound",   "Preferences not found!\n"                                    },
  { (void __far*)0, "PrfResetDef",   "Error; resetting default preferences!\n"                     },
  { (void __far*)0, "PrfLoaded",     "Preferences loaded\n"                                        },
  { (void __far*)0, "PrfCreate",     "Creating Preference file.\n"                                 },

  /* render */
  { (void __far*)0, "RenNoBdSel",    "Warning: There is no bond selected!\n"                       },
  { (void __far*)0, "RenMol2",       "Molecule: "                                                  },
  { (void __far*)0, "RenAtom",       "Atom: "                                                      },
  { (void __far*)0, "RenHetero",     "Hetero: "                                                    },
  { (void __far*)0, "RenGroup",      "Group: "                                                     },
  { (void __far*)0, "RenChain",      "  Chain: "                                                   },
  { (void __far*)0, "RenModel",      "  Model: "                                                   },
  { (void __far*)0, "RenWorld",      "World "                                                      },
  { (void __far*)0, "RenMol",        "Molecule "                                                   },
  { (void __far*)0, "RenSel",        " selected!\n"                                                },
  { (void __far*)0, "RenScreen",     "Screen "                                                     },
  { (void __far*)0, "RenCoord",      "Coordinates: "                                               },
  { (void __far*)0, "RenAbout",      "about "                                                      },
  { (void __far*)0, "RenRot",        "Rotating about "                                             },
  { (void __far*)0, "RenRot2",       "Rotating "                                                   },
  { (void __far*)0, "RenWOri",       "Rotating about World Origin\n"                               },
  { (void __far*)0, "RenNoCenter",   "Warning: Cannot select this center in world mode!\n"         },
  { (void __far*)0, "RenAtom2",      "Atom #"                                                      },
  { (void __far*)0, "RenAtomDup",    "Error: Duplicate atoms!\n"                                   },
  { (void __far*)0, "RenDist",       "Distance "                                                   },
  { (void __far*)0, "RenAngle",      "Angle "                                                      },
  { (void __far*)0, "RenTorsion",    "Torsion "                                                    },

  /* repress */
  { (void __far*)0, "RepErrMem",     "Renderer Error: Unable to allocate $!"                       },
  { (void __far*)0, "RepErrLab",     "label"                                                       },
  { (void __far*)0, "RepErrMon",     "monitor"                                                     },
  { (void __far*)0, "RepErrDSur",    "dot surface"                                                 },
  { (void __far*)0, "RepErrConB",    "connolly surface: BList"                                     },
  { (void __far*)0, "RepErrConC",    "connolly surface: CList"                                     },
  { (void __far*)0, "RepErrConT",    "connolly surface: tetrad"                                    },
  { (void __far*)0, "RepErrTabl",    "dot vector table"                                            },
  { (void __far*)0, "RepErrVect",    "dot vectors"                                                 },
  { (void __far*)0, "RepErrPrb",     "probe vectors"                                               },

  { (void __far*)0, "RepDupAtom",    "Error: Duplicate atom serial numbers!\n"                     },
  { (void __far*)0, "RepAtomSer",    "Error: Atom serial number $ not found!\n"                    },
  { (void __far*)0, "RepAllDot",     " dots read from file\n"                                      },
  { (void __far*)0, "RepOneDot",     "1 dot read from file\n"                                      },
  { (void __far*)0, "RepNoDot",      "No dots read from file!\n"                                   },

  /* script */           
  { (void __far*)0, "ScrNoFile",     "Script Error: Unable to create file `"                       },

  /* transfor */
  { (void __far*)0, "TrnNoAtSel",    "No atoms selected!\n"                                        },
  { (void __far*)0, "TrnOneAtSel",   "1 atom selected!\n"                                          },
  { (void __far*)0, "TrnManAtSel",   " atom selected!\n"                                           },
  { (void __far*)0, "TrnNoShade",    "Warning: Unable to allocate shade!\n"                        },
  { (void __far*)0, "TrnNoPot",      "Warning: No surface potential available!\n"                  },
  { (void __far*)0, "TrnNoCol",      "Warning: No user supplied colour records!\n"                 },

  /* Help */
  { (void __far*)0, "HlpHFil",       "No help available on the command line!\n",                   },
  { (void __far*)0, "HlpHTop",       "Ambiguous help topic requested!\n",                          },
  { (void __far*)0, "HlpHNone",      "No available help on requested topic!\n",                    },
  { (void __far*)0, "HlpHTMLHelp",   "Error: HTML Help location unknown!\n",                       },


  /* langsel */
  { (void __far*)0, "LANNoLoc",      "Error: Dictionary location unknown!\n",                      },
  { (void __far*)0, "LANNoFile",     "Error: Dictionary not found!\n",                             },

 /*************
 * RasTopWin *
 *************/

  /* datadisplay */
  { (void __far*)0, "RTPErrScrData", "Error with script data; script may not work!\n"              },

  /* dialog */
  { (void __far*)0, "RTPErrChar",    "Error: Use alphanumeric, '$', or '_' characters!\n"          },
  { (void __far*)0, "RTPErrStrd",    "Only 1, 2, 3, 4, 5, or 9 strands are allowed!\n"             },
  { (void __far*)0, "RTPNoClip",     "Warning: clipboard not available!\n"                         },
  { (void __far*)0, "RTPNoFile",     "Error: Failure in creating file!\n"                          },
  { (void __far*)0, "RTPSaveData",   "Error: Failure in saving data!\n"                            },
  { (void __far*)0, "RTPErrDialog",  "Warning: Failure in dialog!\n"                               },
  { (void __far*)0, "RTPErrMem",     "Warning: Memory failure!\n"                                  },
  { (void __far*)0, "RTPErrPath",    "Warning: The specified path is too long!\n"                  },

  /* mainframe */
  { (void __far*)0, "RTPWCoor",      "Coordinate system switched to world coordinates!\n"          },
  { (void __far*)0, "RTPSCoor",      "Coordinate system switched to screen coordinates!\n"         },
  { (void __far*)0, "RTPACoor",      "Coordinate system switched to absolute coordinates!\n"       },
  { (void __far*)0, "RTPMCoor",      "Coordinate system switched to molecule coordinates!\n"       },
  { (void __far*)0, "RTPStopScr",    "Type any key to continue the script or <Esc> to terminate\n" },

  /* rasgraphics */
  { (void __far*)0, "RTPZeroAt",     "Zero selected atom copied to clipboard!\n"                   },
  { (void __far*)0, "RTPOneAt",      "1 selected atom copied to clipboard!\n"                      },
  { (void __far*)0, "RTPManyAt",     " selected atoms copied to clipboard!\n"                      },
  { (void __far*)0, "RTPPosClip",    "Position copied to clipboard!\n"                             },
  { (void __far*)0, "RTPNoMemSel",   "Warning: Insuffisant memory to copy selection!\n"            },
  { (void __far*)0, "RTPNoDataClip", "Warning: the clipboard does not contain any data!\n"         },
  { (void __far*)0, "RTPNoComdClip", "Warning: no valid commands in clipboard!\n"                  },
  { (void __far*)0, "RTPUnkCmd",     "Warning: unknown commands in clipboard!\n"                   },
  { (void __far*)0, "RTPNoSelClip",  "No selection to paste from clipboard!\n"                     },
  { (void __far*)0, "RTPOneSelClip", "1 selected atom pasted from clipboard!\n"                    },
  { (void __far*)0, "RTPManSelClip", " selected atoms pasted from clipboard!\n"                    },
  { (void __far*)0, "RTPPastePos",   "Position pasted from clipboard!\n"                           },
  { (void __far*)0, "RTPScrPaste",   "Script pasted from clipboard!\n"                             },
  { (void __far*)0, "RTPSelBig",     "Warning: Selection too large, cannot copy to clipboard!\n"   },

  /* rasdoc - rasview */
  { (void __far*)0, "RTPImgBig",     "Error: Image too big!\n"                                     },
  { (void __far*)0, "RTPNoWind",     "Error: Cannot create new window!\n"                          },
  { (void __far*)0, "RTPErrDat",     "Warning: Failure in loading data!\n"                         },
  { (void __far*)0, "RTPNoSave",     "No modification to save!\n"                                  },
  { (void __far*)0, "RTPRsmLoad",    "File $ loaded!\n"                                            },
  { (void __far*)0, "RTPScrLoad",    "Loading script file...\n"                                    },
  { (void __far*)0, "RTPScrBox",     "Select RasMol Script File"                                   },
  { (void __far*)0, "RTPScrInter",   "Loading script interrupted!\n"                               },
  { (void __far*)0, "RTPScrDone",    "Script executed!\n"                                          },
  { (void __far*)0, "RTPErrImg",     "Error during image processing!\n"                            },
  { (void __far*)0, "RTPZoomFac",    "Warning: Image too big, smaller zoom factor is used!\n"      },
  { (void __far*)0, "RTPScrLong",    "Warning:	Script command line too long!\n"                   },
  { (void __far*)0, "RTPSelMol",     "Select Molecular Coordinate File"                            },
  { (void __far*)0, "RTPSelDir",     "Select Directory and Filename"                               },

};


static int InternalFlag = 0;


static void SetInternalEng (void )
{
    register int len,i;

    for( i=0; i<MaxStrFlag; i++ )
    {   len = strlen(langstrs[i].engmsg)+1;
        if( !len ) continue;
        langstrs[i].msg=(char*)_fmalloc(len);
        if( !(langstrs[i].msg)) RasMolFatalExit(MsgStrs(RfeMemLang));
        memcpy( langstrs[i].msg, langstrs[i].engmsg, len);
    }
    
    InternalFlag = 1;
}


static FreeMsg( void )
{
    register int i;

    for( i=0; i<MaxStrFlag; i++ )
        if( langstrs[i].msg ) _ffree(langstrs[i].msg);
}


void InitialiseLangSel( void )
{
    SetInternalEng ();

}


void ExitLangSel( void )
{
    FreeMsg();
}


char * MsgStrs( int m )
{   
    return langstrs[m].msg;
}


/* Replace '$' in msgflag by value - buffer is 80 characters-long */
void FormatMsg( char *buffer, int msgflag, char * value)
{
    char temp[80];
    char *ptr,*tag;
    int count,i;

    i = 0;

    /* No flag means input buffer contains text to format */
    if( msgflag == 0 )
    {   temp[0] = '\0';
        strcpy(temp,buffer);
        ptr = temp;
    } else
        ptr = langstrs[msgflag].msg;

    if( !ptr ) 
    {   strcpy(buffer,"Error Formatting local message!\n");
        return;
    }
    
    tag = strchr(ptr,'$');

    if( !tag ) 
    {   strcpy(buffer,"Error Formatting local message!\n");
        return;
    }

    count = tag - ptr;

    buffer[0] = '\0';
    i = 0;
    while( count )
    {   *buffer++ = *ptr++;
        count--;
        i++;
        if( i == 80 )
        {   *buffer = '\0';
            *(buffer-1) = '\n';
            return;
        }
    }
    while( *value )
    {   *buffer++ = *value++;
        i++;
        if( i == 80 )
        {   *buffer = '\0';
            *(buffer-1) = '\n';
            return;
        }
    }
    ptr++;
    while( *ptr )
    {   *buffer++ = *ptr++;
        i++;
        if( i == 80 )
        {   *buffer = '\0';
            *(buffer-1) = '\n';
            return;
        }
    }
    *buffer = '\0';
    return;
}


void SwitchLang( char *lang )
{   char langpath[1024];
    char buffer[80];
    char mtag[80];
    char mval[80];
    char langext[80];
    int pos,cur;
    FILE *fp;
    register char *ptr, *temp;


    if(!*RasMolPath)
    {   InvalidateCmndLine();
        WriteString(MsgStrs(LANNoLoc));
        return;
    }

    strcpy(langext,lang);
    ptr = langext;
//    while( *ptr++ = tolower(*ptr));
    ptr = RasMolPath;
    ptr = strcpy(langpath,ptr);
    while( *ptr++ );
    ptr--;
    ptr = strcpy(ptr,"lib");
    ptr += 3;
    *ptr++ = DirChar;
    strcpy(ptr,"rasmol_");
    while( *ptr++ );
    ptr--;
    ptr = strcpy(ptr,langext);
    while( *ptr++ );
    ptr--;
    strcpy(ptr,".lang");


    fp=fopen(langpath,"rb");
    if( !fp )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(LANNoFile));
        return;
    }

    pos= 0;
    while( xfgets(buffer,100,fp) )
    {   if( *buffer=='#' )
                 continue;
        ptr = strchr(buffer,'=');
        if( !ptr )
            continue;
        ptr = strchr(buffer,'\r');
        if( !ptr ) {
            ptr = strchr(buffer,'\n');
            if( !ptr ) 
                continue;   //bug?
        }
        *ptr = '\0';

        ptr = buffer;
        temp = mtag;
        while( *ptr != '=')
        {   if( isspace(*ptr) )
            {   ptr++;
                continue;
            }
            *temp++ = *ptr++;
        }
        *temp = '\0';
        ptr++;


        /* No trim for value */
        temp = mval;
        while( *ptr ) *temp++ = *ptr++;
        *temp = '\0';

        /* Search for encoded characters */
        while (1) {
            ptr = strchr(mval,'\\');
            if (ptr) {
                if (*(ptr+1) == '\0') {
                    /* Error typo or ligne too long */
                    *ptr++ = '\0';
                } else if (*(ptr+1) == 'n') {
                    *ptr++ = '\n';
                    while (*ptr++ = *(ptr+1));
                } else if (*(ptr+1) == 't') {
                    *ptr++ = '\t';
                    while (*ptr++ = *(ptr+1));
                }
            } else break;
        }

        /* Naturally, search the next tag in the next position */
        cur = pos;
        while( cur<MaxStrFlag )
        {   if( !strcmp(mtag,langstrs[cur].msgtag) )
            {   int len;
                _ffree(langstrs[cur].msg);
                len = strlen(mval)+1;
                langstrs[cur].msg=(char*)_fmalloc(len);
                if( !(langstrs[cur].msg)) RasMolFatalExit(MsgStrs(RfeMemLang));
                strcpy(langstrs[cur].msg,mval);
                cur++;
                break;
            }
            cur++;
        }
        if( cur == MaxStrFlag ) 
            cur = 0;
        while( cur<pos )
        {   if( !strcmp(mtag,langstrs[cur].msgtag) )
            {   int len;
                _ffree(langstrs[cur].msg);
                len = strlen(mval)+1;
                langstrs[cur].msg=(char*)_fmalloc(len);
                if( !(langstrs[cur].msg)) RasMolFatalExit(MsgStrs(RfeMemLang));
                strcpy(langstrs[cur].msg,mval);
                cur++;
                break;
            }
            cur++;
        }
        pos =cur;
    }
    fclose(fp);


}
