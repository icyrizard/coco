#include "types.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"
#include "copy.h"
#include "ctinfo.h"
#include "loop.h"

/***********************   INFO   ***********************/
struct INFO {
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));

    return result;
}

static info *FreeInfo(info *info)
{
    info = MEMfree(info);

    return info;
}

/******************   Help Functions   ******************/
node *change_dowhile(node *arg_node)
{
    node *body, *whileloop, *head, *tail,
         *dowhile = STATEMENTLIST_HEAD(arg_node);

    /* create new whileloop */
    whileloop = TBmakeWhileloop(DOWHILELOOP_EXPR(dowhile), DOWHILELOOP_BLOCK(dowhile));

    /* copy body in order to place it above while loop */
    body = COPYdoCopy(DOWHILELOOP_BLOCK(dowhile));

    /* create new statementlist and go to the tail */
    tail = head = TBmakeStatementlist(STATEMENTLIST_HEAD(body), STATEMENTLIST_NEXT(body));
    while(STATEMENTLIST_NEXT(tail))
        tail = STATEMENTLIST_NEXT(tail);

    /* set the original set of next statement list to the nexts of the new
     * statementlist*/
    STATEMENTLIST_NEXT(tail) = TBmakeStatementlist(whileloop, STATEMENTLIST_NEXT(arg_node));

    /* return the new statementlist */
    return head;
}

node *change_forloop(node *arg_node)
{
    node *whileloop, *forloop, *head, *tail, *step, *assign, *counterid;
    binop op = BO_lt;
    //int step_value;

    /* get the forloop and counter id */
    forloop = STATEMENTLIST_HEAD(arg_node);
    counterid = ASSIGN_LET(FORLOOP_STARTVALUE(forloop));
    step = FORLOOP_STEPVALUE(forloop);

    //step_value = STEP_VALUE(step);

    /* create the new while loop */
    //if(step < 0) {
    //    op = BO_lt;
    //} else if(step == 0) {
    //    CTIwarnLine(NODE_LINE(arg_node), "Step value of zero in for loop creates undefined behaviour");
    //    op = BO_eq;
    //} else {
    //    op = BO_gt;
    //}

    whileloop = TBmakeWhileloop(TBmakeBinop(op, counterid, FORLOOP_STOPVALUE(forloop)), FORLOOP_BLOCK(forloop));

    /* create new statementlist with the 'forloop variable' initialization */
    head = TBmakeStatementlist(COPYdoCopy(FORLOOP_STARTVALUE(forloop)), TBmakeStatementlist(whileloop, STATEMENTLIST_NEXT(arg_node)));

    /* get the tail of the forloop block */
    tail = FORLOOP_BLOCK(forloop);
    while(STATEMENTLIST_NEXT(tail))
        tail = STATEMENTLIST_NEXT(tail);

    /* create loop counter increase assignment when no step value was given */
    if(!step)
        step = TBmakeNum(1);

    /* create new loop counter increase assign and add it to the end of the
     * while block */
    assign = TBmakeAssign(COPYdoCopy(counterid), TBmakeBinop(BO_add, COPYdoCopy(counterid), step));
    STATEMENTLIST_NEXT(tail) = TBmakeStatementlist(assign, NULL);

    return head;
}



/*********************   Traverse   *********************/

node *LOOPstatementlist(node *arg_node, info *arg_info)
{
    type head_type;
    DBUG_ENTER("LOOPstatementlist");

    head_type = NODE_TYPE(STATEMENTLIST_HEAD(arg_node));

    if(head_type == N_dowhileloop)
        arg_node = change_dowhile(arg_node);
    else if(head_type == N_forloop)
        arg_node = change_forloop(arg_node);

    STATEMENTLIST_HEAD(arg_node) = TRAVdo(STATEMENTLIST_HEAD(arg_node), arg_info);
    STATEMENTLIST_NEXT(arg_node) = TRAVopt(STATEMENTLIST_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CTPdoLoop(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("CTPdoLinkFun");

    DBUG_ASSERT((syntaxtree != NULL), "CTPdoLogic called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_loop);

    syntaxtree = TRAVdo(syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN(syntaxtree);
}
