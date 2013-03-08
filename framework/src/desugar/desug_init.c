#include "types.h"
#include "desug_init.h"
#include "memory.h"
#include "dbug.h"
#include "tree_basic.h"
#include "traverse.h"
#include "globals.h"

#define INFO_FIRSTERROR(n) ((n)->firsterror)

struct ass_list {
    node *this;
    node *next;
};

struct INFO {
    bool firsterror;
    struct ass_list *root;
};

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));
    result->root = MEMmalloc(sizeof(struct ass_list));

    INFO_FIRSTERROR(result) = FALSE;

    return result;
}

static info *FreeInfo( info *info)
{
    info = MEMfree( info);

    return info;
}

node *
INITprogram(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("INITprogram");

    PROGRAM_HEAD( arg_node) = TRAVdo( PROGRAM_HEAD( arg_node), arg_info);

    PROGRAM_TAIL( arg_node) = TRAVopt( PROGRAM_TAIL( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}

node *INITglobaldef (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("INITglobaldef");

    //if (GLOBALDEF_EXPORT( arg_node))

    //switch (GLOBALDEF_TYPE( arg_node)) {
    //  case TYPE_unknown:
    //    DBUG_ASSERT( 0, "unknown type detected!");
   // }


    GLOBALDEF_ID( arg_node) = TRAVdo( GLOBALDEF_ID( arg_node), arg_info);

//    if(GLOBALDEF_EXPR( arg_node) != NULL) {
//        printf(" = ");
//        GLOBALDEF_EXPR( arg_node) = TRAVdo( GLOBALDEF_EXPR( arg_node), arg_info);
//    }

    DBUG_RETURN (arg_node);
}

node * DSPdoInit(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSPdoInit");

    DBUG_ASSERT( (syntaxtree != NULL), "DSPdoInit called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_init);

    syntaxtree = TRAVdo(syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    printf("\n\n-------------------------------------\n\n");

    DBUG_RETURN( syntaxtree);
}
