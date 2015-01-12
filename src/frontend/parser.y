%{
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

%}

%union{
    int ival;
    CompOp cval;
    float rval;
    char *sval;
    NODE *n;
}

%token
      RW_CREATE
      RW_DROP
      RW_SHOW
      RW_DESC
      RW_USE
      RW_TABLES
      RW_TABLE
      RW_INDEX
      RW_DATABASE
      RW_LOAD
      RW_SET
      RW_HELP
      RW_PRINT
      RW_EXIT
      RW_SELECT
      RW_FROM
      RW_WHERE
      RW_INSERT
      RW_DELETE
      RW_UPDATE
      RW_AND
      RW_IS
      RW_NOT
      RW_NULL
      RW_INTO
      RW_VALUES
      RW_PRIMARY
      RW_KEY
      RW_INT
      RW_VCHAR
      T_EQ
      T_LT
      T_LE
      T_GT
      T_GE
      T_NE
      T_EOF
      NOTOKEN
      RW_RESET
      RW_IO
      RW_BUFFER
      RW_RESIZE
      RW_QUERY_PLAN
      RW_ON
      RW_OFF

%token   <ival>   T_INT

%token   <rval>   T_REAL

%token   <sval>   T_STRING
      T_QSTRING
      T_SHELL_CMD

%type   <cval>   op

%type   <sval>   opt_relname

%type   <n>   command
      ddl
      dml
      utility
      createdatabase
      dropdatabase
      usedatabase
      showtables
      desctable
      createtable
      createindex
      droptable
      dropindex
      load
      set
      help
      print
      exit
      query
      insert
      delete
      update
      insert_value_list
      non_mt_attrtype_list
      attrtype
      non_mt_relattr_list
      non_mt_select_clause
      relattr
      non_mt_relation_list
      relation
      opt_where_clause
      non_mt_cond_list
      condition
      relattr_or_value
      non_mt_value_list
      value
      null_value
%%

start
   : command ';'
   {
      parse_tree = $1;
      YYACCEPT;
   }
   | T_SHELL_CMD
   {
      if (!isatty(0)) {
        cout << ($1) << "\n";
        cout.flush();
      }
      system($1);
      parse_tree = nullptr;
      YYACCEPT;
   }
   | error
   {
      reset_scanner();
      parse_tree = nullptr;
      YYACCEPT;
   }
   | T_EOF
   {
      parse_tree = nullptr;
      bExit = 1;
      YYACCEPT;
   }
   ;

command
   : ddl
   | dml
   | utility
   | nothing
   {
      $$ = nullptr;
   }
   ;

ddl
   : createtable
   | createindex
   | droptable
   | dropindex
   | createdatabase
   | dropdatabase
   | usedatabase
   | showtables
   | desctable
   ;

dml
   : query
   | insert
   | delete
   | update
   ;

utility
   : load
   | exit
   | set
   | help
   | print
   ;

createdatabase
   : RW_CREATE RW_DATABASE T_STRING
   {
      $$ = create_database_node($3);
   }
   ;

dropdatabase
   : RW_DROP RW_DATABASE T_STRING
   {
      $$ = drop_database_node($3);
   }
   ;

usedatabase
   : RW_USE T_STRING
   {
      $$ = use_database_node($2);
   }
   ;

showtables
   : RW_SHOW RW_TABLES
   {
      $$ = show_tables_node();
   }
   ;

desctable
   : RW_DESC T_STRING
   {
      $$ = desc_table_node($2);
   }
   ;

createtable
   : RW_CREATE RW_TABLE T_STRING '(' non_mt_attrtype_list ')'
   {
      $$ = create_table_node($3, $5);
   }
   ;

createindex
   : RW_CREATE RW_INDEX T_STRING '(' T_STRING ')'
   {
      $$ = create_index_node($3, $5);
   }
   ;

droptable
   : RW_DROP RW_TABLE T_STRING
   {
      $$ = drop_table_node($3);
   }
   ;

dropindex
   : RW_DROP RW_INDEX T_STRING '(' T_STRING ')'
   {
      $$ = drop_index_node($3, $5);
   }
   ;

load
   : RW_LOAD T_STRING '(' T_QSTRING ')'
   {
      $$ = load_node($2, $4);
   }
   ;


set
   : RW_SET T_STRING T_EQ T_QSTRING
   {
      $$ = set_node($2, $4);
   }
   ;

help
   : RW_HELP opt_relname
   {
      $$ = help_node($2);
   }
   ;

print
   : RW_PRINT T_STRING
   {
      $$ = print_node($2);
   }
   ;

exit
   : RW_EXIT
   {
      $$ = nullptr;
      bExit = 1;
   }
   ;

