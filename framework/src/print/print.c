
/**
 * @file print.c
 *
 * Functions needed by print traversal.
 *
 */

/**
 * @defgroup print Print Functions.
 *
 * Functions needed by print traversal.
 *
 * @{
 */


#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"


/*
 * INFO structure
 */
struct INFO {
    bool firsterror;
    int indent;
};

void print_indent( int n)
{
    int i = 0;

    for(; i < n; i++)
        printf("    ");

}


#define INFO_FIRSTERROR(n) ((n)->firsterror)

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->indent = 0;

    INFO_FIRSTERROR(result) = FALSE;

    return result;
}


static info *FreeInfo( info *info)
{
    info = MEMfree( info);

    return info;
}



/** <!--******************************************************************-->
 *
 * @fn PRTprogram
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTprogram(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTprogram");

    PROGRAM_HEAD( arg_node) = TRAVdo( PROGRAM_HEAD( arg_node), arg_info);

    PROGRAM_NEXT( arg_node) = TRAVopt( PROGRAM_NEXT( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTinstr
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTassign (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTassign");

    if (ASSIGN_LET( arg_node) != NULL) {
        ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
        printf( " = ");
    }

    ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTbinop
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTbinop (node * arg_node, info * arg_info)
{
    char *tmp;

    DBUG_ENTER ("PRTbinop");

    printf( "(");

    BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);

    switch (BINOP_OP( arg_node)) {
        case BO_add:
            tmp = "+";
            break;
        case BO_sub:
            tmp = "-";
            break;
        case BO_mul:
            tmp = "*";
            break;
        case BO_div:
            tmp = "/";
            break;
        case BO_mod:
            tmp = "%";
            break;
        case BO_lt:
            tmp = "<";
            break;
        case BO_le:
            tmp = "<=";
            break;
        case BO_gt:
            tmp = ">";
            break;
        case BO_ge:
            tmp = ">=";
            break;
        case BO_eq:
            tmp = "==";
            break;
        case BO_ne:
            tmp = "!=";
            break;
        case BO_or:
            tmp = "||";
            break;
        case BO_and:
            tmp = "&&";
            break;
        case BO_unknown:
            DBUG_ASSERT( 0, "unknown binop detected!");
    }

    printf( " %s ", tmp);

    BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);

    printf( ")");

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTfloat
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Float node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTfloat (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTfloat");

    printf( "%f", FLOAT_VALUE( arg_node));

    DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn PRTnum
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Num node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTnum (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTnum");

    printf( "%i", NUM_VALUE( arg_node));

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTboolean
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Boolean node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTbool (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTbool");

    if (BOOL_VALUE( arg_node)) {
        printf( "true");
    }
    else {
        printf( "false");
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTvar
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

/*node *
PRTvar (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTvar");

    printf( "%s", VAR_NAME( arg_node));

    DBUG_RETURN (arg_node);
}*/


/** <!--******************************************************************-->
 *
 * @fn PRTvarlet
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvarlet (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTvarlet");

    /* print varlet as string */
    printf( "%s_", VARLET_NAME( arg_node));

    /* print varlet as its declaration pointer */
    printf( "%p", VARLET_DECL( arg_node));

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTerror
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTerror (node * arg_node, info * arg_info)
{
    bool first_error;

    DBUG_ENTER ("PRTerror");

    if (NODE_ERROR (arg_node) != NULL) {
        NODE_ERROR (arg_node) = TRAVdo (NODE_ERROR (arg_node), arg_info);
    }

    first_error = INFO_FIRSTERROR( arg_info);

    if( (global.outfile != NULL)
            && (ERROR_ANYPHASE( arg_node) == global.compiler_anyphase)) {

        if ( first_error) {
            printf ( "\n/******* BEGIN TREE CORRUPTION ********\n");
            INFO_FIRSTERROR( arg_info) = FALSE;
        }

        printf ( "%s\n", ERROR_MESSAGE( arg_node));

        if (ERROR_NEXT (arg_node) != NULL) {
            TRAVopt (ERROR_NEXT (arg_node), arg_info);
        }

        if ( first_error) {
            printf ( "********    END TREE CORRUPTION    *******/\n");
            INFO_FIRSTERROR( arg_info) = TRUE;
        }
    }

    DBUG_RETURN (arg_node);
}

