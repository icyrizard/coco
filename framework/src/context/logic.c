#include "types.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"
#include "copy.h"
#include "ctinfo.h"
#include "list_hash.h"
#include "logic.h"


/* TODO uhmm deze hele files heeft nu errors en shit dus.. ALLES! */
int glob = 0;

/***********************   INFO   ***********************/
struct INFO {
    int nest_level;
    list *tmp_bools;
    node *place_holder;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->nest_level = 0;
    result->tmp_bools = list_create();

    return result;
}

static info *FreeInfo( info *info)
{
    list_free(info->tmp_bools);
    info = MEMfree( info);

    return info;
}

/******************   Help Functions   ******************/
void swap_assign(node *ass1, node *ass2)
{
    node *var, *expr;

    var = ASSIGN_LET(ass1);
    expr = ASSIGN_EXPR(ass1);

    ASSIGN_LET(ass1) = ASSIGN_LET(ass2);
    ASSIGN_EXPR(ass1) = ASSIGN_EXPR(ass2);

    ASSIGN_LET(ass2)  = var;
    ASSIGN_EXPR(ass2) = expr;
}

void swap_whileloop(node *assign, node *list)
{
    node *whileloop;

    whileloop = STATEMENTLIST_HEAD(list);
    WHILELOOP_EXPR(whileloop) = ASSIGN_LET(assign);
}

void swap(node *assign, node *list)
{
    node *tmp;

    tmp = STATEMENTLIST_HEAD(list);
    STATEMENTLIST_HEAD(list) = assign;
    STATEMENTLIST_NEXT(list) = TBmakeStatementlist(tmp, STATEMENTLIST_NEXT(list));
}

void set_expr(node *arg_node, char *var_name)
{
    arg_node = TBmakeVar(var_name);
}

node *create_and(node *arg_node, info *arg_info)
{
    node *ass_left, *ass_right, *block, *new_if, *tmp1;
    char *num, *var_name;

    num = STRitoa(arg_info->nest_level);
    var_name = STRcat("_b", num);
    MEMfree(num);

    /* create seperate assigns to tmp with left and right */
    ass_left = TBmakeAssign(TBmakeVar(STRcpy(var_name)), BINOP_LEFT(arg_node));
    ass_right = TBmakeAssign(TBmakeVar(STRcpy(var_name)), BINOP_RIGHT(arg_node));

    /* create if with assign to right as block and temp var as condition */
    block = TBmakeStatementlist(ass_right, NULL);
    new_if = TBmakeConditionif(TBmakeVar(STRcpy(var_name)), block, NULL);

    ///* hack inorder to place the new statements at the right spot */
    swap(ass_left, arg_info->place_holder);

    ///* add new if condition and assign to statementlist */
    tmp1 = TBmakeStatementlist(new_if, STATEMENTLIST_NEXT(arg_info->place_holder));

    ///* add new if condition and assign to statementlist */
    STATEMENTLIST_NEXT(arg_info->place_holder) = tmp1;

    ///* set expression of last assign to temp variable */
    //ASSIGN_EXPR(ass_right) = TBmakeVar(var_name);

    ass_left = TRAVdo(ass_left, arg_info);
    //ass_right = TRAVdo(ass_right, arg_info);

    return TBmakeVar(var_name);
}

node *create_or(node *arg_node, info *arg_info)
{
    node *ass_left, *ass_right, *block, *new_if, *tmp1;
    char *num, *var_name;

    num = STRitoa(arg_info->nest_level);
    var_name = STRcat("_b", num);
    MEMfree(num);

    /* create seperate assigns to tmp with left and right */
    ass_left = TBmakeAssign(TBmakeVar(STRcpy(var_name)), BINOP_LEFT(arg_node));
    ass_right = TBmakeAssign(TBmakeVar(STRcpy(var_name)), BINOP_RIGHT(arg_node));

    /* create if with assign to right as block and temp var as condition */
    block = TBmakeStatementlist(ass_right, NULL);
    new_if = TBmakeConditionif(TBmakeMonop(MO_not, TBmakeVar(STRcpy(var_name))), block, NULL);

    ///* hack inorder to place the new statements at the right spot */
    swap(ass_left, arg_info->place_holder);

    ///* add new if condition and assign to statementlist */
    tmp1 = TBmakeStatementlist(new_if, STATEMENTLIST_NEXT(arg_info->place_holder));

    ///* add new if condition and assign to statementlist */
    STATEMENTLIST_NEXT(arg_info->place_holder) = tmp1;

    ///* set expression of last assign to temp variable */
    //ASSIGN_EXPR(ass_right) = TBmakeVar(var_name);

    ass_left = TRAVdo(ass_left, arg_info);
    //ass_right = TRAVdo(ass_right, arg_info);

    return TBmakeVar(var_name);
}


void add_to_end_of_block(node *block, node *assign)
{
    while(STATEMENTLIST_NEXT(block))
        block = STATEMENTLIST_NEXT(block);

    STATEMENTLIST_NEXT(block) = TBmakeStatementlist(assign, NULL);
}