query
   : RW_SELECT non_mt_select_clause RW_FROM non_mt_relation_list opt_where_clause
   {
      $$ = query_node($2, $4, $5);
   }
   ;

insert
   : RW_INSERT RW_INTO T_STRING RW_VALUES insert_value_list
   {
      $$ = insert_node($3, $5);
   }
   ;

insert_value_list
   : '(' non_mt_value_list ')' ',' insert_value_list
   {
      $$ = prepend($2, $5);
   }
   | '(' non_mt_value_list ')'
   {
      $$ = list_node($2);
   }
   ;

delete
   : RW_DELETE RW_FROM T_STRING opt_where_clause
   {
      $$ = delete_node($3, $4);
   }
   ;

update
   : RW_UPDATE T_STRING RW_SET relattr T_EQ relattr_or_value opt_where_clause
   {
      $$ = update_node($2, $4, $6, $7);
   }
   ;

non_mt_attrtype_list
   : attrtype ',' non_mt_attrtype_list
   {
      $$ = prepend($1, $3);
   }
   | attrtype
   {
      $$ = list_node($1);
   }
   ;

attrtype
   : T_STRING T_STRING '(' T_INT ')'
   {
      $$ = attrtype_node($1, $2, $4, 1);
   }
   | T_STRING T_STRING '(' T_INT ')' RW_NOT RW_NULL
   {
      $$ = attrtype_node($1, $2, $4, 0);
   }
   | RW_PRIMARY RW_KEY '(' T_STRING ')'
   {
      $$ = attrprop_node(PRIMARY_KEY, $4);
   }
   ;

non_mt_select_clause
   : non_mt_relattr_list
   | '*'
   {
       $$ = list_node(relattr_node(nullptr, (char*)"*"));
   }


non_mt_relattr_list
   : relattr ',' non_mt_relattr_list
   {
      $$ = prepend($1, $3);
   }
   | relattr
   {
      $$ = list_node($1);
   }
   ;

relattr
   : T_STRING '.' T_STRING
   {
      $$ = relattr_node($1, $3);
   }
   | T_STRING
   {
      $$ = relattr_node(nullptr, $1);
   }
   ;

non_mt_relation_list
   : relation ',' non_mt_relation_list
   {
      $$ = prepend($1, $3);
   }
   | relation
   {
      $$ = list_node($1);
   }
   ;

relation
   : T_STRING
   {
      $$ = relation_node($1);
   }
   ;

opt_where_clause
   : RW_WHERE non_mt_cond_list
   {
      $$ = $2;
   }
   | nothing
   {
      $$ = nullptr;
   }
   ;

non_mt_cond_list
   : condition RW_AND non_mt_cond_list
   {
      $$ = prepend($1, $3);
   }
   | condition
   {
      $$ = list_node($1);
   }
   ;

condition
   : relattr op relattr_or_value
   {
      $$ = condition_node($1, $2, $3);
   }
   | relattr RW_IS RW_NULL
   {
      $$ = condition_node($1, IS_NULL_OP, nullptr);
   }
   | relattr RW_IS RW_NOT RW_NULL
   {
      $$ = condition_node($1, NOT_NULL_OP, nullptr);
   }
   ;

relattr_or_value
   : relattr
   {
      $$ = relattr_or_value_node($1, nullptr);
   }
   | value
   {
      $$ = relattr_or_value_node(nullptr, $1);
   }
   ;

non_mt_value_list
   : value ',' non_mt_value_list
   {
      $$ = prepend($1, $3);
   }
   | value
   {
      $$ = list_node($1);
   }
   | null_value ',' non_mt_value_list
   {
      $$ = prepend($1, $3);
   }
   | null_value
   {
      $$ = list_node($1);
   }
   ;

value
   : T_QSTRING
   {
      $$ = value_node(STRING, (void *) $1);
   }
   | T_INT
   {
      $$ = value_node(INT, (void *)& $1);
   }
   | T_REAL
   {
      $$ = value_node(FLOAT, (void *)& $1);
   }
   ;

null_value
   : RW_NULL
   {
      $$ = value_node(NULLV, nullptr);
   }
   ;

opt_relname
   : T_STRING
   {
      $$ = $1;
   }
   | nothing
   {
      $$ = nullptr;
   }
   ;

op
   : T_LT
   {
      $$ = LT_OP;
   }
   | T_LE
   {
      $$ = LE_OP;
   }
   | T_GT
   {
      $$ = GT_OP;
   }
   | T_GE
   {
      $$ = GE_OP;
   }
   | T_EQ
   {
      $$ = EQ_OP;
   }
   | T_NE
   {
      $$ = NE_OP;
   }
   ;

nothing
   : /* epsilon */
   ;

%%

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