node *
PRTmonop (node * arg_node, info * arg_info)
{
    char *tmp;

    DBUG_ENTER ("PRTmonop");

    printf("(");

    switch (MONOP_OP( arg_node)) {
        case MO_not:
            tmp = "!";
            break;
        case MO_neg:
            tmp = "-";
            break;
        case MO_unknown:
            DBUG_ASSERT( 0, "unknown minop detected!");
    }

    printf( " %s ", tmp);

    MONOP_RIGHT( arg_node) = TRAVdo( MONOP_RIGHT( arg_node), arg_info);

    printf(")");

    DBUG_RETURN (arg_node);
}


node *PRTfundec (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTfundec");

    printf("extern ");

    FUNDEC_HEADER( arg_node) = TRAVdo( FUNDEC_HEADER( arg_node), arg_info);

    printf(";\n");

    DBUG_RETURN (arg_node);
}

node *PRTglobaldec (node * arg_node, info * arg_info)
{
    char* tmp;

    DBUG_ENTER ("PRTglobaldec");

    printf("exern ");

    switch (GLOBALDEC_TYPE( arg_node)) {
      case TYPE_bool:
        tmp = "bool";
        break;
      case TYPE_int:
        tmp = "int";
        break;
      case TYPE_float:
        tmp = "float";
        break;
      case TYPE_void:
        tmp = "void";
        break;
      case TYPE_unknown:
        DBUG_ASSERT( 0, "unknown type detected!");
    }

    printf("%s ", tmp);

    GLOBALDEC_ID( arg_node) = TRAVdo( GLOBALDEC_ID( arg_node), arg_info);

    printf(";\n");

    DBUG_RETURN (arg_node);

}

node *PRTglobaldef (node * arg_node, info * arg_info)
{
    char* tmp;

    DBUG_ENTER ("PRTglobaldef");

    if (GLOBALDEF_EXPORT( arg_node))
        printf("export ");

    switch (GLOBALDEF_TYPE( arg_node)) {
      case TYPE_bool:
        tmp = "bool";
        break;
      case TYPE_int:
        tmp = "int";
        break;
      case TYPE_float:
        tmp = "float";
        break;
      case TYPE_void:
        tmp = "void";
        break;
      case TYPE_unknown:
        DBUG_ASSERT( 0, "unknown type detected!");
    }

    printf("%s ", tmp);

    GLOBALDEF_ID( arg_node) = TRAVdo( GLOBALDEF_ID( arg_node), arg_info);

    if(GLOBALDEF_EXPR( arg_node) != NULL) {
        printf(" = ");
        GLOBALDEF_EXPR( arg_node) = TRAVdo( GLOBALDEF_EXPR( arg_node), arg_info);
    }
    printf(";\n");

    DBUG_RETURN (arg_node);
}

node *PRTcast (node * arg_node, info * arg_info)
{
    char *tmp;

    DBUG_ENTER ("PRTcast");

    printf("(");

    switch (CAST_TYPE( arg_node)) {
      case TYPE_bool:
        tmp = "bool";
        break;
      case TYPE_int:
        tmp = "int";
        break;
      case TYPE_float:
        tmp = "float";
        break;
      case TYPE_void:
        tmp = "void";
        break;
      case TYPE_unknown:
        DBUG_ASSERT( 0, "unknown type detected!");
    }
    printf(")");

    CAST_RIGHT( arg_node) = TRAVdo( CAST_RIGHT( arg_node), arg_info);

    DBUG_RETURN (arg_node);

}

node *PRTconditionif (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTconditionif");

    print_indent(arg_info->indent);
    printf("if(");

    CONDITIONIF_EXPR( arg_node) = TRAVdo( CONDITIONIF_EXPR( arg_node), arg_info);

    printf(")\n");
    print_indent(arg_info->indent++);
    printf("{\n");

    CONDITIONIF_BLOCK( arg_node) = TRAVdo( CONDITIONIF_BLOCK( arg_node), arg_info);

    print_indent(--arg_info->indent);
    printf("}\n");

    if(CONDITIONIF_ELSEBLOCK( arg_node) != NULL) {
        print_indent(arg_info->indent);
        printf("else\n");
        print_indent(arg_info->indent++);
        printf("{\n");

        CONDITIONIF_ELSEBLOCK( arg_node) = TRAVdo(CONDITIONIF_ELSEBLOCK(\
                    arg_node), arg_info);
        print_indent(--arg_info->indent);
        printf("}\n");
    }

    DBUG_RETURN (arg_node);
}

