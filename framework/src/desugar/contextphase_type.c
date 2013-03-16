#include "types.h"
#include "context_type.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"
#include "str.h"

/***********************   INFO   ***********************/
/* INFO object containing two linked list.
 */
struct INFO {
    node *head;
};

/* MakeInfo allocates a new info 'object'
 *
 * The two linked list are initialized with a empty head node
 */
static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));

    return result;
}

static info *FreeInfo( info *info)
{
    info = MEMfree( info);

    /* TODO free the two heads nodes */
    return info;
}

node *TYPEassign(node *arg_node, node *arg_info){
    type t_expected = VARDEC_TYPE(VAR_DECL(ASSIGN_LET(arg_node)));


}

node *TYPEvar(node *arg_node, info *arg_info){
     VAR_DECL



}


node * CTPdoTypeCheck(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSPdoTypeCheck");

    DBUG_ASSERT( ( syntaxtree != NULL), "DSPdoTypeCheck called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_typecheck);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo( info);

    DBUG_RETURN( syntaxtree);
}
