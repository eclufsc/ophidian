
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         defyyparse
#define yylex           defyylex
#define yyerror         defyyerror
#define yylval          defyylval
#define yychar          defyychar
#define yydebug         defyydebug
#define yynerrs         defyynerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 58 "def.y"

#include <stdlib.h>
#include <string.h>
#include "defrReader.hpp"
#include "defiUser.hpp"
#include "defrCallBacks.hpp"
#include "lex.h"

#define DEF_MAX_INT 2147483647
#define YYDEBUG 1     // this is temp fix for pcr 755132 
// TX_DIR:TRANSLATION ON


#include "defrData.hpp"
#include "defrSettings.hpp"
#include "defrCallBacks.hpp"

BEGIN_LEFDEF_PARSER_NAMESPACE

extern int errors;        // from lex.cpph
extern int defInPropDef;  // from lex.cpph, tracking if insided propDef

// From def_keywords.cpp 
extern char* ringCopy(const char* string);

extern void pathIsDone(int shield, int reset, int netOsnet, int *needCbk);

// Macro to describe how we handle a callback.
// If the function was set then call it.
// If the function returns non zero then there was an error
// so call the error routine and exit.
//
#define CALLBACK(func, typ, data) \
    if (!defData->errors) {\
      if (func) { \
        if ((defData->defRetVal = (*func)(typ, data, defSettings->UserData)) == PARSE_OK) { \
        } else if (defData->defRetVal == STOP_PARSE) { \
          return defData->defRetVal; \
        } else { \
          defError(6010, "An error has been reported in callback."); \
          return defData->defRetVal; \
        } \
      } \
    }

#define CHKERR() \
    if (defData->errors > 20) {\
      defError(6011, "Too many syntax defData->errors have been reported."); \
      defData->errors = 0; \
      return 1; \
    }

#define CHKPROPTYPE(propType, propName, name) \
    if (propType == 'N') { \
       defData->warningMsg = (char*)defMalloc(strlen(propName)+strlen(name)+40); \
       sprintf(defData->warningMsg, "The PropName %s is not defined for %s.", \
               propName, name); \
       defWarning(7010, defData->warningMsg); \
       defFree(defData->warningMsg); \
    }

int validateMaskInput(int input, int warningIndex, int getWarningsIndex) 
{
    if (defData->VersionNum < 5.8 && input > 0) {
      if (warningIndex++ < getWarningsIndex) {
          defData->defMsg = (char*)defMalloc(1000);
          sprintf (defData->defMsg,
             "The MASK statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
          defError(7415, defData->defMsg);
          defFree(defData->defMsg);
          CHKERR();
          
          return 0;
        }
    }   
    
    return 1; 
}

double convert_defname2num(char *versionName)
{
    char majorNm[80];
    char minorNm[80];
    char *subMinorNm = NULL;
    char *versionNm = strdup(versionName);

    double major = 0, minor = 0, subMinor = 0;
    double version;

    sscanf(versionNm, "%[^.].%s", majorNm, minorNm);
    
    char *p1 = strchr(minorNm, '.');
    if (p1) {
       subMinorNm = p1+1;
       *p1 = '\0';
    }
    major = atof(majorNm);
    minor = atof(minorNm);
    if (subMinorNm)
       subMinor = atof(subMinorNm);

    version = major;

    if (minor > 0)
       version = major + minor/10;

    if (subMinor > 0)
       version = version + subMinor/1000;

    free(versionNm);
    return version;
}

int numIsInt (char* volt) {
    if (strchr(volt, '.'))  // a floating point
       return 0;
    else
       return 1;
}

int defValidNum(int values) {
    char *outMsg;
    switch (values) {
        case 100:
        case 200:
        case 1000:
        case 2000:
                return 1;
        case 400:
        case 800:
        case 4000:
        case 8000:
        case 10000:
        case 20000:
             if (defData->VersionNum < 5.6) {
                if (defCallbacks->UnitsCbk) {
                  if (defData->unitsWarnings++ < defSettings->UnitsWarnings) {
                    outMsg = (char*)defMalloc(1000);
                    sprintf (outMsg,
                    "An error has been found while processing the DEF file '%s'\nUnit %d is a 5.6 or later syntax. Define the DEF file as 5.6 and then try again.",
                    defSettings->FileName, values);
                    defError(6501, outMsg);
                    defFree(outMsg);
                  }
                }
                
                return 0;
             } else {
                return 1;
             }
    }
    if (defCallbacks->UnitsCbk) {
      if (defData->unitsWarnings++ < defSettings->UnitsWarnings) {
        outMsg = (char*)defMalloc(10000);
        sprintf (outMsg,
          "The value %d defined for DEF UNITS DISTANCE MICRON is invalid\n. The valid values are 100, 200, 400, 800, 1000, 2000, 4000, 8000, 10000, or 20000. Specify a valid value and then try again.", values);
        defError(6502, outMsg);
        defFree(outMsg);
        CHKERR();
      }
    }
    return 0;
}

#define FIXED 1
#define COVER 2
#define PLACED 3
#define UNPLACED 4


/* Line 189 of yacc.c  */
#line 252 "def.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     QSTRING = 258,
     T_STRING = 259,
     SITE_PATTERN = 260,
     NUMBER = 261,
     K_HISTORY = 262,
     K_NAMESCASESENSITIVE = 263,
     K_DESIGN = 264,
     K_VIAS = 265,
     K_TECH = 266,
     K_UNITS = 267,
     K_ARRAY = 268,
     K_FLOORPLAN = 269,
     K_SITE = 270,
     K_CANPLACE = 271,
     K_CANNOTOCCUPY = 272,
     K_DIEAREA = 273,
     K_PINS = 274,
     K_DEFAULTCAP = 275,
     K_MINPINS = 276,
     K_WIRECAP = 277,
     K_TRACKS = 278,
     K_GCELLGRID = 279,
     K_DO = 280,
     K_BY = 281,
     K_STEP = 282,
     K_LAYER = 283,
     K_ROW = 284,
     K_RECT = 285,
     K_COMPS = 286,
     K_COMP_GEN = 287,
     K_SOURCE = 288,
     K_WEIGHT = 289,
     K_EEQMASTER = 290,
     K_FIXED = 291,
     K_COVER = 292,
     K_UNPLACED = 293,
     K_PLACED = 294,
     K_FOREIGN = 295,
     K_REGION = 296,
     K_REGIONS = 297,
     K_NETS = 298,
     K_START_NET = 299,
     K_MUSTJOIN = 300,
     K_ORIGINAL = 301,
     K_USE = 302,
     K_STYLE = 303,
     K_PATTERN = 304,
     K_PATTERNNAME = 305,
     K_ESTCAP = 306,
     K_ROUTED = 307,
     K_NEW = 308,
     K_SNETS = 309,
     K_SHAPE = 310,
     K_WIDTH = 311,
     K_VOLTAGE = 312,
     K_SPACING = 313,
     K_NONDEFAULTRULE = 314,
     K_NONDEFAULTRULES = 315,
     K_N = 316,
     K_S = 317,
     K_E = 318,
     K_W = 319,
     K_FN = 320,
     K_FE = 321,
     K_FS = 322,
     K_FW = 323,
     K_GROUPS = 324,
     K_GROUP = 325,
     K_SOFT = 326,
     K_MAXX = 327,
     K_MAXY = 328,
     K_MAXHALFPERIMETER = 329,
     K_CONSTRAINTS = 330,
     K_NET = 331,
     K_PATH = 332,
     K_SUM = 333,
     K_DIFF = 334,
     K_SCANCHAINS = 335,
     K_START = 336,
     K_FLOATING = 337,
     K_ORDERED = 338,
     K_STOP = 339,
     K_IN = 340,
     K_OUT = 341,
     K_RISEMIN = 342,
     K_RISEMAX = 343,
     K_FALLMIN = 344,
     K_FALLMAX = 345,
     K_WIREDLOGIC = 346,
     K_MAXDIST = 347,
     K_ASSERTIONS = 348,
     K_DISTANCE = 349,
     K_MICRONS = 350,
     K_END = 351,
     K_IOTIMINGS = 352,
     K_RISE = 353,
     K_FALL = 354,
     K_VARIABLE = 355,
     K_SLEWRATE = 356,
     K_CAPACITANCE = 357,
     K_DRIVECELL = 358,
     K_FROMPIN = 359,
     K_TOPIN = 360,
     K_PARALLEL = 361,
     K_TIMINGDISABLES = 362,
     K_THRUPIN = 363,
     K_MACRO = 364,
     K_PARTITIONS = 365,
     K_TURNOFF = 366,
     K_FROMCLOCKPIN = 367,
     K_FROMCOMPPIN = 368,
     K_FROMIOPIN = 369,
     K_TOCLOCKPIN = 370,
     K_TOCOMPPIN = 371,
     K_TOIOPIN = 372,
     K_SETUPRISE = 373,
     K_SETUPFALL = 374,
     K_HOLDRISE = 375,
     K_HOLDFALL = 376,
     K_VPIN = 377,
     K_SUBNET = 378,
     K_XTALK = 379,
     K_PIN = 380,
     K_SYNTHESIZED = 381,
     K_DEFINE = 382,
     K_DEFINES = 383,
     K_DEFINEB = 384,
     K_IF = 385,
     K_THEN = 386,
     K_ELSE = 387,
     K_FALSE = 388,
     K_TRUE = 389,
     K_EQ = 390,
     K_NE = 391,
     K_LE = 392,
     K_LT = 393,
     K_GE = 394,
     K_GT = 395,
     K_OR = 396,
     K_AND = 397,
     K_NOT = 398,
     K_SPECIAL = 399,
     K_DIRECTION = 400,
     K_RANGE = 401,
     K_FPC = 402,
     K_HORIZONTAL = 403,
     K_VERTICAL = 404,
     K_ALIGN = 405,
     K_MIN = 406,
     K_MAX = 407,
     K_EQUAL = 408,
     K_BOTTOMLEFT = 409,
     K_TOPRIGHT = 410,
     K_ROWS = 411,
     K_TAPER = 412,
     K_TAPERRULE = 413,
     K_VERSION = 414,
     K_DIVIDERCHAR = 415,
     K_BUSBITCHARS = 416,
     K_PROPERTYDEFINITIONS = 417,
     K_STRING = 418,
     K_REAL = 419,
     K_INTEGER = 420,
     K_PROPERTY = 421,
     K_BEGINEXT = 422,
     K_ENDEXT = 423,
     K_NAMEMAPSTRING = 424,
     K_ON = 425,
     K_OFF = 426,
     K_X = 427,
     K_Y = 428,
     K_COMPONENT = 429,
     K_MASK = 430,
     K_MASKSHIFT = 431,
     K_COMPSMASKSHIFT = 432,
     K_SAMEMASK = 433,
     K_PINPROPERTIES = 434,
     K_TEST = 435,
     K_COMMONSCANPINS = 436,
     K_SNET = 437,
     K_COMPONENTPIN = 438,
     K_REENTRANTPATHS = 439,
     K_SHIELD = 440,
     K_SHIELDNET = 441,
     K_NOSHIELD = 442,
     K_VIRTUAL = 443,
     K_ANTENNAPINPARTIALMETALAREA = 444,
     K_ANTENNAPINPARTIALMETALSIDEAREA = 445,
     K_ANTENNAPINGATEAREA = 446,
     K_ANTENNAPINDIFFAREA = 447,
     K_ANTENNAPINMAXAREACAR = 448,
     K_ANTENNAPINMAXSIDEAREACAR = 449,
     K_ANTENNAPINPARTIALCUTAREA = 450,
     K_ANTENNAPINMAXCUTCAR = 451,
     K_SIGNAL = 452,
     K_POWER = 453,
     K_GROUND = 454,
     K_CLOCK = 455,
     K_TIEOFF = 456,
     K_ANALOG = 457,
     K_SCAN = 458,
     K_RESET = 459,
     K_RING = 460,
     K_STRIPE = 461,
     K_FOLLOWPIN = 462,
     K_IOWIRE = 463,
     K_COREWIRE = 464,
     K_BLOCKWIRE = 465,
     K_FILLWIRE = 466,
     K_BLOCKAGEWIRE = 467,
     K_PADRING = 468,
     K_BLOCKRING = 469,
     K_BLOCKAGES = 470,
     K_PLACEMENT = 471,
     K_SLOTS = 472,
     K_FILLS = 473,
     K_PUSHDOWN = 474,
     K_NETLIST = 475,
     K_DIST = 476,
     K_USER = 477,
     K_TIMING = 478,
     K_BALANCED = 479,
     K_STEINER = 480,
     K_TRUNK = 481,
     K_FIXEDBUMP = 482,
     K_FENCE = 483,
     K_FREQUENCY = 484,
     K_GUIDE = 485,
     K_MAXBITS = 486,
     K_PARTITION = 487,
     K_TYPE = 488,
     K_ANTENNAMODEL = 489,
     K_DRCFILL = 490,
     K_OXIDE1 = 491,
     K_OXIDE2 = 492,
     K_OXIDE3 = 493,
     K_OXIDE4 = 494,
     K_CUTSIZE = 495,
     K_CUTSPACING = 496,
     K_DESIGNRULEWIDTH = 497,
     K_DIAGWIDTH = 498,
     K_ENCLOSURE = 499,
     K_HALO = 500,
     K_GROUNDSENSITIVITY = 501,
     K_HARDSPACING = 502,
     K_LAYERS = 503,
     K_MINCUTS = 504,
     K_NETEXPR = 505,
     K_OFFSET = 506,
     K_ORIGIN = 507,
     K_ROWCOL = 508,
     K_STYLES = 509,
     K_POLYGON = 510,
     K_PORT = 511,
     K_SUPPLYSENSITIVITY = 512,
     K_VIA = 513,
     K_VIARULE = 514,
     K_WIREEXT = 515,
     K_EXCEPTPGNET = 516,
     K_FILLWIREOPC = 517,
     K_OPC = 518,
     K_PARTIAL = 519,
     K_ROUTEHALO = 520
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 228 "def.y"

        double dval ;
        int    integer ;
        char * string ;
        int    keyword ;  // really just a nop 
        struct LefDefParser::defpoint pt;
        LefDefParser::defTOKEN *tk;



/* Line 214 of yacc.c  */
#line 564 "def.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 576 "def.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1483

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  273
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  521
/* YYNRULES -- Number of rules.  */
#define YYNRULES  955
/* YYNRULES -- Number of states.  */
#define YYNSTATES  1665

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   520

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     269,   270,   271,   268,   272,   267,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   266,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     8,     9,    10,    15,    16,    20,    24,
      25,    28,    30,    32,    34,    36,    38,    40,    42,    44,
      46,    48,    50,    52,    54,    56,    58,    60,    62,    64,
      66,    68,    70,    72,    74,    76,    78,    80,    82,    84,
      86,    88,    90,    92,    94,    96,    98,   100,   102,   104,
     106,   108,   109,   114,   117,   118,   123,   124,   129,   130,
     135,   137,   138,   144,   145,   148,   149,   155,   156,   162,
     163,   169,   170,   176,   177,   183,   184,   190,   191,   197,
     198,   204,   205,   211,   214,   215,   220,   221,   226,   228,
     231,   234,   235,   237,   243,   247,   251,   252,   267,   268,
     283,   285,   287,   289,   291,   293,   295,   297,   299,   300,
     307,   311,   314,   315,   318,   324,   327,   331,   335,   336,
     339,   340,   341,   342,   353,   354,   357,   360,   362,   366,
     370,   371,   376,   377,   382,   386,   389,   390,   391,   401,
     402,   403,   415,   416,   426,   430,   435,   440,   445,   450,
     451,   458,   459,   466,   471,   472,   479,   483,   484,   487,
     488,   491,   492,   495,   496,   499,   502,   503,   506,   509,
     511,   513,   515,   517,   519,   521,   523,   525,   527,   529,
     531,   533,   534,   535,   539,   542,   543,   544,   556,   557,
     563,   564,   568,   569,   572,   573,   578,   579,   582,   585,
     588,   591,   592,   602,   605,   607,   609,   612,   613,   617,
     618,   620,   621,   622,   627,   628,   631,   633,   642,   644,
     647,   651,   655,   656,   659,   660,   661,   668,   669,   672,
     673,   681,   682,   683,   694,   695,   700,   701,   702,   727,
     729,   731,   736,   741,   748,   749,   754,   756,   758,   759,
     762,   767,   772,   777,   782,   783,   787,   790,   795,   799,
     800,   803,   804,   805,   813,   816,   820,   821,   824,   825,
     830,   834,   835,   838,   841,   844,   847,   849,   851,   855,
     859,   863,   864,   867,   869,   870,   873,   877,   880,   881,
     886,   887,   890,   893,   894,   897,   899,   901,   903,   905,
     907,   909,   911,   913,   915,   917,   919,   921,   923,   924,
     929,   930,   936,   937,   939,   943,   945,   947,   949,   951,
     954,   957,   960,   964,   965,   974,   975,   977,   978,   985,
     986,   991,   993,   996,   999,  1002,  1005,  1008,  1009,  1016,
    1018,  1021,  1025,  1028,  1033,  1037,  1040,  1043,  1046,  1050,
    1053,  1057,  1061,  1062,  1065,  1069,  1071,  1072,  1076,  1077,
    1081,  1082,  1089,  1090,  1093,  1094,  1101,  1102,  1109,  1110,
    1117,  1118,  1120,  1123,  1124,  1127,  1128,  1133,  1137,  1140,
    1141,  1146,  1147,  1152,  1156,  1160,  1164,  1168,  1172,  1176,
    1177,  1182,  1184,  1185,  1190,  1191,  1192,  1198,  1199,  1200,
    1201,  1210,  1211,  1216,  1218,  1220,  1223,  1226,  1229,  1232,
    1234,  1236,  1238,  1240,  1242,  1243,  1250,  1251,  1256,  1257,
    1258,  1262,  1263,  1267,  1269,  1271,  1273,  1275,  1277,  1279,
    1281,  1284,  1285,  1289,  1290,  1291,  1298,  1301,  1304,  1305,
    1308,  1310,  1314,  1317,  1322,  1333,  1342,  1352,  1364,  1366,
    1368,  1369,  1380,  1381,  1386,  1388,  1393,  1398,  1403,  1408,
    1414,  1420,  1426,  1432,  1437,  1442,  1447,  1454,  1455,  1458,
    1460,  1462,  1464,  1465,  1469,  1472,  1473,  1476,  1480,  1484,
    1487,  1489,  1491,  1493,  1495,  1497,  1499,  1501,  1503,  1505,
    1507,  1509,  1511,  1515,  1516,  1519,  1523,  1524,  1527,  1529,
    1531,  1533,  1535,  1538,  1539,  1544,  1545,  1546,  1553,  1557,
    1561,  1562,  1563,  1573,  1574,  1581,  1582,  1583,  1592,  1596,
    1599,  1603,  1604,  1609,  1613,  1617,  1621,  1625,  1629,  1630,
    1635,  1637,  1638,  1640,  1642,  1644,  1646,  1648,  1650,  1652,
    1654,  1655,  1656,  1659,  1660,  1666,  1667,  1672,  1673,  1674,
    1682,  1684,  1687,  1690,  1693,  1696,  1697,  1701,  1702,  1706,
    1708,  1710,  1712,  1714,  1716,  1719,  1720,  1724,  1725,  1726,
    1734,  1736,  1740,  1743,  1747,  1751,  1752,  1755,  1760,  1761,
    1765,  1766,  1769,  1771,  1772,  1775,  1779,  1780,  1785,  1786,
    1791,  1793,  1796,  1798,  1799,  1802,  1805,  1808,  1811,  1812,
    1815,  1818,  1821,  1824,  1827,  1831,  1835,  1839,  1843,  1844,
    1847,  1849,  1851,  1856,  1857,  1861,  1862,  1869,  1874,  1879,
    1881,  1884,  1888,  1889,  1898,  1899,  1901,  1902,  1905,  1909,
    1913,  1917,  1921,  1924,  1927,  1931,  1935,  1936,  1939,  1943,
    1944,  1948,  1949,  1952,  1953,  1955,  1956,  1962,  1963,  1968,
    1969,  1974,  1975,  1981,  1982,  1987,  1988,  1994,  1996,  1997,
    2002,  2011,  2012,  2015,  2016,  2020,  2021,  2026,  2035,  2048,
    2049,  2052,  2053,  2057,  2058,  2063,  2072,  2085,  2086,  2089,
    2092,  2096,  2100,  2101,  2104,  2108,  2109,  2116,  2117,  2120,
    2126,  2132,  2136,  2137,  2138,  2145,  2147,  2148,  2149,  2156,
    2157,  2158,  2162,  2163,  2166,  2168,  2170,  2173,  2178,  2182,
    2183,  2186,  2187,  2188,  2197,  2199,  2201,  2203,  2206,  2209,
    2212,  2213,  2216,  2217,  2222,  2223,  2228,  2229,  2232,  2233,
    2239,  2240,  2246,  2250,  2254,  2255,  2258,  2259,  2260,  2271,
    2272,  2279,  2280,  2287,  2291,  2292,  2293,  2300,  2301,  2305,
    2308,  2312,  2316,  2317,  2320,  2324,  2325,  2330,  2331,  2335,
    2336,  2338,  2340,  2341,  2343,  2345,  2346,  2349,  2350,  2358,
    2359,  2366,  2367,  2373,  2374,  2382,  2383,  2390,  2391,  2397,
    2399,  2400,  2405,  2406,  2409,  2413,  2417,  2418,  2421,  2422,
    2425,  2428,  2431,  2434,  2437,  2439,  2442,  2446,  2450,  2454,
    2458,  2461,  2462,  2465,  2466,  2473,  2474,  2477,  2478,  2481,
    2482,  2486,  2487,  2491,  2493,  2495,  2497,  2499,  2503,  2507,
    2508,  2510,  2513,  2514,  2517,  2518,  2519,  2527,  2528,  2531,
    2532,  2537,  2538,  2541,  2544,  2547,  2550,  2554,  2558,  2561,
    2562,  2565,  2570,  2571,  2572,  2579,  2580,  2585,  2586,  2589,
    2593,  2597,  2599,  2601,  2605,  2606,  2611,  2614,  2617,  2620,
    2623,  2624,  2627,  2628,  2633,  2636,  2639,  2643,  2644,  2647,
    2651,  2652,  2659,  2663,  2667,  2670,  2671,  2674,  2678,  2679,
    2680,  2687,  2688,  2691,  2695,  2696,  2703,  2707,  2711,  2714,
    2715,  2718,  2722,  2723,  2724,  2733,  2734,  2735,  2743,  2744,
    2747,  2751,  2752,  2759,  2760,  2763,  2765,  2767,  2770,  2773,
    2774,  2777,  2779,  2781,  2784,  2788,  2792,  2797,  2801,  2804,
    2805,  2808,  2809,  2810,  2817,  2818,  2821,  2824,  2825,  2826,
    2835,  2836,  2841,  2842,  2847,  2848,  2854,  2856,  2857,  2860,
    2863,  2866,  2869,  2870,  2875,  2876,  2879,  2882,  2885,  2888,
    2892,  2896,  2899,  2900,  2903,  2904
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     274,     0,    -1,   275,   277,   278,   283,    -1,    -1,    -1,
     159,   276,     4,   266,    -1,    -1,     8,   170,   266,    -1,
       8,   171,   266,    -1,    -1,   278,   279,    -1,     1,    -1,
     280,    -1,   575,    -1,   715,    -1,   412,    -1,   576,    -1,
     374,    -1,   746,    -1,   411,    -1,   637,    -1,   557,    -1,
     619,    -1,   452,    -1,   768,    -1,   666,    -1,   702,    -1,
     398,    -1,   593,    -1,   735,    -1,   517,    -1,   788,    -1,
     653,    -1,   376,    -1,   286,    -1,   310,    -1,   311,    -1,
     313,    -1,   281,    -1,   316,    -1,   309,    -1,   288,    -1,
     373,    -1,   290,    -1,   318,    -1,   323,    -1,   291,    -1,
     352,    -1,   284,    -1,   362,    -1,   308,    -1,    -1,     9,
     282,     4,   266,    -1,    96,     9,    -1,    -1,    11,   285,
       4,   266,    -1,    -1,    13,   287,     4,   266,    -1,    -1,
      14,   289,     4,   266,    -1,     7,    -1,    -1,   162,   292,
     293,    96,   162,    -1,    -1,   293,   294,    -1,    -1,     9,
     295,     4,   304,   266,    -1,    -1,    76,   296,     4,   304,
     266,    -1,    -1,   182,   297,     4,   304,   266,    -1,    -1,
      41,   298,     4,   304,   266,    -1,    -1,    70,   299,     4,
     304,   266,    -1,    -1,   174,   300,     4,   304,   266,    -1,
      -1,    29,   301,     4,   304,   266,    -1,    -1,   183,   302,
       4,   304,   266,    -1,    -1,    59,   303,     4,   304,   266,
      -1,     1,   266,    -1,    -1,   165,   305,   546,   307,    -1,
      -1,   164,   306,   546,   307,    -1,   163,    -1,   163,     3,
      -1,   169,     4,    -1,    -1,     6,    -1,    12,    94,    95,
       6,   266,    -1,   160,     3,   266,    -1,   161,     3,   266,
      -1,    -1,    16,   312,     4,     6,     6,   315,    25,     6,
      26,     6,    27,     6,     6,   266,    -1,    -1,    17,   314,
       4,     6,     6,   315,    25,     6,    26,     6,    27,     6,
       6,   266,    -1,    61,    -1,    64,    -1,    62,    -1,    63,
      -1,    65,    -1,    68,    -1,    67,    -1,    66,    -1,    -1,
      18,   317,   392,   393,   394,   266,    -1,   319,   320,   322,
      -1,    20,     6,    -1,    -1,   320,   321,    -1,    21,     6,
      22,     6,   266,    -1,    96,    20,    -1,   324,   325,   351,
      -1,    19,     6,   266,    -1,    -1,   325,   326,    -1,    -1,
      -1,    -1,   267,   327,     4,   268,    76,   328,     4,   329,
     330,   266,    -1,    -1,   330,   331,    -1,   268,   144,    -1,
     375,    -1,   268,   145,     4,    -1,   268,   250,     3,    -1,
      -1,   268,   257,   332,     4,    -1,    -1,   268,   246,   333,
       4,    -1,   268,    47,   348,    -1,   268,   256,    -1,    -1,
      -1,   268,    28,   334,     4,   335,   342,   345,   395,   395,
      -1,    -1,    -1,   268,   255,   336,     4,   337,   344,   346,
     392,   393,   393,   394,    -1,    -1,   268,   258,   338,     4,
     343,   269,     6,     6,   270,    -1,   449,   395,   315,    -1,
     268,   189,     6,   349,    -1,   268,   190,     6,   349,    -1,
     268,   191,     6,   349,    -1,   268,   192,     6,   349,    -1,
      -1,   268,   193,     6,    28,   339,     4,    -1,    -1,   268,
     194,     6,    28,   340,     4,    -1,   268,   195,     6,   349,
      -1,    -1,   268,   196,     6,    28,   341,     4,    -1,   268,
     234,   347,    -1,    -1,   175,     6,    -1,    -1,   175,     6,
      -1,    -1,   175,     6,    -1,    -1,    58,     6,    -1,   242,
       6,    -1,    -1,    58,     6,    -1,   242,     6,    -1,   236,
      -1,   237,    -1,   238,    -1,   239,    -1,   197,    -1,   198,
      -1,   199,    -1,   200,    -1,   201,    -1,   202,    -1,   203,
      -1,   204,    -1,    -1,    -1,    28,   350,     4,    -1,    96,
      19,    -1,    -1,    -1,    29,   353,     4,     4,     6,     6,
     315,   354,   355,   357,   266,    -1,    -1,    25,     6,    26,
       6,   356,    -1,    -1,    27,     6,     6,    -1,    -1,   357,
     358,    -1,    -1,   268,   166,   359,   360,    -1,    -1,   360,
     361,    -1,     4,     6,    -1,     4,     3,    -1,     4,     4,
      -1,    -1,   364,     6,   363,    25,     6,    27,     6,   366,
     266,    -1,    23,   365,    -1,   172,    -1,   173,    -1,   367,
     369,    -1,    -1,   175,     6,   368,    -1,    -1,   178,    -1,
      -1,    -1,    28,   370,   372,   371,    -1,    -1,   372,   371,
      -1,     4,    -1,    24,   365,     6,    25,     6,    27,     6,
     266,    -1,   167,    -1,   268,   167,    -1,   377,   378,   397,
      -1,    10,     6,   266,    -1,    -1,   378,   379,    -1,    -1,
      -1,   267,   380,     4,   381,   382,   266,    -1,    -1,   382,
     383,    -1,    -1,   268,    30,   384,     4,   396,   395,   395,
      -1,    -1,    -1,   268,   255,   385,     4,   396,   386,   392,
     393,   393,   394,    -1,    -1,   268,    50,   387,     4,    -1,
      -1,    -1,   268,   259,   388,     4,   268,   240,     6,     6,
     268,   248,   389,     4,     4,     4,   268,   241,     6,     6,
     268,   244,     6,     6,     6,     6,    -1,   390,    -1,   375,
      -1,   268,   253,     6,     6,    -1,   268,   252,     6,     6,
      -1,   268,   251,     6,     6,     6,     6,    -1,    -1,   268,
      49,   391,     4,    -1,   395,    -1,   395,    -1,    -1,   394,
     393,    -1,   269,     6,     6,   270,    -1,   269,   271,     6,
     270,    -1,   269,     6,   271,   270,    -1,   269,   271,   271,
     270,    -1,    -1,   268,   175,     6,    -1,    96,    10,    -1,
     399,   400,    96,    42,    -1,    42,     6,   266,    -1,    -1,
     400,   401,    -1,    -1,    -1,   267,   402,     4,   403,   404,
     405,   266,    -1,   395,   395,    -1,   404,   395,   395,    -1,
      -1,   405,   406,    -1,    -1,   268,   166,   407,   408,    -1,
     268,   233,   410,    -1,    -1,   408,   409,    -1,     4,     6,
      -1,     4,     3,    -1,     4,     4,    -1,   228,    -1,   230,
      -1,   177,   414,   266,    -1,   413,   416,   451,    -1,    31,
       6,   266,    -1,    -1,   414,   415,    -1,     4,    -1,    -1,
     416,   417,    -1,   418,   422,   266,    -1,   419,   421,    -1,
      -1,   267,   420,     4,     4,    -1,    -1,   421,   271,    -1,
     421,     4,    -1,    -1,   422,   423,    -1,   427,    -1,   430,
      -1,   447,    -1,   450,    -1,   448,    -1,   444,    -1,   432,
      -1,   425,    -1,   434,    -1,   437,    -1,   439,    -1,   424,
      -1,   375,    -1,    -1,   268,    35,   426,     4,    -1,    -1,
     268,    32,   428,     4,   429,    -1,    -1,     4,    -1,   268,
      33,   431,    -1,   220,    -1,   221,    -1,   222,    -1,   223,
      -1,   443,   433,    -1,   443,     4,    -1,   395,   395,    -1,
     433,   395,   395,    -1,    -1,   268,   245,   435,   436,     6,
       6,     6,     6,    -1,    -1,    71,    -1,    -1,   268,   265,
       6,   438,     4,     4,    -1,    -1,   268,   166,   440,   441,
      -1,   442,    -1,   441,   442,    -1,     4,     6,    -1,     4,
       3,    -1,     4,     4,    -1,   268,    41,    -1,    -1,   268,
      40,   445,     4,   446,   315,    -1,   395,    -1,     6,     6,
      -1,   449,   395,   315,    -1,   268,    38,    -1,   268,    38,
     395,   315,    -1,   268,   176,     6,    -1,   268,    36,    -1,
     268,    37,    -1,   268,    39,    -1,   268,    34,     6,    -1,
      96,    31,    -1,   453,   454,   515,    -1,    43,     6,   266,
      -1,    -1,   454,   455,    -1,   456,   468,   266,    -1,   457,
      -1,    -1,   267,   458,   459,    -1,    -1,     4,   460,   462,
      -1,    -1,    45,   269,     4,   461,     4,   270,    -1,    -1,
     462,   463,    -1,    -1,   269,     4,   464,     4,   467,   270,
      -1,    -1,   269,   271,   465,     4,   467,   270,    -1,    -1,
     269,   125,   466,     4,   467,   270,    -1,    -1,   375,    -1,
     268,   126,    -1,    -1,   468,   469,    -1,    -1,   268,   492,
     470,   493,    -1,   268,    33,   483,    -1,   268,   227,    -1,
      -1,   268,   229,   471,     6,    -1,    -1,   268,    46,   472,
       4,    -1,   268,    49,   547,    -1,   268,    34,     6,    -1,
     268,   124,     6,    -1,   268,    51,     6,    -1,   268,    47,
     348,    -1,   268,    48,     6,    -1,    -1,   268,    59,   473,
       4,    -1,   484,    -1,    -1,   268,   186,   474,     4,    -1,
      -1,    -1,   268,   187,   475,   476,   493,    -1,    -1,    -1,
      -1,   268,   123,   477,     4,   478,   693,   479,   697,    -1,
      -1,   268,   166,   480,   481,    -1,   375,    -1,   482,    -1,
     481,   482,    -1,     4,     6,    -1,     4,     3,    -1,     4,
       4,    -1,   220,    -1,   221,    -1,   222,    -1,   223,    -1,
     180,    -1,    -1,   486,   488,   395,   395,   485,   490,    -1,
      -1,   268,   122,   487,     4,    -1,    -1,    -1,    28,   489,
       4,    -1,    -1,   491,   395,   315,    -1,    39,    -1,    36,
      -1,    37,    -1,    36,    -1,    37,    -1,    52,    -1,   496,
      -1,   493,   494,    -1,    -1,    53,   495,   496,    -1,    -1,
      -1,     4,   497,   508,   505,   498,   501,    -1,   188,   506,
      -1,    30,   507,    -1,    -1,   501,   502,    -1,     4,    -1,
     175,     6,     4,    -1,     4,   315,    -1,   175,     6,     4,
     315,    -1,   175,     6,     4,    25,     6,    26,     6,    27,
       6,     6,    -1,     4,    25,     6,    26,     6,    27,     6,
       6,    -1,     4,   315,    25,     6,    26,     6,    27,     6,
       6,    -1,   175,     6,     4,   315,    25,     6,    26,     6,
      27,     6,     6,    -1,   499,    -1,   500,    -1,    -1,   175,
       6,    30,   503,   269,     6,     6,     6,     6,   270,    -1,
      -1,   175,     6,   504,   505,    -1,   505,    -1,   269,     6,
       6,   270,    -1,   269,   271,     6,   270,    -1,   269,     6,
     271,   270,    -1,   269,   271,   271,   270,    -1,   269,     6,
       6,     6,   270,    -1,   269,   271,     6,     6,   270,    -1,
     269,     6,   271,     6,   270,    -1,   269,   271,   271,     6,
     270,    -1,   269,     6,     6,   270,    -1,   269,   271,     6,
     270,    -1,   269,     6,   271,   270,    -1,   269,     6,     6,
       6,     6,   270,    -1,    -1,   508,   509,    -1,   512,    -1,
     510,    -1,   157,    -1,    -1,   158,   511,     4,    -1,    48,
       6,    -1,    -1,   513,   514,    -1,   268,    55,   516,    -1,
     268,    48,     6,    -1,    96,    43,    -1,   205,    -1,   206,
      -1,   207,    -1,   208,    -1,   209,    -1,   210,    -1,   211,
      -1,   262,    -1,   235,    -1,   212,    -1,   213,    -1,   214,
      -1,   555,   518,   556,    -1,    -1,   518,   519,    -1,   456,
     520,   266,    -1,    -1,   520,   521,    -1,   536,    -1,   538,
      -1,   540,    -1,   522,    -1,   268,   492,    -1,    -1,   268,
     492,   523,   548,    -1,    -1,    -1,   268,   185,   524,     4,
     525,   534,    -1,   268,    55,   516,    -1,   268,   175,     6,
      -1,    -1,    -1,   268,   255,   526,     4,   527,   392,   393,
     393,   394,    -1,    -1,   268,    30,   528,     4,   395,   395,
      -1,    -1,    -1,   268,   258,   529,     4,   533,   530,   392,
     394,    -1,   268,    33,   431,    -1,   268,   227,    -1,   268,
     229,     6,    -1,    -1,   268,    46,   531,     4,    -1,   268,
      49,   547,    -1,   268,    34,     6,    -1,   268,    51,     6,
      -1,   268,    47,   348,    -1,   268,    48,     6,    -1,    -1,
     268,   166,   532,   543,    -1,   375,    -1,    -1,    61,    -1,
      64,    -1,    62,    -1,    63,    -1,    65,    -1,    68,    -1,
      67,    -1,    66,    -1,    -1,    -1,   535,   548,    -1,    -1,
     268,    56,   537,     4,     6,    -1,    -1,   268,    57,   539,
       4,    -1,    -1,    -1,   268,    58,   541,     4,     6,   542,
     545,    -1,   544,    -1,   543,   544,    -1,     4,     6,    -1,
       4,     3,    -1,     4,     4,    -1,    -1,   146,     6,     6,
      -1,    -1,   146,     6,     6,    -1,   224,    -1,   225,    -1,
     226,    -1,    91,    -1,   551,    -1,   548,   549,    -1,    -1,
      53,   550,   551,    -1,    -1,    -1,     4,   552,   554,   513,
     505,   553,   501,    -1,     6,    -1,    54,     6,   266,    -1,
      96,    54,    -1,   558,   559,   574,    -1,    69,     6,   266,
      -1,    -1,   559,   560,    -1,   561,   563,   565,   266,    -1,
      -1,   267,   562,     4,    -1,    -1,   563,   564,    -1,     4,
      -1,    -1,   565,   566,    -1,   268,    71,   572,    -1,    -1,
     268,   166,   567,   570,    -1,    -1,   268,    41,   568,   569,
      -1,   375,    -1,   395,   395,    -1,     4,    -1,    -1,   570,
     571,    -1,     4,     6,    -1,     4,     3,    -1,     4,     4,
      -1,    -1,   572,   573,    -1,    72,     6,    -1,    73,     6,
      -1,    74,     6,    -1,    96,    69,    -1,   577,   579,   592,
      -1,   578,   579,   591,    -1,    93,     6,   266,    -1,    75,
       6,   266,    -1,    -1,   579,   580,    -1,   581,    -1,   586,
      -1,   267,   582,   589,   266,    -1,    -1,    76,   583,     4,
      -1,    -1,    77,   584,     4,     4,     4,     4,    -1,    78,
     269,   585,   270,    -1,    79,   269,   585,   270,    -1,   582,
      -1,   585,   582,    -1,   585,   272,   582,    -1,    -1,   267,
      91,   587,     4,   588,    92,     6,   266,    -1,    -1,   268,
      -1,    -1,   589,   590,    -1,   268,    87,     6,    -1,   268,
      88,     6,    -1,   268,    89,     6,    -1,   268,    90,     6,
      -1,    96,    75,    -1,    96,    93,    -1,   594,   595,   618,
      -1,    80,     6,   266,    -1,    -1,   595,   596,    -1,   597,
     599,   266,    -1,    -1,   267,   598,     4,    -1,    -1,   599,
     601,    -1,    -1,     4,    -1,    -1,   268,    81,   602,     4,
     600,    -1,    -1,   268,    82,   603,   609,    -1,    -1,   268,
      83,   604,   613,    -1,    -1,   268,    84,   605,     4,   600,
      -1,    -1,   268,   181,   606,   608,    -1,    -1,   268,   232,
     607,     4,   617,    -1,   375,    -1,    -1,   269,     4,     4,
     270,    -1,   269,     4,     4,   270,   269,     4,     4,   270,
      -1,    -1,   609,   610,    -1,    -1,     4,   611,   612,    -1,
      -1,   269,     4,     4,   270,    -1,   269,     4,     4,   270,
     269,     4,     4,   270,    -1,   269,     4,     4,   270,   269,
       4,     4,   270,   269,     4,     4,   270,    -1,    -1,   613,
     614,    -1,    -1,     4,   615,   616,    -1,    -1,   269,     4,
       4,   270,    -1,   269,     4,     4,   270,   269,     4,     4,
     270,    -1,   269,     4,     4,   270,   269,     4,     4,   270,
     269,     4,     4,   270,    -1,    -1,   231,     6,    -1,    96,
      80,    -1,   620,   621,   636,    -1,    97,     6,   266,    -1,
      -1,   621,   622,    -1,   623,   625,   266,    -1,    -1,   267,
     269,   624,     4,     4,   270,    -1,    -1,   625,   626,    -1,
     268,   635,   100,     6,     6,    -1,   268,   635,   101,     6,
       6,    -1,   268,   102,     6,    -1,    -1,    -1,   268,   103,
     627,     4,   628,   629,    -1,   375,    -1,    -1,    -1,   632,
     105,   630,     4,   631,   634,    -1,    -1,    -1,   104,   633,
       4,    -1,    -1,   106,     6,    -1,    98,    -1,    99,    -1,
      96,    97,    -1,   638,   639,    96,   147,    -1,   147,     6,
     266,    -1,    -1,   639,   640,    -1,    -1,    -1,   267,   641,
       4,   643,   642,   644,   645,   266,    -1,   148,    -1,   149,
      -1,   150,    -1,   152,     6,    -1,   151,     6,    -1,   153,
       6,    -1,    -1,   645,   646,    -1,    -1,   268,   154,   647,
     649,    -1,    -1,   268,   155,   648,   649,    -1,    -1,   649,
     650,    -1,    -1,   269,   156,   651,     4,   270,    -1,    -1,
     269,    31,   652,     4,   270,    -1,   654,   655,   665,    -1,
     107,     6,   266,    -1,    -1,   655,   656,    -1,    -1,    -1,
     267,   104,   657,     4,     4,   105,   658,     4,     4,   266,
      -1,    -1,   267,   108,   659,     4,     4,   266,    -1,    -1,
     267,   109,   660,     4,   661,   266,    -1,   267,   184,   266,
      -1,    -1,    -1,   104,   662,     4,   105,   663,     4,    -1,
      -1,   108,   664,     4,    -1,    96,   107,    -1,   667,   668,
     692,    -1,   110,     6,   266,    -1,    -1,   668,   669,    -1,
     670,   675,   266,    -1,    -1,   267,   671,     4,   672,    -1,
      -1,   111,   673,   674,    -1,    -1,   118,    -1,   119,    -1,
      -1,   120,    -1,   121,    -1,    -1,   675,   676,    -1,    -1,
     268,   112,   677,     4,     4,   635,   683,    -1,    -1,   268,
     113,   678,     4,     4,   690,    -1,    -1,   268,   114,   679,
       4,   688,    -1,    -1,   268,   115,   680,     4,     4,   635,
     683,    -1,    -1,   268,   116,   681,     4,     4,   690,    -1,
      -1,   268,   117,   682,     4,   688,    -1,   375,    -1,    -1,
     685,    19,   684,   687,    -1,    -1,   685,   686,    -1,   151,
       6,     6,    -1,   152,     6,     6,    -1,    -1,   687,     4,
      -1,    -1,   688,   689,    -1,    87,     6,    -1,    89,     6,
      -1,    88,     6,    -1,    90,     6,    -1,   691,    -1,   690,
     691,    -1,    87,     6,     6,    -1,    89,     6,     6,    -1,
      88,     6,     6,    -1,    90,     6,     6,    -1,    96,   110,
      -1,    -1,   693,   694,    -1,    -1,   269,   695,     4,     4,
     696,   270,    -1,    -1,   268,   126,    -1,    -1,   697,   698,
      -1,    -1,   701,   699,   493,    -1,    -1,    59,   700,     4,
      -1,    36,    -1,    37,    -1,    52,    -1,   187,    -1,   703,
     706,   705,    -1,   179,     6,   704,    -1,    -1,   266,    -1,
      96,   179,    -1,    -1,   706,   707,    -1,    -1,    -1,   267,
     708,     4,     4,   709,   710,   266,    -1,    -1,   710,   711,
      -1,    -1,   268,   166,   712,   713,    -1,    -1,   713,   714,
      -1,     4,     6,    -1,     4,     3,    -1,     4,     4,    -1,
     716,   718,   717,    -1,   215,     6,   266,    -1,    96,   215,
      -1,    -1,   718,   719,    -1,   720,   733,   732,   266,    -1,
      -1,    -1,   267,    28,   721,     4,   722,   724,    -1,    -1,
     267,   216,   723,   729,    -1,    -1,   724,   725,    -1,   268,
      58,     6,    -1,   268,   242,     6,    -1,   726,    -1,   727,
      -1,   268,   175,     6,    -1,    -1,   268,   174,   728,     4,
      -1,   268,   217,    -1,   268,   218,    -1,   268,   219,    -1,
     268,   261,    -1,    -1,   729,   730,    -1,    -1,   268,   174,
     731,     4,    -1,   268,   219,    -1,   268,    71,    -1,   268,
     264,     6,    -1,    -1,   732,   733,    -1,    30,   395,   395,
      -1,    -1,   255,   734,   392,   393,   393,   394,    -1,   736,
     738,   737,    -1,   217,     6,   266,    -1,    96,   217,    -1,
      -1,   738,   739,    -1,   740,   743,   266,    -1,    -1,    -1,
     267,    28,   741,     4,   742,   744,    -1,    -1,   743,   744,
      -1,    30,   395,   395,    -1,    -1,   255,   745,   392,   393,
     393,   394,    -1,   747,   749,   748,    -1,   218,     6,   266,
      -1,    96,   218,    -1,    -1,   749,   750,    -1,   753,   756,
     266,    -1,    -1,    -1,   267,   258,   751,     4,   752,   763,
     762,   266,    -1,    -1,    -1,   267,    28,   754,     4,   755,
     759,   757,    -1,    -1,   756,   757,    -1,    30,   395,   395,
      -1,    -1,   255,   758,   392,   393,   393,   394,    -1,    -1,
     759,   760,    -1,   761,    -1,   766,    -1,   268,   263,    -1,
     392,   394,    -1,    -1,   763,   764,    -1,   765,    -1,   767,
      -1,   268,   263,    -1,   268,   175,     6,    -1,   268,   175,
       6,    -1,   769,   772,   771,   770,    -1,    60,     6,   266,
      -1,    96,    60,    -1,    -1,   771,   772,    -1,    -1,    -1,
     267,   773,     4,   774,   775,   266,    -1,    -1,   775,   776,
      -1,   268,   247,    -1,    -1,    -1,   268,    28,   777,     4,
      56,     6,   778,   782,    -1,    -1,   268,   258,   779,     4,
      -1,    -1,   268,   259,   780,     4,    -1,    -1,   268,   249,
     781,     4,     6,    -1,   784,    -1,    -1,   782,   783,    -1,
     243,     6,    -1,    58,     6,    -1,   260,     6,    -1,    -1,
     268,   166,   785,   786,    -1,    -1,   786,   787,    -1,     4,
       6,    -1,     4,     3,    -1,     4,     4,    -1,   789,   791,
     790,    -1,   254,     6,   266,    -1,    96,   254,    -1,    -1,
     791,   792,    -1,    -1,   267,    48,     6,   793,   392,   393,
     394,   266,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   315,   315,   318,   319,   319,   345,   346,   359,   377,
     378,   379,   382,   382,   382,   382,   383,   383,   383,   383,
     384,   384,   384,   385,   385,   385,   386,   386,   386,   387,
     387,   387,   387,   388,   391,   391,   391,   391,   392,   392,
     392,   393,   393,   393,   394,   394,   394,   395,   395,   395,
     395,   400,   400,   407,   432,   432,   438,   438,   444,   444,
     450,   457,   456,   472,   473,   476,   476,   485,   485,   494,
     494,   503,   503,   512,   512,   521,   521,   530,   530,   541,
     540,   551,   550,   571,   573,   573,   578,   578,   584,   589,
     594,   600,   601,   604,   612,   619,   626,   626,   638,   638,
     651,   652,   653,   654,   655,   656,   657,   658,   661,   660,
     673,   676,   688,   689,   692,   703,   706,   709,   715,   716,
     719,   720,   721,   719,   733,   734,   736,   742,   748,   754,
     775,   775,   795,   795,   815,   819,   840,   841,   840,   859,
     860,   859,   895,   895,   921,   932,   950,   968,   986,  1004,
    1004,  1022,  1022,  1041,  1059,  1059,  1077,  1094,  1095,  1109,
    1110,  1117,  1118,  1131,  1132,  1155,  1179,  1180,  1203,  1227,
    1232,  1237,  1242,  1248,  1250,  1252,  1254,  1256,  1258,  1260,
    1262,  1267,  1268,  1268,  1271,  1277,  1279,  1277,  1293,  1303,
    1341,  1344,  1352,  1353,  1356,  1356,  1360,  1361,  1364,  1376,
    1385,  1396,  1395,  1429,  1434,  1436,  1439,  1441,  1442,  1453,
    1454,  1457,  1458,  1458,  1461,  1462,  1465,  1471,  1500,  1506,
    1509,  1512,  1518,  1519,  1522,  1523,  1522,  1534,  1535,  1538,
    1538,  1545,  1546,  1545,  1572,  1572,  1582,  1584,  1582,  1607,
    1608,  1614,  1626,  1638,  1650,  1650,  1663,  1666,  1669,  1670,
    1673,  1680,  1686,  1692,  1699,  1700,  1703,  1709,  1715,  1721,
    1722,  1725,  1726,  1725,  1735,  1738,  1743,  1744,  1747,  1747,
    1750,  1765,  1766,  1769,  1784,  1793,  1803,  1805,  1808,  1825,
    1828,  1835,  1836,  1839,  1846,  1847,  1850,  1856,  1862,  1862,
    1870,  1871,  1876,  1882,  1883,  1886,  1886,  1886,  1886,  1886,
    1887,  1887,  1887,  1887,  1888,  1888,  1888,  1891,  1898,  1898,
    1904,  1904,  1912,  1913,  1916,  1922,  1924,  1926,  1928,  1933,
    1935,  1941,  1952,  1965,  1964,  1986,  1987,  2007,  2007,  2027,
    2027,  2031,  2032,  2035,  2046,  2055,  2065,  2068,  2068,  2083,
    2085,  2088,  2095,  2102,  2116,  2125,  2127,  2129,  2132,  2138,
    2144,  2147,  2154,  2155,  2158,  2169,  2173,  2173,  2176,  2175,
    2184,  2184,  2192,  2193,  2196,  2196,  2210,  2210,  2217,  2217,
    2226,  2227,  2234,  2239,  2240,  2244,  2243,  2262,  2265,  2282,
    2282,  2300,  2300,  2303,  2306,  2309,  2312,  2315,  2318,  2321,
    2321,  2332,  2334,  2334,  2337,  2338,  2337,  2384,  2390,  2400,
    2383,  2414,  2414,  2418,  2424,  2425,  2428,  2439,  2448,  2458,
    2460,  2462,  2464,  2466,  2470,  2469,  2480,  2480,  2483,  2484,
    2484,  2487,  2488,  2491,  2493,  2495,  2498,  2500,  2502,  2506,
    2510,  2513,  2513,  2519,  2538,  2518,  2552,  2569,  2586,  2587,
    2592,  2603,  2617,  2624,  2635,  2659,  2693,  2727,  2752,  2753,
    2754,  2754,  2765,  2764,  2774,  2785,  2793,  2800,  2807,  2813,
    2821,  2829,  2837,  2846,  2854,  2861,  2870,  2879,  2880,  2882,
    2883,  2886,  2890,  2890,  2895,  2915,  2916,  2920,  2924,  2944,
    2950,  2952,  2954,  2956,  2958,  2960,  2962,  2964,  2980,  2982,
    2984,  2986,  2989,  2992,  2993,  2996,  2999,  3000,  3003,  3003,
    3004,  3004,  3007,  3021,  3020,  3042,  3043,  3042,  3049,  3053,
    3059,  3060,  3059,  3100,  3100,  3133,  3134,  3133,  3166,  3169,
    3172,  3175,  3175,  3178,  3181,  3184,  3193,  3196,  3199,  3199,
    3203,  3207,  3208,  3209,  3210,  3211,  3212,  3213,  3214,  3215,
    3218,  3232,  3232,  3287,  3287,  3296,  3296,  3314,  3315,  3314,
    3322,  3323,  3326,  3338,  3347,  3357,  3358,  3363,  3364,  3367,
    3369,  3371,  3373,  3377,  3394,  3397,  3397,  3416,  3424,  3415,
    3432,  3437,  3444,  3451,  3454,  3460,  3461,  3464,  3470,  3470,
    3481,  3482,  3485,  3492,  3493,  3496,  3498,  3498,  3501,  3501,
    3503,  3509,  3519,  3524,  3525,  3528,  3539,  3548,  3558,  3559,
    3562,  3570,  3578,  3587,  3594,  3598,  3601,  3615,  3629,  3630,
    3633,  3634,  3644,  3657,  3657,  3662,  3662,  3667,  3672,  3678,
    3679,  3681,  3683,  3683,  3692,  3693,  3696,  3697,  3700,  3705,
    3710,  3715,  3721,  3732,  3743,  3746,  3752,  3753,  3756,  3762,
    3762,  3769,  3770,  3775,  3776,  3779,  3779,  3783,  3783,  3786,
    3785,  3794,  3794,  3798,  3798,  3800,  3800,  3818,  3825,  3826,
    3835,  3849,  3850,  3854,  3853,  3864,  3865,  3878,  3899,  3930,
    3931,  3935,  3934,  3943,  3944,  3957,  3978,  4010,  4011,  4014,
    4023,  4026,  4037,  4038,  4041,  4047,  4047,  4053,  4054,  4058,
    4063,  4068,  4073,  4074,  4073,  4082,  4089,  4090,  4088,  4096,
    4097,  4097,  4103,  4104,  4110,  4110,  4112,  4118,  4124,  4130,
    4131,  4134,  4135,  4134,  4139,  4141,  4144,  4146,  4148,  4150,
    4153,  4154,  4158,  4157,  4161,  4160,  4165,  4166,  4168,  4168,
    4170,  4170,  4173,  4177,  4184,  4185,  4188,  4189,  4188,  4197,
    4197,  4205,  4205,  4213,  4219,  4220,  4219,  4225,  4225,  4231,
    4238,  4241,  4248,  4249,  4252,  4258,  4258,  4264,  4265,  4272,
    4273,  4275,  4279,  4280,  4282,  4285,  4286,  4289,  4289,  4295,
    4295,  4301,  4301,  4307,  4307,  4313,  4313,  4319,  4319,  4324,
    4332,  4331,  4335,  4336,  4339,  4344,  4350,  4351,  4354,  4355,
    4357,  4359,  4361,  4363,  4367,  4368,  4371,  4374,  4377,  4380,
    4384,  4388,  4389,  4392,  4392,  4401,  4402,  4405,  4406,  4409,
    4408,  4421,  4421,  4424,  4426,  4428,  4430,  4433,  4435,  4441,
    4442,  4445,  4449,  4450,  4453,  4454,  4453,  4463,  4464,  4466,
    4466,  4470,  4471,  4474,  4485,  4494,  4504,  4506,  4510,  4514,
    4515,  4518,  4527,  4528,  4527,  4547,  4546,  4563,  4564,  4567,
    4593,  4615,  4616,  4619,  4628,  4628,  4647,  4668,  4689,  4707,
    4739,  4740,  4745,  4745,  4763,  4781,  4815,  4851,  4852,  4855,
    4861,  4860,  4886,  4888,  4892,  4896,  4897,  4900,  4908,  4909,
    4908,  4916,  4917,  4920,  4926,  4925,  4938,  4940,  4944,  4948,
    4949,  4952,  4959,  4960,  4959,  4969,  4970,  4969,  4978,  4979,
    4982,  4988,  4987,  5006,  5007,  5009,  5010,  5015,  5034,  5044,
    5045,  5047,  5048,  5053,  5073,  5083,  5093,  5096,  5114,  5118,
    5119,  5122,  5123,  5122,  5133,  5134,  5137,  5142,  5144,  5142,
    5151,  5151,  5157,  5157,  5163,  5163,  5169,  5172,  5173,  5176,
    5182,  5188,  5196,  5196,  5200,  5201,  5204,  5215,  5224,  5235,
    5237,  5254,  5258,  5259,  5263,  5262
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "QSTRING", "T_STRING", "SITE_PATTERN",
  "NUMBER", "K_HISTORY", "K_NAMESCASESENSITIVE", "K_DESIGN", "K_VIAS",
  "K_TECH", "K_UNITS", "K_ARRAY", "K_FLOORPLAN", "K_SITE", "K_CANPLACE",
  "K_CANNOTOCCUPY", "K_DIEAREA", "K_PINS", "K_DEFAULTCAP", "K_MINPINS",
  "K_WIRECAP", "K_TRACKS", "K_GCELLGRID", "K_DO", "K_BY", "K_STEP",
  "K_LAYER", "K_ROW", "K_RECT", "K_COMPS", "K_COMP_GEN", "K_SOURCE",
  "K_WEIGHT", "K_EEQMASTER", "K_FIXED", "K_COVER", "K_UNPLACED",
  "K_PLACED", "K_FOREIGN", "K_REGION", "K_REGIONS", "K_NETS",
  "K_START_NET", "K_MUSTJOIN", "K_ORIGINAL", "K_USE", "K_STYLE",
  "K_PATTERN", "K_PATTERNNAME", "K_ESTCAP", "K_ROUTED", "K_NEW", "K_SNETS",
  "K_SHAPE", "K_WIDTH", "K_VOLTAGE", "K_SPACING", "K_NONDEFAULTRULE",
  "K_NONDEFAULTRULES", "K_N", "K_S", "K_E", "K_W", "K_FN", "K_FE", "K_FS",
  "K_FW", "K_GROUPS", "K_GROUP", "K_SOFT", "K_MAXX", "K_MAXY",
  "K_MAXHALFPERIMETER", "K_CONSTRAINTS", "K_NET", "K_PATH", "K_SUM",
  "K_DIFF", "K_SCANCHAINS", "K_START", "K_FLOATING", "K_ORDERED", "K_STOP",
  "K_IN", "K_OUT", "K_RISEMIN", "K_RISEMAX", "K_FALLMIN", "K_FALLMAX",
  "K_WIREDLOGIC", "K_MAXDIST", "K_ASSERTIONS", "K_DISTANCE", "K_MICRONS",
  "K_END", "K_IOTIMINGS", "K_RISE", "K_FALL", "K_VARIABLE", "K_SLEWRATE",
  "K_CAPACITANCE", "K_DRIVECELL", "K_FROMPIN", "K_TOPIN", "K_PARALLEL",
  "K_TIMINGDISABLES", "K_THRUPIN", "K_MACRO", "K_PARTITIONS", "K_TURNOFF",
  "K_FROMCLOCKPIN", "K_FROMCOMPPIN", "K_FROMIOPIN", "K_TOCLOCKPIN",
  "K_TOCOMPPIN", "K_TOIOPIN", "K_SETUPRISE", "K_SETUPFALL", "K_HOLDRISE",
  "K_HOLDFALL", "K_VPIN", "K_SUBNET", "K_XTALK", "K_PIN", "K_SYNTHESIZED",
  "K_DEFINE", "K_DEFINES", "K_DEFINEB", "K_IF", "K_THEN", "K_ELSE",
  "K_FALSE", "K_TRUE", "K_EQ", "K_NE", "K_LE", "K_LT", "K_GE", "K_GT",
  "K_OR", "K_AND", "K_NOT", "K_SPECIAL", "K_DIRECTION", "K_RANGE", "K_FPC",
  "K_HORIZONTAL", "K_VERTICAL", "K_ALIGN", "K_MIN", "K_MAX", "K_EQUAL",
  "K_BOTTOMLEFT", "K_TOPRIGHT", "K_ROWS", "K_TAPER", "K_TAPERRULE",
  "K_VERSION", "K_DIVIDERCHAR", "K_BUSBITCHARS", "K_PROPERTYDEFINITIONS",
  "K_STRING", "K_REAL", "K_INTEGER", "K_PROPERTY", "K_BEGINEXT",
  "K_ENDEXT", "K_NAMEMAPSTRING", "K_ON", "K_OFF", "K_X", "K_Y",
  "K_COMPONENT", "K_MASK", "K_MASKSHIFT", "K_COMPSMASKSHIFT", "K_SAMEMASK",
  "K_PINPROPERTIES", "K_TEST", "K_COMMONSCANPINS", "K_SNET",
  "K_COMPONENTPIN", "K_REENTRANTPATHS", "K_SHIELD", "K_SHIELDNET",
  "K_NOSHIELD", "K_VIRTUAL", "K_ANTENNAPINPARTIALMETALAREA",
  "K_ANTENNAPINPARTIALMETALSIDEAREA", "K_ANTENNAPINGATEAREA",
  "K_ANTENNAPINDIFFAREA", "K_ANTENNAPINMAXAREACAR",
  "K_ANTENNAPINMAXSIDEAREACAR", "K_ANTENNAPINPARTIALCUTAREA",
  "K_ANTENNAPINMAXCUTCAR", "K_SIGNAL", "K_POWER", "K_GROUND", "K_CLOCK",
  "K_TIEOFF", "K_ANALOG", "K_SCAN", "K_RESET", "K_RING", "K_STRIPE",
  "K_FOLLOWPIN", "K_IOWIRE", "K_COREWIRE", "K_BLOCKWIRE", "K_FILLWIRE",
  "K_BLOCKAGEWIRE", "K_PADRING", "K_BLOCKRING", "K_BLOCKAGES",
  "K_PLACEMENT", "K_SLOTS", "K_FILLS", "K_PUSHDOWN", "K_NETLIST", "K_DIST",
  "K_USER", "K_TIMING", "K_BALANCED", "K_STEINER", "K_TRUNK",
  "K_FIXEDBUMP", "K_FENCE", "K_FREQUENCY", "K_GUIDE", "K_MAXBITS",
  "K_PARTITION", "K_TYPE", "K_ANTENNAMODEL", "K_DRCFILL", "K_OXIDE1",
  "K_OXIDE2", "K_OXIDE3", "K_OXIDE4", "K_CUTSIZE", "K_CUTSPACING",
  "K_DESIGNRULEWIDTH", "K_DIAGWIDTH", "K_ENCLOSURE", "K_HALO",
  "K_GROUNDSENSITIVITY", "K_HARDSPACING", "K_LAYERS", "K_MINCUTS",
  "K_NETEXPR", "K_OFFSET", "K_ORIGIN", "K_ROWCOL", "K_STYLES", "K_POLYGON",
  "K_PORT", "K_SUPPLYSENSITIVITY", "K_VIA", "K_VIARULE", "K_WIREEXT",
  "K_EXCEPTPGNET", "K_FILLWIREOPC", "K_OPC", "K_PARTIAL", "K_ROUTEHALO",
  "';'", "'-'", "'+'", "'('", "')'", "'*'", "','", "$accept", "def_file",
  "version_stmt", "$@1", "case_sens_stmt", "rules", "rule",
  "design_section", "design_name", "$@2", "end_design", "tech_name", "$@3",
  "array_name", "$@4", "floorplan_name", "$@5", "history",
  "prop_def_section", "$@6", "property_defs", "property_def", "$@7", "$@8",
  "$@9", "$@10", "$@11", "$@12", "$@13", "$@14", "$@15",
  "property_type_and_val", "$@16", "$@17", "opt_num_val", "units",
  "divider_char", "bus_bit_chars", "canplace", "$@18", "cannotoccupy",
  "$@19", "orient", "die_area", "$@20", "pin_cap_rule", "start_def_cap",
  "pin_caps", "pin_cap", "end_def_cap", "pin_rule", "start_pins", "pins",
  "pin", "$@21", "$@22", "$@23", "pin_options", "pin_option", "$@24",
  "$@25", "$@26", "$@27", "$@28", "$@29", "$@30", "$@31", "$@32", "$@33",
  "pin_layer_mask_opt", "pin_via_mask_opt", "pin_poly_mask_opt",
  "pin_layer_spacing_opt", "pin_poly_spacing_opt", "pin_oxide", "use_type",
  "pin_layer_opt", "$@34", "end_pins", "row_rule", "$@35", "$@36",
  "row_do_option", "row_step_option", "row_options", "row_option", "$@37",
  "row_prop_list", "row_prop", "tracks_rule", "$@38", "track_start",
  "track_type", "track_opts", "track_mask_statement", "same_mask",
  "track_layer_statement", "$@39", "track_layers", "track_layer",
  "gcellgrid", "extension_section", "extension_stmt", "via_section", "via",
  "via_declarations", "via_declaration", "$@40", "$@41", "layer_stmts",
  "layer_stmt", "$@42", "$@43", "$@44", "$@45", "$@46", "$@47",
  "layer_viarule_opts", "$@48", "firstPt", "nextPt", "otherPts", "pt",
  "mask", "via_end", "regions_section", "regions_start", "regions_stmts",
  "regions_stmt", "$@49", "$@50", "rect_list", "region_options",
  "region_option", "$@51", "region_prop_list", "region_prop",
  "region_type", "comps_maskShift_section", "comps_section", "start_comps",
  "layer_statement", "maskLayer", "comps_rule", "comp", "comp_start",
  "comp_id_and_name", "$@52", "comp_net_list", "comp_options",
  "comp_option", "comp_extension_stmt", "comp_eeq", "$@53",
  "comp_generate", "$@54", "opt_pattern", "comp_source", "source_type",
  "comp_region", "comp_pnt_list", "comp_halo", "$@55", "halo_soft",
  "comp_routehalo", "$@56", "comp_property", "$@57", "comp_prop_list",
  "comp_prop", "comp_region_start", "comp_foreign", "$@58", "opt_paren",
  "comp_type", "maskShift", "placement_status", "weight", "end_comps",
  "nets_section", "start_nets", "net_rules", "one_net",
  "net_and_connections", "net_start", "$@59", "net_name", "$@60", "$@61",
  "net_connections", "net_connection", "$@62", "$@63", "$@64", "conn_opt",
  "net_options", "net_option", "$@65", "$@66", "$@67", "$@68", "$@69",
  "$@70", "$@71", "$@72", "$@73", "$@74", "$@75", "net_prop_list",
  "net_prop", "netsource_type", "vpin_stmt", "$@76", "vpin_begin", "$@77",
  "vpin_layer_opt", "$@78", "vpin_options", "vpin_status", "net_type",
  "paths", "new_path", "$@79", "path", "$@80", "$@81", "virtual_statement",
  "rect_statement", "path_item_list", "path_item", "$@82", "$@83",
  "path_pt", "virtual_pt", "rect_pts", "opt_taper_style_s",
  "opt_taper_style", "opt_taper", "$@84", "opt_style", "opt_spaths",
  "opt_shape_style", "end_nets", "shape_type", "snets_section",
  "snet_rules", "snet_rule", "snet_options", "snet_option",
  "snet_other_option", "$@85", "$@86", "$@87", "$@88", "$@89", "$@90",
  "$@91", "$@92", "$@93", "$@94", "orient_pt", "shield_layer", "$@95",
  "snet_width", "$@96", "snet_voltage", "$@97", "snet_spacing", "$@98",
  "$@99", "snet_prop_list", "snet_prop", "opt_snet_range", "opt_range",
  "pattern_type", "spaths", "snew_path", "$@100", "spath", "$@101",
  "$@102", "width", "start_snets", "end_snets", "groups_section",
  "groups_start", "group_rules", "group_rule", "start_group", "$@103",
  "group_members", "group_member", "group_options", "group_option",
  "$@104", "$@105", "group_region", "group_prop_list", "group_prop",
  "group_soft_options", "group_soft_option", "groups_end",
  "assertions_section", "constraint_section", "assertions_start",
  "constraints_start", "constraint_rules", "constraint_rule",
  "operand_rule", "operand", "$@106", "$@107", "operand_list",
  "wiredlogic_rule", "$@108", "opt_plus", "delay_specs", "delay_spec",
  "constraints_end", "assertions_end", "scanchains_section",
  "scanchain_start", "scanchain_rules", "scan_rule", "start_scan", "$@109",
  "scan_members", "opt_pin", "scan_member", "$@110", "$@111", "$@112",
  "$@113", "$@114", "$@115", "opt_common_pins", "floating_inst_list",
  "one_floating_inst", "$@116", "floating_pins", "ordered_inst_list",
  "one_ordered_inst", "$@117", "ordered_pins", "partition_maxbits",
  "scanchain_end", "iotiming_section", "iotiming_start", "iotiming_rules",
  "iotiming_rule", "start_iotiming", "$@118", "iotiming_members",
  "iotiming_member", "$@119", "$@120", "iotiming_drivecell_opt", "$@121",
  "$@122", "iotiming_frompin", "$@123", "iotiming_parallel", "risefall",
  "iotiming_end", "floorplan_contraints_section", "fp_start", "fp_stmts",
  "fp_stmt", "$@124", "$@125", "h_or_v", "constraint_type",
  "constrain_what_list", "constrain_what", "$@126", "$@127",
  "row_or_comp_list", "row_or_comp", "$@128", "$@129",
  "timingdisables_section", "timingdisables_start", "timingdisables_rules",
  "timingdisables_rule", "$@130", "$@131", "$@132", "$@133",
  "td_macro_option", "$@134", "$@135", "$@136", "timingdisables_end",
  "partitions_section", "partitions_start", "partition_rules",
  "partition_rule", "start_partition", "$@137", "turnoff", "turnoff_setup",
  "turnoff_hold", "partition_members", "partition_member", "$@138",
  "$@139", "$@140", "$@141", "$@142", "$@143", "minmaxpins", "$@144",
  "min_or_max_list", "min_or_max_member", "pin_list",
  "risefallminmax1_list", "risefallminmax1", "risefallminmax2_list",
  "risefallminmax2", "partitions_end", "comp_names", "comp_name", "$@145",
  "subnet_opt_syn", "subnet_options", "subnet_option", "$@146", "$@147",
  "subnet_type", "pin_props_section", "begin_pin_props", "opt_semi",
  "end_pin_props", "pin_prop_list", "pin_prop_terminal", "$@148", "$@149",
  "pin_prop_options", "pin_prop", "$@150", "pin_prop_name_value_list",
  "pin_prop_name_value", "blockage_section", "blockage_start",
  "blockage_end", "blockage_defs", "blockage_def", "blockage_rule",
  "$@151", "$@152", "$@153", "layer_blockage_rules", "layer_blockage_rule",
  "mask_blockage_rule", "comp_blockage_rule", "$@154",
  "placement_comp_rules", "placement_comp_rule", "$@155",
  "rectPoly_blockage_rules", "rectPoly_blockage", "$@156", "slot_section",
  "slot_start", "slot_end", "slot_defs", "slot_def", "slot_rule", "$@157",
  "$@158", "geom_slot_rules", "geom_slot", "$@159", "fill_section",
  "fill_start", "fill_end", "fill_defs", "fill_def", "$@160", "$@161",
  "fill_rule", "$@162", "$@163", "geom_fill_rules", "geom_fill", "$@164",
  "fill_layer_mask_opc_opt", "opt_mask_opc_l", "fill_layer_opc",
  "fill_via_pt", "fill_via_mask_opc_opt", "opt_mask_opc", "fill_via_opc",
  "fill_mask", "fill_viaMask", "nondefaultrule_section",
  "nondefault_start", "nondefault_end", "nondefault_defs",
  "nondefault_def", "$@165", "$@166", "nondefault_options",
  "nondefault_option", "$@167", "$@168", "$@169", "$@170", "$@171",
  "nondefault_layer_options", "nondefault_layer_option",
  "nondefault_prop_opt", "$@172", "nondefault_prop_list",
  "nondefault_prop", "styles_section", "styles_start", "styles_end",
  "styles_rules", "styles_rule", "$@173", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,    59,    45,    43,    40,
      41,    42,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   273,   274,   275,   276,   275,   277,   277,   277,   278,
     278,   278,   279,   279,   279,   279,   279,   279,   279,   279,
     279,   279,   279,   279,   279,   279,   279,   279,   279,   279,
     279,   279,   279,   279,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   282,   281,   283,   285,   284,   287,   286,   289,   288,
     290,   292,   291,   293,   293,   295,   294,   296,   294,   297,
     294,   298,   294,   299,   294,   300,   294,   301,   294,   302,
     294,   303,   294,   294,   305,   304,   306,   304,   304,   304,
     304,   307,   307,   308,   309,   310,   312,   311,   314,   313,
     315,   315,   315,   315,   315,   315,   315,   315,   317,   316,
     318,   319,   320,   320,   321,   322,   323,   324,   325,   325,
     327,   328,   329,   326,   330,   330,   331,   331,   331,   331,
     332,   331,   333,   331,   331,   331,   334,   335,   331,   336,
     337,   331,   338,   331,   331,   331,   331,   331,   331,   339,
     331,   340,   331,   331,   341,   331,   331,   342,   342,   343,
     343,   344,   344,   345,   345,   345,   346,   346,   346,   347,
     347,   347,   347,   348,   348,   348,   348,   348,   348,   348,
     348,   349,   350,   349,   351,   353,   354,   352,   355,   355,
     356,   356,   357,   357,   359,   358,   360,   360,   361,   361,
     361,   363,   362,   364,   365,   365,   366,   367,   367,   368,
     368,   369,   370,   369,   371,   371,   372,   373,   374,   375,
     376,   377,   378,   378,   380,   381,   379,   382,   382,   384,
     383,   385,   386,   383,   387,   383,   388,   389,   383,   383,
     383,   390,   390,   390,   391,   390,   392,   393,   394,   394,
     395,   395,   395,   395,   396,   396,   397,   398,   399,   400,
     400,   402,   403,   401,   404,   404,   405,   405,   407,   406,
     406,   408,   408,   409,   409,   409,   410,   410,   411,   412,
     413,   414,   414,   415,   416,   416,   417,   418,   420,   419,
     421,   421,   421,   422,   422,   423,   423,   423,   423,   423,
     423,   423,   423,   423,   423,   423,   423,   424,   426,   425,
     428,   427,   429,   429,   430,   431,   431,   431,   431,   432,
     432,   433,   433,   435,   434,   436,   436,   438,   437,   440,
     439,   441,   441,   442,   442,   442,   443,   445,   444,   446,
     446,   447,   447,   447,   448,   449,   449,   449,   450,   451,
     452,   453,   454,   454,   455,   456,   458,   457,   460,   459,
     461,   459,   462,   462,   464,   463,   465,   463,   466,   463,
     467,   467,   467,   468,   468,   470,   469,   469,   469,   471,
     469,   472,   469,   469,   469,   469,   469,   469,   469,   473,
     469,   469,   474,   469,   475,   476,   469,   477,   478,   479,
     469,   480,   469,   469,   481,   481,   482,   482,   482,   483,
     483,   483,   483,   483,   485,   484,   487,   486,   488,   489,
     488,   490,   490,   491,   491,   491,   492,   492,   492,   493,
     493,   495,   494,   497,   498,   496,   499,   500,   501,   501,
     502,   502,   502,   502,   502,   502,   502,   502,   502,   502,
     503,   502,   504,   502,   502,   505,   505,   505,   505,   505,
     505,   505,   505,   506,   506,   506,   507,   508,   508,   509,
     509,   510,   511,   510,   512,   513,   513,   514,   514,   515,
     516,   516,   516,   516,   516,   516,   516,   516,   516,   516,
     516,   516,   517,   518,   518,   519,   520,   520,   521,   521,
     521,   521,   522,   523,   522,   524,   525,   522,   522,   522,
     526,   527,   522,   528,   522,   529,   530,   522,   522,   522,
     522,   531,   522,   522,   522,   522,   522,   522,   532,   522,
     522,   533,   533,   533,   533,   533,   533,   533,   533,   533,
     534,   535,   534,   537,   536,   539,   538,   541,   542,   540,
     543,   543,   544,   544,   544,   545,   545,   546,   546,   547,
     547,   547,   547,   548,   548,   550,   549,   552,   553,   551,
     554,   555,   556,   557,   558,   559,   559,   560,   562,   561,
     563,   563,   564,   565,   565,   566,   567,   566,   568,   566,
     566,   569,   569,   570,   570,   571,   571,   571,   572,   572,
     573,   573,   573,   574,   575,   576,   577,   578,   579,   579,
     580,   580,   581,   583,   582,   584,   582,   582,   582,   585,
     585,   585,   587,   586,   588,   588,   589,   589,   590,   590,
     590,   590,   591,   592,   593,   594,   595,   595,   596,   598,
     597,   599,   599,   600,   600,   602,   601,   603,   601,   604,
     601,   605,   601,   606,   601,   607,   601,   601,   608,   608,
     608,   609,   609,   611,   610,   612,   612,   612,   612,   613,
     613,   615,   614,   616,   616,   616,   616,   617,   617,   618,
     619,   620,   621,   621,   622,   624,   623,   625,   625,   626,
     626,   626,   627,   628,   626,   626,   630,   631,   629,   632,
     633,   632,   634,   634,   635,   635,   636,   637,   638,   639,
     639,   641,   642,   640,   643,   643,   644,   644,   644,   644,
     645,   645,   647,   646,   648,   646,   649,   649,   651,   650,
     652,   650,   653,   654,   655,   655,   657,   658,   656,   659,
     656,   660,   656,   656,   662,   663,   661,   664,   661,   665,
     666,   667,   668,   668,   669,   671,   670,   672,   672,   673,
     673,   673,   674,   674,   674,   675,   675,   677,   676,   678,
     676,   679,   676,   680,   676,   681,   676,   682,   676,   676,
     684,   683,   685,   685,   686,   686,   687,   687,   688,   688,
     689,   689,   689,   689,   690,   690,   691,   691,   691,   691,
     692,   693,   693,   695,   694,   696,   696,   697,   697,   699,
     698,   700,   698,   701,   701,   701,   701,   702,   703,   704,
     704,   705,   706,   706,   708,   709,   707,   710,   710,   712,
     711,   713,   713,   714,   714,   714,   715,   716,   717,   718,
     718,   719,   721,   722,   720,   723,   720,   724,   724,   725,
     725,   725,   725,   726,   728,   727,   727,   727,   727,   727,
     729,   729,   731,   730,   730,   730,   730,   732,   732,   733,
     734,   733,   735,   736,   737,   738,   738,   739,   741,   742,
     740,   743,   743,   744,   745,   744,   746,   747,   748,   749,
     749,   750,   751,   752,   750,   754,   755,   753,   756,   756,
     757,   758,   757,   759,   759,   760,   760,   761,   762,   763,
     763,   764,   764,   765,   766,   767,   768,   769,   770,   771,
     771,   773,   774,   772,   775,   775,   776,   777,   778,   776,
     779,   776,   780,   776,   781,   776,   776,   782,   782,   783,
     783,   783,   785,   784,   786,   786,   787,   787,   787,   788,
     789,   790,   791,   791,   793,   792
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     0,     0,     4,     0,     3,     3,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     4,     2,     0,     4,     0,     4,     0,     4,
       1,     0,     5,     0,     2,     0,     5,     0,     5,     0,
       5,     0,     5,     0,     5,     0,     5,     0,     5,     0,
       5,     0,     5,     2,     0,     4,     0,     4,     1,     2,
       2,     0,     1,     5,     3,     3,     0,    14,     0,    14,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     6,
       3,     2,     0,     2,     5,     2,     3,     3,     0,     2,
       0,     0,     0,    10,     0,     2,     2,     1,     3,     3,
       0,     4,     0,     4,     3,     2,     0,     0,     9,     0,
       0,    11,     0,     9,     3,     4,     4,     4,     4,     0,
       6,     0,     6,     4,     0,     6,     3,     0,     2,     0,
       2,     0,     2,     0,     2,     2,     0,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     0,     3,     2,     0,     0,    11,     0,     5,
       0,     3,     0,     2,     0,     4,     0,     2,     2,     2,
       2,     0,     9,     2,     1,     1,     2,     0,     3,     0,
       1,     0,     0,     4,     0,     2,     1,     8,     1,     2,
       3,     3,     0,     2,     0,     0,     6,     0,     2,     0,
       7,     0,     0,    10,     0,     4,     0,     0,    24,     1,
       1,     4,     4,     6,     0,     4,     1,     1,     0,     2,
       4,     4,     4,     4,     0,     3,     2,     4,     3,     0,
       2,     0,     0,     7,     2,     3,     0,     2,     0,     4,
       3,     0,     2,     2,     2,     2,     1,     1,     3,     3,
       3,     0,     2,     1,     0,     2,     3,     2,     0,     4,
       0,     2,     2,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     4,
       0,     5,     0,     1,     3,     1,     1,     1,     1,     2,
       2,     2,     3,     0,     8,     0,     1,     0,     6,     0,
       4,     1,     2,     2,     2,     2,     2,     0,     6,     1,
       2,     3,     2,     4,     3,     2,     2,     2,     3,     2,
       3,     3,     0,     2,     3,     1,     0,     3,     0,     3,
       0,     6,     0,     2,     0,     6,     0,     6,     0,     6,
       0,     1,     2,     0,     2,     0,     4,     3,     2,     0,
       4,     0,     4,     3,     3,     3,     3,     3,     3,     0,
       4,     1,     0,     4,     0,     0,     5,     0,     0,     0,
       8,     0,     4,     1,     1,     2,     2,     2,     2,     1,
       1,     1,     1,     1,     0,     6,     0,     4,     0,     0,
       3,     0,     3,     1,     1,     1,     1,     1,     1,     1,
       2,     0,     3,     0,     0,     6,     2,     2,     0,     2,
       1,     3,     2,     4,    10,     8,     9,    11,     1,     1,
       0,    10,     0,     4,     1,     4,     4,     4,     4,     5,
       5,     5,     5,     4,     4,     4,     6,     0,     2,     1,
       1,     1,     0,     3,     2,     0,     2,     3,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     0,     2,     3,     0,     2,     1,     1,
       1,     1,     2,     0,     4,     0,     0,     6,     3,     3,
       0,     0,     9,     0,     6,     0,     0,     8,     3,     2,
       3,     0,     4,     3,     3,     3,     3,     3,     0,     4,
       1,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     0,     2,     0,     5,     0,     4,     0,     0,     7,
       1,     2,     2,     2,     2,     0,     3,     0,     3,     1,
       1,     1,     1,     1,     2,     0,     3,     0,     0,     7,
       1,     3,     2,     3,     3,     0,     2,     4,     0,     3,
       0,     2,     1,     0,     2,     3,     0,     4,     0,     4,
       1,     2,     1,     0,     2,     2,     2,     2,     0,     2,
       2,     2,     2,     2,     3,     3,     3,     3,     0,     2,
       1,     1,     4,     0,     3,     0,     6,     4,     4,     1,
       2,     3,     0,     8,     0,     1,     0,     2,     3,     3,
       3,     3,     2,     2,     3,     3,     0,     2,     3,     0,
       3,     0,     2,     0,     1,     0,     5,     0,     4,     0,
       4,     0,     5,     0,     4,     0,     5,     1,     0,     4,
       8,     0,     2,     0,     3,     0,     4,     8,    12,     0,
       2,     0,     3,     0,     4,     8,    12,     0,     2,     2,
       3,     3,     0,     2,     3,     0,     6,     0,     2,     5,
       5,     3,     0,     0,     6,     1,     0,     0,     6,     0,
       0,     3,     0,     2,     1,     1,     2,     4,     3,     0,
       2,     0,     0,     8,     1,     1,     1,     2,     2,     2,
       0,     2,     0,     4,     0,     4,     0,     2,     0,     5,
       0,     5,     3,     3,     0,     2,     0,     0,    10,     0,
       6,     0,     6,     3,     0,     0,     6,     0,     3,     2,
       3,     3,     0,     2,     3,     0,     4,     0,     3,     0,
       1,     1,     0,     1,     1,     0,     2,     0,     7,     0,
       6,     0,     5,     0,     7,     0,     6,     0,     5,     1,
       0,     4,     0,     2,     3,     3,     0,     2,     0,     2,
       2,     2,     2,     2,     1,     2,     3,     3,     3,     3,
       2,     0,     2,     0,     6,     0,     2,     0,     2,     0,
       3,     0,     3,     1,     1,     1,     1,     3,     3,     0,
       1,     2,     0,     2,     0,     0,     7,     0,     2,     0,
       4,     0,     2,     2,     2,     2,     3,     3,     2,     0,
       2,     4,     0,     0,     6,     0,     4,     0,     2,     3,
       3,     1,     1,     3,     0,     4,     2,     2,     2,     2,
       0,     2,     0,     4,     2,     2,     3,     0,     2,     3,
       0,     6,     3,     3,     2,     0,     2,     3,     0,     0,
       6,     0,     2,     3,     0,     6,     3,     3,     2,     0,
       2,     3,     0,     0,     8,     0,     0,     7,     0,     2,
       3,     0,     6,     0,     2,     1,     1,     2,     2,     0,
       2,     1,     1,     2,     3,     3,     4,     3,     2,     0,
       2,     0,     0,     6,     0,     2,     2,     0,     0,     8,
       0,     4,     0,     4,     0,     5,     1,     0,     2,     2,
       2,     2,     0,     4,     0,     2,     2,     2,     2,     3,
       3,     2,     0,     2,     0,     8
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       3,     4,     0,     6,     0,     1,     0,     0,     0,     0,
       0,    11,     0,     5,     7,     8,    60,    51,     0,    54,
       0,    56,    58,    96,    98,   108,     0,     0,     0,     0,
     185,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    61,   218,   281,
       0,     0,     0,     0,     0,    10,    12,    38,     2,    48,
      34,    41,    43,    46,    50,    40,    35,    36,    37,    39,
      44,   112,    45,   118,    47,    49,     0,    42,    17,    33,
     222,    27,   259,    19,    15,   284,    23,   352,    30,   493,
      21,   575,    13,    16,   608,   608,    28,   636,    22,   682,
      20,   709,    32,   734,    25,   752,    26,   822,    14,   839,
      29,   875,    18,   889,    24,     0,    31,   952,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   111,   204,
     205,   203,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    53,     0,     0,     0,     0,     0,     0,
      63,     0,   819,     0,     0,     0,     0,     0,     0,   201,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   921,   919,     0,
       0,   221,     0,     0,     0,     0,     0,     0,     0,     0,
     246,   117,     0,     0,   280,   258,   351,   571,   917,   574,
     607,   635,   606,   681,   733,   751,   708,    94,    95,     0,
     283,   278,   282,   820,   818,   837,   873,   887,   950,     0,
       0,   113,   110,     0,   120,   119,   116,     0,     0,   224,
     223,   220,     0,   261,   260,     0,   288,   285,   293,   290,
     279,     0,   356,   353,   373,   355,   350,     0,   496,   494,
     492,     0,   578,   576,   580,   573,     0,     0,   609,   610,
     611,   604,     0,   605,     0,   639,   637,   641,   634,     0,
       0,   683,   687,   680,     0,   711,   710,     0,     0,   735,
     732,     0,   755,   753,   765,   750,     0,   824,   817,   823,
       0,     0,   836,   840,     0,     0,     0,   872,   876,   881,
       0,     0,   886,   890,   898,     0,     0,     0,     0,   949,
     953,    52,    55,     0,    57,    59,     0,     0,     0,     0,
     248,   247,     0,     0,     0,    65,    77,    71,    81,    73,
      67,     0,    75,    69,    79,    64,     0,   115,   184,     0,
       0,   256,     0,   257,     0,   349,     0,     0,   287,   479,
       0,     0,   572,     0,   603,     0,   583,   633,   613,   615,
       0,     0,   622,   626,   632,   679,     0,     0,   706,   685,
       0,   707,     0,   749,   736,   739,   741,     0,   800,     0,
       0,   821,     0,   838,   842,   845,     0,   870,   867,   874,
     878,     0,   888,   895,   892,     0,   922,     0,   916,   920,
     951,     0,    93,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    83,     0,     0,     0,     0,     0,     0,    62,
       0,     0,     0,     0,     0,     0,   225,   262,     0,   286,
       0,   307,   294,   306,   302,   295,   296,   301,   303,   304,
     305,     0,   300,   297,   299,     0,   298,   292,   291,   358,
       0,   357,   354,     0,   403,   374,   391,   418,   495,     0,
     530,   497,   501,   498,   499,   500,   579,   582,   581,     0,
       0,     0,     0,     0,     0,     0,   640,   638,     0,   657,
     642,     0,   684,     0,   695,   688,     0,     0,     0,     0,
     743,   757,   754,     0,   779,   766,     0,     0,   860,     0,
       0,     0,     0,     0,   884,   877,   882,     0,     0,     0,
     901,   891,   899,   924,   918,   954,   100,   102,   103,   101,
     104,   107,   106,   105,     0,     0,   250,   252,   251,   253,
     109,   249,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   227,     0,   289,   310,
       0,     0,   308,   345,   346,   342,   347,   337,   336,   329,
     219,     0,   323,     0,   320,     0,   319,     0,   362,     0,
       0,     0,   426,   427,   381,     0,     0,     0,     0,   428,
     389,   416,   397,     0,   401,   392,   394,   378,   379,   375,
     419,     0,   513,     0,     0,   521,     0,     0,     0,     0,
       0,   543,   545,   547,   528,     0,   505,   519,     0,   510,
     515,   502,   577,     0,   590,   584,   614,     0,   619,     0,
       0,   624,   612,     0,   627,   645,   647,   649,   651,   653,
     655,     0,   704,   705,     0,   692,     0,   714,   715,   712,
       0,     0,     0,   759,   756,   767,   769,   771,   773,   775,
     777,   825,   843,   846,   869,     0,   841,   868,   879,     0,
       0,   896,   893,     0,     0,     0,     0,     0,     0,     0,
     186,    88,    86,    84,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   114,   121,   207,     0,     0,   266,
       0,   315,   316,   317,   318,   314,   348,     0,     0,     0,
       0,   344,   325,   327,   321,     0,   341,   359,   360,   413,
     409,   410,   411,   412,   377,   384,     0,   173,   174,   175,
     176,   177,   178,   179,   180,   387,   388,   562,   559,   560,
     561,   383,   386,     0,     0,     0,   385,     0,     0,   395,
       0,     0,     0,     0,     0,   518,   524,     0,   526,   527,
     523,   525,   480,   481,   482,   483,   484,   485,   486,   489,
     490,   491,   488,   487,   508,     0,     0,     0,     0,   509,
       0,   520,     0,     0,     0,   588,   598,   586,     0,   617,
       0,   620,   618,   625,     0,     0,     0,     0,     0,     0,
     661,   669,     0,   658,     0,     0,   691,     0,     0,     0,
       0,     0,     0,   744,   747,     0,   760,   761,   762,     0,
       0,     0,     0,     0,     0,   827,   847,     0,   861,     0,
       0,   883,     0,   903,   909,   900,     0,   923,     0,   925,
     936,     0,     0,     0,   217,   188,    89,   557,   557,    90,
      66,    78,    72,    82,    74,    68,    76,    70,    80,     0,
       0,     0,   211,   226,     0,   240,   228,   239,   264,     0,
       0,   312,   309,   343,     0,     0,   330,   331,   326,     0,
       0,   322,     0,   363,     0,   382,   390,   417,   398,     0,
     402,   404,   393,     0,   380,   433,   376,   429,   420,   414,
       0,   522,     0,   546,     0,     0,   529,   550,   506,   511,
     531,   567,   504,   563,     0,   585,   593,     0,   621,     0,
     628,   629,   630,   631,   643,   648,   650,   643,     0,   654,
     677,   686,   693,     0,     0,   716,     0,     0,     0,   720,
     737,   740,     0,     0,   742,   763,   764,   758,     0,     0,
     788,     0,     0,   788,     0,   844,   865,   862,   864,     0,
     248,   880,     0,     0,     0,     0,   927,   942,   926,   934,
     930,   932,   248,     0,     0,     0,   192,     0,    91,    91,
     122,   209,   202,   212,   206,   229,   244,   234,     0,     0,
       0,   231,   236,   265,   263,     0,   267,   313,   311,     0,
     339,     0,   334,   335,   333,   332,     0,     0,   364,   368,
     366,     0,   801,   407,   408,   406,   405,   396,   467,   431,
     430,   421,     0,   544,   548,   553,   554,   552,   551,   540,
       0,   532,   534,   535,   533,   536,   539,   538,   537,   516,
       0,   565,   564,   592,     0,   589,     0,     0,     0,   599,
     587,   616,     0,   644,   646,   663,   662,   671,   670,   652,
       0,     0,   656,   699,   689,   690,   718,   717,   719,     0,
       0,     0,   748,     0,     0,   772,     0,     0,   778,   826,
       0,   828,     0,   848,   851,   852,     0,   866,   871,   248,
       0,   897,   904,   905,   906,     0,   248,     0,   910,   911,
     912,   248,     0,   944,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    92,    87,    85,   124,   210,   208,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   268,     0,
     340,   338,     0,   328,     0,     0,     0,   361,   399,     0,
       0,   424,   425,   423,   415,     0,   514,   555,   507,     0,
       0,     0,   570,   475,     0,   591,   600,   601,   602,     0,
     594,   623,   665,   673,     0,   678,   700,   694,     0,   713,
       0,   721,     0,   745,   782,     0,     0,     0,     0,   770,
     794,     0,     0,     0,     0,   789,   782,   776,   829,     0,
     854,     0,   856,   857,   858,     0,   859,   863,   885,     0,
     907,     0,   913,   908,   894,   902,     0,   943,     0,   931,
     933,   955,     0,     0,     0,   187,     0,   193,   558,     0,
     216,   214,   254,   245,   235,     0,   242,   241,   254,     0,
     271,   276,   277,   270,     0,   370,   370,   370,   803,   807,
     802,     0,   471,   472,     0,   434,   468,   470,   469,   432,
       0,     0,   549,   542,     0,   248,     0,   566,   596,   597,
     595,     0,   664,     0,   672,   659,     0,   696,   722,   724,
       0,     0,   768,     0,     0,     0,     0,     0,   795,   790,
     792,   791,   793,   774,   831,   849,     0,   853,   850,   914,
     915,     0,     0,   945,   935,     0,     0,   190,   194,   123,
       0,   125,   127,     0,   213,   214,     0,     0,     0,   232,
       0,   269,   324,     0,   371,     0,     0,     0,     0,   400,
     474,     0,     0,     0,   438,   422,     0,   248,   517,     0,
     568,   476,     0,     0,     0,   701,     0,   726,   726,   738,
     746,   780,     0,     0,   783,   796,   798,   797,   799,   830,
     855,   928,   947,   948,   946,     0,     0,     0,   189,   196,
     136,     0,   126,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   132,     0,   139,   135,   130,   142,     0,
     215,     0,     0,   243,     0,     0,     0,   272,   372,   365,
     369,   367,     0,   813,   814,   815,   811,   816,   808,   809,
     473,     0,     0,     0,     0,   435,   556,   512,     0,     0,
     438,     0,     0,     0,   697,   723,   725,   786,     0,     0,
       0,   832,   937,    97,    99,     0,   195,     0,   134,   128,
     181,   181,   181,   181,     0,     0,   181,     0,   169,   170,
     171,   172,   156,     0,   129,     0,     0,     0,   144,   255,
     230,     0,     0,   274,   275,   273,   805,     0,     0,     0,
     455,     0,   457,     0,   456,     0,   458,   440,     0,     0,
       0,   448,   449,   439,   454,   478,   477,   569,   666,   674,
       0,   702,     0,   727,   781,   784,   785,   834,   835,   833,
     929,   191,     0,   197,   137,   182,   145,   146,   147,   148,
     149,   151,   153,   154,   133,   140,   131,   159,     0,     0,
       0,     0,   812,   810,   459,   461,   460,   462,     0,   442,
       0,   437,   452,     0,   436,     0,     0,   660,     0,   698,
     730,   728,   787,     0,     0,     0,   938,   199,   200,   198,
     157,     0,     0,     0,     0,   161,     0,     0,   248,     0,
     806,   804,     0,     0,     0,   441,   450,     0,     0,     0,
       0,     0,   703,     0,     0,   940,   939,   941,     0,   163,
     183,   150,   152,   155,     0,   166,   160,     0,   233,   237,
       0,     0,     0,     0,   443,     0,   453,     0,     0,     0,
       0,     0,     0,     0,   158,     0,     0,     0,   162,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     463,   465,   464,   667,   675,   731,   729,   164,   165,     0,
     167,   168,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   138,     0,   143,     0,     0,     0,   466,
       0,     0,     0,     0,     0,   248,     0,   445,     0,     0,
       0,     0,     0,     0,   141,     0,   446,     0,     0,     0,
     668,   676,     0,   444,     0,   451,     0,   447,     0,     0,
       0,     0,     0,     0,   238
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,     7,    12,    55,    56,    57,   118,
      58,    59,   120,    60,   122,    61,   123,    62,    63,   150,
     209,   335,   413,   418,   421,   415,   417,   420,   414,   422,
     416,   675,   838,   837,  1104,    64,    65,    66,    67,   124,
      68,   125,   524,    69,   126,    70,    71,   157,   221,   222,
      72,    73,   158,   225,   339,   849,  1106,  1209,  1291,  1436,
    1433,  1417,  1530,  1435,  1535,  1437,  1532,  1533,  1534,  1559,
    1537,  1565,  1587,  1591,  1432,   725,  1486,  1531,   226,    74,
     133,   835,   966,  1348,  1101,  1207,  1349,  1416,  1483,    75,
     227,    76,   131,   851,   852,  1108,   974,  1109,  1294,  1295,
      77,    78,  1304,    79,    80,   160,   230,   342,   546,   687,
     856,  1110,  1116,  1374,  1112,  1117,  1593,   857,  1111,   189,
     531,   409,   321,  1297,   231,    81,    82,   161,   234,   344,
     547,   689,   860,   986,  1220,  1301,  1377,  1223,    83,    84,
      85,   151,   212,   162,   237,   238,   239,   346,   348,   347,
     432,   433,   434,   697,   435,   690,   988,   436,   695,   437,
     566,   438,   702,   869,   439,   870,   440,   700,   866,   867,
     441,   442,   699,   991,   443,   444,   445,   446,   240,    86,
      87,   163,   243,   244,   245,   350,   451,   568,   874,   707,
     873,  1124,  1126,  1125,  1305,   351,   455,   741,   740,   716,
     733,   738,   739,   883,   735,  1002,  1229,   737,   880,   881,
     714,   456,  1011,   457,   734,   591,   742,  1134,  1135,   589,
     886,  1010,  1130,   887,  1008,  1314,  1461,  1462,  1395,  1463,
    1575,  1547,  1464,  1514,  1511,  1129,  1236,  1237,  1311,  1238,
    1246,  1321,   246,   764,    88,   164,   249,   353,   461,   462,
     774,   770,  1019,   772,  1020,   744,   773,  1141,   747,   768,
    1029,  1138,  1139,   463,   765,   464,   766,   465,   767,  1137,
     896,   897,  1242,   968,   731,   902,  1032,  1144,   903,  1030,
    1400,  1143,    89,   250,    90,    91,   165,   253,   254,   355,
     356,   468,   469,   615,   906,   904,  1035,  1040,  1150,   905,
    1039,   255,    92,    93,    94,    95,   166,   258,   259,   618,
     470,   471,   619,   260,   474,   784,   475,   624,   263,   261,
      96,    97,   168,   266,   267,   366,   367,  1044,   480,   789,
     790,   791,   792,   793,   794,   919,   915,  1046,  1152,  1252,
     916,  1048,  1153,  1254,  1052,   268,    98,    99,   169,   271,
     272,   481,   370,   485,   797,  1053,  1157,  1326,  1471,  1158,
    1256,  1519,   636,   273,   100,   101,   170,   276,   372,   800,
     639,   929,  1059,  1161,  1327,  1328,  1405,  1473,  1554,  1553,
     102,   103,   171,   279,   487,  1060,   488,   489,   805,   932,
    1261,   933,   280,   104,   105,   172,   283,   284,   379,   644,
     808,   937,   380,   495,   809,   810,   811,   812,   813,   814,
    1262,  1407,  1263,  1334,  1474,  1065,  1175,  1169,  1170,   285,
    1128,  1230,  1308,  1501,  1309,  1388,  1448,  1447,  1389,   106,
     107,   214,   288,   173,   289,   382,   815,   944,  1071,  1274,
    1339,  1411,   108,   109,   292,   174,   293,   294,   497,   816,
     498,   945,  1073,  1074,  1075,  1276,   653,   818,  1076,   501,
     388,   500,   110,   111,   297,   175,   298,   299,   502,   820,
     391,   506,   660,   112,   113,   302,   176,   303,   508,   824,
     304,   507,   823,   395,   512,   664,   953,  1082,  1083,  1087,
     954,  1088,  1089,  1084,  1090,   114,   115,   398,   306,   178,
     305,   513,   665,   829,  1092,  1412,  1095,  1096,  1094,  1480,
    1526,   830,  1093,  1197,  1283,   116,   117,   309,   179,   310,
     666
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -1120
static const yytype_int16 yypact[] =
{
     -67, -1120,    97,   127,   141, -1120,   -58,    46,  -112,  -100,
     -96, -1120,   338, -1120, -1120, -1120, -1120, -1120,   191, -1120,
     111, -1120, -1120, -1120, -1120, -1120,   231,   238,   -32,   -32,
   -1120,   242,   252,   271,   279,   282,   302,   316,   324,   332,
     245,   340,   347,   354,   370,   385,   394, -1120, -1120, -1120,
     393,   406,   421,   427,   431, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,   436, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120,   180, -1120, -1120,   447,   214,
     487,   407,   502,   507,   519,   538,   285,   294, -1120, -1120,
   -1120, -1120,   560,   566,   306,   307,   308,   309,   310,   311,
     315,   317,   318, -1120,   319,   320,   323,   327,   329,   333,
   -1120,     9,   348,   352,   365,   367,   371,    65,   -52, -1120,
     -46,   -35,   -29,   -24,   -22,   -20,   -18,   -17,   -11,    -5,
       3,     6,    24,    37,    38,    50,    56, -1120, -1120,    66,
     372, -1120,   375,   572,   376,   379,   576,   584,    12,   285,
   -1120, -1120,   609,   587, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   330,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   630,
     620, -1120, -1120,   624, -1120, -1120, -1120,   635,   648, -1120,
   -1120, -1120,   625, -1120, -1120,   641, -1120, -1120, -1120, -1120,
   -1120,   623, -1120, -1120, -1120, -1120, -1120,   619, -1120, -1120,
   -1120,   612, -1120, -1120, -1120, -1120,   589,   325, -1120, -1120,
   -1120, -1120,   608, -1120,   604, -1120, -1120, -1120, -1120,   588,
     417, -1120, -1120, -1120,   541, -1120, -1120,   582,     1, -1120,
   -1120,   580, -1120, -1120, -1120, -1120,   512, -1120, -1120, -1120,
     477,    17, -1120, -1120,    10,   485,   675, -1120, -1120, -1120,
     486,    14, -1120, -1120, -1120,   728,    67,   480,   687, -1120,
   -1120, -1120, -1120,   470, -1120, -1120,   731,   734,    13,    15,
   -1120, -1120,   735,   736,   478, -1120, -1120, -1120, -1120, -1120,
   -1120,   581, -1120, -1120, -1120, -1120,   723, -1120, -1120,   742,
     741, -1120,   744, -1120,   749, -1120,   750,  -137,     8, -1120,
      83,   -74, -1120,   -42, -1120,   751,   752, -1120, -1120, -1120,
     488,   489, -1120, -1120, -1120, -1120,   756,    61, -1120, -1120,
     104, -1120,   757, -1120, -1120, -1120, -1120,   496, -1120,   759,
     116, -1120,   762, -1120, -1120, -1120,   285, -1120, -1120, -1120,
   -1120,   -13, -1120, -1120, -1120,     0, -1120,   707, -1120, -1120,
   -1120,   763, -1120,   633,   633,   500,   501,   503,   505,  -162,
     745,   768, -1120,   772,   773,   775,   776,   777,   778, -1120,
     779,   780,   781,   782,   518,   760, -1120, -1120,   785, -1120,
     140, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,    18, -1120, -1120, -1120,   285, -1120, -1120, -1120, -1120,
     521, -1120, -1120,   628, -1120, -1120, -1120,   764, -1120,   564,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   164,
     792,   793,   287,   287,   794,   170, -1120, -1120,   312, -1120,
   -1120,   795, -1120,   288, -1120, -1120,    71,   796,   797,   798,
   -1120,   692, -1120,   357, -1120, -1120,   800,   801, -1120,   285,
     285,     2,   802,   285, -1120, -1120, -1120,   803,   804,   285,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120,   784,   786, -1120, -1120, -1120, -1120,
   -1120, -1120,   806,   633,    53,    53,    53,    53,    53,    53,
      53,    53,    53,   544,   737,   810, -1120,   285, -1120, -1120,
     202,   811, -1120, -1120, -1120,   285, -1120, -1120, -1120, -1120,
   -1120,   812, -1120,   814, -1120,   285,   285,   633, -1120,   817,
     188,   818, -1120, -1120, -1120,   509,   819,   -37,   820, -1120,
   -1120, -1120, -1120,   821, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   285, -1120,   202,   823, -1120,   509,   824,   -37,   825,
     443, -1120, -1120, -1120, -1120,   826, -1120, -1120,   827, -1120,
   -1120,   830, -1120,    43, -1120, -1120, -1120,   831, -1120,     4,
      40,   555, -1120,   374, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   832, -1120, -1120,   833, -1120,   181, -1120, -1120, -1120,
     834,   836,    87,   195, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,   573, -1120,   285, -1120, -1120, -1120,   285,
     285, -1120, -1120,   285,   285,   224,   285,   837,   838,   579,
   -1120,   839, -1120, -1120,   842,   583,   585,   586,   590,   592,
     593,   594,   595,   596, -1120, -1120,   672,   235,   285,   285,
     844, -1120, -1120, -1120, -1120, -1120, -1120,   846,   633,   849,
     850, -1120,   799, -1120, -1120,   285, -1120,   597, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,   859, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,   860,   863,   864, -1120,   865,   867, -1120,
     866,   869,   870,   285,   871, -1120, -1120,   872, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120,   873,   874,   876,   877, -1120,
     878, -1120,   879,   880,   881, -1120, -1120, -1120,   883, -1120,
     287, -1120, -1120, -1120,   805,   882,   884,   885,   886,   889,
   -1120, -1120,   891,   627,   894,   629, -1120,   896,   895,   897,
     331,   807,   636, -1120, -1120,   638, -1120, -1120,   299,   901,
     902,   903,   905,   906,   907, -1120, -1120,   -23, -1120,   285,
      16, -1120,   285, -1120, -1120, -1120,   285, -1120,    62, -1120,
   -1120,   285,   887,   888, -1120,   890, -1120,   743,   743, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   912,
     911,   652,   892, -1120,    73, -1120, -1120, -1120, -1120,   285,
     248,   915, -1120, -1120,    33,   208,   850, -1120, -1120,   916,
     917, -1120,     7, -1120,   919, -1120, -1120, -1120, -1120,   411,
     865, -1120, -1120,   869, -1120, -1120,   875, -1120, -1120, -1120,
     285, -1120,   918, -1120,   920,   440,   877, -1120, -1120, -1120,
     653, -1120,   893, -1120,    20,   157, -1120,   921, -1120,   923,
   -1120, -1120, -1120, -1120,   926,   927,   928,   926,   929, -1120,
     696, -1120, -1120,   930,   931, -1120,   932,   933,   934, -1120,
   -1120, -1120,   937,   938, -1120, -1120, -1120, -1120,   939,   940,
   -1120,   941,   943, -1120,   254,   666, -1120, -1120, -1120,   942,
   -1120, -1120,   285,   -16,   189,   285, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,   944,   945,   946, -1120,   948,   949,   949,
   -1120,   771, -1120, -1120, -1120, -1120, -1120, -1120,   950,   951,
     952, -1120, -1120, -1120, -1120,   -95, -1120, -1120, -1120,   953,
   -1120,   633, -1120, -1120, -1120, -1120,   954,   957, -1120, -1120,
   -1120,   665, -1120, -1120, -1120, -1120, -1120,   875, -1120, -1120,
   -1120,   305,   285, -1120, -1120, -1120, -1120, -1120, -1120,   958,
     285, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
     959, -1120, -1120, -1120,   285, -1120,   961,   962,   963, -1120,
     960, -1120,   697, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
     966,   965, -1120,   868, -1120, -1120, -1120, -1120, -1120,   262,
     969,   898, -1120,   368,   388,   399,   368,   388,   399, -1120,
     808, -1120,   -15, -1120, -1120, -1120,   971, -1120,   285, -1120,
     -81, -1120, -1120, -1120, -1120,   -79, -1120,   710, -1120, -1120,
   -1120, -1120,   973, -1120,   974,   975,   976,  -158,   955,   956,
     964,   275,   978, -1120, -1120, -1120, -1120, -1120, -1120,   977,
     981,   982,   983,   985,   986,   987,   984,   990, -1120,    39,
   -1120, -1120,   989, -1120,   992,   993,   994, -1120,   720,   -10,
     869, -1120, -1120, -1120, -1120,   285, -1120,   853, -1120,   881,
     285,   285, -1120, -1120,   881, -1120, -1120, -1120, -1120,   450,
   -1120, -1120,   732,   733,   730, -1120, -1120, -1120,   899, -1120,
     377, -1120,  1001, -1120, -1120,  1000,  1002,  1004,  1005,   388,
   -1120,  1006,  1007,  1008,  1010, -1120, -1120,   388, -1120,  1011,
   -1120,  1012, -1120, -1120, -1120,  1013, -1120, -1120,   285,  1014,
   -1120,  1015, -1120,   285, -1120,   285,   967,  1003,  1016, -1120,
   -1120, -1120,  1018,  1019,  1020, -1120,   861, -1120, -1120,   293,
   -1120,   977,   761, -1120, -1120,  1022, -1120, -1120,   761,   765,
   -1120, -1120, -1120, -1120,  1024,   766,   766,   766, -1120, -1120,
   -1120,  1025, -1120, -1120,    19, -1120, -1120, -1120, -1120, -1120,
     633,  1026, -1120,   893,   285, -1120,   267, -1120, -1120, -1120,
   -1120,  1031, -1120,  1032, -1120,   769,  1033, -1120, -1120, -1120,
     774,  1035, -1120,    49,  1036,  1037,  1038,  1039, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,  1042, -1120, -1120, -1120,
   -1120,  1041,   504, -1120, -1120,  1043,  1044,  1021, -1120, -1120,
     413, -1120, -1120,   285, -1120,   977,   900,   285,  1045, -1120,
     813,  1048, -1120,   -31, -1120,   788,   789,   790,  1051,   131,
   -1120,  1052,    21,    22, -1120, -1120,  1055, -1120,   285,   102,
   -1120, -1120,  1058,  1059,  1060, -1120,  1061, -1120, -1120, -1120,
   -1120, -1120,  1062,  1063, -1120, -1120, -1120, -1120, -1120,  1066,
   -1120, -1120, -1120, -1120, -1120,   815,   816,  1065, -1120, -1120,
   -1120,   509, -1120,  1068,  1067,  1070,  1071,  1072,  1073,  1074,
    1077,  1078,   258, -1120,  1064, -1120, -1120, -1120, -1120,   633,
   -1120,  1079,   285, -1120,   285,  1080,   515, -1120, -1120, -1120,
   -1120, -1120,  1083, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,    27,    28,    29,    31,    11, -1120,   285,  1082,   443,
   -1120,   822,   828,  1085, -1120,   835,   835, -1120,  1084,  1087,
     523, -1120, -1120, -1120, -1120,  1088,  1091,  1093, -1120, -1120,
    1046,  1046,  1046,  1046,  1075,  1081,  1046,  1086, -1120, -1120,
   -1120, -1120, -1120,  1095, -1120,  1096,  1097,  1098, -1120, -1120,
   -1120,   285,  1099, -1120, -1120, -1120,   840,  1102,   869,   841,
   -1120,   843, -1120,   845, -1120,   847, -1120,   484,   851,  1101,
     852, -1120, -1120, -1120, -1120, -1120, -1120,    11,   854,   855,
     848,   935,    42, -1120,  1106, -1120, -1120, -1120, -1120, -1120,
      -9, -1120,   561, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120,   947,   285,   857,
    1009,   856, -1120,   875, -1120, -1120, -1120, -1120,  1110,  1094,
    1121, -1120,    94,    23, -1120,  1108,  1124, -1120,  1123, -1120,
   -1120, -1120, -1120,  1125,  1126,  1127, -1120, -1120, -1120, -1120,
     968,  1130,  1132,  1133,  1134,   970,  1135,   904, -1120,   908,
   -1120, -1120,  1040,  1138,  1140,   562, -1120,   909,    25,  1141,
    1136,  1144, -1120,  1145,  1146, -1120, -1120, -1120,  1147,    -7,
   -1120, -1120, -1120, -1120,  1148,    -6, -1120,  1149,   285, -1120,
    1151,  1104,  1152,  1153,  1114,   910, -1120,   913,   914,   922,
     924,   925,   936,   972, -1120,  1154,  1155,   285, -1120,  1156,
    1157,   285,  1158,  1161,  1139,  1162,  1163,  1150,  1164,  1166,
   -1120, -1120, -1120,   980,   988, -1120, -1120, -1120, -1120,   285,
   -1120, -1120,   285,   991,  1170,  1169,  1159,   995,  1171,  1165,
    1174,  1177,  1178, -1120,   285, -1120,  1181,  1182,  1183, -1120,
    1160,  1184,  1187,  1192,  1193, -1120,   979, -1120,  1194,  1195,
    1172,  1196,   996,   997,   285,   998, -1120,  1197,  1198,   999,
   -1120, -1120,  1199, -1120,  1201, -1120,  1202, -1120,  1017,  1027,
    1203,  1204,  1205,  1206, -1120
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   187, -1120, -1120,   122, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120,  -403, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120,  -592, -1086, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120,  1122, -1120, -1120, -1120, -1120, -1120,  -143,    89,
   -1120, -1120,  -344, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,  -495,
    -187,  -942,  -126,    -4, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   622, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   350,
   -1120, -1120, -1120, -1120, -1120, -1120,    26, -1120, -1120, -1120,
   -1120, -1120, -1120,  1049, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120,  -658, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   337,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   767,
    -877, -1120, -1120,    88, -1120, -1120, -1120, -1120,  -181, -1120,
   -1120, -1120, -1119, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,  -179, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   326, -1120,   383,   626,    84, -1120, -1120,    81, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,  1137, -1120, -1120,  -241,
   -1120, -1120,   754, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120,   313, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120,  -908, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,   -99, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
      52, -1120, -1120, -1120, -1120,   290, -1120,   167, -1076, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
     739, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   416, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120,   278, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,  1023,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -542
static const yytype_int16 yytable[] =
{
     190,   525,   320,   431,   748,   655,  1007,   454,  1078,   460,
    1235,   998,   447,   210,   509,  1457,   363,   503,   318,   405,
    1097,   407,   564,   479,  1033,  1312,   484,  1391,  1393,  1548,
     509,  1577,   386,  1449,  1451,  1453,   494,  1455,  1231,   989,
     386,  1458,   393,  1179,   223,   384,   503,    11,   946,  1523,
     228,  1585,  1589,    -9,   727,    -9,    -9,    -9,    -9,    -9,
      -9,   232,    -9,    -9,    -9,    -9,    -9,   235,  1331,    -9,
      -9,  1118,   241,  1520,   247,    -9,   251,    -9,   256,   262,
     358,   359,   360,   361,   775,   264,   219,   449,    -9,    -9,
     956,   269,     1,  1268,  1189,  1378,  1191,     5,  1545,   274,
      -9,  1268,   277,   975,   530,   374,    -9,   188,  1201,   375,
     376,   188,     9,    10,   776,    -9,   358,   359,   360,   361,
     281,    -9,   976,   977,  1546,   614,    -9,  1320,   450,   429,
     670,   430,   999,   286,   290,     6,   560,  1188,  1119,    -9,
     129,   130,    -9,    -9,  1193,     8,   295,  1232,  1233,  1195,
    1398,   947,   300,    -9,    13,  1164,    -9,  1399,  1176,  1180,
    1181,   220,   307,   397,   706,   822,    14,  1383,  1384,   826,
      15,   831,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,  1190,  1385,  1192,   377,  1459,   728,   729,   730,
    1386,   803,   452,    -9,   453,   804,   948,   119,  1521,  1460,
    1332,  1333,  1182,  1183,  1184,   121,    -9,    -9,    -9,   777,
     560,   992,   993,    -9,   994,   224,   671,   672,   673,   637,
     638,   229,   674,    -9,   458,    -9,   459,  1185,   957,  1036,
    1037,  1038,   233,   385,  1524,  1586,  1590,   127,   236,   510,
     560,   949,   504,   242,   128,   242,  1186,   252,   134,   257,
     257,  1525,  1080,   505,   143,   510,   265,   387,   135,  1234,
     499,    -9,   270,    -9,    -9,   387,   511,  1221,   656,  1222,
     275,   504,   394,   278,   779,   211,   780,   136,  1000,   448,
    1234,   798,   799,   319,   406,   137,   408,   188,   138,   188,
    1313,   282,  1392,  1394,  1549,   863,  1578,  1450,  1452,  1454,
      -9,  1456,   188,  1318,   287,   291,   559,   560,   139,   958,
     782,   959,   780,   806,   807,   565,   561,   296,  1387,   567,
     960,   961,   140,   301,   978,   979,   980,   477,   981,   478,
     141,   324,   982,   308,   177,  1487,  1488,  1489,   142,   325,
    1492,  1131,  1132,   855,  1133,    16,   144,    17,    18,    19,
      20,    21,    22,   145,    23,    24,    25,    26,    27,   326,
     146,    28,    29,   358,   359,   360,   361,    30,   709,    31,
     482,   327,   483,   654,   190,  1397,   147,   659,   781,   781,
      32,    33,   492,   663,   493,   562,   632,   633,   148,   328,
     634,   635,    34,   625,   626,   627,   628,   149,    35,   152,
     329,   358,   359,   360,   361,   563,   330,    36,   710,   711,
     712,   713,   153,    37,  1003,  1004,   362,  1005,    38,   935,
     936,   688,   691,   692,   693,   694,   331,   154,  1576,   698,
     612,    39,   613,   155,    40,    41,   622,   156,   623,   704,
     705,  1350,   159,  1015,  1016,    42,  1017,   177,    43,   553,
     554,   180,   556,  1248,  1249,   560,  1250,  1085,   188,  1086,
    1351,   785,   786,   787,   788,   743,   632,   633,   819,   645,
     646,   647,   648,   649,   650,  1165,  1166,  1167,  1168,   560,
     181,   925,   926,   927,   928,    44,  1171,  1172,  1173,  1174,
     827,   182,   828,   629,  1428,  1429,  1430,  1431,    45,    46,
      47,   853,   183,   854,   332,    48,   184,  1342,  1343,  1508,
    1344,   185,   333,   334,   984,    49,   985,    50,  1443,  1444,
    1069,  1445,  1070,   186,   560,  1140,  1477,  1478,  1159,  1479,
    1160,  1258,  1259,   821,   190,  1319,  1234,   825,   190,   908,
     190,  1205,   187,  1206,   630,   516,   517,   518,   519,   520,
     521,   522,   523,    51,   188,    52,    53,  1352,  1353,  1289,
     191,  1290,   858,   859,  1527,  1528,   192,  1529,  1306,  1307,
     193,  1503,   194,   195,   196,   197,   198,   199,   313,   871,
     560,   200,   316,   201,   202,   203,   204,  1573,  1121,   205,
     317,   323,    54,   206,   592,   207,  1568,   593,   594,   208,
     572,   573,  1354,  1355,  1356,  1357,  1358,  1359,  1360,  1361,
     595,   596,   597,   598,   213,   599,   579,   889,   215,   600,
     601,   602,   603,   516,   517,   518,   519,   520,   521,   522,
     523,   216,   950,   217,   322,   952,   336,   218,   311,   955,
     337,   312,   314,   338,   962,   315,  1245,  1362,   752,   753,
     754,   755,   756,   757,   758,   759,   760,   761,   341,  1363,
     340,   570,   571,  1364,   572,   573,   349,   343,  1365,  1366,
    1367,  1368,   345,   352,   574,   575,   576,   577,   762,   578,
     579,   354,   357,   364,   365,   368,   369,   580,   371,   373,
     378,   381,   383,  1644,   516,   517,   518,   519,   520,   521,
     522,   523,   389,   390,   392,   763,   717,   718,   719,   720,
     721,   722,   723,   724,  1021,  1022,  1023,  1024,  1025,  1026,
    1027,  1028,   676,   677,   678,   679,   680,   681,   682,   683,
     604,   560,   396,   983,   400,   401,   402,   403,   990,   605,
     404,   410,   411,   419,   412,   423,   424,   425,   426,   606,
     581,   582,   583,   427,   428,   466,   467,   472,   473,  1418,
     476,   486,   490,   491,  1012,  1079,   496,   514,  1091,   515,
     526,   527,   532,   528,   533,   529,   534,   535,  1034,   536,
     537,   538,   539,   540,   541,   542,   544,   545,   543,   548,
     569,   607,   590,   608,   584,   560,   616,   617,   621,   631,
     640,   641,   642,   643,   651,   652,   658,   661,   662,   667,
     684,   668,   669,   685,   585,   586,   686,   696,   701,   609,
     703,   708,   610,   783,   715,   726,   732,   736,   190,   746,
     749,   751,   769,   771,  -503,   778,   795,  1315,   801,   796,
     802,   817,   836,   832,   833,   834,   839,   850,   861,   840,
     862,   841,   842,   864,   865,   587,   843,   588,   844,   845,
     846,   847,   848,   875,   876,  1292,   872,   877,   878,   879,
     868,   882,   884,   885,   888,   890,   891,   892,   893,  1441,
     894,   895,   898,   899,   900,   901,  1136,   907,   910,   967,
     911,   912,   913,   914,   190,   917,   918,   909,   920,   921,
     922,   923,   931,   924,   934,   938,   939,   940,  1145,   941,
     942,   943,   930,   963,   964,   965,   970,   971,   972,   987,
     973,   997,   996,  1001,  1013,  1041,  1014,  1051,  1009,  1042,
    1043,  1045,  1047,  1050,  1072,  1127,  1054,  1055,  1056,  1057,
    1058,  1061,  1062,  1063,  1064,  1066,  1031,  1067,  1077,  1107,
    1098,  1099,  1100,  1244,  1102,  1103,  1113,  1114,  1115,  1120,
    1122,  1123,  -541,  1151,  1149,  1142,  1438,  1146,  1147,  1148,
    1154,  1155,  1156,  1162,  1178,  1187,  1194,  1196,  1198,  1199,
    1200,  1210,  1202,  1203,  1208,  1212,  1213,  1214,  1218,  1228,
    1204,  1215,  1216,  1217,  1219,  1224,  1225,  1226,  1227,  1241,
    1255,  1251,  1253,  1163,  1257,  1260,  1264,  1282,  1265,  1240,
    1266,  1267,  1269,  1270,  1271,   190,  1272,  1275,  1277,  1278,
    1279,  1280,  1284,  1281,  1285,  1286,  1287,  1288,  1298,  1296,
    1302,  1310,  1316,  1300,  1303,  1322,  1323,  1325,  1324,  1330,
    1329,  1518,  1335,  1336,  1337,  1338,  1340,  1341,  1347,  1345,
    1346,  1373,  1376,  1375,  1509,  1382,  1390,  1317,  1379,  1380,
    1381,  1396,  1401,  1402,  1403,  1404,  1570,  1434,  1408,  1409,
    1410,  1415,  1419,  1420,  1485,  1371,  1421,  1422,  1423,  1424,
    1425,  1413,  1414,  1426,  1427,  1439,  1442,  1446,  1465,  1470,
    1475,  1105,  1468,  1476,  1481,  1482,  1612,  1484,  1469,  1494,
    1495,  1496,  1497,  1490,  1472,  1499,  1502,  1512,  1500,  1491,
    1522,  1504,  1550,  1505,  1493,  1506,  1542,  1507,  1517,  1543,
    1510,  1513,  1536,  1515,  1516,  1539,  1541,  1544,  1551,  1552,
    1595,  1555,  1556,  1557,  1560,  1540,  1561,  1562,  1563,  1598,
    1580,  1566,  1574,  1558,  1571,  1564,  1572,  1579,  1581,  1582,
    1583,   132,  1370,  1584,  1588,  1592,  1569,  1594,  1596,  1597,
    1607,  1608,  1610,  1611,  1613,  1614,  1615,  1369,  1616,  1617,
    1619,  1372,  1620,  1567,  1626,  1627,  1618,  1630,  1234,  1599,
    1632,  1633,  1634,  1600,  1601,  1636,  1628,  1639,  1637,  1638,
    1640,  1631,  1602,  1641,  1603,  1604,  1642,  1643,  1211,  1648,
    1646,  1647,  1649,  1653,  1654,  1656,  1605,  1657,  1658,  1661,
    1662,  1663,  1664,   248,  1299,   745,   995,  1006,  1239,  1467,
    1466,   969,  1018,  1243,   750,  1247,   611,   620,  1273,  1406,
    1049,  1081,   167,  1068,  1177,  1293,   951,     0,     0,  1652,
     657,     0,  1606,     0,     0,     0,  1440,  1645,   190,  1621,
       0,     0,     0,     0,  1498,     0,     0,  1622,     0,     0,
       0,  1625,     0,     0,     0,  1629,  1650,  1651,     0,  1655,
       0,  1660,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1659,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1538,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   399,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1624,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1635,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1609,     0,     0,     0,   190,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1623
};

static const yytype_int16 yycheck[] =
{
     126,   404,   189,   347,   596,   500,   883,   351,   950,   353,
    1129,     4,     4,     4,    30,     4,   257,    30,     6,     6,
     962,     6,     4,   367,     4,     6,   370,     6,     6,     6,
      30,     6,    30,     6,     6,     6,   380,     6,    48,     6,
      30,    30,    28,    58,    96,    28,    30,     1,    71,    58,
      96,    58,    58,     7,    91,     9,    10,    11,    12,    13,
      14,    96,    16,    17,    18,    19,    20,    96,    19,    23,
      24,   166,    96,    31,    96,    29,    96,    31,    96,    96,
      76,    77,    78,    79,    41,    96,    21,     4,    42,    43,
      28,    96,   159,  1169,   175,   126,   175,     0,     4,    96,
      54,  1177,    96,    30,   266,   104,    60,   269,   266,   108,
     109,   269,   170,   171,    71,    69,    76,    77,    78,    79,
      96,    75,    49,    50,    30,   469,    80,  1246,    45,   266,
     533,   268,   125,    96,    96,     8,   167,  1079,   233,    93,
     172,   173,    96,    97,  1086,     4,    96,   157,   158,  1091,
      48,   174,    96,   107,   266,  1063,   110,    55,  1066,   174,
     175,    96,    96,    96,   567,   660,   266,    36,    37,   664,
     266,   666,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,   263,    52,   263,   184,   175,   224,   225,   226,
      59,   104,   266,   147,   268,   108,   219,     6,   156,   188,
     151,   152,   217,   218,   219,    94,   160,   161,   162,   166,
     167,     3,     4,   167,     6,   267,   163,   164,   165,   148,
     149,   267,   169,   177,   266,   179,   268,   242,   166,    72,
      73,    74,   267,   216,   243,   242,   242,     6,   267,   255,
     167,   264,   255,   267,     6,   267,   261,   267,     6,   267,
     267,   260,   268,   266,     9,   255,   267,   255,     6,   269,
     386,   215,   267,   217,   218,   255,   266,   228,   266,   230,
     267,   255,   258,   267,   270,   266,   272,     6,   271,   271,
     269,   100,   101,   271,   271,     6,   271,   269,     6,   269,
     271,   267,   271,   271,   271,   698,   271,   270,   270,   270,
     254,   270,   269,  1245,   267,   267,   166,   167,     6,   247,
     270,   249,   272,   118,   119,   441,   176,   267,   187,   445,
     258,   259,     6,   267,   251,   252,   253,   266,   255,   268,
       6,     1,   259,   267,   267,  1421,  1422,  1423,     6,     9,
    1426,    36,    37,   687,    39,     7,     6,     9,    10,    11,
      12,    13,    14,     6,    16,    17,    18,    19,    20,    29,
       6,    23,    24,    76,    77,    78,    79,    29,   180,    31,
     266,    41,   268,   499,   500,  1317,     6,   503,   619,   620,
      42,    43,   266,   509,   268,   245,    98,    99,     3,    59,
     102,   103,    54,    81,    82,    83,    84,     3,    60,     6,
      70,    76,    77,    78,    79,   265,    76,    69,   220,   221,
     222,   223,     6,    75,     3,     4,    91,     6,    80,   120,
     121,   547,   220,   221,   222,   223,    96,     6,  1547,   555,
     266,    93,   268,     6,    96,    97,   266,     6,   268,   565,
     566,    28,     6,     3,     4,   107,     6,   267,   110,    36,
      37,     4,    39,     3,     4,   167,     6,   268,   269,   954,
      47,    87,    88,    89,    90,   591,    98,    99,   655,   112,
     113,   114,   115,   116,   117,    87,    88,    89,    90,   167,
     266,   150,   151,   152,   153,   147,    87,    88,    89,    90,
     266,     4,   268,   181,   236,   237,   238,   239,   160,   161,
     162,   266,    95,   268,   174,   167,     4,     3,     4,    25,
       6,     4,   182,   183,   266,   177,   268,   179,     3,     4,
     266,     6,   268,     4,   167,  1020,     3,     4,   266,     6,
     268,   154,   155,   659,   660,   268,   269,   663,   664,   780,
     666,   266,     4,   268,   232,    61,    62,    63,    64,    65,
      66,    67,    68,   215,   269,   217,   218,   144,   145,   266,
     266,   268,   688,   689,     3,     4,     6,     6,  1226,  1227,
       4,  1448,   266,   266,   266,   266,   266,   266,     6,   705,
     167,   266,     6,   266,   266,   266,   266,    25,   991,   266,
       6,     4,   254,   266,    30,   266,  1538,    33,    34,   266,
      36,    37,   189,   190,   191,   192,   193,   194,   195,   196,
      46,    47,    48,    49,   266,    51,    52,   743,   266,    55,
      56,    57,    58,    61,    62,    63,    64,    65,    66,    67,
      68,   266,   819,   266,    25,   822,     6,   266,   266,   826,
      20,   266,   266,    19,   831,   266,  1141,   234,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,    10,   246,
      25,    33,    34,   250,    36,    37,    43,    42,   255,   256,
     257,   258,    31,    54,    46,    47,    48,    49,   235,    51,
      52,    69,    93,    75,    80,    97,   269,    59,   147,   107,
     110,   179,   215,  1635,    61,    62,    63,    64,    65,    66,
      67,    68,   217,    28,   218,   262,   197,   198,   199,   200,
     201,   202,   203,   204,    61,    62,    63,    64,    65,    66,
      67,    68,   535,   536,   537,   538,   539,   540,   541,   542,
     166,   167,     4,   859,   254,    48,   266,     6,   864,   175,
       6,     6,     6,   162,   266,    22,     4,     6,     4,   185,
     122,   123,   124,     4,     4,     4,     4,   269,   269,  1351,
       4,     4,   266,     4,   890,   952,     4,    60,   955,     6,
     270,   270,    27,   270,     6,   270,     4,     4,   904,     4,
       4,     4,     4,     4,     4,     4,   268,    27,     6,     4,
     269,   227,    28,   229,   166,   167,     4,     4,     4,     4,
       4,     4,     4,   111,     4,     4,     4,     4,     4,    25,
     266,    25,     6,    76,   186,   187,     6,     6,     6,   255,
       6,     4,   258,   268,     6,     6,     6,     6,   954,     6,
       6,     6,     6,     6,     4,     4,     4,  1240,     4,     6,
       4,   268,     3,     6,     6,   266,     4,   175,     4,   266,
       4,   266,   266,     4,     4,   227,   266,   229,   266,   266,
     266,   266,   266,     4,     4,  1209,   269,     4,     4,     4,
      71,     4,     6,     4,     4,     4,     4,     4,     4,  1374,
       4,     4,     4,     4,     4,     4,  1012,     4,     6,   146,
       6,     6,     6,     4,  1020,     4,   269,    92,     4,   270,
       4,     6,   266,     6,   266,     4,     4,     4,  1034,     4,
       4,     4,   105,    26,    26,    25,     4,     6,   266,     4,
      28,     4,     6,     4,     6,     4,     6,   231,    53,     6,
       4,     4,     4,     4,   268,   270,     6,     6,     6,     6,
       6,     4,     4,     4,     4,     4,    53,     4,     6,   178,
       6,     6,     6,  1140,     6,     6,     6,     6,     6,     6,
       6,     4,     4,   266,     4,     6,  1369,     6,     6,     6,
       4,     6,   104,     4,   166,     4,   266,     4,     4,     4,
       4,     4,    27,    27,     6,     4,     4,     4,     4,   269,
      26,     6,     6,     6,     4,     6,     4,     4,     4,   146,
     270,   269,   269,   105,   105,     4,     6,     4,     6,  1135,
       6,     6,     6,     6,     6,  1141,     6,     6,     6,     6,
       6,     6,     6,    56,     6,     6,     6,   166,     6,   268,
       6,     6,     6,   268,   268,     4,     4,     4,   269,     4,
     266,   106,     6,     6,     6,     6,     4,     6,    27,     6,
       6,     6,     4,   240,  1457,     4,     4,  1244,   270,   270,
     270,     6,     4,     4,     4,     4,    26,     3,     6,     6,
       4,     6,     4,     6,    28,   175,     6,     6,     6,     6,
       6,   266,   266,     6,     6,     6,     6,     4,     6,     4,
       6,   969,   270,     6,     6,     4,  1591,     4,   270,     4,
       4,     4,     4,    28,   269,     6,     4,     6,   268,    28,
       4,   270,     4,   270,    28,   270,     6,   270,   270,    25,
     269,   269,   175,   269,   269,   268,   270,     6,     4,     6,
      26,     6,     6,     6,     4,   126,     4,     4,     4,    25,
       4,     6,  1545,   175,     6,   175,     6,     6,     4,     4,
       4,    29,  1295,     6,     6,     6,   248,     6,     6,     6,
       6,     6,     6,     6,     6,     4,    27,  1293,     6,     6,
       6,  1297,     6,   269,     4,     6,    26,     6,   269,   269,
       6,     4,     4,   270,   270,     4,    27,    27,     6,     6,
       6,    26,   270,     6,   270,   270,     4,     4,  1109,    27,
       6,     6,     6,     6,     6,     6,   270,     6,     6,     6,
       6,     6,     6,   164,  1218,   593,   866,   880,  1130,  1400,
    1399,   838,   896,  1139,   598,  1144,   459,   473,  1176,  1328,
     917,   953,    95,   943,  1067,  1209,   820,    -1,    -1,   241,
     501,    -1,   270,    -1,    -1,    -1,  1372,   268,  1374,   269,
      -1,    -1,    -1,    -1,  1441,    -1,    -1,   269,    -1,    -1,
      -1,   270,    -1,    -1,    -1,   270,   270,   270,    -1,   270,
      -1,   244,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   268,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1498,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   306,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1612,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1624,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1587,    -1,    -1,    -1,  1591,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1609
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   159,   274,   275,   276,     0,     8,   277,     4,   170,
     171,     1,   278,   266,   266,   266,     7,     9,    10,    11,
      12,    13,    14,    16,    17,    18,    19,    20,    23,    24,
      29,    31,    42,    43,    54,    60,    69,    75,    80,    93,
      96,    97,   107,   110,   147,   160,   161,   162,   167,   177,
     179,   215,   217,   218,   254,   279,   280,   281,   283,   284,
     286,   288,   290,   291,   308,   309,   310,   311,   313,   316,
     318,   319,   323,   324,   352,   362,   364,   373,   374,   376,
     377,   398,   399,   411,   412,   413,   452,   453,   517,   555,
     557,   558,   575,   576,   577,   578,   593,   594,   619,   620,
     637,   638,   653,   654,   666,   667,   702,   703,   715,   716,
     735,   736,   746,   747,   768,   769,   788,   789,   282,     6,
     285,    94,   287,   289,   312,   314,   317,     6,     6,   172,
     173,   365,   365,   353,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     9,     6,     6,     6,     6,     3,     3,
     292,   414,     6,     6,     6,     6,     6,   320,   325,     6,
     378,   400,   416,   454,   518,   559,   579,   579,   595,   621,
     639,   655,   668,   706,   718,   738,   749,   267,   772,   791,
       4,   266,     4,    95,     4,     4,     4,     4,   269,   392,
     395,   266,     6,     4,   266,   266,   266,   266,   266,   266,
     266,   266,   266,   266,   266,   266,   266,   266,   266,   293,
       4,   266,   415,   266,   704,   266,   266,   266,   266,    21,
      96,   321,   322,    96,   267,   326,   351,   363,    96,   267,
     379,   397,    96,   267,   401,    96,   267,   417,   418,   419,
     451,    96,   267,   455,   456,   457,   515,    96,   456,   519,
     556,    96,   267,   560,   561,   574,    96,   267,   580,   581,
     586,   592,    96,   591,    96,   267,   596,   597,   618,    96,
     267,   622,   623,   636,    96,   267,   640,    96,   267,   656,
     665,    96,   267,   669,   670,   692,    96,   267,   705,   707,
      96,   267,   717,   719,   720,    96,   267,   737,   739,   740,
      96,   267,   748,   750,   753,   773,   771,    96,   267,   790,
     792,   266,   266,     6,   266,   266,     6,     6,     6,   271,
     393,   395,    25,     4,     1,     9,    29,    41,    59,    70,
      76,    96,   174,   182,   183,   294,     6,    20,    19,   327,
      25,    10,   380,    42,   402,    31,   420,   422,   421,    43,
     458,   468,    54,   520,    69,   562,   563,    93,    76,    77,
      78,    79,    91,   582,    75,    80,   598,   599,    97,   269,
     625,   147,   641,   107,   104,   108,   109,   184,   110,   671,
     675,   179,   708,   215,    28,   216,    30,   255,   733,   217,
      28,   743,   218,    28,   258,   756,     4,    96,   770,   772,
     254,    48,   266,     6,     6,     6,   271,     6,   271,   394,
       6,     6,   266,   295,   301,   298,   303,   299,   296,   162,
     300,   297,   302,    22,     4,     6,     4,     4,     4,   266,
     268,   375,   423,   424,   425,   427,   430,   432,   434,   437,
     439,   443,   444,   447,   448,   449,   450,     4,   271,     4,
      45,   459,   266,   268,   375,   469,   484,   486,   266,   268,
     375,   521,   522,   536,   538,   540,     4,     4,   564,   565,
     583,   584,   269,   269,   587,   589,     4,   266,   268,   375,
     601,   624,   266,   268,   375,   626,     4,   657,   659,   660,
     266,     4,   266,   268,   375,   676,     4,   721,   723,   395,
     734,   732,   741,    30,   255,   266,   744,   754,   751,    30,
     255,   266,   757,   774,    60,     6,    61,    62,    63,    64,
      65,    66,    67,    68,   315,   315,   270,   270,   270,   270,
     266,   393,    27,     6,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     6,   268,    27,   381,   403,     4,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,   166,
     167,   176,   245,   265,     4,   395,   433,   395,   460,   269,
      33,    34,    36,    37,    46,    47,    48,    49,    51,    52,
      59,   122,   123,   124,   166,   186,   187,   227,   229,   492,
      28,   488,    30,    33,    34,    46,    47,    48,    49,    51,
      55,    56,    57,    58,   166,   175,   185,   227,   229,   255,
     258,   492,   266,   268,   375,   566,     4,     4,   582,   585,
     585,     4,   266,   268,   590,    81,    82,    83,    84,   181,
     232,     4,    98,    99,   102,   103,   635,   148,   149,   643,
       4,     4,     4,   111,   672,   112,   113,   114,   115,   116,
     117,     4,     4,   729,   395,   392,   266,   733,     4,   395,
     745,     4,     4,   395,   758,   775,   793,    25,    25,     6,
     315,   163,   164,   165,   169,   304,   304,   304,   304,   304,
     304,   304,   304,   304,   266,    76,     6,   382,   395,   404,
     428,   220,   221,   222,   223,   431,     6,   426,   395,   445,
     440,     6,   435,     6,   395,   395,   315,   462,     4,   180,
     220,   221,   222,   223,   483,     6,   472,   197,   198,   199,
     200,   201,   202,   203,   204,   348,     6,    91,   224,   225,
     226,   547,     6,   473,   487,   477,     6,   480,   474,   475,
     471,   470,   489,   395,   528,   431,     6,   531,   348,     6,
     547,     6,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   235,   262,   516,   537,   539,   541,   532,     6,
     524,     6,   526,   529,   523,    41,    71,   166,     4,   270,
     272,   582,   270,   268,   588,    87,    88,    89,    90,   602,
     603,   604,   605,   606,   607,     4,     6,   627,   100,   101,
     642,     4,     4,   104,   108,   661,   118,   119,   673,   677,
     678,   679,   680,   681,   682,   709,   722,   268,   730,   393,
     742,   395,   392,   755,   752,   395,   392,   266,   268,   776,
     784,   392,     6,     6,   266,   354,     3,   306,   305,     4,
     266,   266,   266,   266,   266,   266,   266,   266,   266,   328,
     175,   366,   367,   266,   268,   375,   383,   390,   395,   395,
     405,     4,     4,   315,     4,     4,   441,   442,    71,   436,
     438,   395,   269,   463,   461,     4,     4,     4,     4,     4,
     481,   482,     4,   476,     6,     4,   493,   496,     4,   395,
       4,     4,     4,     4,     4,     4,   543,   544,     4,     4,
       4,     4,   548,   551,   568,   572,   567,     4,   582,    92,
       6,     6,     6,     6,     4,   609,   613,     4,   269,   608,
       4,   270,     4,     6,     6,   150,   151,   152,   153,   644,
     105,   266,   662,   664,   266,   120,   121,   674,     4,     4,
       4,     4,     4,     4,   710,   724,    71,   174,   219,   264,
     393,   744,   393,   759,   763,   393,    28,   166,   247,   249,
     258,   259,   393,    26,    26,    25,   355,   146,   546,   546,
       4,     6,   266,    28,   369,    30,    49,    50,   251,   252,
     253,   255,   259,   395,   266,   268,   406,     4,   429,     6,
     395,   446,     3,     4,     6,   442,     6,     4,     4,   125,
     271,     4,   478,     3,     4,     6,   482,   493,   497,    53,
     494,   485,   395,     6,     6,     3,     4,     6,   544,   525,
     527,    61,    62,    63,    64,    65,    66,    67,    68,   533,
     552,    53,   549,     4,   395,   569,    72,    73,    74,   573,
     570,     4,     6,     4,   600,     4,   610,     4,   614,   600,
       4,   231,   617,   628,     6,     6,     6,     6,     6,   645,
     658,     4,     4,     4,     4,   688,     4,     4,   688,   266,
     268,   711,   268,   725,   726,   727,   731,     6,   394,   393,
     268,   757,   760,   761,   766,   268,   392,   762,   764,   765,
     767,   393,   777,   785,   781,   779,   780,   394,     6,     6,
       6,   357,     6,     6,   307,   307,   329,   178,   368,   370,
     384,   391,   387,     6,     6,     6,   385,   388,   166,   233,
       6,   315,     6,     4,   464,   466,   465,   270,   693,   508,
     495,    36,    37,    39,   490,   491,   395,   542,   534,   535,
     392,   530,     6,   554,   550,   395,     6,     6,     6,     4,
     571,   266,   611,   615,     4,     6,   104,   629,   632,   266,
     268,   646,     4,   105,   635,    87,    88,    89,    90,   690,
     691,    87,    88,    89,    90,   689,   635,   690,   166,    58,
     174,   175,   217,   218,   219,   242,   261,     4,   394,   175,
     263,   175,   263,   394,   266,   394,     4,   786,     4,     4,
       4,   266,    27,    27,    26,   266,   268,   358,     6,   330,
       4,   372,     4,     4,     4,     6,     6,     6,     4,     4,
     407,   228,   230,   410,     6,     4,     4,     4,   269,   479,
     694,    48,   157,   158,   269,   505,   509,   510,   512,   496,
     395,   146,   545,   548,   393,   392,   513,   551,     3,     4,
       6,   269,   612,   269,   616,   270,   633,   105,   154,   155,
       4,   663,   683,   685,     6,     6,     6,     6,   691,     6,
       6,     6,     6,   683,   712,     6,   728,     6,     6,     6,
       6,    56,     4,   787,     6,     6,     6,     6,   166,   266,
     268,   331,   375,   449,   371,   372,   268,   396,     6,   396,
     268,   408,     6,   268,   375,   467,   467,   467,   695,   697,
       6,   511,     6,   271,   498,   315,     6,   393,   394,   268,
     505,   514,     4,     4,   269,     4,   630,   647,   648,   266,
       4,    19,   151,   152,   686,     6,     6,     6,     6,   713,
       4,     6,     3,     4,     6,     6,     6,    27,   356,   359,
      28,    47,   144,   145,   189,   190,   191,   192,   193,   194,
     195,   196,   234,   246,   250,   255,   256,   257,   258,   395,
     371,   175,   395,     6,   386,   240,     4,   409,   126,   270,
     270,   270,     4,    36,    37,    52,    59,   187,   698,   701,
       4,     6,   271,     6,   271,   501,     6,   394,    48,    55,
     553,     4,     4,     4,     4,   649,   649,   684,     6,     6,
       4,   714,   778,   266,   266,     6,   360,   334,   348,     4,
       6,     6,     6,     6,     6,     6,     6,     6,   236,   237,
     238,   239,   347,   333,     3,   336,   332,   338,   315,     6,
     395,   392,     6,     3,     4,     6,     4,   700,   699,     6,
     270,     6,   270,     6,   270,     6,   270,     4,    30,   175,
     188,   499,   500,   502,   505,     6,   516,   501,   270,   270,
       4,   631,   269,   650,   687,     6,     6,     3,     4,     6,
     782,     6,     4,   361,     4,    28,   349,   349,   349,   349,
      28,    28,   349,    28,     4,     4,     4,     4,   393,     6,
     268,   696,     4,   493,   270,   270,   270,   270,    25,   315,
     269,   507,     6,   269,   506,   269,   269,   270,   106,   634,
      31,   156,     4,    58,   243,   260,   783,     3,     4,     6,
     335,   350,   339,   340,   341,   337,   175,   343,   393,   268,
     126,   270,     6,    25,     6,     4,    30,   504,     6,   271,
       4,     4,     6,   652,   651,     6,     6,     6,   175,   342,
       4,     4,     4,     4,   175,   344,     6,   269,   394,   248,
      26,     6,     6,    25,   315,   503,   505,     6,   271,     6,
       4,     4,     4,     4,     6,    58,   242,   345,     6,    58,
     242,   346,     6,   389,     6,    26,     6,     6,    25,   269,
     270,   270,   270,   270,   270,   270,   270,     6,     6,   395,
       6,     6,   392,     6,     4,    27,     6,     6,    26,     6,
       6,   269,   269,   395,   393,   270,     4,     6,    27,   270,
       6,    26,     6,     4,     4,   393,     4,     6,     6,    27,
       6,     6,     4,     4,   394,   268,     6,     6,    27,     6,
     270,   270,   241,     6,     6,   270,     6,     6,     6,   268,
     244,     6,     6,     6,     6
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:

/* Line 1455 of yacc.c  */
#line 319 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 320 "def.y"
    {
        defData->VersionNum = convert_defname2num((yyvsp[(3) - (4)].string));
        if (defData->VersionNum > 5.8) {
          char temp[300];
          sprintf(temp,
          "The execution has been stopped because the DEF parser 5.8 does not support DEF file with version %s.\nUpdate your DEF file to version 5.8 or earlier.",
                  (yyvsp[(3) - (4)].string));
          defError(6503, temp);
          return 1;
        }
        if (defCallbacks->VersionStrCbk) {
          CALLBACK(defCallbacks->VersionStrCbk, defrVersionStrCbkType, (yyvsp[(3) - (4)].string));
        } else if (defCallbacks->VersionCbk) {
            CALLBACK(defCallbacks->VersionCbk, defrVersionCbkType, defData->VersionNum);
        }
        if (defData->VersionNum > 5.3 && defData->VersionNum < 5.4)
          defData->defIgnoreVersion = 1;
        if (defData->VersionNum < 5.6)     // default to false before 5.6
          defData->names_case_sensitive = defSettings->reader_case_sensitive;
        else
          defData->names_case_sensitive = 1;
        defData->hasVer = 1;
        defData->doneDesign = 0;
    ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 347 "def.y"
    {
        if (defData->VersionNum < 5.6) {
          defData->names_case_sensitive = 1;
          if (defCallbacks->CaseSensitiveCbk)
            CALLBACK(defCallbacks->CaseSensitiveCbk, defrCaseSensitiveCbkType,
                     defData->names_case_sensitive); 
          defData->hasNameCase = 1;
        } else
          if (defCallbacks->CaseSensitiveCbk) // write error only if cbk is set 
             if (defData->caseSensitiveWarnings++ < defSettings->CaseSensitiveWarnings)
               defWarning(7011, "The NAMESCASESENSITIVE statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement.");
      ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 360 "def.y"
    {
        if (defData->VersionNum < 5.6) {
          defData->names_case_sensitive = 0;
          if (defCallbacks->CaseSensitiveCbk)
            CALLBACK(defCallbacks->CaseSensitiveCbk, defrCaseSensitiveCbkType,
                     defData->names_case_sensitive);
          defData->hasNameCase = 1;
        } else {
          if (defCallbacks->CaseSensitiveCbk) { // write error only if cbk is set 
            if (defData->caseSensitiveWarnings++ < defSettings->CaseSensitiveWarnings) {
              defError(6504, "Def parser version 5.7 and later does not support NAMESCASESENSITIVE OFF.\nEither remove this optional construct or set it to ON.");
              CHKERR();
            }
          }
        }
      ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 400 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 401 "def.y"
    {
            if (defCallbacks->DesignCbk)
              CALLBACK(defCallbacks->DesignCbk, defrDesignStartCbkType, (yyvsp[(3) - (4)].string));
            defData->hasDes = 1;
          ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 408 "def.y"
    {
            defData->doneDesign = 1;
            if (defCallbacks->DesignEndCbk)
              CALLBACK(defCallbacks->DesignEndCbk, defrDesignEndCbkType, 0);
            // 11/16/2001 - pcr 408334
            // Return 1 if there is any defData->errors during parsing
            if (defData->errors)
                return 1;

            if (!defData->hasVer) {
              char temp[300];
              sprintf(temp, "No VERSION statement found, using the default value %2g.", defData->VersionNum);
              defWarning(7012, temp);            
            }
            if (!defData->hasNameCase && defData->VersionNum < 5.6)
              defWarning(7013, "The DEF file is invalid if NAMESCASESENSITIVE is undefined.\nNAMESCASESENSITIVE is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the NAMESCASESENSITIVE statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!defData->hasBusBit && defData->VersionNum < 5.6)
              defWarning(7014, "The DEF file is invalid if BUSBITCHARS is undefined.\nBUSBITCHARS is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the BUSBITCHARS statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!defData->hasDivChar && defData->VersionNum < 5.6)
              defWarning(7015, "The DEF file is invalid if DIVIDERCHAR is undefined.\nDIVIDERCHAR is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the DIVIDERCHAR statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!defData->hasDes)
              defWarning(7016, "DESIGN is a mandatory statement in the DEF file. Ensure that it exists in the file.");
          ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 432 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 433 "def.y"
    { 
            if (defCallbacks->TechnologyCbk)
              CALLBACK(defCallbacks->TechnologyCbk, defrTechNameCbkType, (yyvsp[(3) - (4)].string));
          ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 438 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1;;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 439 "def.y"
    { 
            if (defCallbacks->ArrayNameCbk)
              CALLBACK(defCallbacks->ArrayNameCbk, defrArrayNameCbkType, (yyvsp[(3) - (4)].string));
          ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 444 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 445 "def.y"
    { 
            if (defCallbacks->FloorPlanNameCbk)
              CALLBACK(defCallbacks->FloorPlanNameCbk, defrFloorPlanNameCbkType, (yyvsp[(3) - (4)].string));
          ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 451 "def.y"
    { 
            if (defCallbacks->HistoryCbk)
              CALLBACK(defCallbacks->HistoryCbk, defrHistoryCbkType, &defData->History_text[0]);
          ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 457 "def.y"
    {
            defData->parsing_property = 1;
            defData->defInPropDef = 1;     // set flag as inside propertydefinitions 
            if (defCallbacks->PropDefStartCbk)
              CALLBACK(defCallbacks->PropDefStartCbk, defrPropDefStartCbkType, 0);
          ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 464 "def.y"
    { 
            if (defCallbacks->PropDefEndCbk)
              CALLBACK(defCallbacks->PropDefEndCbk, defrPropDefEndCbkType, 0);
            defData->real_num = 0;     // just want to make sure it is reset 
            defData->parsing_property = 0;
            defData->defInPropDef = 0;     // reset flag 
          ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 474 "def.y"
    { ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 476 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1; defData->Prop.clear(); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 478 "def.y"
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("design", (yyvsp[(3) - (5)].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defData->DesignProp.setPropType((yyvsp[(3) - (5)].string), defData->defPropDefType);
            ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 485 "def.y"
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 487 "def.y"
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("net", (yyvsp[(3) - (5)].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defData->NetProp.setPropType((yyvsp[(3) - (5)].string), defData->defPropDefType);
            ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 494 "def.y"
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 496 "def.y"
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("specialnet", (yyvsp[(3) - (5)].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defData->SNetProp.setPropType((yyvsp[(3) - (5)].string), defData->defPropDefType);
            ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 503 "def.y"
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 505 "def.y"
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("region", (yyvsp[(3) - (5)].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defData->RegionProp.setPropType((yyvsp[(3) - (5)].string), defData->defPropDefType);
            ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 512 "def.y"
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 514 "def.y"
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("group", (yyvsp[(3) - (5)].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defData->GroupProp.setPropType((yyvsp[(3) - (5)].string), defData->defPropDefType);
            ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 521 "def.y"
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 523 "def.y"
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("component", (yyvsp[(3) - (5)].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defData->CompProp.setPropType((yyvsp[(3) - (5)].string), defData->defPropDefType);
            ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 530 "def.y"
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 532 "def.y"
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("row", (yyvsp[(3) - (5)].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defData->RowProp.setPropType((yyvsp[(3) - (5)].string), defData->defPropDefType);
            ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 541 "def.y"
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 543 "def.y"
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("componentpin", (yyvsp[(3) - (5)].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defData->CompPinProp.setPropType((yyvsp[(3) - (5)].string), defData->defPropDefType);
            ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 551 "def.y"
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 553 "def.y"
    {
              if (defData->VersionNum < 5.6) {
                if (defData->nonDefaultWarnings++ < defSettings->NonDefaultWarnings) {
                  defData->defMsg = (char*)defMalloc(1000); 
                  sprintf (defData->defMsg,
                     "The NONDEFAULTRULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6505, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              } else {
                if (defCallbacks->PropCbk) {
                  defData->Prop.setPropType("nondefaultrule", (yyvsp[(3) - (5)].string));
                  CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
                }
                defData->NDefProp.setPropType((yyvsp[(3) - (5)].string), defData->defPropDefType);
              }
            ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 571 "def.y"
    { yyerrok; yyclearin;;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 573 "def.y"
    { defData->real_num = 0 ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 574 "def.y"
    {
              if (defCallbacks->PropCbk) defData->Prop.setPropInteger();
              defData->defPropDefType = 'I';
            ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 578 "def.y"
    { defData->real_num = 1 ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 579 "def.y"
    {
              if (defCallbacks->PropCbk) defData->Prop.setPropReal();
              defData->defPropDefType = 'R';
              defData->real_num = 0;
            ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 585 "def.y"
    {
              if (defCallbacks->PropCbk) defData->Prop.setPropString();
              defData->defPropDefType = 'S';
            ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 590 "def.y"
    {
              if (defCallbacks->PropCbk) defData->Prop.setPropQString((yyvsp[(2) - (2)].string));
              defData->defPropDefType = 'Q';
            ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 595 "def.y"
    {
              if (defCallbacks->PropCbk) defData->Prop.setPropNameMapString((yyvsp[(2) - (2)].string));
              defData->defPropDefType = 'S';
            ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 602 "def.y"
    { if (defCallbacks->PropCbk) defData->Prop.setNumber((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 605 "def.y"
    {
            if (defCallbacks->UnitsCbk) {
              if (defValidNum((int)(yyvsp[(4) - (5)].dval)))
                CALLBACK(defCallbacks->UnitsCbk,  defrUnitsCbkType, (yyvsp[(4) - (5)].dval));
            }
          ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 613 "def.y"
    {
            if (defCallbacks->DividerCbk)
              CALLBACK(defCallbacks->DividerCbk, defrDividerCbkType, (yyvsp[(2) - (3)].string));
            defData->hasDivChar = 1;
          ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 620 "def.y"
    { 
            if (defCallbacks->BusBitCbk)
              CALLBACK(defCallbacks->BusBitCbk, defrBusBitCbkType, (yyvsp[(2) - (3)].string));
            defData->hasBusBit = 1;
          ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 626 "def.y"
    {defData->dumb_mode = 1;defData->no_num = 1; ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 628 "def.y"
    {
              if (defCallbacks->CanplaceCbk) {
                defData->Canplace.setName((yyvsp[(3) - (14)].string));
                defData->Canplace.setLocation((yyvsp[(4) - (14)].dval),(yyvsp[(5) - (14)].dval));
                defData->Canplace.setOrient((yyvsp[(6) - (14)].integer));
                defData->Canplace.setDo((yyvsp[(8) - (14)].dval),(yyvsp[(10) - (14)].dval),(yyvsp[(12) - (14)].dval),(yyvsp[(13) - (14)].dval));
                CALLBACK(defCallbacks->CanplaceCbk, defrCanplaceCbkType,
                &(defData->Canplace));
              }
            ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 638 "def.y"
    {defData->dumb_mode = 1;defData->no_num = 1; ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 640 "def.y"
    {
              if (defCallbacks->CannotOccupyCbk) {
                defData->CannotOccupy.setName((yyvsp[(3) - (14)].string));
                defData->CannotOccupy.setLocation((yyvsp[(4) - (14)].dval),(yyvsp[(5) - (14)].dval));
                defData->CannotOccupy.setOrient((yyvsp[(6) - (14)].integer));
                defData->CannotOccupy.setDo((yyvsp[(8) - (14)].dval),(yyvsp[(10) - (14)].dval),(yyvsp[(12) - (14)].dval),(yyvsp[(13) - (14)].dval));
                CALLBACK(defCallbacks->CannotOccupyCbk, defrCannotOccupyCbkType,
                        &(defData->CannotOccupy));
              }
            ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 651 "def.y"
    {(yyval.integer) = 0;;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 652 "def.y"
    {(yyval.integer) = 1;;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 653 "def.y"
    {(yyval.integer) = 2;;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 654 "def.y"
    {(yyval.integer) = 3;;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 655 "def.y"
    {(yyval.integer) = 4;;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 656 "def.y"
    {(yyval.integer) = 5;;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 657 "def.y"
    {(yyval.integer) = 6;;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 658 "def.y"
    {(yyval.integer) = 7;;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 661 "def.y"
    {
            defData->Geometries.Reset();
          ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 665 "def.y"
    {
            if (defCallbacks->DieAreaCbk) {
               defData->DieArea.addPoint(&defData->Geometries);
               CALLBACK(defCallbacks->DieAreaCbk, defrDieAreaCbkType, &(defData->DieArea));
            }
          ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 674 "def.y"
    { ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 677 "def.y"
    {
          if (defData->VersionNum < 5.4) {
             if (defCallbacks->DefaultCapCbk)
                CALLBACK(defCallbacks->DefaultCapCbk, defrDefaultCapCbkType, ROUND((yyvsp[(2) - (2)].dval)));
          } else {
             if (defCallbacks->DefaultCapCbk) // write error only if cbk is set 
                if (defData->defaultCapWarnings++ < defSettings->DefaultCapWarnings)
                   defWarning(7017, "The DEFAULTCAP statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 693 "def.y"
    {
            if (defData->VersionNum < 5.4) {
              if (defCallbacks->PinCapCbk) {
                defData->PinCap.setPin(ROUND((yyvsp[(2) - (5)].dval)));
                defData->PinCap.setCap((yyvsp[(4) - (5)].dval));
                CALLBACK(defCallbacks->PinCapCbk, defrPinCapCbkType, &(defData->PinCap));
              }
            }
          ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 704 "def.y"
    { ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 707 "def.y"
    { ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 710 "def.y"
    { 
            if (defCallbacks->StartPinsCbk)
              CALLBACK(defCallbacks->StartPinsCbk, defrStartPinsCbkType, ROUND((yyvsp[(2) - (3)].dval)));
          ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 719 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 720 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 721 "def.y"
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              defData->Pin.Setup((yyvsp[(3) - (7)].string), (yyvsp[(7) - (7)].string));
            }
            defData->hasPort = 0;
          ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 728 "def.y"
    { 
            if (defCallbacks->PinCbk)
              CALLBACK(defCallbacks->PinCbk, defrPinCbkType, &defData->Pin);
          ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 737 "def.y"
    {
            if (defCallbacks->PinCbk)
              defData->Pin.setSpecial();
          ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 743 "def.y"
    { 
            if (defCallbacks->PinExtCbk)
              CALLBACK(defCallbacks->PinExtCbk, defrPinExtCbkType, &defData->History_text[0]);
          ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 749 "def.y"
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.setDirection((yyvsp[(3) - (3)].string));
          ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 755 "def.y"
    {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The NETEXPR statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6506, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.setNetExpr((yyvsp[(3) - (3)].string));

            }
          ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 775 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 776 "def.y"
    {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The SUPPLYSENSITIVITY statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6507, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.setSupplySens((yyvsp[(4) - (4)].string));
            }
          ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 795 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 796 "def.y"
    {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The GROUNDSENSITIVITY statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6508, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.setGroundSens((yyvsp[(4) - (4)].string));
            }
          ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 816 "def.y"
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) defData->Pin.setUse((yyvsp[(3) - (3)].string));
          ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 820 "def.y"
    {
            if (defData->VersionNum < 5.7) {
               if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                 if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                     (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                   defData->defMsg = (char*)defMalloc(10000);
                   sprintf (defData->defMsg,
                     "The PORT in PINS is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                   defError(6555, defData->defMsg);
                   defFree(defData->defMsg);
                   CHKERR();
                 }
               }
            } else {
               if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                 defData->Pin.addPort();
               defData->hasPort = 1;
            }
          ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 840 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 841 "def.y"
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.addPortLayer((yyvsp[(4) - (4)].string));
              else
                 defData->Pin.addLayer((yyvsp[(4) - (4)].string));
            }
          ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 850 "def.y"
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.addPortLayerPts((yyvsp[(8) - (9)].pt).x, (yyvsp[(8) - (9)].pt).y, (yyvsp[(9) - (9)].pt).x, (yyvsp[(9) - (9)].pt).y);
              else
                 defData->Pin.addLayerPts((yyvsp[(8) - (9)].pt).x, (yyvsp[(8) - (9)].pt).y, (yyvsp[(9) - (9)].pt).x, (yyvsp[(9) - (9)].pt).y);
            }
          ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 859 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 860 "def.y"
    {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6509, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolygon((yyvsp[(4) - (4)].string));
                else
                   defData->Pin.addPolygon((yyvsp[(4) - (4)].string));
              }
            }
            
            defData->Geometries.Reset();            
          ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 885 "def.y"
    {
            if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolygonPts(&defData->Geometries);
                else
                   defData->Pin.addPolygonPts(&defData->Geometries);
              }
            }
          ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 895 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 896 "def.y"
    {
            if (defData->VersionNum < 5.7) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The PIN VIA statement is available in version 5.7 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6556, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortVia((yyvsp[(4) - (9)].string), (int)(yyvsp[(7) - (9)].dval),
                                               (int)(yyvsp[(8) - (9)].dval), (yyvsp[(5) - (9)].integer));
                else
                   defData->Pin.addVia((yyvsp[(4) - (9)].string), (int)(yyvsp[(7) - (9)].dval),
                                               (int)(yyvsp[(8) - (9)].dval), (yyvsp[(5) - (9)].integer));
              }
            }
          ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 922 "def.y"
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.setPortPlacement((yyvsp[(1) - (3)].integer), (yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, (yyvsp[(3) - (3)].integer));
              else
                 defData->Pin.setPlacement((yyvsp[(1) - (3)].integer), (yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, (yyvsp[(3) - (3)].integer));
            }
          ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 933 "def.y"
    {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINPARTIALMETALAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6510, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinPartialMetalArea((int)(yyvsp[(3) - (4)].dval), (yyvsp[(4) - (4)].string)); 
          ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 951 "def.y"
    {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINPARTIALMETALSIDEAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6511, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinPartialMetalSideArea((int)(yyvsp[(3) - (4)].dval), (yyvsp[(4) - (4)].string)); 
          ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 969 "def.y"
    {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINGATEAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6512, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.addAPinGateArea((int)(yyvsp[(3) - (4)].dval), (yyvsp[(4) - (4)].string)); 
            ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 987 "def.y"
    {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINDIFFAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6513, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinDiffArea((int)(yyvsp[(3) - (4)].dval), (yyvsp[(4) - (4)].string)); 
          ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 1004 "def.y"
    {defData->dumb_mode=1;;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 1005 "def.y"
    {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINMAXAREACAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6514, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinMaxAreaCar((int)(yyvsp[(3) - (6)].dval), (yyvsp[(6) - (6)].string)); 
          ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 1022 "def.y"
    {defData->dumb_mode=1;;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 1024 "def.y"
    {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINMAXSIDEAREACAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6515, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinMaxSideAreaCar((int)(yyvsp[(3) - (6)].dval), (yyvsp[(6) - (6)].string)); 
          ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 1042 "def.y"
    {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINPARTIALCUTAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6516, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinPartialCutArea((int)(yyvsp[(3) - (4)].dval), (yyvsp[(4) - (4)].string)); 
          ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 1059 "def.y"
    {defData->dumb_mode=1;;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 1060 "def.y"
    {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINMAXCUTCAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6517, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinMaxCutCar((int)(yyvsp[(3) - (6)].dval), (yyvsp[(6) - (6)].string)); 
          ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 1078 "def.y"
    {  // 5.5 syntax 
            if (defData->VersionNum < 5.5) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAMODEL statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6518, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
          ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 1096 "def.y"
    { 
           if (validateMaskInput((int)(yyvsp[(2) - (2)].dval), defData->pinWarnings, defSettings->PinWarnings)) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortLayerMask((int)(yyvsp[(2) - (2)].dval));
                else
                   defData->Pin.addLayerMask((int)(yyvsp[(2) - (2)].dval));
              }
           }
         ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 1109 "def.y"
    { (yyval.integer) = 0; ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 1111 "def.y"
    { 
           if (validateMaskInput((int)(yyvsp[(2) - (2)].dval), defData->pinWarnings, defSettings->PinWarnings)) {
             (yyval.integer) = (yyvsp[(2) - (2)].dval);
           }
         ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 1119 "def.y"
    { 
           if (validateMaskInput((int)(yyvsp[(2) - (2)].dval), defData->pinWarnings, defSettings->PinWarnings)) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolyMask((int)(yyvsp[(2) - (2)].dval));
                else
                   defData->Pin.addPolyMask((int)(yyvsp[(2) - (2)].dval));
              }
           }
         ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 1133 "def.y"
    {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The SPACING statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6519, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortLayerSpacing((int)(yyvsp[(2) - (2)].dval));
                else
                   defData->Pin.addLayerSpacing((int)(yyvsp[(2) - (2)].dval));
              }
            }
          ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1156 "def.y"
    {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "DESIGNRULEWIDTH statement is a version 5.6 and later syntax.\nYour def file is defined with version %g", defData->VersionNum);
                  defError(6520, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortLayerDesignRuleWidth((int)(yyvsp[(2) - (2)].dval));
                else
                   defData->Pin.addLayerDesignRuleWidth((int)(yyvsp[(2) - (2)].dval));
              }
            }
          ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1181 "def.y"
    {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "SPACING statement is a version 5.6 and later syntax.\nYour def file is defined with version %g", defData->VersionNum);
                  defError(6521, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolySpacing((int)(yyvsp[(2) - (2)].dval));
                else
                   defData->Pin.addPolySpacing((int)(yyvsp[(2) - (2)].dval));
              }
            }
          ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1204 "def.y"
    {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The DESIGNRULEWIDTH statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6520, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolyDesignRuleWidth((int)(yyvsp[(2) - (2)].dval));
                else
                   defData->Pin.addPolyDesignRuleWidth((int)(yyvsp[(2) - (2)].dval));
              }
            }
          ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 1228 "def.y"
    { defData->aOxide = 1;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1233 "def.y"
    { defData->aOxide = 2;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 1238 "def.y"
    { defData->aOxide = 3;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 1243 "def.y"
    { defData->aOxide = 4;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 1249 "def.y"
    { (yyval.string) = (char*)"SIGNAL"; ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 1251 "def.y"
    { (yyval.string) = (char*)"POWER"; ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 1253 "def.y"
    { (yyval.string) = (char*)"GROUND"; ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 1255 "def.y"
    { (yyval.string) = (char*)"CLOCK"; ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 1257 "def.y"
    { (yyval.string) = (char*)"TIEOFF"; ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 1259 "def.y"
    { (yyval.string) = (char*)"ANALOG"; ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 1261 "def.y"
    { (yyval.string) = (char*)"SCAN"; ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 1263 "def.y"
    { (yyval.string) = (char*)"RESET"; ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 1267 "def.y"
    { (yyval.string) = (char*)""; ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 1268 "def.y"
    {defData->dumb_mode=1;;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 1269 "def.y"
    { (yyval.string) = (yyvsp[(3) - (3)].string); ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 1272 "def.y"
    { 
          if (defCallbacks->PinEndCbk)
            CALLBACK(defCallbacks->PinEndCbk, defrPinEndCbkType, 0);
        ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1277 "def.y"
    {defData->dumb_mode = 2; defData->no_num = 2; ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 1279 "def.y"
    {
          if (defCallbacks->RowCbk) {
            defData->rowName = (yyvsp[(3) - (7)].string);
            defData->Row.setup((yyvsp[(3) - (7)].string), (yyvsp[(4) - (7)].string), (yyvsp[(5) - (7)].dval), (yyvsp[(6) - (7)].dval), (yyvsp[(7) - (7)].integer));
          }
        ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 1287 "def.y"
    {
          if (defCallbacks->RowCbk) 
            CALLBACK(defCallbacks->RowCbk, defrRowCbkType, &defData->Row);
        ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 1293 "def.y"
    {
          if (defData->VersionNum < 5.6) {
            if (defCallbacks->RowCbk) {
              if (defData->rowWarnings++ < defSettings->RowWarnings) {
                defError(6523, "Invalid ROW statement defined in the DEF file. The DO statement which is required in the ROW statement is not defined.\nUpdate your DEF file with a DO statement.");
                CHKERR();
              }
            }
          }
        ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 1304 "def.y"
    {
          // 06/05/2002 - pcr 448455 
          // Check for 1 and 0 in the correct position 
          // 07/26/2002 - Commented out due to pcr 459218 
          if (defData->hasDoStep) {
            // 04/29/2004 - pcr 695535 
            // changed the testing 
            if ((((yyvsp[(4) - (5)].dval) == 1) && (defData->yStep == 0)) ||
                (((yyvsp[(2) - (5)].dval) == 1) && (defData->xStep == 0))) {
              // do nothing 
            } else 
              if (defData->VersionNum < 5.6) {
                if (defCallbacks->RowCbk) {
                  if (defData->rowWarnings++ < defSettings->RowWarnings) {
                    defData->defMsg = (char*)defMalloc(1000);
                    sprintf(defData->defMsg,
                            "The DO statement in the ROW statement with the name %s has invalid syntax.\nThe valid syntax is \"DO numX BY 1 STEP spaceX 0 | DO 1 BY numY STEP 0 spaceY\".\nSpecify the valid syntax and try again.", defData->rowName);
                    defWarning(7018, defData->defMsg);
                    defFree(defData->defMsg);
                    }
                  }
              }
          }
          // pcr 459218 - Error if at least numX or numY does not equal 1 
          if (((yyvsp[(2) - (5)].dval) != 1) && ((yyvsp[(4) - (5)].dval) != 1)) {
            if (defCallbacks->RowCbk) {
              if (defData->rowWarnings++ < defSettings->RowWarnings) {
                defError(6524, "Invalid syntax specified. The valid syntax is either \"DO 1 BY num or DO num BY 1\". Specify the valid syntax and try again.");
                CHKERR();
              }
            }
          }
          if (defCallbacks->RowCbk)
            defData->Row.setDo(ROUND((yyvsp[(2) - (5)].dval)), ROUND((yyvsp[(4) - (5)].dval)), defData->xStep, defData->yStep);
        ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 1341 "def.y"
    {
          defData->hasDoStep = 0;
        ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 1345 "def.y"
    {
          defData->hasDoStep = 1;
          defData->Row.setHasDoStep();
          defData->xStep = (yyvsp[(2) - (3)].dval);
          defData->yStep = (yyvsp[(3) - (3)].dval);
        ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 1356 "def.y"
    {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1;;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 1358 "def.y"
    { defData->dumb_mode = 0; defData->parsing_property = 0; ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 1365 "def.y"
    {
          if (defCallbacks->RowCbk) {
             char propTp;
             char* str = ringCopy("                       ");
             propTp =  defData->RowProp.propType((yyvsp[(1) - (2)].string));
             CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "ROW");
             // For backword compatibility, also set the string value 
             sprintf(str, "%g", (yyvsp[(2) - (2)].dval));
             defData->Row.addNumProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), str, propTp);
          }
        ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 1377 "def.y"
    {
          if (defCallbacks->RowCbk) {
             char propTp;
             propTp =  defData->RowProp.propType((yyvsp[(1) - (2)].string));
             CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "ROW");
             defData->Row.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
          }
        ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 1386 "def.y"
    {
          if (defCallbacks->RowCbk) {
             char propTp;
             propTp =  defData->RowProp.propType((yyvsp[(1) - (2)].string));
             CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "ROW");
             defData->Row.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
          }
        ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 1396 "def.y"
    {
          if (defCallbacks->TrackCbk) {
            defData->Track.setup((yyvsp[(1) - (2)].string));
          }
        ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 1402 "def.y"
    {
          if (((yyvsp[(5) - (9)].dval) <= 0) && (defData->VersionNum >= 5.4)) {
            if (defCallbacks->TrackCbk)
              if (defData->trackWarnings++ < defSettings->TrackWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The DO number %g in TRACK is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[(5) - (9)].dval));
                defError(6525, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if ((yyvsp[(7) - (9)].dval) < 0) {
            if (defCallbacks->TrackCbk)
              if (defData->trackWarnings++ < defSettings->TrackWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The STEP number %g in TRACK is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[(7) - (9)].dval));
                defError(6526, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if (defCallbacks->TrackCbk) {
            defData->Track.setDo(ROUND((yyvsp[(2) - (9)].dval)), ROUND((yyvsp[(5) - (9)].dval)), (yyvsp[(7) - (9)].dval));
            CALLBACK(defCallbacks->TrackCbk, defrTrackCbkType, &defData->Track);
          }
        ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 1430 "def.y"
    {
          (yyval.string) = (yyvsp[(2) - (2)].string);
        ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 1435 "def.y"
    { (yyval.string) = (char*)"X";;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 1437 "def.y"
    { (yyval.string) = (char*)"Y";;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 1443 "def.y"
    { 
              if (validateMaskInput((int)(yyvsp[(2) - (3)].dval), defData->trackWarnings, defSettings->TrackWarnings)) {
                  if (defCallbacks->TrackCbk) {
                    defData->Track.addMask((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].integer));
                  }
               }
            ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 1453 "def.y"
    { (yyval.integer) = 0; ;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 1455 "def.y"
    { (yyval.integer) = 1; ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 1458 "def.y"
    { defData->dumb_mode = 1000; ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 1459 "def.y"
    { defData->dumb_mode = 0; ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1466 "def.y"
    {
          if (defCallbacks->TrackCbk)
            defData->Track.addLayer((yyvsp[(1) - (1)].string));
        ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1473 "def.y"
    {
          if ((yyvsp[(5) - (8)].dval) <= 0) {
            if (defCallbacks->GcellGridCbk)
              if (defData->gcellGridWarnings++ < defSettings->GcellGridWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The DO number %g in GCELLGRID is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[(5) - (8)].dval));
                defError(6527, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if ((yyvsp[(7) - (8)].dval) < 0) {
            if (defCallbacks->GcellGridCbk)
              if (defData->gcellGridWarnings++ < defSettings->GcellGridWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The STEP number %g in GCELLGRID is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[(7) - (8)].dval));
                defError(6528, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if (defCallbacks->GcellGridCbk) {
            defData->GcellGrid.setup((yyvsp[(2) - (8)].string), ROUND((yyvsp[(3) - (8)].dval)), ROUND((yyvsp[(5) - (8)].dval)), (yyvsp[(7) - (8)].dval));
            CALLBACK(defCallbacks->GcellGridCbk, defrGcellGridCbkType, &defData->GcellGrid);
          }
        ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1501 "def.y"
    {
          if (defCallbacks->ExtensionCbk)
             CALLBACK(defCallbacks->ExtensionCbk, defrExtensionCbkType, &defData->History_text[0]);
        ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1507 "def.y"
    { ;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 1513 "def.y"
    {
          if (defCallbacks->ViaStartCbk)
            CALLBACK(defCallbacks->ViaStartCbk, defrViaStartCbkType, ROUND((yyvsp[(2) - (3)].dval)));
        ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 1522 "def.y"
    {defData->dumb_mode = 1;defData->no_num = 1; ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 1523 "def.y"
    {
              if (defCallbacks->ViaCbk) defData->Via.setup((yyvsp[(3) - (3)].string));
              defData->viaRule = 0;
            ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1528 "def.y"
    {
              if (defCallbacks->ViaCbk)
                CALLBACK(defCallbacks->ViaCbk, defrViaCbkType, &defData->Via);
              defData->Via.clear();
            ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 1538 "def.y"
    {defData->dumb_mode = 1;defData->no_num = 1; ;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 1539 "def.y"
    { 
              if (defCallbacks->ViaCbk)
                if (validateMaskInput((yyvsp[(5) - (7)].integer), defData->viaWarnings, defSettings->ViaWarnings)) {
                    defData->Via.addLayer((yyvsp[(4) - (7)].string), (yyvsp[(6) - (7)].pt).x, (yyvsp[(6) - (7)].pt).y, (yyvsp[(7) - (7)].pt).x, (yyvsp[(7) - (7)].pt).y, (yyvsp[(5) - (7)].integer));
                }
            ;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 1545 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 1546 "def.y"
    {
              if (defData->VersionNum < 5.6) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defData->defMsg = (char*)defMalloc(1000);
                    sprintf (defData->defMsg,
                       "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                    defError(6509, defData->defMsg);
                    defFree(defData->defMsg);
                    CHKERR();
                  }
                }
              }
              
              defData->Geometries.Reset();
              
            ;}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 1564 "def.y"
    {
              if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
                if (defCallbacks->ViaCbk)
                  if (validateMaskInput((yyvsp[(5) - (10)].integer), defData->viaWarnings, defSettings->ViaWarnings)) {
                    defData->Via.addPolygon((yyvsp[(4) - (10)].string), &defData->Geometries, (yyvsp[(5) - (10)].integer));
                  }
              }
            ;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 1572 "def.y"
    {defData->dumb_mode = 1;defData->no_num = 1; ;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 1573 "def.y"
    {
              if (defData->VersionNum < 5.6) {
                if (defCallbacks->ViaCbk)
                  defData->Via.addPattern((yyvsp[(4) - (4)].string));
              } else
                if (defCallbacks->ViaCbk)
                  if (defData->viaWarnings++ < defSettings->ViaWarnings)
                    defWarning(7019, "The PATTERNNAME statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement."); 
            ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 1582 "def.y"
    {defData->dumb_mode = 1;defData->no_num = 1; ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 1584 "def.y"
    {defData->dumb_mode = 3;defData->no_num = 1; ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 1587 "def.y"
    {
               defData->viaRule = 1;
               if (defData->VersionNum < 5.6) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defData->defMsg = (char*)defMalloc(1000);
                    sprintf (defData->defMsg,
                       "The VIARULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                    defError(6557, defData->defMsg);
                    defFree(defData->defMsg);
                    CHKERR();
                  }
                }
              } else {
                if (defCallbacks->ViaCbk)
                   defData->Via.addViaRule((yyvsp[(4) - (24)].string), (int)(yyvsp[(7) - (24)].dval), (int)(yyvsp[(8) - (24)].dval), (yyvsp[(12) - (24)].string), (yyvsp[(13) - (24)].string),
                                             (yyvsp[(14) - (24)].string), (int)(yyvsp[(17) - (24)].dval), (int)(yyvsp[(18) - (24)].dval), (int)(yyvsp[(21) - (24)].dval),
                                             (int)(yyvsp[(22) - (24)].dval), (int)(yyvsp[(23) - (24)].dval), (int)(yyvsp[(24) - (24)].dval)); 
              }
            ;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 1609 "def.y"
    { 
            if (defCallbacks->ViaExtCbk)
              CALLBACK(defCallbacks->ViaExtCbk, defrViaExtCbkType, &defData->History_text[0]);
          ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 1615 "def.y"
    {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6559, "The ROWCOL statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addRowCol((int)(yyvsp[(3) - (4)].dval), (int)(yyvsp[(4) - (4)].dval));
            ;}
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 1627 "def.y"
    {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6560, "The ORIGIN statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addOrigin((int)(yyvsp[(3) - (4)].dval), (int)(yyvsp[(4) - (4)].dval));
            ;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 1639 "def.y"
    {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6561, "The OFFSET statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addOffset((int)(yyvsp[(3) - (6)].dval), (int)(yyvsp[(4) - (6)].dval), (int)(yyvsp[(5) - (6)].dval), (int)(yyvsp[(6) - (6)].dval));
            ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 1650 "def.y"
    {defData->dumb_mode = 1;defData->no_num = 1; ;}
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 1651 "def.y"
    {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6562, "The PATTERN statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addCutPattern((yyvsp[(4) - (4)].string));
            ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 1664 "def.y"
    { defData->Geometries.startList((yyvsp[(1) - (1)].pt).x, (yyvsp[(1) - (1)].pt).y); ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 1667 "def.y"
    { defData->Geometries.addToList((yyvsp[(1) - (1)].pt).x, (yyvsp[(1) - (1)].pt).y); ;}
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 1674 "def.y"
    {
            defData->save_x = (yyvsp[(2) - (4)].dval);
            defData->save_y = (yyvsp[(3) - (4)].dval);
            (yyval.pt).x = ROUND((yyvsp[(2) - (4)].dval));
            (yyval.pt).y = ROUND((yyvsp[(3) - (4)].dval));
          ;}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 1681 "def.y"
    {
            defData->save_y = (yyvsp[(3) - (4)].dval);
            (yyval.pt).x = ROUND(defData->save_x);
            (yyval.pt).y = ROUND((yyvsp[(3) - (4)].dval));
          ;}
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 1687 "def.y"
    {
            defData->save_x = (yyvsp[(2) - (4)].dval);
            (yyval.pt).x = ROUND((yyvsp[(2) - (4)].dval));
            (yyval.pt).y = ROUND(defData->save_y);
          ;}
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 1693 "def.y"
    {
            (yyval.pt).x = ROUND(defData->save_x);
            (yyval.pt).y = ROUND(defData->save_y);
          ;}
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 1699 "def.y"
    { (yyval.integer) = 0; ;}
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 1701 "def.y"
    { (yyval.integer) = (yyvsp[(3) - (3)].dval); ;}
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 1704 "def.y"
    { 
          if (defCallbacks->ViaEndCbk)
            CALLBACK(defCallbacks->ViaEndCbk, defrViaEndCbkType, 0);
        ;}
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 1710 "def.y"
    {
          if (defCallbacks->RegionEndCbk)
            CALLBACK(defCallbacks->RegionEndCbk, defrRegionEndCbkType, 0);
        ;}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 1716 "def.y"
    {
          if (defCallbacks->RegionStartCbk)
            CALLBACK(defCallbacks->RegionStartCbk, defrRegionStartCbkType, ROUND((yyvsp[(2) - (3)].dval)));
        ;}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 1723 "def.y"
    {;}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 1725 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 1726 "def.y"
    {
          if (defCallbacks->RegionCbk)
             defData->Region.setup((yyvsp[(3) - (3)].string));
          defData->regTypeDef = 0;
        ;}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 1732 "def.y"
    { CALLBACK(defCallbacks->RegionCbk, defrRegionCbkType, &defData->Region); ;}
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 1736 "def.y"
    { if (defCallbacks->RegionCbk)
          defData->Region.addRect((yyvsp[(1) - (2)].pt).x, (yyvsp[(1) - (2)].pt).y, (yyvsp[(2) - (2)].pt).x, (yyvsp[(2) - (2)].pt).y); ;}
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 1739 "def.y"
    { if (defCallbacks->RegionCbk)
          defData->Region.addRect((yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, (yyvsp[(3) - (3)].pt).x, (yyvsp[(3) - (3)].pt).y); ;}
    break;

  case 268:

/* Line 1455 of yacc.c  */
#line 1747 "def.y"
    {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; ;}
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 1749 "def.y"
    { defData->dumb_mode = 0; defData->parsing_property = 0; ;}
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 1751 "def.y"
    {
           if (defData->regTypeDef) {
              if (defCallbacks->RegionCbk) {
                if (defData->regionWarnings++ < defSettings->RegionWarnings) {
                  defError(6563, "The TYPE statement already exists. It has been defined in the REGION statement.");
                  CHKERR();
                }
              }
           }
           if (defCallbacks->RegionCbk) defData->Region.setType((yyvsp[(3) - (3)].string));
           defData->regTypeDef = 1;
         ;}
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 1770 "def.y"
    {
          if (defCallbacks->RegionCbk) {
             char propTp;
             char* str = ringCopy("                       ");
             propTp = defData->RegionProp.propType((yyvsp[(1) - (2)].string));
             CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "REGION");
             // For backword compatibility, also set the string value 
             // We will use a temporary string to store the number.
             // The string space is borrowed from the ring buffer
             // in the lexer.
             sprintf(str, "%g", (yyvsp[(2) - (2)].dval));
             defData->Region.addNumProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), str, propTp);
          }
        ;}
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 1785 "def.y"
    {
          if (defCallbacks->RegionCbk) {
             char propTp;
             propTp = defData->RegionProp.propType((yyvsp[(1) - (2)].string));
             CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "REGION");
             defData->Region.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
          }
        ;}
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 1794 "def.y"
    {
          if (defCallbacks->RegionCbk) {
             char propTp;
             propTp = defData->RegionProp.propType((yyvsp[(1) - (2)].string));
             CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "REGION");
             defData->Region.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
          }
        ;}
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 1804 "def.y"
    { (yyval.string) = (char*)"FENCE"; ;}
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 1806 "def.y"
    { (yyval.string) = (char*)"GUIDE"; ;}
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 1809 "def.y"
    {
           if (defData->VersionNum < 5.8) {
                if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                   defData->defMsg = (char*)defMalloc(10000);
                   sprintf (defData->defMsg,
                     "The MASKSHIFT statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                   defError(7415, defData->defMsg);
                   defFree(defData->defMsg);
                   CHKERR();
                }
            }
            if (defCallbacks->ComponentMaskShiftLayerCbk) {
                CALLBACK(defCallbacks->ComponentMaskShiftLayerCbk, defrComponentMaskShiftLayerCbkType, &defData->ComponentMaskShiftLayer);
            }
         ;}
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 1829 "def.y"
    { 
            if (defCallbacks->ComponentStartCbk)
              CALLBACK(defCallbacks->ComponentStartCbk, defrComponentStartCbkType,
                       ROUND((yyvsp[(2) - (3)].dval)));
         ;}
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 1840 "def.y"
    {
            if (defCallbacks->ComponentMaskShiftLayerCbk) {
              defData->ComponentMaskShiftLayer.addMaskShiftLayer((yyvsp[(1) - (1)].string));
            }
        ;}
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 1851 "def.y"
    {
            if (defCallbacks->ComponentCbk)
              CALLBACK(defCallbacks->ComponentCbk, defrComponentCbkType, &defData->Component);
         ;}
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 1857 "def.y"
    {
            defData->dumb_mode = 0;
            defData->no_num = 0;
         ;}
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 1862 "def.y"
    {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; ;}
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 1864 "def.y"
    {
            if (defCallbacks->ComponentCbk)
              defData->Component.IdAndName((yyvsp[(3) - (4)].string), (yyvsp[(4) - (4)].string));
         ;}
    break;

  case 290:

/* Line 1455 of yacc.c  */
#line 1870 "def.y"
    { ;}
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 1872 "def.y"
    {
              if (defCallbacks->ComponentCbk)
                defData->Component.addNet("*");
            ;}
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 1877 "def.y"
    {
              if (defCallbacks->ComponentCbk)
                defData->Component.addNet((yyvsp[(2) - (2)].string));
            ;}
    break;

  case 307:

/* Line 1455 of yacc.c  */
#line 1892 "def.y"
    {
          if (defCallbacks->ComponentCbk)
            CALLBACK(defCallbacks->ComponentExtCbk, defrComponentExtCbkType,
                     &defData->History_text[0]);
        ;}
    break;

  case 308:

/* Line 1455 of yacc.c  */
#line 1898 "def.y"
    {defData->dumb_mode=1; defData->no_num = 1; ;}
    break;

  case 309:

/* Line 1455 of yacc.c  */
#line 1899 "def.y"
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setEEQ((yyvsp[(4) - (4)].string));
        ;}
    break;

  case 310:

/* Line 1455 of yacc.c  */
#line 1904 "def.y"
    { defData->dumb_mode = 2;  defData->no_num = 2; ;}
    break;

  case 311:

/* Line 1455 of yacc.c  */
#line 1906 "def.y"
    {
          if (defCallbacks->ComponentCbk)
             defData->Component.setGenerate((yyvsp[(4) - (5)].string), (yyvsp[(5) - (5)].string));
        ;}
    break;

  case 312:

/* Line 1455 of yacc.c  */
#line 1912 "def.y"
    { (yyval.string) = (char*)""; ;}
    break;

  case 313:

/* Line 1455 of yacc.c  */
#line 1914 "def.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 314:

/* Line 1455 of yacc.c  */
#line 1917 "def.y"
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setSource((yyvsp[(3) - (3)].string));
        ;}
    break;

  case 315:

/* Line 1455 of yacc.c  */
#line 1923 "def.y"
    { (yyval.string) = (char*)"NETLIST"; ;}
    break;

  case 316:

/* Line 1455 of yacc.c  */
#line 1925 "def.y"
    { (yyval.string) = (char*)"DIST"; ;}
    break;

  case 317:

/* Line 1455 of yacc.c  */
#line 1927 "def.y"
    { (yyval.string) = (char*)"USER"; ;}
    break;

  case 318:

/* Line 1455 of yacc.c  */
#line 1929 "def.y"
    { (yyval.string) = (char*)"TIMING"; ;}
    break;

  case 319:

/* Line 1455 of yacc.c  */
#line 1934 "def.y"
    { ;}
    break;

  case 320:

/* Line 1455 of yacc.c  */
#line 1936 "def.y"
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setRegionName((yyvsp[(2) - (2)].string));
        ;}
    break;

  case 321:

/* Line 1455 of yacc.c  */
#line 1942 "def.y"
    { 
          // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->ComponentCbk)
               defData->Component.setRegionBounds((yyvsp[(1) - (2)].pt).x, (yyvsp[(1) - (2)].pt).y, 
                                                            (yyvsp[(2) - (2)].pt).x, (yyvsp[(2) - (2)].pt).y);
          }
          else
            defWarning(7020, "The REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
        ;}
    break;

  case 322:

/* Line 1455 of yacc.c  */
#line 1953 "def.y"
    { 
          // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->ComponentCbk)
               defData->Component.setRegionBounds((yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y,
                                                            (yyvsp[(3) - (3)].pt).x, (yyvsp[(3) - (3)].pt).y);
          }
          else
            defWarning(7020, "The REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
        ;}
    break;

  case 323:

/* Line 1455 of yacc.c  */
#line 1965 "def.y"
    {
          if (defData->VersionNum < 5.6) {
             if (defCallbacks->ComponentCbk) {
               if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                 defData->defMsg = (char*)defMalloc(1000);
                 sprintf (defData->defMsg,
                    "The HALO statement is a version 5.6 and later syntax.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                 defError(6529, defData->defMsg);
                 defFree(defData->defMsg);
                 CHKERR();
               }
             }
          }
        ;}
    break;

  case 324:

/* Line 1455 of yacc.c  */
#line 1980 "def.y"
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setHalo((int)(yyvsp[(5) - (8)].dval), (int)(yyvsp[(6) - (8)].dval),
                                                 (int)(yyvsp[(7) - (8)].dval), (int)(yyvsp[(8) - (8)].dval));
        ;}
    break;

  case 326:

/* Line 1455 of yacc.c  */
#line 1988 "def.y"
    {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->ComponentCbk) {
             if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                defData->defMsg = (char*)defMalloc(10000);
                sprintf (defData->defMsg,
                  "The HALO SOFT is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                defError(6550, defData->defMsg);
                defFree(defData->defMsg);
                CHKERR();
             }
           }
        } else {
           if (defCallbacks->ComponentCbk)
             defData->Component.setHaloSoft();
        }
      ;}
    break;

  case 327:

/* Line 1455 of yacc.c  */
#line 2007 "def.y"
    { defData->dumb_mode = 2; defData->no_num = 2; ;}
    break;

  case 328:

/* Line 1455 of yacc.c  */
#line 2008 "def.y"
    {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->ComponentCbk) {
             if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                defData->defMsg = (char*)defMalloc(10000);
                sprintf (defData->defMsg,
                  "The ROUTEHALO is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                defError(6551, defData->defMsg);
                defFree(defData->defMsg);
                CHKERR();
             }
           }
        } else {
           if (defCallbacks->ComponentCbk)
             defData->Component.setRouteHalo(
                            (int)(yyvsp[(3) - (6)].dval), (yyvsp[(5) - (6)].string), (yyvsp[(6) - (6)].string));
        }
      ;}
    break;

  case 329:

/* Line 1455 of yacc.c  */
#line 2027 "def.y"
    { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; ;}
    break;

  case 330:

/* Line 1455 of yacc.c  */
#line 2029 "def.y"
    { defData->dumb_mode = 0; defData->parsing_property = 0; ;}
    break;

  case 333:

/* Line 1455 of yacc.c  */
#line 2036 "def.y"
    {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            char* str = ringCopy("                       ");
            propTp = defData->CompProp.propType((yyvsp[(1) - (2)].string));
            CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "COMPONENT");
            sprintf(str, "%g", (yyvsp[(2) - (2)].dval));
            defData->Component.addNumProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), str, propTp);
          }
        ;}
    break;

  case 334:

/* Line 1455 of yacc.c  */
#line 2047 "def.y"
    {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            propTp = defData->CompProp.propType((yyvsp[(1) - (2)].string));
            CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "COMPONENT");
            defData->Component.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
          }
        ;}
    break;

  case 335:

/* Line 1455 of yacc.c  */
#line 2056 "def.y"
    {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            propTp = defData->CompProp.propType((yyvsp[(1) - (2)].string));
            CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "COMPONENT");
            defData->Component.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
          }
        ;}
    break;

  case 336:

/* Line 1455 of yacc.c  */
#line 2066 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 337:

/* Line 1455 of yacc.c  */
#line 2068 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 338:

/* Line 1455 of yacc.c  */
#line 2070 "def.y"
    { 
          if (defData->VersionNum < 5.6) {
            if (defCallbacks->ComponentCbk) {
              defData->Component.setForeignName((yyvsp[(4) - (6)].string));
              defData->Component.setForeignLocation((yyvsp[(5) - (6)].pt).x, (yyvsp[(5) - (6)].pt).y, (yyvsp[(6) - (6)].integer));
            }
         } else
            if (defCallbacks->ComponentCbk)
              if (defData->componentWarnings++ < defSettings->ComponentWarnings)
                defWarning(7021, "The FOREIGN statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement.");
         ;}
    break;

  case 339:

/* Line 1455 of yacc.c  */
#line 2084 "def.y"
    { (yyval.pt) = (yyvsp[(1) - (1)].pt); ;}
    break;

  case 340:

/* Line 1455 of yacc.c  */
#line 2086 "def.y"
    { (yyval.pt).x = ROUND((yyvsp[(1) - (2)].dval)); (yyval.pt).y = ROUND((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 341:

/* Line 1455 of yacc.c  */
#line 2089 "def.y"
    {
          if (defCallbacks->ComponentCbk) {
            defData->Component.setPlacementStatus((yyvsp[(1) - (3)].integer));
            defData->Component.setPlacementLocation((yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, (yyvsp[(3) - (3)].integer));
          }
        ;}
    break;

  case 342:

/* Line 1455 of yacc.c  */
#line 2096 "def.y"
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setPlacementStatus(
                                         DEFI_COMPONENT_UNPLACED);
            defData->Component.setPlacementLocation(-1, -1, -1);
        ;}
    break;

  case 343:

/* Line 1455 of yacc.c  */
#line 2103 "def.y"
    {
          if (defData->VersionNum < 5.4) {   // PCR 495463 
            if (defCallbacks->ComponentCbk) {
              defData->Component.setPlacementStatus(
                                          DEFI_COMPONENT_UNPLACED);
              defData->Component.setPlacementLocation((yyvsp[(3) - (4)].pt).x, (yyvsp[(3) - (4)].pt).y, (yyvsp[(4) - (4)].integer));
            }
          } else {
            if (defData->componentWarnings++ < defSettings->ComponentWarnings)
               defWarning(7022, "In the COMPONENT UNPLACED statement, point and orient are invalid in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        ;}
    break;

  case 344:

/* Line 1455 of yacc.c  */
#line 2117 "def.y"
    {  
          if (defCallbacks->ComponentCbk) {
            if (validateMaskInput((int)(yyvsp[(3) - (3)].dval), defData->componentWarnings, defSettings->ComponentWarnings)) {
                defData->Component.setMaskShift(int((yyvsp[(3) - (3)].dval)));
            }
          }
        ;}
    break;

  case 345:

/* Line 1455 of yacc.c  */
#line 2126 "def.y"
    { (yyval.integer) = DEFI_COMPONENT_FIXED; ;}
    break;

  case 346:

/* Line 1455 of yacc.c  */
#line 2128 "def.y"
    { (yyval.integer) = DEFI_COMPONENT_COVER; ;}
    break;

  case 347:

/* Line 1455 of yacc.c  */
#line 2130 "def.y"
    { (yyval.integer) = DEFI_COMPONENT_PLACED; ;}
    break;

  case 348:

/* Line 1455 of yacc.c  */
#line 2133 "def.y"
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setWeight(ROUND((yyvsp[(3) - (3)].dval)));
        ;}
    break;

  case 349:

/* Line 1455 of yacc.c  */
#line 2139 "def.y"
    { 
          if (defCallbacks->ComponentCbk)
            CALLBACK(defCallbacks->ComponentEndCbk, defrComponentEndCbkType, 0);
        ;}
    break;

  case 351:

/* Line 1455 of yacc.c  */
#line 2148 "def.y"
    { 
          if (defCallbacks->NetStartCbk)
            CALLBACK(defCallbacks->NetStartCbk, defrNetStartCbkType, ROUND((yyvsp[(2) - (3)].dval)));
          defData->netOsnet = 1;
        ;}
    break;

  case 354:

/* Line 1455 of yacc.c  */
#line 2159 "def.y"
    { 
          if (defCallbacks->NetCbk)
            CALLBACK(defCallbacks->NetCbk, defrNetCbkType, &defData->Net);
        ;}
    break;

  case 355:

/* Line 1455 of yacc.c  */
#line 2170 "def.y"
    {defData->dumb_mode = 0; defData->no_num = 0; ;}
    break;

  case 356:

/* Line 1455 of yacc.c  */
#line 2173 "def.y"
    {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; defData->nondef_is_keyword = TRUE; defData->mustjoin_is_keyword = TRUE;;}
    break;

  case 358:

/* Line 1455 of yacc.c  */
#line 2176 "def.y"
    {
          // 9/22/1999 
          // this is shared by both net and special net 
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.setName((yyvsp[(1) - (1)].string));
          if (defCallbacks->NetNameCbk)
            CALLBACK(defCallbacks->NetNameCbk, defrNetNameCbkType, (yyvsp[(1) - (1)].string));
        ;}
    break;

  case 360:

/* Line 1455 of yacc.c  */
#line 2184 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1;;}
    break;

  case 361:

/* Line 1455 of yacc.c  */
#line 2185 "def.y"
    {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addMustPin((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].string), 0);
          defData->dumb_mode = 3;
          defData->no_num = 3;
        ;}
    break;

  case 364:

/* Line 1455 of yacc.c  */
#line 2196 "def.y"
    {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT;;}
    break;

  case 365:

/* Line 1455 of yacc.c  */
#line 2198 "def.y"
    {
          // 9/22/1999 
          // since the code is shared by both net & special net, 
          // need to check on both flags 
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin((yyvsp[(2) - (6)].string), (yyvsp[(4) - (6)].string), (yyvsp[(5) - (6)].integer));
          // 1/14/2000 - pcr 289156 
          // reset defData->dumb_mode & defData->no_num to 3 , just in case 
          // the next statement is another net_connection 
          defData->dumb_mode = 3;
          defData->no_num = 3;
        ;}
    break;

  case 366:

/* Line 1455 of yacc.c  */
#line 2210 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1;;}
    break;

  case 367:

/* Line 1455 of yacc.c  */
#line 2211 "def.y"
    {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin("*", (yyvsp[(4) - (6)].string), (yyvsp[(5) - (6)].integer));
          defData->dumb_mode = 3;
          defData->no_num = 3;
        ;}
    break;

  case 368:

/* Line 1455 of yacc.c  */
#line 2217 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1;;}
    break;

  case 369:

/* Line 1455 of yacc.c  */
#line 2218 "def.y"
    {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin("PIN", (yyvsp[(4) - (6)].string), (yyvsp[(5) - (6)].integer));
          defData->dumb_mode = 3;
          defData->no_num = 3;
        ;}
    break;

  case 370:

/* Line 1455 of yacc.c  */
#line 2226 "def.y"
    { (yyval.integer) = 0; ;}
    break;

  case 371:

/* Line 1455 of yacc.c  */
#line 2228 "def.y"
    {
          if (defCallbacks->NetConnectionExtCbk)
            CALLBACK(defCallbacks->NetConnectionExtCbk, defrNetConnectionExtCbkType,
              &defData->History_text[0]);
          (yyval.integer) = 0;
        ;}
    break;

  case 372:

/* Line 1455 of yacc.c  */
#line 2235 "def.y"
    { (yyval.integer) = 1; ;}
    break;

  case 375:

/* Line 1455 of yacc.c  */
#line 2244 "def.y"
    {  
          if (defCallbacks->NetCbk) defData->Net.addWire((yyvsp[(2) - (2)].string), NULL);
        ;}
    break;

  case 376:

/* Line 1455 of yacc.c  */
#line 2248 "def.y"
    {
          defData->by_is_keyword = FALSE;
          defData->do_is_keyword = FALSE;
          defData->new_is_keyword = FALSE;
          defData->nondef_is_keyword = FALSE;
          defData->mustjoin_is_keyword = FALSE;
          defData->step_is_keyword = FALSE;
          defData->orient_is_keyword = FALSE;
          defData->virtual_is_keyword = FALSE;
          defData->rect_is_keyword = FALSE;
          defData->mask_is_keyword = FALSE;
          defData->needNPCbk = 0;
        ;}
    break;

  case 377:

/* Line 1455 of yacc.c  */
#line 2263 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.setSource((yyvsp[(3) - (3)].string)); ;}
    break;

  case 378:

/* Line 1455 of yacc.c  */
#line 2266 "def.y"
    {
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->NetCbk) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                 defData->defMsg = (char*)defMalloc(1000);
                 sprintf (defData->defMsg,
                    "The FIXEDBUMP statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                 defError(6530, defData->defMsg);
                 defFree(defData->defMsg);
                 CHKERR();
              }
            }
          }
          if (defCallbacks->NetCbk) defData->Net.setFixedbump();
        ;}
    break;

  case 379:

/* Line 1455 of yacc.c  */
#line 2282 "def.y"
    { defData->real_num = 1; ;}
    break;

  case 380:

/* Line 1455 of yacc.c  */
#line 2283 "def.y"
    {
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->NetCbk) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                 defData->defMsg = (char*)defMalloc(1000);
                 sprintf (defData->defMsg,
                    "The FREQUENCY statement is a version 5.5 and later syntax.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                 defError(6558, defData->defMsg);
                 defFree(defData->defMsg);
                 CHKERR();
              }
            }
          }
          if (defCallbacks->NetCbk) defData->Net.setFrequency((yyvsp[(4) - (4)].dval));
          defData->real_num = 0;
        ;}
    break;

  case 381:

/* Line 1455 of yacc.c  */
#line 2300 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1;;}
    break;

  case 382:

/* Line 1455 of yacc.c  */
#line 2301 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.setOriginal((yyvsp[(4) - (4)].string)); ;}
    break;

  case 383:

/* Line 1455 of yacc.c  */
#line 2304 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.setPattern((yyvsp[(3) - (3)].string)); ;}
    break;

  case 384:

/* Line 1455 of yacc.c  */
#line 2307 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.setWeight(ROUND((yyvsp[(3) - (3)].dval))); ;}
    break;

  case 385:

/* Line 1455 of yacc.c  */
#line 2310 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.setXTalk(ROUND((yyvsp[(3) - (3)].dval))); ;}
    break;

  case 386:

/* Line 1455 of yacc.c  */
#line 2313 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.setCap((yyvsp[(3) - (3)].dval)); ;}
    break;

  case 387:

/* Line 1455 of yacc.c  */
#line 2316 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.setUse((yyvsp[(3) - (3)].string)); ;}
    break;

  case 388:

/* Line 1455 of yacc.c  */
#line 2319 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.setStyle((int)(yyvsp[(3) - (3)].dval)); ;}
    break;

  case 389:

/* Line 1455 of yacc.c  */
#line 2321 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 390:

/* Line 1455 of yacc.c  */
#line 2322 "def.y"
    { 
          if (defCallbacks->NetCbk && defCallbacks->NetNonDefaultRuleCbk) {
             // User wants a callback on nondefaultrule 
             CALLBACK(defCallbacks->NetNonDefaultRuleCbk,
                      defrNetNonDefaultRuleCbkType, (yyvsp[(4) - (4)].string));
          }
          // Still save data in the class 
          if (defCallbacks->NetCbk) defData->Net.setNonDefaultRule((yyvsp[(4) - (4)].string));
        ;}
    break;

  case 392:

/* Line 1455 of yacc.c  */
#line 2334 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 393:

/* Line 1455 of yacc.c  */
#line 2335 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.addShieldNet((yyvsp[(4) - (4)].string)); ;}
    break;

  case 394:

/* Line 1455 of yacc.c  */
#line 2337 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 395:

/* Line 1455 of yacc.c  */
#line 2338 "def.y"
    { // since the parser still support 5.3 and earlier, can't 
          // move NOSHIELD in net_type 
          if (defData->VersionNum < 5.4) {   // PCR 445209 
            if (defCallbacks->NetCbk) defData->Net.addNoShield("");
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->shield = TRUE;    // save the path info in the defData->shield paths 
          } else
            if (defCallbacks->NetCbk) defData->Net.addWire("NOSHIELD", NULL);
        ;}
    break;

  case 396:

/* Line 1455 of yacc.c  */
#line 2355 "def.y"
    {
          if (defData->VersionNum < 5.4) {   // PCR 445209 
            defData->shield = FALSE;
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->nondef_is_keyword = FALSE;
            defData->mustjoin_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
          } else {
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->nondef_is_keyword = FALSE;
            defData->mustjoin_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
          }
          defData->needNPCbk = 0;
        ;}
    break;

  case 397:

/* Line 1455 of yacc.c  */
#line 2384 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1;
          if (defCallbacks->NetCbk) {
            defData->Subnet = (defiSubnet*)defMalloc(sizeof(defiSubnet));
            defData->Subnet->Init();
          }
        ;}
    break;

  case 398:

/* Line 1455 of yacc.c  */
#line 2390 "def.y"
    {
          if (defCallbacks->NetCbk && defCallbacks->NetSubnetNameCbk) {
            // User wants a callback on Net subnetName 
            CALLBACK(defCallbacks->NetSubnetNameCbk, defrNetSubnetNameCbkType, (yyvsp[(4) - (4)].string));
          }
          // Still save the subnet name in the class 
          if (defCallbacks->NetCbk) {
            defData->Subnet->setName((yyvsp[(4) - (4)].string));
          }
        ;}
    break;

  case 399:

/* Line 1455 of yacc.c  */
#line 2400 "def.y"
    {
          defData->routed_is_keyword = TRUE;
          defData->fixed_is_keyword = TRUE;
          defData->cover_is_keyword = TRUE;
        ;}
    break;

  case 400:

/* Line 1455 of yacc.c  */
#line 2404 "def.y"
    {
          if (defCallbacks->NetCbk) {
            defData->Net.addSubnet(defData->Subnet);
            defData->Subnet = NULL;
            defData->routed_is_keyword = FALSE;
            defData->fixed_is_keyword = FALSE;
            defData->cover_is_keyword = FALSE;
          }
        ;}
    break;

  case 401:

/* Line 1455 of yacc.c  */
#line 2414 "def.y"
    {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; ;}
    break;

  case 402:

/* Line 1455 of yacc.c  */
#line 2416 "def.y"
    { defData->dumb_mode = 0; defData->parsing_property = 0; ;}
    break;

  case 403:

/* Line 1455 of yacc.c  */
#line 2419 "def.y"
    { 
          if (defCallbacks->NetExtCbk)
            CALLBACK(defCallbacks->NetExtCbk, defrNetExtCbkType, &defData->History_text[0]);
        ;}
    break;

  case 406:

/* Line 1455 of yacc.c  */
#line 2429 "def.y"
    {
          if (defCallbacks->NetCbk) {
            char propTp;
            char* str = ringCopy("                       ");
            propTp = defData->NetProp.propType((yyvsp[(1) - (2)].string));
            CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "NET");
            sprintf(str, "%g", (yyvsp[(2) - (2)].dval));
            defData->Net.addNumProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), str, propTp);
          }
        ;}
    break;

  case 407:

/* Line 1455 of yacc.c  */
#line 2440 "def.y"
    {
          if (defCallbacks->NetCbk) {
            char propTp;
            propTp = defData->NetProp.propType((yyvsp[(1) - (2)].string));
            CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "NET");
            defData->Net.addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
          }
        ;}
    break;

  case 408:

/* Line 1455 of yacc.c  */
#line 2449 "def.y"
    {
          if (defCallbacks->NetCbk) {
            char propTp;
            propTp = defData->NetProp.propType((yyvsp[(1) - (2)].string));
            CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "NET");
            defData->Net.addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
          }
        ;}
    break;

  case 409:

/* Line 1455 of yacc.c  */
#line 2459 "def.y"
    { (yyval.string) = (char*)"NETLIST"; ;}
    break;

  case 410:

/* Line 1455 of yacc.c  */
#line 2461 "def.y"
    { (yyval.string) = (char*)"DIST"; ;}
    break;

  case 411:

/* Line 1455 of yacc.c  */
#line 2463 "def.y"
    { (yyval.string) = (char*)"USER"; ;}
    break;

  case 412:

/* Line 1455 of yacc.c  */
#line 2465 "def.y"
    { (yyval.string) = (char*)"TIMING"; ;}
    break;

  case 413:

/* Line 1455 of yacc.c  */
#line 2467 "def.y"
    { (yyval.string) = (char*)"TEST"; ;}
    break;

  case 414:

/* Line 1455 of yacc.c  */
#line 2470 "def.y"
    {
          // vpin_options may have to deal with orient 
          defData->orient_is_keyword = TRUE;
        ;}
    break;

  case 415:

/* Line 1455 of yacc.c  */
#line 2475 "def.y"
    { if (defCallbacks->NetCbk)
            defData->Net.addVpinBounds((yyvsp[(3) - (6)].pt).x, (yyvsp[(3) - (6)].pt).y, (yyvsp[(4) - (6)].pt).x, (yyvsp[(4) - (6)].pt).y);
          defData->orient_is_keyword = FALSE;
        ;}
    break;

  case 416:

/* Line 1455 of yacc.c  */
#line 2480 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1;;}
    break;

  case 417:

/* Line 1455 of yacc.c  */
#line 2481 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.addVpin((yyvsp[(4) - (4)].string)); ;}
    break;

  case 419:

/* Line 1455 of yacc.c  */
#line 2484 "def.y"
    {defData->dumb_mode=1;;}
    break;

  case 420:

/* Line 1455 of yacc.c  */
#line 2485 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.addVpinLayer((yyvsp[(3) - (3)].string)); ;}
    break;

  case 422:

/* Line 1455 of yacc.c  */
#line 2489 "def.y"
    { if (defCallbacks->NetCbk) defData->Net.addVpinLoc((yyvsp[(1) - (3)].string), (yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, (yyvsp[(3) - (3)].integer)); ;}
    break;

  case 423:

/* Line 1455 of yacc.c  */
#line 2492 "def.y"
    { (yyval.string) = (char*)"PLACED"; ;}
    break;

  case 424:

/* Line 1455 of yacc.c  */
#line 2494 "def.y"
    { (yyval.string) = (char*)"FIXED"; ;}
    break;

  case 425:

/* Line 1455 of yacc.c  */
#line 2496 "def.y"
    { (yyval.string) = (char*)"COVER"; ;}
    break;

  case 426:

/* Line 1455 of yacc.c  */
#line 2499 "def.y"
    { (yyval.string) = (char*)"FIXED"; defData->dumb_mode = 1; ;}
    break;

  case 427:

/* Line 1455 of yacc.c  */
#line 2501 "def.y"
    { (yyval.string) = (char*)"COVER"; defData->dumb_mode = 1; ;}
    break;

  case 428:

/* Line 1455 of yacc.c  */
#line 2503 "def.y"
    { (yyval.string) = (char*)"ROUTED"; defData->dumb_mode = 1; ;}
    break;

  case 429:

/* Line 1455 of yacc.c  */
#line 2507 "def.y"
    { if (defData->NeedPathData && defCallbacks->NetCbk)
          pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needNPCbk);
      ;}
    break;

  case 430:

/* Line 1455 of yacc.c  */
#line 2511 "def.y"
    { ;}
    break;

  case 431:

/* Line 1455 of yacc.c  */
#line 2513 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 432:

/* Line 1455 of yacc.c  */
#line 2514 "def.y"
    { if (defData->NeedPathData && defCallbacks->NetCbk)
          pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needNPCbk);
      ;}
    break;

  case 433:

/* Line 1455 of yacc.c  */
#line 2519 "def.y"
    {
        if ((strcmp((yyvsp[(1) - (1)].string), "TAPER") == 0) || (strcmp((yyvsp[(1) - (1)].string), "TAPERRULE") == 0)) {
          if (defData->NeedPathData && defCallbacks->NetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6531, "The layerName which is required in path is missing. Include the layerName in the path and then try again.");
              CHKERR();
            }
          }
          // Since there is already error, the next token is insignificant 
          defData->dumb_mode = 1; defData->no_num = 1;
        } else {
          // CCR 766289 - Do not accummulate the layer information if there 
          // is not a callback set 
          if (defData->NeedPathData && defCallbacks->NetCbk)
              defData->PathObj.addLayer((yyvsp[(1) - (1)].string));
          defData->dumb_mode = 0; defData->no_num = 0;
        }
      ;}
    break;

  case 434:

/* Line 1455 of yacc.c  */
#line 2538 "def.y"
    { defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
/*
       dumb_mode = 1; by_is_keyword = TRUE; do_is_keyword = TRUE;
*/
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE; 
        defData->orient_is_keyword = TRUE; defData->virtual_is_keyword = TRUE;
        defData->mask_is_keyword = TRUE, defData->rect_is_keyword = TRUE;  ;}
    break;

  case 435:

/* Line 1455 of yacc.c  */
#line 2548 "def.y"
    { defData->dumb_mode = 0;   defData->virtual_is_keyword = FALSE; defData->mask_is_keyword = FALSE,
       defData->rect_is_keyword = FALSE; ;}
    break;

  case 436:

/* Line 1455 of yacc.c  */
#line 2553 "def.y"
    {
      if (defData->VersionNum < 5.8) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The VIRTUAL statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
          }
    ;}
    break;

  case 437:

/* Line 1455 of yacc.c  */
#line 2570 "def.y"
    {
      if (defData->VersionNum < 5.8) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The RECT statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
      }
    ;}
    break;

  case 440:

/* Line 1455 of yacc.c  */
#line 2593 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          if (strcmp((yyvsp[(1) - (1)].string), "TAPER") == 0)
            defData->PathObj.setTaper();
          else {
            defData->PathObj.addVia((yyvsp[(1) - (1)].string));
            }
        }
      ;}
    break;

  case 441:

/* Line 1455 of yacc.c  */
#line 2604 "def.y"
    {
        if (validateMaskInput((int)(yyvsp[(2) - (3)].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
                (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
              if (strcmp((yyvsp[(3) - (3)].string), "TAPER") == 0)
                defData->PathObj.setTaper();
              else {
                defData->PathObj.addViaMask((yyvsp[(2) - (3)].dval));
                defData->PathObj.addVia((yyvsp[(3) - (3)].string));
                }
            }
        }
      ;}
    break;

  case 442:

/* Line 1455 of yacc.c  */
#line 2618 "def.y"
    { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
            defData->PathObj.addVia((yyvsp[(1) - (2)].string));
            defData->PathObj.addViaRotation((yyvsp[(2) - (2)].integer));
        }
      ;}
    break;

  case 443:

/* Line 1455 of yacc.c  */
#line 2625 "def.y"
    { 
        if (validateMaskInput((int)(yyvsp[(2) - (4)].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
                (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
                defData->PathObj.addViaMask((yyvsp[(2) - (4)].dval));
                defData->PathObj.addVia((yyvsp[(3) - (4)].string));
                defData->PathObj.addViaRotation((yyvsp[(4) - (4)].integer));
            }
        }
      ;}
    break;

  case 444:

/* Line 1455 of yacc.c  */
#line 2636 "def.y"
    {
        if (validateMaskInput((int)(yyvsp[(2) - (10)].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {      
            if (((yyvsp[(5) - (10)].dval) == 0) || ((yyvsp[(7) - (10)].dval) == 0)) {
              if (defData->NeedPathData &&
                  defCallbacks->SNetCbk) {
                if (defData->netWarnings++ < defSettings->NetWarnings) {
                  defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
                  CHKERR();
                }
              }
            }
            if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
                defData->PathObj.addViaMask((yyvsp[(2) - (10)].dval));
                defData->PathObj.addVia((yyvsp[(3) - (10)].string));
                defData->PathObj.addViaData((int)(yyvsp[(5) - (10)].dval), (int)(yyvsp[(7) - (10)].dval), (int)(yyvsp[(9) - (10)].dval), (int)(yyvsp[(10) - (10)].dval));
            }  else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
                CHKERR();
              }
            }
        }
      ;}
    break;

  case 445:

/* Line 1455 of yacc.c  */
#line 2660 "def.y"
    {
        if (defData->VersionNum < 5.5) {
          if (defData->NeedPathData && 
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The VIA DO statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6532, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        }
        if (((yyvsp[(3) - (8)].dval) == 0) || ((yyvsp[(5) - (8)].dval) == 0)) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
              CHKERR();
            }
          }
        }
        if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
            defData->PathObj.addVia((yyvsp[(1) - (8)].string));
            defData->PathObj.addViaData((int)(yyvsp[(3) - (8)].dval), (int)(yyvsp[(5) - (8)].dval), (int)(yyvsp[(7) - (8)].dval), (int)(yyvsp[(8) - (8)].dval));
        }  else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
          if (defData->netWarnings++ < defSettings->NetWarnings) {
            defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
            CHKERR();
          }
        }
      ;}
    break;

  case 446:

/* Line 1455 of yacc.c  */
#line 2694 "def.y"
    {
        if (defData->VersionNum < 5.5) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The VIA DO statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6532, defData->defMsg);
              CHKERR();
            }
          }
        }
        if (((yyvsp[(4) - (9)].dval) == 0) || ((yyvsp[(6) - (9)].dval) == 0)) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
              CHKERR();
            }
          }
        }
        if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
            defData->PathObj.addVia((yyvsp[(1) - (9)].string));
            defData->PathObj.addViaRotation((yyvsp[(2) - (9)].integer));
            defData->PathObj.addViaData((int)(yyvsp[(4) - (9)].dval), (int)(yyvsp[(6) - (9)].dval), (int)(yyvsp[(8) - (9)].dval), (int)(yyvsp[(9) - (9)].dval));
        } else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
          if (defData->netWarnings++ < defSettings->NetWarnings) {
            defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
            CHKERR();
          }
        }
      ;}
    break;

  case 447:

/* Line 1455 of yacc.c  */
#line 2728 "def.y"
    {
        if (validateMaskInput((int)(yyvsp[(2) - (11)].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (((yyvsp[(6) - (11)].dval) == 0) || ((yyvsp[(8) - (11)].dval) == 0)) {
              if (defData->NeedPathData &&
                  defCallbacks->SNetCbk) {
                if (defData->netWarnings++ < defSettings->NetWarnings) {
                  defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
                  CHKERR();
                }
              }
            }
            if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
                defData->PathObj.addViaMask((yyvsp[(2) - (11)].dval)); 
                defData->PathObj.addVia((yyvsp[(3) - (11)].string));
                defData->PathObj.addViaRotation((yyvsp[(4) - (11)].integer));;
                defData->PathObj.addViaData((int)(yyvsp[(6) - (11)].dval), (int)(yyvsp[(8) - (11)].dval), (int)(yyvsp[(10) - (11)].dval), (int)(yyvsp[(11) - (11)].dval));
            } else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
                CHKERR();
              }
            }
        }
      ;}
    break;

  case 450:

/* Line 1455 of yacc.c  */
#line 2754 "def.y"
    { defData->dumb_mode = 6; ;}
    break;

  case 451:

/* Line 1455 of yacc.c  */
#line 2755 "def.y"
    {
      if (validateMaskInput((int)(yyvsp[(2) - (10)].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addMask((yyvsp[(2) - (10)].dval));
          defData->PathObj.addViaRect((yyvsp[(6) - (10)].dval), (yyvsp[(7) - (10)].dval), (yyvsp[(8) - (10)].dval), (yyvsp[(9) - (10)].dval));
        }
      }
    ;}
    break;

  case 452:

/* Line 1455 of yacc.c  */
#line 2765 "def.y"
    {
       if (validateMaskInput((int)(yyvsp[(2) - (2)].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addMask((yyvsp[(2) - (2)].dval)); 
        }
       }  
    ;}
    break;

  case 454:

/* Line 1455 of yacc.c  */
#line 2775 "def.y"
    {
       // reset defData->dumb_mode to 1 just incase the next token is a via of the path
        // 2/5/2004 - pcr 686781
        defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE;
        defData->orient_is_keyword = TRUE;
    ;}
    break;

  case 455:

/* Line 1455 of yacc.c  */
#line 2786 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND((yyvsp[(2) - (4)].dval)), ROUND((yyvsp[(3) - (4)].dval))); 
        defData->save_x = (yyvsp[(2) - (4)].dval);
        defData->save_y = (yyvsp[(3) - (4)].dval); 
      ;}
    break;

  case 456:

/* Line 1455 of yacc.c  */
#line 2794 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND(defData->save_x), ROUND((yyvsp[(3) - (4)].dval))); 
        defData->save_y = (yyvsp[(3) - (4)].dval);
      ;}
    break;

  case 457:

/* Line 1455 of yacc.c  */
#line 2801 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND((yyvsp[(2) - (4)].dval)), ROUND(defData->save_y)); 
        defData->save_x = (yyvsp[(2) - (4)].dval);
      ;}
    break;

  case 458:

/* Line 1455 of yacc.c  */
#line 2808 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND(defData->save_x), ROUND(defData->save_y)); 
      ;}
    break;

  case 459:

/* Line 1455 of yacc.c  */
#line 2814 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND((yyvsp[(2) - (5)].dval)), ROUND((yyvsp[(3) - (5)].dval)), ROUND((yyvsp[(4) - (5)].dval))); 
        defData->save_x = (yyvsp[(2) - (5)].dval);
        defData->save_y = (yyvsp[(3) - (5)].dval);
      ;}
    break;

  case 460:

/* Line 1455 of yacc.c  */
#line 2822 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND(defData->save_x), ROUND((yyvsp[(3) - (5)].dval)),
          ROUND((yyvsp[(4) - (5)].dval))); 
        defData->save_y = (yyvsp[(3) - (5)].dval);
      ;}
    break;

  case 461:

/* Line 1455 of yacc.c  */
#line 2830 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND((yyvsp[(2) - (5)].dval)), ROUND(defData->save_y),
          ROUND((yyvsp[(4) - (5)].dval))); 
        defData->save_x = (yyvsp[(2) - (5)].dval);
      ;}
    break;

  case 462:

/* Line 1455 of yacc.c  */
#line 2838 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND(defData->save_x), ROUND(defData->save_y),
          ROUND((yyvsp[(4) - (5)].dval))); 
      ;}
    break;

  case 463:

/* Line 1455 of yacc.c  */
#line 2847 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND((yyvsp[(2) - (4)].dval)), ROUND((yyvsp[(3) - (4)].dval))); 
        defData->save_x = (yyvsp[(2) - (4)].dval);
        defData->save_y = (yyvsp[(3) - (4)].dval);
      ;}
    break;

  case 464:

/* Line 1455 of yacc.c  */
#line 2855 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND(defData->save_x), ROUND((yyvsp[(3) - (4)].dval))); 
        defData->save_y = (yyvsp[(3) - (4)].dval);
      ;}
    break;

  case 465:

/* Line 1455 of yacc.c  */
#line 2862 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND((yyvsp[(2) - (4)].dval)), ROUND(defData->save_y)); 
        defData->save_x = (yyvsp[(2) - (4)].dval);
      ;}
    break;

  case 466:

/* Line 1455 of yacc.c  */
#line 2871 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addViaRect((yyvsp[(2) - (6)].dval), (yyvsp[(3) - (6)].dval), (yyvsp[(4) - (6)].dval), (yyvsp[(5) - (6)].dval)); 
        }    
    ;}
    break;

  case 471:

/* Line 1455 of yacc.c  */
#line 2887 "def.y"
    { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.setTaper(); ;}
    break;

  case 472:

/* Line 1455 of yacc.c  */
#line 2890 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 473:

/* Line 1455 of yacc.c  */
#line 2891 "def.y"
    { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addTaperRule((yyvsp[(3) - (3)].string)); ;}
    break;

  case 474:

/* Line 1455 of yacc.c  */
#line 2896 "def.y"
    { 
        if (defData->VersionNum < 5.6) {
           if (defData->NeedPathData && (defCallbacks->NetCbk ||
               defCallbacks->SNetCbk)) {
             if (defData->netWarnings++ < defSettings->NetWarnings) {
               defData->defMsg = (char*)defMalloc(1000);
               sprintf (defData->defMsg,
                  "The STYLE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
               defError(6534, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else
           if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
             (defCallbacks->SNetCbk && (defData->netOsnet==2))))
             defData->PathObj.addStyle((int)(yyvsp[(2) - (2)].dval));
      ;}
    break;

  case 477:

/* Line 1455 of yacc.c  */
#line 2921 "def.y"
    { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addShape((yyvsp[(3) - (3)].string)); ;}
    break;

  case 478:

/* Line 1455 of yacc.c  */
#line 2925 "def.y"
    { if (defData->VersionNum < 5.6) {
          if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The STYLE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6534, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else {
          if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
            defData->PathObj.addStyle((int)(yyvsp[(3) - (3)].dval));
        }
      ;}
    break;

  case 479:

/* Line 1455 of yacc.c  */
#line 2945 "def.y"
    { 
            CALLBACK(defCallbacks->NetEndCbk, defrNetEndCbkType, 0);
            defData->netOsnet = 0;
          ;}
    break;

  case 480:

/* Line 1455 of yacc.c  */
#line 2951 "def.y"
    { (yyval.string) = (char*)"RING"; ;}
    break;

  case 481:

/* Line 1455 of yacc.c  */
#line 2953 "def.y"
    { (yyval.string) = (char*)"STRIPE"; ;}
    break;

  case 482:

/* Line 1455 of yacc.c  */
#line 2955 "def.y"
    { (yyval.string) = (char*)"FOLLOWPIN"; ;}
    break;

  case 483:

/* Line 1455 of yacc.c  */
#line 2957 "def.y"
    { (yyval.string) = (char*)"IOWIRE"; ;}
    break;

  case 484:

/* Line 1455 of yacc.c  */
#line 2959 "def.y"
    { (yyval.string) = (char*)"COREWIRE"; ;}
    break;

  case 485:

/* Line 1455 of yacc.c  */
#line 2961 "def.y"
    { (yyval.string) = (char*)"BLOCKWIRE"; ;}
    break;

  case 486:

/* Line 1455 of yacc.c  */
#line 2963 "def.y"
    { (yyval.string) = (char*)"FILLWIRE"; ;}
    break;

  case 487:

/* Line 1455 of yacc.c  */
#line 2965 "def.y"
    {
              if (defData->VersionNum < 5.7) {
                 if (defData->NeedPathData) {
                   if (defData->fillWarnings++ < defSettings->FillWarnings) {
                     defData->defMsg = (char*)defMalloc(10000);
                     sprintf (defData->defMsg,
                       "The FILLWIREOPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                     defError(6552, defData->defMsg);
                     defFree(defData->defMsg);
                     CHKERR();
                  }
                }
              }
              (yyval.string) = (char*)"FILLWIREOPC";
            ;}
    break;

  case 488:

/* Line 1455 of yacc.c  */
#line 2981 "def.y"
    { (yyval.string) = (char*)"DRCFILL"; ;}
    break;

  case 489:

/* Line 1455 of yacc.c  */
#line 2983 "def.y"
    { (yyval.string) = (char*)"BLOCKAGEWIRE"; ;}
    break;

  case 490:

/* Line 1455 of yacc.c  */
#line 2985 "def.y"
    { (yyval.string) = (char*)"PADRING"; ;}
    break;

  case 491:

/* Line 1455 of yacc.c  */
#line 2987 "def.y"
    { (yyval.string) = (char*)"BLOCKRING"; ;}
    break;

  case 495:

/* Line 1455 of yacc.c  */
#line 2997 "def.y"
    { CALLBACK(defCallbacks->SNetCbk, defrSNetCbkType, &defData->Net); ;}
    break;

  case 502:

/* Line 1455 of yacc.c  */
#line 3008 "def.y"
    {
             if (defData->VersionNum >= 5.8) {
                defData->specialWire_routeStatus = (yyvsp[(2) - (2)].string);
             } else {
                 if (defCallbacks->SNetCbk) {   // PCR 902306 
                   defData->defMsg = (char*)defMalloc(1024);
                   sprintf(defData->defMsg, "The SPECIAL NET statement, with type %s, does not have any net statement defined.\nThe DEF parser will ignore this statemnet.", (yyvsp[(2) - (2)].string));
                   defWarning(7023, defData->defMsg);
                   defFree(defData->defMsg);
                 }
             }
            ;}
    break;

  case 503:

/* Line 1455 of yacc.c  */
#line 3021 "def.y"
    {
            if (defCallbacks->SNetCbk) defData->Net.addWire((yyvsp[(2) - (2)].string), NULL);
            ;}
    break;

  case 504:

/* Line 1455 of yacc.c  */
#line 3025 "def.y"
    {
            // 7/17/2003 - Fix for pcr 604848, add a callback for each wire
            if (defCallbacks->SNetWireCbk) {
               CALLBACK(defCallbacks->SNetWireCbk, defrSNetWireCbkType, &defData->Net);
               defData->Net.freeWire();
            }
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->needSNPCbk = 0;
            ;}
    break;

  case 505:

/* Line 1455 of yacc.c  */
#line 3042 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 506:

/* Line 1455 of yacc.c  */
#line 3043 "def.y"
    { defData->shieldName = (yyvsp[(4) - (4)].string); 
              defData->specialWire_routeStatus = (char*)"SHIELD";
              defData->specialWire_routeStatusName = (yyvsp[(4) - (4)].string); 
            ;}
    break;

  case 508:

/* Line 1455 of yacc.c  */
#line 3050 "def.y"
    {  
            defData->specialWire_shapeType = (yyvsp[(3) - (3)].string);
          ;}
    break;

  case 509:

/* Line 1455 of yacc.c  */
#line 3054 "def.y"
    {
            if (validateMaskInput((int)(yyvsp[(3) - (3)].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
                defData->specialWire_mask = (yyvsp[(3) - (3)].dval);
            }     
          ;}
    break;

  case 510:

/* Line 1455 of yacc.c  */
#line 3059 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 511:

/* Line 1455 of yacc.c  */
#line 3060 "def.y"
    {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6535, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            
            defData->Geometries.Reset();
          ;}
    break;

  case 512:

/* Line 1455 of yacc.c  */
#line 3077 "def.y"
    {
            if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
              if (defCallbacks->SNetCbk) {
                // defData->needSNPCbk will indicate that it has reach the max
                // memory and if user has set partialPathCBk, def parser
                // will make the callback.
                // This will improve performance
                // This construct is only in specialnet
                defData->Net.addPolygon((yyvsp[(4) - (9)].string), &defData->Geometries, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType,
                                                                defData->specialWire_routeStatusName);
                defData->specialWire_mask = 0;
                defData->specialWire_routeStatus = (char*)"ROUTED";
                defData->specialWire_shapeType = (char*)"";
                if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
                   CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                            &defData->Net);
                   defData->Net.clearRectPolyNPath();
                   defData->Net.clearVia();
                }
              }
            }
          ;}
    break;

  case 513:

/* Line 1455 of yacc.c  */
#line 3100 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 514:

/* Line 1455 of yacc.c  */
#line 3101 "def.y"
    {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The RECT statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->SNetCbk) {
              // defData->needSNPCbk will indicate that it has reach the max
              // memory and if user has set partialPathCBk, def parser
              // will make the callback.
              // This will improve performance
              // This construct is only in specialnet
              defData->Net.addRect((yyvsp[(4) - (6)].string), (yyvsp[(5) - (6)].pt).x, (yyvsp[(5) - (6)].pt).y, (yyvsp[(6) - (6)].pt).x, (yyvsp[(6) - (6)].pt).y, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType, defData->specialWire_routeStatusName);
              defData->specialWire_mask = 0;
              defData->specialWire_routeStatus = (char*)"ROUTED";
              defData->specialWire_shapeType = (char*)"";
              defData->specialWire_routeStatusName = (char*)"";
              if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
                 CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                          &defData->Net);
                 defData->Net.clearRectPolyNPath();
                 defData->Net.clearVia();
              }
            }
          ;}
    break;

  case 515:

/* Line 1455 of yacc.c  */
#line 3133 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 516:

/* Line 1455 of yacc.c  */
#line 3134 "def.y"
    {
          if (defData->VersionNum < 5.8) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The VIA statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
          }
        ;}
    break;

  case 517:

/* Line 1455 of yacc.c  */
#line 3149 "def.y"
    {
          if (defData->VersionNum >= 5.8 && defCallbacks->SNetCbk) {
              defData->Net.addPts((yyvsp[(4) - (8)].string), (yyvsp[(5) - (8)].integer), &defData->Geometries, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType,
                                                          defData->specialWire_routeStatusName);
              defData->specialWire_mask = 0;
              defData->specialWire_routeStatus = (char*)"ROUTED";
              defData->specialWire_shapeType = (char*)"";
              defData->specialWire_routeStatusName = (char*)"";
              if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
                 CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                          &defData->Net);
                 defData->Net.clearRectPolyNPath();
                 defData->Net.clearVia();
              }
            }
        ;}
    break;

  case 518:

/* Line 1455 of yacc.c  */
#line 3167 "def.y"
    { if (defCallbacks->SNetCbk) defData->Net.setSource((yyvsp[(3) - (3)].string)); ;}
    break;

  case 519:

/* Line 1455 of yacc.c  */
#line 3170 "def.y"
    { if (defCallbacks->SNetCbk) defData->Net.setFixedbump(); ;}
    break;

  case 520:

/* Line 1455 of yacc.c  */
#line 3173 "def.y"
    { if (defCallbacks->SNetCbk) defData->Net.setFrequency((yyvsp[(3) - (3)].dval)); ;}
    break;

  case 521:

/* Line 1455 of yacc.c  */
#line 3175 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1;;}
    break;

  case 522:

/* Line 1455 of yacc.c  */
#line 3176 "def.y"
    { if (defCallbacks->SNetCbk) defData->Net.setOriginal((yyvsp[(4) - (4)].string)); ;}
    break;

  case 523:

/* Line 1455 of yacc.c  */
#line 3179 "def.y"
    { if (defCallbacks->SNetCbk) defData->Net.setPattern((yyvsp[(3) - (3)].string)); ;}
    break;

  case 524:

/* Line 1455 of yacc.c  */
#line 3182 "def.y"
    { if (defCallbacks->SNetCbk) defData->Net.setWeight(ROUND((yyvsp[(3) - (3)].dval))); ;}
    break;

  case 525:

/* Line 1455 of yacc.c  */
#line 3185 "def.y"
    { 
              // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
              if (defData->VersionNum < 5.5)
                 if (defCallbacks->SNetCbk) defData->Net.setCap((yyvsp[(3) - (3)].dval));
              else
                 defWarning(7024, "The ESTCAP statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
            ;}
    break;

  case 526:

/* Line 1455 of yacc.c  */
#line 3194 "def.y"
    { if (defCallbacks->SNetCbk) defData->Net.setUse((yyvsp[(3) - (3)].string)); ;}
    break;

  case 527:

/* Line 1455 of yacc.c  */
#line 3197 "def.y"
    { if (defCallbacks->SNetCbk) defData->Net.setStyle((int)(yyvsp[(3) - (3)].dval)); ;}
    break;

  case 528:

/* Line 1455 of yacc.c  */
#line 3199 "def.y"
    {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; ;}
    break;

  case 529:

/* Line 1455 of yacc.c  */
#line 3201 "def.y"
    { defData->dumb_mode = 0; defData->parsing_property = 0; ;}
    break;

  case 530:

/* Line 1455 of yacc.c  */
#line 3204 "def.y"
    { CALLBACK(defCallbacks->NetExtCbk, defrNetExtCbkType, &defData->History_text[0]); ;}
    break;

  case 531:

/* Line 1455 of yacc.c  */
#line 3207 "def.y"
    { (yyval.integer) = 0; ;}
    break;

  case 532:

/* Line 1455 of yacc.c  */
#line 3208 "def.y"
    {(yyval.integer) = 0;;}
    break;

  case 533:

/* Line 1455 of yacc.c  */
#line 3209 "def.y"
    {(yyval.integer) = 1;;}
    break;

  case 534:

/* Line 1455 of yacc.c  */
#line 3210 "def.y"
    {(yyval.integer) = 2;;}
    break;

  case 535:

/* Line 1455 of yacc.c  */
#line 3211 "def.y"
    {(yyval.integer) = 3;;}
    break;

  case 536:

/* Line 1455 of yacc.c  */
#line 3212 "def.y"
    {(yyval.integer) = 4;;}
    break;

  case 537:

/* Line 1455 of yacc.c  */
#line 3213 "def.y"
    {(yyval.integer) = 5;;}
    break;

  case 538:

/* Line 1455 of yacc.c  */
#line 3214 "def.y"
    {(yyval.integer) = 6;;}
    break;

  case 539:

/* Line 1455 of yacc.c  */
#line 3215 "def.y"
    {(yyval.integer) = 7;;}
    break;

  case 540:

/* Line 1455 of yacc.c  */
#line 3218 "def.y"
    {
                if (defCallbacks->SNetCbk) {
                    if (defData->VersionNum < 5.8) { 
                        defData->defMsg = (char*)defMalloc(1024);
                        sprintf(defData->defMsg, "The SPECIAL NET SHIELD statement doesn't have routing points definition.\nWill be ignored.");
                        defWarning(7025, defData->defMsg);
                        defFree(defData->defMsg);
                    } else {  // CCR 1244433
                      defData->specialWire_routeStatus = (char*)"SHIELD";
                      defData->specialWire_routeStatusName = defData->shieldName;
                    }
                }
            ;}
    break;

  case 541:

/* Line 1455 of yacc.c  */
#line 3232 "def.y"
    { // since the parser still supports 5.3 and earlier, 
              // can't just move SHIELD in net_type 
              if (defData->VersionNum < 5.4) { // PCR 445209 
                if (defCallbacks->SNetCbk) defData->Net.addShield(defData->shieldName);
                defData->by_is_keyword = FALSE;
                defData->do_is_keyword = FALSE;
                defData->new_is_keyword = FALSE;
                defData->step_is_keyword = FALSE;
                defData->orient_is_keyword = FALSE;
                defData->virtual_is_keyword = FALSE;
                defData->mask_is_keyword = FALSE;
                defData->rect_is_keyword = FALSE;
                defData->specialWire_routeStatus = (char*)"ROUTED";
                defData->specialWire_routeStatusName = (char*)"";
                defData->shield = TRUE;   // save the path info in the defData->shield paths 
              } else
                if (defCallbacks->SNetCbk) defData->Net.addWire("SHIELD", defData->shieldName);
                defData->specialWire_routeStatus = (char*)"ROUTED";
                defData->specialWire_routeStatusName = (char*)"";
            ;}
    break;

  case 542:

/* Line 1455 of yacc.c  */
#line 3253 "def.y"
    {
              // 7/17/2003 - Fix for pcr 604848, add a callback for each wire
              if (defCallbacks->SNetWireCbk) {
                 CALLBACK(defCallbacks->SNetWireCbk, defrSNetWireCbkType, &defData->Net);
                 if (defData->VersionNum < 5.4)
                   defData->Net.freeShield();
                 else
                   defData->Net.freeWire();
              }
              if (defData->VersionNum < 5.4) {  // PCR 445209 
                defData->shield = FALSE;
                defData->by_is_keyword = FALSE;
                defData->do_is_keyword = FALSE;
                defData->new_is_keyword = FALSE;
                defData->step_is_keyword = FALSE;
                defData->nondef_is_keyword = FALSE;
                defData->mustjoin_is_keyword = FALSE;
                defData->orient_is_keyword = FALSE;
                defData->virtual_is_keyword = FALSE;
                defData->mask_is_keyword = FALSE;
                defData->rect_is_keyword = FALSE;
              } else {
                defData->by_is_keyword = FALSE;
                defData->do_is_keyword = FALSE;
                defData->new_is_keyword = FALSE;
                defData->step_is_keyword = FALSE;
                defData->orient_is_keyword = FALSE;
                defData->virtual_is_keyword = FALSE;
                defData->mask_is_keyword = FALSE;
                defData->rect_is_keyword = FALSE;
              }
              defData->needSNPCbk = 0;
            ;}
    break;

  case 543:

/* Line 1455 of yacc.c  */
#line 3287 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 544:

/* Line 1455 of yacc.c  */
#line 3288 "def.y"
    {
              // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
              if (defData->VersionNum < 5.5)
                 if (defCallbacks->SNetCbk) defData->Net.setWidth((yyvsp[(4) - (5)].string), (yyvsp[(5) - (5)].dval));
              else
                 defWarning(7026, "The WIDTH statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
            ;}
    break;

  case 545:

/* Line 1455 of yacc.c  */
#line 3296 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 546:

/* Line 1455 of yacc.c  */
#line 3297 "def.y"
    {
              if (numIsInt((yyvsp[(4) - (4)].string))) {
                 if (defCallbacks->SNetCbk) defData->Net.setVoltage(atoi((yyvsp[(4) - (4)].string)));
              } else {
                 if (defCallbacks->SNetCbk) {
                   if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                     defData->defMsg = (char*)defMalloc(1000);
                     sprintf (defData->defMsg,
                        "The value %s for statement VOLTAGE is invalid. The value can only be integer.\nSpecify a valid value in units of millivolts", (yyvsp[(4) - (4)].string));
                     defError(6537, defData->defMsg);
                     defFree(defData->defMsg);
                     CHKERR();
                   }
                 }
              }
            ;}
    break;

  case 547:

/* Line 1455 of yacc.c  */
#line 3314 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 548:

/* Line 1455 of yacc.c  */
#line 3315 "def.y"
    {
              if (defCallbacks->SNetCbk) defData->Net.setSpacing((yyvsp[(4) - (5)].string),(yyvsp[(5) - (5)].dval));
            ;}
    break;

  case 549:

/* Line 1455 of yacc.c  */
#line 3319 "def.y"
    {
            ;}
    break;

  case 552:

/* Line 1455 of yacc.c  */
#line 3327 "def.y"
    {
              if (defCallbacks->SNetCbk) {
                char propTp;
                char* str = ringCopy("                       ");
                propTp = defData->SNetProp.propType((yyvsp[(1) - (2)].string));
                CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "SPECIAL NET");
                // For backword compatibility, also set the string value 
                sprintf(str, "%g", (yyvsp[(2) - (2)].dval));
                defData->Net.addNumProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), str, propTp);
              }
            ;}
    break;

  case 553:

/* Line 1455 of yacc.c  */
#line 3339 "def.y"
    {
              if (defCallbacks->SNetCbk) {
                char propTp;
                propTp = defData->SNetProp.propType((yyvsp[(1) - (2)].string));
                CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "SPECIAL NET");
                defData->Net.addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
              }
            ;}
    break;

  case 554:

/* Line 1455 of yacc.c  */
#line 3348 "def.y"
    {
              if (defCallbacks->SNetCbk) {
                char propTp;
                propTp = defData->SNetProp.propType((yyvsp[(1) - (2)].string));
                CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "SPECIAL NET");
                defData->Net.addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
              }
            ;}
    break;

  case 556:

/* Line 1455 of yacc.c  */
#line 3359 "def.y"
    {
              if (defCallbacks->SNetCbk) defData->Net.setRange((yyvsp[(2) - (3)].dval),(yyvsp[(3) - (3)].dval));
            ;}
    break;

  case 558:

/* Line 1455 of yacc.c  */
#line 3365 "def.y"
    { defData->Prop.setRange((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].dval)); ;}
    break;

  case 559:

/* Line 1455 of yacc.c  */
#line 3368 "def.y"
    { (yyval.string) = (char*)"BALANCED"; ;}
    break;

  case 560:

/* Line 1455 of yacc.c  */
#line 3370 "def.y"
    { (yyval.string) = (char*)"STEINER"; ;}
    break;

  case 561:

/* Line 1455 of yacc.c  */
#line 3372 "def.y"
    { (yyval.string) = (char*)"TRUNK"; ;}
    break;

  case 562:

/* Line 1455 of yacc.c  */
#line 3374 "def.y"
    { (yyval.string) = (char*)"WIREDLOGIC"; ;}
    break;

  case 563:

/* Line 1455 of yacc.c  */
#line 3378 "def.y"
    { 
        if (defData->NeedPathData && defCallbacks->SNetCbk) {
           if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) { 
              // require a callback before proceed because defData->needSNPCbk must be
              // set to 1 from the previous pathIsDone and user has registered
              // a callback routine.
              CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                       &defData->Net);
              defData->needSNPCbk = 0;   // reset the flag
              pathIsDone(defData->shield, 1, defData->netOsnet, &defData->needSNPCbk);
              defData->Net.clearRectPolyNPath();
              defData->Net.clearVia();
           } else
              pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needSNPCbk);
        }
      ;}
    break;

  case 564:

/* Line 1455 of yacc.c  */
#line 3395 "def.y"
    { ;}
    break;

  case 565:

/* Line 1455 of yacc.c  */
#line 3397 "def.y"
    { defData->dumb_mode = 1; ;}
    break;

  case 566:

/* Line 1455 of yacc.c  */
#line 3398 "def.y"
    { if (defData->NeedPathData && defCallbacks->SNetCbk) {
           if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
              // require a callback before proceed because defData->needSNPCbk must be
              // set to 1 from the previous pathIsDone and user has registered
              // a callback routine.
              CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                       &defData->Net);
              defData->needSNPCbk = 0;   // reset the flag
              pathIsDone(defData->shield, 1, defData->netOsnet, &defData->needSNPCbk);
              // reset any poly or rect in special wiring statement
              defData->Net.clearRectPolyNPath();
              defData->Net.clearVia();
           } else
              pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needSNPCbk);
        }
      ;}
    break;

  case 567:

/* Line 1455 of yacc.c  */
#line 3416 "def.y"
    { if (defData->NeedPathData && defCallbacks->SNetCbk)
           defData->PathObj.addLayer((yyvsp[(1) - (1)].string));
        defData->dumb_mode = 0; defData->no_num = 0;
      ;}
    break;

  case 568:

/* Line 1455 of yacc.c  */
#line 3424 "def.y"
    { defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE;
         defData->orient_is_keyword = TRUE; defData->rect_is_keyword = TRUE, defData->mask_is_keyword = TRUE; 
         defData->virtual_is_keyword = TRUE;  ;}
    break;

  case 569:

/* Line 1455 of yacc.c  */
#line 3430 "def.y"
    { defData->dumb_mode = 0; defData->rect_is_keyword = FALSE, defData->mask_is_keyword = FALSE, defData->virtual_is_keyword = FALSE; ;}
    break;

  case 570:

/* Line 1455 of yacc.c  */
#line 3433 "def.y"
    { if (defData->NeedPathData && defCallbacks->SNetCbk)
          defData->PathObj.addWidth(ROUND((yyvsp[(1) - (1)].dval)));
      ;}
    break;

  case 571:

/* Line 1455 of yacc.c  */
#line 3438 "def.y"
    { 
        if (defCallbacks->SNetStartCbk)
          CALLBACK(defCallbacks->SNetStartCbk, defrSNetStartCbkType, ROUND((yyvsp[(2) - (3)].dval)));
        defData->netOsnet = 2;
      ;}
    break;

  case 572:

/* Line 1455 of yacc.c  */
#line 3445 "def.y"
    { 
        if (defCallbacks->SNetEndCbk)
          CALLBACK(defCallbacks->SNetEndCbk, defrSNetEndCbkType, 0);
        defData->netOsnet = 0;
      ;}
    break;

  case 574:

/* Line 1455 of yacc.c  */
#line 3455 "def.y"
    {
        if (defCallbacks->GroupsStartCbk)
           CALLBACK(defCallbacks->GroupsStartCbk, defrGroupsStartCbkType, ROUND((yyvsp[(2) - (3)].dval)));
      ;}
    break;

  case 577:

/* Line 1455 of yacc.c  */
#line 3465 "def.y"
    {
        if (defCallbacks->GroupCbk)
           CALLBACK(defCallbacks->GroupCbk, defrGroupCbkType, &defData->Group);
      ;}
    break;

  case 578:

/* Line 1455 of yacc.c  */
#line 3470 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 579:

/* Line 1455 of yacc.c  */
#line 3471 "def.y"
    {
        defData->dumb_mode = DEF_MAX_INT;
        defData->no_num = DEF_MAX_INT;
        /* dumb_mode is automatically turned off at the first
         * + in the options or at the ; at the end of the group */
        if (defCallbacks->GroupCbk) defData->Group.setup((yyvsp[(3) - (3)].string));
        if (defCallbacks->GroupNameCbk)
           CALLBACK(defCallbacks->GroupNameCbk, defrGroupNameCbkType, (yyvsp[(3) - (3)].string));
      ;}
    break;

  case 581:

/* Line 1455 of yacc.c  */
#line 3483 "def.y"
    {  ;}
    break;

  case 582:

/* Line 1455 of yacc.c  */
#line 3486 "def.y"
    {
        // if (defCallbacks->GroupCbk) defData->Group.addMember($1); 
        if (defCallbacks->GroupMemberCbk)
          CALLBACK(defCallbacks->GroupMemberCbk, defrGroupMemberCbkType, (yyvsp[(1) - (1)].string));
      ;}
    break;

  case 585:

/* Line 1455 of yacc.c  */
#line 3497 "def.y"
    { ;}
    break;

  case 586:

/* Line 1455 of yacc.c  */
#line 3498 "def.y"
    { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; ;}
    break;

  case 587:

/* Line 1455 of yacc.c  */
#line 3500 "def.y"
    { defData->dumb_mode = 0; defData->parsing_property = 0; ;}
    break;

  case 588:

/* Line 1455 of yacc.c  */
#line 3501 "def.y"
    { defData->dumb_mode = 1;  defData->no_num = 1; ;}
    break;

  case 589:

/* Line 1455 of yacc.c  */
#line 3502 "def.y"
    { ;}
    break;

  case 590:

/* Line 1455 of yacc.c  */
#line 3504 "def.y"
    { 
        if (defCallbacks->GroupMemberCbk)
          CALLBACK(defCallbacks->GroupExtCbk, defrGroupExtCbkType, &defData->History_text[0]);
      ;}
    break;

  case 591:

/* Line 1455 of yacc.c  */
#line 3510 "def.y"
    {
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5) {
          if (defCallbacks->GroupCbk)
            defData->Group.addRegionRect((yyvsp[(1) - (2)].pt).x, (yyvsp[(1) - (2)].pt).y, (yyvsp[(2) - (2)].pt).x, (yyvsp[(2) - (2)].pt).y);
        }
        else
          defWarning(7027, "The GROUP REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      ;}
    break;

  case 592:

/* Line 1455 of yacc.c  */
#line 3520 "def.y"
    { if (defCallbacks->GroupCbk)
          defData->Group.setRegionName((yyvsp[(1) - (1)].string));
      ;}
    break;

  case 595:

/* Line 1455 of yacc.c  */
#line 3529 "def.y"
    {
        if (defCallbacks->GroupCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defData->GroupProp.propType((yyvsp[(1) - (2)].string));
          CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "GROUP");
          sprintf(str, "%g", (yyvsp[(2) - (2)].dval));
          defData->Group.addNumProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), str, propTp);
        }
      ;}
    break;

  case 596:

/* Line 1455 of yacc.c  */
#line 3540 "def.y"
    {
        if (defCallbacks->GroupCbk) {
          char propTp;
          propTp = defData->GroupProp.propType((yyvsp[(1) - (2)].string));
          CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "GROUP");
          defData->Group.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
        }
      ;}
    break;

  case 597:

/* Line 1455 of yacc.c  */
#line 3549 "def.y"
    {
        if (defCallbacks->GroupCbk) {
          char propTp;
          propTp = defData->GroupProp.propType((yyvsp[(1) - (2)].string));
          CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "GROUP");
          defData->Group.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
        }
      ;}
    break;

  case 599:

/* Line 1455 of yacc.c  */
#line 3560 "def.y"
    { ;}
    break;

  case 600:

/* Line 1455 of yacc.c  */
#line 3563 "def.y"
    {
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setMaxX(ROUND((yyvsp[(2) - (2)].dval)));
        else
          defWarning(7028, "The GROUP SOFT MAXX statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      ;}
    break;

  case 601:

/* Line 1455 of yacc.c  */
#line 3571 "def.y"
    { 
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setMaxY(ROUND((yyvsp[(2) - (2)].dval)));
        else
          defWarning(7029, "The GROUP SOFT MAXY statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      ;}
    break;

  case 602:

/* Line 1455 of yacc.c  */
#line 3579 "def.y"
    { 
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setPerim(ROUND((yyvsp[(2) - (2)].dval)));
        else
          defWarning(7030, "The GROUP SOFT MAXHALFPERIMETER statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      ;}
    break;

  case 603:

/* Line 1455 of yacc.c  */
#line 3588 "def.y"
    { 
        if (defCallbacks->GroupsEndCbk)
          CALLBACK(defCallbacks->GroupsEndCbk, defrGroupsEndCbkType, 0);
      ;}
    break;

  case 606:

/* Line 1455 of yacc.c  */
#line 3602 "def.y"
    {
        if ((defData->VersionNum < 5.4) && (defCallbacks->AssertionsStartCbk)) {
          CALLBACK(defCallbacks->AssertionsStartCbk, defrAssertionsStartCbkType,
                   ROUND((yyvsp[(2) - (3)].dval)));
        } else {
          if (defCallbacks->AssertionCbk)
            if (defData->assertionWarnings++ < defSettings->AssertionWarnings)
              defWarning(7031, "The ASSERTIONS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
        if (defCallbacks->AssertionCbk)
          defData->Assertion.setAssertionMode();
      ;}
    break;

  case 607:

/* Line 1455 of yacc.c  */
#line 3616 "def.y"
    {
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintsStartCbk)) {
          CALLBACK(defCallbacks->ConstraintsStartCbk, defrConstraintsStartCbkType,
                   ROUND((yyvsp[(2) - (3)].dval)));
        } else {
          if (defCallbacks->ConstraintCbk)
            if (defData->constraintWarnings++ < defSettings->ConstraintWarnings)
              defWarning(7032, "The CONSTRAINTS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
        if (defCallbacks->ConstraintCbk)
          defData->Assertion.setConstraintMode();
      ;}
    break;

  case 611:

/* Line 1455 of yacc.c  */
#line 3635 "def.y"
    {
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)) {
          if (defData->Assertion.isConstraint()) 
            CALLBACK(defCallbacks->ConstraintCbk, defrConstraintCbkType, &defData->Assertion);
          if (defData->Assertion.isAssertion()) 
            CALLBACK(defCallbacks->AssertionCbk, defrAssertionCbkType, &defData->Assertion);
        }
      ;}
    break;

  case 612:

/* Line 1455 of yacc.c  */
#line 3645 "def.y"
    { 
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)) {
          if (defData->Assertion.isConstraint()) 
            CALLBACK(defCallbacks->ConstraintCbk, defrConstraintCbkType, &defData->Assertion);
          if (defData->Assertion.isAssertion()) 
            CALLBACK(defCallbacks->AssertionCbk, defrAssertionCbkType, &defData->Assertion);
        }
   
        // reset all the flags and everything
        defData->Assertion.clear();
      ;}
    break;

  case 613:

/* Line 1455 of yacc.c  */
#line 3657 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 614:

/* Line 1455 of yacc.c  */
#line 3658 "def.y"
    {
         if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.addNet((yyvsp[(3) - (3)].string));
      ;}
    break;

  case 615:

/* Line 1455 of yacc.c  */
#line 3662 "def.y"
    {defData->dumb_mode = 4; defData->no_num = 4;;}
    break;

  case 616:

/* Line 1455 of yacc.c  */
#line 3663 "def.y"
    {
         if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.addPath((yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string), (yyvsp[(5) - (6)].string), (yyvsp[(6) - (6)].string));
      ;}
    break;

  case 617:

/* Line 1455 of yacc.c  */
#line 3668 "def.y"
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.setSum();
      ;}
    break;

  case 618:

/* Line 1455 of yacc.c  */
#line 3673 "def.y"
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.setDiff();
      ;}
    break;

  case 620:

/* Line 1455 of yacc.c  */
#line 3680 "def.y"
    { ;}
    break;

  case 622:

/* Line 1455 of yacc.c  */
#line 3683 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 623:

/* Line 1455 of yacc.c  */
#line 3685 "def.y"
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setWiredlogic((yyvsp[(4) - (8)].string), (yyvsp[(7) - (8)].dval));
      ;}
    break;

  case 624:

/* Line 1455 of yacc.c  */
#line 3692 "def.y"
    { (yyval.string) = (char*)""; ;}
    break;

  case 625:

/* Line 1455 of yacc.c  */
#line 3694 "def.y"
    { (yyval.string) = (char*)"+"; ;}
    break;

  case 628:

/* Line 1455 of yacc.c  */
#line 3701 "def.y"
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setRiseMin((yyvsp[(3) - (3)].dval));
      ;}
    break;

  case 629:

/* Line 1455 of yacc.c  */
#line 3706 "def.y"
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setRiseMax((yyvsp[(3) - (3)].dval));
      ;}
    break;

  case 630:

/* Line 1455 of yacc.c  */
#line 3711 "def.y"
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setFallMin((yyvsp[(3) - (3)].dval));
      ;}
    break;

  case 631:

/* Line 1455 of yacc.c  */
#line 3716 "def.y"
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setFallMax((yyvsp[(3) - (3)].dval));
      ;}
    break;

  case 632:

/* Line 1455 of yacc.c  */
#line 3722 "def.y"
    { if ((defData->VersionNum < 5.4) && defCallbacks->ConstraintsEndCbk) {
          CALLBACK(defCallbacks->ConstraintsEndCbk, defrConstraintsEndCbkType, 0);
        } else {
          if (defCallbacks->ConstraintsEndCbk) {
            if (defData->constraintWarnings++ < defSettings->ConstraintWarnings)
              defWarning(7032, "The CONSTRAINTS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
      ;}
    break;

  case 633:

/* Line 1455 of yacc.c  */
#line 3733 "def.y"
    { if ((defData->VersionNum < 5.4) && defCallbacks->AssertionsEndCbk) {
          CALLBACK(defCallbacks->AssertionsEndCbk, defrAssertionsEndCbkType, 0);
        } else {
          if (defCallbacks->AssertionsEndCbk) {
            if (defData->assertionWarnings++ < defSettings->AssertionWarnings)
              defWarning(7031, "The ASSERTIONS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
      ;}
    break;

  case 635:

/* Line 1455 of yacc.c  */
#line 3747 "def.y"
    { if (defCallbacks->ScanchainsStartCbk)
          CALLBACK(defCallbacks->ScanchainsStartCbk, defrScanchainsStartCbkType,
                   ROUND((yyvsp[(2) - (3)].dval)));
      ;}
    break;

  case 637:

/* Line 1455 of yacc.c  */
#line 3754 "def.y"
    {;}
    break;

  case 638:

/* Line 1455 of yacc.c  */
#line 3757 "def.y"
    { 
        if (defCallbacks->ScanchainCbk)
          CALLBACK(defCallbacks->ScanchainCbk, defrScanchainCbkType, &defData->Scanchain);
      ;}
    break;

  case 639:

/* Line 1455 of yacc.c  */
#line 3762 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1;;}
    break;

  case 640:

/* Line 1455 of yacc.c  */
#line 3763 "def.y"
    {
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setName((yyvsp[(3) - (3)].string));
        defData->bit_is_keyword = TRUE;
      ;}
    break;

  case 643:

/* Line 1455 of yacc.c  */
#line 3775 "def.y"
    { (yyval.string) = (char*)""; ;}
    break;

  case 644:

/* Line 1455 of yacc.c  */
#line 3777 "def.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 645:

/* Line 1455 of yacc.c  */
#line 3779 "def.y"
    {defData->dumb_mode = 2; defData->no_num = 2;;}
    break;

  case 646:

/* Line 1455 of yacc.c  */
#line 3780 "def.y"
    { if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setStart((yyvsp[(4) - (5)].string), (yyvsp[(5) - (5)].string));
      ;}
    break;

  case 647:

/* Line 1455 of yacc.c  */
#line 3783 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 648:

/* Line 1455 of yacc.c  */
#line 3784 "def.y"
    { defData->dumb_mode = 0; defData->no_num = 0; ;}
    break;

  case 649:

/* Line 1455 of yacc.c  */
#line 3786 "def.y"
    {
         defData->dumb_mode = 1;
         defData->no_num = 1;
         if (defCallbacks->ScanchainCbk)
           defData->Scanchain.addOrderedList();
      ;}
    break;

  case 650:

/* Line 1455 of yacc.c  */
#line 3793 "def.y"
    { defData->dumb_mode = 0; defData->no_num = 0; ;}
    break;

  case 651:

/* Line 1455 of yacc.c  */
#line 3794 "def.y"
    {defData->dumb_mode = 2; defData->no_num = 2; ;}
    break;

  case 652:

/* Line 1455 of yacc.c  */
#line 3795 "def.y"
    { if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setStop((yyvsp[(4) - (5)].string), (yyvsp[(5) - (5)].string));
      ;}
    break;

  case 653:

/* Line 1455 of yacc.c  */
#line 3798 "def.y"
    { defData->dumb_mode = 10; defData->no_num = 10; ;}
    break;

  case 654:

/* Line 1455 of yacc.c  */
#line 3799 "def.y"
    { defData->dumb_mode = 0;  defData->no_num = 0; ;}
    break;

  case 655:

/* Line 1455 of yacc.c  */
#line 3800 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 656:

/* Line 1455 of yacc.c  */
#line 3802 "def.y"
    {
        if (defData->VersionNum < 5.5) {
          if (defCallbacks->ScanchainCbk) {
            if (defData->scanchainWarnings++ < defSettings->ScanchainWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The PARTITION statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6538, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        }
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setPartition((yyvsp[(4) - (5)].string), (yyvsp[(5) - (5)].integer));
      ;}
    break;

  case 657:

/* Line 1455 of yacc.c  */
#line 3819 "def.y"
    {
        if (defCallbacks->ScanChainExtCbk)
          CALLBACK(defCallbacks->ScanChainExtCbk, defrScanChainExtCbkType, &defData->History_text[0]);
      ;}
    break;

  case 658:

/* Line 1455 of yacc.c  */
#line 3825 "def.y"
    { ;}
    break;

  case 659:

/* Line 1455 of yacc.c  */
#line 3827 "def.y"
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[(2) - (4)].string), "IN") == 0 || strcmp((yyvsp[(2) - (4)].string), "in") == 0)
            defData->Scanchain.setCommonIn((yyvsp[(3) - (4)].string));
          else if (strcmp((yyvsp[(2) - (4)].string), "OUT") == 0 || strcmp((yyvsp[(2) - (4)].string), "out") == 0)
            defData->Scanchain.setCommonOut((yyvsp[(3) - (4)].string));
        }
      ;}
    break;

  case 660:

/* Line 1455 of yacc.c  */
#line 3836 "def.y"
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[(2) - (8)].string), "IN") == 0 || strcmp((yyvsp[(2) - (8)].string), "in") == 0)
            defData->Scanchain.setCommonIn((yyvsp[(3) - (8)].string));
          else if (strcmp((yyvsp[(2) - (8)].string), "OUT") == 0 || strcmp((yyvsp[(2) - (8)].string), "out") == 0)
            defData->Scanchain.setCommonOut((yyvsp[(3) - (8)].string));
          if (strcmp((yyvsp[(6) - (8)].string), "IN") == 0 || strcmp((yyvsp[(6) - (8)].string), "in") == 0)
            defData->Scanchain.setCommonIn((yyvsp[(7) - (8)].string));
          else if (strcmp((yyvsp[(6) - (8)].string), "OUT") == 0 || strcmp((yyvsp[(6) - (8)].string), "out") == 0)
            defData->Scanchain.setCommonOut((yyvsp[(7) - (8)].string));
        }
      ;}
    break;

  case 663:

/* Line 1455 of yacc.c  */
#line 3854 "def.y"
    {
        defData->dumb_mode = 1000;
        defData->no_num = 1000;
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.addFloatingInst((yyvsp[(1) - (1)].string));
      ;}
    break;

  case 664:

/* Line 1455 of yacc.c  */
#line 3861 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 665:

/* Line 1455 of yacc.c  */
#line 3864 "def.y"
    { ;}
    break;

  case 666:

/* Line 1455 of yacc.c  */
#line 3866 "def.y"
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[(2) - (4)].string), "IN") == 0 || strcmp((yyvsp[(2) - (4)].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[(3) - (4)].string));
          else if (strcmp((yyvsp[(2) - (4)].string), "OUT") == 0 || strcmp((yyvsp[(2) - (4)].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[(3) - (4)].string));
          else if (strcmp((yyvsp[(2) - (4)].string), "BITS") == 0 || strcmp((yyvsp[(2) - (4)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(3) - (4)].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      ;}
    break;

  case 667:

/* Line 1455 of yacc.c  */
#line 3879 "def.y"
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[(2) - (8)].string), "IN") == 0 || strcmp((yyvsp[(2) - (8)].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[(3) - (8)].string));
          else if (strcmp((yyvsp[(2) - (8)].string), "OUT") == 0 || strcmp((yyvsp[(2) - (8)].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[(3) - (8)].string));
          else if (strcmp((yyvsp[(2) - (8)].string), "BITS") == 0 || strcmp((yyvsp[(2) - (8)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(3) - (8)].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[(6) - (8)].string), "IN") == 0 || strcmp((yyvsp[(6) - (8)].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[(7) - (8)].string));
          else if (strcmp((yyvsp[(6) - (8)].string), "OUT") == 0 || strcmp((yyvsp[(6) - (8)].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[(7) - (8)].string));
          else if (strcmp((yyvsp[(6) - (8)].string), "BITS") == 0 || strcmp((yyvsp[(6) - (8)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(7) - (8)].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      ;}
    break;

  case 668:

/* Line 1455 of yacc.c  */
#line 3901 "def.y"
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[(2) - (12)].string), "IN") == 0 || strcmp((yyvsp[(2) - (12)].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[(3) - (12)].string));
          else if (strcmp((yyvsp[(2) - (12)].string), "OUT") == 0 || strcmp((yyvsp[(2) - (12)].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[(3) - (12)].string));
          else if (strcmp((yyvsp[(2) - (12)].string), "BITS") == 0 || strcmp((yyvsp[(2) - (12)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(3) - (12)].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[(6) - (12)].string), "IN") == 0 || strcmp((yyvsp[(6) - (12)].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[(7) - (12)].string));
          else if (strcmp((yyvsp[(6) - (12)].string), "OUT") == 0 || strcmp((yyvsp[(6) - (12)].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[(7) - (12)].string));
          else if (strcmp((yyvsp[(6) - (12)].string), "BITS") == 0 || strcmp((yyvsp[(6) - (12)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(7) - (12)].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[(10) - (12)].string), "IN") == 0 || strcmp((yyvsp[(10) - (12)].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[(11) - (12)].string));
          else if (strcmp((yyvsp[(10) - (12)].string), "OUT") == 0 || strcmp((yyvsp[(10) - (12)].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[(11) - (12)].string));
          else if (strcmp((yyvsp[(10) - (12)].string), "BITS") == 0 || strcmp((yyvsp[(10) - (12)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(11) - (12)].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      ;}
    break;

  case 671:

/* Line 1455 of yacc.c  */
#line 3935 "def.y"
    { defData->dumb_mode = 1000; defData->no_num = 1000; 
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.addOrderedInst((yyvsp[(1) - (1)].string));
      ;}
    break;

  case 672:

/* Line 1455 of yacc.c  */
#line 3940 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 673:

/* Line 1455 of yacc.c  */
#line 3943 "def.y"
    { ;}
    break;

  case 674:

/* Line 1455 of yacc.c  */
#line 3945 "def.y"
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[(2) - (4)].string), "IN") == 0 || strcmp((yyvsp[(2) - (4)].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[(3) - (4)].string));
          else if (strcmp((yyvsp[(2) - (4)].string), "OUT") == 0 || strcmp((yyvsp[(2) - (4)].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[(3) - (4)].string));
          else if (strcmp((yyvsp[(2) - (4)].string), "BITS") == 0 || strcmp((yyvsp[(2) - (4)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(3) - (4)].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
         }
        }
      ;}
    break;

  case 675:

/* Line 1455 of yacc.c  */
#line 3958 "def.y"
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[(2) - (8)].string), "IN") == 0 || strcmp((yyvsp[(2) - (8)].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[(3) - (8)].string));
          else if (strcmp((yyvsp[(2) - (8)].string), "OUT") == 0 || strcmp((yyvsp[(2) - (8)].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[(3) - (8)].string));
          else if (strcmp((yyvsp[(2) - (8)].string), "BITS") == 0 || strcmp((yyvsp[(2) - (8)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(3) - (8)].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[(6) - (8)].string), "IN") == 0 || strcmp((yyvsp[(6) - (8)].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[(7) - (8)].string));
          else if (strcmp((yyvsp[(6) - (8)].string), "OUT") == 0 || strcmp((yyvsp[(6) - (8)].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[(7) - (8)].string));
          else if (strcmp((yyvsp[(6) - (8)].string), "BITS") == 0 || strcmp((yyvsp[(6) - (8)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(7) - (8)].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
        }
      ;}
    break;

  case 676:

/* Line 1455 of yacc.c  */
#line 3980 "def.y"
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[(2) - (12)].string), "IN") == 0 || strcmp((yyvsp[(2) - (12)].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[(3) - (12)].string));
          else if (strcmp((yyvsp[(2) - (12)].string), "OUT") == 0 || strcmp((yyvsp[(2) - (12)].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[(3) - (12)].string));
          else if (strcmp((yyvsp[(2) - (12)].string), "BITS") == 0 || strcmp((yyvsp[(2) - (12)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(3) - (12)].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[(6) - (12)].string), "IN") == 0 || strcmp((yyvsp[(6) - (12)].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[(7) - (12)].string));
          else if (strcmp((yyvsp[(6) - (12)].string), "OUT") == 0 || strcmp((yyvsp[(6) - (12)].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[(7) - (12)].string));
          else if (strcmp((yyvsp[(6) - (12)].string), "BITS") == 0 || strcmp((yyvsp[(6) - (12)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(7) - (12)].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[(10) - (12)].string), "IN") == 0 || strcmp((yyvsp[(10) - (12)].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[(11) - (12)].string));
          else if (strcmp((yyvsp[(10) - (12)].string), "OUT") == 0 || strcmp((yyvsp[(10) - (12)].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[(11) - (12)].string));
          else if (strcmp((yyvsp[(10) - (12)].string), "BITS") == 0 || strcmp((yyvsp[(10) - (12)].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[(11) - (12)].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
        }
      ;}
    break;

  case 677:

/* Line 1455 of yacc.c  */
#line 4010 "def.y"
    { (yyval.integer) = -1; ;}
    break;

  case 678:

/* Line 1455 of yacc.c  */
#line 4012 "def.y"
    { (yyval.integer) = ROUND((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 679:

/* Line 1455 of yacc.c  */
#line 4015 "def.y"
    { 
        if (defCallbacks->ScanchainsEndCbk)
          CALLBACK(defCallbacks->ScanchainsEndCbk, defrScanchainsEndCbkType, 0);
        defData->bit_is_keyword = FALSE;
        defData->dumb_mode = 0; defData->no_num = 0;
      ;}
    break;

  case 681:

/* Line 1455 of yacc.c  */
#line 4027 "def.y"
    {
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingsStartCbk) {
          CALLBACK(defCallbacks->IOTimingsStartCbk, defrIOTimingsStartCbkType, ROUND((yyvsp[(2) - (3)].dval)));
        } else {
          if (defCallbacks->IOTimingsStartCbk)
            if (defData->iOTimingWarnings++ < defSettings->IOTimingWarnings)
              defWarning(7035, "The IOTIMINGS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
      ;}
    break;

  case 683:

/* Line 1455 of yacc.c  */
#line 4039 "def.y"
    { ;}
    break;

  case 684:

/* Line 1455 of yacc.c  */
#line 4042 "def.y"
    { 
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingCbk)
          CALLBACK(defCallbacks->IOTimingCbk, defrIOTimingCbkType, &defData->IOTiming);
      ;}
    break;

  case 685:

/* Line 1455 of yacc.c  */
#line 4047 "def.y"
    {defData->dumb_mode = 2; defData->no_num = 2; ;}
    break;

  case 686:

/* Line 1455 of yacc.c  */
#line 4048 "def.y"
    {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setName((yyvsp[(4) - (6)].string), (yyvsp[(5) - (6)].string));
      ;}
    break;

  case 689:

/* Line 1455 of yacc.c  */
#line 4059 "def.y"
    {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setVariable((yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].dval), (yyvsp[(5) - (5)].dval));
      ;}
    break;

  case 690:

/* Line 1455 of yacc.c  */
#line 4064 "def.y"
    {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setSlewRate((yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].dval), (yyvsp[(5) - (5)].dval));
      ;}
    break;

  case 691:

/* Line 1455 of yacc.c  */
#line 4069 "def.y"
    {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setCapacitance((yyvsp[(3) - (3)].dval));
      ;}
    break;

  case 692:

/* Line 1455 of yacc.c  */
#line 4073 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 693:

/* Line 1455 of yacc.c  */
#line 4074 "def.y"
    {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setDriveCell((yyvsp[(4) - (4)].string));
      ;}
    break;

  case 695:

/* Line 1455 of yacc.c  */
#line 4083 "def.y"
    {
        if (defData->VersionNum < 5.4 && defCallbacks->IoTimingsExtCbk)
          CALLBACK(defCallbacks->IoTimingsExtCbk, defrIoTimingsExtCbkType, &defData->History_text[0]);
      ;}
    break;

  case 696:

/* Line 1455 of yacc.c  */
#line 4089 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 697:

/* Line 1455 of yacc.c  */
#line 4090 "def.y"
    {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setTo((yyvsp[(4) - (4)].string));
      ;}
    break;

  case 700:

/* Line 1455 of yacc.c  */
#line 4097 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 701:

/* Line 1455 of yacc.c  */
#line 4098 "def.y"
    {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setFrom((yyvsp[(3) - (3)].string));
      ;}
    break;

  case 703:

/* Line 1455 of yacc.c  */
#line 4105 "def.y"
    {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setParallel((yyvsp[(2) - (2)].dval));
      ;}
    break;

  case 704:

/* Line 1455 of yacc.c  */
#line 4110 "def.y"
    { (yyval.string) = (char*)"RISE"; ;}
    break;

  case 705:

/* Line 1455 of yacc.c  */
#line 4110 "def.y"
    { (yyval.string) = (char*)"FALL"; ;}
    break;

  case 706:

/* Line 1455 of yacc.c  */
#line 4113 "def.y"
    {
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingsEndCbk)
          CALLBACK(defCallbacks->IOTimingsEndCbk, defrIOTimingsEndCbkType, 0);
      ;}
    break;

  case 707:

/* Line 1455 of yacc.c  */
#line 4119 "def.y"
    { 
        if (defCallbacks->FPCEndCbk)
          CALLBACK(defCallbacks->FPCEndCbk, defrFPCEndCbkType, 0);
      ;}
    break;

  case 708:

/* Line 1455 of yacc.c  */
#line 4125 "def.y"
    {
        if (defCallbacks->FPCStartCbk)
          CALLBACK(defCallbacks->FPCStartCbk, defrFPCStartCbkType, ROUND((yyvsp[(2) - (3)].dval)));
      ;}
    break;

  case 710:

/* Line 1455 of yacc.c  */
#line 4132 "def.y"
    {;}
    break;

  case 711:

/* Line 1455 of yacc.c  */
#line 4134 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1;  ;}
    break;

  case 712:

/* Line 1455 of yacc.c  */
#line 4135 "def.y"
    { if (defCallbacks->FPCCbk) defData->FPC.setName((yyvsp[(3) - (4)].string), (yyvsp[(4) - (4)].string)); ;}
    break;

  case 713:

/* Line 1455 of yacc.c  */
#line 4137 "def.y"
    { if (defCallbacks->FPCCbk) CALLBACK(defCallbacks->FPCCbk, defrFPCCbkType, &defData->FPC); ;}
    break;

  case 714:

/* Line 1455 of yacc.c  */
#line 4140 "def.y"
    { (yyval.string) = (char*)"HORIZONTAL"; ;}
    break;

  case 715:

/* Line 1455 of yacc.c  */
#line 4142 "def.y"
    { (yyval.string) = (char*)"VERTICAL"; ;}
    break;

  case 716:

/* Line 1455 of yacc.c  */
#line 4145 "def.y"
    { if (defCallbacks->FPCCbk) defData->FPC.setAlign(); ;}
    break;

  case 717:

/* Line 1455 of yacc.c  */
#line 4147 "def.y"
    { if (defCallbacks->FPCCbk) defData->FPC.setMax((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 718:

/* Line 1455 of yacc.c  */
#line 4149 "def.y"
    { if (defCallbacks->FPCCbk) defData->FPC.setMin((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 719:

/* Line 1455 of yacc.c  */
#line 4151 "def.y"
    { if (defCallbacks->FPCCbk) defData->FPC.setEqual((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 722:

/* Line 1455 of yacc.c  */
#line 4158 "def.y"
    { if (defCallbacks->FPCCbk) defData->FPC.setDoingBottomLeft(); ;}
    break;

  case 724:

/* Line 1455 of yacc.c  */
#line 4161 "def.y"
    { if (defCallbacks->FPCCbk) defData->FPC.setDoingTopRight(); ;}
    break;

  case 728:

/* Line 1455 of yacc.c  */
#line 4168 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 729:

/* Line 1455 of yacc.c  */
#line 4169 "def.y"
    { if (defCallbacks->FPCCbk) defData->FPC.addRow((yyvsp[(4) - (5)].string)); ;}
    break;

  case 730:

/* Line 1455 of yacc.c  */
#line 4170 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 731:

/* Line 1455 of yacc.c  */
#line 4171 "def.y"
    { if (defCallbacks->FPCCbk) defData->FPC.addComps((yyvsp[(4) - (5)].string)); ;}
    break;

  case 733:

/* Line 1455 of yacc.c  */
#line 4178 "def.y"
    { 
        if (defCallbacks->TimingDisablesStartCbk)
          CALLBACK(defCallbacks->TimingDisablesStartCbk, defrTimingDisablesStartCbkType,
                   ROUND((yyvsp[(2) - (3)].dval)));
      ;}
    break;

  case 735:

/* Line 1455 of yacc.c  */
#line 4186 "def.y"
    {;}
    break;

  case 736:

/* Line 1455 of yacc.c  */
#line 4188 "def.y"
    { defData->dumb_mode = 2; defData->no_num = 2;  ;}
    break;

  case 737:

/* Line 1455 of yacc.c  */
#line 4189 "def.y"
    { defData->dumb_mode = 2; defData->no_num = 2;  ;}
    break;

  case 738:

/* Line 1455 of yacc.c  */
#line 4190 "def.y"
    {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setFromTo((yyvsp[(4) - (10)].string), (yyvsp[(5) - (10)].string), (yyvsp[(8) - (10)].string), (yyvsp[(9) - (10)].string));
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                &defData->TimingDisable);
        }
      ;}
    break;

  case 739:

/* Line 1455 of yacc.c  */
#line 4197 "def.y"
    {defData->dumb_mode = 2; defData->no_num = 2; ;}
    break;

  case 740:

/* Line 1455 of yacc.c  */
#line 4198 "def.y"
    {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setThru((yyvsp[(4) - (6)].string), (yyvsp[(5) - (6)].string));
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                   &defData->TimingDisable);
        }
      ;}
    break;

  case 741:

/* Line 1455 of yacc.c  */
#line 4205 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1;;}
    break;

  case 742:

/* Line 1455 of yacc.c  */
#line 4206 "def.y"
    {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setMacro((yyvsp[(4) - (6)].string));
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                &defData->TimingDisable);
        }
      ;}
    break;

  case 743:

/* Line 1455 of yacc.c  */
#line 4214 "def.y"
    { if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setReentrantPathsFlag();
      ;}
    break;

  case 744:

/* Line 1455 of yacc.c  */
#line 4219 "def.y"
    {defData->dumb_mode = 1; defData->no_num = 1;;}
    break;

  case 745:

/* Line 1455 of yacc.c  */
#line 4220 "def.y"
    {defData->dumb_mode=1; defData->no_num = 1;;}
    break;

  case 746:

/* Line 1455 of yacc.c  */
#line 4221 "def.y"
    {
        if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setMacroFromTo((yyvsp[(3) - (6)].string),(yyvsp[(6) - (6)].string));
      ;}
    break;

  case 747:

/* Line 1455 of yacc.c  */
#line 4225 "def.y"
    {defData->dumb_mode=1; defData->no_num = 1;;}
    break;

  case 748:

/* Line 1455 of yacc.c  */
#line 4226 "def.y"
    {
        if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setMacroThru((yyvsp[(3) - (3)].string));
      ;}
    break;

  case 749:

/* Line 1455 of yacc.c  */
#line 4232 "def.y"
    { 
        if (defCallbacks->TimingDisablesEndCbk)
          CALLBACK(defCallbacks->TimingDisablesEndCbk, defrTimingDisablesEndCbkType, 0);
      ;}
    break;

  case 751:

/* Line 1455 of yacc.c  */
#line 4242 "def.y"
    {
        if (defCallbacks->PartitionsStartCbk)
          CALLBACK(defCallbacks->PartitionsStartCbk, defrPartitionsStartCbkType,
                   ROUND((yyvsp[(2) - (3)].dval)));
      ;}
    break;

  case 753:

/* Line 1455 of yacc.c  */
#line 4250 "def.y"
    { ;}
    break;

  case 754:

/* Line 1455 of yacc.c  */
#line 4253 "def.y"
    { 
        if (defCallbacks->PartitionCbk)
          CALLBACK(defCallbacks->PartitionCbk, defrPartitionCbkType, &defData->Partition);
      ;}
    break;

  case 755:

/* Line 1455 of yacc.c  */
#line 4258 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 756:

/* Line 1455 of yacc.c  */
#line 4259 "def.y"
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setName((yyvsp[(3) - (4)].string));
      ;}
    break;

  case 758:

/* Line 1455 of yacc.c  */
#line 4266 "def.y"
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.addTurnOff((yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].string));
      ;}
    break;

  case 759:

/* Line 1455 of yacc.c  */
#line 4272 "def.y"
    { (yyval.string) = (char*)" "; ;}
    break;

  case 760:

/* Line 1455 of yacc.c  */
#line 4274 "def.y"
    { (yyval.string) = (char*)"R"; ;}
    break;

  case 761:

/* Line 1455 of yacc.c  */
#line 4276 "def.y"
    { (yyval.string) = (char*)"F"; ;}
    break;

  case 762:

/* Line 1455 of yacc.c  */
#line 4279 "def.y"
    { (yyval.string) = (char*)" "; ;}
    break;

  case 763:

/* Line 1455 of yacc.c  */
#line 4281 "def.y"
    { (yyval.string) = (char*)"R"; ;}
    break;

  case 764:

/* Line 1455 of yacc.c  */
#line 4283 "def.y"
    { (yyval.string) = (char*)"F"; ;}
    break;

  case 767:

/* Line 1455 of yacc.c  */
#line 4289 "def.y"
    {defData->dumb_mode=2; defData->no_num = 2;;}
    break;

  case 768:

/* Line 1455 of yacc.c  */
#line 4291 "def.y"
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromClockPin((yyvsp[(4) - (7)].string), (yyvsp[(5) - (7)].string));
      ;}
    break;

  case 769:

/* Line 1455 of yacc.c  */
#line 4295 "def.y"
    {defData->dumb_mode=2; defData->no_num = 2; ;}
    break;

  case 770:

/* Line 1455 of yacc.c  */
#line 4297 "def.y"
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromCompPin((yyvsp[(4) - (6)].string), (yyvsp[(5) - (6)].string));
      ;}
    break;

  case 771:

/* Line 1455 of yacc.c  */
#line 4301 "def.y"
    {defData->dumb_mode=1; defData->no_num = 1; ;}
    break;

  case 772:

/* Line 1455 of yacc.c  */
#line 4303 "def.y"
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromIOPin((yyvsp[(4) - (5)].string));
      ;}
    break;

  case 773:

/* Line 1455 of yacc.c  */
#line 4307 "def.y"
    {defData->dumb_mode=2; defData->no_num = 2; ;}
    break;

  case 774:

/* Line 1455 of yacc.c  */
#line 4309 "def.y"
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToClockPin((yyvsp[(4) - (7)].string), (yyvsp[(5) - (7)].string));
      ;}
    break;

  case 775:

/* Line 1455 of yacc.c  */
#line 4313 "def.y"
    {defData->dumb_mode=2; defData->no_num = 2; ;}
    break;

  case 776:

/* Line 1455 of yacc.c  */
#line 4315 "def.y"
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToCompPin((yyvsp[(4) - (6)].string), (yyvsp[(5) - (6)].string));
      ;}
    break;

  case 777:

/* Line 1455 of yacc.c  */
#line 4319 "def.y"
    {defData->dumb_mode=1; defData->no_num = 2; ;}
    break;

  case 778:

/* Line 1455 of yacc.c  */
#line 4320 "def.y"
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToIOPin((yyvsp[(4) - (5)].string));
      ;}
    break;

  case 779:

/* Line 1455 of yacc.c  */
#line 4325 "def.y"
    { 
        if (defCallbacks->PartitionsExtCbk)
          CALLBACK(defCallbacks->PartitionsExtCbk, defrPartitionsExtCbkType,
                   &defData->History_text[0]);
      ;}
    break;

  case 780:

/* Line 1455 of yacc.c  */
#line 4332 "def.y"
    { defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; ;}
    break;

  case 781:

/* Line 1455 of yacc.c  */
#line 4333 "def.y"
    { defData->dumb_mode = 0; defData->no_num = 0; ;}
    break;

  case 783:

/* Line 1455 of yacc.c  */
#line 4337 "def.y"
    { ;}
    break;

  case 784:

/* Line 1455 of yacc.c  */
#line 4340 "def.y"
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setMin((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].dval));
      ;}
    break;

  case 785:

/* Line 1455 of yacc.c  */
#line 4345 "def.y"
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setMax((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].dval));
      ;}
    break;

  case 787:

/* Line 1455 of yacc.c  */
#line 4352 "def.y"
    { if (defCallbacks->PartitionCbk) defData->Partition.addPin((yyvsp[(2) - (2)].string)); ;}
    break;

  case 790:

/* Line 1455 of yacc.c  */
#line 4358 "def.y"
    { if (defCallbacks->PartitionCbk) defData->Partition.addRiseMin((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 791:

/* Line 1455 of yacc.c  */
#line 4360 "def.y"
    { if (defCallbacks->PartitionCbk) defData->Partition.addFallMin((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 792:

/* Line 1455 of yacc.c  */
#line 4362 "def.y"
    { if (defCallbacks->PartitionCbk) defData->Partition.addRiseMax((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 793:

/* Line 1455 of yacc.c  */
#line 4364 "def.y"
    { if (defCallbacks->PartitionCbk) defData->Partition.addFallMax((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 796:

/* Line 1455 of yacc.c  */
#line 4372 "def.y"
    { if (defCallbacks->PartitionCbk)
          defData->Partition.addRiseMinRange((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].dval)); ;}
    break;

  case 797:

/* Line 1455 of yacc.c  */
#line 4375 "def.y"
    { if (defCallbacks->PartitionCbk)
          defData->Partition.addFallMinRange((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].dval)); ;}
    break;

  case 798:

/* Line 1455 of yacc.c  */
#line 4378 "def.y"
    { if (defCallbacks->PartitionCbk)
          defData->Partition.addRiseMaxRange((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].dval)); ;}
    break;

  case 799:

/* Line 1455 of yacc.c  */
#line 4381 "def.y"
    { if (defCallbacks->PartitionCbk)
          defData->Partition.addFallMaxRange((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].dval)); ;}
    break;

  case 800:

/* Line 1455 of yacc.c  */
#line 4385 "def.y"
    { if (defCallbacks->PartitionsEndCbk)
          CALLBACK(defCallbacks->PartitionsEndCbk, defrPartitionsEndCbkType, 0); ;}
    break;

  case 802:

/* Line 1455 of yacc.c  */
#line 4390 "def.y"
    { ;}
    break;

  case 803:

/* Line 1455 of yacc.c  */
#line 4392 "def.y"
    {defData->dumb_mode=2; defData->no_num = 2; ;}
    break;

  case 804:

/* Line 1455 of yacc.c  */
#line 4394 "def.y"
    {
        // note that the defData->first T_STRING could be the keyword VPIN 
        if (defCallbacks->NetCbk)
          defData->Subnet->addPin((yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string), (yyvsp[(5) - (6)].integer));
      ;}
    break;

  case 805:

/* Line 1455 of yacc.c  */
#line 4401 "def.y"
    { (yyval.integer) = 0; ;}
    break;

  case 806:

/* Line 1455 of yacc.c  */
#line 4403 "def.y"
    { (yyval.integer) = 1; ;}
    break;

  case 809:

/* Line 1455 of yacc.c  */
#line 4409 "def.y"
    {  
        if (defCallbacks->NetCbk) defData->Subnet->addWire((yyvsp[(1) - (1)].string));
      ;}
    break;

  case 810:

/* Line 1455 of yacc.c  */
#line 4413 "def.y"
    {  
        defData->by_is_keyword = FALSE;
        defData->do_is_keyword = FALSE;
        defData->new_is_keyword = FALSE;
        defData->step_is_keyword = FALSE;
        defData->orient_is_keyword = FALSE;
        defData->needNPCbk = 0;
      ;}
    break;

  case 811:

/* Line 1455 of yacc.c  */
#line 4421 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 812:

/* Line 1455 of yacc.c  */
#line 4422 "def.y"
    { if (defCallbacks->NetCbk) defData->Subnet->setNonDefault((yyvsp[(3) - (3)].string)); ;}
    break;

  case 813:

/* Line 1455 of yacc.c  */
#line 4425 "def.y"
    { (yyval.string) = (char*)"FIXED"; defData->dumb_mode = 1; ;}
    break;

  case 814:

/* Line 1455 of yacc.c  */
#line 4427 "def.y"
    { (yyval.string) = (char*)"COVER"; defData->dumb_mode = 1; ;}
    break;

  case 815:

/* Line 1455 of yacc.c  */
#line 4429 "def.y"
    { (yyval.string) = (char*)"ROUTED"; defData->dumb_mode = 1; ;}
    break;

  case 816:

/* Line 1455 of yacc.c  */
#line 4431 "def.y"
    { (yyval.string) = (char*)"NOSHIELD"; defData->dumb_mode = 1; ;}
    break;

  case 818:

/* Line 1455 of yacc.c  */
#line 4436 "def.y"
    { if (defCallbacks->PinPropStartCbk)
          CALLBACK(defCallbacks->PinPropStartCbk, defrPinPropStartCbkType, ROUND((yyvsp[(2) - (3)].dval))); ;}
    break;

  case 819:

/* Line 1455 of yacc.c  */
#line 4441 "def.y"
    { ;}
    break;

  case 820:

/* Line 1455 of yacc.c  */
#line 4443 "def.y"
    { ;}
    break;

  case 821:

/* Line 1455 of yacc.c  */
#line 4446 "def.y"
    { if (defCallbacks->PinPropEndCbk)
          CALLBACK(defCallbacks->PinPropEndCbk, defrPinPropEndCbkType, 0); ;}
    break;

  case 824:

/* Line 1455 of yacc.c  */
#line 4453 "def.y"
    { defData->dumb_mode = 2; defData->no_num = 2; ;}
    break;

  case 825:

/* Line 1455 of yacc.c  */
#line 4454 "def.y"
    { if (defCallbacks->PinPropCbk) defData->PinProp.setName((yyvsp[(3) - (4)].string), (yyvsp[(4) - (4)].string)); ;}
    break;

  case 826:

/* Line 1455 of yacc.c  */
#line 4456 "def.y"
    { if (defCallbacks->PinPropCbk) {
          CALLBACK(defCallbacks->PinPropCbk, defrPinPropCbkType, &defData->PinProp);
         // reset the property number
         defData->PinProp.clear();
        }
      ;}
    break;

  case 829:

/* Line 1455 of yacc.c  */
#line 4466 "def.y"
    { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; ;}
    break;

  case 830:

/* Line 1455 of yacc.c  */
#line 4468 "def.y"
    { defData->dumb_mode = 0; defData->parsing_property = 0; ;}
    break;

  case 833:

/* Line 1455 of yacc.c  */
#line 4475 "def.y"
    {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defData->CompPinProp.propType((yyvsp[(1) - (2)].string));
          CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "PINPROPERTIES");
          sprintf(str, "%g", (yyvsp[(2) - (2)].dval));
          defData->PinProp.addNumProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), str, propTp);
        }
      ;}
    break;

  case 834:

/* Line 1455 of yacc.c  */
#line 4486 "def.y"
    {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          propTp = defData->CompPinProp.propType((yyvsp[(1) - (2)].string));
          CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "PINPROPERTIES");
          defData->PinProp.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
        }
      ;}
    break;

  case 835:

/* Line 1455 of yacc.c  */
#line 4495 "def.y"
    {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          propTp = defData->CompPinProp.propType((yyvsp[(1) - (2)].string));
          CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "PINPROPERTIES");
          defData->PinProp.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
        }
      ;}
    break;

  case 837:

/* Line 1455 of yacc.c  */
#line 4507 "def.y"
    { if (defCallbacks->BlockageStartCbk)
          CALLBACK(defCallbacks->BlockageStartCbk, defrBlockageStartCbkType, ROUND((yyvsp[(2) - (3)].dval))); ;}
    break;

  case 838:

/* Line 1455 of yacc.c  */
#line 4511 "def.y"
    { if (defCallbacks->BlockageEndCbk)
          CALLBACK(defCallbacks->BlockageEndCbk, defrBlockageEndCbkType, 0); ;}
    break;

  case 841:

/* Line 1455 of yacc.c  */
#line 4520 "def.y"
    {
        if (defCallbacks->BlockageCbk) {
          CALLBACK(defCallbacks->BlockageCbk, defrBlockageCbkType, &defData->Blockage);
          defData->Blockage.clear();
        }
      ;}
    break;

  case 842:

/* Line 1455 of yacc.c  */
#line 4527 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 843:

/* Line 1455 of yacc.c  */
#line 4528 "def.y"
    {
        if (defCallbacks->BlockageCbk) {
          if (defData->Blockage.hasPlacement() != 0) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6539, "Invalid BLOCKAGE statement defined in the DEF file. The BLOCKAGE statment has both the LAYER and the PLACEMENT statements defined.\nUpdate your DEF file to have either BLOCKAGE or PLACEMENT statement only.");
              CHKERR();
            }
          }
          defData->Blockage.setLayer((yyvsp[(4) - (4)].string));
          defData->Blockage.clearPoly(); // free poly, if any
        }
        defData->hasBlkLayerComp = 0;
        defData->hasBlkLayerSpac = 0;
        defData->hasBlkLayerTypeComp = 0;
      ;}
    break;

  case 845:

/* Line 1455 of yacc.c  */
#line 4547 "def.y"
    {
        if (defCallbacks->BlockageCbk) {
          if (defData->Blockage.hasLayer() != 0) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6539, "Invalid BLOCKAGE statement defined in the DEF file. The BLOCKAGE statment has both the LAYER and the PLACEMENT statements defined.\nUpdate your DEF file to have either BLOCKAGE or PLACEMENT statement only.");
              CHKERR();
            }
          }
          defData->Blockage.setPlacement();
          defData->Blockage.clearPoly(); // free poly, if any
        }
        defData->hasBlkPlaceComp = 0;
        defData->hasBlkPlaceTypeComp = 0;
      ;}
    break;

  case 849:

/* Line 1455 of yacc.c  */
#line 4568 "def.y"
    {
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The SPACING statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6540, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else if (defData->hasBlkLayerSpac) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk)
            defData->Blockage.setSpacing((int)(yyvsp[(3) - (3)].dval));
          defData->hasBlkLayerSpac = 1;
        }
      ;}
    break;

  case 850:

/* Line 1455 of yacc.c  */
#line 4594 "def.y"
    {
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else if (defData->hasBlkLayerSpac) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk)
            defData->Blockage.setDesignRuleWidth((int)(yyvsp[(3) - (3)].dval));
          defData->hasBlkLayerSpac = 1;
        }
      ;}
    break;

  case 853:

/* Line 1455 of yacc.c  */
#line 4620 "def.y"
    {      
        if (validateMaskInput((int)(yyvsp[(3) - (3)].dval), defData->blockageWarnings, defSettings->BlockageWarnings)) {
          defData->Blockage.setMask((int)(yyvsp[(3) - (3)].dval));
        }
      ;}
    break;

  case 854:

/* Line 1455 of yacc.c  */
#line 4628 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 855:

/* Line 1455 of yacc.c  */
#line 4629 "def.y"
    {
        if (defData->hasBlkLayerComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk) {
            defData->Blockage.setComponent((yyvsp[(4) - (4)].string));
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
        }
      ;}
    break;

  case 856:

/* Line 1455 of yacc.c  */
#line 4648 "def.y"
    {
        if (defData->hasBlkLayerComp || defData->hasBlkLayerTypeComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk) {
            defData->Blockage.setSlots();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
          if (defData->VersionNum == 5.8) {
            defData->hasBlkLayerTypeComp = 1;
          }
        }
      ;}
    break;

  case 857:

/* Line 1455 of yacc.c  */
#line 4669 "def.y"
    {
        if (defData->hasBlkLayerComp || defData->hasBlkLayerTypeComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setFills();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
          if (defData->VersionNum == 5.8) {
            defData->hasBlkLayerTypeComp = 1;
          }
        }
      ;}
    break;

  case 858:

/* Line 1455 of yacc.c  */
#line 4690 "def.y"
    {
        if (defData->hasBlkLayerComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setPushdown();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
        }
      ;}
    break;

  case 859:

/* Line 1455 of yacc.c  */
#line 4708 "def.y"
    {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->BlockageCbk) {
             if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The EXCEPTPGNET is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6549, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
              }
           }
        } else {
           if (defData->hasBlkLayerComp) {
             if (defCallbacks->BlockageCbk) {
               if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                 defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
                 CHKERR();
               }
             }
           } else {
             if (defCallbacks->BlockageCbk){
               defData->Blockage.setExceptpgnet();
             }
             if (defData->VersionNum < 5.8){
               defData->hasBlkLayerComp = 1;
             }
           }
        }
      ;}
    break;

  case 862:

/* Line 1455 of yacc.c  */
#line 4745 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 863:

/* Line 1455 of yacc.c  */
#line 4746 "def.y"
    {
        if (defData->hasBlkPlaceComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setComponent((yyvsp[(4) - (4)].string));
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkPlaceComp = 1;
          }
        }
      ;}
    break;

  case 864:

/* Line 1455 of yacc.c  */
#line 4764 "def.y"
    {
        if (defData->hasBlkPlaceComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setPushdown();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkPlaceComp = 1;
          }
        }
      ;}
    break;

  case 865:

/* Line 1455 of yacc.c  */
#line 4782 "def.y"
    {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->BlockageCbk) {
             if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The PLACEMENT SOFT is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6547, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else {
           if (defData->hasBlkPlaceComp || defData->hasBlkPlaceTypeComp) {
             if (defCallbacks->BlockageCbk) {
               if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                 defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
                 CHKERR();
               }
             }
           } else {
             if (defCallbacks->BlockageCbk){
               defData->Blockage.setSoft();
             }
             if (defData->VersionNum < 5.8) {
               defData->hasBlkPlaceComp = 1;
             }
             if (defData->VersionNum == 5.8) {
               defData->hasBlkPlaceTypeComp = 1;
             }
           }
        }
      ;}
    break;

  case 866:

/* Line 1455 of yacc.c  */
#line 4816 "def.y"
    {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->BlockageCbk) {
             if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                defData->defMsg = (char*)defMalloc(10000);
                sprintf (defData->defMsg,
                  "The PARTIAL is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                defError(6548, defData->defMsg);
                defFree(defData->defMsg);
                CHKERR();
             }
           }
        } else {
           if (defData->hasBlkPlaceComp || defData->hasBlkPlaceTypeComp) {
             if (defCallbacks->BlockageCbk) {
               if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                 defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
                 CHKERR();
               }
             }
           } else {
             if (defCallbacks->BlockageCbk){
               defData->Blockage.setPartial((yyvsp[(3) - (3)].dval));
             } 
             if (defData->VersionNum < 5.8) {
               defData->hasBlkPlaceComp = 1;
             }
             if (defData->VersionNum == 5.8) {
               defData->hasBlkPlaceTypeComp = 1;
             }
           }
         }
      ;}
    break;

  case 869:

/* Line 1455 of yacc.c  */
#line 4856 "def.y"
    {
        if (defCallbacks->BlockageCbk)
          defData->Blockage.addRect((yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, (yyvsp[(3) - (3)].pt).x, (yyvsp[(3) - (3)].pt).y);
      ;}
    break;

  case 870:

/* Line 1455 of yacc.c  */
#line 4861 "def.y"
    {
        if (defCallbacks->BlockageCbk) {
            defData->Geometries.Reset();
        }
      ;}
    break;

  case 871:

/* Line 1455 of yacc.c  */
#line 4867 "def.y"
    {
        if (defCallbacks->BlockageCbk) {
          if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond
            if (defData->Blockage.hasLayer()) {  // only in layer
              if (defCallbacks->BlockageCbk)
                defData->Blockage.addPolygon(&defData->Geometries);
            } else {
              if (defCallbacks->BlockageCbk) {
                if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                  defError(6544, "A POLYGON statement is defined in the BLOCKAGE statement,\nbut it is not defined in the BLOCKAGE LAYER statement.\nUpdate your DEF file to either remove the POLYGON statement from the BLOCKAGE statement or\ndefine the POLYGON statement in a BLOCKAGE LAYER statement.");
                  CHKERR();
                }
              }
            }
          }
        }
      ;}
    break;

  case 873:

/* Line 1455 of yacc.c  */
#line 4889 "def.y"
    { if (defCallbacks->SlotStartCbk)
          CALLBACK(defCallbacks->SlotStartCbk, defrSlotStartCbkType, ROUND((yyvsp[(2) - (3)].dval))); ;}
    break;

  case 874:

/* Line 1455 of yacc.c  */
#line 4893 "def.y"
    { if (defCallbacks->SlotEndCbk)
          CALLBACK(defCallbacks->SlotEndCbk, defrSlotEndCbkType, 0); ;}
    break;

  case 877:

/* Line 1455 of yacc.c  */
#line 4901 "def.y"
    {
        if (defCallbacks->SlotCbk) {
          CALLBACK(defCallbacks->SlotCbk, defrSlotCbkType, &defData->Slot);
          defData->Slot.clear();
        }
      ;}
    break;

  case 878:

/* Line 1455 of yacc.c  */
#line 4908 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 879:

/* Line 1455 of yacc.c  */
#line 4909 "def.y"
    {
        if (defCallbacks->SlotCbk) {
          defData->Slot.setLayer((yyvsp[(4) - (4)].string));
          defData->Slot.clearPoly();     // free poly, if any
        }
      ;}
    break;

  case 883:

/* Line 1455 of yacc.c  */
#line 4921 "def.y"
    {
        if (defCallbacks->SlotCbk)
          defData->Slot.addRect((yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, (yyvsp[(3) - (3)].pt).x, (yyvsp[(3) - (3)].pt).y);
      ;}
    break;

  case 884:

/* Line 1455 of yacc.c  */
#line 4926 "def.y"
    {
          defData->Geometries.Reset();
      ;}
    break;

  case 885:

/* Line 1455 of yacc.c  */
#line 4930 "def.y"
    {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond
          if (defCallbacks->SlotCbk)
            defData->Slot.addPolygon(&defData->Geometries);
        }
      ;}
    break;

  case 887:

/* Line 1455 of yacc.c  */
#line 4941 "def.y"
    { if (defCallbacks->FillStartCbk)
          CALLBACK(defCallbacks->FillStartCbk, defrFillStartCbkType, ROUND((yyvsp[(2) - (3)].dval))); ;}
    break;

  case 888:

/* Line 1455 of yacc.c  */
#line 4945 "def.y"
    { if (defCallbacks->FillEndCbk)
          CALLBACK(defCallbacks->FillEndCbk, defrFillEndCbkType, 0); ;}
    break;

  case 891:

/* Line 1455 of yacc.c  */
#line 4953 "def.y"
    {
        if (defCallbacks->FillCbk) {
          CALLBACK(defCallbacks->FillCbk, defrFillCbkType, &defData->Fill);
          defData->Fill.clear();
        }
      ;}
    break;

  case 892:

/* Line 1455 of yacc.c  */
#line 4959 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 893:

/* Line 1455 of yacc.c  */
#line 4960 "def.y"
    {
        if (defCallbacks->FillCbk) {
          defData->Fill.setVia((yyvsp[(4) - (4)].string));
          defData->Fill.clearPts();
          defData->Geometries.Reset();
        }
      ;}
    break;

  case 895:

/* Line 1455 of yacc.c  */
#line 4969 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 896:

/* Line 1455 of yacc.c  */
#line 4970 "def.y"
    {
        if (defCallbacks->FillCbk) {
          defData->Fill.setLayer((yyvsp[(4) - (4)].string));
          defData->Fill.clearPoly();    // free poly, if any
        }
      ;}
    break;

  case 900:

/* Line 1455 of yacc.c  */
#line 4983 "def.y"
    {
        if (defCallbacks->FillCbk)
          defData->Fill.addRect((yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, (yyvsp[(3) - (3)].pt).x, (yyvsp[(3) - (3)].pt).y);
      ;}
    break;

  case 901:

/* Line 1455 of yacc.c  */
#line 4988 "def.y"
    {
        defData->Geometries.Reset();
      ;}
    break;

  case 902:

/* Line 1455 of yacc.c  */
#line 4992 "def.y"
    {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond
          if (defCallbacks->FillCbk)
            defData->Fill.addPolygon(&defData->Geometries);
        } else {
            defData->defMsg = (char*)defMalloc(10000);
            sprintf (defData->defMsg,
              "POLYGON statement in FILLS LAYER is a version 5.6 and later syntax.\nYour def file is defined with version %g.", defData->VersionNum);
            defError(6564, defData->defMsg);
            defFree(defData->defMsg);
            CHKERR();
        }
      ;}
    break;

  case 907:

/* Line 1455 of yacc.c  */
#line 5016 "def.y"
    {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->FillCbk) {
             if (defData->fillWarnings++ < defSettings->FillWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The LAYER OPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6553, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else {
           if (defCallbacks->FillCbk)
             defData->Fill.setLayerOpc();
        }
      ;}
    break;

  case 908:

/* Line 1455 of yacc.c  */
#line 5035 "def.y"
    {
        if (defCallbacks->FillCbk) {
          defData->Fill.addPts(&defData->Geometries);
          CALLBACK(defCallbacks->FillCbk, defrFillCbkType, &defData->Fill);
          defData->Fill.clear();
        }
    ;}
    break;

  case 913:

/* Line 1455 of yacc.c  */
#line 5054 "def.y"
    {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->FillCbk) {
             if (defData->fillWarnings++ < defSettings->FillWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The VIA OPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6554, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else {
           if (defCallbacks->FillCbk)
             defData->Fill.setViaOpc();
        }
      ;}
    break;

  case 914:

/* Line 1455 of yacc.c  */
#line 5074 "def.y"
    { 
        if (validateMaskInput((int)(yyvsp[(3) - (3)].dval), defData->fillWarnings, defSettings->FillWarnings)) {
             if (defCallbacks->FillCbk) {
                defData->Fill.setMask((int)(yyvsp[(3) - (3)].dval));
             }
        }
      ;}
    break;

  case 915:

/* Line 1455 of yacc.c  */
#line 5084 "def.y"
    { 
        if (validateMaskInput((int)(yyvsp[(3) - (3)].dval), defData->fillWarnings, defSettings->FillWarnings)) {
             if (defCallbacks->FillCbk) {
                defData->Fill.setMask((int)(yyvsp[(3) - (3)].dval));
             }
        }
      ;}
    break;

  case 917:

/* Line 1455 of yacc.c  */
#line 5097 "def.y"
    { 
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->NonDefaultStartCbk) {
            if (defData->nonDefaultWarnings++ < defSettings->NonDefaultWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The NONDEFAULTRULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
              defError(6545, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else if (defCallbacks->NonDefaultStartCbk)
          CALLBACK(defCallbacks->NonDefaultStartCbk, defrNonDefaultStartCbkType,
                   ROUND((yyvsp[(2) - (3)].dval)));
      ;}
    break;

  case 918:

/* Line 1455 of yacc.c  */
#line 5115 "def.y"
    { if (defCallbacks->NonDefaultEndCbk)
          CALLBACK(defCallbacks->NonDefaultEndCbk, defrNonDefaultEndCbkType, 0); ;}
    break;

  case 921:

/* Line 1455 of yacc.c  */
#line 5122 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 922:

/* Line 1455 of yacc.c  */
#line 5123 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.clear(); 
          defData->NonDefault.setName((yyvsp[(3) - (3)].string));
        }
      ;}
    break;

  case 923:

/* Line 1455 of yacc.c  */
#line 5130 "def.y"
    { if (defCallbacks->NonDefaultCbk)
          CALLBACK(defCallbacks->NonDefaultCbk, defrNonDefaultCbkType, &defData->NonDefault); ;}
    break;

  case 926:

/* Line 1455 of yacc.c  */
#line 5138 "def.y"
    {
        if (defCallbacks->NonDefaultCbk)
          defData->NonDefault.setHardspacing();
      ;}
    break;

  case 927:

/* Line 1455 of yacc.c  */
#line 5142 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 928:

/* Line 1455 of yacc.c  */
#line 5144 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addLayer((yyvsp[(4) - (6)].string));
          defData->NonDefault.addWidth((yyvsp[(6) - (6)].dval));
        }
      ;}
    break;

  case 930:

/* Line 1455 of yacc.c  */
#line 5151 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 931:

/* Line 1455 of yacc.c  */
#line 5152 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addVia((yyvsp[(4) - (4)].string));
        }
      ;}
    break;

  case 932:

/* Line 1455 of yacc.c  */
#line 5157 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 933:

/* Line 1455 of yacc.c  */
#line 5158 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addViaRule((yyvsp[(4) - (4)].string));
        }
      ;}
    break;

  case 934:

/* Line 1455 of yacc.c  */
#line 5163 "def.y"
    { defData->dumb_mode = 1; defData->no_num = 1; ;}
    break;

  case 935:

/* Line 1455 of yacc.c  */
#line 5164 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addMinCuts((yyvsp[(4) - (5)].string), (int)(yyvsp[(5) - (5)].dval));
        }
      ;}
    break;

  case 939:

/* Line 1455 of yacc.c  */
#line 5177 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addDiagWidth((yyvsp[(2) - (2)].dval));
        }
      ;}
    break;

  case 940:

/* Line 1455 of yacc.c  */
#line 5183 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addSpacing((yyvsp[(2) - (2)].dval));
        }
      ;}
    break;

  case 941:

/* Line 1455 of yacc.c  */
#line 5189 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addWireExt((yyvsp[(2) - (2)].dval));
        }
      ;}
    break;

  case 942:

/* Line 1455 of yacc.c  */
#line 5196 "def.y"
    { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; ;}
    break;

  case 943:

/* Line 1455 of yacc.c  */
#line 5198 "def.y"
    { defData->dumb_mode = 0; defData->parsing_property = 0; ;}
    break;

  case 946:

/* Line 1455 of yacc.c  */
#line 5205 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defData->NDefProp.propType((yyvsp[(1) - (2)].string));
          CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "NONDEFAULTRULE");
          sprintf(str, "%g", (yyvsp[(2) - (2)].dval));
          defData->NonDefault.addNumProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), str, propTp);
        }
      ;}
    break;

  case 947:

/* Line 1455 of yacc.c  */
#line 5216 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          propTp = defData->NDefProp.propType((yyvsp[(1) - (2)].string));
          CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "NONDEFAULTRULE");
          defData->NonDefault.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
        }
      ;}
    break;

  case 948:

/* Line 1455 of yacc.c  */
#line 5225 "def.y"
    {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          propTp = defData->NDefProp.propType((yyvsp[(1) - (2)].string));
          CHKPROPTYPE(propTp, (yyvsp[(1) - (2)].string), "NONDEFAULTRULE");
          defData->NonDefault.addProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
        }
      ;}
    break;

  case 950:

/* Line 1455 of yacc.c  */
#line 5238 "def.y"
    {
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->StylesStartCbk) {
            if (defData->stylesWarnings++ < defSettings->StylesWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The STYLES statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6546, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else if (defCallbacks->StylesStartCbk)
          CALLBACK(defCallbacks->StylesStartCbk, defrStylesStartCbkType, ROUND((yyvsp[(2) - (3)].dval)));
      ;}
    break;

  case 951:

/* Line 1455 of yacc.c  */
#line 5255 "def.y"
    { if (defCallbacks->StylesEndCbk)
          CALLBACK(defCallbacks->StylesEndCbk, defrStylesEndCbkType, 0); ;}
    break;

  case 954:

/* Line 1455 of yacc.c  */
#line 5263 "def.y"
    {
        if (defCallbacks->StylesCbk) defData->Styles.setStyle((int)(yyvsp[(3) - (3)].dval));
        defData->Geometries.Reset();
      ;}
    break;

  case 955:

/* Line 1455 of yacc.c  */
#line 5268 "def.y"
    {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond will call the callback
          if (defCallbacks->StylesCbk) {
            defData->Styles.setPolygon(&defData->Geometries);
            CALLBACK(defCallbacks->StylesCbk, defrStylesCbkType, &defData->Styles);
          }
        }
      ;}
    break;



/* Line 1455 of yacc.c  */
#line 11478 "def.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 5278 "def.y"


END_LEFDEF_PARSER_NAMESPACE