node *PRTwhileloop (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTwhileloop");

    printf("while(");

    WHILELOOP_EXPR( arg_node) = TRAVdo( WHILELOOP_EXPR( arg_node), arg_info);

    printf(")\n");

    if(WHILELOOP_BLOCK( arg_node) != NULL) {
        print_indent(arg_info->indent++);
        printf("{\n");

        WHILELOOP_BLOCK( arg_node) = TRAVdo( WHILELOOP_BLOCK( arg_node), arg_info);

        print_indent(--arg_info->indent);
        printf("}\n");
    }

    DBUG_RETURN (arg_node);
}

node *PRTdowhileloop (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTdowhileloop");

    printf("do\n");

    WHILELOOP_BLOCK( arg_node) = TRAVdo( WHILELOOP_BLOCK( arg_node), arg_info);

    printf("while(");

    WHILELOOP_EXPR( arg_node) = TRAVdo( WHILELOOP_EXPR( arg_node), arg_info);

    printf(");\n");

    DBUG_RETURN (arg_node);

}

node *PRTforloop (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTforloop");

    printf("for( ");

    FORLOOP_STARTVALUE( arg_node) = TRAVdo( FORLOOP_STARTVALUE( arg_node),
            arg_info);

    printf(", ");

    FORLOOP_STOPVALUE( arg_node) = TRAVdo( FORLOOP_STOPVALUE( arg_node),
                arg_info);

    if(FORLOOP_STEPVALUE( arg_node) != NULL) {
        printf("; ");
        FORLOOP_STEPVALUE( arg_node) = TRAVdo( FORLOOP_STEPVALUE( arg_node),
                arg_info);
    }
    printf(")\n");
    print_indent(arg_info->indent++);
    printf("{\n");
    FORLOOP_BLOCK( arg_node) = TRAVdo( FORLOOP_BLOCK( arg_node), arg_info);
    print_indent(--arg_info->indent);
    printf("}\n");


    DBUG_RETURN (arg_node);
}

node *PRTconst (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTconst");

    printf(" %s ", CONST_NAME( arg_node));

    DBUG_RETURN (arg_node);
}

node *PRTfuncall (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTfuncall");

    FUNCALL_ID( arg_node) = TRAVdo( FUNCALL_ID( arg_node), arg_info);

    printf("(");

    FUNCALL_ARGUMENTS( arg_node) = TRAVopt( FUNCALL_ARGUMENTS( arg_node), arg_info);

    printf(")");


    DBUG_RETURN (arg_node);
}

node *PRTfundef (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTfundef");

    if(FUNDEF_EXPORT( arg_node))
        printf("export ");

    FUNDEF_HEADER( arg_node) = TRAVdo( FUNDEF_HEADER( arg_node), arg_info);

    printf("\n{\n");

    FUNDEF_BODY( arg_node) = TRAVopt( FUNDEF_BODY( arg_node), arg_info);

    printf("}\n\n");

    DBUG_RETURN (arg_node);
}

node *PRTfunbody (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTfunbody");

    arg_info->indent++;

    if(FUNBODY_VARS( arg_node) != NULL) {
        FUNBODY_VARS( arg_node) = TRAVopt( FUNBODY_VARS( arg_node), arg_info);
        printf("\n");
    }

    if(FUNBODY_STATEMENTS( arg_node) != NULL) {
        FUNBODY_STATEMENTS( arg_node) = TRAVopt( FUNBODY_STATEMENTS( arg_node), arg_info);
    }

    if(FUNBODY_RETURN( arg_node) != NULL) {
        printf("\n");
        print_indent(arg_info->indent);
        printf("return ");

        FUNBODY_RETURN( arg_node) = TRAVdo( FUNBODY_RETURN( arg_node), arg_info);

        printf(";\n");
    }
    arg_info->indent--;

    DBUG_RETURN (arg_node);
}

node *PRTexprlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTexprlist");

    EXPRLIST_HEAD( arg_node) = TRAVopt( EXPRLIST_HEAD( arg_node), arg_info);

    if(EXPRLIST_NEXT( arg_node) != NULL){
        printf(", ");
        EXPRLIST_NEXT( arg_node) = TRAVdo( EXPRLIST_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);

}

