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
    type *t;
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
    DBUG_ENTER ("TYPEassign");
    type t_expected = VARDEC_TYPE(VAR_DECL(ASSIGN_LET(arg_node)));
    TRAVdo(ASSIGN_EXPR(arg_node));
    DBUG_RETURN(arg_node);
}

void type_to_string(type t, char *type_str){
     switch (VARDEC_TYPE( arg_node)) {
        case TYPE_bool:
            type_str = "bool";
            break;
        case TYPE_int:
            type_str = "int";
            break;
        case TYPE_float:
            type_str = "float";
            break;
        case TYPE_void:
            type_str = "void";
            break;
    }
}

node *TYPEvar(node *arg_node, info *arg_info){
    DBUG_ENTER("TYPEvar");
    char *type_str;
    /* get type of vardec */
    type t = VARDEC_TYPE(VAR_DECL(arg_node));

    /* compare to expected type */
    if(t != arg_info->t){
        DBUG_ASSERT(0, "type mismatch %s and %s",
                type_to_string(t, type_str), type_to_string(arg_info->t, type_str);
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
