/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         defyyparse
#define yylex           defyylex
#define yyerror         defyyerror
#define yydebug         defyydebug
#define yynerrs         defyynerrs

#define yylval          defyylval
#define yychar          defyychar

/* Copy the first part of user declarations.  */
#line 58 "def.y" /* yacc.c:339  */

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

#line 244 "def.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "def.tab.h".  */
#ifndef YY_DEFYY_DEF_TAB_H_INCLUDED
# define YY_DEFYY_DEF_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int defyydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 228 "def.y" /* yacc.c:355  */

        double dval ;
        int    integer ;
        char * string ;
        int    keyword ;  // really just a nop 
        struct LefDefParser::defpoint pt;
        LefDefParser::defTOKEN *tk;

#line 559 "def.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE defyylval;

int defyyparse (void);

#endif /* !YY_DEFYY_DEF_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 576 "def.tab.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1485

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  273
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  521
/* YYNRULES -- Number of rules.  */
#define YYNRULES  956
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1667

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   520

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
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
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
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
    2821,  2829,  2837,  2846,  2854,  2861,  2868,  2876,  2885,  2886,
    2888,  2889,  2892,  2896,  2896,  2901,  2921,  2922,  2926,  2930,
    2950,  2956,  2958,  2960,  2962,  2964,  2966,  2968,  2970,  2986,
    2988,  2990,  2992,  2995,  2998,  2999,  3002,  3005,  3006,  3009,
    3009,  3010,  3010,  3013,  3027,  3026,  3048,  3049,  3048,  3055,
    3059,  3065,  3066,  3065,  3106,  3106,  3139,  3140,  3139,  3172,
    3175,  3178,  3181,  3181,  3184,  3187,  3190,  3199,  3202,  3205,
    3205,  3209,  3213,  3214,  3215,  3216,  3217,  3218,  3219,  3220,
    3221,  3224,  3238,  3238,  3293,  3293,  3302,  3302,  3320,  3321,
    3320,  3328,  3329,  3332,  3344,  3353,  3363,  3364,  3369,  3370,
    3373,  3375,  3377,  3379,  3383,  3400,  3403,  3403,  3422,  3430,
    3421,  3438,  3443,  3450,  3457,  3460,  3466,  3467,  3470,  3476,
    3476,  3487,  3488,  3491,  3498,  3499,  3502,  3504,  3504,  3507,
    3507,  3509,  3515,  3525,  3530,  3531,  3534,  3545,  3554,  3564,
    3565,  3568,  3576,  3584,  3593,  3600,  3604,  3607,  3621,  3635,
    3636,  3639,  3640,  3650,  3663,  3663,  3668,  3668,  3673,  3678,
    3684,  3685,  3687,  3689,  3689,  3698,  3699,  3702,  3703,  3706,
    3711,  3716,  3721,  3727,  3738,  3749,  3752,  3758,  3759,  3762,
    3768,  3768,  3775,  3776,  3781,  3782,  3785,  3785,  3789,  3789,
    3792,  3791,  3800,  3800,  3804,  3804,  3806,  3806,  3824,  3831,
    3832,  3841,  3855,  3856,  3860,  3859,  3870,  3871,  3884,  3905,
    3936,  3937,  3941,  3940,  3949,  3950,  3963,  3984,  4016,  4017,
    4020,  4029,  4032,  4043,  4044,  4047,  4053,  4053,  4059,  4060,
    4064,  4069,  4074,  4079,  4080,  4079,  4088,  4095,  4096,  4094,
    4102,  4103,  4103,  4109,  4110,  4116,  4116,  4118,  4124,  4130,
    4136,  4137,  4140,  4141,  4140,  4145,  4147,  4150,  4152,  4154,
    4156,  4159,  4160,  4164,  4163,  4167,  4166,  4171,  4172,  4174,
    4174,  4176,  4176,  4179,  4183,  4190,  4191,  4194,  4195,  4194,
    4203,  4203,  4211,  4211,  4219,  4225,  4226,  4225,  4231,  4231,
    4237,  4244,  4247,  4254,  4255,  4258,  4264,  4264,  4270,  4271,
    4278,  4279,  4281,  4285,  4286,  4288,  4291,  4292,  4295,  4295,
    4301,  4301,  4307,  4307,  4313,  4313,  4319,  4319,  4325,  4325,
    4330,  4338,  4337,  4341,  4342,  4345,  4350,  4356,  4357,  4360,
    4361,  4363,  4365,  4367,  4369,  4373,  4374,  4377,  4380,  4383,
    4386,  4390,  4394,  4395,  4398,  4398,  4407,  4408,  4411,  4412,
    4415,  4414,  4427,  4427,  4430,  4432,  4434,  4436,  4439,  4441,
    4447,  4448,  4451,  4455,  4456,  4459,  4460,  4459,  4469,  4470,
    4472,  4472,  4476,  4477,  4480,  4491,  4500,  4510,  4512,  4516,
    4520,  4521,  4524,  4533,  4534,  4533,  4553,  4552,  4569,  4570,
    4573,  4599,  4621,  4622,  4625,  4634,  4634,  4653,  4674,  4695,
    4713,  4745,  4746,  4751,  4751,  4769,  4787,  4821,  4857,  4858,
    4861,  4867,  4866,  4892,  4894,  4898,  4902,  4903,  4906,  4914,
    4915,  4914,  4922,  4923,  4926,  4932,  4931,  4944,  4946,  4950,
    4954,  4955,  4958,  4965,  4966,  4965,  4975,  4976,  4975,  4984,
    4985,  4988,  4994,  4993,  5012,  5013,  5015,  5016,  5021,  5040,
    5050,  5051,  5053,  5054,  5059,  5079,  5089,  5099,  5102,  5120,
    5124,  5125,  5128,  5129,  5128,  5139,  5140,  5143,  5148,  5150,
    5148,  5157,  5157,  5163,  5163,  5169,  5169,  5175,  5178,  5179,
    5182,  5188,  5194,  5202,  5202,  5206,  5207,  5210,  5221,  5230,
    5241,  5243,  5260,  5264,  5265,  5269,  5268
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
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
  "styles_rules", "styles_rule", "$@173", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
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

#define YYPACT_NINF -1120

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1120)))

#define YYTABLE_NINF -543

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -91, -1120,    82,    78,   100, -1120,    80,    46,  -137,  -116,
    -106, -1120,   338, -1120, -1120, -1120, -1120, -1120,   186, -1120,
     104, -1120, -1120, -1120, -1120, -1120,   228,   231,    47,    47,
   -1120,   240,   242,   251,   265,   271,   276,   279,   282,   302,
     308,   334,   340,   347,   362,   360,   369, -1120, -1120, -1120,
     370,   384,   406,   421,   427, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,   436, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120,   180, -1120, -1120,   450,   218,
     492,   407,   502,   507,   519,   527,   273,   288, -1120, -1120,
   -1120, -1120,   554,   561,   310,   311,   315,   316,   317,   318,
     319,   320,   323, -1120,   324,   329,   333,   348,   352,   365,
   -1120,     9,   367,   368,   371,   372,   375,    59,   -46, -1120,
     -29,   -24,   -23,   -22,   -20,   -18,   -11,    -5,     5,     6,
      14,    24,    51,    55,    56,    66,    67, -1120, -1120,    70,
     376, -1120,   377,   572,   379,   392,   585,   587,    12,   273,
   -1120, -1120,   542,   566, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   330,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   630,
     620, -1120, -1120,   641, -1120, -1120, -1120,   642,   656, -1120,
   -1120, -1120,   631, -1120, -1120,   650, -1120, -1120, -1120, -1120,
   -1120,   629, -1120, -1120, -1120, -1120, -1120,   632, -1120, -1120,
   -1120,   613, -1120, -1120, -1120, -1120,   590,   126, -1120, -1120,
   -1120, -1120,   609, -1120,   605, -1120, -1120, -1120, -1120,   591,
     420, -1120, -1120, -1120,   543, -1120, -1120,   584,    92, -1120,
   -1120,   582, -1120, -1120, -1120, -1120,   514, -1120, -1120, -1120,
     479,    17, -1120, -1120,    10,   487,   704, -1120, -1120, -1120,
     516,    16, -1120, -1120, -1120,   731,    74,   482,   689, -1120,
   -1120, -1120, -1120,   474, -1120, -1120,   735,   736,    13,    15,
   -1120, -1120,   737,   738,   480, -1120, -1120, -1120, -1120, -1120,
   -1120,   583, -1120, -1120, -1120, -1120,   725, -1120, -1120,   744,
     747, -1120,   750, -1120,   751, -1120,   752,    76,     8, -1120,
      95,   116, -1120,   131, -1120,   753,   754, -1120, -1120, -1120,
     491,   493, -1120, -1120, -1120, -1120,   757,   164, -1120, -1120,
     185, -1120,   759, -1120, -1120, -1120, -1120,   500, -1120,   763,
     222, -1120,   765, -1120, -1120, -1120,   273, -1120, -1120, -1120,
   -1120,   -13, -1120, -1120, -1120,     0, -1120,   710, -1120, -1120,
   -1120,   766, -1120,   635,   635,   501,   503,   505,   506,  -128,
     755,   768, -1120,   773,   775,   776,   777,   779,   780, -1120,
     781,   782,   783,   784,   520,   762, -1120, -1120,   788, -1120,
     140, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,    18, -1120, -1120, -1120,   273, -1120, -1120, -1120, -1120,
     528, -1120, -1120,   628, -1120, -1120, -1120,   770, -1120,   564,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   229,
     792,   795,   325,   325,   796,   235, -1120, -1120,   312, -1120,
   -1120,   797, -1120,    -6, -1120, -1120,   119,   798,   799,   800,
   -1120,   694, -1120,   357, -1120, -1120,   802,   803, -1120,   273,
     273,     3,   804,   273, -1120, -1120, -1120,   805,   806,   273,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120,   786,   787, -1120, -1120, -1120, -1120,
   -1120, -1120,   807,   635,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   550,   741,   812, -1120,   273, -1120, -1120,
     188,   814, -1120, -1120, -1120,   273, -1120, -1120, -1120, -1120,
   -1120,   815, -1120,   817, -1120,   273,   273,   635, -1120,   820,
    -109,   819, -1120, -1120, -1120,   509,   821,   -37,   823, -1120,
   -1120, -1120, -1120,   824, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   273, -1120,   188,   825, -1120,   509,   826,   -37,   827,
     443, -1120, -1120, -1120, -1120,   828, -1120, -1120,   829, -1120,
   -1120,   822, -1120,   289, -1120, -1120, -1120,   832, -1120,    40,
      57,   570, -1120,   327, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   835, -1120, -1120,   834, -1120,   213, -1120, -1120, -1120,
     837,   838,   -21,   301, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,   575, -1120,   273, -1120, -1120, -1120,   273,
     273, -1120, -1120,   273,   273,   248,   273,   839,   840,   578,
   -1120,   844, -1120, -1120,   845,   586,   588,   592,   593,   594,
     595,   596,   597,   598, -1120, -1120,   673,   254,   273,   273,
     846, -1120, -1120, -1120, -1120, -1120, -1120,   847,   635,   849,
     852, -1120,   801, -1120, -1120,   273, -1120,   599, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,   862, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,   863,   865,   866, -1120,   867,   869, -1120,
     868,   871,   872,   273,   873, -1120, -1120,   874, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120,   876,   877,   878,   879, -1120,
     880, -1120,   881,   883,   884, -1120, -1120, -1120,   885, -1120,
     325, -1120, -1120, -1120,   808,   886,   887,   889,   890,   893,
   -1120, -1120,   894,   621,   895,   633, -1120,   897,   896,   898,
     272,   809,   625, -1120, -1120,   639, -1120, -1120,   337,   902,
     903,   905,   906,   907,   908, -1120, -1120,   -28, -1120,   273,
      49, -1120,   273, -1120, -1120, -1120,   273, -1120,    62, -1120,
   -1120,   273,   891,   892, -1120,   888, -1120,   769,   769, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   912,
     913,   654,   899, -1120,    73, -1120, -1120, -1120, -1120,   273,
     260,   917, -1120, -1120,    36,   191,   852, -1120, -1120,   916,
     919, -1120,     7, -1120,   920, -1120, -1120, -1120, -1120,   212,
     867, -1120, -1120,   871, -1120, -1120,   875, -1120, -1120, -1120,
     273, -1120,   923, -1120,   924,   223,   879, -1120, -1120, -1120,
     653, -1120,   882, -1120,    20,   364, -1120,   921, -1120,   925,
   -1120, -1120, -1120, -1120,   922,   928,   929,   922,   930, -1120,
     705, -1120, -1120,   931,   932, -1120,   933,   934,   935, -1120,
   -1120, -1120,   938,   939, -1120, -1120, -1120, -1120,   940,   941,
   -1120,   942,   943, -1120,   261,   680, -1120, -1120, -1120,   944,
   -1120, -1120,   273,   -16,   198,   273, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,   945,   946,   948, -1120,   949,   950,   950,
   -1120,   771, -1120, -1120, -1120, -1120, -1120, -1120,   951,   952,
     953, -1120, -1120, -1120, -1120,  -105, -1120, -1120, -1120,   954,
   -1120,   635, -1120, -1120, -1120, -1120,   955,   958, -1120, -1120,
   -1120,   693, -1120, -1120, -1120, -1120, -1120,   875, -1120, -1120,
   -1120,   299,   273, -1120, -1120, -1120, -1120, -1120, -1120,   960,
     273, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
     959, -1120, -1120, -1120,   273, -1120,   961,   962,   963, -1120,
     966, -1120,   706, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
     967,   968, -1120,   900, -1120, -1120, -1120, -1120, -1120,   264,
     969,   870, -1120,   437,   374,   388,   437,   374,   388, -1120,
     810, -1120,    -7, -1120, -1120, -1120,   973, -1120,   273, -1120,
     -81, -1120, -1120, -1120, -1120,   -80, -1120,   712, -1120, -1120,
   -1120, -1120,   975, -1120,   976,   977,   978,  -121,   956,   957,
     964,   275,   979, -1120, -1120, -1120, -1120, -1120, -1120,   982,
     983,   984,   985,   986,   987,   988,   991,   992, -1120,   331,
   -1120, -1120,   993, -1120,   994,   996,   997, -1120,   722,     1,
     871, -1120, -1120, -1120, -1120,   273, -1120,   851, -1120,   884,
     273,   273, -1120, -1120,   884, -1120, -1120, -1120, -1120,   440,
   -1120, -1120,   733,   734,   740, -1120, -1120, -1120,   901, -1120,
     414, -1120,  1001, -1120, -1120,  1002,  1005,  1006,  1007,   374,
   -1120,  1008,  1010,  1011,  1012, -1120, -1120,   374, -1120,  1013,
   -1120,  1014, -1120, -1120, -1120,  1015, -1120, -1120,   273,  1016,
   -1120,  1017, -1120,   273, -1120,   273,   970,  1003,  1018, -1120,
   -1120, -1120,  1019,  1021,  1022, -1120,   864, -1120, -1120,   298,
   -1120,   982,   761, -1120, -1120,  1025, -1120, -1120,   761,   764,
   -1120, -1120, -1120, -1120,  1027,   767,   767,   767, -1120, -1120,
   -1120,  1028, -1120, -1120,    19, -1120, -1120, -1120, -1120, -1120,
     635,  1030, -1120,   882,   273, -1120,   304, -1120, -1120, -1120,
   -1120,  1033, -1120,  1034, -1120,   772,  1035, -1120, -1120, -1120,
     774,  1038, -1120,    33,  1037,  1039,  1040,  1041, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,  1044, -1120, -1120, -1120,
   -1120,  1043,   483, -1120, -1120,  1045,  1046,  1023, -1120, -1120,
     413, -1120, -1120,   273, -1120,   982,   904,   273,  1047, -1120,
     816,  1051, -1120,    64, -1120,   789,   790,   791,  1054,    72,
   -1120,  1058,    21,    22, -1120, -1120,  1057, -1120,   273,    50,
   -1120, -1120,  1060,  1061,  1062, -1120,  1063, -1120, -1120, -1120,
   -1120, -1120,  1064,  1065, -1120, -1120, -1120, -1120, -1120,  1068,
   -1120, -1120, -1120, -1120, -1120,   778,   811,  1067, -1120, -1120,
   -1120,   509, -1120,  1070,  1069,  1072,  1074,  1075,  1076,  1077,
    1078,  1079,   244, -1120,  1066, -1120, -1120, -1120, -1120,   635,
   -1120,  1080,   273, -1120,   273,  1081,   488, -1120, -1120, -1120,
   -1120, -1120,  1084, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,    28,    29,    31,    32,    11, -1120,   273,  1083,   443,
   -1120,   830,   831,  1086, -1120,   833,   833, -1120,  1085,  1087,
     504, -1120, -1120, -1120, -1120,  1088,  1091,  1092, -1120, -1120,
    1048,  1048,  1048,  1048,  1071,  1082,  1048,  1089, -1120, -1120,
   -1120, -1120, -1120,  1094, -1120,  1099,  1100,  1101, -1120, -1120,
   -1120,   273,  1102, -1120, -1120, -1120,   841,  1103,   871,   836,
   -1120,   842, -1120,   843, -1120,   848, -1120,   484,   850,  1105,
     853, -1120, -1120, -1120, -1120, -1120, -1120,    11,   854,   855,
     856,  1009,    53, -1120,  1110, -1120, -1120, -1120, -1120, -1120,
     -19, -1120,   515, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120,   965,   273,   857,
     990,   858, -1120,   875, -1120, -1120, -1120, -1120,  1114,  1096,
    1121, -1120,    77,    23, -1120,  1125,  1126, -1120,  1127, -1120,
   -1120, -1120, -1120,  1128,  1129,  1130, -1120, -1120, -1120, -1120,
     971,  1133,  1134,  1135,  1137,   972,  1138,   909, -1120,   910,
   -1120, -1120,  1042,  1139,  1142,   562, -1120,   911,    25,    26,
    1145,  1146, -1120,  1147,  1148, -1120, -1120, -1120,  1149,   -12,
   -1120, -1120, -1120, -1120,  1150,   -10, -1120,  1151,   273, -1120,
    1153,  1106,  1154,  1155,  1118,   914, -1120,   861,   915,   918,
     926,   927,   936,   937,   947, -1120,  1156,  1157,   273, -1120,
    1158,  1159,   273,  1160,  1164,  1143,  1163,  1166,  1161,  1167,
    1168, -1120, -1120, -1120, -1120,   974,   980, -1120, -1120, -1120,
   -1120,   273, -1120, -1120,   273,   981,  1171,  1170,  1152,   989,
    1175,  1165,  1176,  1173,  1180, -1120,   273, -1120,  1182,  1183,
    1184, -1120,  1172,  1186,  1187,  1190,  1191, -1120,   995, -1120,
    1192,  1194,  1174,  1196,   998,   999,   273,  1000, -1120,  1197,
    1198,  1004, -1120, -1120,  1199, -1120,  1202, -1120,  1203, -1120,
    1020,  1026,  1204,  1205,  1206,  1207, -1120
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
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
     222,    27,   259,    19,    15,   284,    23,   352,    30,   494,
      21,   576,    13,    16,   609,   609,    28,   637,    22,   683,
      20,   710,    32,   735,    25,   753,    26,   823,    14,   840,
      29,   876,    18,   890,    24,     0,    31,   953,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   111,   204,
     205,   203,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    53,     0,     0,     0,     0,     0,     0,
      63,     0,   820,     0,     0,     0,     0,     0,     0,   201,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   922,   920,     0,
       0,   221,     0,     0,     0,     0,     0,     0,     0,     0,
     246,   117,     0,     0,   280,   258,   351,   572,   918,   575,
     608,   636,   607,   682,   734,   752,   709,    94,    95,     0,
     283,   278,   282,   821,   819,   838,   874,   888,   951,     0,
       0,   113,   110,     0,   120,   119,   116,     0,     0,   224,
     223,   220,     0,   261,   260,     0,   288,   285,   293,   290,
     279,     0,   356,   353,   373,   355,   350,     0,   497,   495,
     493,     0,   579,   577,   581,   574,     0,     0,   610,   611,
     612,   605,     0,   606,     0,   640,   638,   642,   635,     0,
       0,   684,   688,   681,     0,   712,   711,     0,     0,   736,
     733,     0,   756,   754,   766,   751,     0,   825,   818,   824,
       0,     0,   837,   841,     0,     0,     0,   873,   877,   882,
       0,     0,   887,   891,   899,     0,     0,     0,     0,   950,
     954,    52,    55,     0,    57,    59,     0,     0,     0,     0,
     248,   247,     0,     0,     0,    65,    77,    71,    81,    73,
      67,     0,    75,    69,    79,    64,     0,   115,   184,     0,
       0,   256,     0,   257,     0,   349,     0,     0,   287,   480,
       0,     0,   573,     0,   604,     0,   584,   634,   614,   616,
       0,     0,   623,   627,   633,   680,     0,     0,   707,   686,
       0,   708,     0,   750,   737,   740,   742,     0,   801,     0,
       0,   822,     0,   839,   843,   846,     0,   871,   868,   875,
     879,     0,   889,   896,   893,     0,   923,     0,   917,   921,
     952,     0,    93,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    83,     0,     0,     0,     0,     0,     0,    62,
       0,     0,     0,     0,     0,     0,   225,   262,     0,   286,
       0,   307,   294,   306,   302,   295,   296,   301,   303,   304,
     305,     0,   300,   297,   299,     0,   298,   292,   291,   358,
       0,   357,   354,     0,   403,   374,   391,   418,   496,     0,
     531,   498,   502,   499,   500,   501,   580,   583,   582,     0,
       0,     0,     0,     0,     0,     0,   641,   639,     0,   658,
     643,     0,   685,     0,   696,   689,     0,     0,     0,     0,
     744,   758,   755,     0,   780,   767,     0,     0,   861,     0,
       0,     0,     0,     0,   885,   878,   883,     0,     0,     0,
     902,   892,   900,   925,   919,   955,   100,   102,   103,   101,
     104,   107,   106,   105,     0,     0,   250,   252,   251,   253,
     109,   249,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   227,     0,   289,   310,
       0,     0,   308,   345,   346,   342,   347,   337,   336,   329,
     219,     0,   323,     0,   320,     0,   319,     0,   362,     0,
       0,     0,   426,   427,   381,     0,     0,     0,     0,   428,
     389,   416,   397,     0,   401,   392,   394,   378,   379,   375,
     419,     0,   514,     0,     0,   522,     0,     0,     0,     0,
       0,   544,   546,   548,   529,     0,   506,   520,     0,   511,
     516,   503,   578,     0,   591,   585,   615,     0,   620,     0,
       0,   625,   613,     0,   628,   646,   648,   650,   652,   654,
     656,     0,   705,   706,     0,   693,     0,   715,   716,   713,
       0,     0,     0,   760,   757,   768,   770,   772,   774,   776,
     778,   826,   844,   847,   870,     0,   842,   869,   880,     0,
       0,   897,   894,     0,     0,     0,     0,     0,     0,     0,
     186,    88,    86,    84,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   114,   121,   207,     0,     0,   266,
       0,   315,   316,   317,   318,   314,   348,     0,     0,     0,
       0,   344,   325,   327,   321,     0,   341,   359,   360,   413,
     409,   410,   411,   412,   377,   384,     0,   173,   174,   175,
     176,   177,   178,   179,   180,   387,   388,   563,   560,   561,
     562,   383,   386,     0,     0,     0,   385,     0,     0,   395,
       0,     0,     0,     0,     0,   519,   525,     0,   527,   528,
     524,   526,   481,   482,   483,   484,   485,   486,   487,   490,
     491,   492,   489,   488,   509,     0,     0,     0,     0,   510,
       0,   521,     0,     0,     0,   589,   599,   587,     0,   618,
       0,   621,   619,   626,     0,     0,     0,     0,     0,     0,
     662,   670,     0,   659,     0,     0,   692,     0,     0,     0,
       0,     0,     0,   745,   748,     0,   761,   762,   763,     0,
       0,     0,     0,     0,     0,   828,   848,     0,   862,     0,
       0,   884,     0,   904,   910,   901,     0,   924,     0,   926,
     937,     0,     0,     0,   217,   188,    89,   558,   558,    90,
      66,    78,    72,    82,    74,    68,    76,    70,    80,     0,
       0,     0,   211,   226,     0,   240,   228,   239,   264,     0,
       0,   312,   309,   343,     0,     0,   330,   331,   326,     0,
       0,   322,     0,   363,     0,   382,   390,   417,   398,     0,
     402,   404,   393,     0,   380,   433,   376,   429,   420,   414,
       0,   523,     0,   547,     0,     0,   530,   551,   507,   512,
     532,   568,   505,   564,     0,   586,   594,     0,   622,     0,
     629,   630,   631,   632,   644,   649,   651,   644,     0,   655,
     678,   687,   694,     0,     0,   717,     0,     0,     0,   721,
     738,   741,     0,     0,   743,   764,   765,   759,     0,     0,
     789,     0,     0,   789,     0,   845,   866,   863,   865,     0,
     248,   881,     0,     0,     0,     0,   928,   943,   927,   935,
     931,   933,   248,     0,     0,     0,   192,     0,    91,    91,
     122,   209,   202,   212,   206,   229,   244,   234,     0,     0,
       0,   231,   236,   265,   263,     0,   267,   313,   311,     0,
     339,     0,   334,   335,   333,   332,     0,     0,   364,   368,
     366,     0,   802,   407,   408,   406,   405,   396,   468,   431,
     430,   421,     0,   545,   549,   554,   555,   553,   552,   541,
       0,   533,   535,   536,   534,   537,   540,   539,   538,   517,
       0,   566,   565,   593,     0,   590,     0,     0,     0,   600,
     588,   617,     0,   645,   647,   664,   663,   672,   671,   653,
       0,     0,   657,   700,   690,   691,   719,   718,   720,     0,
       0,     0,   749,     0,     0,   773,     0,     0,   779,   827,
       0,   829,     0,   849,   852,   853,     0,   867,   872,   248,
       0,   898,   905,   906,   907,     0,   248,     0,   911,   912,
     913,   248,     0,   945,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    92,    87,    85,   124,   210,   208,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   268,     0,
     340,   338,     0,   328,     0,     0,     0,   361,   399,     0,
       0,   424,   425,   423,   415,     0,   515,   556,   508,     0,
       0,     0,   571,   476,     0,   592,   601,   602,   603,     0,
     595,   624,   666,   674,     0,   679,   701,   695,     0,   714,
       0,   722,     0,   746,   783,     0,     0,     0,     0,   771,
     795,     0,     0,     0,     0,   790,   783,   777,   830,     0,
     855,     0,   857,   858,   859,     0,   860,   864,   886,     0,
     908,     0,   914,   909,   895,   903,     0,   944,     0,   932,
     934,   956,     0,     0,     0,   187,     0,   193,   559,     0,
     216,   214,   254,   245,   235,     0,   242,   241,   254,     0,
     271,   276,   277,   270,     0,   370,   370,   370,   804,   808,
     803,     0,   472,   473,     0,   434,   469,   471,   470,   432,
       0,     0,   550,   543,     0,   248,     0,   567,   597,   598,
     596,     0,   665,     0,   673,   660,     0,   697,   723,   725,
       0,     0,   769,     0,     0,     0,     0,     0,   796,   791,
     793,   792,   794,   775,   832,   850,     0,   854,   851,   915,
     916,     0,     0,   946,   936,     0,     0,   190,   194,   123,
       0,   125,   127,     0,   213,   214,     0,     0,     0,   232,
       0,   269,   324,     0,   371,     0,     0,     0,     0,   400,
     475,     0,     0,     0,   438,   422,     0,   248,   518,     0,
     569,   477,     0,     0,     0,   702,     0,   727,   727,   739,
     747,   781,     0,     0,   784,   797,   799,   798,   800,   831,
     856,   929,   948,   949,   947,     0,     0,     0,   189,   196,
     136,     0,   126,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   132,     0,   139,   135,   130,   142,     0,
     215,     0,     0,   243,     0,     0,     0,   272,   372,   365,
     369,   367,     0,   814,   815,   816,   812,   817,   809,   810,
     474,     0,     0,     0,     0,   435,   557,   513,     0,     0,
     438,     0,     0,     0,   698,   724,   726,   787,     0,     0,
       0,   833,   938,    97,    99,     0,   195,     0,   134,   128,
     181,   181,   181,   181,     0,     0,   181,     0,   169,   170,
     171,   172,   156,     0,   129,     0,     0,     0,   144,   255,
     230,     0,     0,   274,   275,   273,   806,     0,     0,     0,
     455,     0,   457,     0,   456,     0,   458,   440,     0,     0,
       0,   448,   449,   439,   454,   479,   478,   570,   667,   675,
       0,   703,     0,   728,   782,   785,   786,   835,   836,   834,
     930,   191,     0,   197,   137,   182,   145,   146,   147,   148,
     149,   151,   153,   154,   133,   140,   131,   159,     0,     0,
       0,     0,   813,   811,   459,   461,   460,   462,     0,   442,
       0,   437,   452,     0,   436,     0,     0,   661,     0,   699,
     731,   729,   788,     0,     0,     0,   939,   199,   200,   198,
     157,     0,     0,     0,     0,   161,     0,     0,   248,     0,
     807,   805,     0,     0,     0,   441,   450,     0,     0,     0,
       0,     0,   704,     0,     0,   941,   940,   942,     0,   163,
     183,   150,   152,   155,     0,   166,   160,     0,   233,   237,
       0,     0,     0,     0,   443,     0,   453,     0,     0,     0,
       0,     0,     0,     0,     0,   158,     0,     0,     0,   162,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   463,   465,   464,   466,   668,   676,   732,   730,   164,
     165,     0,   167,   168,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   138,     0,   143,     0,     0,
       0,   467,     0,     0,     0,     0,     0,   248,     0,   445,
       0,     0,     0,     0,     0,     0,   141,     0,   446,     0,
       0,     0,   669,   677,     0,   444,     0,   451,     0,   447,
       0,     0,     0,     0,     0,     0,   238
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   187, -1120, -1120,   123, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120,  -403, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120,  -592, -1035, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120,  1124, -1120, -1120, -1120, -1120, -1120,  -141,   105,
   -1120, -1120,  -344, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,  -495,
    -187,  -942,  -126,    -3, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   623, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   353,
   -1120, -1120, -1120, -1120, -1120, -1120,    27, -1120, -1120, -1120,
   -1120, -1120, -1120,  1056, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120,  -652, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   341,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   785,
    -877, -1120, -1120,    88, -1120, -1120, -1120, -1120,  -178, -1120,
   -1120, -1120, -1119, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,  -176, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   328, -1120,   387,   634,    87, -1120, -1120,    83, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,  1136, -1120, -1120,  -241,
   -1120, -1120,   756, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120,   313, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120,  -909, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,  -100, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
      58, -1120, -1120, -1120, -1120,   290, -1120,   168,  -955, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
     739, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   417, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120,   285, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,  1024,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120
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
    1537,  1565,  1588,  1592,  1432,   725,  1486,  1531,   226,    74,
     133,   835,   966,  1348,  1101,  1207,  1349,  1416,  1483,    75,
     227,    76,   131,   851,   852,  1108,   974,  1109,  1294,  1295,
      77,    78,  1304,    79,    80,   160,   230,   342,   546,   687,
     856,  1110,  1116,  1374,  1112,  1117,  1594,   857,  1111,   189,
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

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     190,   525,   320,   431,   748,   655,  1007,   454,  1078,   460,
    1235,   998,   447,   210,   509,  1457,   363,   503,   318,   405,
    1097,   407,   564,   479,  1033,  1312,   484,  1391,  1393,  1548,
     509,  1577,  1579,   386,  1449,  1451,   494,  1453,  1455,  1523,
     386,  1458,   989,   946,   393,   384,  1586,    11,  1590,  1231,
     223,  1179,  1331,    -9,   727,    -9,    -9,    -9,    -9,    -9,
      -9,  1118,    -9,    -9,    -9,    -9,    -9,   228,     1,    -9,
      -9,   709,   232,   235,   241,    -9,   247,    -9,   251,   503,
     219,  1545,     5,   803,  1520,   256,     6,   804,    -9,    -9,
     956,   262,   632,   633,  1189,  1191,   634,   635,  1398,   449,
      -9,   264,   269,   975,     8,  1399,    -9,  1546,  1383,  1384,
     274,   710,   711,   712,   713,    -9,   358,   359,   360,   361,
     277,    -9,   976,   977,  1385,   614,    -9,  1320,  1119,    13,
     670,  1386,   999,   358,   359,   360,   361,  1188,   530,    -9,
     450,   188,    -9,    -9,  1193,  1201,   947,   281,   188,  1195,
      14,   286,   290,    -9,  1164,   220,    -9,  1176,  1232,  1233,
      15,   560,   295,   300,   706,   822,   307,  1180,  1181,   826,
     397,   831,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,  1190,  1192,  1332,  1333,  1459,   728,   729,   730,
    1378,   948,   119,    -9,   992,   993,   374,   994,   121,  1460,
     375,   376,   358,   359,   360,   361,    -9,    -9,    -9,  1521,
    1182,  1183,  1184,    -9,  1268,  1003,  1004,   362,  1005,   129,
     130,   224,  1268,    -9,  1524,    -9,  1015,  1016,   957,  1017,
    1587,   560,  1591,   385,   127,  1185,   949,   128,   229,   510,
     560,  1525,   504,   233,   236,   242,   134,   242,   135,   252,
       9,    10,  1080,   505,  1186,   510,   257,   136,   387,  1387,
     499,    -9,   257,    -9,    -9,   387,   511,   637,   638,   656,
    1234,   137,   265,   270,   394,   211,   377,   138,  1000,   448,
    1234,   275,   139,   319,   406,   140,   408,   188,   141,   188,
    1313,   278,  1392,  1394,  1549,   863,  1578,  1580,  1450,  1452,
      -9,  1454,  1456,  1318,   504,   188,   559,   560,   142,   958,
     779,   959,   780,   798,   799,   565,   561,   143,   282,   567,
     960,   961,   287,   291,   978,   979,   980,   782,   981,   780,
     775,   324,   982,   296,   301,  1131,  1132,   308,  1133,   325,
     144,   177,   429,   855,   430,    16,   145,    17,    18,    19,
      20,    21,    22,   146,    23,    24,    25,    26,    27,   326,
     776,    28,    29,   148,   671,   672,   673,    30,   147,    31,
     674,   327,   149,   654,   190,  1397,   152,   659,   781,   781,
      32,    33,   452,   663,   453,   562,  1487,  1488,  1489,   328,
     153,  1492,    34,   625,   626,   627,   628,   458,    35,   459,
     329,   358,   359,   360,   361,   563,   330,    36,   691,   692,
     693,   694,   154,    37,   785,   786,   787,   788,    38,   806,
     807,   688,   925,   926,   927,   928,   331,   155,  1576,   698,
     477,    39,   478,   156,    40,    41,  1036,  1037,  1038,   704,
     705,  1350,   159,  1248,  1249,    42,  1250,   177,    43,   553,
     554,   482,   556,   483,   180,   777,   560,   935,   936,  1086,
    1351,  1165,  1166,  1167,  1168,   743,  1085,   188,   819,   645,
     646,   647,   648,   649,   650,  1171,  1172,  1173,  1174,   560,
    1428,  1429,  1430,  1431,   181,    44,  1342,  1343,   492,  1344,
     493,  1443,  1444,   629,  1445,   612,   182,   613,    45,    46,
      47,   622,   183,   623,   332,    48,   184,  1477,  1478,  1508,
    1479,   185,   333,   334,   827,    49,   828,    50,  1527,  1528,
     853,  1529,   854,   186,   560,  1140,   984,  1069,   985,  1070,
    1159,   187,  1160,   821,   190,   632,   633,   825,   190,   908,
     190,  1205,   188,  1206,   630,   516,   517,   518,   519,   520,
     521,   522,   523,    51,   191,    52,    53,  1352,  1353,  1221,
     192,  1222,   858,   859,  1289,   193,  1290,   322,  1258,  1259,
     323,  1503,  1319,  1234,  1306,  1307,   194,   195,   313,   871,
     560,   196,   197,   198,   199,   200,   201,  1573,  1121,   202,
     203,   316,    54,   317,   592,   204,  1568,   593,   594,   205,
     572,   573,  1354,  1355,  1356,  1357,  1358,  1359,  1360,  1361,
     595,   596,   597,   598,   206,   599,   579,   889,   207,   600,
     601,   602,   603,   516,   517,   518,   519,   520,   521,   522,
     523,   208,   950,   213,   215,   952,   336,   216,   217,   955,
     337,   218,   311,   312,   962,   314,  1245,  1362,   752,   753,
     754,   755,   756,   757,   758,   759,   760,   761,   315,  1363,
     338,   570,   571,  1364,   572,   573,   341,   340,  1365,  1366,
    1367,  1368,   349,   343,   574,   575,   576,   577,   762,   578,
     579,   345,   354,   357,   364,   365,   352,   580,   368,   369,
     371,   373,   378,   381,   383,  1646,   516,   517,   518,   519,
     520,   521,   522,   523,   389,   763,   717,   718,   719,   720,
     721,   722,   723,   724,  1021,  1022,  1023,  1024,  1025,  1026,
    1027,  1028,   676,   677,   678,   679,   680,   681,   682,   683,
     604,   560,   390,   983,   392,   396,   400,   401,   990,   605,
     402,   403,   404,   410,   411,   419,   412,   423,   424,   606,
     581,   582,   583,   425,   426,   427,   428,   466,   467,  1418,
     472,   476,   473,   486,  1012,  1079,   490,   491,  1091,   496,
     514,   526,   515,   527,   533,   528,   529,   534,  1034,   535,
     536,   537,   532,   538,   539,   540,   541,   542,   544,   545,
     543,   607,   548,   608,   584,   560,   616,   569,   590,   617,
     621,   631,   640,   641,   642,   643,   651,   652,   658,   661,
     662,   667,   668,   669,   585,   586,   684,   685,   686,   609,
     696,   701,   610,   703,   708,   715,  -504,   726,   190,   732,
     736,   746,   749,   751,   769,   771,   778,  1315,   783,   795,
     796,   801,   802,   817,   834,   832,   833,   836,   850,   839,
     861,   862,   840,   864,   841,   587,   865,   588,   842,   843,
     844,   845,   846,   847,   848,  1292,   875,   876,   872,   877,
     878,   879,   868,   882,   884,   885,   888,   890,   891,  1441,
     892,   893,   894,   895,   898,   899,  1136,   900,   901,   907,
     918,   931,   910,   911,   190,   912,   913,   914,   917,   920,
     909,   922,   923,   921,   924,   934,   938,   939,  1145,   940,
     941,   942,   943,   965,   930,   967,   970,   963,   964,   971,
     972,   987,   996,   997,  1001,  1041,  1043,   973,  1009,  1013,
    1014,  1042,  1045,  1047,  1050,  1031,  1051,  1054,  1055,  1056,
    1057,  1058,  1061,  1062,  1063,  1064,  1066,  1067,  1072,  1107,
    1077,  1098,  1099,  1244,  1100,  1102,  1103,  1113,  1114,  1115,
    1120,  1122,  1123,  1127,  -542,  1142,  1438,  1146,  1147,  1148,
    1149,  1154,  1151,  1162,  1155,  1163,  1178,  1187,  1194,  1196,
    1198,  1199,  1200,  1202,  1203,  1208,  1210,  1212,  1213,  1214,
    1204,  1228,  1215,  1216,  1217,  1218,  1219,  1241,  1225,  1224,
    1226,  1227,  1251,  1253,  1156,  1260,  1257,  1282,  1264,  1240,
    1255,  1265,  1266,  1267,  1269,   190,  1270,  1271,  1272,  1275,
    1277,  1278,  1279,  1280,  1284,  1285,  1281,  1286,  1287,  1296,
    1288,  1298,  1300,  1302,  1310,  1303,  1316,  1322,  1323,  1325,
    1329,  1324,  1330,  1335,  1413,  1336,  1337,  1338,  1340,  1341,
    1347,  1345,  1346,  1373,  1509,  1376,  1375,  1317,  1382,  1379,
    1380,  1381,  1390,  1396,  1401,  1402,  1403,  1404,  1570,  1434,
    1408,  1409,  1410,  1415,  1419,  1420,  1485,  1414,  1421,  1371,
    1422,  1423,  1424,  1425,  1426,  1427,  1439,  1442,  1446,  1465,
    1470,  1475,  1105,  1476,  1481,  1482,  1484,  1614,  1494,  1490,
    1468,  1469,  1472,  1495,  1496,  1497,  1504,  1502,  1499,  1500,
    1491,  1512,  1505,  1506,  1522,  1518,  1540,  1493,  1507,  1510,
    1542,  1543,  1513,  1515,  1516,  1539,  1517,  1544,  1541,  1550,
    1551,  1601,  1596,  1552,  1555,  1556,  1557,  1560,  1561,  1562,
    1536,  1563,  1574,  1599,  1566,  1571,  1558,  1564,  1572,  1581,
    1582,  1583,  1584,   132,  1370,  1585,  1589,  1593,  1569,  1595,
    1597,  1598,  1609,  1610,  1612,  1613,  1615,  1369,  1616,  1618,
    1617,  1372,  1619,  1621,  1622,  1628,  1629,  1635,  1567,  1630,
    1234,  1632,  1634,  1600,  1636,  1602,  1638,  1620,  1603,  1639,
    1640,  1633,  1642,  1643,  1644,  1645,  1604,  1605,  1648,  1641,
    1649,  1650,  1651,  1655,  1656,  1658,  1606,  1607,  1659,  1660,
    1663,  1664,  1665,  1666,  1211,  1299,   745,  1608,  1239,   995,
     248,  1006,  1467,  1466,  1018,   969,  1243,  1247,  1406,   620,
    1049,   167,   750,  1068,  1273,  1177,  1293,   951,  1081,     0,
     657,  1654,     0,  1623,   611,     0,  1440,     0,   190,  1624,
       0,  1627,     0,     0,  1498,     0,     0,     0,     0,  1631,
       0,     0,     0,  1647,     0,     0,     0,     0,  1652,  1653,
    1662,     0,     0,     0,  1657,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1661,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1538,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     399,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1626,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1637,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1611,     0,     0,     0,   190,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1625
};

