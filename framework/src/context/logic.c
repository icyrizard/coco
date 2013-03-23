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
    hashmap *tmp_bools;
    node *place_holder;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->nest_level = 0;
    result->tmp_bools = hashmap_create();

    return result;
}

static info *FreeInfo(info *info)
{
    hashmap_free(info->tmp_bools);
    info = MEMfree(info);

    return info;
}

/******************   Help Functions   ******************/
void swap(node *assign, node *list)
{
    node *tmp;

    tmp = STATEMENTLIST_HEAD(list);
    STATEMENTLIST_HEAD(list) = assign;
    STATEMENTLIST_NEXT(list) = TBmakeStatementlist(tmp, STATEMENTLIST_NEXT(list));
}

char *create_var_name(info *arg_info)
{
    char *num, *var_name;

    num = STRitoa(arg_info->nest_level);
    var_name = STRcat("_b", num);
    MEMfree(num);

    return var_name;
}

node *get_create_var_dec(node *var, info *arg_info)
{
    char *var_name = VAR_NAME(var);
    node *var_decl;

    var_decl = hashmap_get(arg_info->tmp_bools, var_name);
    if(!var_decl) {
        var_decl = TBmakeVardec(TYPE_bool, COPYdoCopy(var), NULL);
        hashmap_add(arg_info->tmp_bools, var_name, var_decl);
    }

    return var_decl;
}

node *create_var(info *arg_info)
{
    node *var;

    var = TBmakeVar(create_var_name(arg_info));
    VAR_DECL(var) = get_create_var_dec(var, arg_info);

    return var;
}

node *create_and(node *arg_node, info *arg_info)
{
    node *ass_left, *ass_right, *block, *new_if, *tmp1;

    /* create seperate assigns to tmp with left and right */
    ass_left = TBmakeAssign(create_var(arg_info), BINOP_LEFT(arg_node));
    ass_right = TBmakeAssign(create_var(arg_info), BINOP_RIGHT(arg_node));

    /* create if with assign to right as block and temp var as condition */
    block = TBmakeStatementlist(ass_right, NULL);
    new_if = TBmakeConditionif(create_var(arg_info), block, NULL);

    ///* hack inorder to place the new statements at the right spot */
    swap(ass_left, arg_info->place_holder);

    ///* add new if condition and assign to statementlist */
    tmp1 = TBmakeStatementlist(new_if, STATEMENTLIST_NEXT(arg_info->place_holder));

    ///* add new if condition and assign to statementlist */
    STATEMENTLIST_NEXT(arg_info->place_holder) = tmp1;

    ass_left = TRAVdo(ass_left, arg_info);

    return create_var(arg_info);
}

node *create_or(node *arg_node, info *arg_info)
{
    node *ass_left, *ass_right, *block, *new_if, *tmp1;
    char *num, *var_name;

    num = STRitoa(arg_info->nest_level);
    var_name = STRcat("_b", num);
    MEMfree(num);

    /* create seperate assigns to tmp with left and right */
    ass_left = TBmakeAssign(create_var(arg_info), BINOP_LEFT(arg_node));
    ass_right = TBmakeAssign(create_var(arg_info), BINOP_RIGHT(arg_node));

    /* create if with assign to right as block and temp var as condition */
    block = TBmakeStatementlist(ass_right, NULL);
    new_if = TBmakeConditionif(TBmakeMonop(MO_not, create_var(arg_info)), block, NULL);

    /* hack inorder to place the new statements at the right spot */
    swap(ass_left, arg_info->place_holder);

    ///* add new if condition and assign to statementlist */
    tmp1 = TBmakeStatementlist(new_if, STATEMENTLIST_NEXT(arg_info->place_holder));

    ///* add new if condition and assign to statementlist */
    STATEMENTLIST_NEXT(arg_info->place_holder) = tmp1;

    ass_left = TRAVdo(ass_left, arg_info);

    return create_var(arg_info);
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


node *create_vardec_hashmap(hashmap *var_decs)
{
    node *vardec_list = NULL;

    while((var_decs = var_decs->next))
        vardec_list = TBmakeVardeclist(var_decs->value, vardec_list);
    return vardec_list;
}

node *concat_vardec_lists(node *var_decs, hashmap *tmp_bool_decs)
{
    node *tmps_list = create_vardec_hashmap(tmp_bool_decs);
    node *tail = tmps_list;

    printf("halloooo\n");

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
    if(!hashmap_is_empty(arg_info->tmp_bools))
        FUNBODY_VARS(arg_node) = concat_vardec_lists(FUNBODY_VARS(arg_node), arg_info->tmp_bools);

    /* empty the boolean temp vardecs for the next function */
    hashmap_empty(arg_info->tmp_bools);

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

    DBUG_ASSERT((syntaxtree != NULL), "CTPdoLink called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_logic);

    syntaxtree = TRAVdo(syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);


    DBUG_RETURN(syntaxtree);
}