node *PRTvardeclist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTvardeclist");

    VARDECLIST_HEAD( arg_node) = TRAVdo( VARDECLIST_HEAD( arg_node), arg_info);
    printf(";\n");

    VARDECLIST_NEXT( arg_node) = TRAVopt( VARDECLIST_NEXT( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

node *PRTvardec (node * arg_node, info * arg_info)
{
    char* tmp;

    DBUG_ENTER ("PRTvardec");

    switch (VARDEC_TYPE( arg_node)) {
        case TYPE_bool:
            tmp = "bool";
            break;
        case TYPE_int:
            tmp = "int";
            break;
        case TYPE_float:
            tmp = "float";
            break;
        case TYPE_void:
            tmp = "void";
            break;
        case TYPE_unknown:
            DBUG_ASSERT( 0, "no or unknown type defined");
    }

    print_indent(arg_info->indent);
    printf("%s ", tmp);

    VARDEC_ID( arg_node) = TRAVdo( VARDEC_ID( arg_node), arg_info);

    if(VARDEC_VALUE( arg_node) != NULL) {
        printf(" = ");

        VARDEC_VALUE( arg_node) = TRAVdo( VARDEC_VALUE( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}

node *PRTstatementlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTstatementlist");

    if(NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) != N_conditionif)
        print_indent( arg_info->indent);
    STATEMENTLIST_HEAD( arg_node) = TRAVdo( STATEMENTLIST_HEAD( arg_node), arg_info);

    if((NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_funcall) || (NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_assign) || (NODE_TYPE(STATEMENTLIST_HEAD(arg_node)) == N_dowhileloop))
        printf(";\n");

    STATEMENTLIST_NEXT( arg_node) = TRAVopt( STATEMENTLIST_NEXT( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

node *PRTfunheader (node * arg_node, info * arg_info)
{
    char* tmp;

    DBUG_ENTER ("PRTfunheader");

    switch (FUNHEADER_RETTYPE( arg_node)) {
        case TYPE_bool:
            tmp = "bool";
            break;
        case TYPE_int:
            tmp = "int";
            break;
        case TYPE_float:
            tmp = "float";
            break;
        case TYPE_void:
            tmp = "void";
            break;
        case TYPE_unknown:
            DBUG_ASSERT( 0, "no or unknown type defined");
    }

    printf("%s ", tmp);

    FUNHEADER_ID( arg_node) = TRAVdo( FUNHEADER_ID( arg_node), arg_info);

    printf("(");

    FUNHEADER_PARAMS( arg_node) = TRAVopt( FUNHEADER_PARAMS( arg_node),
            arg_info);

    printf(")");

    DBUG_RETURN (arg_node);
}

node *PRTparamlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTparamlist");

    PARAMLIST_HEAD( arg_node) = TRAVdo( PARAMLIST_HEAD( arg_node), arg_info);

    if(PARAMLIST_NEXT( arg_node) != NULL) {
        printf(", ");
        PARAMLIST_NEXT( arg_node) = TRAVopt( PARAMLIST_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}

node *PRTparam (node * arg_node, info * arg_info)
{
    char *tmp;

    DBUG_ENTER ("PRTparam");

    switch (PARAM_TYPE( arg_node)) {
        case TYPE_bool:
            tmp = "bool";
            break;
        case TYPE_int:
            tmp = "int";
            break;
        case TYPE_float:
            tmp = "float";
            break;
        case TYPE_void:
            tmp = "void";
            break;
        case TYPE_unknown:
            DBUG_ASSERT( 0, "no or unknown type defined");
    }

    printf("%s ", tmp);
    PARAM_ID( arg_node) = TRAVdo( PARAM_ID( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}


/** <!-- ****************************************************************** -->
 * @brief Prints the given syntaxtree
 *
 * @param syntaxtree a node structure
 *
 * @return the unchanged nodestructure
 ******************************************************************************/

node
*PRTdoPrint( node *syntaxtree)
{
    info *info;

    DBUG_ENTER("PRTdoPrint");

    DBUG_ASSERT( (syntaxtree!= NULL), "PRTdoPrint called with empty syntaxtree");

    printf( "\n\n------------------------------\n\n");

    info = MakeInfo();

    TRAVpush( TR_prt);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    printf( "\n\n------------------------------\n\n");

    DBUG_RETURN( syntaxtree);
}

/**
 * @}
 */