static const yytype_int16 yycheck[] =
{
     126,   404,   189,   347,   596,   500,   883,   351,   950,   353,
    1129,     4,     4,     4,    30,     4,   257,    30,     6,     6,
     962,     6,     4,   367,     4,     6,   370,     6,     6,     6,
      30,     6,     6,    30,     6,     6,   380,     6,     6,    58,
      30,    30,     6,    71,    28,    28,    58,     1,    58,    48,
      96,    58,    19,     7,    91,     9,    10,    11,    12,    13,
      14,   166,    16,    17,    18,    19,    20,    96,   159,    23,
      24,   180,    96,    96,    96,    29,    96,    31,    96,    30,
      21,     4,     0,   104,    31,    96,     8,   108,    42,    43,
      28,    96,    98,    99,   175,   175,   102,   103,    48,     4,
      54,    96,    96,    30,     4,    55,    60,    30,    36,    37,
      96,   220,   221,   222,   223,    69,    76,    77,    78,    79,
      96,    75,    49,    50,    52,   469,    80,  1246,   233,   266,
     533,    59,   125,    76,    77,    78,    79,  1079,   266,    93,
      45,   269,    96,    97,  1086,   266,   174,    96,   269,  1091,
     266,    96,    96,   107,  1063,    96,   110,  1066,   157,   158,
     266,   167,    96,    96,   567,   660,    96,   174,   175,   664,
      96,   666,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,   263,   263,   151,   152,   175,   224,   225,   226,
     126,   219,     6,   147,     3,     4,   104,     6,    94,   188,
     108,   109,    76,    77,    78,    79,   160,   161,   162,   156,
     217,   218,   219,   167,  1169,     3,     4,    91,     6,   172,
     173,   267,  1177,   177,   243,   179,     3,     4,   166,     6,
     242,   167,   242,   216,     6,   242,   264,     6,   267,   255,
     167,   260,   255,   267,   267,   267,     6,   267,     6,   267,
     170,   171,   268,   266,   261,   255,   267,     6,   255,   187,
     386,   215,   267,   217,   218,   255,   266,   148,   149,   266,
     269,     6,   267,   267,   258,   266,   184,     6,   271,   271,
     269,   267,     6,   271,   271,     6,   271,   269,     6,   269,
     271,   267,   271,   271,   271,   698,   271,   271,   270,   270,
     254,   270,   270,  1245,   255,   269,   166,   167,     6,   247,
     270,   249,   272,   100,   101,   441,   176,     9,   267,   445,
     258,   259,   267,   267,   251,   252,   253,   270,   255,   272,
      41,     1,   259,   267,   267,    36,    37,   267,    39,     9,
       6,   267,   266,   687,   268,     7,     6,     9,    10,    11,
      12,    13,    14,     6,    16,    17,    18,    19,    20,    29,
      71,    23,    24,     3,   163,   164,   165,    29,     6,    31,
     169,    41,     3,   499,   500,  1317,     6,   503,   619,   620,
      42,    43,   266,   509,   268,   245,  1421,  1422,  1423,    59,
       6,  1426,    54,    81,    82,    83,    84,   266,    60,   268,
      70,    76,    77,    78,    79,   265,    76,    69,   220,   221,
     222,   223,     6,    75,    87,    88,    89,    90,    80,   118,
     119,   547,   150,   151,   152,   153,    96,     6,  1547,   555,
     266,    93,   268,     6,    96,    97,    72,    73,    74,   565,
     566,    28,     6,     3,     4,   107,     6,   267,   110,    36,
      37,   266,    39,   268,     4,   166,   167,   120,   121,   954,
      47,    87,    88,    89,    90,   591,   268,   269,   655,   112,
     113,   114,   115,   116,   117,    87,    88,    89,    90,   167,
     236,   237,   238,   239,   266,   147,     3,     4,   266,     6,
     268,     3,     4,   181,     6,   266,     4,   268,   160,   161,
     162,   266,    95,   268,   174,   167,     4,     3,     4,    25,
       6,     4,   182,   183,   266,   177,   268,   179,     3,     4,
     266,     6,   268,     4,   167,  1020,   266,   266,   268,   268,
     266,     4,   268,   659,   660,    98,    99,   663,   664,   780,
     666,   266,   269,   268,   232,    61,    62,    63,    64,    65,
      66,    67,    68,   215,   266,   217,   218,   144,   145,   228,
       6,   230,   688,   689,   266,     4,   268,    25,   154,   155,
       4,  1448,   268,   269,  1226,  1227,   266,   266,     6,   705,
     167,   266,   266,   266,   266,   266,   266,    25,   991,   266,
     266,     6,   254,     6,    30,   266,  1538,    33,    34,   266,
      36,    37,   189,   190,   191,   192,   193,   194,   195,   196,
      46,    47,    48,    49,   266,    51,    52,   743,   266,    55,
      56,    57,    58,    61,    62,    63,    64,    65,    66,    67,
      68,   266,   819,   266,   266,   822,     6,   266,   266,   826,
      20,   266,   266,   266,   831,   266,  1141,   234,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   266,   246,
      19,    33,    34,   250,    36,    37,    10,    25,   255,   256,
     257,   258,    43,    42,    46,    47,    48,    49,   235,    51,
      52,    31,    69,    93,    75,    80,    54,    59,    97,   269,
     147,   107,   110,   179,   215,  1637,    61,    62,    63,    64,
      65,    66,    67,    68,   217,   262,   197,   198,   199,   200,
     201,   202,   203,   204,    61,    62,    63,    64,    65,    66,
      67,    68,   535,   536,   537,   538,   539,   540,   541,   542,
     166,   167,    28,   859,   218,     4,   254,    48,   864,   175,
     266,     6,     6,     6,     6,   162,   266,    22,     4,   185,
     122,   123,   124,     6,     4,     4,     4,     4,     4,  1351,
     269,     4,   269,     4,   890,   952,   266,     4,   955,     4,
      60,   270,     6,   270,     6,   270,   270,     4,   904,     4,
       4,     4,    27,     4,     4,     4,     4,     4,   268,    27,
       6,   227,     4,   229,   166,   167,     4,   269,    28,     4,
       4,     4,     4,     4,     4,   111,     4,     4,     4,     4,
       4,    25,    25,     6,   186,   187,   266,    76,     6,   255,
       6,     6,   258,     6,     4,     6,     4,     6,   954,     6,
       6,     6,     6,     6,     6,     6,     4,  1240,   268,     4,
       6,     4,     4,   268,   266,     6,     6,     3,   175,     4,
       4,     4,   266,     4,   266,   227,     4,   229,   266,   266,
     266,   266,   266,   266,   266,  1209,     4,     4,   269,     4,
       4,     4,    71,     4,     6,     4,     4,     4,     4,  1374,
       4,     4,     4,     4,     4,     4,  1012,     4,     4,     4,
     269,   266,     6,     6,  1020,     6,     6,     4,     4,     4,
      92,     4,     6,   270,     6,   266,     4,     4,  1034,     4,
       4,     4,     4,    25,   105,   146,     4,    26,    26,     6,
     266,     4,     6,     4,     4,     4,     4,    28,    53,     6,
       6,     6,     4,     4,     4,    53,   231,     6,     6,     6,
       6,     6,     4,     4,     4,     4,     4,     4,   268,   178,
       6,     6,     6,  1140,     6,     6,     6,     6,     6,     6,
       6,     6,     4,   270,     4,     6,  1369,     6,     6,     6,
       4,     4,   266,     4,     6,   105,   166,     4,   266,     4,
       4,     4,     4,    27,    27,     6,     4,     4,     4,     4,
      26,   269,     6,     6,     6,     4,     4,   146,     4,     6,
       4,     4,   269,   269,   104,     4,   105,     4,     6,  1135,
     270,     6,     6,     6,     6,  1141,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,    56,     6,     6,   268,
     166,     6,   268,     6,     6,   268,     6,     4,     4,     4,
     266,   269,     4,     6,   266,     6,     6,     6,     4,     6,
      27,     6,     6,     6,  1457,     4,   240,  1244,     4,   270,
     270,   270,     4,     6,     4,     4,     4,     4,    26,     3,
       6,     6,     4,     6,     4,     6,    28,   266,     6,   175,
       6,     6,     6,     6,     6,     6,     6,     6,     4,     6,
       4,     6,   969,     6,     6,     4,     4,  1592,     4,    28,
     270,   270,   269,     4,     4,     4,   270,     4,     6,   268,
      28,     6,   270,   270,     4,   106,   126,    28,   270,   269,
       6,    25,   269,   269,   269,   268,   270,     6,   270,     4,
       4,   270,    26,     6,     6,     6,     6,     4,     4,     4,
     175,     4,  1545,    25,     6,     6,   175,   175,     6,     4,
       4,     4,     4,    29,  1295,     6,     6,     6,   248,     6,
       6,     6,     6,     6,     6,     6,     6,  1293,     4,     6,
      27,  1297,     6,     6,     6,     4,     6,     4,   269,    27,
     269,     6,     6,   269,     4,   270,     4,    26,   270,     6,
       6,    26,     6,     6,     4,     4,   270,   270,     6,    27,
       6,    27,     6,     6,     6,     6,   270,   270,     6,     6,
       6,     6,     6,     6,  1109,  1218,   593,   270,  1130,   866,
     164,   880,  1400,  1399,   896,   838,  1139,  1144,  1328,   473,
     917,    95,   598,   943,  1176,  1067,  1209,   820,   953,    -1,
     501,   241,    -1,   269,   459,    -1,  1372,    -1,  1374,   269,
      -1,   270,    -1,    -1,  1441,    -1,    -1,    -1,    -1,   270,
      -1,    -1,    -1,   268,    -1,    -1,    -1,    -1,   270,   270,
     244,    -1,    -1,    -1,   270,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   268,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1498,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     306,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1614,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1626,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1588,    -1,    -1,    -1,  1592,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1611
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
     271,     4,     4,     4,     4,     6,    58,   242,   345,     6,
      58,   242,   346,     6,   389,     6,    26,     6,     6,    25,
     269,   270,   270,   270,   270,   270,   270,   270,   270,     6,
       6,   395,     6,     6,   392,     6,     4,    27,     6,     6,
      26,     6,     6,   269,   269,   395,   393,   270,     4,     6,
      27,   270,     6,    26,     6,     4,     4,   393,     4,     6,
       6,    27,     6,     6,     4,     4,   394,   268,     6,     6,
      27,     6,   270,   270,   241,     6,     6,   270,     6,     6,
       6,   268,   244,     6,     6,     6,     6
};

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
     505,   505,   505,   506,   506,   506,   506,   507,   508,   508,
     509,   509,   510,   511,   510,   512,   513,   513,   514,   514,
     515,   516,   516,   516,   516,   516,   516,   516,   516,   516,
     516,   516,   516,   517,   518,   518,   519,   520,   520,   521,
     521,   521,   521,   522,   523,   522,   524,   525,   522,   522,
     522,   526,   527,   522,   528,   522,   529,   530,   522,   522,
     522,   522,   531,   522,   522,   522,   522,   522,   522,   532,
     522,   522,   533,   533,   533,   533,   533,   533,   533,   533,
     533,   534,   535,   534,   537,   536,   539,   538,   541,   542,
     540,   543,   543,   544,   544,   544,   545,   545,   546,   546,
     547,   547,   547,   547,   548,   548,   550,   549,   552,   553,
     551,   554,   555,   556,   557,   558,   559,   559,   560,   562,
     561,   563,   563,   564,   565,   565,   566,   567,   566,   568,
     566,   566,   569,   569,   570,   570,   571,   571,   571,   572,
     572,   573,   573,   573,   574,   575,   576,   577,   578,   579,
     579,   580,   580,   581,   583,   582,   584,   582,   582,   582,
     585,   585,   585,   587,   586,   588,   588,   589,   589,   590,
     590,   590,   590,   591,   592,   593,   594,   595,   595,   596,
     598,   597,   599,   599,   600,   600,   602,   601,   603,   601,
     604,   601,   605,   601,   606,   601,   607,   601,   601,   608,
     608,   608,   609,   609,   611,   610,   612,   612,   612,   612,
     613,   613,   615,   614,   616,   616,   616,   616,   617,   617,
     618,   619,   620,   621,   621,   622,   624,   623,   625,   625,
     626,   626,   626,   627,   628,   626,   626,   630,   631,   629,
     632,   633,   632,   634,   634,   635,   635,   636,   637,   638,
     639,   639,   641,   642,   640,   643,   643,   644,   644,   644,
     644,   645,   645,   647,   646,   648,   646,   649,   649,   651,
     650,   652,   650,   653,   654,   655,   655,   657,   658,   656,
     659,   656,   660,   656,   656,   662,   663,   661,   664,   661,
     665,   666,   667,   668,   668,   669,   671,   670,   672,   672,
     673,   673,   673,   674,   674,   674,   675,   675,   677,   676,
     678,   676,   679,   676,   680,   676,   681,   676,   682,   676,
     676,   684,   683,   685,   685,   686,   686,   687,   687,   688,
     688,   689,   689,   689,   689,   690,   690,   691,   691,   691,
     691,   692,   693,   693,   695,   694,   696,   696,   697,   697,
     699,   698,   700,   698,   701,   701,   701,   701,   702,   703,
     704,   704,   705,   706,   706,   708,   709,   707,   710,   710,
     712,   711,   713,   713,   714,   714,   714,   715,   716,   717,
     718,   718,   719,   721,   722,   720,   723,   720,   724,   724,
     725,   725,   725,   725,   726,   728,   727,   727,   727,   727,
     727,   729,   729,   731,   730,   730,   730,   730,   732,   732,
     733,   734,   733,   735,   736,   737,   738,   738,   739,   741,
     742,   740,   743,   743,   744,   745,   744,   746,   747,   748,
     749,   749,   750,   751,   752,   750,   754,   755,   753,   756,
     756,   757,   758,   757,   759,   759,   760,   760,   761,   762,
     763,   763,   764,   764,   765,   766,   767,   768,   769,   770,
     771,   771,   773,   774,   772,   775,   775,   776,   777,   778,
     776,   779,   776,   780,   776,   781,   776,   776,   782,   782,
     783,   783,   783,   785,   784,   786,   786,   787,   787,   787,
     788,   789,   790,   791,   791,   793,   792
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
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
       5,     5,     5,     4,     4,     4,     4,     6,     0,     2,
       1,     1,     1,     0,     3,     2,     0,     2,     3,     3,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     0,     2,     3,     0,     2,     1,
       1,     1,     1,     2,     0,     4,     0,     0,     6,     3,
       3,     0,     0,     9,     0,     6,     0,     0,     8,     3,
       2,     3,     0,     4,     3,     3,     3,     3,     3,     0,
       4,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     0,     2,     0,     5,     0,     4,     0,     0,
       7,     1,     2,     2,     2,     2,     0,     3,     0,     3,
       1,     1,     1,     1,     1,     2,     0,     3,     0,     0,
       7,     1,     3,     2,     3,     3,     0,     2,     4,     0,
       3,     0,     2,     1,     0,     2,     3,     0,     4,     0,
       4,     1,     2,     1,     0,     2,     2,     2,     2,     0,
       2,     2,     2,     2,     2,     3,     3,     3,     3,     0,
       2,     1,     1,     4,     0,     3,     0,     6,     4,     4,
       1,     2,     3,     0,     8,     0,     1,     0,     2,     3,
       3,     3,     3,     2,     2,     3,     3,     0,     2,     3,
       0,     3,     0,     2,     0,     1,     0,     5,     0,     4,
       0,     4,     0,     5,     0,     4,     0,     5,     1,     0,
       4,     8,     0,     2,     0,     3,     0,     4,     8,    12,
       0,     2,     0,     3,     0,     4,     8,    12,     0,     2,
       2,     3,     3,     0,     2,     3,     0,     6,     0,     2,
       5,     5,     3,     0,     0,     6,     1,     0,     0,     6,
       0,     0,     3,     0,     2,     1,     1,     2,     4,     3,
       0,     2,     0,     0,     8,     1,     1,     1,     2,     2,
       2,     0,     2,     0,     4,     0,     4,     0,     2,     0,
       5,     0,     5,     3,     3,     0,     2,     0,     0,    10,
       0,     6,     0,     6,     3,     0,     0,     6,     0,     3,
       2,     3,     3,     0,     2,     3,     0,     4,     0,     3,
       0,     1,     1,     0,     1,     1,     0,     2,     0,     7,
       0,     6,     0,     5,     0,     7,     0,     6,     0,     5,
       1,     0,     4,     0,     2,     3,     3,     0,     2,     0,
       2,     2,     2,     2,     2,     1,     2,     3,     3,     3,
       3,     2,     0,     2,     0,     6,     0,     2,     0,     2,
       0,     3,     0,     3,     1,     1,     1,     1,     3,     3,
       0,     1,     2,     0,     2,     0,     0,     7,     0,     2,
       0,     4,     0,     2,     2,     2,     2,     3,     3,     2,
       0,     2,     4,     0,     0,     6,     0,     4,     0,     2,
       3,     3,     1,     1,     3,     0,     4,     2,     2,     2,
       2,     0,     2,     0,     4,     2,     2,     3,     0,     2,
       3,     0,     6,     3,     3,     2,     0,     2,     3,     0,
       0,     6,     0,     2,     3,     0,     6,     3,     3,     2,
       0,     2,     3,     0,     0,     8,     0,     0,     7,     0,
       2,     3,     0,     6,     0,     2,     1,     1,     2,     2,
       0,     2,     1,     1,     2,     3,     3,     4,     3,     2,
       0,     2,     0,     0,     6,     0,     2,     2,     0,     0,
       8,     0,     4,     0,     4,     0,     5,     1,     0,     2,
       2,     2,     2,     0,     4,     0,     2,     2,     2,     2,
       3,     3,     2,     0,     2,     0,     8
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

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
#line 319 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 3038 "def.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 320 "def.y" /* yacc.c:1646  */
    {
        defData->VersionNum = convert_defname2num((yyvsp[-1].string));
        if (defData->VersionNum > CURRENT_VERSION) {
          char temp[300];
          sprintf(temp,
          "The execution has been stopped because the DEF parser %.1f does not support DEF file with version %s.\nUpdate your DEF file to version 5.8 or earlier.",
                  CURRENT_VERSION, (yyvsp[-1].string));
          defError(6503, temp);
          return 1;
        }
        if (defCallbacks->VersionStrCbk) {
          CALLBACK(defCallbacks->VersionStrCbk, defrVersionStrCbkType, (yyvsp[-1].string));
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
    }
#line 3067 "def.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 347 "def.y" /* yacc.c:1646  */
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
      }
