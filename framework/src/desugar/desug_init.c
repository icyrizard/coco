#include "types.h"
#include "desug_init.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"

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

/* Remove the expressions of global variable definitions.
 * Store the expressions the info struct for later uses when
 * constructing the __init funtion
 *
 * Returns a pointer to the current node
 *
 **/
node *INITglobaldef (node * arg_node, info * arg_info)
{
    node *assign, *new_tail;

    DBUG_ENTER ("INITglobaldef");

    /* Do nothing for global variable definitions without an expression */
    if (GLOBALDEF_EXPR( arg_node) == NULL)
        DBUG_RETURN (arg_node);

    /* Create a new assign node and store it in a statmentlist */
    assign = TBmakeAssign( GLOBALDEF_ID( arg_node), GLOBALDEF_EXPR( arg_node));
    new_tail = TBmakeStatementlist( assign, NULL);

    /* Add the new assign to the info node statmentlist */
    STATEMENTLIST_NEXT(arg_info->tail) = new_tail;
    arg_info->tail = new_tail;

    /* Remove expression from global variable defnition */
    GLOBALDEF_EXPR( arg_node) = NULL;

    DBUG_RETURN( arg_node);
}


/* Create a function definition with name '__init' and with all the
 * global variable assign statments
 *
 * Returns a pointer to the new fundef node
 *
 **/
node* create_init_fundef(info* info)
{
    node *header, *body;



    header = TBmakeFunheader( TYPE_void , TBmakeVarlet(STRcpy("__init")), NULL);
    //body   = TBmakeFunbody( NULL, STATEMENTLIST_NEXT(info->head), NULL);
    body   = TBmakeFunbody( NULL, NULL, NULL);

    return TBmakeFundef( FALSE, header, body);
}


/* Create a new tree root node where the '__init' function
 * is added
 *
 * Returns the new root node
 *
 **/
node* add_init( node *syntaxtree, info *info)
{
    node *__init = create_init_fundef( info);

    return TBmakeProgram( __init, TBmakeProgram( PROGRAM_HEAD( syntaxtree),
                PROGRAM_NEXT( syntaxtree)));
}

node * DSPdoInit(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSPdoInit");

    DBUG_ASSERT( ( syntaxtree != NULL), "DSPdoInit called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_init);

    syntaxtree = TRAVdo( syntaxtree, info);

    syntaxtree = add_init( syntaxtree, info);

    TRAVpop();

    info = FreeInfo( info);

    printf("\n\n-------------------------------------\n\n");

    DBUG_RETURN( syntaxtree);
}
