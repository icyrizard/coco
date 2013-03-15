#include "types.h"
#include "desug_init.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"
#include "string.h"

/* TODO create wrapper functions!!! */

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

struct var_list {
    char *var_name;
    int num;

    struct var_list *next;
};

struct INFO {
    int nest_level;

    struct var_list *vars;
    node *decs_head,
         *decs_tail;
};

/* MakeInfo allocates a new info 'object'
 *
 * The two linked list are initialized with a empty head node
 */
static info *MakeInfo()
{
    info *result;

    /* alloc info object and initialize it */
    result = MEMmalloc(sizeof(info));
    result->nest_level = 0;
    result->vars = NULL;

    /* initialize var dec list with a dummy head */
    result->decs_head = TBmakeVardeclist(NULL, NULL);
    result->decs_tail = result->decs_head;

    return result;
}

static info *FreeInfo( info *info)
{
    info = MEMfree( info);

    /* TODO free the dummy head node */

    return info;
}

/******************   Help Functions   ******************/
char* apply_rules(char* id, info *arg_info)
{
    char *result;
    struct var_list *curr = arg_info->vars;

    while(curr != NULL) {
        if(!strcmp(curr->var_name, id)) {
            result = STRcat(id, STRcat( "$", STRitoa( curr->num)));
            MEMfree(id);
            return result;
        }

        curr = curr->next;
    }

    /* no match found */
    return id;
}

/* pushes a new identifier name rewrite rule to the rule list. */
void push(node *forloop, info *arg_info)
{
    struct var_list *new = MEMmalloc( sizeof(struct var_list));

    new->var_name = STRcpy( VAR_NAME( ASSIGN_LET(FORLOOP_STARTVALUE( forloop))));
    new->num = arg_info->nest_level;
    new->next = arg_info->vars;
    arg_info->vars = new;
}

/* pop the top identifier name rewrite rule of the rule list. */
void pop(info *arg_info)
{
    arg_info->vars = arg_info->vars->next;

}

void reset_info( info *arg_info)
{
    /* empty vardec list */
    VARDECLIST_NEXT( arg_info->decs_tail) = NULL;
    arg_info->decs_tail = arg_info->decs_head;

    /* remove all rules */
    /* TODO MEMfree all allocated strings in arg_info->vars */
    arg_info->vars = NULL;
}


/*********************   Traverse   *********************/

node *FORfunbody(node *arg_node, info *arg_info)
{
    DBUG_ENTER( "FORfunbody");

    FUNBODY_STATEMENTS( arg_node) = TRAVopt( FUNBODY_STATEMENTS( arg_node), arg_info);

    /* add the vardecs to head of vardecs */
    VARDECLIST_NEXT( arg_info->decs_tail) = FUNBODY_VARS( arg_node);
    FUNBODY_VARS( arg_node) = VARDECLIST_NEXT( arg_info->decs_head);

    /* reset info struct by emptying the vardec list
     * and removing all the rules */
    reset_info(arg_info);

    DBUG_RETURN( arg_node);
}

node *FORforloop(node *arg_node, info *arg_info)
{
    node *new_var_dec;

    DBUG_ENTER("FORfunbody");

    /* new loop adds one to nest count */
    arg_info->nest_level++;

    /* traverse loop intialization, stopvalue and stepvalue expressions
     * to apply the id renaming rules of outer loops
     */
    ASSIGN_EXPR ( FORLOOP_STARTVALUE( arg_node)) = TRAVdo( ASSIGN_EXPR( FORLOOP_STARTVALUE( arg_node)), arg_info);
    FORLOOP_STOPVALUE( arg_node) = TRAVdo( FORLOOP_STOPVALUE( arg_node), arg_info);
    FORLOOP_STEPVALUE( arg_node) = TRAVopt( FORLOOP_STEPVALUE( arg_node), arg_info);

    /* add new rewrite rule of current for loop */
    push(arg_node, arg_info);

    /* traverse the statements inside the loop and apply all id renamin rules
     * */
    FORLOOP_BLOCK ( arg_node) = TRAVopt( FORLOOP_BLOCK( arg_node), arg_info);

    /* apply rule to loop variable initialization */
    /* TODO wrapper funtion with FREE calls */
    VAR_NAME ( ASSIGN_LET ( FORLOOP_STARTVALUE( arg_node))) = STRcat(VAR_NAME ( ASSIGN_LET ( FORLOOP_STARTVALUE( arg_node))), STRcat( "$", STRitoa( arg_info->nest_level)));

    /* create a new var dec */
    new_var_dec = TBmakeVardeclist( TBmakeVardec(TYPE_int, TBmakeVar( STRcpy(VAR_NAME( ASSIGN_LET (FORLOOP_STARTVALUE( arg_node))))), NULL), NULL);
    VARDECLIST_NEXT( arg_info->decs_tail) = new_var_dec;
    arg_info->decs_tail = new_var_dec;

    /* leaving the loop means one less nested count */
    arg_info->nest_level--;

    /* remove the current forloop rewrite rule */
    pop(arg_info);

    DBUG_RETURN( arg_node);
}

node *FORvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("FORvar");

    if(arg_info->nest_level == 0)
        DBUG_RETURN( arg_node);

    VAR_NAME( arg_node) = apply_rules( VAR_NAME( arg_node), arg_info);


    DBUG_RETURN( arg_node);
}


node * DSPdoFor(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSPdoFor");

    DBUG_ASSERT( ( syntaxtree != NULL), "DSPdoInit called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_for);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo( info);

    DBUG_RETURN( syntaxtree);
}