#line 3084 "def.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 360 "def.y" /* yacc.c:1646  */
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
      }
#line 3105 "def.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 400 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1; }
#line 3111 "def.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 401 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->DesignCbk)
              CALLBACK(defCallbacks->DesignCbk, defrDesignStartCbkType, (yyvsp[-1].string));
            defData->hasDes = 1;
          }
#line 3121 "def.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 408 "def.y" /* yacc.c:1646  */
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
          }
#line 3149 "def.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 432 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 3155 "def.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 433 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->TechnologyCbk)
              CALLBACK(defCallbacks->TechnologyCbk, defrTechNameCbkType, (yyvsp[-1].string));
          }
#line 3164 "def.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 438 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 3170 "def.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 439 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->ArrayNameCbk)
              CALLBACK(defCallbacks->ArrayNameCbk, defrArrayNameCbkType, (yyvsp[-1].string));
          }
#line 3179 "def.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 444 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 3185 "def.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 445 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->FloorPlanNameCbk)
              CALLBACK(defCallbacks->FloorPlanNameCbk, defrFloorPlanNameCbkType, (yyvsp[-1].string));
          }
#line 3194 "def.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 451 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->HistoryCbk)
              CALLBACK(defCallbacks->HistoryCbk, defrHistoryCbkType, &defData->History_text[0]);
          }
#line 3203 "def.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 457 "def.y" /* yacc.c:1646  */
    {
            defData->parsing_property = 1;
            defData->defInPropDef = 1;     // set flag as inside propertydefinitions 
            if (defCallbacks->PropDefStartCbk)
              CALLBACK(defCallbacks->PropDefStartCbk, defrPropDefStartCbkType, 0);
          }
#line 3214 "def.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 464 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->PropDefEndCbk)
              CALLBACK(defCallbacks->PropDefEndCbk, defrPropDefEndCbkType, 0);
            defData->real_num = 0;     // just want to make sure it is reset 
            defData->parsing_property = 0;
            defData->defInPropDef = 0;     // reset flag 
          }
#line 3226 "def.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 474 "def.y" /* yacc.c:1646  */
    { }
#line 3232 "def.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 476 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1; defData->Prop.clear(); }
#line 3238 "def.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 478 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("design", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->DesignProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3250 "def.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 485 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3256 "def.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 487 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("net", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->NetProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3268 "def.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 494 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3274 "def.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 496 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("specialnet", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->SNetProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3286 "def.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 503 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3292 "def.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 505 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("region", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->RegionProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3304 "def.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 512 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3310 "def.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 514 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("group", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->GroupProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3322 "def.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 521 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3328 "def.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 523 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("component", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->CompProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3340 "def.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 530 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3346 "def.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 532 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("row", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->RowProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3358 "def.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 541 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3364 "def.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 543 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("componentpin", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->CompPinProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3376 "def.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 551 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3382 "def.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 553 "def.y" /* yacc.c:1646  */
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
                  defData->Prop.setPropType("nondefaultrule", (yyvsp[-2].string));
                  CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
                }
                defSettings->NDefProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
              }
            }
#line 3405 "def.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 571 "def.y" /* yacc.c:1646  */
    { yyerrok; yyclearin;}
#line 3411 "def.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 573 "def.y" /* yacc.c:1646  */
    { defData->real_num = 0; }
#line 3417 "def.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 574 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) defData->Prop.setPropInteger();
              defData->defPropDefType = 'I';
            }
#line 3426 "def.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 578 "def.y" /* yacc.c:1646  */
    { defData->real_num = 1; }
#line 3432 "def.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 579 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) defData->Prop.setPropReal();
              defData->defPropDefType = 'R';
              defData->real_num = 0;
            }
#line 3442 "def.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 585 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) defData->Prop.setPropString();
              defData->defPropDefType = 'S';
            }
#line 3451 "def.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 590 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) defData->Prop.setPropQString((yyvsp[0].string));
              defData->defPropDefType = 'Q';
            }
#line 3460 "def.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 595 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->PropCbk) defData->Prop.setPropNameMapString((yyvsp[0].string));
              defData->defPropDefType = 'S';
            }
#line 3469 "def.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 602 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PropCbk) defData->Prop.setNumber((yyvsp[0].dval)); }
#line 3475 "def.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 605 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->UnitsCbk) {
              if (defValidNum((int)(yyvsp[-1].dval)))
                CALLBACK(defCallbacks->UnitsCbk,  defrUnitsCbkType, (yyvsp[-1].dval));
            }
          }
#line 3486 "def.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 613 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->DividerCbk)
              CALLBACK(defCallbacks->DividerCbk, defrDividerCbkType, (yyvsp[-1].string));
            defData->hasDivChar = 1;
          }
#line 3496 "def.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 620 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->BusBitCbk)
              CALLBACK(defCallbacks->BusBitCbk, defrBusBitCbkType, (yyvsp[-1].string));
            defData->hasBusBit = 1;
          }
#line 3506 "def.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 626 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1;defData->no_num = 1; }
#line 3512 "def.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 628 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->CanplaceCbk) {
                defData->Canplace.setName((yyvsp[-11].string));
                defData->Canplace.setLocation((yyvsp[-10].dval),(yyvsp[-9].dval));
                defData->Canplace.setOrient((yyvsp[-8].integer));
                defData->Canplace.setDo((yyvsp[-6].dval),(yyvsp[-4].dval),(yyvsp[-2].dval),(yyvsp[-1].dval));
                CALLBACK(defCallbacks->CanplaceCbk, defrCanplaceCbkType,
                &(defData->Canplace));
              }
            }
#line 3527 "def.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 638 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1;defData->no_num = 1; }
#line 3533 "def.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 640 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->CannotOccupyCbk) {
                defData->CannotOccupy.setName((yyvsp[-11].string));
                defData->CannotOccupy.setLocation((yyvsp[-10].dval),(yyvsp[-9].dval));
                defData->CannotOccupy.setOrient((yyvsp[-8].integer));
                defData->CannotOccupy.setDo((yyvsp[-6].dval),(yyvsp[-4].dval),(yyvsp[-2].dval),(yyvsp[-1].dval));
                CALLBACK(defCallbacks->CannotOccupyCbk, defrCannotOccupyCbkType,
                        &(defData->CannotOccupy));
              }
            }
#line 3548 "def.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 651 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 0;}
#line 3554 "def.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 652 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 1;}
#line 3560 "def.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 653 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 2;}
#line 3566 "def.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 654 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 3;}
#line 3572 "def.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 655 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 4;}
#line 3578 "def.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 656 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 5;}
#line 3584 "def.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 657 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 6;}
#line 3590 "def.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 658 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 7;}
#line 3596 "def.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 661 "def.y" /* yacc.c:1646  */
    {
            defData->Geometries.Reset();
          }
#line 3604 "def.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 665 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->DieAreaCbk) {
               defData->DieArea.addPoint(&defData->Geometries);
               CALLBACK(defCallbacks->DieAreaCbk, defrDieAreaCbkType, &(defData->DieArea));
            }
          }
#line 3615 "def.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 674 "def.y" /* yacc.c:1646  */
    { }
#line 3621 "def.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 677 "def.y" /* yacc.c:1646  */
    {
          if (defData->VersionNum < 5.4) {
             if (defCallbacks->DefaultCapCbk)
                CALLBACK(defCallbacks->DefaultCapCbk, defrDefaultCapCbkType, ROUND((yyvsp[0].dval)));
          } else {
             if (defCallbacks->DefaultCapCbk) // write error only if cbk is set 
                if (defData->defaultCapWarnings++ < defSettings->DefaultCapWarnings)
                   defWarning(7017, "The DEFAULTCAP statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
#line 3636 "def.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 693 "def.y" /* yacc.c:1646  */
    {
            if (defData->VersionNum < 5.4) {
              if (defCallbacks->PinCapCbk) {
                defData->PinCap.setPin(ROUND((yyvsp[-3].dval)));
                defData->PinCap.setCap((yyvsp[-1].dval));
                CALLBACK(defCallbacks->PinCapCbk, defrPinCapCbkType, &(defData->PinCap));
              }
            }
          }
#line 3650 "def.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 704 "def.y" /* yacc.c:1646  */
    { }
#line 3656 "def.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 707 "def.y" /* yacc.c:1646  */
    { }
#line 3662 "def.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 710 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->StartPinsCbk)
              CALLBACK(defCallbacks->StartPinsCbk, defrStartPinsCbkType, ROUND((yyvsp[-1].dval)));
          }
#line 3671 "def.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 719 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1; }
#line 3677 "def.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 720 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1; }
#line 3683 "def.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 721 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              defData->Pin.Setup((yyvsp[-4].string), (yyvsp[0].string));
            }
            defData->hasPort = 0;
          }
#line 3694 "def.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 728 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->PinCbk)
              CALLBACK(defCallbacks->PinCbk, defrPinCbkType, &defData->Pin);
          }
#line 3703 "def.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 737 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->PinCbk)
              defData->Pin.setSpecial();
          }
#line 3712 "def.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 743 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->PinExtCbk)
              CALLBACK(defCallbacks->PinExtCbk, defrPinExtCbkType, &defData->History_text[0]);
          }
#line 3721 "def.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 749 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.setDirection((yyvsp[0].string));
          }
#line 3730 "def.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 755 "def.y" /* yacc.c:1646  */
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
                defData->Pin.setNetExpr((yyvsp[0].string));

            }
          }
#line 3754 "def.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 775 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 3760 "def.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 776 "def.y" /* yacc.c:1646  */
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
                defData->Pin.setSupplySens((yyvsp[0].string));
            }
          }
#line 3783 "def.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 795 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 3789 "def.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 796 "def.y" /* yacc.c:1646  */
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
                defData->Pin.setGroundSens((yyvsp[0].string));
            }
          }
#line 3812 "def.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 816 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) defData->Pin.setUse((yyvsp[0].string));
          }
#line 3820 "def.tab.c" /* yacc.c:1646  */
    break;

  case 135:
#line 820 "def.y" /* yacc.c:1646  */
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
          }
#line 3844 "def.tab.c" /* yacc.c:1646  */
    break;

  case 136:
#line 840 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 3850 "def.tab.c" /* yacc.c:1646  */
    break;

  case 137:
#line 841 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.addPortLayer((yyvsp[0].string));
              else
                 defData->Pin.addLayer((yyvsp[0].string));
            }
          }
#line 3863 "def.tab.c" /* yacc.c:1646  */
    break;

  case 138:
#line 850 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.addPortLayerPts((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
              else
                 defData->Pin.addLayerPts((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
            }
          }
#line 3876 "def.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 859 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 3882 "def.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 860 "def.y" /* yacc.c:1646  */
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
                   defData->Pin.addPortPolygon((yyvsp[0].string));
                else
                   defData->Pin.addPolygon((yyvsp[0].string));
              }
            }
            
            defData->Geometries.Reset();            
          }
#line 3911 "def.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 885 "def.y" /* yacc.c:1646  */
    {
            if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolygonPts(&defData->Geometries);
                else
                   defData->Pin.addPolygonPts(&defData->Geometries);
              }
            }
          }
#line 3926 "def.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 895 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 3932 "def.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 896 "def.y" /* yacc.c:1646  */
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
                   defData->Pin.addPortVia((yyvsp[-5].string), (int)(yyvsp[-2].dval),
                                               (int)(yyvsp[-1].dval), (yyvsp[-4].integer));
                else
                   defData->Pin.addVia((yyvsp[-5].string), (int)(yyvsp[-2].dval),
                                               (int)(yyvsp[-1].dval), (yyvsp[-4].integer));
              }
            }
          }
#line 3961 "def.tab.c" /* yacc.c:1646  */
    break;

  case 144:
#line 922 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.setPortPlacement((yyvsp[-2].integer), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
              else
                 defData->Pin.setPlacement((yyvsp[-2].integer), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
            }
          }
#line 3974 "def.tab.c" /* yacc.c:1646  */
    break;

  case 145:
#line 933 "def.y" /* yacc.c:1646  */
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
              defData->Pin.addAPinPartialMetalArea((int)(yyvsp[-1].dval), (yyvsp[0].string)); 
          }
#line 3996 "def.tab.c" /* yacc.c:1646  */
    break;

  case 146:
#line 951 "def.y" /* yacc.c:1646  */
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
              defData->Pin.addAPinPartialMetalSideArea((int)(yyvsp[-1].dval), (yyvsp[0].string)); 
          }
#line 4018 "def.tab.c" /* yacc.c:1646  */
    break;

  case 147:
#line 969 "def.y" /* yacc.c:1646  */
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
                defData->Pin.addAPinGateArea((int)(yyvsp[-1].dval), (yyvsp[0].string)); 
            }
#line 4040 "def.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 987 "def.y" /* yacc.c:1646  */
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
              defData->Pin.addAPinDiffArea((int)(yyvsp[-1].dval), (yyvsp[0].string)); 
          }
#line 4062 "def.tab.c" /* yacc.c:1646  */
    break;

  case 149:
#line 1004 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=1;}
#line 4068 "def.tab.c" /* yacc.c:1646  */
    break;

  case 150:
#line 1005 "def.y" /* yacc.c:1646  */
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
              defData->Pin.addAPinMaxAreaCar((int)(yyvsp[-3].dval), (yyvsp[0].string)); 
          }
#line 4090 "def.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 1022 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=1;}
#line 4096 "def.tab.c" /* yacc.c:1646  */
    break;

  case 152:
#line 1024 "def.y" /* yacc.c:1646  */
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
              defData->Pin.addAPinMaxSideAreaCar((int)(yyvsp[-3].dval), (yyvsp[0].string)); 
          }
#line 4118 "def.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 1042 "def.y" /* yacc.c:1646  */
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
              defData->Pin.addAPinPartialCutArea((int)(yyvsp[-1].dval), (yyvsp[0].string)); 
          }
#line 4140 "def.tab.c" /* yacc.c:1646  */
    break;

  case 154:
#line 1059 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=1;}
#line 4146 "def.tab.c" /* yacc.c:1646  */
    break;

  case 155:
#line 1060 "def.y" /* yacc.c:1646  */
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
              defData->Pin.addAPinMaxCutCar((int)(yyvsp[-3].dval), (yyvsp[0].string)); 
          }
#line 4168 "def.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 1078 "def.y" /* yacc.c:1646  */
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
          }
#line 4188 "def.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 1096 "def.y" /* yacc.c:1646  */
    { 
           if (validateMaskInput((int)(yyvsp[0].dval), defData->pinWarnings, defSettings->PinWarnings)) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortLayerMask((int)(yyvsp[0].dval));
                else
                   defData->Pin.addLayerMask((int)(yyvsp[0].dval));
              }
           }
         }
#line 4203 "def.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 1109 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 0; }
#line 4209 "def.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 1111 "def.y" /* yacc.c:1646  */
    { 
           if (validateMaskInput((int)(yyvsp[0].dval), defData->pinWarnings, defSettings->PinWarnings)) {
             (yyval.integer) = (yyvsp[0].dval);
           }
         }
#line 4219 "def.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 1119 "def.y" /* yacc.c:1646  */
    { 
           if (validateMaskInput((int)(yyvsp[0].dval), defData->pinWarnings, defSettings->PinWarnings)) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolyMask((int)(yyvsp[0].dval));
                else
                   defData->Pin.addPolyMask((int)(yyvsp[0].dval));
              }
           }
         }
#line 4234 "def.tab.c" /* yacc.c:1646  */
    break;

  case 164:
