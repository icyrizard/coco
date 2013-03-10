#include "types.h"
#include "desug_init.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"

struct INFO {
    node *head,
         *tail,
         *fun_head,
         *fun_tail;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));

    result->head = TBmakeStatementlist(NULL, NULL);
    result->tail = result->head;

    result->fun_head = TBmakeStatementlist(NULL, NULL);
    result->fun_tail = result->fun_head;

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
    node *assign, *new_tail, *new_varlet;

    DBUG_ENTER ("INITglobaldef");

    /* Do nothing for global variable definitions without an expression */
    if (GLOBALDEF_EXPR( arg_node) == NULL)
        DBUG_RETURN (arg_node);

    /* Create a copy of the varlet for the new assignment node */
    new_varlet = TBmakeVarlet( STRcpy( VARLET_NAME( GLOBALDEF_ID( arg_node))));

    /* Create a new assign node and store it in a statmentlist */
    assign = TBmakeAssign( new_varlet, GLOBALDEF_EXPR( arg_node));
    new_tail = TBmakeStatementlist( assign, NULL);

    /* Add the new assign to the info node statmentlist */
    STATEMENTLIST_NEXT(arg_info->tail) = new_tail;
    arg_info->tail = new_tail;

    /* Remove expression from global variable definition */
    GLOBALDEF_EXPR( arg_node) = NULL;

    DBUG_RETURN( arg_node);
}


node *INITfunbody(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("INITfunbody");

    /* Traverse all local variable declarations of the function.
     * Assignment will be removed and stored in the info structure */
    if(FUNBODY_VARS( arg_node) != NULL)
        FUNBODY_VARS( arg_node) = TRAVopt( FUNBODY_VARS( arg_node), arg_info);


    /* Append funbody statements to the assignment list */
    STATEMENTLIST_NEXT( arg_info->fun_tail) = FUNBODY_STATEMENTS( arg_node);

    /* Set statements of the function to new assignments + old statements */
    FUNBODY_STATEMENTS( arg_node) = STATEMENTLIST_NEXT( arg_info->fun_head);

    /* Reset the info fun_head and fun_tail for next function */
    STATEMENTLIST_NEXT( arg_info->fun_head) = NULL;
    arg_info->fun_tail = arg_info->fun_head;

    DBUG_RETURN( arg_node);
}


node *INITvardec(node * arg_node, info * arg_info)
{
    node *assign, *new_tail, *new_varlet;

    DBUG_ENTER ("INITvardeclist");

    if (VARDEC_VALUE( arg_node) == NULL)
        DBUG_RETURN (arg_node);

    /* Create a copy of the varlet for the new assignment node */
    new_varlet = TBmakeVarlet( STRcpy( VARLET_NAME( VARDEC_ID( arg_node))));

    /* Create a new assign node and store it in a statmentlist */
    assign = TBmakeAssign( new_varlet, VARDEC_VALUE( arg_node));
    new_tail = TBmakeStatementlist( assign, NULL);

    /* Add the new assign to the info node statmentlist */
    STATEMENTLIST_NEXT(arg_info->fun_tail) = new_tail;
    arg_info->fun_tail = new_tail;

    /* Remove expression from local variable definition */
    VARDEC_VALUE( arg_node) = NULL;

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
    body   = TBmakeFunbody( NULL, STATEMENTLIST_NEXT(info->head), NULL);

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

    return TBmakeProgram( __init, syntaxtree);
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
