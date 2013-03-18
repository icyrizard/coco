#include "types.h"
#include "contextphase_type.h"
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
    type t;
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

node *TCHECKassign(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCHECKassign");
    type t_expected = VARDEC_TYPE(VAR_DECL(ASSIGN_LET(arg_node)));
    arg_info->t = t_expected;
    type tdef = TYPE_unknown;

    TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

    arg_info->t = tdef;

    DBUG_RETURN(arg_node);
}

node *TCHECKvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCHECKvardec");
    DBUG_RETURN(arg_node);
}

node *TCHECKvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCHECKvar");
    type t = VARDEC_TYPE(VAR_DECL(arg_node));

    if (arg_info->t != NULL){
        if (arg_info->t != t)
            DBUG_ASSERT(0, "type mismatch");

        arg_info->t = t;
    }

    DBUG_RETURN(arg_node);
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