#line 1133 "def.y" /* yacc.c:1646  */
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
                   defData->Pin.addPortLayerSpacing((int)(yyvsp[0].dval));
                else
                   defData->Pin.addLayerSpacing((int)(yyvsp[0].dval));
              }
            }
          }
#line 4261 "def.tab.c" /* yacc.c:1646  */
    break;

  case 165:
#line 1156 "def.y" /* yacc.c:1646  */
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
                   defData->Pin.addPortLayerDesignRuleWidth((int)(yyvsp[0].dval));
                else
                   defData->Pin.addLayerDesignRuleWidth((int)(yyvsp[0].dval));
              }
            }
          }
#line 4288 "def.tab.c" /* yacc.c:1646  */
    break;

  case 167:
#line 1181 "def.y" /* yacc.c:1646  */
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
                   defData->Pin.addPortPolySpacing((int)(yyvsp[0].dval));
                else
                   defData->Pin.addPolySpacing((int)(yyvsp[0].dval));
              }
            }
          }
#line 4315 "def.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 1204 "def.y" /* yacc.c:1646  */
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
                   defData->Pin.addPortPolyDesignRuleWidth((int)(yyvsp[0].dval));
                else
                   defData->Pin.addPolyDesignRuleWidth((int)(yyvsp[0].dval));
              }
            }
          }
#line 4342 "def.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 1228 "def.y" /* yacc.c:1646  */
    { defData->aOxide = 1;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
#line 4351 "def.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 1233 "def.y" /* yacc.c:1646  */
    { defData->aOxide = 2;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
#line 4360 "def.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 1238 "def.y" /* yacc.c:1646  */
    { defData->aOxide = 3;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
#line 4369 "def.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 1243 "def.y" /* yacc.c:1646  */
    { defData->aOxide = 4;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
#line 4378 "def.tab.c" /* yacc.c:1646  */
    break;

  case 173:
#line 1249 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"SIGNAL"; }
#line 4384 "def.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 1251 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"POWER"; }
#line 4390 "def.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 1253 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"GROUND"; }
#line 4396 "def.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 1255 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"CLOCK"; }
#line 4402 "def.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 1257 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"TIEOFF"; }
#line 4408 "def.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 1259 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"ANALOG"; }
#line 4414 "def.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 1261 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"SCAN"; }
#line 4420 "def.tab.c" /* yacc.c:1646  */
    break;

  case 180:
#line 1263 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"RESET"; }
#line 4426 "def.tab.c" /* yacc.c:1646  */
    break;

  case 181:
#line 1267 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)""; }
#line 4432 "def.tab.c" /* yacc.c:1646  */
    break;

  case 182:
#line 1268 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=1;}
#line 4438 "def.tab.c" /* yacc.c:1646  */
    break;

  case 183:
#line 1269 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string); }
#line 4444 "def.tab.c" /* yacc.c:1646  */
    break;

  case 184:
#line 1272 "def.y" /* yacc.c:1646  */
    { 
          if (defCallbacks->PinEndCbk)
            CALLBACK(defCallbacks->PinEndCbk, defrPinEndCbkType, 0);
        }
#line 4453 "def.tab.c" /* yacc.c:1646  */
    break;

  case 185:
#line 1277 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 2; defData->no_num = 2; }
#line 4459 "def.tab.c" /* yacc.c:1646  */
    break;

  case 186:
#line 1279 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RowCbk) {
            defData->rowName = (yyvsp[-4].string);
            defData->Row.setup((yyvsp[-4].string), (yyvsp[-3].string), (yyvsp[-2].dval), (yyvsp[-1].dval), (yyvsp[0].integer));
          }
        }
#line 4470 "def.tab.c" /* yacc.c:1646  */
    break;

  case 187:
#line 1287 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RowCbk) 
            CALLBACK(defCallbacks->RowCbk, defrRowCbkType, &defData->Row);
        }
#line 4479 "def.tab.c" /* yacc.c:1646  */
    break;

  case 188:
#line 1293 "def.y" /* yacc.c:1646  */
    {
          if (defData->VersionNum < 5.6) {
            if (defCallbacks->RowCbk) {
              if (defData->rowWarnings++ < defSettings->RowWarnings) {
                defError(6523, "Invalid ROW statement defined in the DEF file. The DO statement which is required in the ROW statement is not defined.\nUpdate your DEF file with a DO statement.");
                CHKERR();
              }
            }
          }
        }
#line 4494 "def.tab.c" /* yacc.c:1646  */
    break;

  case 189:
#line 1304 "def.y" /* yacc.c:1646  */
    {
          // 06/05/2002 - pcr 448455 
          // Check for 1 and 0 in the correct position 
          // 07/26/2002 - Commented out due to pcr 459218 
          if (defData->hasDoStep) {
            // 04/29/2004 - pcr 695535 
            // changed the testing 
            if ((((yyvsp[-1].dval) == 1) && (defData->yStep == 0)) ||
                (((yyvsp[-3].dval) == 1) && (defData->xStep == 0))) {
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
          if (((yyvsp[-3].dval) != 1) && ((yyvsp[-1].dval) != 1)) {
            if (defCallbacks->RowCbk) {
              if (defData->rowWarnings++ < defSettings->RowWarnings) {
                defError(6524, "Invalid syntax specified. The valid syntax is either \"DO 1 BY num or DO num BY 1\". Specify the valid syntax and try again.");
                CHKERR();
              }
            }
          }
          if (defCallbacks->RowCbk)
            defData->Row.setDo(ROUND((yyvsp[-3].dval)), ROUND((yyvsp[-1].dval)), defData->xStep, defData->yStep);
        }
#line 4534 "def.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 1341 "def.y" /* yacc.c:1646  */
    {
          defData->hasDoStep = 0;
        }
#line 4542 "def.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 1345 "def.y" /* yacc.c:1646  */
    {
          defData->hasDoStep = 1;
          defData->Row.setHasDoStep();
          defData->xStep = (yyvsp[-1].dval);
          defData->yStep = (yyvsp[0].dval);
        }
#line 4553 "def.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 1356 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1;}
#line 4559 "def.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 1358 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 4565 "def.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 1365 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RowCbk) {
             char propTp;
             char* str = ringCopy("                       ");
             propTp =  defSettings->RowProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "ROW");
             // For backword compatibility, also set the string value 
             sprintf(str, "%g", (yyvsp[0].dval));
             defData->Row.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
          }
        }
#line 4581 "def.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 1377 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RowCbk) {
             char propTp;
             propTp =  defSettings->RowProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "ROW");
             defData->Row.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 4594 "def.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 1386 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RowCbk) {
             char propTp;
             propTp =  defSettings->RowProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "ROW");
             defData->Row.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 4607 "def.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 1396 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->TrackCbk) {
            defData->Track.setup((yyvsp[-1].string));
          }
        }
#line 4617 "def.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 1402 "def.y" /* yacc.c:1646  */
    {
          if (((yyvsp[-4].dval) <= 0) && (defData->VersionNum >= 5.4)) {
            if (defCallbacks->TrackCbk)
              if (defData->trackWarnings++ < defSettings->TrackWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The DO number %g in TRACK is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[-4].dval));
                defError(6525, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if ((yyvsp[-2].dval) < 0) {
            if (defCallbacks->TrackCbk)
              if (defData->trackWarnings++ < defSettings->TrackWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The STEP number %g in TRACK is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[-2].dval));
                defError(6526, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if (defCallbacks->TrackCbk) {
            defData->Track.setDo(ROUND((yyvsp[-7].dval)), ROUND((yyvsp[-4].dval)), (yyvsp[-2].dval));
            CALLBACK(defCallbacks->TrackCbk, defrTrackCbkType, &defData->Track);
          }
        }
#line 4648 "def.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 1430 "def.y" /* yacc.c:1646  */
    {
          (yyval.string) = (yyvsp[0].string);
        }
#line 4656 "def.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 1435 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"X";}
#line 4662 "def.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 1437 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"Y";}
#line 4668 "def.tab.c" /* yacc.c:1646  */
    break;

  case 208:
#line 1443 "def.y" /* yacc.c:1646  */
    { 
              if (validateMaskInput((int)(yyvsp[-1].dval), defData->trackWarnings, defSettings->TrackWarnings)) {
                  if (defCallbacks->TrackCbk) {
                    defData->Track.addMask((yyvsp[-1].dval), (yyvsp[0].integer));
                  }
               }
            }
#line 4680 "def.tab.c" /* yacc.c:1646  */
    break;

  case 209:
#line 1453 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 0; }
#line 4686 "def.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 1455 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 1; }
#line 4692 "def.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 1458 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1000; }
#line 4698 "def.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 1459 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; }
#line 4704 "def.tab.c" /* yacc.c:1646  */
    break;

  case 216:
#line 1466 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->TrackCbk)
            defData->Track.addLayer((yyvsp[0].string));
        }
#line 4713 "def.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 1473 "def.y" /* yacc.c:1646  */
    {
          if ((yyvsp[-3].dval) <= 0) {
            if (defCallbacks->GcellGridCbk)
              if (defData->gcellGridWarnings++ < defSettings->GcellGridWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The DO number %g in GCELLGRID is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[-3].dval));
                defError(6527, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if ((yyvsp[-1].dval) < 0) {
            if (defCallbacks->GcellGridCbk)
              if (defData->gcellGridWarnings++ < defSettings->GcellGridWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The STEP number %g in GCELLGRID is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[-1].dval));
                defError(6528, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if (defCallbacks->GcellGridCbk) {
            defData->GcellGrid.setup((yyvsp[-6].string), ROUND((yyvsp[-5].dval)), ROUND((yyvsp[-3].dval)), (yyvsp[-1].dval));
            CALLBACK(defCallbacks->GcellGridCbk, defrGcellGridCbkType, &defData->GcellGrid);
          }
        }
#line 4744 "def.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 1501 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ExtensionCbk)
             CALLBACK(defCallbacks->ExtensionCbk, defrExtensionCbkType, &defData->History_text[0]);
        }
#line 4753 "def.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 1507 "def.y" /* yacc.c:1646  */
    { }
#line 4759 "def.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 1513 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ViaStartCbk)
            CALLBACK(defCallbacks->ViaStartCbk, defrViaStartCbkType, ROUND((yyvsp[-1].dval)));
        }
#line 4768 "def.tab.c" /* yacc.c:1646  */
    break;

  case 224:
#line 1522 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1;defData->no_num = 1; }
#line 4774 "def.tab.c" /* yacc.c:1646  */
    break;

  case 225:
#line 1523 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->ViaCbk) defData->Via.setup((yyvsp[0].string));
              defData->viaRule = 0;
            }
#line 4783 "def.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 1528 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->ViaCbk)
                CALLBACK(defCallbacks->ViaCbk, defrViaCbkType, &defData->Via);
              defData->Via.clear();
            }
#line 4793 "def.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 1538 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1;defData->no_num = 1; }
#line 4799 "def.tab.c" /* yacc.c:1646  */
    break;

  case 230:
#line 1539 "def.y" /* yacc.c:1646  */
    { 
              if (defCallbacks->ViaCbk)
                if (validateMaskInput((yyvsp[-2].integer), defData->viaWarnings, defSettings->ViaWarnings)) {
                    defData->Via.addLayer((yyvsp[-3].string), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y, (yyvsp[-2].integer));
                }
            }
#line 4810 "def.tab.c" /* yacc.c:1646  */
    break;

  case 231:
#line 1545 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 4816 "def.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 1546 "def.y" /* yacc.c:1646  */
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
              
            }
#line 4838 "def.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 1564 "def.y" /* yacc.c:1646  */
    {
              if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
                if (defCallbacks->ViaCbk)
                  if (validateMaskInput((yyvsp[-5].integer), defData->viaWarnings, defSettings->ViaWarnings)) {
                    defData->Via.addPolygon((yyvsp[-6].string), &defData->Geometries, (yyvsp[-5].integer));
                  }
              }
            }
#line 4851 "def.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 1572 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1;defData->no_num = 1; }
#line 4857 "def.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 1573 "def.y" /* yacc.c:1646  */
    {
              if (defData->VersionNum < 5.6) {
                if (defCallbacks->ViaCbk)
                  defData->Via.addPattern((yyvsp[0].string));
              } else
                if (defCallbacks->ViaCbk)
                  if (defData->viaWarnings++ < defSettings->ViaWarnings)
                    defWarning(7019, "The PATTERNNAME statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement."); 
            }
#line 4871 "def.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 1582 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1;defData->no_num = 1; }
#line 4877 "def.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 1584 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 3;defData->no_num = 1; }
#line 4883 "def.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 1587 "def.y" /* yacc.c:1646  */
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
                   defData->Via.addViaRule((yyvsp[-20].string), (int)(yyvsp[-17].dval), (int)(yyvsp[-16].dval), (yyvsp[-12].string), (yyvsp[-11].string),
                                             (yyvsp[-10].string), (int)(yyvsp[-7].dval), (int)(yyvsp[-6].dval), (int)(yyvsp[-3].dval),
                                             (int)(yyvsp[-2].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval)); 
              }
            }
#line 4908 "def.tab.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1609 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->ViaExtCbk)
              CALLBACK(defCallbacks->ViaExtCbk, defrViaExtCbkType, &defData->History_text[0]);
          }
#line 4917 "def.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1615 "def.y" /* yacc.c:1646  */
    {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6559, "The ROWCOL statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addRowCol((int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
            }
#line 4933 "def.tab.c" /* yacc.c:1646  */
    break;

  case 242:
#line 1627 "def.y" /* yacc.c:1646  */
    {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6560, "The ORIGIN statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addOrigin((int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
            }
#line 4949 "def.tab.c" /* yacc.c:1646  */
    break;

  case 243:
#line 1639 "def.y" /* yacc.c:1646  */
    {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6561, "The OFFSET statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addOffset((int)(yyvsp[-3].dval), (int)(yyvsp[-2].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
            }
#line 4965 "def.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 1650 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1;defData->no_num = 1; }
#line 4971 "def.tab.c" /* yacc.c:1646  */
    break;

  case 245:
#line 1651 "def.y" /* yacc.c:1646  */
    {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6562, "The PATTERN statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addCutPattern((yyvsp[0].string));
            }
#line 4987 "def.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 1664 "def.y" /* yacc.c:1646  */
    { defData->Geometries.startList((yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 4993 "def.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1667 "def.y" /* yacc.c:1646  */
    { defData->Geometries.addToList((yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 4999 "def.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 1674 "def.y" /* yacc.c:1646  */
    {
            defData->save_x = (yyvsp[-2].dval);
            defData->save_y = (yyvsp[-1].dval);
            (yyval.pt).x = ROUND((yyvsp[-2].dval));
            (yyval.pt).y = ROUND((yyvsp[-1].dval));
          }
#line 5010 "def.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 1681 "def.y" /* yacc.c:1646  */
    {
            defData->save_y = (yyvsp[-1].dval);
            (yyval.pt).x = ROUND(defData->save_x);
            (yyval.pt).y = ROUND((yyvsp[-1].dval));
          }
#line 5020 "def.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 1687 "def.y" /* yacc.c:1646  */
    {
            defData->save_x = (yyvsp[-2].dval);
            (yyval.pt).x = ROUND((yyvsp[-2].dval));
            (yyval.pt).y = ROUND(defData->save_y);
          }
#line 5030 "def.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 1693 "def.y" /* yacc.c:1646  */
    {
            (yyval.pt).x = ROUND(defData->save_x);
            (yyval.pt).y = ROUND(defData->save_y);
          }
#line 5039 "def.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 1699 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 0; }
#line 5045 "def.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 1701 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = (yyvsp[0].dval); }
#line 5051 "def.tab.c" /* yacc.c:1646  */
    break;

  case 256:
#line 1704 "def.y" /* yacc.c:1646  */
    { 
          if (defCallbacks->ViaEndCbk)
            CALLBACK(defCallbacks->ViaEndCbk, defrViaEndCbkType, 0);
        }
#line 5060 "def.tab.c" /* yacc.c:1646  */
    break;

  case 257:
#line 1710 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RegionEndCbk)
            CALLBACK(defCallbacks->RegionEndCbk, defrRegionEndCbkType, 0);
        }
#line 5069 "def.tab.c" /* yacc.c:1646  */
    break;

  case 258:
#line 1716 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RegionStartCbk)
            CALLBACK(defCallbacks->RegionStartCbk, defrRegionStartCbkType, ROUND((yyvsp[-1].dval)));
        }
#line 5078 "def.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 1723 "def.y" /* yacc.c:1646  */
    {}
#line 5084 "def.tab.c" /* yacc.c:1646  */
    break;

  case 261:
#line 1725 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 5090 "def.tab.c" /* yacc.c:1646  */
    break;

  case 262:
#line 1726 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RegionCbk)
             defData->Region.setup((yyvsp[0].string));
          defData->regTypeDef = 0;
        }
#line 5100 "def.tab.c" /* yacc.c:1646  */
    break;

  case 263:
#line 1732 "def.y" /* yacc.c:1646  */
    { CALLBACK(defCallbacks->RegionCbk, defrRegionCbkType, &defData->Region); }
#line 5106 "def.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 1736 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->RegionCbk)
          defData->Region.addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 5113 "def.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 1739 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->RegionCbk)
          defData->Region.addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 5120 "def.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 1747 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 5126 "def.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 1749 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 5132 "def.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 1751 "def.y" /* yacc.c:1646  */
    {
           if (defData->regTypeDef) {
              if (defCallbacks->RegionCbk) {
                if (defData->regionWarnings++ < defSettings->RegionWarnings) {
                  defError(6563, "The TYPE statement already exists. It has been defined in the REGION statement.");
                  CHKERR();
                }
              }
           }
           if (defCallbacks->RegionCbk) defData->Region.setType((yyvsp[0].string));
           defData->regTypeDef = 1;
         }
#line 5149 "def.tab.c" /* yacc.c:1646  */
    break;

  case 273:
#line 1770 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RegionCbk) {
             char propTp;
             char* str = ringCopy("                       ");
             propTp = defSettings->RegionProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "REGION");
             // For backword compatibility, also set the string value 
             // We will use a temporary string to store the number.
             // The string space is borrowed from the ring buffer
             // in the lexer.
             sprintf(str, "%g", (yyvsp[0].dval));
             defData->Region.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
          }
        }
#line 5168 "def.tab.c" /* yacc.c:1646  */
    break;

  case 274:
#line 1785 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RegionCbk) {
             char propTp;
             propTp = defSettings->RegionProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "REGION");
             defData->Region.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 5181 "def.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 1794 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->RegionCbk) {
             char propTp;
             propTp = defSettings->RegionProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "REGION");
             defData->Region.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 5194 "def.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 1804 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"FENCE"; }
#line 5200 "def.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 1806 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"GUIDE"; }
#line 5206 "def.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1809 "def.y" /* yacc.c:1646  */
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
         }
#line 5226 "def.tab.c" /* yacc.c:1646  */
    break;

  case 280:
#line 1829 "def.y" /* yacc.c:1646  */
    { 
            if (defCallbacks->ComponentStartCbk)
              CALLBACK(defCallbacks->ComponentStartCbk, defrComponentStartCbkType,
                       ROUND((yyvsp[-1].dval)));
         }
#line 5236 "def.tab.c" /* yacc.c:1646  */
    break;

  case 283:
#line 1840 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->ComponentMaskShiftLayerCbk) {
              defData->ComponentMaskShiftLayer.addMaskShiftLayer((yyvsp[0].string));
            }
        }
#line 5246 "def.tab.c" /* yacc.c:1646  */
    break;

  case 286:
#line 1851 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->ComponentCbk)
              CALLBACK(defCallbacks->ComponentCbk, defrComponentCbkType, &defData->Component);
         }
#line 5255 "def.tab.c" /* yacc.c:1646  */
    break;

  case 287:
#line 1857 "def.y" /* yacc.c:1646  */
    {
            defData->dumb_mode = 0;
            defData->no_num = 0;
         }
#line 5264 "def.tab.c" /* yacc.c:1646  */
    break;

  case 288:
#line 1862 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; }
#line 5270 "def.tab.c" /* yacc.c:1646  */
    break;

  case 289:
#line 1864 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->ComponentCbk)
              defData->Component.IdAndName((yyvsp[-1].string), (yyvsp[0].string));
         }
#line 5279 "def.tab.c" /* yacc.c:1646  */
    break;

  case 290:
#line 1870 "def.y" /* yacc.c:1646  */
    { }
#line 5285 "def.tab.c" /* yacc.c:1646  */
    break;

  case 291:
#line 1872 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->ComponentCbk)
                defData->Component.addNet("*");
            }
#line 5294 "def.tab.c" /* yacc.c:1646  */
    break;

  case 292:
#line 1877 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->ComponentCbk)
                defData->Component.addNet((yyvsp[0].string));
            }
#line 5303 "def.tab.c" /* yacc.c:1646  */
    break;

  case 307:
#line 1892 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk)
            CALLBACK(defCallbacks->ComponentExtCbk, defrComponentExtCbkType,
                     &defData->History_text[0]);
        }
#line 5313 "def.tab.c" /* yacc.c:1646  */
    break;

  case 308:
#line 1898 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=1; defData->no_num = 1; }
#line 5319 "def.tab.c" /* yacc.c:1646  */
    break;

  case 309:
#line 1899 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setEEQ((yyvsp[0].string));
        }
#line 5328 "def.tab.c" /* yacc.c:1646  */
    break;

  case 310:
#line 1904 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 2;  defData->no_num = 2; }
#line 5334 "def.tab.c" /* yacc.c:1646  */
    break;

  case 311:
#line 1906 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk)
             defData->Component.setGenerate((yyvsp[-1].string), (yyvsp[0].string));
        }
#line 5343 "def.tab.c" /* yacc.c:1646  */
    break;

  case 312:
#line 1912 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)""; }
#line 5349 "def.tab.c" /* yacc.c:1646  */
    break;

  case 313:
#line 1914 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string); }
#line 5355 "def.tab.c" /* yacc.c:1646  */
    break;

  case 314:
#line 1917 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setSource((yyvsp[0].string));
        }
#line 5364 "def.tab.c" /* yacc.c:1646  */
    break;

  case 315:
#line 1923 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"NETLIST"; }
#line 5370 "def.tab.c" /* yacc.c:1646  */
    break;

  case 316:
#line 1925 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"DIST"; }
#line 5376 "def.tab.c" /* yacc.c:1646  */
    break;

  case 317:
#line 1927 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"USER"; }
#line 5382 "def.tab.c" /* yacc.c:1646  */
    break;

  case 318:
#line 1929 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"TIMING"; }
#line 5388 "def.tab.c" /* yacc.c:1646  */
    break;

  case 319:
#line 1934 "def.y" /* yacc.c:1646  */
    { }
#line 5394 "def.tab.c" /* yacc.c:1646  */
    break;

  case 320:
#line 1936 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setRegionName((yyvsp[0].string));
        }
#line 5403 "def.tab.c" /* yacc.c:1646  */
    break;

  case 321:
#line 1942 "def.y" /* yacc.c:1646  */
    { 
          // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->ComponentCbk)
               defData->Component.setRegionBounds((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, 
                                                            (yyvsp[0].pt).x, (yyvsp[0].pt).y);
          }
          else
            defWarning(7020, "The REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
        }
#line 5418 "def.tab.c" /* yacc.c:1646  */
    break;

  case 322:
#line 1953 "def.y" /* yacc.c:1646  */
    { 
          // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->ComponentCbk)
               defData->Component.setRegionBounds((yyvsp[-1].pt).x, (yyvsp[-1].pt).y,
                                                            (yyvsp[0].pt).x, (yyvsp[0].pt).y);
          }
          else
            defWarning(7020, "The REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
        }
#line 5433 "def.tab.c" /* yacc.c:1646  */
    break;

  case 323:
#line 1965 "def.y" /* yacc.c:1646  */
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
        }
#line 5452 "def.tab.c" /* yacc.c:1646  */
    break;

  case 324:
#line 1980 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setHalo((int)(yyvsp[-3].dval), (int)(yyvsp[-2].dval),
                                                 (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
        }
#line 5462 "def.tab.c" /* yacc.c:1646  */
    break;

  case 326:
#line 1988 "def.y" /* yacc.c:1646  */
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
      }
#line 5484 "def.tab.c" /* yacc.c:1646  */
    break;

  case 327:
#line 2007 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 2; defData->no_num = 2; }
#line 5490 "def.tab.c" /* yacc.c:1646  */
    break;

  case 328:
#line 2008 "def.y" /* yacc.c:1646  */
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
                            (int)(yyvsp[-3].dval), (yyvsp[-1].string), (yyvsp[0].string));
        }
      }
#line 5513 "def.tab.c" /* yacc.c:1646  */
    break;

  case 329:
#line 2027 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 5519 "def.tab.c" /* yacc.c:1646  */
    break;

  case 330:
#line 2029 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 5525 "def.tab.c" /* yacc.c:1646  */
    break;

  case 333:
#line 2036 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            char* str = ringCopy("                       ");
            propTp = defSettings->CompProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "COMPONENT");
            sprintf(str, "%g", (yyvsp[0].dval));
            defData->Component.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
          }
        }
#line 5540 "def.tab.c" /* yacc.c:1646  */
    break;

  case 334:
#line 2047 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            propTp = defSettings->CompProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "COMPONENT");
            defData->Component.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 5553 "def.tab.c" /* yacc.c:1646  */
    break;

  case 335:
#line 2056 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            propTp = defSettings->CompProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "COMPONENT");
            defData->Component.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 5566 "def.tab.c" /* yacc.c:1646  */
    break;

  case 336:
