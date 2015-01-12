/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
/* Line 1529 of yacc.c.  */
#line 161 "/Users/badpoet/Thursday/3a/DB/projects/SQLeast/src/frontend/parser.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

