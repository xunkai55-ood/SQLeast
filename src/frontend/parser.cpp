/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     RW_CREATE = 258,
     RW_DROP = 259,
     RW_SHOW = 260,
     RW_DESC = 261,
     RW_USE = 262,
     RW_TABLES = 263,
     RW_TABLE = 264,
     RW_INDEX = 265,
     RW_DATABASE = 266,
     RW_LOAD = 267,
     RW_SET = 268,
     RW_HELP = 269,
     RW_PRINT = 270,
     RW_EXIT = 271,
     RW_SELECT = 272,
     RW_FROM = 273,
     RW_WHERE = 274,
     RW_INSERT = 275,
     RW_DELETE = 276,
     RW_UPDATE = 277,
     RW_AND = 278,
     RW_IS = 279,
     RW_NOT = 280,
     RW_NULL = 281,
     RW_INTO = 282,
     RW_VALUES = 283,
     RW_PRIMARY = 284,
     RW_KEY = 285,
     RW_INT = 286,
     RW_VCHAR = 287,
     T_EQ = 288,
     T_LT = 289,
     T_LE = 290,
     T_GT = 291,
     T_GE = 292,
     T_NE = 293,
     T_EOF = 294,
     NOTOKEN = 295,
     RW_RESET = 296,
     RW_IO = 297,
     RW_BUFFER = 298,
     RW_RESIZE = 299,
     RW_QUERY_PLAN = 300,
     RW_ON = 301,
     RW_OFF = 302,
     T_INT = 303,
     T_REAL = 304,
     T_STRING = 305,
     T_QSTRING = 306,
     T_SHELL_CMD = 307
   };
#endif
/* Tokens.  */
#define RW_CREATE 258
#define RW_DROP 259
#define RW_SHOW 260
#define RW_DESC 261
#define RW_USE 262
#define RW_TABLES 263
#define RW_TABLE 264
#define RW_INDEX 265
#define RW_DATABASE 266
#define RW_LOAD 267
#define RW_SET 268
#define RW_HELP 269
#define RW_PRINT 270
#define RW_EXIT 271
#define RW_SELECT 272
#define RW_FROM 273
#define RW_WHERE 274
#define RW_INSERT 275
#define RW_DELETE 276
#define RW_UPDATE 277
#define RW_AND 278
#define RW_IS 279
#define RW_NOT 280
#define RW_NULL 281
#define RW_INTO 282
#define RW_VALUES 283
#define RW_PRIMARY 284
#define RW_KEY 285
#define RW_INT 286
#define RW_VCHAR 287
#define T_EQ 288
#define T_LT 289
#define T_LE 290
#define T_GT 291
#define T_GE 292
#define T_NE 293
#define T_EOF 294
#define NOTOKEN 295
#define RW_RESET 296
#define RW_IO 297
#define RW_BUFFER 298
#define RW_RESIZE 299
#define RW_QUERY_PLAN 300
#define RW_ON 301
#define RW_OFF 302
#define T_INT 303
#define T_REAL 304
#define T_STRING 305
#define T_QSTRING 306
#define T_SHELL_CMD 307




/* Copy the first part of user declarations.  */
#line 1 "parser.y"

/*
 * parser.y: yacc specification for RQL
 *
 * Authors: Dallan Quass
 *          Jan Jannink
 *          Jason McHugh
 *
 * originally by: Mark McAuliffe, University of Wisconsin - Madison, 1991
 *
 * 1997: Added "print buffer", "print io", "reset io" and the "*" in
 * SFW Query.
 * 1998: Added "reset buffer", "resize buffer [int]", "queryplans on",
 * and "queryplans off".
 * 2000: Added "const" to yyerror-header
 *
 */

#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <unistd.h>
#include "sqleast.h"
#include "parser_internal.h"

using namespace std;
using namespace sqleast;

  // Added by Wendy Tobagus
void yyrestart(FILE*);

/*
 * string representation of tokens; provided by scanner
 */
extern char *yytext;

/*
 * points to root of parse tree
 */
static NODE *parse_tree;