#line 2066 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 5572 "def.tab.c" /* yacc.c:1646  */
    break;

  case 337:
#line 2068 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 5578 "def.tab.c" /* yacc.c:1646  */
    break;

  case 338:
#line 2070 "def.y" /* yacc.c:1646  */
    { 
          if (defData->VersionNum < 5.6) {
            if (defCallbacks->ComponentCbk) {
              defData->Component.setForeignName((yyvsp[-2].string));
              defData->Component.setForeignLocation((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
            }
         } else
            if (defCallbacks->ComponentCbk)
              if (defData->componentWarnings++ < defSettings->ComponentWarnings)
                defWarning(7021, "The FOREIGN statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement.");
         }
#line 5594 "def.tab.c" /* yacc.c:1646  */
    break;

  case 339:
#line 2084 "def.y" /* yacc.c:1646  */
    { (yyval.pt) = (yyvsp[0].pt); }
#line 5600 "def.tab.c" /* yacc.c:1646  */
    break;

  case 340:
#line 2086 "def.y" /* yacc.c:1646  */
    { (yyval.pt).x = ROUND((yyvsp[-1].dval)); (yyval.pt).y = ROUND((yyvsp[0].dval)); }
#line 5606 "def.tab.c" /* yacc.c:1646  */
    break;

  case 341:
#line 2089 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk) {
            defData->Component.setPlacementStatus((yyvsp[-2].integer));
            defData->Component.setPlacementLocation((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
          }
        }
#line 5617 "def.tab.c" /* yacc.c:1646  */
    break;

  case 342:
#line 2096 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setPlacementStatus(
                                         DEFI_COMPONENT_UNPLACED);
            defData->Component.setPlacementLocation(-1, -1, -1);
        }
#line 5628 "def.tab.c" /* yacc.c:1646  */
    break;

  case 343:
#line 2103 "def.y" /* yacc.c:1646  */
    {
          if (defData->VersionNum < 5.4) {   // PCR 495463 
            if (defCallbacks->ComponentCbk) {
              defData->Component.setPlacementStatus(
                                          DEFI_COMPONENT_UNPLACED);
              defData->Component.setPlacementLocation((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
            }
          } else {
            if (defData->componentWarnings++ < defSettings->ComponentWarnings)
               defWarning(7022, "In the COMPONENT UNPLACED statement, point and orient are invalid in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
#line 5645 "def.tab.c" /* yacc.c:1646  */
    break;

  case 344:
#line 2117 "def.y" /* yacc.c:1646  */
    {  
          if (defCallbacks->ComponentCbk) {
            if (validateMaskInput((int)(yyvsp[0].dval), defData->componentWarnings, defSettings->ComponentWarnings)) {
                defData->Component.setMaskShift(int((yyvsp[0].dval)));
            }
          }
        }
#line 5657 "def.tab.c" /* yacc.c:1646  */
    break;

  case 345:
#line 2126 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = DEFI_COMPONENT_FIXED; }
#line 5663 "def.tab.c" /* yacc.c:1646  */
    break;

  case 346:
#line 2128 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = DEFI_COMPONENT_COVER; }
#line 5669 "def.tab.c" /* yacc.c:1646  */
    break;

  case 347:
#line 2130 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = DEFI_COMPONENT_PLACED; }
#line 5675 "def.tab.c" /* yacc.c:1646  */
    break;

  case 348:
#line 2133 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->ComponentCbk)
            defData->Component.setWeight(ROUND((yyvsp[0].dval)));
        }
#line 5684 "def.tab.c" /* yacc.c:1646  */
    break;

  case 349:
#line 2139 "def.y" /* yacc.c:1646  */
    { 
          if (defCallbacks->ComponentCbk)
            CALLBACK(defCallbacks->ComponentEndCbk, defrComponentEndCbkType, 0);
        }
#line 5693 "def.tab.c" /* yacc.c:1646  */
    break;

  case 351:
#line 2148 "def.y" /* yacc.c:1646  */
    { 
          if (defCallbacks->NetStartCbk)
            CALLBACK(defCallbacks->NetStartCbk, defrNetStartCbkType, ROUND((yyvsp[-1].dval)));
          defData->netOsnet = 1;
        }
#line 5703 "def.tab.c" /* yacc.c:1646  */
    break;

  case 354:
#line 2159 "def.y" /* yacc.c:1646  */
    { 
          if (defCallbacks->NetCbk)
            CALLBACK(defCallbacks->NetCbk, defrNetCbkType, &defData->Net);
        }
#line 5712 "def.tab.c" /* yacc.c:1646  */
    break;

  case 355:
#line 2170 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 0; defData->no_num = 0; }
#line 5718 "def.tab.c" /* yacc.c:1646  */
    break;

  case 356:
#line 2173 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; defData->nondef_is_keyword = TRUE; defData->mustjoin_is_keyword = TRUE;}
#line 5724 "def.tab.c" /* yacc.c:1646  */
    break;

  case 358:
#line 2176 "def.y" /* yacc.c:1646  */
    {
          // 9/22/1999 
          // this is shared by both net and special net 
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.setName((yyvsp[0].string));
          if (defCallbacks->NetNameCbk)
            CALLBACK(defCallbacks->NetNameCbk, defrNetNameCbkType, (yyvsp[0].string));
        }
#line 5737 "def.tab.c" /* yacc.c:1646  */
    break;

  case 360:
#line 2184 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 5743 "def.tab.c" /* yacc.c:1646  */
    break;

  case 361:
#line 2185 "def.y" /* yacc.c:1646  */
    {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addMustPin((yyvsp[-3].string), (yyvsp[-1].string), 0);
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }
#line 5754 "def.tab.c" /* yacc.c:1646  */
    break;

  case 364:
#line 2196 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT;}
#line 5760 "def.tab.c" /* yacc.c:1646  */
    break;

  case 365:
#line 2198 "def.y" /* yacc.c:1646  */
    {
          // 9/22/1999 
          // since the code is shared by both net & special net, 
          // need to check on both flags 
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin((yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].integer));
          // 1/14/2000 - pcr 289156 
          // reset defData->dumb_mode & defData->no_num to 3 , just in case 
          // the next statement is another net_connection 
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }
#line 5777 "def.tab.c" /* yacc.c:1646  */
    break;

  case 366:
#line 2210 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 5783 "def.tab.c" /* yacc.c:1646  */
    break;

  case 367:
#line 2211 "def.y" /* yacc.c:1646  */
    {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin("*", (yyvsp[-2].string), (yyvsp[-1].integer));
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }
#line 5794 "def.tab.c" /* yacc.c:1646  */
    break;

  case 368:
#line 2217 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 5800 "def.tab.c" /* yacc.c:1646  */
    break;

  case 369:
#line 2218 "def.y" /* yacc.c:1646  */
    {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin("PIN", (yyvsp[-2].string), (yyvsp[-1].integer));
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }
#line 5811 "def.tab.c" /* yacc.c:1646  */
    break;

  case 370:
#line 2226 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 0; }
#line 5817 "def.tab.c" /* yacc.c:1646  */
    break;

  case 371:
#line 2228 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->NetConnectionExtCbk)
            CALLBACK(defCallbacks->NetConnectionExtCbk, defrNetConnectionExtCbkType,
              &defData->History_text[0]);
          (yyval.integer) = 0;
        }
#line 5828 "def.tab.c" /* yacc.c:1646  */
    break;

  case 372:
#line 2235 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 1; }
#line 5834 "def.tab.c" /* yacc.c:1646  */
    break;

  case 375:
#line 2244 "def.y" /* yacc.c:1646  */
    {  
          if (defCallbacks->NetCbk) defData->Net.addWire((yyvsp[0].string), NULL);
        }
#line 5842 "def.tab.c" /* yacc.c:1646  */
    break;

  case 376:
#line 2248 "def.y" /* yacc.c:1646  */
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
        }
#line 5860 "def.tab.c" /* yacc.c:1646  */
    break;

  case 377:
#line 2263 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.setSource((yyvsp[0].string)); }
#line 5866 "def.tab.c" /* yacc.c:1646  */
    break;

  case 378:
#line 2266 "def.y" /* yacc.c:1646  */
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
        }
#line 5886 "def.tab.c" /* yacc.c:1646  */
    break;

  case 379:
#line 2282 "def.y" /* yacc.c:1646  */
    { defData->real_num = 1; }
#line 5892 "def.tab.c" /* yacc.c:1646  */
    break;

  case 380:
#line 2283 "def.y" /* yacc.c:1646  */
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
          if (defCallbacks->NetCbk) defData->Net.setFrequency((yyvsp[0].dval));
          defData->real_num = 0;
        }
#line 5913 "def.tab.c" /* yacc.c:1646  */
    break;

  case 381:
#line 2300 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 5919 "def.tab.c" /* yacc.c:1646  */
    break;

  case 382:
#line 2301 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.setOriginal((yyvsp[0].string)); }
#line 5925 "def.tab.c" /* yacc.c:1646  */
    break;

  case 383:
#line 2304 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.setPattern((yyvsp[0].string)); }
#line 5931 "def.tab.c" /* yacc.c:1646  */
    break;

  case 384:
#line 2307 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.setWeight(ROUND((yyvsp[0].dval))); }
#line 5937 "def.tab.c" /* yacc.c:1646  */
    break;

  case 385:
#line 2310 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.setXTalk(ROUND((yyvsp[0].dval))); }
#line 5943 "def.tab.c" /* yacc.c:1646  */
    break;

  case 386:
#line 2313 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.setCap((yyvsp[0].dval)); }
#line 5949 "def.tab.c" /* yacc.c:1646  */
    break;

  case 387:
#line 2316 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.setUse((yyvsp[0].string)); }
#line 5955 "def.tab.c" /* yacc.c:1646  */
    break;

  case 388:
#line 2319 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.setStyle((int)(yyvsp[0].dval)); }
#line 5961 "def.tab.c" /* yacc.c:1646  */
    break;

  case 389:
#line 2321 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 5967 "def.tab.c" /* yacc.c:1646  */
    break;

  case 390:
#line 2322 "def.y" /* yacc.c:1646  */
    { 
          if (defCallbacks->NetCbk && defCallbacks->NetNonDefaultRuleCbk) {
             // User wants a callback on nondefaultrule 
             CALLBACK(defCallbacks->NetNonDefaultRuleCbk,
                      defrNetNonDefaultRuleCbkType, (yyvsp[0].string));
          }
          // Still save data in the class 
          if (defCallbacks->NetCbk) defData->Net.setNonDefaultRule((yyvsp[0].string));
        }
#line 5981 "def.tab.c" /* yacc.c:1646  */
    break;

  case 392:
#line 2334 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 5987 "def.tab.c" /* yacc.c:1646  */
    break;

  case 393:
#line 2335 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.addShieldNet((yyvsp[0].string)); }
#line 5993 "def.tab.c" /* yacc.c:1646  */
    break;

  case 394:
#line 2337 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 5999 "def.tab.c" /* yacc.c:1646  */
    break;

  case 395:
#line 2338 "def.y" /* yacc.c:1646  */
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
        }
#line 6020 "def.tab.c" /* yacc.c:1646  */
    break;

  case 396:
#line 2355 "def.y" /* yacc.c:1646  */
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
        }
#line 6052 "def.tab.c" /* yacc.c:1646  */
    break;

  case 397:
#line 2384 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1;
          if (defCallbacks->NetCbk) {
            defData->Subnet = (defiSubnet*)defMalloc(sizeof(defiSubnet));
            defData->Subnet->Init();
          }
        }
#line 6063 "def.tab.c" /* yacc.c:1646  */
    break;

  case 398:
#line 2390 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->NetCbk && defCallbacks->NetSubnetNameCbk) {
            // User wants a callback on Net subnetName 
            CALLBACK(defCallbacks->NetSubnetNameCbk, defrNetSubnetNameCbkType, (yyvsp[0].string));
          }
          // Still save the subnet name in the class 
          if (defCallbacks->NetCbk) {
            defData->Subnet->setName((yyvsp[0].string));
          }
        }
#line 6078 "def.tab.c" /* yacc.c:1646  */
    break;

  case 399:
#line 2400 "def.y" /* yacc.c:1646  */
    {
          defData->routed_is_keyword = TRUE;
          defData->fixed_is_keyword = TRUE;
          defData->cover_is_keyword = TRUE;
        }
#line 6088 "def.tab.c" /* yacc.c:1646  */
    break;

  case 400:
#line 2404 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->NetCbk) {
            defData->Net.addSubnet(defData->Subnet);
            defData->Subnet = NULL;
            defData->routed_is_keyword = FALSE;
            defData->fixed_is_keyword = FALSE;
            defData->cover_is_keyword = FALSE;
          }
        }
#line 6102 "def.tab.c" /* yacc.c:1646  */
    break;

  case 401:
#line 2414 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 6108 "def.tab.c" /* yacc.c:1646  */
    break;

  case 402:
#line 2416 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 6114 "def.tab.c" /* yacc.c:1646  */
    break;

  case 403:
#line 2419 "def.y" /* yacc.c:1646  */
    { 
          if (defCallbacks->NetExtCbk)
            CALLBACK(defCallbacks->NetExtCbk, defrNetExtCbkType, &defData->History_text[0]);
        }
#line 6123 "def.tab.c" /* yacc.c:1646  */
    break;

  case 406:
#line 2429 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->NetCbk) {
            char propTp;
            char* str = ringCopy("                       ");
            propTp = defSettings->NetProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "NET");
            sprintf(str, "%g", (yyvsp[0].dval));
            defData->Net.addNumProp((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
          }
        }
#line 6138 "def.tab.c" /* yacc.c:1646  */
    break;

  case 407:
#line 2440 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->NetCbk) {
            char propTp;
            propTp = defSettings->NetProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "NET");
            defData->Net.addProp((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 6151 "def.tab.c" /* yacc.c:1646  */
    break;

  case 408:
#line 2449 "def.y" /* yacc.c:1646  */
    {
          if (defCallbacks->NetCbk) {
            char propTp;
            propTp = defSettings->NetProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "NET");
            defData->Net.addProp((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 6164 "def.tab.c" /* yacc.c:1646  */
    break;

  case 409:
#line 2459 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"NETLIST"; }
#line 6170 "def.tab.c" /* yacc.c:1646  */
    break;

  case 410:
#line 2461 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"DIST"; }
#line 6176 "def.tab.c" /* yacc.c:1646  */
    break;

  case 411:
#line 2463 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"USER"; }
#line 6182 "def.tab.c" /* yacc.c:1646  */
    break;

  case 412:
#line 2465 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"TIMING"; }
#line 6188 "def.tab.c" /* yacc.c:1646  */
    break;

  case 413:
#line 2467 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"TEST"; }
#line 6194 "def.tab.c" /* yacc.c:1646  */
    break;

  case 414:
#line 2470 "def.y" /* yacc.c:1646  */
    {
          // vpin_options may have to deal with orient 
          defData->orient_is_keyword = TRUE;
        }
#line 6203 "def.tab.c" /* yacc.c:1646  */
    break;

  case 415:
#line 2475 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk)
            defData->Net.addVpinBounds((yyvsp[-3].pt).x, (yyvsp[-3].pt).y, (yyvsp[-2].pt).x, (yyvsp[-2].pt).y);
          defData->orient_is_keyword = FALSE;
        }
#line 6212 "def.tab.c" /* yacc.c:1646  */
    break;

  case 416:
#line 2480 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 6218 "def.tab.c" /* yacc.c:1646  */
    break;

  case 417:
#line 2481 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.addVpin((yyvsp[0].string)); }
#line 6224 "def.tab.c" /* yacc.c:1646  */
    break;

  case 419:
#line 2484 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=1;}
#line 6230 "def.tab.c" /* yacc.c:1646  */
    break;

  case 420:
#line 2485 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.addVpinLayer((yyvsp[0].string)); }
#line 6236 "def.tab.c" /* yacc.c:1646  */
    break;

  case 422:
#line 2489 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Net.addVpinLoc((yyvsp[-2].string), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer)); }
#line 6242 "def.tab.c" /* yacc.c:1646  */
    break;

  case 423:
#line 2492 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"PLACED"; }
#line 6248 "def.tab.c" /* yacc.c:1646  */
    break;

  case 424:
#line 2494 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"FIXED"; }
#line 6254 "def.tab.c" /* yacc.c:1646  */
    break;

  case 425:
#line 2496 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"COVER"; }
#line 6260 "def.tab.c" /* yacc.c:1646  */
    break;

  case 426:
#line 2499 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"FIXED"; defData->dumb_mode = 1; }
#line 6266 "def.tab.c" /* yacc.c:1646  */
    break;

  case 427:
#line 2501 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"COVER"; defData->dumb_mode = 1; }
#line 6272 "def.tab.c" /* yacc.c:1646  */
    break;

  case 428:
#line 2503 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"ROUTED"; defData->dumb_mode = 1; }
#line 6278 "def.tab.c" /* yacc.c:1646  */
    break;

  case 429:
#line 2507 "def.y" /* yacc.c:1646  */
    { if (defData->NeedPathData && defCallbacks->NetCbk)
          pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needNPCbk);
      }
#line 6286 "def.tab.c" /* yacc.c:1646  */
    break;

  case 430:
#line 2511 "def.y" /* yacc.c:1646  */
    { }
#line 6292 "def.tab.c" /* yacc.c:1646  */
    break;

  case 431:
#line 2513 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 6298 "def.tab.c" /* yacc.c:1646  */
    break;

  case 432:
#line 2514 "def.y" /* yacc.c:1646  */
    { if (defData->NeedPathData && defCallbacks->NetCbk)
          pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needNPCbk);
      }
#line 6306 "def.tab.c" /* yacc.c:1646  */
    break;

  case 433:
#line 2519 "def.y" /* yacc.c:1646  */
    {
        if ((strcmp((yyvsp[0].string), "TAPER") == 0) || (strcmp((yyvsp[0].string), "TAPERRULE") == 0)) {
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
              defData->PathObj.addLayer((yyvsp[0].string));
          defData->dumb_mode = 0; defData->no_num = 0;
        }
      }
#line 6329 "def.tab.c" /* yacc.c:1646  */
    break;

  case 434:
#line 2538 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
/*
       dumb_mode = 1; by_is_keyword = TRUE; do_is_keyword = TRUE;
*/
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE; 
        defData->orient_is_keyword = TRUE; defData->virtual_is_keyword = TRUE;
        defData->mask_is_keyword = TRUE, defData->rect_is_keyword = TRUE;  }
#line 6341 "def.tab.c" /* yacc.c:1646  */
    break;

  case 435:
#line 2548 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0;   defData->virtual_is_keyword = FALSE; defData->mask_is_keyword = FALSE,
       defData->rect_is_keyword = FALSE; }
#line 6348 "def.tab.c" /* yacc.c:1646  */
    break;

  case 436:
#line 2553 "def.y" /* yacc.c:1646  */
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
    }
#line 6367 "def.tab.c" /* yacc.c:1646  */
    break;

  case 437:
#line 2570 "def.y" /* yacc.c:1646  */
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
    }
#line 6386 "def.tab.c" /* yacc.c:1646  */
    break;

  case 440:
#line 2593 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          if (strcmp((yyvsp[0].string), "TAPER") == 0)
            defData->PathObj.setTaper();
          else {
            defData->PathObj.addVia((yyvsp[0].string));
            }
        }
      }
#line 6401 "def.tab.c" /* yacc.c:1646  */
    break;

  case 441:
#line 2604 "def.y" /* yacc.c:1646  */
    {
        if (validateMaskInput((int)(yyvsp[-1].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
                (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
              if (strcmp((yyvsp[0].string), "TAPER") == 0)
                defData->PathObj.setTaper();
              else {
                defData->PathObj.addViaMask((yyvsp[-1].dval));
                defData->PathObj.addVia((yyvsp[0].string));
                }
            }
        }
      }
#line 6419 "def.tab.c" /* yacc.c:1646  */
    break;

  case 442:
#line 2618 "def.y" /* yacc.c:1646  */
    { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
            defData->PathObj.addVia((yyvsp[-1].string));
            defData->PathObj.addViaRotation((yyvsp[0].integer));
        }
      }
#line 6430 "def.tab.c" /* yacc.c:1646  */
    break;

  case 443:
#line 2625 "def.y" /* yacc.c:1646  */
    { 
        if (validateMaskInput((int)(yyvsp[-2].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
                (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
                defData->PathObj.addViaMask((yyvsp[-2].dval));
                defData->PathObj.addVia((yyvsp[-1].string));
                defData->PathObj.addViaRotation((yyvsp[0].integer));
            }
        }
      }
#line 6445 "def.tab.c" /* yacc.c:1646  */
    break;

  case 444:
#line 2636 "def.y" /* yacc.c:1646  */
    {
        if (validateMaskInput((int)(yyvsp[-8].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {      
            if (((yyvsp[-5].dval) == 0) || ((yyvsp[-3].dval) == 0)) {
              if (defData->NeedPathData &&
                  defCallbacks->SNetCbk) {
                if (defData->netWarnings++ < defSettings->NetWarnings) {
                  defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
                  CHKERR();
                }
              }
            }
            if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
                defData->PathObj.addViaMask((yyvsp[-8].dval));
                defData->PathObj.addVia((yyvsp[-7].string));
                defData->PathObj.addViaData((int)(yyvsp[-5].dval), (int)(yyvsp[-3].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
            }  else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
                CHKERR();
              }
            }
        }
      }
#line 6473 "def.tab.c" /* yacc.c:1646  */
    break;

  case 445:
#line 2660 "def.y" /* yacc.c:1646  */
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
        if (((yyvsp[-5].dval) == 0) || ((yyvsp[-3].dval) == 0)) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
              CHKERR();
            }
          }
        }
        if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
            defData->PathObj.addVia((yyvsp[-7].string));
            defData->PathObj.addViaData((int)(yyvsp[-5].dval), (int)(yyvsp[-3].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
        }  else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
          if (defData->netWarnings++ < defSettings->NetWarnings) {
            defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
            CHKERR();
          }
        }
      }
#line 6511 "def.tab.c" /* yacc.c:1646  */
    break;

  case 446:
#line 2694 "def.y" /* yacc.c:1646  */
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
        if (((yyvsp[-5].dval) == 0) || ((yyvsp[-3].dval) == 0)) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
              CHKERR();
            }
          }
        }
        if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
            defData->PathObj.addVia((yyvsp[-8].string));
            defData->PathObj.addViaRotation((yyvsp[-7].integer));
            defData->PathObj.addViaData((int)(yyvsp[-5].dval), (int)(yyvsp[-3].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
        } else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
          if (defData->netWarnings++ < defSettings->NetWarnings) {
            defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
            CHKERR();
          }
        }
      }
#line 6549 "def.tab.c" /* yacc.c:1646  */
    break;

  case 447:
#line 2728 "def.y" /* yacc.c:1646  */
    {
        if (validateMaskInput((int)(yyvsp[-9].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (((yyvsp[-5].dval) == 0) || ((yyvsp[-3].dval) == 0)) {
              if (defData->NeedPathData &&
                  defCallbacks->SNetCbk) {
                if (defData->netWarnings++ < defSettings->NetWarnings) {
                  defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
                  CHKERR();
                }
              }
            }
            if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
                defData->PathObj.addViaMask((yyvsp[-9].dval)); 
                defData->PathObj.addVia((yyvsp[-8].string));
                defData->PathObj.addViaRotation((yyvsp[-7].integer));;
                defData->PathObj.addViaData((int)(yyvsp[-5].dval), (int)(yyvsp[-3].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
            } else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
                CHKERR();
              }
            }
        }
      }
#line 6578 "def.tab.c" /* yacc.c:1646  */
    break;

  case 450:
#line 2754 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 6; }
#line 6584 "def.tab.c" /* yacc.c:1646  */
    break;

  case 451:
#line 2755 "def.y" /* yacc.c:1646  */
    {
      if (validateMaskInput((int)(yyvsp[-8].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addMask((yyvsp[-8].dval));
          defData->PathObj.addViaRect((yyvsp[-4].dval), (yyvsp[-3].dval), (yyvsp[-2].dval), (yyvsp[-1].dval));
        }
      }
    }
#line 6598 "def.tab.c" /* yacc.c:1646  */
    break;

  case 452:
#line 2765 "def.y" /* yacc.c:1646  */
    {
       if (validateMaskInput((int)(yyvsp[0].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addMask((yyvsp[0].dval)); 
        }
       }  
    }
#line 6611 "def.tab.c" /* yacc.c:1646  */
    break;

  case 454:
#line 2775 "def.y" /* yacc.c:1646  */
    {
       // reset defData->dumb_mode to 1 just incase the next token is a via of the path
        // 2/5/2004 - pcr 686781
        defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE;
        defData->orient_is_keyword = TRUE;
    }
#line 6623 "def.tab.c" /* yacc.c:1646  */
    break;

  case 455:
#line 2786 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND((yyvsp[-2].dval)), ROUND((yyvsp[-1].dval))); 
        defData->save_x = (yyvsp[-2].dval);
        defData->save_y = (yyvsp[-1].dval); 
      }
#line 6635 "def.tab.c" /* yacc.c:1646  */
    break;

  case 456:
#line 2794 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND(defData->save_x), ROUND((yyvsp[-1].dval))); 
        defData->save_y = (yyvsp[-1].dval);
      }
#line 6646 "def.tab.c" /* yacc.c:1646  */
    break;

  case 457:
#line 2801 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND((yyvsp[-2].dval)), ROUND(defData->save_y)); 
        defData->save_x = (yyvsp[-2].dval);
      }
