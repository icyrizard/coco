#include <stdio.h>
#include <string.h>
#include "types.h"
#include "link_functions.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"
#include "list_hash.h"


/* arrays for easy conversion of type numbers into strings */
char* monops[3] = { "not", "neg", "unknown" };
char* binops[14] = { "add", "sub", "mul", "div", "modulo", "less than",
                     "less equals", "greater than", "greater equals", "equals",
                     "not equals", "and", "or", "unknown" };
char* types[5] = { "bool", "int", "float", "void", "unknown" };

/***********************   INFO   ***********************/

struct INFO {
    type t;
    int line;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->t = 100;

    return result;
}

static info *FreeInfo( info *info)
{
    info = MEMfree( info);

    return info;
}

/******************   Help Functions   ******************/
int count_num_args(node *funcall)
{
    int result = 0;
    node* list = FUNCALL_ARGUMENTS(funcall);

    while(list) {
        list = EXPRLIST_NEXT(list);
        result++;
    }

    return result;
}

int count_num_params(node *funheader)
{
    int result = 0;
    node *list;

    list = FUNHEADER_PARAMS(funheader);

    while(list) {
        list = PARAMLIST_NEXT(list);
        result++;
    }

    return result;
}

type get_decl_type(node *decl)
{
    switch(NODE_TYPE(decl))
    {
        case N_globaldec:
            return GLOBALDEC_TYPE(decl);
        case N_globaldef:
            return GLOBALDEF_TYPE(decl);
        case N_vardec:
            return VARDEC_TYPE(decl);
        case N_param:
            return PARAM_TYPE(decl);
        default:
            fprintf(stderr, "Unknown node type %d\n", NODE_TYPE(decl));
            return TYPE_unknown;
    }
}

void check_argument_type(node *exprlist, node *paramlist, info *arg_info)
{
    type t_expr, t_param;
    int i = 1;

    /* go through list and check types */
    while(exprlist) {
        TRAVdo(EXPRLIST_HEAD(exprlist), arg_info);
        t_expr = arg_info->t;
        t_param = PARAM_TYPE(PARAMLIST_HEAD(paramlist));

        if(t_expr != t_param)
            CTIerror(":%d: error: argument %d is of type '%s' but expected type '%s'",
                    NODE_LINE(EXPRLIST_HEAD(exprlist)), i, types[t_expr], types[t_param]);
        exprlist = EXPRLIST_NEXT(exprlist);
        paramlist = PARAMLIST_NEXT(paramlist);
        i++;
    }
    return;
}

/*********************   Traverse   *********************/
node *TYPEfundef(node *arg_node, info *arg_info)
{
    type rtn_def, rtn_expr;
    DBUG_ENTER("TYPEfundef");

    /* traverse header and get return type */
    TRAVdo(FUNDEF_HEADER(arg_node), arg_info);
    rtn_def = arg_info->t;


    if(FUNDEF_BODY(arg_node) == NULL) {
        rtn_expr = TYPE_void;
    } else {
        TRAVopt(FUNDEF_BODY(arg_node), arg_info);
        rtn_expr = arg_info->t;
    }

    if(rtn_def != rtn_expr) {
        if(rtn_expr != TYPE_void)
            CTIerror(":%d: error return of function '%s' is of type '%s' but must be '%s'",
                NODE_LINE(FUNBODY_RETURN(FUNDEF_BODY(arg_node))), VAR_NAME(FUNHEADER_ID(FUNDEF_HEADER(arg_node))),
                types[rtn_expr], types[rtn_def]);
        else
            CTIerror(":%d: error no return found in function '%s' expected return type '%s'",
                NODE_LINE(arg_node), VAR_NAME(FUNHEADER_ID(FUNDEF_HEADER(arg_node))),
                types[rtn_def]);
    }
    DBUG_RETURN(arg_node);
}

node *TYPEfunheader(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEfunheader");

    arg_info->t = FUNHEADER_RETTYPE(arg_node);

    DBUG_RETURN(arg_node);
}

