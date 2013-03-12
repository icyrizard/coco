#include "types.h"
#include "desug_for.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"


/* using a list of strings */
struct var_list {
  char *var_name;
  struct var_list *tail;
};

/* INFO obj */
struct INFO {
    int nest_level;
    struct *var_list; // varlist of strings
};

/* MakeInfo allocates a new info 'object'
 *
 * The two linked list are initialized with a empty head node
 */
static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->nest_level = 0;
    result->var_list = NULL;

    return result;
}

static info *FreeInfo( info *info)
{
    info = MEMfree( info);

    return info;
}

node *FORforloop(node *arg_node, info *arg_info){
    DBUG_ENTER("FORforloop");
    printf("forloop\n");
    type t_int = TYPE_INT;

    arg_info->nest_level++;

    FORLOOP_STARTVALUE( arg_node) = TRAVdo(FORLOOP_STARTVALUE(arg_node), arg_info);

    arg_info->nest_level--;

    DBUG_RETURN(arg_node);
}

node *FORassign(node *arg_node, info *arg_info){
    DBUG_ENTER("FORassign");
    node *vardec;

    ASSIGN_LET( arg_node) = TRAVdo(ASSIGN_LET(arg_node), arg_info);
    //vardec = TBmakeVarDec("int", ASSIGN_LET(arg_node));
    DBUG_RETURN(arg_node);
}

node *FORvarlet(node *arg_node, info *arg_info){
    DBUG_ENTER("FORvarlet");
    char *tmp = "";
    int i;

    for(i = 0; i < arg_info->nest_level; i++){
        tmp = STRcat("_", STRcpy(tmp));
    }

    tmp = (char *)STRcat(tmp, STRcpy(VARLET_NAME(arg_node)));
    arg_info->vars->tail = tmp;

    VARLET_NAME(arg_node ) = tmp;
    DBUG_RETURN(arg_node);
}

node *FORfunbody(node * arg_node, info * arg_info)
{
    DBUG_ENTER("FORfunbody");
    FUNBODY_STATEMENTS( arg_node) = TRAVopt( FUNBODY_STATEMENTS( arg_node), arg_info);

    // info object
    DBUG_RETURN(arg_node);
}

node *DSPdoFor(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSPdoFor");

    DBUG_ASSERT( ( syntaxtree != NULL), "DSPdoFor called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_for);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo( info);

    printf("\n\n-------------------------------------\n\n");

    DBUG_RETURN( syntaxtree);

}
