#include "types.h"
#include "desug_init.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"
#include "list_hash.h"

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
    list *global_assigns,
         *local_assigns;
};

/* MakeInfo allocates a new info 'object'
 *
 * The two linked list are initialized with a empty head node
 */
static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));

    result->global_assigns = list_create();
    result->local_assigns = list_create();

    return result;
}

static info *FreeInfo( info *info)
{
    list_free(info->global_assigns);
    list_free(info->local_assigns);

    info = MEMfree( info);

    return info;
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
node* create_new_assign(node *id, node *expr)
{
    node *new_var, *assign;

    /* Create a copy of the varfor the new assignment node */
    new_var= TBmakeVar( STRcpy( VAR_NAME( id)));

    /* Create a new assign node and store it in a statmentlist */
    assign = TBmakeAssign( new_var, expr);

    return assign;
}

node *create_statementlist(list *assigns)
{
    node *statementlist = NULL;

    while((assigns = assigns->next))
        statementlist = TBmakeStatementlist(assigns->value, statementlist);
    return statementlist;
}


node *concat_statements(node *statements, list *assigns) {
    node *assign_list = create_statementlist(assigns);
    node *tail = assign_list;

    while(STATEMENTLIST_NEXT(tail))
        tail = STATEMENTLIST_NEXT(tail);

    STATEMENTLIST_NEXT(tail) = statements;

    return assign_list;
}

/*********************   Traverse   *********************/

/* Remove the expressions of global variable definitions.
 * Store the expressions the info struct for later uses when
 * constructing the __init funtion
 *
 **/
node *INITglobaldef (node * arg_node, info * arg_info)
{
    node *assign;

    DBUG_ENTER ("INITglobaldef");

    /* Do nothing for global variable definitions without an expression */
    if (GLOBALDEF_EXPR( arg_node) == NULL)
        DBUG_RETURN (arg_node);

    /* Create a new assign node */
    assign = create_new_assign(GLOBALDEF_ID( arg_node), GLOBALDEF_EXPR( arg_node));

    /* Add the new assign to the info node statmentlist */
    list_addtofront(arg_info->global_assigns, assign);

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
        FUNBODY_VARS(arg_node) = TRAVopt( FUNBODY_VARS( arg_node), arg_info);

    if(!list_length(arg_info->local_assigns) == 0) {
        FUNBODY_STATEMENTS( arg_node) = concat_statements(
                FUNBODY_STATEMENTS(arg_node), arg_info->local_assigns);

        /* empty local list for new function */
        list_empty(arg_info->local_assigns);
    }

    DBUG_RETURN( arg_node);
}


node *INITvardec(node * arg_node, info * arg_info)
{
    node *assign;

    DBUG_ENTER ("INITvardeclist");

    if (VARDEC_VALUE( arg_node) == NULL)
        DBUG_RETURN (arg_node);

    /* Create a new assign node and store it in a statmentlist */
    assign = create_new_assign( VARDEC_ID( arg_node),
                    VARDEC_VALUE( arg_node));

    /* Add the new assign to the info node statmentlist */
    list_addtofront(arg_info->local_assigns, assign);

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

    header = TBmakeFunheader( TYPE_void , TBmakeVar(STRcpy("__init")), NULL);
    body   = TBmakeFunbody( NULL, create_statementlist(info->global_assigns), NULL);

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
}

node * DSPdoInit(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSPdoInit");

    DBUG_ASSERT( ( syntaxtree != NULL), "DSPdoInit called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_init);

    syntaxtree = TRAVdo( syntaxtree, info);

    add_init(syntaxtree, info);

    TRAVpop();

    info = FreeInfo( info);

    DBUG_RETURN( syntaxtree);
}