node *TYPEfunbody(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEfunbody");

    TRAVopt(FUNBODY_STATEMENTS(arg_node), arg_info);

    /* traverse return expression */
    arg_info->t = TYPE_void;
    TRAVopt(FUNBODY_RETURN(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *TYPEfuncall(node *arg_node, info *arg_info)
{
    int num_args = 0,
        num_params = 0;

    DBUG_ENTER("TYPEfuncall");

    /* check for correct number of arguments */
    num_args = count_num_args(arg_node);
    num_params = count_num_params(VAR_DECL(FUNCALL_ID(arg_node)));

    if(num_args != num_params) {
        char *funname = VAR_NAME(FUNCALL_ID(arg_node));

        if(num_params == 1)
            CTIerror(":%d: error: '%s' takes 1 parameter, but %d arguments are passed", NODE_LINE(arg_node), funname, num_args);
        else if(num_args == 1)
            CTIerror(":%d: error: '%s' takes %d parameters, but 1 argument is passed", NODE_LINE(arg_node), funname, num_params);
        else
            CTIerror(":%d: error: '%s' takes %d parameters, but %d arguments are passed", NODE_LINE(arg_node), funname, num_params, num_args);

        arg_info->t = TYPE_unknown;
    }

    /* check argument type */
    check_argument_type(FUNCALL_ARGUMENTS(arg_node), FUNHEADER_PARAMS(VAR_DECL(FUNCALL_ID(arg_node))), arg_info);

    /* set type in info to return type */
    arg_info->t = FUNHEADER_RETTYPE(VAR_DECL(FUNCALL_ID(arg_node)));


    DBUG_RETURN(arg_node);
}

node *TYPEbinop(node *arg_node, info *arg_info)
{
    type left, right;

    DBUG_ENTER("TYPEexpr");

    /* get the type of left expression */
    TRAVdo( BINOP_LEFT(arg_node), arg_info);
    left = arg_info->t;

    if(left == TYPE_unknown)
        DBUG_RETURN(arg_node);

    /* get the type of right expression */
    TRAVdo( BINOP_RIGHT(arg_node), arg_info);
    right = arg_info->t;

    if(right == TYPE_unknown)
        DBUG_RETURN(arg_node);

    /* check type of left and right correspond with the operation */
    switch(BINOP_OP(arg_node)) {
        case BO_add:
        case BO_mul:
            /* left and right must be same type */
            if(left != right) {
                CTIerror(":%d: error: invalid operand type to operator", NODE_LINE(arg_node));
                arg_info->t = TYPE_unknown;
            } else
                arg_info->t = left;
            break;
        case BO_sub:
        case BO_div:
            /* left and right cannot be bool and must be same type */
            if(left != right || left == TYPE_bool || right == TYPE_bool) {
                CTIerror(":%d: error: invalid operand type to operator", NODE_LINE(arg_node));
                arg_info->t = TYPE_unknown;
            } else
                arg_info->t = left;
            break;
        case BO_mod:
            /* left and right must be int */
            if(left != TYPE_int || right != TYPE_int) {
                CTIerror(":%d: error: both sides of the modulo operator must be of integer type", NODE_LINE(arg_node));
                arg_info->t = TYPE_unknown;
            } else
                arg_info->t = TYPE_int;
            break;
        case BO_lt:
        case BO_le:
        case BO_gt:
        case BO_ge:
            /* left and right must be same type */
            if(left == TYPE_bool || right == TYPE_bool) {
                CTIerror(":%d: error: can only compare arithmetic types "\
                         "with this comparison", NODE_LINE(arg_node));
                arg_info->t = TYPE_unknown;
            } else if(left != right) {
                    CTIerror(":%d: error: cannot compare expression of type "
                            "'%s' with expression of type '%s'",
                            NODE_LINE(arg_node), types[left], types[right]);
                arg_info->t = TYPE_unknown;
            } else
                arg_info->t = TYPE_bool;
            break;
        case BO_eq:
        case BO_ne:
            if(left != right) {
                CTIerror(":%d: error: cannot compare expression of type "
                        "'%s' with expression of type '%s'",
                        NODE_LINE(arg_node), types[left], types[right]);
                arg_info->t = TYPE_unknown;
            } else
                arg_info->t = TYPE_bool;
            break;
        case BO_or:
        case BO_and:
            /* left and right must be bool */
            if(left != TYPE_bool || right != TYPE_bool) {
                CTIerror(":%d: error: both sides of a boolean operator must be boolean expressions", NODE_LINE(arg_node));
                arg_info->t = TYPE_unknown;
            } else
                arg_info->t = TYPE_bool;
            break;
        default:
            fprintf(stderr, "unknown binop operator TYPEbinop()\n");
    }
    DBUG_RETURN(arg_node);
}

node *TYPEmonop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEmonop");

    TRAVdo(MONOP_RIGHT(arg_node), arg_info);

    switch(MONOP_OP(arg_node))
    {
        case MO_not:
            if(arg_info->t != TYPE_bool) {
                CTIerror(":%d: error: logical negation is only defined for booleans",
                        NODE_LINE(arg_node));
                arg_info->t = TYPE_unknown;
            }
            break;
        case MO_neg:
            if(arg_info->t == TYPE_bool) {
                CTIerror(":%d: error: unary minus is only defined for arithmetic types",
                        NODE_LINE(arg_node));
                arg_info->t = TYPE_unknown;
            }
            break;
        case MO_unknown:
            break;
    }

    DBUG_RETURN(arg_node);
}


node *TYPEvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEvar");

    arg_info->t = get_decl_type(VAR_DECL(arg_node));

    DBUG_RETURN(arg_node);
}

node *TYPEfloat(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEfloat");

    arg_info->t = TYPE_float;

    DBUG_RETURN(arg_node);
}

node *TYPEnum(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEnum");

    arg_info->t = TYPE_int;

    DBUG_RETURN(arg_node);
}

node *TYPEbool(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEbool");

    arg_info->t = TYPE_bool;

    DBUG_RETURN(arg_node);
}

node *TYPEassign(node *arg_node, info *arg_info)
{
    char *tmp, *let;
    type varlet;
    DBUG_ENTER("TYPEassign");

    /* get type of varlet */
    varlet = get_decl_type(VAR_DECL(ASSIGN_LET(arg_node)));

    let = VAR_NAME(ASSIGN_LET(arg_node));

    /* check if expr is of same type as varlet */
    TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

    if(varlet != TYPE_unknown && arg_info->t != TYPE_unknown &&
            varlet != arg_info->t)
        CTIerror(":%d: error: cannot initialize '%s', of type '%s', with an "
                 "expression of type '%s'", NODE_LINE(arg_node), let,
                 types[varlet], types[arg_info->t]);

    /* check if we are assigning to a loop counter */
    tmp = strchr(let, '$');

    if(tmp) {
        int length = tmp - let;
        tmp = STRncpy(let, length);
        CTIerror(":%d: error: cannot write to loop counter '%s'", NODE_LINE(arg_node), tmp);
        MEMfree(tmp);
    }

    DBUG_RETURN(arg_node);
}

node *TYPEconditionif(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEconditionif");

    /* check if expr is of type bool */
    TRAVdo(CONDITIONIF_EXPR(arg_node), arg_info);

    if(arg_info->t != TYPE_unknown && arg_info->t != TYPE_bool)
        CTIerror(":%d: error: conditional expression must be of 'bool' type",
                NODE_LINE(CONDITIONIF_EXPR(arg_node)));

    /* type check loop condition body and optional else block */
    TRAVdo(CONDITIONIF_BLOCK(arg_node), arg_info);
    TRAVopt(CONDITIONIF_ELSEBLOCK(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *TYPEwhileloop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEwhileloop");

    /* check if expr is of type bool */
    TRAVdo(WHILELOOP_EXPR(arg_node), arg_info);

    if(arg_info->t != TYPE_bool)
        CTIerror(":%d: error: conditional expression must be of 'bool' type",
                NODE_LINE(WHILELOOP_EXPR(arg_node)));

    /* type check the loop body */
    TRAVdo(WHILELOOP_BLOCK(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *TYPEdowhileloop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEwhileloop");


    /* type check the loop body */
    TRAVdo(DOWHILELOOP_BLOCK(arg_node), arg_info);

    /* check if expr is of type bool */
    TRAVdo(DOWHILELOOP_EXPR(arg_node), arg_info);

    if(arg_info->t != TYPE_bool)
        CTIerror(":%d: error: conditional expression must be of 'bool' type",
                NODE_LINE(DOWHILELOOP_EXPR(arg_node)));

    DBUG_RETURN(arg_node);
}


node *TYPEcast(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEcast");

    TRAVdo(CAST_RIGHT(arg_node), arg_info);

    /* check on casting void types */
    switch(arg_info->t)
    {
        case TYPE_void:
            CTIerror(":%d: error: can only cast between basic types",
                NODE_LINE(arg_node));
            arg_info->t = TYPE_unknown;
            break;
        case TYPE_int:
        case TYPE_float:
        case TYPE_bool:
            arg_info->t = CAST_TYPE(arg_node);
        case TYPE_unknown:
            break;
    }

    DBUG_RETURN(arg_node);
}

node *TYPEforloop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TYPEforloop");

    /* check forloop variable initialization expr on int type */
    TRAVdo(ASSIGN_EXPR(FORLOOP_STARTVALUE(arg_node)), arg_info);
    if(arg_info->t != TYPE_int)
        CTIerror(":%d: error: expression must be of type 'int'",
                NODE_LINE(ASSIGN_EXPR(FORLOOP_STARTVALUE(arg_node))));

    /* check stop value expr on int type */
    TRAVdo(FORLOOP_STOPVALUE(arg_node), arg_info);
    if(arg_info->t != TYPE_int)
        CTIerror(":%d: error: expression must be of type 'int'",
                NODE_LINE(FORLOOP_STOPVALUE(arg_node)));

    /* check step value expr on int type */
    if(FORLOOP_STEPVALUE(arg_node) != NULL) {
        TRAVdo(FORLOOP_STEPVALUE(arg_node), arg_info);
        if(arg_info->t != TYPE_int)
            CTIerror(":%d: error: expression must be of type 'int'",
                NODE_LINE(ASSIGN_EXPR(FORLOOP_STOPVALUE(arg_node))));
    }

    /* type check the statements in the forloop */
    TRAVdo(FORLOOP_BLOCK(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CTPdoType(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("TYPEdoType");

    DBUG_ASSERT( ( syntaxtree != NULL), "CTPdoType called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_type);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo( info);

    DBUG_RETURN( syntaxtree);
}
