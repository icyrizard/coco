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
    if (arg_info->t != TYPE_unknown){
        if(t != arg_info->t){
            DBUG_ASSERT(0, "type mismatch %s and %s",
                    type_to_string(t, type_str), type_to_string(arg_info->t, type_str);
        }

        arg_info->t = t;
    }

    DBUG_RETURN(arg_node);
}
node *TCHECKassign(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCHECKassign");
    type t_expected = VARDEC_TYPE(VAR_DECL(ASSIGN_LET(arg_node)));
    arg_info->t = t_expected;

    /*default type*/
    type tdef = TYPE_unknown;

    /*traverse tree*/
    TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

    /*set back to default*/
    arg_info->t = tdef;

    DBUG_RETURN(arg_node);
}

node *TCHECKvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCHECKvar");
    type t = VARDEC_TYPE(VAR_DECL(arg_node));

    if (arg_info->t != TYPE_unknown){
        if (arg_info->t != t){
            DBUG_ASSERT(0, "type mismatch");
        }

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

    TRAVpush(TR_tcheck);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo( info);

    DBUG_RETURN( syntaxtree);
}
