#include "types.h"
#include "desug_init.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"

/***********************   INFO   ***********************/
/* INFO object containing two linked list.
 *
 * The first one, head and tail, is used to store the global variable
 * initializations as assignmnents.
 *
 * The second one, fun_head and fun_tail, is used to store the local
 * variable initializations as assignments. This linked list has to be
 * 'emptied' ever time a function desugering is done
 *
 * NOTE: we do not support nested functions and arrays yet
 */
struct INFO {
    node *head,
         *tail,
         *fun_head,
         *fun_tail;
};

/* MakeInfo allocates a new info 'object'
 *
 * The two linked list are initialized with a empty head node
 */
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

    /* TODO free the two heads nodes */

    return info;
}

void empty_info_funlist(info *arg_info)
{
    STATEMENTLIST_NEXT( arg_info->fun_head) = NULL;
    arg_info->fun_tail = arg_info->fun_head;
}

/**************** Traverse Help Functions ***************/

/* Creates a new assign node based on the given ID and EXPR
 * The assign node is then placed inside a statementlist with
 * empty NEXT so it can be used as the tail of the assign
 * statementlist
 *
 * Returns a pointer to the statementlist
 *
 */
node* create_new_assign_statement_list(node *id, node *expr)
{
    node *new_varlet, *assign;

    /* Create a copy of the varlet for the new assignment node */
    new_varlet = TBmakeVarlet( STRcpy( VARLET_NAME( id)));

    /* Create a new assign node and store it in a statmentlist */
    assign = TBmakeAssign( new_varlet, expr);

    return TBmakeStatementlist( assign, NULL);
}


/*********************   Traverse   *********************/

/* Remove the expressions of global variable definitions.
 * Store the expressions the info struct for later uses when
 * constructing the __init funtion
 *
 * Returns a pointer to the current node
 *
 **/
node *INITglobaldef (node * arg_node, info * arg_info)
{
    node *new_tail;

    DBUG_ENTER ("INITglobaldef");

    /* Do nothing for global variable definitions without an expression */
    if (GLOBALDEF_EXPR( arg_node) == NULL)
        DBUG_RETURN (arg_node);

    /* Create a new assign node and store it in a statmentlist */
    new_tail = create_new_assign_statement_list(GLOBALDEF_ID( arg_node),
                    GLOBALDEF_EXPR( arg_node));

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

    /* Append funbody statements to the initialization assignment list */
    STATEMENTLIST_NEXT( arg_info->fun_tail) = FUNBODY_STATEMENTS( arg_node);

    /* Set statements of the function to new assignments + old statements */
    FUNBODY_STATEMENTS( arg_node) = STATEMENTLIST_NEXT( arg_info->fun_head);

    /* Reset the info fun_head and fun_tail inorder to let the next function
     * use it */
    empty_info_funlist(arg_info);

    DBUG_RETURN( arg_node);
}


node *INITvardec(node * arg_node, info * arg_info)
{
    node *new_tail;

    DBUG_ENTER ("INITvardeclist");

    if (VARDEC_VALUE( arg_node) == NULL)
        DBUG_RETURN (arg_node);

    /* Create a new assign node and store it in a statmentlist */
    new_tail = create_new_assign_statement_list( VARDEC_ID( arg_node),
                    VARDEC_VALUE( arg_node));

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
void add_init( node *syntaxtree, info *info)
{
    node *curr;
    node *__init = create_init_fundef( info);

    curr = syntaxtree;

    while(PROGRAM_NEXT(curr) != NULL) {
        curr = PROGRAM_NEXT(curr);
    }

    PROGRAM_NEXT(curr) = TBmakeProgram(__init, NULL);

    //return TBmakeProgram( __init, syntaxtree);
}

node * DSPdoInit(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSPdoInit");

    DBUG_ASSERT( ( syntaxtree != NULL), "DSPdoInit called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_init);

    syntaxtree = TRAVdo( syntaxtree, info);

    add_init( syntaxtree, info);

    TRAVpop();

    info = FreeInfo( info);

    DBUG_RETURN( syntaxtree);
}