int bExit;                 // when to return from RBparse

int bQueryPlans;           // When to print the query plans



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

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 49 "parser.y"
{
    int ival;
    CompOp cval;
    float rval;
    char *sval;
    NODE *n;
}
/* Line 193 of yacc.c.  */
#line 256 "/Users/badpoet/Workspace/Repositories/SQLeast/src/frontend/parser.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 269 "/Users/badpoet/Workspace/Repositories/SQLeast/src/frontend/parser.cpp"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
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
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  65
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   133

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  59
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  87
/* YYNRULES -- Number of states.  */
#define YYNSTATES  159

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      54,    55,    57,     2,    56,     2,    58,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    53,
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
      45,    46,    47,    48,    49,    50,    51,    52
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     8,    10,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
      40,    42,    44,    46,    48,    50,    52,    54,    56,    60,
      64,    67,    70,    73,    80,    87,    91,    98,   104,   109,
     112,   115,   117,   123,   129,   135,   139,   144,   152,   156,
     158,   164,   172,   178,   180,   182,   186,   188,   192,   194,
     198,   200,   202,   205,   207,   211,   213,   217,   221,   226,
     228,   230,   234,   236,   240,   242,   244,   246,   248,   250,
     252,   254,   256,   258,   260,   262,   264,   266
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      60,     0,    -1,    61,    53,    -1,    52,    -1,     1,    -1,
      39,    -1,    62,    -1,    63,    -1,    64,    -1,   100,    -1,
      70,    -1,    71,    -1,    72,    -1,    73,    -1,    65,    -1,
      66,    -1,    67,    -1,    68,    -1,    69,    -1,    79,    -1,
      80,    -1,    82,    -1,    83,    -1,    74,    -1,    78,    -1,
      75,    -1,    76,    -1,    77,    -1,     3,    11,    50,    -1,
       4,    11,    50,    -1,     7,    50,    -1,     5,     8,    -1,
       6,    50,    -1,     3,     9,    50,    54,    84,    55,    -1,
       3,    10,    50,    54,    50,    55,    -1,     4,     9,    50,
      -1,     4,    10,    50,    54,    50,    55,    -1,    12,    50,
      54,    51,    55,    -1,    13,    50,    33,    51,    -1,    14,
      98,    -1,    15,    50,    -1,    16,    -1,    17,    86,    18,
      89,    91,    -1,    20,    27,    50,    28,    81,    -1,    54,
      95,    55,    56,    81,    -1,    54,    95,    55,    -1,    21,
      18,    50,    91,    -1,    22,    50,    13,    88,    33,    94,
      91,    -1,    85,    56,    84,    -1,    85,    -1,    50,    50,
      54,    48,    55,    -1,    50,    50,    54,    48,    55,    25,
      26,    -1,    29,    30,    54,    50,    55,    -1,    87,    -1,
      57,    -1,    88,    56,    87,    -1,    88,    -1,    50,    58,
      50,    -1,    50,    -1,    90,    56,    89,    -1,    90,    -1,
      50,    -1,    19,    92,    -1,   100,    -1,    93,    23,    92,
      -1,    93,    -1,    88,    99,    94,    -1,    88,    24,    26,
      -1,    88,    24,    25,    26,    -1,    88,    -1,    96,    -1,
      96,    56,    95,    -1,    96,    -1,    97,    56,    95,    -1,
      97,    -1,    51,    -1,    48,    -1,    49,    -1,    26,    -1,
      50,    -1,   100,    -1,    34,    -1,    35,    -1,    36,    -1,
      37,    -1,    33,    -1,    38,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   156,   156,   161,   171,   177,   186,   187,   188,   189,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   208,
     209,   210,   211,   215,   216,   217,   218,   219,   223,   230,
     237,   244,   251,   258,   265,   272,   279,   286,   294,   301,
     308,   315,   323,   330,   337,   341,   348,   355,   362,   366,
     373,   377,   381,   388,   389,   396,   400,   407,   411,   418,
     422,   429,   436,   440,   447,   451,   458,   462,   466,   473,
     477,   484,   488,   492,   496,   503,   507,   511,   518,   525,
     529,   536,   540,   544,   548,   552,   556,   562
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "RW_CREATE", "RW_DROP", "RW_SHOW",
  "RW_DESC", "RW_USE", "RW_TABLES", "RW_TABLE", "RW_INDEX", "RW_DATABASE",
  "RW_LOAD", "RW_SET", "RW_HELP", "RW_PRINT", "RW_EXIT", "RW_SELECT",
  "RW_FROM", "RW_WHERE", "RW_INSERT", "RW_DELETE", "RW_UPDATE", "RW_AND",
  "RW_IS", "RW_NOT", "RW_NULL", "RW_INTO", "RW_VALUES", "RW_PRIMARY",
  "RW_KEY", "RW_INT", "RW_VCHAR", "T_EQ", "T_LT", "T_LE", "T_GT", "T_GE",
  "T_NE", "T_EOF", "NOTOKEN", "RW_RESET", "RW_IO", "RW_BUFFER",
  "RW_RESIZE", "RW_QUERY_PLAN", "RW_ON", "RW_OFF", "T_INT", "T_REAL",
  "T_STRING", "T_QSTRING", "T_SHELL_CMD", "';'", "'('", "')'", "','",
  "'*'", "'.'", "$accept", "start", "command", "ddl", "dml", "utility",
  "createdatabase", "dropdatabase", "usedatabase", "showtables",
  "desctable", "createtable", "createindex", "droptable", "dropindex",
  "load", "set", "help", "print", "exit", "query", "insert",
  "insert_value_list", "delete", "update", "non_mt_attrtype_list",
  "attrtype", "non_mt_select_clause", "non_mt_relattr_list", "relattr",
  "non_mt_relation_list", "relation", "opt_where_clause",
  "non_mt_cond_list", "condition", "relattr_or_value", "non_mt_value_list",
  "value", "null_value", "opt_relname", "op", "nothing", 0
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
     305,   306,   307,    59,    40,    41,    44,    42,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    59,    60,    60,    60,    60,    61,    61,    61,    61,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    63,
      63,    63,    63,    64,    64,    64,    64,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    81,    82,    83,    84,    84,
      85,    85,    85,    86,    86,    87,    87,    88,    88,    89,
      89,    90,    91,    91,    92,    92,    93,    93,    93,    94,
      94,    95,    95,    95,    95,    96,    96,    96,    97,    98,
      98,    99,    99,    99,    99,    99,    99,   100
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       2,     2,     2,     6,     6,     3,     6,     5,     4,     2,
       2,     1,     5,     5,     5,     3,     4,     7,     3,     1,
       5,     7,     5,     1,     1,     3,     1,     3,     1,     3,
       1,     1,     2,     1,     3,     1,     3,     3,     4,     1,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     4,     0,     0,     0,     0,     0,     0,     0,    87,
       0,    41,     0,     0,     0,     0,     5,     3,     0,     0,
       6,     7,     8,    14,    15,    16,    17,    18,    10,    11,
      12,    13,    23,    25,    26,    27,    24,    19,    20,    21,
      22,     9,     0,     0,     0,     0,     0,     0,    31,    32,
      30,     0,     0,    79,    39,    80,    40,    58,    54,     0,
      53,    56,     0,     0,     0,     1,     2,     0,     0,    28,
      35,     0,    29,     0,     0,     0,     0,     0,     0,    87,
       0,     0,     0,     0,     0,    38,    57,    61,    87,    60,
      55,     0,     0,    46,    63,     0,     0,     0,     0,    49,
       0,     0,    37,    42,     0,     0,    43,     0,    62,    65,
       0,     0,     0,    33,     0,    34,    36,    59,    78,    76,
      77,    75,     0,    72,    74,     0,    85,    81,    82,    83,
      84,    86,     0,     0,    69,    87,    70,     0,     0,    48,
      45,     0,     0,     0,    67,    66,    64,    47,     0,     0,
       0,    71,    73,    68,    52,    50,    44,     0,    51
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,   106,    39,    40,    98,    99,    59,    60,    61,
      88,    89,    93,   108,   109,   135,   122,   123,   124,    54,
     132,    94
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -101
static const yytype_int8 yypact[] =
{
       2,  -101,    40,    47,    -6,   -25,   -19,    12,    13,    15,
      16,  -101,   -29,    34,    46,    17,  -101,  -101,    68,    18,
    -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,
    -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,
    -101,  -101,    19,    20,    22,    23,    24,    25,  -101,  -101,
    -101,    26,    43,  -101,  -101,  -101,  -101,    21,  -101,    59,
    -101,    27,    28,    31,    69,  -101,  -101,    30,    32,  -101,
    -101,    33,  -101,    37,    38,    35,    41,    42,    62,    74,
      42,   -16,    44,    45,    48,  -101,  -101,  -101,    74,    49,
    -101,    50,    42,  -101,  -101,    63,    67,    51,    52,    53,
      55,    56,  -101,  -101,    41,   -22,  -101,     9,  -101,    75,
     -13,    54,    58,  -101,   -16,  -101,  -101,  -101,  -101,  -101,
    -101,  -101,    60,    57,    61,    14,  -101,  -101,  -101,  -101,
    -101,  -101,   -13,    42,  -101,    74,  -101,    64,    70,  -101,
      65,   -22,   -22,    73,  -101,  -101,  -101,  -101,    71,    72,
      50,  -101,  -101,  -101,  -101,    77,  -101,    80,  -101
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,
    -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,
    -101,  -101,   -50,  -101,  -101,     5,  -101,  -101,    39,   -80,
      29,  -101,   -87,   -11,  -101,   -12,   -82,  -100,  -101,  -101,
    -101,    11
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -88
static const yytype_int16 yytable[] =
{
      95,   103,    48,     1,   118,     2,     3,     4,     5,     6,
     136,    41,   107,    96,     7,     8,     9,    10,    11,    12,
      55,    57,    13,    14,    15,    49,   119,   120,    58,   121,
     134,    50,   136,   125,    97,   119,   120,    57,   121,   143,
     144,    16,   126,   127,   128,   129,   130,   131,   147,    42,
      43,    44,   134,   107,    17,   -87,    45,    46,    47,   151,
     152,    62,    51,    52,    63,    53,    56,    64,    65,    67,
      68,    66,    69,    70,    71,    72,    74,    76,    78,    75,
      73,    79,    80,    77,    81,    86,    82,    83,    84,    85,
      91,    87,    57,    92,   100,   101,   110,   111,   133,   153,
     156,   112,   157,   102,   105,   104,   158,   113,   137,   114,
     115,   116,   138,   141,   148,   140,    90,   142,   149,   139,
     145,   150,   146,     0,     0,     0,   154,   155,     0,     0,
       0,     0,     0,   117
};

static const yytype_int16 yycheck[] =
{
      80,    88,     8,     1,    26,     3,     4,     5,     6,     7,
     110,     0,    92,    29,    12,    13,    14,    15,    16,    17,
       9,    50,    20,    21,    22,    50,    48,    49,    57,    51,
     110,    50,   132,    24,    50,    48,    49,    50,    51,    25,
      26,    39,    33,    34,    35,    36,    37,    38,   135,     9,
      10,    11,   132,   133,    52,    53,     9,    10,    11,   141,
     142,    27,    50,    50,    18,    50,    50,    50,     0,    50,
      50,    53,    50,    50,    50,    50,    33,    18,    50,    58,
      54,    50,    13,    56,    54,    50,    54,    54,    51,    51,
      28,    50,    50,    19,    50,    50,    33,    30,    23,    26,
     150,    50,    25,    55,    54,    56,    26,    55,    54,    56,
      55,    55,    54,    56,    50,    55,    77,    56,    48,   114,
     132,    56,   133,    -1,    -1,    -1,    55,    55,    -1,    -1,
      -1,    -1,    -1,   104
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,    12,    13,    14,
      15,    16,    17,    20,    21,    22,    39,    52,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    82,
      83,   100,     9,    10,    11,     9,    10,    11,     8,    50,
      50,    50,    50,    50,    98,   100,    50,    50,    57,    86,
      87,    88,    27,    18,    50,     0,    53,    50,    50,    50,
      50,    50,    50,    54,    33,    58,    18,    56,    50,    50,
      13,    54,    54,    54,    51,    51,    50,    50,    89,    90,
      87,    28,    19,    91,   100,    88,    29,    50,    84,    85,
      50,    50,    55,    91,    56,    54,    81,    88,    92,    93,
      33,    30,    50,    55,    56,    55,    55,    89,    26,    48,
      49,    51,    95,    96,    97,    24,    33,    34,    35,    36,
      37,    38,    99,    23,    88,    94,    96,    54,    54,    84,
      55,    56,    56,    25,    26,    94,    92,    91,    50,    48,
      56,    95,    95,    26,    55,    55,    81,    25,    26
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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

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
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
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
        case 2:
#line 157 "parser.y"
    {
      parse_tree = (yyvsp[(1) - (2)].n);
      YYACCEPT;
   ;}
    break;

  case 3:
#line 162 "parser.y"
    {
      if (!isatty(0)) {
        cout << ((yyvsp[(1) - (1)].sval)) << "\n";
        cout.flush();
      }
      system((yyvsp[(1) - (1)].sval));
      parse_tree = nullptr;
      YYACCEPT;
   ;}
    break;

  case 4:
#line 172 "parser.y"
    {
      reset_scanner();
      parse_tree = nullptr;
      YYACCEPT;
   ;}
    break;

  case 5:
#line 178 "parser.y"
    {
      parse_tree = nullptr;
      bExit = 1;
      YYACCEPT;
   ;}
    break;

  case 9:
#line 190 "parser.y"
    {
      (yyval.n) = nullptr;
   ;}
    break;

  case 28:
#line 224 "parser.y"
    {
      (yyval.n) = create_database_node((yyvsp[(3) - (3)].sval));
   ;}
    break;

  case 29:
#line 231 "parser.y"
    {
      (yyval.n) = drop_database_node((yyvsp[(3) - (3)].sval));
   ;}
    break;

  case 30:
#line 238 "parser.y"
    {
      (yyval.n) = use_database_node((yyvsp[(2) - (2)].sval));
   ;}
    break;

  case 31:
#line 245 "parser.y"
    {
      (yyval.n) = show_tables_node();
   ;}
    break;

  case 32:
#line 252 "parser.y"
    {
      (yyval.n) = desc_table_node((yyvsp[(2) - (2)].sval));
   ;}
    break;

  case 33:
#line 259 "parser.y"
    {
      (yyval.n) = create_table_node((yyvsp[(3) - (6)].sval), (yyvsp[(5) - (6)].n));
   ;}
    break;

  case 34:
#line 266 "parser.y"
    {
      (yyval.n) = create_index_node((yyvsp[(3) - (6)].sval), (yyvsp[(5) - (6)].sval));
   ;}
    break;

  case 35:
#line 273 "parser.y"
    {
      (yyval.n) = drop_table_node((yyvsp[(3) - (3)].sval));
   ;}
    break;

  case 36:
#line 280 "parser.y"
    {
      (yyval.n) = drop_index_node((yyvsp[(3) - (6)].sval), (yyvsp[(5) - (6)].sval));
   ;}
    break;

  case 37:
#line 287 "parser.y"
    {
      (yyval.n) = load_node((yyvsp[(2) - (5)].sval), (yyvsp[(4) - (5)].sval));
   ;}
    break;

  case 38:
#line 295 "parser.y"
    {
      (yyval.n) = set_node((yyvsp[(2) - (4)].sval), (yyvsp[(4) - (4)].sval));
   ;}
    break;

  case 39:
#line 302 "parser.y"
    {
      (yyval.n) = help_node((yyvsp[(2) - (2)].sval));
   ;}
    break;

  case 40:
#line 309 "parser.y"
    {
      (yyval.n) = print_node((yyvsp[(2) - (2)].sval));
   ;}
    break;

  case 41:
#line 316 "parser.y"
    {
      (yyval.n) = nullptr;
      bExit = 1;
   ;}
    break;

  case 42:
#line 324 "parser.y"
    {
      (yyval.n) = query_node((yyvsp[(2) - (5)].n), (yyvsp[(4) - (5)].n), (yyvsp[(5) - (5)].n));
   ;}
    break;

  case 43:
#line 331 "parser.y"
    {
      (yyval.n) = insert_node((yyvsp[(3) - (5)].sval), (yyvsp[(5) - (5)].n));
   ;}
    break;

  case 44:
#line 338 "parser.y"
    {
      (yyval.n) = prepend((yyvsp[(2) - (5)].n), (yyvsp[(5) - (5)].n));
   ;}
    break;

  case 45:
#line 342 "parser.y"
    {
      (yyval.n) = list_node((yyvsp[(2) - (3)].n));
   ;}
    break;

  case 46:
#line 349 "parser.y"
    {
      (yyval.n) = delete_node((yyvsp[(3) - (4)].sval), (yyvsp[(4) - (4)].n));
   ;}
    break;

  case 47:
#line 356 "parser.y"
    {
      (yyval.n) = update_node((yyvsp[(2) - (7)].sval), (yyvsp[(4) - (7)].n), (yyvsp[(6) - (7)].n), (yyvsp[(7) - (7)].n));
   ;}
    break;

  case 48:
#line 363 "parser.y"
    {
      (yyval.n) = prepend((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n));
   ;}
    break;

  case 49:
#line 367 "parser.y"
    {
      (yyval.n) = list_node((yyvsp[(1) - (1)].n));
   ;}
    break;

  case 50:
#line 374 "parser.y"
    {
      (yyval.n) = attrtype_node((yyvsp[(1) - (5)].sval), (yyvsp[(2) - (5)].sval), (yyvsp[(4) - (5)].ival), 1);
   ;}
    break;

  case 51:
#line 378 "parser.y"
    {
      (yyval.n) = attrtype_node((yyvsp[(1) - (7)].sval), (yyvsp[(2) - (7)].sval), (yyvsp[(4) - (7)].ival), 0);
   ;}
    break;

  case 52:
#line 382 "parser.y"
    {
      (yyval.n) = attrprop_node(PRIMARY_KEY, (yyvsp[(4) - (5)].sval));
   ;}
    break;

  case 54:
#line 390 "parser.y"
    {
       (yyval.n) = list_node(relattr_node(nullptr, (char*)"*"));
   ;}
    break;

  case 55:
#line 397 "parser.y"
    {
      (yyval.n) = prepend((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n));
   ;}
    break;

  case 56:
#line 401 "parser.y"
    {
      (yyval.n) = list_node((yyvsp[(1) - (1)].n));
   ;}
    break;

  case 57:
#line 408 "parser.y"
    {
      (yyval.n) = relattr_node((yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].sval));
   ;}
    break;

  case 58:
#line 412 "parser.y"
    {
      (yyval.n) = relattr_node(nullptr, (yyvsp[(1) - (1)].sval));
   ;}
    break;

  case 59:
#line 419 "parser.y"
    {
      (yyval.n) = prepend((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n));
   ;}
    break;

  case 60:
#line 423 "parser.y"
    {
      (yyval.n) = list_node((yyvsp[(1) - (1)].n));
   ;}
    break;

  case 61:
#line 430 "parser.y"
    {
      (yyval.n) = relation_node((yyvsp[(1) - (1)].sval));
   ;}
    break;

  case 62:
#line 437 "parser.y"
    {
      (yyval.n) = (yyvsp[(2) - (2)].n);
   ;}
    break;

  case 63:
#line 441 "parser.y"
    {
      (yyval.n) = nullptr;
   ;}
    break;

  case 64:
#line 448 "parser.y"
    {
      (yyval.n) = prepend((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n));
   ;}
    break;

  case 65:
#line 452 "parser.y"
    {
      (yyval.n) = list_node((yyvsp[(1) - (1)].n));
   ;}
    break;

  case 66:
#line 459 "parser.y"
    {
      (yyval.n) = condition_node((yyvsp[(1) - (3)].n), (yyvsp[(2) - (3)].cval), (yyvsp[(3) - (3)].n));
   ;}
    break;

  case 67:
#line 463 "parser.y"
    {
      (yyval.n) = condition_node((yyvsp[(1) - (3)].n), IS_NULL_OP, nullptr);
   ;}
    break;

  case 68:
#line 467 "parser.y"
    {
      (yyval.n) = condition_node((yyvsp[(1) - (4)].n), NOT_NULL_OP, nullptr);
   ;}
    break;

  case 69:
#line 474 "parser.y"
    {
      (yyval.n) = relattr_or_value_node((yyvsp[(1) - (1)].n), nullptr);
   ;}
    break;

  case 70:
#line 478 "parser.y"
    {
      (yyval.n) = relattr_or_value_node(nullptr, (yyvsp[(1) - (1)].n));
   ;}
    break;

  case 71:
#line 485 "parser.y"
    {
      (yyval.n) = prepend((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n));
   ;}
    break;

  case 72:
#line 489 "parser.y"
    {
      (yyval.n) = list_node((yyvsp[(1) - (1)].n));
   ;}
    break;

  case 73:
#line 493 "parser.y"
    {
      (yyval.n) = prepend((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n));
   ;}
    break;

  case 74:
#line 497 "parser.y"
    {
      (yyval.n) = list_node((yyvsp[(1) - (1)].n));
   ;}
    break;

  case 75:
#line 504 "parser.y"
    {
      (yyval.n) = value_node(STRING, (void *) (yyvsp[(1) - (1)].sval));
   ;}
    break;

  case 76:
#line 508 "parser.y"
    {
      (yyval.n) = value_node(INT, (void *)& (yyvsp[(1) - (1)].ival));
   ;}
    break;

  case 77:
#line 512 "parser.y"
    {
      (yyval.n) = value_node(FLOAT, (void *)& (yyvsp[(1) - (1)].rval));
   ;}
    break;

  case 78:
#line 519 "parser.y"
    {
      (yyval.n) = value_node(NULLV, nullptr);
   ;}
    break;

  case 79:
#line 526 "parser.y"
    {
      (yyval.sval) = (yyvsp[(1) - (1)].sval);
   ;}
    break;

  case 80:
#line 530 "parser.y"
    {
      (yyval.sval) = nullptr;
   ;}
    break;

  case 81:
#line 537 "parser.y"
    {
      (yyval.cval) = LT_OP;
   ;}
    break;

  case 82:
#line 541 "parser.y"
    {
      (yyval.cval) = LE_OP;
   ;}
    break;

  case 83:
#line 545 "parser.y"
    {
      (yyval.cval) = GT_OP;
   ;}
    break;

  case 84:
#line 549 "parser.y"
    {
      (yyval.cval) = GE_OP;
   ;}
    break;

  case 85:
#line 553 "parser.y"
    {
      (yyval.cval) = EQ_OP;
   ;}
    break;

  case 86:
#line 557 "parser.y"
    {
      (yyval.cval) = NE_OP;
   ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2064 "/Users/badpoet/Workspace/Repositories/SQLeast/src/frontend/parser.cpp"
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
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
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

  if (yyn == YYFINAL)
    YYACCEPT;

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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
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


#line 566 "parser.y"


void sqleast::sqleast_parse()
{

   // Set up global variables to their defaults
   bExit = 0;

   /* Do forever */
   while (!bExit) {

      /* Reset parser and scanner for a new query */
      new_query();

      /* Print a prompt */
      cout << PROMPT;

      /* Get the prompt to actually show up on the screen */
      cout.flush();

      /* If a query was successfully read, interpret it */
      if(yyparse() == 0 && parse_tree != nullptr)
         sqleast::ql::interp(parse_tree);
   }
}

/*
 * Required by yacc
 */
void yyerror(char const *s) // New in 2000
{
   puts(s);
}

/*
 * Sometimes required
 */
int yywrap(void)
{
   return 1;
}

