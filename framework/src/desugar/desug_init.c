#include "types.h"
#include "desug_init.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"

struct INFO {
    node *head;
    node *tail;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));

    result->head = TBmakeStatementlist(NULL, NULL);
    result->tail = result->head;

    return result;
}

static info *FreeInfo( info *info)
{
    info = MEMfree( info);

    return info;
}


/***************************************************/

node *INITglobaldef (node * arg_node, info * arg_info)
{
    node *assign, *new_tail;

    DBUG_ENTER ("INITglobaldef");

    if (GLOBALDEF_EXPR( arg_node) == NULL)
        DBUG_RETURN (arg_node);

    assign = TBmakeAssign( GLOBALDEF_ID( arg_node), GLOBALDEF_EXPR( arg_node));
    new_tail = TBmakeStatementlist( assign, NULL);

    STATEMENTLIST_TAIL(arg_info->tail) = new_tail;
    arg_info->tail = new_tail;

    GLOBALDEF_EXPR( arg_node) = NULL;

    DBUG_RETURN (arg_node);
}

void add_init(node *syntaxtree, info *info)
{
    node *header, *body, *__init;

    header = TBmakeFunheader( TYPE_void , TBmakeVarlet("__init"), NULL);
    body   = TBmakeFunbody( NULL, STATEMENTLIST_TAIL(info->head), NULL);
    __init = TBmakeFundef( FALSE, header, body);

    PROGRAM_HEAD(syntaxtree) = TBmakeProgram(__init, PROGRAM_HEAD(syntaxtree));
}

node * DSPdoInit(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSPdoInit");

    DBUG_ASSERT( (syntaxtree != NULL), "DSPdoInit called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_init);

    syntaxtree = TRAVdo(syntaxtree, info);

    /* create init function and add all the assignments */
    add_init(syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    printf("\n\n-------------------------------------\n\n");

    DBUG_RETURN( syntaxtree);
}
