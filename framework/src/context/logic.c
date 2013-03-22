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
    int if_nest_level;
    node *place_holder;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->if_nest_level = 0;

    return result;
}

static info *FreeInfo( info *info)
{
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

void create_and(node *arg_node, info *arg_info)
{
    node *assign1, *assign2, *block, *new_if, *tmp1, *tmp2;

    assign1 = TBmakeAssign(TBmakeVar(STRcpy("_b")), BINOP_LEFT(arg_node));
    assign2 = TBmakeAssign(TBmakeVar(STRcpy("_b")), BINOP_RIGHT(arg_node));
    block = TBmakeStatementlist(assign2, NULL);
    new_if = TBmakeConditionif(TBmakeVar(STRcpy("_b")), block, NULL);

    swap_assign(assign1, STATEMENTLIST_HEAD(arg_info->place_holder));

    tmp1 = TBmakeStatementlist(assign1, STATEMENTLIST_NEXT(arg_info->place_holder));
    tmp2 = TBmakeStatementlist(new_if, tmp1);

    STATEMENTLIST_NEXT(arg_info->place_holder) = tmp2;

    ASSIGN_EXPR(assign1) = TBmakeVar(STRcpy("_b"));

    //TRAVdo(ASSIGN_EXPR(STATEMENTLIST_HEAD(arg_info->place_holder)), arg_info);

    //tmp1 = arg_info->place_holder;
    //arg_info->place_holder = block;

    //TRAVdo(ASSIGN_EXPR(assign2), arg_info);

    //arg_info->place_holder = tmp1;
}

void create_or(node *arg_node, info *arg_info)
{
    node *assign1, *assign2, *block, *new_if, *tmp1, *tmp2;

    assign1 = TBmakeAssign(TBmakeVar(STRcpy("_b")), BINOP_LEFT(arg_node));
    assign2 = TBmakeAssign(TBmakeVar(STRcpy("_b")), BINOP_RIGHT(arg_node));
    block = TBmakeStatementlist(assign2, NULL);
    new_if = TBmakeConditionif(TBmakeMonop(MO_not, TBmakeVar(STRcpy("_b"))), block, NULL);

    swap_assign(assign1, STATEMENTLIST_HEAD(arg_info->place_holder));

    tmp1 = TBmakeStatementlist(assign1, STATEMENTLIST_NEXT(arg_info->place_holder));
    tmp2 = TBmakeStatementlist(new_if, tmp1);

    STATEMENTLIST_NEXT(arg_info->place_holder) = tmp2;

    ASSIGN_EXPR(assign1) = TBmakeVar(STRcpy("_b"));

    //TRAVdo(ASSIGN_EXPR(STATEMENTLIST_HEAD(arg_info->place_holder)), arg_info);

    //tmp1 = arg_info->place_holder;
    //arg_info->place_holder = block;

    //TRAVdo(ASSIGN_EXPR(assign2), arg_info);

    //arg_info->place_holder = tmp1;
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

    printf("%d\n", type);

    if(type == N_var || type == N_float || type == N_num || type == N_bool)
        return 0;
    return 1;
}

/*********************   Traverse   *********************/
extern node *LOGICfunbody(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICstatementlist");

    FUNBODY_STATEMENTS(arg_node) = TRAVopt(FUNBODY_STATEMENTS(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

extern node *LOGICstatementlist(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICstatementlist");

    arg_info->place_holder = arg_node;

    printf("<%p> %d\n", arg_node, NODE_TYPE(STATEMENTLIST_HEAD(arg_node)));

    TRAVdo(STATEMENTLIST_HEAD(arg_node), arg_info);

    TRAVopt(STATEMENTLIST_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

extern node *LOGICconditionif(node *arg_node, info *arg_info)
{
    node *assign, *head, *next;

    DBUG_ENTER("LOGICconditionif");

    /* create a assign statement above the if when the expression
     * consists of multiple values/binops */
    if(expr_is_complex(CONDITIONIF_EXPR(arg_node))) {
        head = STATEMENTLIST_HEAD(arg_info->place_holder);
        next = STATEMENTLIST_NEXT(arg_info->place_holder);

        assign = TBmakeAssign(TBmakeVar(STRcpy("_b")), CONDITIONIF_EXPR(arg_node));

        CONDITIONIF_EXPR(arg_node) = TBmakeVar(STRcpy("_b"));

        STATEMENTLIST_HEAD(arg_info->place_holder) = assign;
        STATEMENTLIST_NEXT(arg_info->place_holder) = TBmakeStatementlist(head, next);

        TRAVdo(ASSIGN_EXPR(assign), arg_info);
    }
    CONDITIONIF_BLOCK(arg_node) = TRAVdo(CONDITIONIF_BLOCK(arg_node), arg_info);
    CONDITIONIF_ELSEBLOCK(arg_node) = TRAVopt(CONDITIONIF_ELSEBLOCK(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

extern node *LOGICwhileloop(node *arg_node, info *arg_info)
{
    node *assign, *assign_end, *head, *next;

    DBUG_ENTER("LOGICwhileloop");

    head = STATEMENTLIST_HEAD(arg_info->place_holder);
    next = STATEMENTLIST_NEXT(arg_info->place_holder);

    assign = TBmakeAssign(TBmakeVar(STRcpy("_b")), WHILELOOP_EXPR(arg_node));
    assign_end = COPYdoCopy(assign);

    WHILELOOP_EXPR(arg_node) = TBmakeVar(STRcpy("_b"));

    STATEMENTLIST_HEAD(arg_info->place_holder) = assign;
    STATEMENTLIST_NEXT(arg_info->place_holder) = TBmakeStatementlist(head, next);

    TRAVdo(ASSIGN_EXPR(assign), arg_info);

    add_to_end_of_block(WHILELOOP_BLOCK(arg_node), assign_end);

    TRAVdo(WHILELOOP_BLOCK(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}



extern node *LOGICassign(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICassign");

    TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

extern node *LOGICbinop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("LOGICbinop");

    /* expr1 && expr2 */
    if(BINOP_OP(arg_node) == BO_and) {
        create_and(arg_node, arg_info);
    } else if(BINOP_OP(arg_node) == BO_or) {
        create_or(arg_node, arg_info);
    }

    DBUG_RETURN(arg_node);
}


extern node *CTPdoLogic(node *syntaxtree)
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