#line 6657 "def.tab.c" /* yacc.c:1646  */
    break;

  case 458:
#line 2808 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND(defData->save_x), ROUND(defData->save_y)); 
      }
#line 6667 "def.tab.c" /* yacc.c:1646  */
    break;

  case 459:
#line 2814 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND((yyvsp[-3].dval)), ROUND((yyvsp[-2].dval)), ROUND((yyvsp[-1].dval))); 
        defData->save_x = (yyvsp[-3].dval);
        defData->save_y = (yyvsp[-2].dval);
      }
#line 6679 "def.tab.c" /* yacc.c:1646  */
    break;

  case 460:
#line 2822 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND(defData->save_x), ROUND((yyvsp[-2].dval)),
          ROUND((yyvsp[-1].dval))); 
        defData->save_y = (yyvsp[-2].dval);
      }
#line 6691 "def.tab.c" /* yacc.c:1646  */
    break;

  case 461:
#line 2830 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND((yyvsp[-3].dval)), ROUND(defData->save_y),
          ROUND((yyvsp[-1].dval))); 
        defData->save_x = (yyvsp[-3].dval);
      }
#line 6703 "def.tab.c" /* yacc.c:1646  */
    break;

  case 462:
#line 2838 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND(defData->save_x), ROUND(defData->save_y),
          ROUND((yyvsp[-1].dval))); 
      }
#line 6714 "def.tab.c" /* yacc.c:1646  */
    break;

  case 463:
#line 2847 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND((yyvsp[-2].dval)), ROUND((yyvsp[-1].dval))); 
        defData->save_x = (yyvsp[-2].dval);
        defData->save_y = (yyvsp[-1].dval);
      }
#line 6726 "def.tab.c" /* yacc.c:1646  */
    break;

  case 464:
#line 2855 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND(defData->save_x), ROUND((yyvsp[-1].dval))); 
        defData->save_y = (yyvsp[-1].dval);
      }
#line 6737 "def.tab.c" /* yacc.c:1646  */
    break;

  case 465:
#line 2862 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND((yyvsp[-2].dval)), ROUND(defData->save_y)); 
        defData->save_x = (yyvsp[-2].dval);
      }
#line 6748 "def.tab.c" /* yacc.c:1646  */
    break;

  case 466:
#line 2869 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND(defData->save_x), ROUND(defData->save_y));
      }
#line 6758 "def.tab.c" /* yacc.c:1646  */
    break;

  case 467:
#line 2877 "def.y" /* yacc.c:1646  */
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addViaRect((yyvsp[-4].dval), (yyvsp[-3].dval), (yyvsp[-2].dval), (yyvsp[-1].dval)); 
        }    
    }
#line 6769 "def.tab.c" /* yacc.c:1646  */
    break;

  case 472:
#line 2893 "def.y" /* yacc.c:1646  */
    { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.setTaper(); }
#line 6777 "def.tab.c" /* yacc.c:1646  */
    break;

  case 473:
#line 2896 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 6783 "def.tab.c" /* yacc.c:1646  */
    break;

  case 474:
#line 2897 "def.y" /* yacc.c:1646  */
    { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addTaperRule((yyvsp[0].string)); }
#line 6791 "def.tab.c" /* yacc.c:1646  */
    break;

  case 475:
#line 2902 "def.y" /* yacc.c:1646  */
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
             defData->PathObj.addStyle((int)(yyvsp[0].dval));
      }
#line 6814 "def.tab.c" /* yacc.c:1646  */
    break;

  case 478:
#line 2927 "def.y" /* yacc.c:1646  */
    { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addShape((yyvsp[0].string)); }
#line 6822 "def.tab.c" /* yacc.c:1646  */
    break;

  case 479:
#line 2931 "def.y" /* yacc.c:1646  */
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
            defData->PathObj.addStyle((int)(yyvsp[0].dval));
        }
      }
#line 6845 "def.tab.c" /* yacc.c:1646  */
    break;

  case 480:
#line 2951 "def.y" /* yacc.c:1646  */
    { 
            CALLBACK(defCallbacks->NetEndCbk, defrNetEndCbkType, 0);
            defData->netOsnet = 0;
          }
#line 6854 "def.tab.c" /* yacc.c:1646  */
    break;

  case 481:
#line 2957 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"RING"; }
#line 6860 "def.tab.c" /* yacc.c:1646  */
    break;

  case 482:
#line 2959 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"STRIPE"; }
#line 6866 "def.tab.c" /* yacc.c:1646  */
    break;

  case 483:
#line 2961 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"FOLLOWPIN"; }
#line 6872 "def.tab.c" /* yacc.c:1646  */
    break;

  case 484:
#line 2963 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"IOWIRE"; }
#line 6878 "def.tab.c" /* yacc.c:1646  */
    break;

  case 485:
#line 2965 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"COREWIRE"; }
#line 6884 "def.tab.c" /* yacc.c:1646  */
    break;

  case 486:
#line 2967 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"BLOCKWIRE"; }
#line 6890 "def.tab.c" /* yacc.c:1646  */
    break;

  case 487:
#line 2969 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"FILLWIRE"; }
#line 6896 "def.tab.c" /* yacc.c:1646  */
    break;

  case 488:
#line 2971 "def.y" /* yacc.c:1646  */
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
            }
#line 6916 "def.tab.c" /* yacc.c:1646  */
    break;

  case 489:
#line 2987 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"DRCFILL"; }
#line 6922 "def.tab.c" /* yacc.c:1646  */
    break;

  case 490:
#line 2989 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"BLOCKAGEWIRE"; }
#line 6928 "def.tab.c" /* yacc.c:1646  */
    break;

  case 491:
#line 2991 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"PADRING"; }
#line 6934 "def.tab.c" /* yacc.c:1646  */
    break;

  case 492:
#line 2993 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"BLOCKRING"; }
#line 6940 "def.tab.c" /* yacc.c:1646  */
    break;

  case 496:
#line 3003 "def.y" /* yacc.c:1646  */
    { CALLBACK(defCallbacks->SNetCbk, defrSNetCbkType, &defData->Net); }
#line 6946 "def.tab.c" /* yacc.c:1646  */
    break;

  case 503:
#line 3014 "def.y" /* yacc.c:1646  */
    {
             if (defData->VersionNum >= 5.8) {
                defData->specialWire_routeStatus = (yyvsp[0].string);
             } else {
                 if (defCallbacks->SNetCbk) {   // PCR 902306 
                   defData->defMsg = (char*)defMalloc(1024);
                   sprintf(defData->defMsg, "The SPECIAL NET statement, with type %s, does not have any net statement defined.\nThe DEF parser will ignore this statemnet.", (yyvsp[0].string));
                   defWarning(7023, defData->defMsg);
                   defFree(defData->defMsg);
                 }
             }
            }
#line 6963 "def.tab.c" /* yacc.c:1646  */
    break;

  case 504:
#line 3027 "def.y" /* yacc.c:1646  */
    {
            if (defCallbacks->SNetCbk) defData->Net.addWire((yyvsp[0].string), NULL);
            }
#line 6971 "def.tab.c" /* yacc.c:1646  */
    break;

  case 505:
#line 3031 "def.y" /* yacc.c:1646  */
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
            }
#line 6992 "def.tab.c" /* yacc.c:1646  */
    break;

  case 506:
#line 3048 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 6998 "def.tab.c" /* yacc.c:1646  */
    break;

  case 507:
#line 3049 "def.y" /* yacc.c:1646  */
    { defData->shieldName = (yyvsp[0].string); 
              defData->specialWire_routeStatus = (char*)"SHIELD";
              defData->specialWire_routeStatusName = (yyvsp[0].string); 
            }
#line 7007 "def.tab.c" /* yacc.c:1646  */
    break;

  case 509:
#line 3056 "def.y" /* yacc.c:1646  */
    {  
            defData->specialWire_shapeType = (yyvsp[0].string);
          }
#line 7015 "def.tab.c" /* yacc.c:1646  */
    break;

  case 510:
#line 3060 "def.y" /* yacc.c:1646  */
    {
            if (validateMaskInput((int)(yyvsp[0].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
                defData->specialWire_mask = (yyvsp[0].dval);
            }     
          }
#line 7025 "def.tab.c" /* yacc.c:1646  */
    break;

  case 511:
#line 3065 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 7031 "def.tab.c" /* yacc.c:1646  */
    break;

  case 512:
#line 3066 "def.y" /* yacc.c:1646  */
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
          }
#line 7052 "def.tab.c" /* yacc.c:1646  */
    break;

  case 513:
#line 3083 "def.y" /* yacc.c:1646  */
    {
            if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
              if (defCallbacks->SNetCbk) {
                // defData->needSNPCbk will indicate that it has reach the max
                // memory and if user has set partialPathCBk, def parser
                // will make the callback.
                // This will improve performance
                // This construct is only in specialnet
                defData->Net.addPolygon((yyvsp[-5].string), &defData->Geometries, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType,
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
          }
#line 7079 "def.tab.c" /* yacc.c:1646  */
    break;

  case 514:
#line 3106 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 7085 "def.tab.c" /* yacc.c:1646  */
    break;

  case 515:
#line 3107 "def.y" /* yacc.c:1646  */
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
              defData->Net.addRect((yyvsp[-2].string), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType, defData->specialWire_routeStatusName);
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
          }
#line 7122 "def.tab.c" /* yacc.c:1646  */
    break;

  case 516:
#line 3139 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 7128 "def.tab.c" /* yacc.c:1646  */
    break;

  case 517:
#line 3140 "def.y" /* yacc.c:1646  */
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
        }
#line 7147 "def.tab.c" /* yacc.c:1646  */
    break;

  case 518:
#line 3155 "def.y" /* yacc.c:1646  */
    {
          if (defData->VersionNum >= 5.8 && defCallbacks->SNetCbk) {
              defData->Net.addPts((yyvsp[-4].string), (yyvsp[-3].integer), &defData->Geometries, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType,
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
        }
#line 7168 "def.tab.c" /* yacc.c:1646  */
    break;

  case 519:
#line 3173 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->SNetCbk) defData->Net.setSource((yyvsp[0].string)); }
#line 7174 "def.tab.c" /* yacc.c:1646  */
    break;

  case 520:
#line 3176 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->SNetCbk) defData->Net.setFixedbump(); }
#line 7180 "def.tab.c" /* yacc.c:1646  */
    break;

  case 521:
#line 3179 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->SNetCbk) defData->Net.setFrequency((yyvsp[0].dval)); }
#line 7186 "def.tab.c" /* yacc.c:1646  */
    break;

  case 522:
#line 3181 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 7192 "def.tab.c" /* yacc.c:1646  */
    break;

  case 523:
#line 3182 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->SNetCbk) defData->Net.setOriginal((yyvsp[0].string)); }
#line 7198 "def.tab.c" /* yacc.c:1646  */
    break;

  case 524:
#line 3185 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->SNetCbk) defData->Net.setPattern((yyvsp[0].string)); }
#line 7204 "def.tab.c" /* yacc.c:1646  */
    break;

  case 525:
#line 3188 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->SNetCbk) defData->Net.setWeight(ROUND((yyvsp[0].dval))); }
#line 7210 "def.tab.c" /* yacc.c:1646  */
    break;

  case 526:
#line 3191 "def.y" /* yacc.c:1646  */
    { 
              // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
              if (defData->VersionNum < 5.5)
                 if (defCallbacks->SNetCbk) defData->Net.setCap((yyvsp[0].dval));
              else
                 defWarning(7024, "The ESTCAP statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
            }
#line 7222 "def.tab.c" /* yacc.c:1646  */
    break;

  case 527:
#line 3200 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->SNetCbk) defData->Net.setUse((yyvsp[0].string)); }
#line 7228 "def.tab.c" /* yacc.c:1646  */
    break;

  case 528:
#line 3203 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->SNetCbk) defData->Net.setStyle((int)(yyvsp[0].dval)); }
#line 7234 "def.tab.c" /* yacc.c:1646  */
    break;

  case 529:
#line 3205 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 7240 "def.tab.c" /* yacc.c:1646  */
    break;

  case 530:
#line 3207 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 7246 "def.tab.c" /* yacc.c:1646  */
    break;

  case 531:
#line 3210 "def.y" /* yacc.c:1646  */
    { CALLBACK(defCallbacks->NetExtCbk, defrNetExtCbkType, &defData->History_text[0]); }
#line 7252 "def.tab.c" /* yacc.c:1646  */
    break;

  case 532:
#line 3213 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 0; }
#line 7258 "def.tab.c" /* yacc.c:1646  */
    break;

  case 533:
#line 3214 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 0;}
#line 7264 "def.tab.c" /* yacc.c:1646  */
    break;

  case 534:
#line 3215 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 1;}
#line 7270 "def.tab.c" /* yacc.c:1646  */
    break;

  case 535:
#line 3216 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 2;}
#line 7276 "def.tab.c" /* yacc.c:1646  */
    break;

  case 536:
#line 3217 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 3;}
#line 7282 "def.tab.c" /* yacc.c:1646  */
    break;

  case 537:
#line 3218 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 4;}
#line 7288 "def.tab.c" /* yacc.c:1646  */
    break;

  case 538:
#line 3219 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 5;}
#line 7294 "def.tab.c" /* yacc.c:1646  */
    break;

  case 539:
#line 3220 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 6;}
#line 7300 "def.tab.c" /* yacc.c:1646  */
    break;

  case 540:
#line 3221 "def.y" /* yacc.c:1646  */
    {(yyval.integer) = 7;}
#line 7306 "def.tab.c" /* yacc.c:1646  */
    break;

  case 541:
#line 3224 "def.y" /* yacc.c:1646  */
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
            }
#line 7324 "def.tab.c" /* yacc.c:1646  */
    break;

  case 542:
#line 3238 "def.y" /* yacc.c:1646  */
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
            }
#line 7349 "def.tab.c" /* yacc.c:1646  */
    break;

  case 543:
#line 3259 "def.y" /* yacc.c:1646  */
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
            }
#line 7387 "def.tab.c" /* yacc.c:1646  */
    break;

  case 544:
#line 3293 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 7393 "def.tab.c" /* yacc.c:1646  */
    break;

  case 545:
#line 3294 "def.y" /* yacc.c:1646  */
    {
              // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
              if (defData->VersionNum < 5.5)
                 if (defCallbacks->SNetCbk) defData->Net.setWidth((yyvsp[-1].string), (yyvsp[0].dval));
              else
                 defWarning(7026, "The WIDTH statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
            }
#line 7405 "def.tab.c" /* yacc.c:1646  */
    break;

  case 546:
#line 3302 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 7411 "def.tab.c" /* yacc.c:1646  */
    break;

  case 547:
#line 3303 "def.y" /* yacc.c:1646  */
    {
              if (numIsInt((yyvsp[0].string))) {
                 if (defCallbacks->SNetCbk) defData->Net.setVoltage(atoi((yyvsp[0].string)));
              } else {
                 if (defCallbacks->SNetCbk) {
                   if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                     defData->defMsg = (char*)defMalloc(1000);
                     sprintf (defData->defMsg,
                        "The value %s for statement VOLTAGE is invalid. The value can only be integer.\nSpecify a valid value in units of millivolts", (yyvsp[0].string));
                     defError(6537, defData->defMsg);
                     defFree(defData->defMsg);
                     CHKERR();
                   }
                 }
              }
            }
#line 7432 "def.tab.c" /* yacc.c:1646  */
    break;

  case 548:
#line 3320 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 7438 "def.tab.c" /* yacc.c:1646  */
    break;

  case 549:
#line 3321 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->SNetCbk) defData->Net.setSpacing((yyvsp[-1].string),(yyvsp[0].dval));
            }
#line 7446 "def.tab.c" /* yacc.c:1646  */
    break;

  case 550:
#line 3325 "def.y" /* yacc.c:1646  */
    {
            }
#line 7453 "def.tab.c" /* yacc.c:1646  */
    break;

  case 553:
#line 3333 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->SNetCbk) {
                char propTp;
                char* str = ringCopy("                       ");
                propTp = defSettings->SNetProp.propType((yyvsp[-1].string));
                CHKPROPTYPE(propTp, (yyvsp[-1].string), "SPECIAL NET");
                // For backword compatibility, also set the string value 
                sprintf(str, "%g", (yyvsp[0].dval));
                defData->Net.addNumProp((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
              }
            }
#line 7469 "def.tab.c" /* yacc.c:1646  */
    break;

  case 554:
#line 3345 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->SNetCbk) {
                char propTp;
                propTp = defSettings->SNetProp.propType((yyvsp[-1].string));
                CHKPROPTYPE(propTp, (yyvsp[-1].string), "SPECIAL NET");
                defData->Net.addProp((yyvsp[-1].string), (yyvsp[0].string), propTp);
              }
            }
#line 7482 "def.tab.c" /* yacc.c:1646  */
    break;

  case 555:
#line 3354 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->SNetCbk) {
                char propTp;
                propTp = defSettings->SNetProp.propType((yyvsp[-1].string));
                CHKPROPTYPE(propTp, (yyvsp[-1].string), "SPECIAL NET");
                defData->Net.addProp((yyvsp[-1].string), (yyvsp[0].string), propTp);
              }
            }
#line 7495 "def.tab.c" /* yacc.c:1646  */
    break;

  case 557:
#line 3365 "def.y" /* yacc.c:1646  */
    {
              if (defCallbacks->SNetCbk) defData->Net.setRange((yyvsp[-1].dval),(yyvsp[0].dval));
            }
#line 7503 "def.tab.c" /* yacc.c:1646  */
    break;

  case 559:
#line 3371 "def.y" /* yacc.c:1646  */
    { defData->Prop.setRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 7509 "def.tab.c" /* yacc.c:1646  */
    break;

  case 560:
#line 3374 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"BALANCED"; }
#line 7515 "def.tab.c" /* yacc.c:1646  */
    break;

  case 561:
#line 3376 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"STEINER"; }
#line 7521 "def.tab.c" /* yacc.c:1646  */
    break;

  case 562:
#line 3378 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"TRUNK"; }
#line 7527 "def.tab.c" /* yacc.c:1646  */
    break;

  case 563:
#line 3380 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"WIREDLOGIC"; }
#line 7533 "def.tab.c" /* yacc.c:1646  */
    break;

  case 564:
#line 3384 "def.y" /* yacc.c:1646  */
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
      }
#line 7554 "def.tab.c" /* yacc.c:1646  */
    break;

  case 565:
#line 3401 "def.y" /* yacc.c:1646  */
    { }
#line 7560 "def.tab.c" /* yacc.c:1646  */
    break;

  case 566:
#line 3403 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; }
#line 7566 "def.tab.c" /* yacc.c:1646  */
    break;

  case 567:
#line 3404 "def.y" /* yacc.c:1646  */
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
      }
#line 7587 "def.tab.c" /* yacc.c:1646  */
    break;

  case 568:
#line 3422 "def.y" /* yacc.c:1646  */
    { if (defData->NeedPathData && defCallbacks->SNetCbk)
           defData->PathObj.addLayer((yyvsp[0].string));
        defData->dumb_mode = 0; defData->no_num = 0;
      }
#line 7596 "def.tab.c" /* yacc.c:1646  */
    break;

  case 569:
#line 3430 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE;
         defData->orient_is_keyword = TRUE; defData->rect_is_keyword = TRUE, defData->mask_is_keyword = TRUE; 
         defData->virtual_is_keyword = TRUE;  }
#line 7605 "def.tab.c" /* yacc.c:1646  */
    break;

  case 570:
#line 3436 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->rect_is_keyword = FALSE, defData->mask_is_keyword = FALSE, defData->virtual_is_keyword = FALSE; }
#line 7611 "def.tab.c" /* yacc.c:1646  */
    break;

  case 571:
#line 3439 "def.y" /* yacc.c:1646  */
    { if (defData->NeedPathData && defCallbacks->SNetCbk)
          defData->PathObj.addWidth(ROUND((yyvsp[0].dval)));
      }
#line 7619 "def.tab.c" /* yacc.c:1646  */
    break;

  case 572:
#line 3444 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->SNetStartCbk)
          CALLBACK(defCallbacks->SNetStartCbk, defrSNetStartCbkType, ROUND((yyvsp[-1].dval)));
        defData->netOsnet = 2;
      }
#line 7629 "def.tab.c" /* yacc.c:1646  */
    break;

  case 573:
#line 3451 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->SNetEndCbk)
          CALLBACK(defCallbacks->SNetEndCbk, defrSNetEndCbkType, 0);
        defData->netOsnet = 0;
      }
#line 7639 "def.tab.c" /* yacc.c:1646  */
    break;

  case 575:
#line 3461 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->GroupsStartCbk)
           CALLBACK(defCallbacks->GroupsStartCbk, defrGroupsStartCbkType, ROUND((yyvsp[-1].dval)));
      }
#line 7648 "def.tab.c" /* yacc.c:1646  */
    break;

  case 578:
#line 3471 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->GroupCbk)
           CALLBACK(defCallbacks->GroupCbk, defrGroupCbkType, &defData->Group);
      }
#line 7657 "def.tab.c" /* yacc.c:1646  */
    break;

  case 579:
#line 3476 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 7663 "def.tab.c" /* yacc.c:1646  */
    break;

  case 580:
#line 3477 "def.y" /* yacc.c:1646  */
    {
        defData->dumb_mode = DEF_MAX_INT;
        defData->no_num = DEF_MAX_INT;
        /* dumb_mode is automatically turned off at the first
         * + in the options or at the ; at the end of the group */
        if (defCallbacks->GroupCbk) defData->Group.setup((yyvsp[0].string));
        if (defCallbacks->GroupNameCbk)
           CALLBACK(defCallbacks->GroupNameCbk, defrGroupNameCbkType, (yyvsp[0].string));
      }
#line 7677 "def.tab.c" /* yacc.c:1646  */
    break;

  case 582:
#line 3489 "def.y" /* yacc.c:1646  */
    {  }
#line 7683 "def.tab.c" /* yacc.c:1646  */
    break;

  case 583:
#line 3492 "def.y" /* yacc.c:1646  */
    {
        // if (defCallbacks->GroupCbk) defData->Group.addMember($1); 
        if (defCallbacks->GroupMemberCbk)
          CALLBACK(defCallbacks->GroupMemberCbk, defrGroupMemberCbkType, (yyvsp[0].string));
      }
#line 7693 "def.tab.c" /* yacc.c:1646  */
    break;

  case 586:
#line 3503 "def.y" /* yacc.c:1646  */
    { }
#line 7699 "def.tab.c" /* yacc.c:1646  */
    break;

  case 587:
#line 3504 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 7705 "def.tab.c" /* yacc.c:1646  */
    break;

  case 588:
#line 3506 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 7711 "def.tab.c" /* yacc.c:1646  */
    break;

  case 589:
#line 3507 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1;  defData->no_num = 1; }
#line 7717 "def.tab.c" /* yacc.c:1646  */
    break;

  case 590:
#line 3508 "def.y" /* yacc.c:1646  */
    { }
#line 7723 "def.tab.c" /* yacc.c:1646  */
    break;

  case 591:
#line 3510 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->GroupMemberCbk)
          CALLBACK(defCallbacks->GroupExtCbk, defrGroupExtCbkType, &defData->History_text[0]);
      }
#line 7732 "def.tab.c" /* yacc.c:1646  */
    break;

  case 592:
#line 3516 "def.y" /* yacc.c:1646  */
    {
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5) {
          if (defCallbacks->GroupCbk)
            defData->Group.addRegionRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
        }
        else
          defWarning(7027, "The GROUP REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
#line 7746 "def.tab.c" /* yacc.c:1646  */
    break;

  case 593:
#line 3526 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->GroupCbk)
          defData->Group.setRegionName((yyvsp[0].string));
      }
#line 7754 "def.tab.c" /* yacc.c:1646  */
    break;

  case 596:
#line 3535 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->GroupCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defSettings->GroupProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "GROUP");
          sprintf(str, "%g", (yyvsp[0].dval));
          defData->Group.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
        }
      }
#line 7769 "def.tab.c" /* yacc.c:1646  */
    break;

  case 597:
#line 3546 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->GroupCbk) {
          char propTp;
          propTp = defSettings->GroupProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "GROUP");
          defData->Group.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 7782 "def.tab.c" /* yacc.c:1646  */
    break;

  case 598:
#line 3555 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->GroupCbk) {
          char propTp;
          propTp = defSettings->GroupProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "GROUP");
          defData->Group.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 7795 "def.tab.c" /* yacc.c:1646  */
    break;

  case 600:
#line 3566 "def.y" /* yacc.c:1646  */
    { }
#line 7801 "def.tab.c" /* yacc.c:1646  */
    break;

  case 601:
