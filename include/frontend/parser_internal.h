/*
 * parser_internal.h: internal declarations for the REDBASE parser
 *
 * Authors: Dallan Quass
 *          Jan Jannink
 *
 * originally by: Mark McAuliffe, University of Wisconsin - Madison, 1991
 */

#ifndef FRONTEND_PARSER_INTERNAL_H
#define FRONTEND_PARSER_INTERNAL_H

#include "sqleast.h"

#define PROMPT "\n<SQLEAST> $ "
#define YYINITDEPTH 100000

using namespace sqleast;

void yyerror(const char *err);

/*
 * use double for real
 */
typedef double real;


/*
 * REL_ATTR: describes a qualified attribute (relName.attrName)
 */
typedef struct{
    char *relName;      /* relation name        */
    char *attrName;     /* attribute name       */
} REL_ATTR;

/*
 * ATTR_VAL: <attribute, value> pair
 */
typedef struct{
    char *attrName;     /* attribute name       */
    AttrType valType;   /* type of value        */
    int valLength;      /* length if type = STRING */
    void *value;        /* value for attribute  */
} ATTR_VAL;

/*
 * all the available kinds of nodes
 */
typedef enum{
    N_CREATEDATABASE,
    N_DROPDATABASE,
    N_USEDATABASE,
    N_SHOWTABLES,
    N_DESCTABLE,
    N_CREATETABLE,
    N_CREATEINDEX,
    N_DROPTABLE,
    N_DROPINDEX,
    N_LOAD,
    N_SET,
    N_HELP,
    N_PRINT,
    N_QUERY,
    N_INSERT,
    N_DELETE,
    N_UPDATE,
    N_RELATTR,
    N_CONDITION,
    N_RELATTR_OR_VALUE,
    N_ATTRTYPE,
    N_ATTRPROP,
    N_VALUE,
    N_RELATION,
    N_STATISTICS,
    N_LIST
} NODEKIND;

/*
 * structure of parse tree nodes
 */
typedef struct node{
   NODEKIND kind;

   union{
      /* SM component nodes */
      /* create table node */

      struct{
          char *dbname;
      } CREATEDATABASE;

       struct{
           char *dbname;
       } DROPDATABASE;

       struct{
           char *dbname;
       } USEDATABASE;

       struct{
       } SHOWTABLES;

       struct{
           char *relname;
       } DESCTABLE;

      struct{
         char *relname;
         struct node *attrlist;
          char *primary;
      } CREATETABLE;

      /* create index node */
      struct{
         char *relname;
         char *attrname;
      } CREATEINDEX;

      /* drop index node */
      struct{
         char *relname;
         char *attrname;
      } DROPINDEX;

      /* drop table node */
      struct{
         char *relname;
      } DROPTABLE;

      /* load node */
      struct{
         char *relname;
         char *filename;
      } LOAD;

      /* set node */
      struct{
         char *paramName;
         char *string;
      } SET;

      /* help node */
      struct{
         char *relname;
      } HELP;

      /* print node */
      struct{
         char *relname;
      } PRINT;

      /* QL component nodes */
      /* query node */
      struct{
         struct node *relattrlist;
         struct node *rellist;
         struct node *conditionlist;
      } QUERY;

      /* insert node */
      struct{
         char *relname;
         struct node *tuplelist;
      } INSERT;

      /* delete node */
      struct{
         char *relname;
         struct node *conditionlist;
      } DELETE;

      /* update node */
      struct{
         char *relname;
         struct node *relattr;
         struct node *relorvalue;
         struct node *conditionlist;
      } UPDATE;

      /* command support nodes */
      /* relation attribute node */
      struct{
         char *relname;
         char *attrname;
      } RELATTR;

      /* condition node */
      struct{
         struct node *lhsRelattr;
         CompOp op;
         struct node *rhsRelattr;
         struct node *rhsValue;
      } CONDITION;

      /* relation-attribute or value */
      struct{
         struct node *relattr;
         struct node *value;
      } RELATTR_OR_VALUE;

      /* <attribute, type> pair */
      struct{
         char *attrname;
         char *type;
          int length;
          int nullable;
      } ATTRTYPE; // upgrade by zxk

       /* zxk: attr prop */
       struct{
           AttrProp attrProp;
           char *attrname;
       } ATTRPROP;

      /* <value, type> pair */
      struct{
         AttrType type;
         int  ival;
         real rval;
         char *sval;
      } VALUE;

      /* relation node */
      struct{
         char *relname;
      } RELATION;

      /* list node */
      struct{
         struct node *curr;
         struct node *next;
      } LIST;
   } u;
} NODE;


/*
 * function prototypes
 */
NODE *newnode(NODEKIND kind);
NODE *create_database_node(char *dbname);
NODE *drop_database_node(char *dbname);
NODE *use_database_node(char *dbname);
NODE *show_tables_node();
NODE *desc_table_node(char *relname);
NODE *create_table_node(char *relname, NODE *attrlist);
NODE *create_index_node(char *relname, char *attrname);
NODE *drop_index_node(char *relname, char *attrname);
NODE *drop_table_node(char *relname);
NODE *load_node(char *relname, char *filename);
NODE *set_node(char *paramName, char *string);
NODE *help_node(char *relname);
NODE *print_node(char *relname);
NODE *query_node(NODE *relattrlist, NODE *rellist, NODE *conditionlist);
NODE *insert_node(char *relname, NODE *valuelist);
NODE *delete_node(char *relname, NODE *conditionlist);
NODE *update_node(char *relname, NODE *relattr, NODE *value,
		  NODE *conditionlist);
NODE *relattr_node(char *relname, char *attrname);
NODE *condition_node(NODE *lhsRelattr, CompOp op, NODE *rhsRelattrOrValue);
NODE *value_node(AttrType type, void *value);
NODE *relattr_or_value_node(NODE *relattr, NODE *value);
NODE *attrtype_node(char *attrname, char *type, int length, int nullable);
NODE *attrprop_node(AttrProp prop, char *attrname);
NODE *relation_node(char *relname);
NODE *list_node(NODE *n);
NODE *prepend(NODE *n, NODE *list);

void reset_scanner(void);
void reset_charptr(void);
void new_query(void);
namespace sqleast {
    namespace ql {
        void interp(NODE *n);
    }
}
int  yylex(void);
int  yyparse(void);

extern "C" int  yywrap(void);

#endif




