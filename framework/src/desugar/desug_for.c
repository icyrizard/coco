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

    result->decs_head = TBmakeVardeclist(NULL, NULL);
    result->decs_tail = result->decs_head;

    return result;
}

static info *FreeInfo( info *info)
{
    info = MEMfree( info);

    /* TODO free the two heads nodes */

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


void push(node *forloop, info *arg_info)
{
    struct var_list *new = MEMmalloc( sizeof(struct var_list));

    new->var_name = STRcpy( VARLET_NAME( ASSIGN_LET(FORLOOP_STARTVALUE( forloop))));
    new->num = arg_info->nest_level;
    new->next = arg_info->vars;
    arg_info->vars= new;
}


/*********************   Traverse   *********************/

node *FORfunbody(node *arg_node, info *arg_info)
{
    DBUG_ENTER( "FORfunbody");

    FUNBODY_STATEMENTS( arg_node) = TRAVopt( FUNBODY_STATEMENTS( arg_node), arg_info);

    DBUG_RETURN( arg_node);
}

node *FORforloop(node *arg_node, info *arg_info)
{
    node *new_var_dec;

    DBUG_ENTER("FORfunbody");


    /* add new variable dec to be created  */

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
    VARLET_NAME ( ASSIGN_LET ( FORLOOP_STARTVALUE( arg_node))) = STRcat(VARLET_NAME ( ASSIGN_LET ( FORLOOP_STARTVALUE( arg_node))), STRcat( "$", STRitoa( arg_info->nest_level)));

    /* create a new var dec */
    //new_var_dec = TBmakeVardec(TYPE_int, VARLET_NAME( ASSIGN_LET (FORLOOP_STARTVALUE( arg_node))));

    /* leaving the loop means one less nested count */
    arg_info->nest_level--;

    //pop(arg_info);

    DBUG_RETURN( arg_node);
}

node *FORvarlet(node *arg_node, info *arg_info)
{
    DBUG_ENTER("FORvarlet");

    //printf("Varlet: %s\n", VARLET_NAME( arg_node));

    if(arg_info->nest_level == 0)
        DBUG_RETURN( arg_node);

    VARLET_NAME( arg_node) = apply_rules( VARLET_NAME( arg_node), arg_info);


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

    printf("\n\n-------------------------------------\n\n");

    DBUG_RETURN( syntaxtree);
}
