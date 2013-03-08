#include "types.h"
#include "desug_init.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"

struct INFO {
    node *statementlist;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));

    result->statementlist = NULL;

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
    node *new_head;

    DBUG_ENTER ("INITglobaldef");

    if (GLOBALDEF_EXPR( arg_node) == NULL)
        DBUG_RETURN (arg_node);

    new_head = TBmakeAssign( GLOBALDEF_ID( arg_node), GLOBALDEF_EXPR( arg_node));

    arg_info->statementlist = TBmakeStatementlist( new_head, arg_info->statementlist);

    GLOBALDEF_EXPR( arg_node) = NULL;

    DBUG_RETURN (arg_node);
}

void add_init(node *syntaxtree, info *info)
{
    node *header, *body, *__init;

    header = TBmakeFunheader( TYPE_void , TBmakeVarlet("__init"), NULL);
    body   = TBmakeFunbody( NULL, info->statementlist, NULL);
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