int expr_is_complex(node *expr)
{
    int type = NODE_TYPE(expr);

    if(type == N_var || type == N_float || type == N_num || type == N_bool)
        return 0;
    return 1;
}

int check_var_dec(void *v, void *w)
{
    return STReq((char*)v, VAR_NAME(VARDEC_ID((node *)w)));
}

void add_vardec(info *arg_info)
{
    char *num, *var_name;

    num = STRitoa(arg_info->nest_level);
    var_name = STRcat("_b", num);

    if(!list_contains_fun(arg_info->tmp_bools, var_name, check_var_dec))
        list_addtofront(arg_info->tmp_bools,TBmakeVardec(TYPE_bool, TBmakeVar(var_name), NULL));
    else
        MEMfree(var_name);
    MEMfree(num);
}

node *create_vardec_list(list *var_decs)
{
    node *vardec_list = NULL;

    while((var_decs = var_decs->next))
        vardec_list = TBmakeVardeclist(var_decs->value, vardec_list);
    return vardec_list;
}

node *concat_vardec_lists(node *var_decs, list *tmp_bool_decs)
{
    node *tmps_list = create_vardec_list(tmp_bool_decs);
    node *tail = tmps_list;

    while(VARDECLIST_NEXT(tail))
        tail = VARDECLIST_NEXT(tail);

    VARDECLIST_NEXT(tail) = var_decs;

    return tmps_list;
}

/*********************   Traverse   *********************/
node *LOGICfunbody(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICstatementlist");

    /* traverse all statements */
    FUNBODY_STATEMENTS(arg_node) = TRAVopt(FUNBODY_STATEMENTS(arg_node), arg_info);

    /* add new boolean temp vardecs */
    if(list_length(arg_info->tmp_bools) > 0)
        FUNBODY_VARS( arg_node) = concat_vardec_lists(FUNBODY_VARS(arg_node), arg_info->tmp_bools);

    /* empty the boolean temp vardecs for the next function */
    list_empty(arg_info->tmp_bools);

    DBUG_RETURN(arg_node);
}

node *LOGICstatementlist(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICstatementlist");

    arg_info->place_holder = arg_node;

    TRAVdo(STATEMENTLIST_HEAD(arg_node), arg_info);

    TRAVopt(STATEMENTLIST_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *LOGICconditionif(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICconditionif");

    /* traverse expression */
    CONDITIONIF_EXPR(arg_node) = TRAVdo(CONDITIONIF_EXPR(arg_node), arg_info);

    arg_info->nest_level++;
    CONDITIONIF_BLOCK(arg_node) = TRAVdo(CONDITIONIF_BLOCK(arg_node), arg_info);
    CONDITIONIF_ELSEBLOCK(arg_node) = TRAVopt(CONDITIONIF_ELSEBLOCK(arg_node), arg_info);
    arg_info->nest_level--;

    DBUG_RETURN(arg_node);
}

node *LOGICwhileloop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICwhileloop");

    WHILELOOP_EXPR(arg_node) = TRAVdo(WHILELOOP_EXPR(arg_node), arg_info);

    //add_to_end_of_block(WHILELOOP_BLOCK(arg_node), assign_end);
    //TRAVdo(WHILELOOP_BLOCK(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *LOGICdowhileloop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICdowhileloop");

    DOWHILELOOP_EXPR(arg_node) = TRAVdo(DOWHILELOOP_EXPR(arg_node), arg_info);

    //add_to_end_of_block(WHILELOOP_BLOCK(arg_node), assign_end);

    //TRAVdo(WHILELOOP_BLOCK(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *LOGICassign(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICassign");

    ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *LOGICbinop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICbinop");


    if(BINOP_OP(arg_node) == BO_and)
        DBUG_RETURN(create_and(arg_node, arg_info));
    else if(BINOP_OP(arg_node) == BO_or)
        DBUG_RETURN(create_or(arg_node, arg_info));

    BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);
    BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *LOGICfuncall(node *arg_node, info *arg_info)
{
    node *expr_list;
    DBUG_ENTER("LOGICfuncall");

    printf("funcall %s\n", VAR_NAME(FUNCALL_ID(arg_node)));

    expr_list = FUNCALL_ARGUMENTS(arg_node);

    /* create a assign statement above the if when the expression
     * consists of multiple values/binops */
    FUNCALL_ARGUMENTS(arg_node) = TRAVopt(FUNCALL_ARGUMENTS(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *LOGICexprlist(node *arg_node, info *arg_info)
{
    node *tmp;

    DBUG_ENTER("LOGICexprlist");

    tmp = arg_info->place_holder;

    arg_info->nest_level++;
    EXPRLIST_NEXT(arg_node) = TRAVopt(EXPRLIST_NEXT(arg_node), arg_info);
    EXPRLIST_HEAD(arg_node) = TRAVopt(EXPRLIST_HEAD(arg_node), arg_info);
    arg_info->nest_level--;

    DBUG_RETURN(arg_node);
}


node *CTPdoLogic(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("CTPdoLinkFun");

    DBUG_ASSERT( ( syntaxtree != NULL), "CTPdoLink called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_logic);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo( info);


    DBUG_RETURN( syntaxtree);
}