#line 3569 "def.y" /* yacc.c:1646  */
    {
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setMaxX(ROUND((yyvsp[0].dval)));
        else
          defWarning(7028, "The GROUP SOFT MAXX statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
#line 7813 "def.tab.c" /* yacc.c:1646  */
    break;

  case 602:
#line 3577 "def.y" /* yacc.c:1646  */
    { 
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setMaxY(ROUND((yyvsp[0].dval)));
        else
          defWarning(7029, "The GROUP SOFT MAXY statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
#line 7825 "def.tab.c" /* yacc.c:1646  */
    break;

  case 603:
#line 3585 "def.y" /* yacc.c:1646  */
    { 
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setPerim(ROUND((yyvsp[0].dval)));
        else
          defWarning(7030, "The GROUP SOFT MAXHALFPERIMETER statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
#line 7837 "def.tab.c" /* yacc.c:1646  */
    break;

  case 604:
#line 3594 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->GroupsEndCbk)
          CALLBACK(defCallbacks->GroupsEndCbk, defrGroupsEndCbkType, 0);
      }
#line 7846 "def.tab.c" /* yacc.c:1646  */
    break;

  case 607:
#line 3608 "def.y" /* yacc.c:1646  */
    {
        if ((defData->VersionNum < 5.4) && (defCallbacks->AssertionsStartCbk)) {
          CALLBACK(defCallbacks->AssertionsStartCbk, defrAssertionsStartCbkType,
                   ROUND((yyvsp[-1].dval)));
        } else {
          if (defCallbacks->AssertionCbk)
            if (defData->assertionWarnings++ < defSettings->AssertionWarnings)
              defWarning(7031, "The ASSERTIONS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
        if (defCallbacks->AssertionCbk)
          defData->Assertion.setAssertionMode();
      }
#line 7863 "def.tab.c" /* yacc.c:1646  */
    break;

  case 608:
#line 3622 "def.y" /* yacc.c:1646  */
    {
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintsStartCbk)) {
          CALLBACK(defCallbacks->ConstraintsStartCbk, defrConstraintsStartCbkType,
                   ROUND((yyvsp[-1].dval)));
        } else {
          if (defCallbacks->ConstraintCbk)
            if (defData->constraintWarnings++ < defSettings->ConstraintWarnings)
              defWarning(7032, "The CONSTRAINTS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
        if (defCallbacks->ConstraintCbk)
          defData->Assertion.setConstraintMode();
      }
#line 7880 "def.tab.c" /* yacc.c:1646  */
    break;

  case 612:
#line 3641 "def.y" /* yacc.c:1646  */
    {
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)) {
          if (defData->Assertion.isConstraint()) 
            CALLBACK(defCallbacks->ConstraintCbk, defrConstraintCbkType, &defData->Assertion);
          if (defData->Assertion.isAssertion()) 
            CALLBACK(defCallbacks->AssertionCbk, defrAssertionCbkType, &defData->Assertion);
        }
      }
#line 7893 "def.tab.c" /* yacc.c:1646  */
    break;

  case 613:
#line 3651 "def.y" /* yacc.c:1646  */
    { 
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)) {
          if (defData->Assertion.isConstraint()) 
            CALLBACK(defCallbacks->ConstraintCbk, defrConstraintCbkType, &defData->Assertion);
          if (defData->Assertion.isAssertion()) 
            CALLBACK(defCallbacks->AssertionCbk, defrAssertionCbkType, &defData->Assertion);
        }
   
        // reset all the flags and everything
        defData->Assertion.clear();
      }
#line 7909 "def.tab.c" /* yacc.c:1646  */
    break;

  case 614:
#line 3663 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 7915 "def.tab.c" /* yacc.c:1646  */
    break;

  case 615:
#line 3664 "def.y" /* yacc.c:1646  */
    {
         if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.addNet((yyvsp[0].string));
      }
#line 7924 "def.tab.c" /* yacc.c:1646  */
    break;

  case 616:
#line 3668 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 4; defData->no_num = 4;}
#line 7930 "def.tab.c" /* yacc.c:1646  */
    break;

  case 617:
#line 3669 "def.y" /* yacc.c:1646  */
    {
         if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.addPath((yyvsp[-3].string), (yyvsp[-2].string), (yyvsp[-1].string), (yyvsp[0].string));
      }
#line 7939 "def.tab.c" /* yacc.c:1646  */
    break;

  case 618:
#line 3674 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.setSum();
      }
#line 7948 "def.tab.c" /* yacc.c:1646  */
    break;

  case 619:
#line 3679 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.setDiff();
      }
#line 7957 "def.tab.c" /* yacc.c:1646  */
    break;

  case 621:
#line 3686 "def.y" /* yacc.c:1646  */
    { }
#line 7963 "def.tab.c" /* yacc.c:1646  */
    break;

  case 623:
#line 3689 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 7969 "def.tab.c" /* yacc.c:1646  */
    break;

  case 624:
#line 3691 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setWiredlogic((yyvsp[-4].string), (yyvsp[-1].dval));
      }
#line 7978 "def.tab.c" /* yacc.c:1646  */
    break;

  case 625:
#line 3698 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)""; }
#line 7984 "def.tab.c" /* yacc.c:1646  */
    break;

  case 626:
#line 3700 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"+"; }
#line 7990 "def.tab.c" /* yacc.c:1646  */
    break;

  case 629:
#line 3707 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setRiseMin((yyvsp[0].dval));
      }
#line 7999 "def.tab.c" /* yacc.c:1646  */
    break;

  case 630:
#line 3712 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setRiseMax((yyvsp[0].dval));
      }
#line 8008 "def.tab.c" /* yacc.c:1646  */
    break;

  case 631:
#line 3717 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setFallMin((yyvsp[0].dval));
      }
#line 8017 "def.tab.c" /* yacc.c:1646  */
    break;

  case 632:
#line 3722 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setFallMax((yyvsp[0].dval));
      }
#line 8026 "def.tab.c" /* yacc.c:1646  */
    break;

  case 633:
#line 3728 "def.y" /* yacc.c:1646  */
    { if ((defData->VersionNum < 5.4) && defCallbacks->ConstraintsEndCbk) {
          CALLBACK(defCallbacks->ConstraintsEndCbk, defrConstraintsEndCbkType, 0);
        } else {
          if (defCallbacks->ConstraintsEndCbk) {
            if (defData->constraintWarnings++ < defSettings->ConstraintWarnings)
              defWarning(7032, "The CONSTRAINTS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
      }
#line 8040 "def.tab.c" /* yacc.c:1646  */
    break;

  case 634:
#line 3739 "def.y" /* yacc.c:1646  */
    { if ((defData->VersionNum < 5.4) && defCallbacks->AssertionsEndCbk) {
          CALLBACK(defCallbacks->AssertionsEndCbk, defrAssertionsEndCbkType, 0);
        } else {
          if (defCallbacks->AssertionsEndCbk) {
            if (defData->assertionWarnings++ < defSettings->AssertionWarnings)
              defWarning(7031, "The ASSERTIONS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
      }
#line 8054 "def.tab.c" /* yacc.c:1646  */
    break;

  case 636:
#line 3753 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->ScanchainsStartCbk)
          CALLBACK(defCallbacks->ScanchainsStartCbk, defrScanchainsStartCbkType,
                   ROUND((yyvsp[-1].dval)));
      }
#line 8063 "def.tab.c" /* yacc.c:1646  */
    break;

  case 638:
#line 3760 "def.y" /* yacc.c:1646  */
    {}
#line 8069 "def.tab.c" /* yacc.c:1646  */
    break;

  case 639:
#line 3763 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->ScanchainCbk)
          CALLBACK(defCallbacks->ScanchainCbk, defrScanchainCbkType, &defData->Scanchain);
      }
#line 8078 "def.tab.c" /* yacc.c:1646  */
    break;

  case 640:
#line 3768 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 8084 "def.tab.c" /* yacc.c:1646  */
    break;

  case 641:
#line 3769 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setName((yyvsp[0].string));
        defData->bit_is_keyword = TRUE;
      }
#line 8094 "def.tab.c" /* yacc.c:1646  */
    break;

  case 644:
#line 3781 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)""; }
#line 8100 "def.tab.c" /* yacc.c:1646  */
    break;

  case 645:
#line 3783 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string); }
#line 8106 "def.tab.c" /* yacc.c:1646  */
    break;

  case 646:
#line 3785 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 2; defData->no_num = 2;}
#line 8112 "def.tab.c" /* yacc.c:1646  */
    break;

  case 647:
#line 3786 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setStart((yyvsp[-1].string), (yyvsp[0].string));
      }
#line 8120 "def.tab.c" /* yacc.c:1646  */
    break;

  case 648:
#line 3789 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8126 "def.tab.c" /* yacc.c:1646  */
    break;

  case 649:
#line 3790 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->no_num = 0; }
#line 8132 "def.tab.c" /* yacc.c:1646  */
    break;

  case 650:
#line 3792 "def.y" /* yacc.c:1646  */
    {
         defData->dumb_mode = 1;
         defData->no_num = 1;
         if (defCallbacks->ScanchainCbk)
           defData->Scanchain.addOrderedList();
      }
#line 8143 "def.tab.c" /* yacc.c:1646  */
    break;

  case 651:
#line 3799 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->no_num = 0; }
#line 8149 "def.tab.c" /* yacc.c:1646  */
    break;

  case 652:
#line 3800 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 2; defData->no_num = 2; }
#line 8155 "def.tab.c" /* yacc.c:1646  */
    break;

  case 653:
#line 3801 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setStop((yyvsp[-1].string), (yyvsp[0].string));
      }
#line 8163 "def.tab.c" /* yacc.c:1646  */
    break;

  case 654:
#line 3804 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 10; defData->no_num = 10; }
#line 8169 "def.tab.c" /* yacc.c:1646  */
    break;

  case 655:
#line 3805 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0;  defData->no_num = 0; }
#line 8175 "def.tab.c" /* yacc.c:1646  */
    break;

  case 656:
#line 3806 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8181 "def.tab.c" /* yacc.c:1646  */
    break;

  case 657:
#line 3808 "def.y" /* yacc.c:1646  */
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
          defData->Scanchain.setPartition((yyvsp[-1].string), (yyvsp[0].integer));
      }
#line 8202 "def.tab.c" /* yacc.c:1646  */
    break;

  case 658:
#line 3825 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ScanChainExtCbk)
          CALLBACK(defCallbacks->ScanChainExtCbk, defrScanChainExtCbkType, &defData->History_text[0]);
      }
#line 8211 "def.tab.c" /* yacc.c:1646  */
    break;

  case 659:
#line 3831 "def.y" /* yacc.c:1646  */
    { }
#line 8217 "def.tab.c" /* yacc.c:1646  */
    break;

  case 660:
#line 3833 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.setCommonIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.setCommonOut((yyvsp[-1].string));
        }
      }
#line 8230 "def.tab.c" /* yacc.c:1646  */
    break;

  case 661:
#line 3842 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
            defData->Scanchain.setCommonIn((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
            defData->Scanchain.setCommonOut((yyvsp[-5].string));
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.setCommonIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.setCommonOut((yyvsp[-1].string));
        }
      }
#line 8247 "def.tab.c" /* yacc.c:1646  */
    break;

  case 664:
#line 3860 "def.y" /* yacc.c:1646  */
    {
        defData->dumb_mode = 1000;
        defData->no_num = 1000;
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.addFloatingInst((yyvsp[0].string));
      }
#line 8258 "def.tab.c" /* yacc.c:1646  */
    break;

  case 665:
#line 3867 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8264 "def.tab.c" /* yacc.c:1646  */
    break;

  case 666:
#line 3870 "def.y" /* yacc.c:1646  */
    { }
#line 8270 "def.tab.c" /* yacc.c:1646  */
    break;

  case 667:
#line 3872 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      }
#line 8287 "def.tab.c" /* yacc.c:1646  */
    break;

  case 668:
#line 3885 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "BITS") == 0 || strcmp((yyvsp[-6].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-5].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      }
#line 8312 "def.tab.c" /* yacc.c:1646  */
    break;

  case 669:
#line 3907 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-10].string), "IN") == 0 || strcmp((yyvsp[-10].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-9].string));
          else if (strcmp((yyvsp[-10].string), "OUT") == 0 || strcmp((yyvsp[-10].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-9].string));
          else if (strcmp((yyvsp[-10].string), "BITS") == 0 || strcmp((yyvsp[-10].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-9].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "BITS") == 0 || strcmp((yyvsp[-6].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-5].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      }
#line 8345 "def.tab.c" /* yacc.c:1646  */
    break;

  case 672:
#line 3941 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1000; defData->no_num = 1000; 
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.addOrderedInst((yyvsp[0].string));
      }
#line 8354 "def.tab.c" /* yacc.c:1646  */
    break;

  case 673:
#line 3946 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8360 "def.tab.c" /* yacc.c:1646  */
    break;

  case 674:
#line 3949 "def.y" /* yacc.c:1646  */
    { }
#line 8366 "def.tab.c" /* yacc.c:1646  */
    break;

  case 675:
#line 3951 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
         }
        }
      }
#line 8383 "def.tab.c" /* yacc.c:1646  */
    break;

  case 676:
#line 3964 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "BITS") == 0 || strcmp((yyvsp[-6].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-5].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
        }
      }
#line 8408 "def.tab.c" /* yacc.c:1646  */
    break;

  case 677:
#line 3986 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-10].string), "IN") == 0 || strcmp((yyvsp[-10].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-9].string));
          else if (strcmp((yyvsp[-10].string), "OUT") == 0 || strcmp((yyvsp[-10].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-9].string));
          else if (strcmp((yyvsp[-10].string), "BITS") == 0 || strcmp((yyvsp[-10].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-9].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "BITS") == 0 || strcmp((yyvsp[-6].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-5].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
        }
      }
#line 8441 "def.tab.c" /* yacc.c:1646  */
    break;

  case 678:
#line 4016 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = -1; }
#line 8447 "def.tab.c" /* yacc.c:1646  */
    break;

  case 679:
#line 4018 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = ROUND((yyvsp[0].dval)); }
#line 8453 "def.tab.c" /* yacc.c:1646  */
    break;

  case 680:
#line 4021 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->ScanchainsEndCbk)
          CALLBACK(defCallbacks->ScanchainsEndCbk, defrScanchainsEndCbkType, 0);
        defData->bit_is_keyword = FALSE;
        defData->dumb_mode = 0; defData->no_num = 0;
      }
#line 8464 "def.tab.c" /* yacc.c:1646  */
    break;

  case 682:
#line 4033 "def.y" /* yacc.c:1646  */
    {
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingsStartCbk) {
          CALLBACK(defCallbacks->IOTimingsStartCbk, defrIOTimingsStartCbkType, ROUND((yyvsp[-1].dval)));
        } else {
          if (defCallbacks->IOTimingsStartCbk)
            if (defData->iOTimingWarnings++ < defSettings->IOTimingWarnings)
              defWarning(7035, "The IOTIMINGS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
      }
#line 8478 "def.tab.c" /* yacc.c:1646  */
    break;

  case 684:
#line 4045 "def.y" /* yacc.c:1646  */
    { }
#line 8484 "def.tab.c" /* yacc.c:1646  */
    break;

  case 685:
#line 4048 "def.y" /* yacc.c:1646  */
    { 
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingCbk)
          CALLBACK(defCallbacks->IOTimingCbk, defrIOTimingCbkType, &defData->IOTiming);
      }
#line 8493 "def.tab.c" /* yacc.c:1646  */
    break;

  case 686:
#line 4053 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 2; defData->no_num = 2; }
#line 8499 "def.tab.c" /* yacc.c:1646  */
    break;

  case 687:
#line 4054 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setName((yyvsp[-2].string), (yyvsp[-1].string));
      }
#line 8508 "def.tab.c" /* yacc.c:1646  */
    break;

  case 690:
#line 4065 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setVariable((yyvsp[-3].string), (yyvsp[-1].dval), (yyvsp[0].dval));
      }
#line 8517 "def.tab.c" /* yacc.c:1646  */
    break;

  case 691:
#line 4070 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setSlewRate((yyvsp[-3].string), (yyvsp[-1].dval), (yyvsp[0].dval));
      }
#line 8526 "def.tab.c" /* yacc.c:1646  */
    break;

  case 692:
#line 4075 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setCapacitance((yyvsp[0].dval));
      }
#line 8535 "def.tab.c" /* yacc.c:1646  */
    break;

  case 693:
#line 4079 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1; }
#line 8541 "def.tab.c" /* yacc.c:1646  */
    break;

  case 694:
#line 4080 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setDriveCell((yyvsp[0].string));
      }
#line 8550 "def.tab.c" /* yacc.c:1646  */
    break;

  case 696:
#line 4089 "def.y" /* yacc.c:1646  */
    {
        if (defData->VersionNum < 5.4 && defCallbacks->IoTimingsExtCbk)
          CALLBACK(defCallbacks->IoTimingsExtCbk, defrIoTimingsExtCbkType, &defData->History_text[0]);
      }
#line 8559 "def.tab.c" /* yacc.c:1646  */
    break;

  case 697:
#line 4095 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1; }
#line 8565 "def.tab.c" /* yacc.c:1646  */
    break;

  case 698:
#line 4096 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setTo((yyvsp[0].string));
      }
#line 8574 "def.tab.c" /* yacc.c:1646  */
    break;

  case 701:
#line 4103 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1; }
#line 8580 "def.tab.c" /* yacc.c:1646  */
    break;

  case 702:
#line 4104 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setFrom((yyvsp[0].string));
      }
#line 8589 "def.tab.c" /* yacc.c:1646  */
    break;

  case 704:
#line 4111 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setParallel((yyvsp[0].dval));
      }
#line 8598 "def.tab.c" /* yacc.c:1646  */
    break;

  case 705:
#line 4116 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"RISE"; }
#line 8604 "def.tab.c" /* yacc.c:1646  */
    break;

  case 706:
#line 4116 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"FALL"; }
#line 8610 "def.tab.c" /* yacc.c:1646  */
    break;

  case 707:
#line 4119 "def.y" /* yacc.c:1646  */
    {
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingsEndCbk)
          CALLBACK(defCallbacks->IOTimingsEndCbk, defrIOTimingsEndCbkType, 0);
      }
#line 8619 "def.tab.c" /* yacc.c:1646  */
    break;

  case 708:
#line 4125 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->FPCEndCbk)
          CALLBACK(defCallbacks->FPCEndCbk, defrFPCEndCbkType, 0);
      }
#line 8628 "def.tab.c" /* yacc.c:1646  */
    break;

  case 709:
#line 4131 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->FPCStartCbk)
          CALLBACK(defCallbacks->FPCStartCbk, defrFPCStartCbkType, ROUND((yyvsp[-1].dval)));
      }
#line 8637 "def.tab.c" /* yacc.c:1646  */
    break;

  case 711:
#line 4138 "def.y" /* yacc.c:1646  */
    {}
#line 8643 "def.tab.c" /* yacc.c:1646  */
    break;

  case 712:
#line 4140 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1;  }
#line 8649 "def.tab.c" /* yacc.c:1646  */
    break;

  case 713:
#line 4141 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FPCCbk) defData->FPC.setName((yyvsp[-1].string), (yyvsp[0].string)); }
#line 8655 "def.tab.c" /* yacc.c:1646  */
    break;

  case 714:
#line 4143 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FPCCbk) CALLBACK(defCallbacks->FPCCbk, defrFPCCbkType, &defData->FPC); }
#line 8661 "def.tab.c" /* yacc.c:1646  */
    break;

  case 715:
#line 4146 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"HORIZONTAL"; }
#line 8667 "def.tab.c" /* yacc.c:1646  */
    break;

  case 716:
#line 4148 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"VERTICAL"; }
#line 8673 "def.tab.c" /* yacc.c:1646  */
    break;

  case 717:
#line 4151 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FPCCbk) defData->FPC.setAlign(); }
#line 8679 "def.tab.c" /* yacc.c:1646  */
    break;

  case 718:
#line 4153 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FPCCbk) defData->FPC.setMax((yyvsp[0].dval)); }
#line 8685 "def.tab.c" /* yacc.c:1646  */
    break;

  case 719:
#line 4155 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FPCCbk) defData->FPC.setMin((yyvsp[0].dval)); }
#line 8691 "def.tab.c" /* yacc.c:1646  */
    break;

  case 720:
#line 4157 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FPCCbk) defData->FPC.setEqual((yyvsp[0].dval)); }
#line 8697 "def.tab.c" /* yacc.c:1646  */
    break;

  case 723:
#line 4164 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FPCCbk) defData->FPC.setDoingBottomLeft(); }
#line 8703 "def.tab.c" /* yacc.c:1646  */
    break;

  case 725:
#line 4167 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FPCCbk) defData->FPC.setDoingTopRight(); }
#line 8709 "def.tab.c" /* yacc.c:1646  */
    break;

  case 729:
#line 4174 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1; }
#line 8715 "def.tab.c" /* yacc.c:1646  */
    break;

  case 730:
#line 4175 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FPCCbk) defData->FPC.addRow((yyvsp[-1].string)); }
#line 8721 "def.tab.c" /* yacc.c:1646  */
    break;

  case 731:
#line 4176 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1; }
#line 8727 "def.tab.c" /* yacc.c:1646  */
    break;

  case 732:
#line 4177 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FPCCbk) defData->FPC.addComps((yyvsp[-1].string)); }
#line 8733 "def.tab.c" /* yacc.c:1646  */
    break;

  case 734:
#line 4184 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->TimingDisablesStartCbk)
          CALLBACK(defCallbacks->TimingDisablesStartCbk, defrTimingDisablesStartCbkType,
                   ROUND((yyvsp[-1].dval)));
      }
#line 8743 "def.tab.c" /* yacc.c:1646  */
    break;

  case 736:
#line 4192 "def.y" /* yacc.c:1646  */
    {}
#line 8749 "def.tab.c" /* yacc.c:1646  */
    break;

  case 737:
#line 4194 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 2; defData->no_num = 2;  }
#line 8755 "def.tab.c" /* yacc.c:1646  */
    break;

  case 738:
#line 4195 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 2; defData->no_num = 2;  }
#line 8761 "def.tab.c" /* yacc.c:1646  */
    break;

  case 739:
#line 4196 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setFromTo((yyvsp[-6].string), (yyvsp[-5].string), (yyvsp[-2].string), (yyvsp[-1].string));
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                &defData->TimingDisable);
        }
      }
#line 8773 "def.tab.c" /* yacc.c:1646  */
    break;

  case 740:
#line 4203 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 2; defData->no_num = 2; }
#line 8779 "def.tab.c" /* yacc.c:1646  */
    break;

  case 741:
#line 4204 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setThru((yyvsp[-2].string), (yyvsp[-1].string));
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                   &defData->TimingDisable);
        }
      }
#line 8791 "def.tab.c" /* yacc.c:1646  */
    break;

  case 742:
#line 4211 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 8797 "def.tab.c" /* yacc.c:1646  */
    break;

  case 743:
#line 4212 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setMacro((yyvsp[-2].string));
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                &defData->TimingDisable);
        }
      }
#line 8809 "def.tab.c" /* yacc.c:1646  */
    break;

  case 744:
#line 4220 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setReentrantPathsFlag();
      }
#line 8817 "def.tab.c" /* yacc.c:1646  */
    break;

  case 745:
#line 4225 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 8823 "def.tab.c" /* yacc.c:1646  */
    break;

  case 746:
#line 4226 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=1; defData->no_num = 1;}
#line 8829 "def.tab.c" /* yacc.c:1646  */
    break;

  case 747:
#line 4227 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setMacroFromTo((yyvsp[-3].string),(yyvsp[0].string));
      }
#line 8838 "def.tab.c" /* yacc.c:1646  */
    break;

  case 748:
#line 4231 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=1; defData->no_num = 1;}
#line 8844 "def.tab.c" /* yacc.c:1646  */
    break;

  case 749:
#line 4232 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setMacroThru((yyvsp[0].string));
      }
#line 8853 "def.tab.c" /* yacc.c:1646  */
    break;

  case 750:
#line 4238 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->TimingDisablesEndCbk)
          CALLBACK(defCallbacks->TimingDisablesEndCbk, defrTimingDisablesEndCbkType, 0);
      }
#line 8862 "def.tab.c" /* yacc.c:1646  */
    break;

  case 752:
#line 4248 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionsStartCbk)
          CALLBACK(defCallbacks->PartitionsStartCbk, defrPartitionsStartCbkType,
                   ROUND((yyvsp[-1].dval)));
      }
#line 8872 "def.tab.c" /* yacc.c:1646  */
    break;

  case 754:
#line 4256 "def.y" /* yacc.c:1646  */
    { }
#line 8878 "def.tab.c" /* yacc.c:1646  */
    break;

  case 755:
#line 4259 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->PartitionCbk)
          CALLBACK(defCallbacks->PartitionCbk, defrPartitionCbkType, &defData->Partition);
      }
#line 8887 "def.tab.c" /* yacc.c:1646  */
    break;

  case 756:
#line 4264 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8893 "def.tab.c" /* yacc.c:1646  */
    break;

  case 757:
#line 4265 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setName((yyvsp[-1].string));
      }
#line 8902 "def.tab.c" /* yacc.c:1646  */
    break;

  case 759:
#line 4272 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.addTurnOff((yyvsp[-1].string), (yyvsp[0].string));
      }
#line 8911 "def.tab.c" /* yacc.c:1646  */
    break;

  case 760:
#line 4278 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)" "; }
#line 8917 "def.tab.c" /* yacc.c:1646  */
    break;

  case 761:
#line 4280 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"R"; }
#line 8923 "def.tab.c" /* yacc.c:1646  */
    break;

  case 762:
#line 4282 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"F"; }
#line 8929 "def.tab.c" /* yacc.c:1646  */
    break;

  case 763:
#line 4285 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)" "; }
#line 8935 "def.tab.c" /* yacc.c:1646  */
    break;

  case 764:
#line 4287 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"R"; }
#line 8941 "def.tab.c" /* yacc.c:1646  */
    break;

  case 765:
#line 4289 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"F"; }
#line 8947 "def.tab.c" /* yacc.c:1646  */
    break;

  case 768:
#line 4295 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=2; defData->no_num = 2;}
#line 8953 "def.tab.c" /* yacc.c:1646  */
    break;

  case 769:
#line 4297 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromClockPin((yyvsp[-3].string), (yyvsp[-2].string));
      }
#line 8962 "def.tab.c" /* yacc.c:1646  */
    break;

  case 770:
#line 4301 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=2; defData->no_num = 2; }
#line 8968 "def.tab.c" /* yacc.c:1646  */
    break;

  case 771:
#line 4303 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromCompPin((yyvsp[-2].string), (yyvsp[-1].string));
      }
#line 8977 "def.tab.c" /* yacc.c:1646  */
    break;

  case 772:
#line 4307 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=1; defData->no_num = 1; }
#line 8983 "def.tab.c" /* yacc.c:1646  */
    break;

  case 773:
#line 4309 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromIOPin((yyvsp[-1].string));
      }
#line 8992 "def.tab.c" /* yacc.c:1646  */
    break;

  case 774:
#line 4313 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=2; defData->no_num = 2; }
#line 8998 "def.tab.c" /* yacc.c:1646  */
    break;

  case 775:
#line 4315 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToClockPin((yyvsp[-3].string), (yyvsp[-2].string));
      }
#line 9007 "def.tab.c" /* yacc.c:1646  */
    break;

  case 776:
#line 4319 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=2; defData->no_num = 2; }
#line 9013 "def.tab.c" /* yacc.c:1646  */
    break;

  case 777:
#line 4321 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToCompPin((yyvsp[-2].string), (yyvsp[-1].string));
      }
#line 9022 "def.tab.c" /* yacc.c:1646  */
    break;

  case 778:
#line 4325 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=1; defData->no_num = 2; }
#line 9028 "def.tab.c" /* yacc.c:1646  */
    break;

  case 779:
#line 4326 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToIOPin((yyvsp[-1].string));
      }
#line 9037 "def.tab.c" /* yacc.c:1646  */
    break;

  case 780:
#line 4331 "def.y" /* yacc.c:1646  */
    { 
        if (defCallbacks->PartitionsExtCbk)
          CALLBACK(defCallbacks->PartitionsExtCbk, defrPartitionsExtCbkType,
                   &defData->History_text[0]);
      }
#line 9047 "def.tab.c" /* yacc.c:1646  */
    break;

  case 781:
#line 4338 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; }
#line 9053 "def.tab.c" /* yacc.c:1646  */
    break;

  case 782:
#line 4339 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->no_num = 0; }
#line 9059 "def.tab.c" /* yacc.c:1646  */
    break;

  case 784:
#line 4343 "def.y" /* yacc.c:1646  */
    { }
#line 9065 "def.tab.c" /* yacc.c:1646  */
    break;

  case 785:
#line 4346 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setMin((yyvsp[-1].dval), (yyvsp[0].dval));
      }
#line 9074 "def.tab.c" /* yacc.c:1646  */
    break;

  case 786:
#line 4351 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setMax((yyvsp[-1].dval), (yyvsp[0].dval));
      }
#line 9083 "def.tab.c" /* yacc.c:1646  */
    break;

  case 788:
#line 4358 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PartitionCbk) defData->Partition.addPin((yyvsp[0].string)); }
#line 9089 "def.tab.c" /* yacc.c:1646  */
    break;

  case 791:
#line 4364 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PartitionCbk) defData->Partition.addRiseMin((yyvsp[0].dval)); }
#line 9095 "def.tab.c" /* yacc.c:1646  */
    break;

  case 792:
#line 4366 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PartitionCbk) defData->Partition.addFallMin((yyvsp[0].dval)); }
#line 9101 "def.tab.c" /* yacc.c:1646  */
    break;

  case 793:
#line 4368 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PartitionCbk) defData->Partition.addRiseMax((yyvsp[0].dval)); }
#line 9107 "def.tab.c" /* yacc.c:1646  */
    break;

  case 794:
#line 4370 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PartitionCbk) defData->Partition.addFallMax((yyvsp[0].dval)); }
#line 9113 "def.tab.c" /* yacc.c:1646  */
    break;

  case 797:
#line 4378 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PartitionCbk)
          defData->Partition.addRiseMinRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 9120 "def.tab.c" /* yacc.c:1646  */
    break;

  case 798:
#line 4381 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PartitionCbk)
          defData->Partition.addFallMinRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 9127 "def.tab.c" /* yacc.c:1646  */
    break;

  case 799:
#line 4384 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PartitionCbk)
          defData->Partition.addRiseMaxRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 9134 "def.tab.c" /* yacc.c:1646  */
    break;

  case 800:
#line 4387 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PartitionCbk)
          defData->Partition.addFallMaxRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 9141 "def.tab.c" /* yacc.c:1646  */
    break;

  case 801:
#line 4391 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PartitionsEndCbk)
          CALLBACK(defCallbacks->PartitionsEndCbk, defrPartitionsEndCbkType, 0); }
#line 9148 "def.tab.c" /* yacc.c:1646  */
    break;

  case 803:
#line 4396 "def.y" /* yacc.c:1646  */
    { }
#line 9154 "def.tab.c" /* yacc.c:1646  */
    break;

  case 804:
#line 4398 "def.y" /* yacc.c:1646  */
    {defData->dumb_mode=2; defData->no_num = 2; }
#line 9160 "def.tab.c" /* yacc.c:1646  */
    break;

  case 805:
#line 4400 "def.y" /* yacc.c:1646  */
    {
        // note that the defData->first T_STRING could be the keyword VPIN 
        if (defCallbacks->NetCbk)
          defData->Subnet->addPin((yyvsp[-3].string), (yyvsp[-2].string), (yyvsp[-1].integer));
      }
#line 9170 "def.tab.c" /* yacc.c:1646  */
    break;

  case 806:
#line 4407 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 0; }
#line 9176 "def.tab.c" /* yacc.c:1646  */
    break;

  case 807:
#line 4409 "def.y" /* yacc.c:1646  */
    { (yyval.integer) = 1; }
#line 9182 "def.tab.c" /* yacc.c:1646  */
    break;

  case 810:
#line 4415 "def.y" /* yacc.c:1646  */
    {  
        if (defCallbacks->NetCbk) defData->Subnet->addWire((yyvsp[0].string));
      }
#line 9190 "def.tab.c" /* yacc.c:1646  */
    break;

  case 811:
#line 4419 "def.y" /* yacc.c:1646  */
    {  
        defData->by_is_keyword = FALSE;
        defData->do_is_keyword = FALSE;
        defData->new_is_keyword = FALSE;
        defData->step_is_keyword = FALSE;
        defData->orient_is_keyword = FALSE;
        defData->needNPCbk = 0;
      }
#line 9203 "def.tab.c" /* yacc.c:1646  */
    break;

  case 812:
#line 4427 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9209 "def.tab.c" /* yacc.c:1646  */
    break;

  case 813:
#line 4428 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NetCbk) defData->Subnet->setNonDefault((yyvsp[0].string)); }
#line 9215 "def.tab.c" /* yacc.c:1646  */
    break;

  case 814:
#line 4431 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"FIXED"; defData->dumb_mode = 1; }
#line 9221 "def.tab.c" /* yacc.c:1646  */
    break;

  case 815:
#line 4433 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"COVER"; defData->dumb_mode = 1; }
#line 9227 "def.tab.c" /* yacc.c:1646  */
    break;

  case 816:
#line 4435 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"ROUTED"; defData->dumb_mode = 1; }
#line 9233 "def.tab.c" /* yacc.c:1646  */
    break;

  case 817:
#line 4437 "def.y" /* yacc.c:1646  */
    { (yyval.string) = (char*)"NOSHIELD"; defData->dumb_mode = 1; }
#line 9239 "def.tab.c" /* yacc.c:1646  */
    break;

  case 819:
#line 4442 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PinPropStartCbk)
          CALLBACK(defCallbacks->PinPropStartCbk, defrPinPropStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 9246 "def.tab.c" /* yacc.c:1646  */
    break;

  case 820:
#line 4447 "def.y" /* yacc.c:1646  */
    { }
#line 9252 "def.tab.c" /* yacc.c:1646  */
    break;

  case 821:
#line 4449 "def.y" /* yacc.c:1646  */
    { }
#line 9258 "def.tab.c" /* yacc.c:1646  */
    break;

  case 822:
#line 4452 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PinPropEndCbk)
          CALLBACK(defCallbacks->PinPropEndCbk, defrPinPropEndCbkType, 0); }
#line 9265 "def.tab.c" /* yacc.c:1646  */
    break;

  case 825:
#line 4459 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 2; defData->no_num = 2; }
#line 9271 "def.tab.c" /* yacc.c:1646  */
    break;

  case 826:
#line 4460 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PinPropCbk) defData->PinProp.setName((yyvsp[-1].string), (yyvsp[0].string)); }
#line 9277 "def.tab.c" /* yacc.c:1646  */
    break;

  case 827:
#line 4462 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->PinPropCbk) {
          CALLBACK(defCallbacks->PinPropCbk, defrPinPropCbkType, &defData->PinProp);
         // reset the property number
         defData->PinProp.clear();
        }
      }
#line 9288 "def.tab.c" /* yacc.c:1646  */
    break;

  case 830:
#line 4472 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 9294 "def.tab.c" /* yacc.c:1646  */
    break;

  case 831:
#line 4474 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 9300 "def.tab.c" /* yacc.c:1646  */
    break;

  case 834:
#line 4481 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defSettings->CompPinProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "PINPROPERTIES");
          sprintf(str, "%g", (yyvsp[0].dval));
          defData->PinProp.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
        }
      }
#line 9315 "def.tab.c" /* yacc.c:1646  */
    break;

  case 835:
#line 4492 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          propTp = defSettings->CompPinProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "PINPROPERTIES");
          defData->PinProp.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 9328 "def.tab.c" /* yacc.c:1646  */
    break;

  case 836:
#line 4501 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          propTp = defSettings->CompPinProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "PINPROPERTIES");
          defData->PinProp.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 9341 "def.tab.c" /* yacc.c:1646  */
    break;

  case 838:
#line 4513 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->BlockageStartCbk)
          CALLBACK(defCallbacks->BlockageStartCbk, defrBlockageStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 9348 "def.tab.c" /* yacc.c:1646  */
    break;

  case 839:
#line 4517 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->BlockageEndCbk)
          CALLBACK(defCallbacks->BlockageEndCbk, defrBlockageEndCbkType, 0); }
#line 9355 "def.tab.c" /* yacc.c:1646  */
    break;

  case 842:
#line 4526 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->BlockageCbk) {
          CALLBACK(defCallbacks->BlockageCbk, defrBlockageCbkType, &defData->Blockage);
          defData->Blockage.clear();
        }
      }
#line 9366 "def.tab.c" /* yacc.c:1646  */
    break;

  case 843:
#line 4533 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9372 "def.tab.c" /* yacc.c:1646  */
    break;

  case 844:
#line 4534 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->BlockageCbk) {
          if (defData->Blockage.hasPlacement() != 0) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6539, "Invalid BLOCKAGE statement defined in the DEF file. The BLOCKAGE statment has both the LAYER and the PLACEMENT statements defined.\nUpdate your DEF file to have either BLOCKAGE or PLACEMENT statement only.");
              CHKERR();
            }
          }
          defData->Blockage.setLayer((yyvsp[0].string));
          defData->Blockage.clearPoly(); // free poly, if any
        }
        defData->hasBlkLayerComp = 0;
        defData->hasBlkLayerSpac = 0;
        defData->hasBlkLayerTypeComp = 0;
      }
#line 9392 "def.tab.c" /* yacc.c:1646  */
    break;

  case 846:
#line 4553 "def.y" /* yacc.c:1646  */
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
      }
#line 9411 "def.tab.c" /* yacc.c:1646  */
    break;

  case 850:
#line 4574 "def.y" /* yacc.c:1646  */
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
            defData->Blockage.setSpacing((int)(yyvsp[0].dval));
          defData->hasBlkLayerSpac = 1;
        }
      }
#line 9441 "def.tab.c" /* yacc.c:1646  */
    break;

  case 851:
#line 4600 "def.y" /* yacc.c:1646  */
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
            defData->Blockage.setDesignRuleWidth((int)(yyvsp[0].dval));
          defData->hasBlkLayerSpac = 1;
        }
      }
#line 9467 "def.tab.c" /* yacc.c:1646  */
    break;

  case 854:
#line 4626 "def.y" /* yacc.c:1646  */
    {      
        if (validateMaskInput((int)(yyvsp[0].dval), defData->blockageWarnings, defSettings->BlockageWarnings)) {
          defData->Blockage.setMask((int)(yyvsp[0].dval));
        }
      }
#line 9477 "def.tab.c" /* yacc.c:1646  */
    break;

  case 855:
#line 4634 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9483 "def.tab.c" /* yacc.c:1646  */
    break;

  case 856:
#line 4635 "def.y" /* yacc.c:1646  */
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
            defData->Blockage.setComponent((yyvsp[0].string));
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
        }
      }
#line 9505 "def.tab.c" /* yacc.c:1646  */
    break;

  case 857:
#line 4654 "def.y" /* yacc.c:1646  */
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
      }
#line 9530 "def.tab.c" /* yacc.c:1646  */
    break;

  case 858:
#line 4675 "def.y" /* yacc.c:1646  */
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
      }
#line 9555 "def.tab.c" /* yacc.c:1646  */
    break;

  case 859:
#line 4696 "def.y" /* yacc.c:1646  */
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
      }
#line 9577 "def.tab.c" /* yacc.c:1646  */
    break;

  case 860:
#line 4714 "def.y" /* yacc.c:1646  */
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
      }
#line 9612 "def.tab.c" /* yacc.c:1646  */
    break;

  case 863:
#line 4751 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9618 "def.tab.c" /* yacc.c:1646  */
    break;

  case 864:
#line 4752 "def.y" /* yacc.c:1646  */
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
            defData->Blockage.setComponent((yyvsp[0].string));
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkPlaceComp = 1;
          }
        }
      }
#line 9640 "def.tab.c" /* yacc.c:1646  */
    break;

  case 865:
#line 4770 "def.y" /* yacc.c:1646  */
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
      }
#line 9662 "def.tab.c" /* yacc.c:1646  */
    break;

  case 866:
#line 4788 "def.y" /* yacc.c:1646  */
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
      }
#line 9700 "def.tab.c" /* yacc.c:1646  */
    break;

  case 867:
#line 4822 "def.y" /* yacc.c:1646  */
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
               defData->Blockage.setPartial((yyvsp[0].dval));
             } 
             if (defData->VersionNum < 5.8) {
               defData->hasBlkPlaceComp = 1;
             }
             if (defData->VersionNum == 5.8) {
               defData->hasBlkPlaceTypeComp = 1;
             }
           }
         }
      }
#line 9738 "def.tab.c" /* yacc.c:1646  */
    break;

  case 870:
#line 4862 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->BlockageCbk)
          defData->Blockage.addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
      }
#line 9747 "def.tab.c" /* yacc.c:1646  */
    break;

  case 871:
#line 4867 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->BlockageCbk) {
            defData->Geometries.Reset();
        }
      }
#line 9757 "def.tab.c" /* yacc.c:1646  */
    break;

  case 872:
#line 4873 "def.y" /* yacc.c:1646  */
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
      }
#line 9779 "def.tab.c" /* yacc.c:1646  */
    break;

  case 874:
#line 4895 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->SlotStartCbk)
          CALLBACK(defCallbacks->SlotStartCbk, defrSlotStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 9786 "def.tab.c" /* yacc.c:1646  */
    break;

  case 875:
#line 4899 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->SlotEndCbk)
          CALLBACK(defCallbacks->SlotEndCbk, defrSlotEndCbkType, 0); }
#line 9793 "def.tab.c" /* yacc.c:1646  */
    break;

  case 878:
#line 4907 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->SlotCbk) {
          CALLBACK(defCallbacks->SlotCbk, defrSlotCbkType, &defData->Slot);
          defData->Slot.clear();
        }
      }
#line 9804 "def.tab.c" /* yacc.c:1646  */
    break;

  case 879:
#line 4914 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9810 "def.tab.c" /* yacc.c:1646  */
    break;

  case 880:
#line 4915 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->SlotCbk) {
          defData->Slot.setLayer((yyvsp[0].string));
          defData->Slot.clearPoly();     // free poly, if any
        }
      }
#line 9821 "def.tab.c" /* yacc.c:1646  */
    break;

  case 884:
#line 4927 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->SlotCbk)
          defData->Slot.addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
      }
#line 9830 "def.tab.c" /* yacc.c:1646  */
    break;

  case 885:
#line 4932 "def.y" /* yacc.c:1646  */
    {
          defData->Geometries.Reset();
      }
#line 9838 "def.tab.c" /* yacc.c:1646  */
    break;

  case 886:
#line 4936 "def.y" /* yacc.c:1646  */
    {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond
          if (defCallbacks->SlotCbk)
            defData->Slot.addPolygon(&defData->Geometries);
        }
      }
#line 9849 "def.tab.c" /* yacc.c:1646  */
    break;

  case 888:
#line 4947 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FillStartCbk)
          CALLBACK(defCallbacks->FillStartCbk, defrFillStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 9856 "def.tab.c" /* yacc.c:1646  */
    break;

  case 889:
#line 4951 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->FillEndCbk)
          CALLBACK(defCallbacks->FillEndCbk, defrFillEndCbkType, 0); }
#line 9863 "def.tab.c" /* yacc.c:1646  */
    break;

  case 892:
#line 4959 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->FillCbk) {
          CALLBACK(defCallbacks->FillCbk, defrFillCbkType, &defData->Fill);
          defData->Fill.clear();
        }
      }
#line 9874 "def.tab.c" /* yacc.c:1646  */
    break;

  case 893:
#line 4965 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9880 "def.tab.c" /* yacc.c:1646  */
    break;

  case 894:
#line 4966 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->FillCbk) {
          defData->Fill.setVia((yyvsp[0].string));
          defData->Fill.clearPts();
          defData->Geometries.Reset();
        }
      }
#line 9892 "def.tab.c" /* yacc.c:1646  */
    break;

  case 896:
#line 4975 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9898 "def.tab.c" /* yacc.c:1646  */
    break;

  case 897:
#line 4976 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->FillCbk) {
          defData->Fill.setLayer((yyvsp[0].string));
          defData->Fill.clearPoly();    // free poly, if any
        }
      }
#line 9909 "def.tab.c" /* yacc.c:1646  */
    break;

  case 901:
#line 4989 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->FillCbk)
          defData->Fill.addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
      }
#line 9918 "def.tab.c" /* yacc.c:1646  */
    break;

  case 902:
#line 4994 "def.y" /* yacc.c:1646  */
    {
        defData->Geometries.Reset();
      }
#line 9926 "def.tab.c" /* yacc.c:1646  */
    break;

  case 903:
#line 4998 "def.y" /* yacc.c:1646  */
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
      }
#line 9944 "def.tab.c" /* yacc.c:1646  */
    break;

  case 908:
#line 5022 "def.y" /* yacc.c:1646  */
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
      }
#line 9966 "def.tab.c" /* yacc.c:1646  */
    break;

  case 909:
#line 5041 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->FillCbk) {
          defData->Fill.addPts(&defData->Geometries);
          CALLBACK(defCallbacks->FillCbk, defrFillCbkType, &defData->Fill);
          defData->Fill.clear();
        }
    }
#line 9978 "def.tab.c" /* yacc.c:1646  */
    break;

  case 914:
#line 5060 "def.y" /* yacc.c:1646  */
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
      }
#line 10000 "def.tab.c" /* yacc.c:1646  */
    break;

  case 915:
#line 5080 "def.y" /* yacc.c:1646  */
    { 
        if (validateMaskInput((int)(yyvsp[0].dval), defData->fillWarnings, defSettings->FillWarnings)) {
             if (defCallbacks->FillCbk) {
                defData->Fill.setMask((int)(yyvsp[0].dval));
             }
        }
      }
#line 10012 "def.tab.c" /* yacc.c:1646  */
    break;

  case 916:
#line 5090 "def.y" /* yacc.c:1646  */
    { 
        if (validateMaskInput((int)(yyvsp[0].dval), defData->fillWarnings, defSettings->FillWarnings)) {
             if (defCallbacks->FillCbk) {
                defData->Fill.setMask((int)(yyvsp[0].dval));
             }
        }
      }
#line 10024 "def.tab.c" /* yacc.c:1646  */
    break;

  case 918:
#line 5103 "def.y" /* yacc.c:1646  */
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
                   ROUND((yyvsp[-1].dval)));
      }
#line 10045 "def.tab.c" /* yacc.c:1646  */
    break;

  case 919:
#line 5121 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NonDefaultEndCbk)
          CALLBACK(defCallbacks->NonDefaultEndCbk, defrNonDefaultEndCbkType, 0); }
#line 10052 "def.tab.c" /* yacc.c:1646  */
    break;

  case 922:
#line 5128 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10058 "def.tab.c" /* yacc.c:1646  */
    break;

  case 923:
#line 5129 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.clear(); 
          defData->NonDefault.setName((yyvsp[0].string));
        }
      }
#line 10069 "def.tab.c" /* yacc.c:1646  */
    break;

  case 924:
#line 5136 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->NonDefaultCbk)
          CALLBACK(defCallbacks->NonDefaultCbk, defrNonDefaultCbkType, &defData->NonDefault); }
#line 10076 "def.tab.c" /* yacc.c:1646  */
    break;

  case 927:
#line 5144 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk)
          defData->NonDefault.setHardspacing();
      }
#line 10085 "def.tab.c" /* yacc.c:1646  */
    break;

  case 928:
#line 5148 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10091 "def.tab.c" /* yacc.c:1646  */
    break;

  case 929:
#line 5150 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addLayer((yyvsp[-2].string));
          defData->NonDefault.addWidth((yyvsp[0].dval));
        }
      }
#line 10102 "def.tab.c" /* yacc.c:1646  */
    break;

  case 931:
#line 5157 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10108 "def.tab.c" /* yacc.c:1646  */
    break;

  case 932:
#line 5158 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addVia((yyvsp[0].string));
        }
      }
#line 10118 "def.tab.c" /* yacc.c:1646  */
    break;

  case 933:
#line 5163 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10124 "def.tab.c" /* yacc.c:1646  */
    break;

  case 934:
#line 5164 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addViaRule((yyvsp[0].string));
        }
      }
#line 10134 "def.tab.c" /* yacc.c:1646  */
    break;

  case 935:
#line 5169 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10140 "def.tab.c" /* yacc.c:1646  */
    break;

  case 936:
#line 5170 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addMinCuts((yyvsp[-1].string), (int)(yyvsp[0].dval));
        }
      }
#line 10150 "def.tab.c" /* yacc.c:1646  */
    break;

  case 940:
#line 5183 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addDiagWidth((yyvsp[0].dval));
        }
      }
#line 10160 "def.tab.c" /* yacc.c:1646  */
    break;

  case 941:
#line 5189 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addSpacing((yyvsp[0].dval));
        }
      }
#line 10170 "def.tab.c" /* yacc.c:1646  */
    break;

  case 942:
#line 5195 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addWireExt((yyvsp[0].dval));
        }
      }
#line 10180 "def.tab.c" /* yacc.c:1646  */
    break;

  case 943:
#line 5202 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 10186 "def.tab.c" /* yacc.c:1646  */
    break;

  case 944:
#line 5204 "def.y" /* yacc.c:1646  */
    { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 10192 "def.tab.c" /* yacc.c:1646  */
    break;

  case 947:
#line 5211 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defSettings->NDefProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "NONDEFAULTRULE");
          sprintf(str, "%g", (yyvsp[0].dval));
          defData->NonDefault.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
        }
      }
#line 10207 "def.tab.c" /* yacc.c:1646  */
    break;

  case 948:
#line 5222 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          propTp = defSettings->NDefProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "NONDEFAULTRULE");
          defData->NonDefault.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 10220 "def.tab.c" /* yacc.c:1646  */
    break;

  case 949:
#line 5231 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          propTp = defSettings->NDefProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "NONDEFAULTRULE");
          defData->NonDefault.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 10233 "def.tab.c" /* yacc.c:1646  */
    break;

  case 951:
#line 5244 "def.y" /* yacc.c:1646  */
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
          CALLBACK(defCallbacks->StylesStartCbk, defrStylesStartCbkType, ROUND((yyvsp[-1].dval)));
      }
#line 10253 "def.tab.c" /* yacc.c:1646  */
    break;

  case 952:
#line 5261 "def.y" /* yacc.c:1646  */
    { if (defCallbacks->StylesEndCbk)
          CALLBACK(defCallbacks->StylesEndCbk, defrStylesEndCbkType, 0); }
#line 10260 "def.tab.c" /* yacc.c:1646  */
    break;

  case 955:
#line 5269 "def.y" /* yacc.c:1646  */
    {
        if (defCallbacks->StylesCbk) defData->Styles.setStyle((int)(yyvsp[0].dval));
        defData->Geometries.Reset();
      }
#line 10269 "def.tab.c" /* yacc.c:1646  */
    break;

  case 956:
#line 5274 "def.y" /* yacc.c:1646  */
    {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond will call the callback
          if (defCallbacks->StylesCbk) {
            defData->Styles.setPolygon(&defData->Geometries);
            CALLBACK(defCallbacks->StylesCbk, defrStylesCbkType, &defData->Styles);
          }
        }
      }
#line 10282 "def.tab.c" /* yacc.c:1646  */
    break;


#line 10286 "def.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 5284 "def.y" /* yacc.c:1906  */


END_LEFDEF_PARSER_NAMESPACE
